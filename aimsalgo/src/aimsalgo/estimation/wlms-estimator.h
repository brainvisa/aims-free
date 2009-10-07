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
class AIMSALGO_API WLMSEstimator : public MEstimator< D >
{
  public:
    WLMSEstimator( MEstimatorFunc& mfunc, float error,
                   bool verbose = false )
                 : MEstimator<D>(), _mfunc( mfunc ), _error( error ),
                   _verbose( verbose) { }
    virtual ~WLMSEstimator() { }

    void doit( const AimsData< AimsVector< float, D > >& x,
               const AimsData< float >& y, float& a,
               AimsVector< float, D >& b );

  protected:
    MEstimatorFunc& _mfunc;
    float _error;
    bool _verbose;

    float sigma( const AimsData< AimsVector< float, D > >& x,
                 const AimsData< float >& y, float a,
                 const AimsVector< float, D >& b,
                 AimsData<float>& error );

    void step( const AimsData< AimsVector< float, D > >& x,
               const AimsData< float >& y, float& a,
               AimsVector< float, D >& b,
               const AimsData<float>& error );
};


template < int D > inline
float WLMSEstimator<D>::sigma( const AimsData< AimsVector< float, D > >& x,
                               const AimsData< float >& y, float a,
                               const AimsVector< float, D >& b,
                               AimsData<float>& error )
{
  int N = y.dimX(), n, k;
  for ( n = 0; n < N; n++ )
  {
    error( n ) = y ( n ) - a;
    for ( k = 0; k < D; k++ )
      error( n ) -= b.item( k ) * x( n ).item( k );
    error( n ) = fabs( error(  n ) );
  }

  AimsData<float> errorbis = error.clone();
  incSorting( errorbis );

  return 1.4826 * errorbis( N / 2 );
}


template < int D >
inline
void
WLMSEstimator<D>::step( const AimsData< AimsVector< float, D > >& x,
                        const AimsData< float >& y, float& a,
                        AimsVector< float, D >& b,
                        const AimsData<float>& error )
{
  int N = y.dimX();
  AimsData<float> mat( D + 1, D + 1 );
  AimsData<float> vec( D + 1 );
  AimsData<float> weight( N );

  mat = 0.0;
  vec = 0.0;
  weight = 0.0;

  int k, n;

  for ( n = 0; n < N; n++ )
  {
    weight( n ) = _mfunc.weight( error( n ) );
    mat( 0, 0 ) += weight( n );
  }

  for ( k = 1; k <= D; k++ )
  {
    for ( n = 0; n < N; n++ )
      mat( k, 0 ) += weight( n ) * x( n ).item( k - 1 );
    mat( 0, k ) = mat( k, 0 );
  }

  int k1, k2;
  for ( k1 = 1; k1 <= D; k1++ )
    for ( k2 = 1; k2 <= k1; k2++ )
    {
      for ( n = 0; n < N; n++ )
        mat( k1, k2 ) += weight( n ) * 
                         x( n ).item( k1 - 1 ) * x( n ).item( k2 - 1 );
      mat( k2, k1 ) = mat( k1, k2 );
    }

  
  for ( n = 0; n < N; n++ )
  {
    vec( 0 ) += weight( n ) * y( n );
    for ( k = 1; k <= D; k++ )
      vec( k ) += weight( n ) * y( n ) * x( n ).item( k - 1 );
  }

  AimsData<float> res = AimsLinearResolutionLU( mat, vec );
  a = res( 0 );
  for ( k = 1; k <= D; k++ )
    b.item( k - 1 ) = res( k );
}


template < int D > inline
void WLMSEstimator<D>::doit( const AimsData< AimsVector< float, D > >& x,
                             const AimsData< float >& y, float& a,
                             AimsVector< float, D >& b )
{
  ASSERT( x.dimY() == 1 && x.dimZ() == 1 && x.dimT() == 1 );
  ASSERT( y.dimY() == 1 && y.dimZ() == 1 && y.dimT() == 1 );
  ASSERT( x.dimX() == y.dimX() );

  float obja, objb;
  float old_a;
  AimsVector< float, D > old_b;

  // estimate starting value with LMS fit
  LMSEstimator<D> lms;
  lms.doit( x, y, a, b );

  AimsData<float> error( x.dimX() );
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
