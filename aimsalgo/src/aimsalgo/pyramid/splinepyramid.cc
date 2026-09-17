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
