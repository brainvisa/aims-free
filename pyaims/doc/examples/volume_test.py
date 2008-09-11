
from soma import aims
import sys

app = aims.AimsApplication( sys.argv, 'Volume test in python' )

app.initialize()

reader = aims.Reader()
vol = reader.read( '/home/riviere/data/irm.ima' )
print 'vol:', vol
h = vol.header()
print 'header:', h

dx = vol.getSizeX()
dy = vol.getSizeY()
dz = vol.getSizeZ()
dt = vol.getSizeT()

mi = vol.value( 0 )
ma = vol.value( 0 )

dims = [ dx, dy, dz, dt ]
print 'volume_dimensions:', dims
print 'voxel_size:', h[ 'voxel_size' ]

# array min/max

arr = vol.arraydata()
#print 'array min/max:', Numeric.minimum( arr ), '-', Numeric.maximum( arr )

print 'setting value 1618 at pos (125, 63, 12 )'
vol.setValue( 1618, 125, 63, 12 )
#print 'C++ min/max:', vol.minimum(), '-', vol.maximum()

#print 'minIndex:', vol.minIndex()
#print 'maxIndex:', vol.maxIndex()

print '(numpy) min:', arr.min(), ' at voxel ', arr.argmin()
print '(numpy) max:', arr.max(), ' at voxel ', arr.argmax()

print 'python min/max...'
for t in xrange( dt ):
    for z in xrange( dz ):
        print 't:', t, 'z:', z
        for y in xrange( dy ):
            for x in xrange( dx ):
                v = vol.value( x, y, z, t )
                if mi > v:
                    mi = v
                if ma < v:
                    ma = v

print 'min:', mi
print 'max:', ma

fileout = '/tmp/toto.img'
print 'writing volume to', fileout
w = aims.Writer()
w.write( vol, fileout )
print 'object type:', w.writtenObjectType()
print 'data type:', w.writtenObjectDataType()
print 'full type:', w.writtenObjectFullType()

