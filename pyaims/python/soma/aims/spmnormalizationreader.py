#  This software and supporting documentation are distributed by
#      Institut Federatif de Recherche 49
#      CEA/NeuroSpin, Batiment 145,
#      91191 Gif-sur-Yvette cedex
#      France
#
# This software is governed by the CeCILL-B license under
# French law and abiding by the rules of distribution of free software.
# You can  use, modify and/or redistribute the software under the
# terms of the CeCILL-B license as circulated by CEA, CNRS
# and INRIA at the following URL "http://www.cecill.info".
#
# As a counterpart to the access to the source code and  rights to copy,
# modify and redistribute granted by the license, users are provided only
# with a limited warranty  and the software's author,  the holder of the
# economic rights,  and the successive licensors  have only  limited
# liability.
#
# In this respect, the user's attention is drawn to the risks associated
# with loading,  using,  modifying and/or developing or reproducing the
# software by the user in light of its specific status of free software,
# that may mean  that it is complicated to manipulate,  and  that  also
# therefore means  that it is reserved for developers  and  experienced
# professionals having in-depth computer knowledge. Users are therefore
# encouraged to load and test the software's suitability as regards their
# requirements in conditions enabling the security of their systems and/or
# data to be ensured and,  more generally, to use and operate it in the
# same conditions as regards security.
#
# The fact that you are presently reading this means that you have had
# knowledge of the CeCILL-B license and that you accept its terms.

import scipy
import scipy.io
import numpy
import sys
from soma import uuid

__docformat__ = 'restructuredtext en'


def readSpmNormalization(matfilename, source=None, destref=None, srcref=None):
    '''
    Read a SPM \*_sn.mat normalization file and converts it to an Aims
    AffineTransformation3d.
    The converted transformation has for source the AIMS referential of the
    source image, and for destination the template referential of the SPM .mat
    file. All coordinates are in millimeters.

    The source image information may be provided either as its filename, its
    header object, or the image itself. It should carry the needed
    information: source volume storage_to_memory transformation matrix,
    voxel_size, etc.
    If None is passed as source (the default), then the source image name will
    be built from the .mat filename and will be read if found.

    - matfilename: *string*

      file name of the \*_sn.mat normalization file to reading

    - source: *filename* (*string*), or *Volume*
      :py:class:`Volume <soma.aims.Volume_FLOAT>`,
      or *volume header* (*MappingType*)

      file name of the \*_sn.mat normalization file to reading

    - destref: *string* or *UUID* (:py:class:`Uuid <soma.uuid.Uuid>`)

      destination referential for the transformation. If not
      specified, none will be set. If provided as a symbolic name
      ('Talairach-MNI template-SPM'), it will be converted to an UUID string.

    - srcref: *string* or *UUID*

      source referential for the transformation. If not specified,
      an attempt will be made to take it from the source image, otherwise it will
      not be set. If provided as a symbolic name ('Talairach-MNI template-SPM'),
      it will be converted to an UUID string.

    - returns:
      :py:class:`AffineTransformation3d <soma.aims.AffineTransformation3d>`
      object

      the converted transformation
    '''
    aims = sys.modules['soma.aims']
    if source is None:
        if matfilename.endswith('_sn3d.mat'):
            # SPM 99
            source = matfilename[:-9]
        elif matfilename.endswith('_sn.mat'):
            # SPM >= 2
            source = matfilename[:-7]
        elif matfilename.endswith('.mat'):
            # unlikely to work...
            source = matfilename[:-4]
        else:
            # unlikely to work either...
            source = matfilename
    st = type(source)
    if st is type('') or st is type(u''):
        f = aims.Finder()
        if not f.check(source):
            raise IOError('could not read source image header for ' + source)
        hdr = f.header()
        if 'object_type' in hdr and hdr['object_type'] == 'genericobject':
            hdr = aims.read(source)
    elif hasattr(source, 'header'):
        hdr = source.header()
    else:
        hdr = source
    try:
        vsA = aims.AffineTransformation3d(numpy.diag(hdr['voxel_size'][:3]
                                                     + [1.]))
    except:
        raise RuntimeError('Source image header information could not be '
                           'accessed.')
    try:
        s2m = aims.AffineTransformation3d(hdr['storage_to_memory'])
    except:
        s2m = aims.AffineTransformation3d()
    sn3d = scipy.io.loadmat(matfilename)
    Affine = aims.AffineTransformation3d(sn3d['Affine'])
    if 'VG' in sn3d:
        # SPM >= 2
        VG = sn3d['VG']
        scipyversion = [int(x) for x in scipy.version.version.split('.')[:2]]
        if scipyversion >= [0, 7]:
            VG = VG[0, 0]
        if scipyversion >= [0, 9]:
            if VG[2].shape != (4, 4):
                # SPM12 has this matrix in VG[4] where it was VG[2] in SPM8...
                MT = aims.AffineTransformation3d(VG[4])
            else:
                MT = aims.AffineTransformation3d(VG[2])
        else:
            MT = aims.AffineTransformation3d(VG.mat)
    else:  # spm99
        MT = aims.AffineTransformation3d(sn3d['MG'])
    # At is the compensation for Affine being used between Matlab arrays
    # indices, starting at 1, not 0
    At = numpy.mat(numpy.diag([1., 1., 1., 1.]))
    At[:3, 3] = numpy.transpose(numpy.mat([-1., -1., -1.]))
    At = aims.AffineTransformation3d(At)
    # AtoT is Aims (mm) to Template (SPM mm) and is the output if
    # no normalized_volume is specified
    AtoT = MT * (vsA * s2m * At * Affine).inverse()
    ahdr = AtoT.header()
    if srcref is None and 'referential' in hdr:
        srcref = hdr['referential']
    if srcref:
        if srcref == aims.StandardReferentials.mniTemplateReferential():
            srcref = aims.StandardReferentials.mniTemplateReferentialID()
        elif srcref == aims.StandardReferentials.acPcReferential():
            srcref = aims.StandardReferentials.acPcReferentialID()
        try:
            uuid.Uuid(srcref)
            ahdr['source_referential'] = srcref
        except:
            print('source referential', srcref,
                  'cannot be converted to a valid UUID - not setting it.',
                  file=sys.stderr)
    if destref:
        if destref == aims.StandardReferentials.mniTemplateReferential():
            destref = aims.StandardReferentials.mniTemplateReferentialID()
        elif destref == aims.StandardReferentials.acPcReferential():
            destref = aims.StandardReferentials.acPcReferentialID()
        try:
            uuid.Uuid(destref)
            ahdr['destination_referential'] = destref
        except:
            print('destination referential', destref,
                  'cannot be converted to a valid UUID - not setting it.',
                  file=sys.stderr)

    return AtoT
