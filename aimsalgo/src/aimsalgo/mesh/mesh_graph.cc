
#include <aims/mesh/mesh_graph_d.h>

using namespace aims;
using namespace aims::meshgraph;
using namespace carto;
using namespace std;

namespace aims
{

  namespace meshgraph
  {

    template
    rc_ptr< vector< vector< uint > > >
    circular_neighborhoods( vector<AimsVector<uint, 3> > const & faces,
                            size_t nVertices );

    template
    class List2GraphMeshConvertor<
      MeshVertexNode<uint>, MeshFaceNode<MeshVertexNode<uint> >,
      vector<Point3df>, vector<AimsVector<uint, 3> >, vector< vector< uint > >
      >;

  }
}
