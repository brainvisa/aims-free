
#ifndef AIMS_SIGNALFILTER_SDFILTER_H
#define AIMS_SIGNALFILTER_SDFILTER_H

#include <aims/signalfilter/filteringimagealgorithm.h>
#include <aims/signalfilter/filteringfunction_element.h>
#include <aims/connectivity/structuring_element.h>

namespace aims {

  template <typename T>
  class StDevFilter: public ElementFilteringImageAlgorithm<T>
  {
  public:
    typedef typename carto::DataTypeTraits<T>::ChannelType ChannelType;
    StDevFilter( const StructuringElement & se = strel::Cube(1.0),
                    const carto::Object & options = carto::none() ):
      ElementFilteringImageAlgorithm<T>( StDevFilterFunc<ChannelType>( options ),
                                      se )
    {}
    StDevFilter( const carto::Object & options ):
      ElementFilteringImageAlgorithm<T>( StDevFilterFunc<ChannelType>( options ),
                                      strel::Cube(1.0) )
    {}
    StDevFilter( const StDevFilter<T> & other ):
      ElementFilteringImageAlgorithm<T>( other )
    {}
    ~StDevFilter() {}
    StDevFilter<T> & operator=( const StDevFilter<T> & other )
    {
      ElementFilteringImageAlgorithm<T>::operator=( (ElementFilteringImageAlgorithm<T>&)other );
      return *this;
    }
    StDevFilter<T> *clone() const { return new StDevFilter<T>(*this); }
  };

} // namespace aims

#endif
