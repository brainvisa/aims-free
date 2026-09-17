
#include <aims/moment/triMoment.h>
#include <aims/moment/momTriFactory.h>


TriangulationMoment::TriangulationMoment( MomentType type )
{
  stgy = 0;
  setMomentType( type );
}


void TriangulationMoment::setMomentType( MomentType type )
{
  delete stgy;
  
  MomentTriangleFactory factory;
  stgy = factory.create( type );
}


void TriangulationMoment::doit( AimsSurfaceTriangle& surfTri )
{
  int i;

  _cx = _cy = _cz = _ct = 1.0;

  _sum = 0.0;
  _m0 = 0.0;

  for ( i=0; i<3; i++ )  _m1[ i ] = 0.0;
  for ( i=0; i<6; i++ )  _m2[ i ] = 0.0;
  for ( i=0; i<10; i++ )  _m3[ i ] = 0.0;

  stgy->doit( (Moment< Void > *)this, surfTri );

  _g[ 0 ] = _m1[ 0 ] / _sum;
  _g[ 1 ] = _m1[ 1 ] / _sum;
  _g[ 2 ] = _m1[ 2 ] / _sum;

  Moment< int16_t >::orientation();
}
