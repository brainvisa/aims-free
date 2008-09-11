
from soma import aims
import sys

reader = aims.Reader()
bck = reader.read( '/neurospin/lnao/Panabase/data/diffusion/chaos/graphe/RchaosBase.data/ss_Bucket.bck' )
print 'bucket:', bck

print 'timesteps:', bck.size()
for t in xrange( bck.size() ):
    print 'time:', t
    b = bck[t]
    print 'bucket:', b, ', size:', b.size()
    for p in b.keys():
        sys.stdout.write( str( p.list() ) + ' ' )
    print

fileout = '/tmp/toto.bck'
print 'writing bucket to', fileout
w = aims.Writer()
w.write( bck, fileout )
print 'object type:', w.writtenObjectType()
print 'data type:', w.writtenObjectDataType()
print 'full type:', w.writtenObjectFullType()

