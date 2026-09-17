
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
