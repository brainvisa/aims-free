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

#include <aims/pyramid/splinepyramid_d.h>

using namespace carto;
using namespace std;

namespace aims {

  //==========================================================================
  //
  //                            SPLINE PYRAMID
  //
  //==========================================================================

  //--------------------------------------------------------------------------
  //   Constructor / Destructor / Assignment
  //--------------------------------------------------------------------------

  SplinePyramid::SplinePyramid( const vector<InterpolatedVolume> & pyramid ):
    _pyramid( pyramid )
  {}

  SplinePyramid::SplinePyramid( const SplinePyramid & other ):
    _pyramid( other._pyramid )
  {}

  SplinePyramid::~SplinePyramid()
  {}

  SplinePyramid & SplinePyramid::operator= ( const SplinePyramid & other )
  {
    if( this != &other )
    {
      _pyramid = other._pyramid;
    }
    return *this;
  }

  //--------------------------------------------------------------------------
  /// Change pyramid
  //--------------------------------------------------------------------------

  const vector<InterpolatedVolume> & SplinePyramid::pyramid() const
  {
    return _pyramid;
  }

  void SplinePyramid::setPyramid( const vector<InterpolatedVolume> & pyramid )
  {
    _pyramid = pyramid;
  }

  //--------------------------------------------------------------------------
  //   Accessor
  //--------------------------------------------------------------------------

  InterpolatedVolume &
  SplinePyramid::operator[] ( unsigned level )
  {
    return _pyramid[level];
  }

  const InterpolatedVolume &
  SplinePyramid::operator[] ( unsigned level ) const
  {
    return _pyramid[level];
  }

  int SplinePyramid::nlevel() const
  {
    return _pyramid.size();
  }

  //--------------------------------------------------------------------------
  //   Dimension
  //--------------------------------------------------------------------------

  double SplinePyramid::getSizeXMm() const
  {
    return _pyramid[0].getSizeXMm();
  }

  double SplinePyramid::getSizeYMm() const
  {
    return _pyramid[0].getSizeYMm();
  }

  double SplinePyramid::getSizeZMm() const
  {
    return _pyramid[0].getSizeZMm();
  }

  carto::PropertySet & SplinePyramid::header( unsigned level )
  {
    return _pyramid[level].header();
  }

  const carto::PropertySet & SplinePyramid::header( unsigned level ) const
  {
    return _pyramid[level].header();
  }

  //--------------------------------------------------------------------------
  //   Instantiate
  //--------------------------------------------------------------------------

#define splinepyr_instantiate_methods( T ) \
  template SplinePyramid::SplinePyramid( const std::vector<carto::VolumeRef<T> > &, unsigned, bool );

  splinepyr_instantiate_methods( int8_t )
  splinepyr_instantiate_methods( uint8_t )
  splinepyr_instantiate_methods( int16_t )
  splinepyr_instantiate_methods( uint16_t )
  splinepyr_instantiate_methods( int32_t )
  splinepyr_instantiate_methods( uint32_t )
  splinepyr_instantiate_methods( int64_t )
  splinepyr_instantiate_methods( uint64_t )
  splinepyr_instantiate_methods( float )
  splinepyr_instantiate_methods( double )

//============================================================================
//
//                         SPLINE PYRAMID BUILDER
//
//============================================================================

class SplinePyramidbuilder;

#define bld_instantiate_method( T ) \
  template SplinePyramid SplinePyramidBuilder::execute<T>( const VolumeRef<T> &, bool ) const;

bld_instantiate_method( int8_t ) \
bld_instantiate_method( uint8_t ) \
bld_instantiate_method( int16_t ) \
bld_instantiate_method( uint16_t ) \
bld_instantiate_method( int32_t ) \
bld_instantiate_method( uint32_t ) \
bld_instantiate_method( int64_t ) \
bld_instantiate_method( uint64_t ) \
bld_instantiate_method( float ) \
bld_instantiate_method( double )

}
