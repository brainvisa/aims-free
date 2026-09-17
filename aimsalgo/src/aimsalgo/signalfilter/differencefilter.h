
#ifndef AIMS_SIGNALFILTER_DIFFERENCEFILTER_H
#define AIMS_SIGNALFILTER_DIFFERENCEFILTER_H

#include <aims/signalfilter/filteringimagealgorithm.h>
#include <aims/signalfilter/filteringfunction_element.h>
#include <aims/connectivity/structuring_element.h>

namespace aims {

  template <typename T>
  class ExtremaDifferenceFilter: public ElementFilteringImageAlgorithm<T>
  {
  public:
    typedef typename carto::DataTypeTraits<T>::ChannelType ChannelType;
    ExtremaDifferenceFilter( const StructuringElement & se = strel::Cube(1.0),
                    const carto::Object & options = carto::none() ):
      ElementFilteringImageAlgorithm<T>( ExtremaDifferenceFilterFunc<ChannelType>( options ),
                                      se )
    {}
    ExtremaDifferenceFilter( const carto::Object & options ):
      ElementFilteringImageAlgorithm<T>( ExtremaDifferenceFilterFunc<ChannelType>( options ),
                                      strel::Cube(1.0) )
    {}
    ExtremaDifferenceFilter( const ExtremaDifferenceFilter<T> & other ):
      ElementFilteringImageAlgorithm<T>( other )
    {}
    ~ExtremaDifferenceFilter() {}
    ExtremaDifferenceFilter<T> & operator=( const ExtremaDifferenceFilter<T> & other )
    {
      ElementFilteringImageAlgorithm<T>::operator=( (ElementFilteringImageAlgorithm<T>&)other );
      return *this;
    }
    ExtremaDifferenceFilter<T> *clone() const { return new ExtremaDifferenceFilter<T>(*this); }
  };

} // namespace aims

#endif
