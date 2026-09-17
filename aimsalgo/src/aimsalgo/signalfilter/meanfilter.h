
#ifndef AIMS_SIGNALFILTER_MEANFILTER_H
#define AIMS_SIGNALFILTER_MEANFILTER_H

#include <aims/signalfilter/filteringimagealgorithm.h>
#include <aims/signalfilter/filteringfunction_element.h>
#include <aims/connectivity/structuring_element.h>

namespace aims
{

  template <typename T>
  class MeanFilter: public ElementFilteringImageAlgorithm<T>
  {
  public:
    typedef typename carto::DataTypeTraits<T>::ChannelType ChannelType;
    MeanFilter( const StructuringElement & se = strel::Cube(1.0),
                    const carto::Object & options = carto::none() ):
      ElementFilteringImageAlgorithm<T>( MeanFilterFunc<ChannelType>( options ),
                                      se )
    {}
    MeanFilter( const carto::Object & options ):
      ElementFilteringImageAlgorithm<T>( MeanFilterFunc<ChannelType>( options ),
                                      strel::Cube(1.0) )
    {}
    MeanFilter( const MeanFilter<T> & other ):
      ElementFilteringImageAlgorithm<T>( other )
    {}
    ~MeanFilter() {}
    MeanFilter<T> & operator=( const MeanFilter<T> & other )
    {
      ElementFilteringImageAlgorithm<T>::operator=( (ElementFilteringImageAlgorithm<T>&)other );
      return *this;
    }
    MeanFilter<T> *clone() const { return new MeanFilter<T>(*this); }
  };


  template <typename T>
  class NotNullMeanFilter: public ElementFilteringImageAlgorithm<T>
  {
  public:
    typedef typename carto::DataTypeTraits<T>::ChannelType ChannelType;
    NotNullMeanFilter( const StructuringElement & se = strel::Cube(1.0),
                    const carto::Object & options = carto::none() ):
      ElementFilteringImageAlgorithm<T>( NotNullMeanFilterFunc<ChannelType>( options ),
                                      se )
    {}
    NotNullMeanFilter( const carto::Object & options ):
      ElementFilteringImageAlgorithm<T>( NotNullMeanFilterFunc<ChannelType>( options ),
                                      strel::Cube(1.0) )
    {}
    NotNullMeanFilter( const NotNullMeanFilter<T> & other ):
      ElementFilteringImageAlgorithm<T>( other )
    {}
    ~NotNullMeanFilter() {}
    NotNullMeanFilter<T> & operator=( const NotNullMeanFilter<T> & other )
    {
      ElementFilteringImageAlgorithm<T>::operator=( (ElementFilteringImageAlgorithm<T>&)other );
      return *this;
    }
    NotNullMeanFilter<T> *clone() const { return new NotNullMeanFilter<T>(*this); }
  };

} // namespace aims


#endif
