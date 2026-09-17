
#include <aims/mesh/mesher.h>

using namespace std;

static 
float _LookUpNormal[ 6 ][ 3 ] = { { -1, 0, 0 }, { 0, -1 , 0 }, { 0, 0, -1 }, 
                                  { 0, 0, 1 }, { 0, 1, 0 }, { 1, 0, 0 } };



void Mesher::getVertices( const vector< Facet* >& vfac,
                          vector< Point3df >& vertex, 
                          float sizeX, float sizeY, float sizeZ )
{
  Facet* facet = NULL;
  int size = (int)vfac.size();

  vertex.reserve( size );

  //
  // -------->                  ------->   -------->
  // vertex[i] = resolution * [ point[i] + Normal[i] / 2 ]
  //
  for ( int n = 0; n < size; n++ )
  {
    facet = vfac[ n ];
    vertex.push_back( Point3df( sizeX * ( facet->location().item( 0 ) +
                                _LookUpNormal[ facet->type() ][ 0 ] / 2.0 ),
                                sizeY * ( facet->location().item( 1 ) +
                                _LookUpNormal[ facet->type() ][ 1 ] / 2.0 ),
                                sizeZ * ( facet->location().item( 2 ) +
                                _LookUpNormal[ facet->type() ][ 2 ] / 2.0 ) ) );
  }
}
