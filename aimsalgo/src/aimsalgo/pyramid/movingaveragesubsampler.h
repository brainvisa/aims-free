#ifndef AIMS_PYRAMID_MOVINGAVERAGESUBSAMPLER_H
#define AIMS_PYRAMID_MOVINGAVERAGESUBSAMPLER_H

//============================================================================
//
//                       MOVING AVERAGE SUBSAMPLER
//
//============================================================================

//--- aims -------------------------------------------------------------------
#include <aims/signalfilter/movingaveragefilter.h>
#include <aims/pyramid/convolutionsubsampler.h>     // aims::ConvolutionFilter
#include <aims/vector/vector.h>                                    // Point4df
//--- cartodata --------------------------------------------------------------
#include <cartodata/volume/volume.h>                       // carto::VolumeRef
//----------------------------------------------------------------------------

namespace aims {

  class MovingAverageSubSampler: public ConvolutionSubSampler<MovingAverageKernel>
  {
  public:

    //------------------------------------------------------------------------
    /// Constructor / Copy
    //------------------------------------------------------------------------
    /// @{
    MovingAverageSubSampler( unsigned r = 2, unsigned length = 3,
                             FilterType::Direction symtype = FilterType::Symmetrical );
    MovingAverageSubSampler( const Point4du & r, unsigned length = 3,
                             FilterType::Direction symtype = FilterType::Symmetrical );
    MovingAverageSubSampler( const Point4du & r, const Point4du & length,
                             FilterType::Direction symtype = FilterType::Symmetrical );
    MovingAverageSubSampler( const MovingAverageSubSampler & other );
    virtual ~MovingAverageSubSampler();
    MovingAverageSubSampler & operator=( const MovingAverageSubSampler & other );
    /// @}

    //------------------------------------------------------------------------
    /// Parameters
    //------------------------------------------------------------------------
    /// @{
    Point4du length() const;
    FilterType::Direction type() const;
    /// Updates underlying basis functions
    void setLength( unsigned length );
    /// Updates underlying basis functions
    void setLength( const Point4du & length );
    /// Updates underlying basis functions
    void setType( FilterType::Direction symtype );
    /// @{

  protected:
    // Make some inherited functions private
    using ConvolutionSubSampler<MovingAverageKernel>::setBasisFunction;
  };

} // namespace aims

//============================================================================
//
//                   MOVING AVERAGE CASCADE SUBSAMPLER
//
//============================================================================

//--- aims -------------------------------------------------------------------
#include <aims/signalfilter/movingaveragefilter.h>
#include <aims/pyramid/convolutionsubsampler.h>     // aims::ConvolutionFilter
#include <aims/vector/vector.h>                                    // Point4df
//--- cartodata --------------------------------------------------------------
#include <cartodata/volume/volume.h>                       // carto::VolumeRef
//----------------------------------------------------------------------------

namespace aims {

  class MovingAverageCascadeSubSampler: public ConvolutionSubSampler<MovingAverageCascadeKernel>
  {
  public:
    //------------------------------------------------------------------------
    /// Constructor / Copy
    //------------------------------------------------------------------------
    /// @{
    MovingAverageCascadeSubSampler( unsigned r = 2, unsigned length = 3, unsigned n = 2,
                                    FilterType::Direction symtype = FilterType::Symmetrical );
    MovingAverageCascadeSubSampler( const Point4du & r, unsigned length = 3, unsigned n = 2,
                                    FilterType::Direction symtype = FilterType::Symmetrical );
    MovingAverageCascadeSubSampler( const Point4du & r, const Point4du & length, unsigned n = 2,
                                    FilterType::Direction symtype = FilterType::Symmetrical );
    MovingAverageCascadeSubSampler( const Point4du & r, const Point4du & length, const Point4du & n,
                                    FilterType::Direction symtype = FilterType::Symmetrical );
    MovingAverageCascadeSubSampler( const MovingAverageCascadeSubSampler & other );
    virtual ~MovingAverageCascadeSubSampler();
    MovingAverageCascadeSubSampler & operator=( const MovingAverageCascadeSubSampler & other );
    /// @}

    //------------------------------------------------------------------------
    /// Parameters
    //------------------------------------------------------------------------
    /// @{
    Point4du length() const;
    Point4du cascade() const;
    FilterType::Direction type() const;
    /// Updates underlying basis functions
    void setLength( unsigned length );
    /// Updates underlying basis functions
    void setLength( const Point4du & length );
    /// Updates underlying basis functions
    void setCascade( unsigned length );
    /// Updates underlying basis functions
    void setCascade( const Point4du & length );
    /// Updates underlying basis functions
    void setType( FilterType::Direction symtype );
    /// @{

  protected:
    // Make some inherited functions private
    using ConvolutionSubSampler<MovingAverageCascadeKernel>::setBasisFunction;
  };

} // namespace aims

#endif // AIMS_PYRAMID_MOVINGAVERAGESUBSAMPLER_H
