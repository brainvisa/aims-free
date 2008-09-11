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

void Mesher::getReducedNeighOutwardCycles( MapOfFacet& thing )
{
  Facet* facet = NULL;
  Point3d LookUpVertex[ 6 ][ 4 ], q, r;
  Point3df LookUpNormal[ 6 ], m, a, b;
  int i, j, k, u;
  bool ok;

  LookUpVertex[ 0 ][ 0 ] = Point3d( 0, 1, 0 );
  LookUpVertex[ 0 ][ 1 ] = Point3d( 0, 1, 1 );
  LookUpVertex[ 0 ][ 2 ] = Point3d( 0, 0, 1 );
  LookUpVertex[ 0 ][ 3 ] = Point3d( 0, 0, 0 );

  LookUpVertex[ 1 ][ 0 ] = Point3d( 1, 0, 0 );
  LookUpVertex[ 1 ][ 1 ] = Point3d( 0, 0, 0 );
  LookUpVertex[ 1 ][ 2 ] = Point3d( 0, 0, 1 );
  LookUpVertex[ 1 ][ 3 ] = Point3d( 1, 0, 1 );

  LookUpVertex[ 2 ][ 0 ] = Point3d( 1, 1, 0 );
  LookUpVertex[ 2 ][ 1 ] = Point3d( 0, 1, 0 );
  LookUpVertex[ 2 ][ 2 ] = Point3d( 0, 0, 0 );
  LookUpVertex[ 2 ][ 3 ] = Point3d( 1, 0, 0 );

  LookUpVertex[ 3 ][ 0 ] = Point3d( 0, 1, 1 );
  LookUpVertex[ 3 ][ 1 ] = Point3d( 1, 1, 1 );
  LookUpVertex[ 3 ][ 2 ] = Point3d( 1, 0, 1 );
  LookUpVertex[ 3 ][ 3 ] = Point3d( 0, 0, 1 );

  LookUpVertex[ 4 ][ 0 ] = Point3d( 1, 1, 1 );
  LookUpVertex[ 4 ][ 1 ] = Point3d( 0, 1, 1 );
  LookUpVertex[ 4 ][ 2 ] = Point3d( 0, 1, 0 );
  LookUpVertex[ 4 ][ 3 ] = Point3d( 1, 1, 0 );

  LookUpVertex[ 5 ][ 0 ] = Point3d( 1, 1, 1 );
  LookUpVertex[ 5 ][ 1 ] = Point3d( 1, 1, 0 );
  LookUpVertex[ 5 ][ 2 ] = Point3d( 1, 0, 0 );
  LookUpVertex[ 5 ][ 3 ] = Point3d( 1, 0, 1 );

  LookUpNormal[ 0 ] = Point3df( -1,  0,  0 );
  LookUpNormal[ 1 ] = Point3df(  0, -1,  0 );
  LookUpNormal[ 2 ] = Point3df(  0,  0, -1 );
  LookUpNormal[ 3 ] = Point3df(  0,  0,  1 );
  LookUpNormal[ 4 ] = Point3df(  0,  1,  0 );
  LookUpNormal[ 5 ] = Point3df(  1,  0,  0 );

  MapOfFacet::iterator it;

  for ( it = thing.begin(); it != thing.end(); it++ )
  {
    facet = it->second;
    //
    // look for the neighbor facet of f that has no common verteces with
    // is neighbor number 0
    //
    u  = 0;
    ok = true;
    for ( j = 1; j < 4; j++ )
    {
      for ( i = 0; i < 4; i++ )
      {
        q = facet->pNeighbor( 0 )->location() + 
            LookUpVertex[ facet->pNeighbor( 0 )->type() ][ i ];
        for ( k = 0; k < 4; k++ )
        {
          r = facet->pNeighbor( j )->location() + 
              LookUpVertex[ facet->pNeighbor( j )->type() ][ k ];
          if ( r == q )
          {
            ok = false;
            k = i = 4;
          }
        }
      }
      if ( ok == true )
      {
        u = j;
        j = 4;
      }
    }
    //
    //  swaps [u] and [2] neighbors in order that [0],[1],[2],[3]
    //  form a cycle (direct or not)
    //                  +-+                   +-+
    //                  |0|                   |0|
    //                +-+-+-+               +-+-+-+
    //                |1|F|3|               |3|F|1|
    //                +-+-+-+               +-+-+-+
    //                  |2|                   |2|
    //                  +-+                   +-+
    //               direct cycle         no-direct cycle
    //                 
    swap( facet->pNeighbor( u ), facet->pNeighbor( 2 ) );

    //
    //                           ->   --->   ----->
    // processes the dot product N.( F[0] ^ [0][1] ) where N is the normal
    // if it is negative, swap [1] and [3] to get a direct cycle
    //
    m[ 0 ] = facet->location().item( 0 ) + 
             LookUpNormal[ facet->type() ][ 0 ] / 2.0f;
    m[ 1 ] = facet->location().item( 1 ) + 
             LookUpNormal[ facet->type() ][ 1 ] / 2.0f;
    m[ 2 ] = facet->location().item( 2 ) + 
             LookUpNormal[ facet->type() ][ 2 ] / 2.0f;

    a[ 0 ] = facet->pNeighbor( 0 )->location().item( 0 ) + 
             LookUpNormal[ facet->pNeighbor( 0 )->type() ][ 0 ] / 2.0f;
    a[ 1 ] = facet->pNeighbor( 0 )->location().item( 1 ) + 
             LookUpNormal[ facet->pNeighbor( 0 )->type() ][ 1 ] / 2.0f;
    a[ 2 ] = facet->pNeighbor( 0 )->location().item( 2 ) + 
             LookUpNormal[ facet->pNeighbor( 0 )->type() ][ 2 ] / 2.0f;

    b[ 0 ] = facet->pNeighbor( 1 )->location().item( 0 ) + 
             LookUpNormal[ facet->pNeighbor( 1 )->type() ][ 0 ] / 2.0f;
    b[ 1 ] = facet->pNeighbor( 1 )->location().item( 1 ) + 
             LookUpNormal[ facet->pNeighbor( 1 )->type() ][ 1 ] / 2.0f;
    b[ 2 ] = facet->pNeighbor( 1 )->location().item( 2 ) + 
             LookUpNormal[ facet->pNeighbor( 1 )->type() ][ 2 ] / 2.0f;


    // m ----> F
    // a ----> [0]
    // b ----> [1]
    if ( LookUpNormal[ facet->type() ].dot( crossed( a - m, b - a) ) < 0 )
      swap( facet->pNeighbor( 1 ), facet->pNeighbor( 3 ) );
  }
}
