/* Copyright (c) 1995-2007 CEA
 *
 *  This software and supporting documentation were developed by
 *      CEA/DSV/SHFJ
 *      4 place du General Leclerc
 *      91401 Orsay cedex
 *      France
 *
 * This software is governed by the CeCILL license version 2 under 
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the 
 * terms of the CeCILL license version 2 as circulated by CEA, CNRS
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
 * knowledge of the CeCILL license version 2 and that you accept its terms.
 */


#include <aims/mesh/mesher.h>

using namespace std;

#define FREE         0
#define SLAVE        1
#define MASTER       2
#define GREAT_MASTER 3

void Mesher::getTriangles( const vector< Facet* >& vfac,
                           vector< AimsVector< uint, 3 > >& triangle )
{
  Facet* facet = NULL;
  int n, size = (int)vfac.size();

  // flags initialization
  vector< byte > vflag( size );
  for ( n = 0; n != size; n++ )
  {
    facet = vfac[ n ];
    // I try to remove this, Denis 09/11/01  ASSERT( facet->nNeighbor() >= 3 );
    facet->id() = n;
    vflag[ n ] = FREE;
  }
  
  // 
  // writes triangles
  //
  uint k1,k2;

  // primary loop
  for ( n = 0; n != size; n++ )
  {
    facet = vfac[ n ];
    if ( vflag[ facet->id() ] == FREE )
    {
      vflag[ facet->id() ] = GREAT_MASTER;
      for ( int v = 0; v < facet->nNeighbor(); v++ )
      {
        k1 = (uint)facet->pNeighbor( v )->id();
        k2 = (uint)facet->pNeighbor( ( v + 1 ) % facet->nNeighbor() )->id();
        if ( vflag[ k1 ] == GREAT_MASTER )
          cerr << "pb" << endl;
        if ( vflag[ k2 ] == GREAT_MASTER )
          cerr << "pb" << endl;
        vflag[ k1 ] = SLAVE;
        vflag[ k2 ] = SLAVE;
      }
    }
  }

  // secondary loop
  int u = -1, v;
  vector< list< AimsVector< uint, 3 > > > tabIntercal( size );

  for ( n = 0; n != size; n++ )
  {
    facet = vfac[ n ];
    if ( vflag[ facet->id() ] == SLAVE )
    {
      vflag[ facet->id() ] = MASTER;

      u = -1;
      for ( v = 0; v < facet->nNeighbor(); v++ )
        if ( vflag[ facet->pNeighbor( v )->id() ] == GREAT_MASTER )
        {
          u = facet->pNeighbor( v )->id();
          break;
        }

      if ( u == -1 )
        u = 0;

      for ( v = 0; v < facet->nNeighbor(); v++ )
      {
        k1 = (uint)facet->pNeighbor( v )->id();
        k2 = (uint)facet->pNeighbor( ( v + 1 ) % facet->nNeighbor() )->id();
        if ( vflag[ k1 ] == SLAVE && vflag[ k2 ] == SLAVE )
        {
          AimsVector< uint, 3 > item;
          item[ 0 ] = facet->id();
          item[ 1 ] = k1;
          item[ 2 ] = k2;
          tabIntercal[ u ].push_back( item );
        }
      }
    }
  }

  u = 0;
  for ( n = 0; n != size; n++ )
  {
    facet = vfac[ n ];
    if ( vflag[ facet->id() ] == GREAT_MASTER )
    {
      u += facet->nNeighbor();
      u += (int)tabIntercal[ facet->id() ].size();
    }
  }

  triangle.reserve( u );

  for ( n = 0; n != size; n++ )
  {
    facet = vfac[ n ];
    if ( vflag[ facet->id() ] == GREAT_MASTER )
    {
      for ( v = 0; v < facet->nNeighbor(); v++ )
      {
        k1 = (uint)facet->pNeighbor( v )->id();
        k2 = (uint)facet->pNeighbor( ( v + 1 ) % facet->nNeighbor() )->id();

        AimsVector< uint, 3 > item;
        item[ 0 ] = facet->id();
        item[ 1 ] = k1;
        item[ 2 ] = k2;
        triangle.push_back( item );

      }
      list< AimsVector< uint, 3 > >::iterator i;
      for ( i = tabIntercal[ facet->id() ].begin();
            i!= tabIntercal[ facet->id() ].end(); i++ )
        triangle.push_back( *i );
    }
  }
}
