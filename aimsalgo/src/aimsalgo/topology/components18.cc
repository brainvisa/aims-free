
#include <aims/topology/components18.h>


Components18Neighborhood::Components18Neighborhood( TopologicalRelation topo, 
						    Adjacency a )
  : Components( topo, a )
{
}


int Components18Neighborhood::getScalar( int *X )
{
  int point, *Xptr = X;

  for ( point=19; point--; Xptr++ )
    if ( *Xptr )  *Xptr = Components::Exist;

  for ( point=8; point--; Xptr++ )
    if ( *Xptr )  *Xptr = Components::TooFar;
  
  return Components::getScalar( X );
}
