#include <cstdlib>
#include <cstdlib>
#include <aims/topology/connectivity6.h>


Connectivity6::Connectivity6() : TopologicalRelation()
{
  fillNeighbors();
}


void Connectivity6::fillNeighbors()
{
  int i, j, x, y, z, *rel;

  for ( i=0; i<27; i++ )
    {
      x = P[ i ][ 0 ];
      y = P[ i ][ 1 ];
      z = P[ i ][ 2 ];

      rel = _relation[ i ];

      _nbNeighbors[ i ] = 0;

      for ( j=0; j<27; j++ )
	if ( ( abs( x - P[ j ][ 0 ] ) <= 1 ) && 
	     ( abs( y - P[ j ][ 1 ] ) <= 1 ) && 
	     ( abs( z - P[ j ][ 2 ] ) <= 1 ) )
	  {
	    int val = abs( x - P[ j ][ 0 ] ) + abs( y - P[ j ][ 1 ] ) +
	      abs( z - P[ j ][ 2 ] );

	    if ( val == 1 )
	      {
		*rel++ = j;
		_nbNeighbors[ i ]++;
	      }
	  }
    }
}
