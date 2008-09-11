
from soma import aims
import sys

reader = aims.Reader()
mesh = reader.read( '/home/riviere/data/ra_head.mesh' )
print 'mesh:', mesh
h = mesh.header()
print 'header:', h

print 'timesteps:', mesh.size()
for t in xrange( mesh.size() ):
    print 'time:', t
    v = mesh.vertex(t)
    print 'vertices:', v
    n = mesh.normal(t)
    print 'normals:', n
    p = mesh.polygon(t)
    print 'polygons:', p

fileout = '/tmp/toto.mesh'
print 'writing mesh to', fileout
w = aims.Writer()
w.write( mesh, fileout )
print 'object type:', w.writtenObjectType()
print 'data type:', w.writtenObjectDataType()
print 'full type:', w.writtenObjectFullType()

