# -*- coding: utf-8 -*-

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

from __future__ import print_function

from __future__ import absolute_import
from soma import aims
import sys
from six.moves import range


app = aims.AimsApplication(sys.argv, 'Volume test in python')

app.initialize()

infile = aims.carto.Paths.findResourceFile(
    'doc/pyanatomist-%s/examples/irm.ima'
    % '.'.join([str(x) for x in aims.version()]))
if not infile:
    infile = 'irm.ima'
vol = aims.read(infile)
print('vol:', vol)
h = vol.header()
print('header:', h)

dx = vol.getSizeX()
dy = vol.getSizeY()
dz = vol.getSizeZ()
dt = vol.getSizeT()

mi = vol.value(0)
ma = vol.value(0)

dims = [dx, dy, dz, dt]
print('volume_dimensions:', dims)
print('voxel_size:', h['voxel_size'])

# array min/max

arr = vol.np
# print('array min/max:', Numeric.minimum( arr ), '-', Numeric.maximum( arr ))

print('setting value 1618 at pos (125, 63, 12 )')
vol.setValue(1618, 125, 63, 12)
# print('C++ min/max:', vol.minimum(), '-', vol.maximum())

# print('minIndex:', vol.minIndex())
# print('maxIndex:', vol.maxIndex())

print('(numpy) min:', arr.min(), ' at voxel ', arr.argmin())
print('(numpy) max:', arr.max(), ' at voxel ', arr.argmax())

print('python min/max...')
for t in range(dt):
    for z in range(dz):
        print('t:', t, 'z:', z)
        for y in range(dy):
            for x in range(dx):
                v = vol.value(x, y, z, t)
                if mi > v:
                    mi = v
                if ma < v:
                    ma = v

print('min:', mi)
print('max:', ma)

fileout = '/tmp/toto.img'
print('writing volume to', fileout)
w = aims.Writer()
w.write(vol, fileout)
print('object type:', w.writtenObjectType())
print('data type:', w.writtenObjectDataType())
print('full type:', w.writtenObjectFullType())
