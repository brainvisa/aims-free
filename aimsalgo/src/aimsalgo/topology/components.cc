/* This software and supporting documentation are distributed by
 *     Institut Federatif de Recherche 49
 *     CEA/NeuroSpin, Batiment 145,
 *     91191 Gif-sur-Yvette cedex
 *     France
 *
 * This software is governed by the CeCILL-B license under
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the
 * terms of the CeCILL-B license as circulated by CEA, CNRS
 * and INRIA at the following URL "http://www.cecill.info".
 *
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability.
 *
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or
 * data to be ensured and,  more generally, to use and operate it in the
 * same conditions as regards security.
 *
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL-B license and that you accept its terms.
 */


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
