
#ifndef AIMS_SIGNALFILTER_MAXFILTER_H
#define AIMS_SIGNALFILTER_MAXFILTER_H

#include <aims/signalfilter/filteringimagealgorithm.h>
#include <aims/signalfilter/filteringfunction_element.h>
#include <aims/connectivity/structuring_element.h>

namespace aims
{

  template <typename T>
  class MaxFilter: public ElementFilteringImageAlgorithm<T>
  {
  public:
    typedef typename carto::DataTypeTraits<T>::ChannelType ChannelType;
    MaxFilter( const StructuringElement & se = strel::Cube(1.0),
                    const carto::Object & options = carto::none() ):
      ElementFilteringImageAlgorithm<T>( MaxFilterFunc<ChannelType>( options ),
                                         se )
    {}
    MaxFilter( const carto::Object & options ):
      ElementFilteringImageAlgorithm<T>( MaxFilterFunc<ChannelType>( options ),
                                         strel::Cube(1.0) )
    {}
    MaxFilter( const MaxFilter<T> & other ):
      ElementFilteringImageAlgorithm<T>( other )
    {}
    ~MaxFilter() {}
    MaxFilter<T> & operator=( const MaxFilter<T> & other )
    {
      ElementFilteringImageAlgorithm<T>::operator=( (ElementFilteringImageAlgorithm<T>&)other );
      return *this;
    }
    MaxFilter<T> *clone() const { return new MaxFilter<T>(*this); }
  };

} // namespace aims

#endif
