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

#include <aims/pyramid/splinesubsampler_d.h>

using namespace carto;

namespace aims {

//============================================================================
//
//                      CONVOLUTION SUBSAMPLER
//
//============================================================================

template class ConvolutionSubSampler<DiscreteBSpline>;

#define conv_instantiate_methods_2( IN, OUT ) \
  template VolumeRef<OUT> ConvolutionSubSampler<DiscreteBSpline>::execute( const VolumeRef<IN> & ) const; \
  template VolumeRef<OUT> ConvolutionSubSampler<DiscreteBSpline>::execute( const VolumeRef<IN> &, VolumeRef<OUT> & ) const;

#define conv_instantiate_methods( T ) \
  conv_instantiate_methods_2( T, uint8_t )  \
  conv_instantiate_methods_2( T, int8_t )   \
  conv_instantiate_methods_2( T, uint16_t ) \
  conv_instantiate_methods_2( T, int16_t )  \
  conv_instantiate_methods_2( T, uint32_t ) \
  conv_instantiate_methods_2( T, int32_t )  \
  conv_instantiate_methods_2( T, uint64_t ) \
  conv_instantiate_methods_2( T, int64_t )  \
  conv_instantiate_methods_2( T, float )    \
  conv_instantiate_methods_2( T, double )

conv_instantiate_methods( uint8_t )
conv_instantiate_methods( int8_t )
conv_instantiate_methods( uint16_t )
conv_instantiate_methods( int16_t )
conv_instantiate_methods( uint32_t )
conv_instantiate_methods( int32_t )
conv_instantiate_methods( uint64_t )
conv_instantiate_methods( int64_t )
conv_instantiate_methods( float )
conv_instantiate_methods( double )

//============================================================================
//
//                      SPLINE SUBSAMPLER
//
//============================================================================

class DirectBSplineSubSampler;

#define instantiate_methods_2( IN, OUT ) \
  template VolumeRef<OUT> DirectBSplineSubSampler::execute( const VolumeRef<IN> &, VolumeRef<OUT> & ) const;

#define instantiate_methods( IN ) \
  instantiate_methods_2( IN, int8_t )      \
  instantiate_methods_2( IN, uint8_t )     \
  instantiate_methods_2( IN, int16_t )     \
  instantiate_methods_2( IN, uint16_t )    \
  instantiate_methods_2( IN, int32_t )     \
  instantiate_methods_2( IN, uint32_t )    \
  instantiate_methods_2( IN, int64_t )     \
  instantiate_methods_2( IN, uint64_t )    \
  instantiate_methods_2( IN, float )       \
  instantiate_methods_2( IN, double )

instantiate_methods( int8_t )   \
instantiate_methods( uint8_t )  \
instantiate_methods( int16_t )  \
instantiate_methods( uint16_t ) \
instantiate_methods( int32_t )  \
instantiate_methods( uint32_t ) \
instantiate_methods( int64_t )  \
instantiate_methods( uint64_t ) \
instantiate_methods( float )    \
instantiate_methods( double )

}
