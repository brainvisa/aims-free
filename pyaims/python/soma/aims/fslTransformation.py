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

'''FSL matrixes seem to transform from/to internal refs, like Aims but with
a different convention:

- X: right -> left
- Y: back -> front
- Z: bottom -> top

which appears to be Y and Z flipped compared to Aims
'''
from __future__ import absolute_import
import six
__docformat__ = 'restructuredtext en'

from soma import aims
from soma.minf.api import readMinf
import numpy
import types
import os
import sys

if sys.version_info[0] >= 3:
    six.string_types = str


def fslMatToTrm(matfile, srcimage, dstimage):
    '''
    As far as I have understood:

    A FSL transformation goes from the disk referential of the source image
    to the disk referential of the destination image.

    BUT:

    if the qform of an image (disk -> "real world") implies a flip (goes from a
    direct referential to an indirect one or the contrary), then a flip along X
    axis is inserted in the matrix, since FSL flirt doesn't allow flipping.
    '''
    # The FSL transform (mat) goes from the potentially flipped disk-oriented,
    # referential of the 1st image, in millimeters (R_FDflipmm1) to the same
    # ref of the 2nd image (R_FDflipmm2) (see figure)
    #
    # We want: the Aims transform (trm) from the Aims referential of the 1st
    # image, in mm (R_AIMSmm1) to the same one for the 2nd image (R_AIMSmm2)
    # see:
    #   http://brainvisa.info/doc/aimsdata/aims_training/en/html/ch06.html
    #   http://brainvisa.info/doc/anatomist/anatomist_referentials.pdf)
    #
    # Figure:
    #                       ----------------
    #   Referentials are in | square boxes |
    #                       ----------------
    #   Transforms are in (round parentheses) and correspond to code variables
    #   Arrows show transform orientations
    #
    # For one image:
    #
    #   ---------------                     ---------------
    #   | R_FDflipmm1 |  ----> (mat) ---->  | R_FDflipmm2 |
    #   ---------------                     ---------------
    #       /\
    #       |  (flip1)    X axis flip, with disk orientation if needed, or id.
    #       |
    #   -----------
    #   | R_FDmm1 |       flipped ref used by FSL
    #   -----------
    #       /\
    #       |  (vsd1)     voxels size transform (homothetic), in disk space
    #       |
    #   ----------------
    #   | R_NIFTI_Dvox |  NIFTI ref, disk oriented, in voxels
    #   ----------------
    #       |
    #       |  (s2m1)     storage_to_memory: disk to Aims, in voxels
    #      \/
    #   -------------
    #   | R_AIMSvox |     Aims orientation, still in voxels
    #   -------------
    #       |
    #       |  (vsm1)    voxels size transform (homothetic), in Aims space
    #      \/
    #   -------------                              -------------
    #   | R_AIMSmm1 |    Aims orientation, in mm   | R_AIMSmm2 |
    #   -------------                              -------------
    #       |                                          /\
    #       |                  (trm)                   |
    #       --------------------------------------------
    #
    # Thus, the final transform chain, for the 1st image, is:
    #   flip1 * vsd1 * inv(s2m1) * inv(vsm1)
    # The same chain is also applied on the other side, and inverted:
    #   vsm2 * s2m2 * inv(vsd2) * inv(flip2)
    # with mot between both parts.

    if isinstance(srcimage, six.string_types):
        f = aims.Finder()
        f.check(srcimage)
        im1 = f.header()
    else:  # try srcimage as a volume
        try:
            im1 = srcimage.header()
        except:  # otherwise it is considered to already be a header
            im1 = srcimage
    if isinstance(dstimage, six.string_types):
        f = aims.Finder()
        f.check(dstimage)
        im2 = f.header()
    else:  # try dstimage as a volume
        try:
            im2 = dstimage.header()
        except:  # otherwise it is considered to already be a header
            im2 = dstimage

    vs1 = im1['voxel_size'][:3]
    vs2 = im2['voxel_size'][:3]

    if os.path.exists(matfile + '.minf'):
        matminf = readMinf(matfile + '.minf')[0]
    else:
        matminf = {}
    s2m1 = matminf.get('source_storage_to_memory', None)
    s2m2 = matminf.get('destination_storage_to_memory', None)
    if not s2m1:
        try:
            s2m1 = im1['storage_to_memory']
        except:
            s2m1 = [-1., 0, 0, im1['volume_dimension'][0] - 1,
                    0, -1., 0, im1['volume_dimension'][1] - 1,
                    0, 0, -1., im1['volume_dimension'][2] - 1,
                    0, 0, 0, 1.]
    if not s2m2:
        try:
            s2m2 = im2['storage_to_memory']
        except:
            s2m2 = [1., 0, 0, 0,
                    0, -1., 0, im2['volume_dimension'][1] - 1,
                    0, 0, -1., im2['volume_dimension'][2] - 1,
                    0, 0, 0, 1.]
    s2m1 = aims.Motion(s2m1)
    s2m2 = aims.Motion(s2m2)
    vsm1 = aims.Motion()
    vsm1.fromMatrix(numpy.diag(vs1 + [0]))
    vsm2 = aims.Motion()
    vsm2.fromMatrix(numpy.diag(vs2 + [0]))
    m2s1 = s2m1.inverse()
    m2s2 = s2m2.inverse()
    dimd1 = [abs(x) for x in m2s1.transform(im1['volume_dimension'][:3])
             - m2s1.transform([0, 0, 0])]
    dimd2 = [abs(x) for x in m2s2.transform(im2['volume_dimension'][:3])
             - m2s2.transform([0, 0, 0])]
    x = [abs(x) for x in m2s1.transform(vs1) - m2s1.transform([0, 0, 0])]
    vsd1 = aims.Motion()
    vsd1.fromMatrix(numpy.diag(x + [0]))
    x = [abs(x) for x in m2s2.transform(vs2) - m2s2.transform([0, 0, 0])]
    vsd2 = aims.Motion()
    vsd2.fromMatrix(numpy.diag(x + [0]))
    flip1 = aims.Motion()
    flip2 = aims.Motion()
    if not s2m1.isDirect():
        flip1.rotation().setValue(-1., 0, 0)
        flip1.translation()[0] = vsd1.rotation().value(0, 0) * (dimd1[0] - 1)
    if not s2m2.isDirect():
        flip2.rotation().setValue(-1., 0, 0)
        flip2.translation()[0] = vsd2.rotation().value(0, 0) * (dimd2[0] - 1)

    mat = [x.split() for x in open(matfile).readlines()]
    mot = aims.Motion()
    mot.fromMatrix(numpy.mat(mat))

    trm = vsm2 * s2m2 * vsd2.inverse() * flip2.inverse() * mot \
        * flip1 * vsd1 * m2s1 * vsm1.inverse()

    return trm
