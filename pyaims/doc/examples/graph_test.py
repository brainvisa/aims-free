
from soma import aims
import sys

reader = aims.Reader()
graph = reader.read( '/neurospin/lnao/Panabase/data/diffusion/chaos/graphe/RchaosBase.arg' )
print 'graph:', graph
print 'global properties:', graph.keys()
print 'nodes:', graph.order()
print 'relations:', graph.edgesSize()

# iterate on properties
for p in graph:
    print 'property:', p, ', type:', \
      ( graph[p].type() if isinstance( graph[p], aims.Object ) \
        else type( graph[p] ) )

# iterate on vertices
vert = graph.vertices()
for v in vert:
    print 'vertex:', v.getSyntax(), ', edges:', v.edgesSize()
    if v.has_key( 'name' ):
        print '  name:', v[ 'name' ]
    if v.has_key( 'label' ):
        print '  label:', v[ 'label' ]
    try:
        # access mesh in attribute 'aims_Tmtktri'
        mesh = v[ 'aims_Tmtktri' ]
        #mesh = aims.AimsSurfaceTriangle.fromObject( m.get() )
        print 'mesh:', mesh, ', polygons:', mesh.polygon().size()
    except:
        pass

# iterate on edges
edges = graph.edges()
for e in edges:
    print 'edge:', e.getSyntax(), ', links', e.verticesSize(), 'vertices'

fileout = '/tmp/toto.arg'
print 'writing graph to', fileout
w = aims.Writer()
w.write( graph, fileout )
print 'object type:', w.writtenObjectType()
print 'data type:', w.writtenObjectDataType()
print 'full type:', w.writtenObjectFullType()

