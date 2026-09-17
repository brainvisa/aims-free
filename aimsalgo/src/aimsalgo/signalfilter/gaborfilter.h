#ifndef AIMS_SIGNALFILTER_GABORFILTER_H
#define AIMS_SIGNALFILTER_GABORFILTER_H

#include <aims/signalfilter/filteringimagealgorithm.h>
#include <aims/signalfilter/filteringfunction_linear.h>

namespace aims {

  template <typename T>
  class GaborFilter: public LinearFilteringImageAlgorithm<T>
  {
  public:
    typedef typename carto::DataTypeTraits<T>::ChannelType ChannelType;
    GaborFilter( const carto::Object & options = carto::none() ):
      LinearFilteringImageAlgorithm<T>( GaborFilterFunc<ChannelType>( options ) )
    {}
    GaborFilter( const GaborFilter<T> & other ):
      LinearFilteringImageAlgorithm<T>( other )
    {}
    ~GaborFilter() {}
    GaborFilter<T> & operator=( const GaborFilter<T> & other )
    {
      LinearFilteringImageAlgorithm<T>::operator=( (LinearFilteringImageAlgorithm<T>&)other );
      return *this;
    }
    GaborFilter<T> *clone() const { return new GaborFilter<T>(*this); }
  };

}

#endif
