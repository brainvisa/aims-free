%ModuleHeaderCode
#include <aims/mesh/meshwatershed.h>
inline int meshWatershed( const AimsSurfaceTriangle & mesh,
                     const std::vector<double> & field,std::vector<int> & idx,
                     std::vector<int> & depth, std::vector<int> & major,
                     std::vector<int> & label, double threshold )
{
  return aims::meshWatershed( mesh, field, idx, depth, major, label,
    threshold );
}
%End

/* namespace aims
{
*/

/*
  void distancesFromMesh( const AimsSurfaceTriangle & mesh,
                          vector_AimsVector_DOUBLE_2 & edges,
                          vector_DOUBLE & weights );
*/
  int meshWatershed( const AimsSurfaceTriangle & mesh,
                     const vector_DOUBLE & field, vector_S32 & idx,
                     vector_S32 & depth, vector_S32 & major,
                     vector_S32 & label, double threshold );

//};

