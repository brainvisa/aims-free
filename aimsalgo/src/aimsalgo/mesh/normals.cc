
#include <aims/mesh/mesher.h>

using namespace std;

static Point3df _zero( 0, 0, 0 );

void Mesher::getNormals( const vector< Facet* >& vfac,
                         const vector< Point3df >& vertex,
                         vector< Point3df >& normal )
{
  Facet* facet = NULL;
  Point3df vec1, vec2, b;
  float absNormal = 0.0;
  int size = (int)vfac.size();
  int k1, k2;

  normal.reserve( size );

  //
  //         +-+
  //         |1|         ______         ----->   ----->
  //       +-+-+-+   ->  \     |        [1][2] ^ [f][1]
  //       | |f|2|   n =  \      - -------------------------
  //       +-+-+-+        /             ----->   ----->
  //         | |         /_____|    ||  [1][2] ^ [f][1]  ||
  //         +-+
  //                 ->  ->    ->
  //                 N = n / ||n||
  //
  Point3df Normal;
  for ( int n = 0; n < size; n++ )
  { 
    facet = vfac[ n ];
    Normal = _zero;
    for ( int v = 0; v < facet->nNeighbor(); v++ )
    { 
      k1 = facet->pNeighbor( v )->id();
      k2 = facet->pNeighbor( ( v + 1 ) % facet->nNeighbor() )->id();
      vec1 = vertex[ k2 ] - vertex[ k1 ];
      vec2 = vertex[ k1 ] - vertex[ facet->id() ];
      b  = crossed( vec1, vec2 );
      absNormal = norm( b );
      if ( absNormal == 0 )
        cout << "Warning : flat triangle encountered" << endl;
      else
      {
        b /= absNormal;
        Normal -= b;
      }
    }
    absNormal = norm( Normal );
    if ( absNormal == 0 )
    {
      cout << "Warning : null normal encountered" << endl;
      Normal[ 0 ] = 1;
    }
    else
      Normal /= absNormal;
    normal.push_back( Normal );
  }
}
