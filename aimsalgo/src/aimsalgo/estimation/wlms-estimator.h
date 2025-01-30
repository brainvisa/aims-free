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


#ifndef AIMS_ESTIMATION_WLMS_ESTIMATOR_H
#define AIMS_ESTIMATION_WLMS_ESTIMATOR_H

#include <cstdlib>
#include <aims/config/aimsalgo_config.h>
#include <aims/estimation/lms-estimator.h>
#include <aims/estimation/m-func.h>

//
// Weighted Linear Mean Squared M-estimator
//
template < int D >
class WLMSEstimator : public MEstimator< D >
{
  public:
    WLMSEstimator( MEstimatorFunc& mfunc, float error,
                   bool verbose = false )
                 : MEstimator<D>(), _mfunc( mfunc ), _error( error ),
                   _verbose( verbose) { }
    virtual ~WLMSEstimator() { }

    void doit(
      const carto::rc_ptr<carto::Volume< AimsVector< float, D > > >& x,
      const carto::rc_ptr<carto::Volume< float > >& y, float& a,
      AimsVector< float, D >& b );

  protected:
    MEstimatorFunc& _mfunc;
    float _error;
    bool _verbose;

    float sigma(
      const carto::rc_ptr<carto::Volume< AimsVector< float, D > > >& x,
      const carto::rc_ptr<carto::Volume< float > >& y, float a,
      const AimsVector< float, D >& b,
      carto::rc_ptr<carto::Volume<float> >& error );

    void step(
      const carto::rc_ptr<carto::Volume< AimsVector< float, D > > >& x,
      const carto::rc_ptr<carto::Volume< float > >& y, float& a,
      AimsVector< float, D >& b,
      const carto::rc_ptr<carto::Volume<float> >& error );
};


template < int D > inline
float WLMSEstimator<D>::sigma(
  const carto::rc_ptr<carto::Volume< AimsVector< float, D > > >& x,
  const carto::rc_ptr<carto::Volume< float > >& y, float a,
  const AimsVector< float, D >& b,
  carto::rc_ptr<carto::Volume<float> >& error )
{
  int N = y->getSizeX(), n, k;
  for ( n = 0; n < N; n++ )
  {
    error->at( n ) = y->at( n ) - a;
    for ( k = 0; k < D; k++ )
      error->at( n ) -= b.item( k ) * x->at( n ).item( k );
    error->at( n ) = fabs( error->at(  n ) );
  }

  carto::Volume<float> errorbis( &(*error) );
  carto::sort( errorbis );

  return 1.4826 * errorbis( N / 2 );
}


template < int D >
inline
void
WLMSEstimator<D>::step( const carto::rc_ptr<carto::Volume< AimsVector< float, D > > >& x,
                        const carto::rc_ptr<carto::Volume< float > >& y, float& a,
                        AimsVector< float, D >& b,
                        const carto::rc_ptr<carto::Volume<float> >& error )
{
  int N = y->getSizeX();
  carto::VolumeRef<float> mat( D + 1, D + 1, 1, 1,
                               carto::AllocatorContext::fast() );
  carto::VolumeRef<float> vec( D + 1, 1, 1, 1,
                               carto::AllocatorContext::fast() );
  carto::VolumeRef<float> weight( N, 1, 1, 1,
                                  carto::AllocatorContext::fast() );

  mat = 0.0;
  vec = 0.0;
  weight = 0.0;

  int k, n;

  for ( n = 0; n < N; n++ )
  {
    weight( n ) = _mfunc.weight( error->at( n ) );
    mat( 0, 0 ) += weight( n );
  }

  for ( k = 1; k <= D; k++ )
  {
    for ( n = 0; n < N; n++ )
      mat( k, 0 ) += weight( n ) * x->at( n ).item( k - 1 );
    mat( 0, k ) = mat( k, 0 );
  }

  int k1, k2;
  for ( k1 = 1; k1 <= D; k1++ )
    for ( k2 = 1; k2 <= k1; k2++ )
    {
      for ( n = 0; n < N; n++ )
        mat( k1, k2 ) += weight( n ) * 
                         x->at( n ).item( k1 - 1 ) * x->at( n ).item( k2 - 1 );
      mat( k2, k1 ) = mat( k1, k2 );
    }

  
  for ( n = 0; n < N; n++ )
  {
    vec( 0 ) += weight( n ) * y->at( n );
    for ( k = 1; k <= D; k++ )
      vec( k ) += weight( n ) * y->at( n ) * x->at( n ).item( k - 1 );
  }

  carto::VolumeRef<float> res = AimsLinearResolutionLU( mat, vec );
  a = res( 0 );
  for ( k = 1; k <= D; k++ )
    b.item( k - 1 ) = res( k );
}


template < int D > inline
void WLMSEstimator<D>::doit( const carto::rc_ptr<carto::Volume< AimsVector< float, D > > >& x,
                             const carto::rc_ptr<carto::Volume< float > >& y, float& a,
                             AimsVector< float, D >& b )
{
  ASSERT( x->getSizeY() == 1 && x->getSizeZ() == 1 && x->getSizeT() == 1 );
  ASSERT( y->getSizeY() == 1 && y->getSizeZ() == 1 && y->getSizeT() == 1 );
  ASSERT( x->getSizeX() == y->getSizeX() );

  float obja, objb;
  float old_a;
  AimsVector< float, D > old_b;

  // estimate starting value with LMS fit
  LMSEstimator<D> lms;
  lms.doit( x, y, a, b );

  carto::VolumeRef<float> error( x->getSizeX() );
  int count = 0;
  // perform iteration of robust estimation until convergence
  do
  {
    ASSERT( count != 100 ); // maximum iteration
    old_a = a;
    old_b = b;
    _mfunc.setSigma( sigma( x, y, old_a,old_b, error ) );
    step( x, y, a, b, error );
    obja = fabs( a - old_a ) / ( fabs( a ) + fabs( old_a ) );
    objb = norm( b - old_b ) / ( norm( b ) + norm( old_b ) );
    if ( _verbose )
      std::cout
           << "Obja : " << obja << "     "
           << "Objb : " << objb << "     "
           << "a : " << a << "     "
           << "b : " << b << std::endl;
    count++;
  }
  while ( obja > _error || objb > _error );
}


#endif
