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
__docformat__ = 'restructuredtext en'

from soma import aims
import numpy as np

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
    for z in xrange(vol.getSizeZ()):
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
        for z in xrange(vol.getSizeZ()-1, 0, -1):
            slicevol = arr[:,:, z,:]
            if np.all(slicevol <= threshold):
                zeroslice = z
            else:
                break
    zup = np.min((zeroslice + border, vol.getSizeZ()))

    zeroslice = -1
    for y in xrange(vol.getSizeY()):
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
        for y in xrange(vol.getSizeY()-1, 0, -1):
            slicevol = arr[:, y,:,:]
            if np.all(slicevol <= threshold):
                zeroslice = y
            else:
                break
    yup = np.min((zeroslice + border, vol.getSizeY()))

    zeroslice = -1
    for x in xrange(vol.getSizeX()):
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
        for x in xrange(vol.getSizeX()-1, 0, -1):
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
    if cropped_vol.header().has_key('referential'):
        del cropped_vol.header()['referential']
    if cropped_vol.header().has_key('uuid'):
        del cropped_vol.header()['uuid']
    vs = vol.getVoxelSize()
    transl.setTranslation((xmin * vs[0], ymin * vs[1], zmin * vs[2]))
    if vol.header().has_key('transformations') \
            and vol.header().has_key('referentials'):
        trans_list = vol.header()['transformations']
        ctrans_list = []
        for trans_v in trans_list:
            trans = aims.AffineTransformation3d(trans_v)
            trans *= transl
            ctrans_list.append(trans.toVector())
        cropped_vol.header()['transformations'] = ctrans_list

    return cropped_vol


