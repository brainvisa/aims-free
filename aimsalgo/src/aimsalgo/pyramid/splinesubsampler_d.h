#ifndef AIMS_PYRAMID_SPLINESUBSAMPLER_D_H
#define AIMS_PYRAMID_SPLINESUBSAMPLER_D_H

//--- aims -------------------------------------------------------------------
#include <aims/pyramid/splinesubsampler.h>
#include <aims/pyramid/convolutionsubsampler_d.h>       // aims::ConvolutionSubSampler
#include <aims/math/bspline.h>                          // aims::TabulBSpline
#include <aims/resampling/splineresampler.h>            // aims::mirrorCoeff
#include <aims/utility/progress.h>                      // aims::Progression
//--- cartobase --------------------------------------------------------------
#include <cartobase/config/verbose.h>                   // carto::verbose
//--- std --------------------------------------------------------------------
#include <cmath>                               // std::ceil/floor/isfinite/NAN
#include <vector>
#include <iostream>
//----------------------------------------------------------------------------

namespace aims {

  //--------------------------------------------------------------------------
  //   CONSTRUCTORS
  //--------------------------------------------------------------------------

  DirectBSplineSubSampler::DirectBSplineSubSampler( unsigned r,
                                                       bool normalize,
                                                       unsigned n ):
    ConvolutionSubSampler<DiscreteBSpline>(r),
    _normalize(normalize)
  {
    setFunctions( Point4du( r, r, r, r ), n );
  }

  DirectBSplineSubSampler::DirectBSplineSubSampler( const Point4du & r,
                                                       bool normalize,
                                                       unsigned n ):
    ConvolutionSubSampler<DiscreteBSpline>(r),
    _normalize(normalize)
  {
    setFunctions( r, n );
  }

  DirectBSplineSubSampler::~DirectBSplineSubSampler()
  {}

  DirectBSplineSubSampler::DirectBSplineSubSampler( const DirectBSplineSubSampler & other ):
    ConvolutionSubSampler<DiscreteBSpline>( other ),
    _normalize( other._normalize )
  {}

  DirectBSplineSubSampler &
  DirectBSplineSubSampler::operator=( const DirectBSplineSubSampler & other )
  {
    if( this != &other )
    {
      ConvolutionSubSampler<DiscreteBSpline>::operator=( other );
      _normalize = other._normalize;
    }
    return *this;
  }

  //--------------------------------------------------------------------------
  //   PARAMETERS
  //--------------------------------------------------------------------------

  unsigned DirectBSplineSubSampler::order() const
  {
    return this->_func[0].order();
  }

  bool DirectBSplineSubSampler::normalize() const
  {
    return _normalize;
  }

  void DirectBSplineSubSampler::setOrder( unsigned n )
  {
    for( std::vector<DiscreteBSpline>::iterator f = this->_func.begin();
         f != this->_func.end(); ++f )
      f->setOrder( n );
  }

  void DirectBSplineSubSampler::setNormalize( bool normalize )
  {
    _normalize = normalize;
  }

  void DirectBSplineSubSampler::setFactor( unsigned r )
  {
    ConvolutionSubSampler<DiscreteBSpline>::setFactor(r);
    for( std::vector<DiscreteBSpline>::iterator f = this->_func.begin();
         f != this->_func.end(); ++f )
      f->setScale( (float)r );
  }

  void DirectBSplineSubSampler::setFactor( const Point4du & r )
  {
    ConvolutionSubSampler<DiscreteBSpline>::setFactor(r);
    int i = 0;
    for( std::vector<DiscreteBSpline>::iterator f = this->_func.begin();
         f != this->_func.end(); ++f, ++i )
      f->setScale( (float)r[i] );
  }

  //--------------------------------------------------------------------------
  //   HELPER
  //--------------------------------------------------------------------------

  void DirectBSplineSubSampler::setFunctions( const Point4du & r, unsigned n )
  {
    std::vector<DiscreteBSpline> f;
    f.reserve( 4 );
    f.push_back( DiscreteBSpline( n, (float)r[0] ) );
    f.push_back( DiscreteBSpline( n, (float)r[1] ) );
    f.push_back( DiscreteBSpline( n, (float)r[2] ) );
    f.push_back( DiscreteBSpline( n, (float)r[3] ) );
    ConvolutionSubSampler<DiscreteBSpline>::setBasisFunction( f );
  }

  //--------------------------------------------------------------------------
  // Execution
  //--------------------------------------------------------------------------

  template <typename OUT, typename IN>
  carto::VolumeRef<OUT> DirectBSplineSubSampler::execute(
    const carto::VolumeRef<IN> & in, carto::VolumeRef<OUT> & out ) const
  {
    ConvolutionSubSampler<DiscreteBSpline>::execute( in, out );
    std::vector<int> size = in.getSize();
    double div = 1;
    for( std::size_t i = 0; i < 4; ++i )
      if( this->_dir[i] && size[i] > 1 )
        div *= ( this->_func.size() > i ? this->_func[i].scale()
                                        : this->_func[0].scale() );
    out /= div;
    return out;
  }

} // namespace aims

#endif // AIMS_PYRAMID_SPLINESUBSAMPLER_D_H
