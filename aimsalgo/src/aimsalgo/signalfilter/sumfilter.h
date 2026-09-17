
#ifndef AIMS_SIGNALFILTER_SUMFILTER_H
#define AIMS_SIGNALFILTER_SUMFILTER_H

#include <aims/signalfilter/filteringimagealgorithm.h>
#include <aims/signalfilter/filteringfunction_element.h>
#include <aims/connectivity/structuring_element.h>

namespace aims {

  template <typename T>
  class SumFilter: public ElementFilteringImageAlgorithm<T>
  {
  public:
    typedef typename carto::DataTypeTraits<T>::ChannelType ChannelType;
    SumFilter( const StructuringElement & se = strel::Cube(1.0),
                    const carto::Object & options = carto::none() ):
      ElementFilteringImageAlgorithm<T>( SumFilterFunc<ChannelType>( options ),
                                      se )
    {}
    SumFilter( const carto::Object & options ):
      ElementFilteringImageAlgorithm<T>( SumFilterFunc<ChannelType>( options ),
                                      strel::Cube(1.0) )
    {}
    SumFilter( const SumFilter<T> & other ):
      ElementFilteringImageAlgorithm<T>( other )
    {}
    ~SumFilter() {};
    SumFilter<T> & operator=( const SumFilter<T> & other )
    {
      ElementFilteringImageAlgorithm<T>::operator=( (ElementFilteringImageAlgorithm<T>&)other );
      return *this;
    }
    SumFilter<T> *clone() const { return new SumFilter<T>(*this); }
  };

} // namespace aims

#endif
