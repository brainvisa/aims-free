
// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/mesh/mesher.h>

using namespace std;

float Mesher::surfaceOfInterface( const AimsSurfaceTriangle& surface )
{
  const vector< Point3df >& vertex = surface.vertex();
  const vector< AimsVector< uint,3> >& triangle = surface.polygon();

  float surf = 0.0f;
  Point3df H, AB, AC, CH, zero( 0.0, 0.0, 0.0 );
  int size = (int)triangle.size();

  for ( int n = 0; n < size; n++ )
  {
    AB = vertex[ triangle[ n ][ 1 ] ] - vertex[ triangle[ n ][ 0 ] ];
    AC = vertex[ triangle[ n ][ 2 ] ] - vertex[ triangle[ n ][ 0 ] ];
    H  = vertex[ triangle[ n ][ 0 ] ] + ( AB.dot( AC ) / norm2( AB ) ) * AB;
    CH = H - vertex[ triangle[ n ][ 2 ] ];
    if ( AB != zero )
      surf += norm( CH ) * norm( AB ) / 2;
  }

  return surf;
}
