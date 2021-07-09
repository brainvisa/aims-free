
%ModuleHeaderCode
#include <aims/mesh/geometric.h>
%End

/* namespace aims
{ */

  vector_AimsVector_U32_3*
  simple_delaunay_triangulation( const vector_POINT2DF & points ) /Factory/;
%Docstring
    SimpleDelaunayTriangulation

    Get a Delaunay triangulation for a polygon. Poins in the polygon should
    be ordered in a clockwise or counterclockwise order.

    Note that it is different from a classical Delaunay algorithm wich meshes
    a cloud point (and its convex hull), not a polygon as here.
%End

%MethodCode
  std::list<AimsVector<uint, 3> > res
    = aims::simple_delaunay_triangulation( *a0 );
  sipRes = new vector_AimsVector_U32_3( res.begin(), res.end() );
%End

// };

