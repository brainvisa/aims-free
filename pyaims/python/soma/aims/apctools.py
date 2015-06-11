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

'''.APC (commissure coordinates) IO and other tools'''
__docformat__ = 'restructuredtext en'

from soma import aims
import types


def apcRead(filename, imagefile=None):
    '''Read a .APC file
    - filename: *string*
    - imagefile: *string*

      optional filename for the image file from which the AC/PC coordinates are
      taken from. Its header may be used to recover millimeters positions from
      voxels if they are not specified in the .APC file itself (for older
      versions of the .APC files)
    - returns: *dict*

      the contents of the file as a dictionary, keys being 'ac', 'pc', 'ih'
      for voxel coordinates, and 'acmm', 'pcmm', 'ihmm' for millimeters
      coordinates, and optionally 'comment'.
    '''
    f = open(filename)
    lines = f.readlines()
    f.close()
    apcdict = {}
    ac = []
    pc = []
    ih = []
    acm = []
    pcm = []
    ihm = []
    for l in lines:
        if l[:3] == 'AC:':
            apcdict['ac'] = map(lambda x: int(x), l.split()[1:4])
        elif l[:3] == 'PC:':
            apcdict['pc'] = map(lambda x: int(x), l.split()[1:4])
        elif l[:3] == 'IH:':
            apcdict['ih'] = map(lambda x: int(x), l.split()[1:4])
        elif l[:5] == 'ACmm:':
            acm = map(lambda x: float(x), l.split()[1:4])
            apcdict['acmm'] = acm
        elif l[:5] == 'PCmm:':
            pcm = map(lambda x: float(x), l.split()[1:4])
            apcdict['pcmm'] = pcm
        elif l[:5] == 'IHmm:':
            ihm = map(lambda x: float(x), l.split()[1:4])
            apcdict['ihmm'] = ihm
        else:
            comment = apcdict.get('comment', None)
            if comment is None:
                comment = []
                apcdict['comment'] = comment
            comment.append(l.strip())

    if len(acm) == 3 and len(pcm) == 3 and len(ihm) == 3:
        return apcdict
    if imagefile is None:
        return apcdict

    if type(imagefile) in types.StringTypes:
        f = aims.Finder()
        if f.check(imagefile):
            image = f.header()
        else:
            raise IOError('Cannot read header of: ' + filename)
    else:
        try:
            image = imagefile.header()
        except:  # otherwise it is considered to already be a header
            image = imagefile
            if not hasattr( image, 'has_key' ) \
                    or not image.has_key('voxel_size'):
                image = {'voxel_size': imagefile}  # directly the VS list ?
    vs = image['voxel_size']
    if len(acm) != 3:
        acm = [ac[0] * vs[0], ac[1] * vs[1], ac[2] * vs[2]]
        apcdict['acmm'] = acm
    if len(pcm) != 3:
        pcm = [pc[0] * vs[0], pc[1] * vs[1], pc[2] * vs[2]]
        apcdict['pcmm'] = pcm
    if len(ihm) != 3:
        ihm = [ih[0] * vs[0], ih[1] * vs[1], ih[2] * vs[2]]
        apcdict['ihmm'] = ihm
    try:
        apcWrite(filename, apcdict)
    except:
        # could not write modifications
        pass

    return apcdict


def apcWrite(apcdict, filename):
    '''Writes a .APC file from a dictionary
    '''
    ac = apcdict['ac']
    pc = apcdict['pc']
    ih = apcdict['ih']
    f = open(filename, 'w')
    f.write('AC: ' + ' '.join([str(x) for x in ac]) + '\n')
    f.write('PC: ' + ' '.join([str(x) for x in pc]) + '\n')
    f.write('IH: ' + ' '.join([str(x) for x in ih]) + '\n')
    comment = apcdict.get('comment', None)
    acm = apcdict.get('acmm', None)
    pcm = apcdict.get('pcmm', None)
    ihm = apcdict.get('ihmm', None)
    if comment is None:
        comment = []
        if acm and pcm and ihm:
            comment = ['# The previous coordinates, used by the system, '
                       'are defined in voxels',
                       '# They stem from the following coordinates in millimeters:']
    for l in comment:
        f.write(l + '\n')
    if acm and pcm and ihm:
        f.write('ACmm: ' + ' '.join([str(x) for x in acm]) + '\n')
        f.write('PCmm: ' + ' '.join([str(x) for x in pcm]) + '\n')
        f.write('IHmm: ' + ' '.join([str(x) for x in ihm]) + '\n')

    f.close()


def apcTransform(apcdict, transform, outimagevoxelsize):
    '''Transforms coordinates of commissures points through a specified
    transformation

    - apcdict: *dict*

      Commissures coordinates, as a dictionary with 'ac', 'pc', 'ih' keys for
      voxel coordinates, 'acmm', 'pcmm', 'ihmm' for millimeters coordinates

    - transform:
      :py:class:`AffineTransformation3d <soma.aims.AffineTransformation3d>`
      object

    - outimagevoxelsize:

      - as *string*: filename for the image whose voxel size should be used
      - as :py:class:`Volume <soma.aims.Volume_FLOAT>`,
        :py:class:`AimsData <soma.aims.AimsData_FLOAT>` or any other object
        with a ``header()`` method: voxel_size is taken from its header
      - as *dict* or *header object*: voxel size is takes as the
        ``voxel_size`` entry of the dictionary

    Coordinates are transformed in the ``apcdict`` dictionary, which is
    modified in-place.
    '''
    if type(outimagevoxelsize) in types.StringTypes:
        f = aims.Finder()
        if f.check(outimagevoxelsize):
            outhdr = f.header()
        else:
            raise IOError('Cannot read header of: ' + outimagevoxelsize)
    else:
        try:
            outhdr = outimagevoxelsize.header()
        except:  # otherwise it is considered to already be a header
            outhdr = outimagevoxelsize
            if not hasattr( outhdr, 'has_key' ) \
                    or not outhdr.has_key('voxel_size'):
                outhdr = {'voxel_size': outhdr}  # directly the VS list ?
    outvs = outhdr['voxel_size']
    for p in ('ac', 'pc', 'ih'):
        px = apcdict.get(p, None)
        pmm = apcdict[p + 'mm']
        pmm2 = transform.transform(pmm)
        apcdict[p + 'mm'] = pmm2
        vs = [pmm[i] / px[i] for i in xrange(3)]
        px2 = [int(round(x / y)) for x, y in zip(pmm2, outvs)]
        apcdict[p] = px2


def apcFileTransform(inAPCfilename, outAPCfilename, transform,
                     outimagevoxelsize, imagefile=None):
    '''Transforms the coordinates of a .APC file points through a given
    transformation. It basically reads ``inAPCfilename``, transforms its
    contents using :py:func:`apcTransform`, then writes the result to
    ``outAPCfilename``.
    '''
    apc = apcRead(inAPCfilename, imagefile)
    apcTransform(apc, transform, outimagevoxelsize)
    apcWrite(apc, outAPCfilename)
