

#include <aims/moment/momSurfStgy.h>


void MomentSurfacicStrategy::update( Moment< Void > *m, Point3df& v1,
				     Point3df& v2, Point3df& v3 )
{
  double Ar, x, y, z;
  double a1, a2, a3, b1, b2, b3, c1, c2, c3;

  computeFirst( v1, v2, v3 );

  a1 = (double)v1[0];
  a2 = (double)v1[1];
  a3 = (double)v1[2];

  b1 = (double)v2[0];
  b2 = (double)v2[1];
  b3 = (double)v2[2];

  c1 = (double)v3[0];
  c2 = (double)v3[1];
  c3 = (double)v3[2];

  // Surface d'un triangle: Ar = ||(v2-v1)x(v3-v1)||/2
  x = ( b2 - a2 ) * ( c3 - a3 ) - ( b3 - a3 ) * ( c2 - a2 );
  y = ( b3 - a3 ) * ( c1 - a1 ) - ( b1 - a1 ) * ( c3 - a3 );
  z = ( b1 - a1 ) * ( c2 - a2 ) - ( b2 - a2 ) * ( c1 - a1 );
  Ar=(double)sqrt( x*x + y*y + z*z ) / 2.0;

  m->sum()++;

  m->m0() += Ar;

  m->m1()[ 0 ] += Ar * _mt1[ 0 ] / 3.0;
  m->m1()[ 1 ] += Ar * _mt1[ 1 ] / 3.0;
  m->m1()[ 2 ] += Ar * _mt1[ 2 ] / 3.0;

  m->m2()[ 0 ] += Ar * _mt2[ 0 ] / 12.0;
  m->m2()[ 1 ] += Ar * _mt2[ 1 ] / 12.0;
  m->m2()[ 2 ] += Ar * _mt2[ 2 ] / 12.0;
  m->m2()[ 3 ] += Ar * _mt2[ 3 ] / 24.0;
  m->m2()[ 4 ] += Ar * _mt2[ 4 ] / 24.0;
  m->m2()[ 5 ] += Ar * _mt2[ 5 ] / 24.0;

  m->m3()[ 0 ] += Ar * _mt3[ 0 ] / 60.0;
  m->m3()[ 1 ] += Ar * _mt3[ 1 ] / 60.0;
  m->m3()[ 2 ] += Ar * _mt3[ 2 ] / 60.0;
  m->m3()[ 3 ] += Ar * _mt3[ 3 ] / 180.0;
  m->m3()[ 4 ] += Ar * _mt3[ 4 ] / 180.0;
  m->m3()[ 5 ] += Ar * _mt3[ 5 ] / 180.0;
  m->m3()[ 6 ] += Ar * _mt3[ 6 ] / 180.0;
  m->m3()[ 7 ] += Ar * _mt3[ 7 ] / 180.0;
  m->m3()[ 8 ] += Ar * _mt3[ 8 ] / 180.0;
  m->m3()[ 9 ] += Ar * _mt3[ 9 ] / 360.0;
}


void MomentSurfacicStrategy::doit( Moment< Void > *m, AimsSurfaceTriangle& s )
{
  AimsSurfaceTriangle::iterator it = s.begin();

  while ( it != s.end() )
    {
      update( m, it->second.vertex()[0], it->second.vertex()[1], 
	      it->second.vertex()[2] );
      ++it;
    }
}
