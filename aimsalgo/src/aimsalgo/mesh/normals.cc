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
        cerr << "Warning : flat triangle encountered" << endl;
      else
      {
        b /= absNormal;
        Normal -= b;
      }
    }
    absNormal = norm( Normal );
    if ( absNormal == 0 )
    {
      cerr << "Warning : null normal encountered" << endl;
      Normal[ 0 ] = 1;
    }
    else
      Normal /= absNormal;
    normal.push_back( Normal );
  }
}
