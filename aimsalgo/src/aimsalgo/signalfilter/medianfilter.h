
#ifndef AIMS_SIGNALFILTER_MEDIANFILTER_H
#define AIMS_SIGNALFILTER_MEDIANFILTER_H

#include <aims/signalfilter/filteringimagealgorithm.h>
#include <aims/signalfilter/filteringfunction_element.h>
#include <aims/connectivity/structuring_element.h>

namespace aims {

  template <typename T>
  class MedianFilter: public ElementFilteringImageAlgorithm<T>
  {
  public:
    typedef typename carto::DataTypeTraits<T>::ChannelType ChannelType;
    MedianFilter( const StructuringElement & se = strel::Cube(1.0),
                    const carto::Object & options = carto::none() ):
      ElementFilteringImageAlgorithm<T>( MedianFilterFunc<ChannelType>( options ),
                                      se )
    {}
    MedianFilter( const carto::Object & options ):
      ElementFilteringImageAlgorithm<T>( MedianFilterFunc<ChannelType>( options ),
                                      strel::Cube(1.0) )
    {}
    MedianFilter( const MedianFilter<T> & other ):
      ElementFilteringImageAlgorithm<T>( other )
    {}
    ~MedianFilter() {}
    MedianFilter<T> & operator=( const MedianFilter<T> & other )
    {
      ElementFilteringImageAlgorithm<T>::operator=( (ElementFilteringImageAlgorithm<T>&)other );
      return *this;
    }
    MedianFilter<T> *clone() const {
      return new MedianFilter<T>(*this); }
  };

  template <typename T>
  class NotNullMedianFilter: public ElementFilteringImageAlgorithm<T>
  {
  public:
    typedef typename carto::DataTypeTraits<T>::ChannelType ChannelType;
    NotNullMedianFilter( const StructuringElement & se = strel::Cube(1.0),
                    const carto::Object & options = carto::none() ):
      ElementFilteringImageAlgorithm<T>( NotNullMedianFilterFunc<ChannelType>( options ),
                                      se )
    {}
    NotNullMedianFilter( const carto::Object & options ):
      ElementFilteringImageAlgorithm<T>( NotNullMedianFilterFunc<ChannelType>( options ),
                                      strel::Cube(1.0) )
    {}
    NotNullMedianFilter<T>( const NotNullMedianFilter<T> & other ):
      ElementFilteringImageAlgorithm<T>( other )
    {}
    ~NotNullMedianFilter() {}
    NotNullMedianFilter<T> & operator=( const NotNullMedianFilter<T> & other )
    {
      ElementFilteringImageAlgorithm<T>::operator=( (ElementFilteringImageAlgorithm<T>&)other );
      return *this;
    }
    NotNullMedianFilter<T> *clone() const { return new NotNullMedianFilter<T>(*this); }
  };

} // namespace aims

//============================================================================
//   Backward compatibility bindings
//============================================================================

#include <vector>

template <typename T>
class MedianSmoothing
{
  public:
    MedianSmoothing( int sx = 3, int sy = 3, int sz = 3 );
    virtual ~MedianSmoothing();
    virtual carto::VolumeRef<T> doit( const carto::VolumeRef<T>& in ) const;
  private:
    MedianSmoothing<T> & operator = ( const MedianSmoothing<T> & );
    int _sx;
    int _sy;
    int _sz;
};

//----------------------------------------------------------------------------
//   DEFINITIONS
//----------------------------------------------------------------------------

template <typename T>
MedianSmoothing<T>::MedianSmoothing( int sx, int sy, int sz ):
  _sx(sx), _sy(sy), _sz(sz)
{}

template <typename T>
MedianSmoothing<T>::~MedianSmoothing()
{}

template <typename T>
MedianSmoothing<T> & MedianSmoothing<T>::operator= (
  const MedianSmoothing<T> & other
)
{
  _sx = other._sx;
  _sy = other._sy;
  _sz = other._sz;
  return (*this);
}

template <typename T>
carto::VolumeRef<T> MedianSmoothing<T>::doit( const carto::VolumeRef<T>& in ) const
{
  std::vector<double> amplitude(3,0.);
  amplitude[0] = .5 * (double)_sx;
  amplitude[1] = .5 * (double)_sy;
  amplitude[2] = .5 * (double)_sz;
  aims::strel::Cube se( amplitude, true );
  aims::MedianFilter<T> f( se );
  return f.execute( in );
}


#endif
