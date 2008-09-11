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


#include <aims/topology/connectivity6.h>


Connectivity6::Connectivity6() : TopologicalRelation()
{
  fillNeighbors();
}


void Connectivity6::fillNeighbors()
{
  int i, j, k, x, y, z, *rel;

  for ( i=0; i<27; i++ )
    {
      x = P[ i ][ 0 ];
      y = P[ i ][ 1 ];
      z = P[ i ][ 2 ];

      rel = _relation[ i ];

      _nbNeighbors[ i ] = 0;

      for ( j=0, k=0; j<27; j++ )
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
