
#ifndef AIMS_SIGNALFILTER_VARIANCEFILTER_H
#define AIMS_SIGNALFILTER_VARIANCEFILTER_H

#include <aims/signalfilter/filteringimagealgorithm.h>
#include <aims/signalfilter/filteringfunction_element.h>
#include <aims/connectivity/structuring_element.h>

namespace aims {

  template <typename T>
  class VarFilter: public ElementFilteringImageAlgorithm<T>
  {
  public:
    typedef typename carto::DataTypeTraits<T>::ChannelType ChannelType;
    VarFilter( const StructuringElement & se = strel::Cube(1.0),
                    const carto::Object & options = carto::none() ):
      ElementFilteringImageAlgorithm<T>( VarFilterFunc<ChannelType>( options ),
                                      se )
    {}
    VarFilter( const carto::Object & options ):
      ElementFilteringImageAlgorithm<T>( VarFilterFunc<ChannelType>( options ),
                                      strel::Cube(1.0) )
    {}
    VarFilter( const VarFilter<T> & other ):
      ElementFilteringImageAlgorithm<T>( other )
    {}
    ~VarFilter() {}
    VarFilter<T> & operator=( const VarFilter<T> & other )
    {
      ElementFilteringImageAlgorithm<T>::operator=( (ElementFilteringImageAlgorithm<T>&)other );
      return *this;
    }
    VarFilter<T> *clone() const { return new VarFilter<T>(*this); }
  };

} // namespace aims

#endif
