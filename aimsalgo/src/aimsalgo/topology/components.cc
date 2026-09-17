
// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/topology/components.h>


Components::Components( TopologicalRelation topo, Adjacency a )
  : _topoRel( topo ), _adj( a )
{
}


int Components::getScalar( int *X )
{
  int point, nbComp = 0, point_courant, vois, voisin_courant;
  int *Xptr = X, list[ 27 ], *listptr;

  listptr = list;
  Xptr = X + _adj.begin();

  for ( point=_adj.begin(); point<=_adj.end(); point++ )
    if ( (*Xptr++) == Components::Exist )
      {
	nbComp++;
	*listptr++ = point;
	X[ point ] = nbComp;

	do
	  {
	    point_courant = *(--listptr);

	    for ( vois=0; vois<_topoRel.nbNeighbors( point_courant ); vois++ )
	      {
		voisin_courant = _topoRel.relation( point_courant, vois );

		if ( X[ voisin_courant ] == Components::Exist )
		  {
		    *listptr++ = voisin_courant;
		    X[ voisin_courant ] = nbComp;
		  }
	      }
	  }
	while( listptr != list );
      }

  return nbComp;
}
