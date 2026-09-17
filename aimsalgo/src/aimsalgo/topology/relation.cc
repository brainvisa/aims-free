#include <cstdlib>
#include <cstdlib>
#include <aims/topology/relation.h>
#include <math.h>


TopologicalRelation::TopologicalRelation()
{
  init();
}


TopologicalRelation::TopologicalRelation( const TopologicalRelation& other )
{
  int i, j;

  for ( i=0; i<27; i++ )
    {
      P[ i ] = other.deplacement( i );
      _nbNeighbors[ i ] = other.nbNeighbors( i );

      for ( j=0; j<26; j++ )
	_relation[ i ][ j ] = other.relation( i, j );
    }
}


void TopologicalRelation::init()
{
  int i, j, k, cpt = 0;

  for ( i=0; i<27; i++ )
    for ( j=0; j<26; j++ )  _relation[ i ][ j ] = 0;

  P[ cpt++ ] = Point3d( 0, 0, 0 );

  for ( i=-1; i<=1; i++ )
    for ( j=-1; j<=1; j++ )
      for ( k=-1; k<=1; k++ )
	if ( ( abs( i ) + abs( j ) + abs( k ) ) == 1 )
	  P[ cpt++ ] = Point3d( i, j, k );

  for ( i=-1; i<=1; i++ )
    for ( j=-1; j<=1; j++ )
      for ( k=-1; k<=1; k++ )
	if ( ( abs( i ) + abs( j ) + abs( k ) ) == 2 )
	  P[ cpt++ ] = Point3d( i, j, k );

  for ( i=-1; i<=1; i++ )
    for ( j=-1; j<=1; j++ )
      for ( k=-1; k<=1; k++ )
	if ( ( abs( i ) + abs( j ) + abs( k ) ) == 3 )
	  P[ cpt++ ] = Point3d( i, j, k );
}
