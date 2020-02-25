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

'''Volume functions'''

from __future__ import print_function

from __future__ import absolute_import
from six.moves import range
from six.moves import zip
__docformat__ = 'restructuredtext en'

from soma import aims
import numpy as np
import sys


if sys.version_info[0] >= 3:
    xrange = range


def crop_volume(vol, threshold=0, border=0):
    '''
    Crop the input volume, removing slices filled with values under a
    given threshold, and keeping a given border.

    If no crop actually takes place, the input volume is returned without
    duplication. If crop is actually performed, then a view into the original
    volume is returned, sharing the same data block which is not copied.

    Transformations in the header are adapted accordingly.

    Parameters
    ----------
    vol: aims Volume
        volume to be cropped
    threshold: volume value, optional
        Minimum value over which a slice cannot be cropped (is supposed to
        contain real data). The default is 0: only value <= 0 is croppable
    border: int, optional
        border around the cropped volume: the cropped volume is enlarged by
        twice this value in each direction, within the limits of the original
        volume (the bounding box always fits in the original volume).
        Values in the border are taken from the original volume, the border is
        not artificially filled with a constant value. The default is 0: no
        border
    '''
    arr = np.asarray(vol)
    # look for empty slices
    zeroslice = -1
    for z in range(vol.getSizeZ()):
        slicevol = arr[:,:, z,:]
        if np.all(slicevol <= threshold):
            zeroslice = z
        else:
            break
    else:
        z = -1
    zmin = np.max((zeroslice + 1 - border, 0))
    zeroslice = vol.getSizeZ()
    if z != -1:
        for z in range(vol.getSizeZ()-1, 0, -1):
            slicevol = arr[:,:, z,:]
            if np.all(slicevol <= threshold):
                zeroslice = z
            else:
                break
    zup = np.min((zeroslice + border, vol.getSizeZ()))

    zeroslice = -1
    for y in range(vol.getSizeY()):
        slicevol = arr[:, y,:,:]
        if np.all(slicevol <= threshold):
            zeroslice = y
        else:
            break
    else:
        y = -1
    ymin = np.max((zeroslice + 1 - border, 0))
    zeroslice = vol.getSizeY()
    if y != -1:
        for y in range(vol.getSizeY()-1, 0, -1):
            slicevol = arr[:, y,:,:]
            if np.all(slicevol <= threshold):
                zeroslice = y
            else:
                break
    yup = np.min((zeroslice + border, vol.getSizeY()))

    zeroslice = -1
    for x in range(vol.getSizeX()):
        slicevol = arr[x,:,:,:]
        if np.all(slicevol <= threshold):
            zeroslice = x
        else:
            break
    else:
        x = -1
    xmin = np.max((zeroslice + 1 - border, 0))
    zeroslice = vol.getSizeX()
    if x != -1:
        for x in range(vol.getSizeX()-1, 0, -1):
            slicevol = arr[x,:,:,:]
            if np.all(slicevol <= threshold):
                zeroslice = x
            else:
                break
    xup = np.min((zeroslice + border, vol.getSizeX()))

    if xmin == 0 and xup == vol.getSizeX() \
            and ymin == 0 and yup == vol.getSizeY() \
            and zmin == 0 and zup == vol.getSizeZ():
        return vol
    cropped_vol = aims.VolumeView(
        vol, vol.Position4Di(xmin, ymin, zmin, 0),
        vol.Position4Di(xup - xmin, yup - ymin, zup - zmin, vol.getSizeT()))
    cropped_vol.copyHeaderFrom(vol.header())
    transl = aims.AffineTransformation3d()
    if 'referential' in cropped_vol.header():
        del cropped_vol.header()['referential']
    if 'uuid' in cropped_vol.header():
        del cropped_vol.header()['uuid']
    vs = vol.getVoxelSize()
    transl.setTranslation((xmin * vs[0], ymin * vs[1], zmin * vs[2]))
    if 'transformations' in vol.header() \
            and 'referentials' in vol.header():
        trans_list = vol.header()['transformations']
        ctrans_list = []
        for trans_v in trans_list:
            trans = aims.AffineTransformation3d(trans_v)
            trans *= transl
            ctrans_list.append(trans.toVector())
        cropped_vol.header()['transformations'] = ctrans_list

    return cropped_vol

def compare_images(vol, vol2, vol1_name='input', vol2_name='output',
                   thresh=1e-6, rel_thresh = False):
#    print('comp vol, sizes:', vol.getSize(), vol2.getSize())
#    print('    vsizes:', str(vol.getVoxelSize()), str(vol2.getVoxelSize()))
    msg = 'comparing %s and %s' % (vol1_name, vol2_name)
    if vol.getSize().list() != vol2.getSize().list():
        raise RuntimeError(msg + ': %s != %s'
                            % (str(vol.getSize()), str(vol2.getSize())))
    if np.max(np.abs(np.asarray(vol.getVoxelSize()) \
                      - vol2.getVoxelSize())) >= 1e-6 :
        raise RuntimeError(msg + ': voxels size differ: %s != %s'
                            % (str(vol.getVoxelSize()), str(vol2.getVoxelSize())))

    if len(np.asarray(vol).shape) == 0:
        # not bound to numpy, elements are supposed to be arrays
        # use suboptimal python loop
        dim = list(vol.getSize())
        pos = [0] * len(dim)
        end = False
        nd = len(dim)
        while not end:
            diff = max([np.abs(x - y) for x, y in zip(vol.at(pos), vol2.at(pos))])
            if diff >= thresh:
                print('values at', pos, ':', vol.at(pos), vol2.at(pos), voldiff.at(pos))
                raise RuntimeError(
                    msg + ', diff %f exceeds max allowed: %f at %s'
                    % (diff, thresh, repr(pos)))
            pos[0] += 1
            d = 0
            while pos[d] == dim[d]:
                pos[d] = 0
                d += 1
                if d == nd:
                    end = True
                    break
                pos[d] += 1
        return True

    if rel_thresh:
        val_range = float(np.max(np.asarray(vol))) \
            - np.min(np.asarray(vol))
        thresh = thresh * val_range

    nvol = np.asarray(vol)
    nvol2 = np.asarray(vol2)
    if nvol.dtype.fields is not None and 'v' in nvol.dtype.fields:
        # bound as struct containing an array named "v" (RGB, AimsVector...)
        # use it as array with additional dimensions
        nvol = nvol['v']
        nvol2 = nvol2['v']
    if np.max(np.abs(nvol - nvol2)) >= thresh:
        raise RuntimeError(msg + ', max diff: %f, max allowed: %f'
                            % (np.max(np.abs(vol - vol2)), thresh))
    
    return True


