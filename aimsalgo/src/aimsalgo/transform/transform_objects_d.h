#include <aims/transform/transform_objects.h>

#include <aims/mesh/surface.h>
#include <aims/mesh/surfacemanip.h>

using namespace std;
using namespace carto;

namespace aims
{

template <int D>
void transformMesh( AimsTimeSurface<D, Void> & mesh,
                    const Transformation3d & direct_transformation )
{
  typename AimsTimeSurface<D, Void>::iterator is, es = mesh.end();

  for( is=mesh.begin(); is!=es; ++is )
  {
    vector<Point3df> & vert = is->second.vertex();
    vector<Point3df>::iterator iv, ev = vert.end();
    for( iv=vert.begin(); iv!=ev; ++iv )
    {
      Point3df & p = *iv;
      p = direct_transformation.transform( p );
    }
  }

  if(!direct_transformation.isDirect()) {
    SurfaceManip::invertSurfacePolygons(mesh);
  }

  if(!mesh.normal().empty()) {
    // Old normals are not valid anymore, recalculate them if they were present
    mesh.updateNormals();
  }
}

} // namespace aims
