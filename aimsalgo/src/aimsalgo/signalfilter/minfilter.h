
#ifndef AIMS_SIGNALFILTER_MINFILTER_H
#define AIMS_SIGNALFILTER_MINFILTER_H

#include <aims/signalfilter/filteringimagealgorithm.h>
#include <aims/signalfilter/filteringfunction_element.h>
#include <aims/connectivity/structuring_element.h>

namespace aims
{

  template <typename T>
  class MinFilter: public ElementFilteringImageAlgorithm<T>
  {
  public:
    typedef typename carto::DataTypeTraits<T>::ChannelType ChannelType;
    MinFilter( const StructuringElement & se = strel::Cube(1.0),
                    const carto::Object & options = carto::none() ):
      ElementFilteringImageAlgorithm<T>( MinFilterFunc<ChannelType>( options ),
                                      se )
    {}
    MinFilter( const carto::Object & options ):
      ElementFilteringImageAlgorithm<T>( MinFilterFunc<ChannelType>( options ),
                                      strel::Cube(1.0) )
    {}
    MinFilter( const MinFilter<T> & other ):
      ElementFilteringImageAlgorithm<T>( other )
    {}
    ~MinFilter() {}
    MinFilter<T> & operator=( const MinFilter<T> & other )
    {
      ElementFilteringImageAlgorithm<T>::operator=( (ElementFilteringImageAlgorithm<T>&)other );
      return *this;
    }
    MinFilter<T> *clone() const { return new MinFilter<T>(*this); }
  };

} // namespace aims

#endif
