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
