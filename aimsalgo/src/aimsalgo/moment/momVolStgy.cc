

#include <aims/moment/momVolStgy.h>


void MomentVolumicStrategy::update( Moment< Void > *m, Point3df& v1,
				    Point3df& v2, Point3df& v3 )
{
  double det = 0.0;
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

  det = a1*( b2*c3-b3*c2 ) - b1*( a2*c3-a3*c2 ) + c1*( a2*b3-a3*b2 );

  m->sum()++;

  m->m0() += det / 6.0;

  m->m1()[ 0 ] += det * _mt1[ 0 ] / 24.0;
  m->m1()[ 1 ] += det * _mt1[ 1 ] / 24.0;
  m->m1()[ 2 ] += det * _mt1[ 2 ] / 24.0;

  m->m2()[ 0 ] += det * _mt2[ 0 ] / 60.0;
  m->m2()[ 1 ] += det * _mt2[ 1 ] / 60.0;
  m->m2()[ 2 ] += det * _mt2[ 2 ] / 60.0;
  m->m2()[ 3 ] += det * _mt2[ 3 ] / 120.0;
  m->m2()[ 4 ] += det * _mt2[ 4 ] / 120.0;
  m->m2()[ 5 ] += det * _mt2[ 5 ] / 120.0;

  m->m3()[ 0 ] += det * _mt3[ 0 ] / 120.0;
  m->m3()[ 1 ] += det * _mt3[ 1 ] / 120.0;
  m->m3()[ 2 ] += det * _mt3[ 2 ] / 120.0;
  m->m3()[ 3 ] += det * _mt3[ 3 ] / 360.0;
  m->m3()[ 4 ] += det * _mt3[ 4 ] / 360.0;
  m->m3()[ 5 ] += det * _mt3[ 5 ] / 360.0;
  m->m3()[ 6 ] += det * _mt3[ 6 ] / 360.0;
  m->m3()[ 7 ] += det * _mt3[ 7 ] / 360.0;
  m->m3()[ 8 ] += det * _mt3[ 8 ] / 360.0;
  m->m3()[ 9 ] += det * _mt3[ 9 ] / 720.0;
}


void MomentVolumicStrategy::doit( Moment< Void > *m, AimsSurfaceTriangle& s )
{
  AimsSurfaceTriangle::iterator it = s.begin();

  while ( it != s.end() )
    {
      update( m, it->second.vertex()[0], it->second.vertex()[1], 
	      it->second.vertex()[2] );
      ++it;
    }
}
