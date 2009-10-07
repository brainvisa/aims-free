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


#include <aims/talairach/cramer.h>


Point3df CramerSolver::doit( const Point3df& a, const Point3df& b, 
                             const Point3df& c, const Point3df& d )
{
  Point3df temp, sol;
  Point3df aa( a ), bb( b ), cc( c ), dd( d );
  
  float det = determinant( aa, bb, cc );
  
  for ( int i=0; i<3; i++ )
    {
      temp[ 0 ] = aa[ i ];
      temp[ 1 ] = bb[ i ];
      temp[ 2 ] = cc[ i ];

      aa[ i ] = dd[ 0 ];
      bb[ i ] = dd[ 1 ];
      cc[ i ] = dd[ 2 ];

      sol[ i ] = determinant( a, b, c );

      aa[ i ] = temp[ 0 ];
      bb[ i ] = temp[ 1 ];
      cc[ i ] = temp[ 2 ];
    }

  sol /= det;

  return sol;
}


float CramerSolver::determinant( const Point3df& a, const Point3df& b, 
                                 const Point3df& c )
{
  int i, j=0, k=1, l=2, carry;
  float res = 0.0f;

  for ( i=0; i<3; i++ )
    {
      res += a[ j ] * b[ k ] * c[ l ];
      carry = j;
      j = k;
      k = l;
      l = carry;
    }

  j = 2;
  k = 1;
  l = 0;

  for ( i=0; i<3; i++ )
    {
      res -= a[ j ] * b[ k ] * c[ l ];
      carry = j;
      j = k;
      k = l;
      l = carry;
    }
    
  return res;
}
