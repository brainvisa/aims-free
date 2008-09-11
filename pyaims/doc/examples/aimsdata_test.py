
from soma import aims
import sys

app = aims.AimsApplication( sys.argv, 'AimsData test in python' )

app.initialize()

reader = aims.Reader()
# read volumes as AimsData
reader.mapType( 'Volume', 'AimsData' )
data = reader.read( '/home/riviere/data/irm.ima' )
print 'data:', data
h = data.header()
print 'header:', h

dx = data.dimX()
dy = data.dimY()
dz = data.dimZ()
dt = data.dimT()

mi = data.value( 0 )
ma = data.value( 0 )

dims = [ dx, dy, dz, dt ]
print 'volume_dimensions:', dims
print 'voxel_size:', [ data.sizeX(), data.sizeY(), data.sizeZ(), data.sizeT() ]

print 'C++ min/max:', data.minimum(), '-', data.maximum()
print 'setting value 1618 at pos (125, 63, 12 )'
data.setValue( 1618, 125, 63, 12 )
print 'C++ min/max:', data.minimum(), '-', data.maximum()

print 'minIndex:', data.minIndex()
print 'maxIndex:', data.maxIndex()

print 'python min/max...'
for t in xrange( dt ):
    for z in xrange( dz ):
        print 't:', t, 'z:', z
        for y in xrange( dy ):
            for x in xrange( dx ):
                v = data.value( x, y, z, t )
                if mi > v:
                    mi = v
                if ma < v:
                    ma = v

print 'min:', mi
print 'max:', ma

fileout = '/tmp/toto.vimg'
print 'writing volume to', fileout
w = aims.Writer()
w.write( data, fileout )
print 'object type:', w.writtenObjectType()
print 'data type:', w.writtenObjectDataType()
print 'full type:', w.writtenObjectFullType()

