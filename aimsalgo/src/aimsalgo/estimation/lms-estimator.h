
#ifndef AIMS_ESTIMATION_LMS_ESTIMATOR_H
#define AIMS_ESTIMATION_LMS_ESTIMATOR_H


#include <aims/config/aimsalgo_config.h>
#include <cartodata/volume/volume.h>
#include <aims/vector/vector.h>
#include <aims/def/assert.h>
#include <aims/math/gausslu.h>
#include <aims/estimation/m-estimator.h>

//
// Linear Mean Squared M-estimator
//
template < int D >
class LMSEstimator : public MEstimator< D >
{
  public:
    LMSEstimator() : MEstimator<D>() { }
    virtual ~LMSEstimator() { }

    void doit(
      const carto::rc_ptr<carto::Volume< AimsVector< float, D > > >& x,
      const carto::rc_ptr<carto::Volume< float > >& y, float& a,
      AimsVector< float, D >& b );
};


template < int D > inline
void
LMSEstimator<D>::doit(
  const carto::rc_ptr<carto::Volume< AimsVector< float, D > > >& x,
  const carto::rc_ptr<carto::Volume< float > >& y, float& a,
  AimsVector< float, D >& b )
{
  ASSERT( x->getSizeY() == 1 && x->getSizeZ() == 1 && x->getSizeT() == 1 );
  ASSERT( y->getSizeY() == 1 && y->getSizeZ() == 1 && y->getSizeT() == 1 );
  ASSERT( x->getSizeX() == y->getSizeX() );

  int N = y->getSizeX();
  carto::VolumeRef<float> mat( D + 1, D + 1 );
  carto::VolumeRef<float> vec( D + 1 );

  mat = 0.0;
  vec = 0.0;
  mat( 0, 0 ) = float( N );
  int k, n;
  for ( k = 1; k <= D; k++ )
  {
    for ( n = 0; n < N; n++ )
      mat( k, 0 ) += x->at( n ).item( k - 1 );
    mat( 0, k ) = mat( k, 0 );
  }

  int k1, k2;
  for ( k1 = 1; k1 <= D; k1++ )
    for ( k2 = 1; k2 <= k1; k2++ )
    {
      for ( n = 0; n < N; n++ )
        mat( k1, k2 ) += x->at( n ).item( k1 - 1 ) * x->at( n ).item( k2 - 1 );
      mat( k2, k1 ) = mat( k1, k2 );
    }

  
  for ( n = 0; n < N; n++ )
  {
    vec( 0 ) += y->at( n );
    for ( k = 1; k <= D; k++ )
      vec( k ) += y->at( n ) * x->at( n ).item( k - 1 );
  }

  carto::VolumeRef<float> res = AimsLinearResolutionLU( mat, vec );
  a = res( 0 );
  for ( k = 1; k <= D; k++ )
    b.item( k - 1 ) = res( k );
}


#endif
