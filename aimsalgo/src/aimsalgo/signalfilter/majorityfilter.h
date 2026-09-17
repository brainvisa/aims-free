
#ifndef AIMS_SIGNALFILTER_MAJORITYFILTER_H
#define AIMS_SIGNALFILTER_MAJORITYFILTER_H

#include <aims/signalfilter/filteringimagealgorithm.h>
#include <aims/signalfilter/filteringfunction_element.h>
#include <aims/connectivity/structuring_element.h>

namespace aims
{

  template <typename T>
  class MajorityFilter: public ElementFilteringImageAlgorithm<T>
  {
  public:
    typedef typename carto::DataTypeTraits<T>::ChannelType ChannelType;
    MajorityFilter( const StructuringElement & se = strel::Cube(1.0),
                    const carto::Object & options = carto::none() ):
      ElementFilteringImageAlgorithm<T>( MajorityFilterFunc<ChannelType>( options ),
                                      se )
    {}
    MajorityFilter( const carto::Object & options ):
      ElementFilteringImageAlgorithm<T>( MajorityFilterFunc<ChannelType>( options ),
                                      strel::Cube(1.0) )
    {}
    MajorityFilter( const MajorityFilter<T> & other ):
      ElementFilteringImageAlgorithm<T>( other )
    {}
    ~MajorityFilter() {}
    MajorityFilter<T> & operator=( const MajorityFilter<T> & other )
    {
      ElementFilteringImageAlgorithm<T>::operator=( (ElementFilteringImageAlgorithm<T>&)other );
      return *this;
    }
    MajorityFilter<T> *clone() const { return new MajorityFilter<T>(*this); }
  };

  template <typename T>
  class NotNullMajorityFilter: public ElementFilteringImageAlgorithm<T>
  {
  public:
    typedef typename carto::DataTypeTraits<T>::ChannelType ChannelType;
    NotNullMajorityFilter( const StructuringElement & se = strel::Cube(1.0),
                           const carto::Object & options = carto::none() ):
      ElementFilteringImageAlgorithm<T>( NotNullMajorityFilterFunc<ChannelType>( options ),
                                      se )
    {}
    NotNullMajorityFilter( const carto::Object & options ):
      ElementFilteringImageAlgorithm<T>( NotNullMajorityFilterFunc<ChannelType>( options ),
                                      strel::Cube(1.0) )
    {}
    NotNullMajorityFilter( const NotNullMajorityFilter<T> & other ):
      ElementFilteringImageAlgorithm<T>( other )
    {}
    ~NotNullMajorityFilter() {}
    NotNullMajorityFilter<T> & operator=( const NotNullMajorityFilter<T> & other )
    {
      ElementFilteringImageAlgorithm<T>::operator=( (ElementFilteringImageAlgorithm<T>&)other );
      return *this;
    }
    NotNullMajorityFilter<T> *clone() const { return new NotNullMajorityFilter<T>(*this); }
  };
} // namespace aims

#endif
