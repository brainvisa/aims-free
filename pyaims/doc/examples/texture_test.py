
from soma import aims
import sys

reader = aims.Reader()
tex = reader.read( '/home/riviere/data/texture2d.tex' )
print 'texture:', tex
h = tex.header()
print 'header:', h

print 'timesteps:', tex.size()
for t in xrange( tex.size() ):
    print 'time:', t
    tx = tex[t]
    print 'texture:', tx, ', size:', tx.nItem()

fileout = '/tmp/toto.tex'
print 'writing texture to', fileout
w = aims.Writer()
w.write( tex, fileout )
print 'object type:', w.writtenObjectType()
print 'data type:', w.writtenObjectDataType()
print 'full type:', w.writtenObjectFullType()

