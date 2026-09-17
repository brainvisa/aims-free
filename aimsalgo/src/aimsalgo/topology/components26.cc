
#include <aims/topology/components26.h>


Components26Neighborhood::Components26Neighborhood( TopologicalRelation topo, 
						    Adjacency a )
  : Components( topo, a )
{
}


int Components26Neighborhood::getScalar( int *X )
{
  int point, *Xptr = X;

  for ( point=27; point--; Xptr++ )
    if ( *Xptr )  *Xptr = Components::Exist;

  return Components::getScalar( X );
}
