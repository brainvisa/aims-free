#ifndef AIMS_SIGNALFILTER_ELEMENTFILTERFACTORY_H
#define AIMS_SIGNALFILTER_ELEMENTFILTERFACTORY_H

//--- aims -------------------------------------------------------------------
#include <aims/connectivity/structuring_element.h>
#include <aims/signalfilter/filteringimagealgorithm.h>
//--- cartobase --------------------------------------------------------------
#include <cartobase/object/object.h>
#include <cartobase/smart/rcptr.h>
//--- aims -------------------------------------------------------------------
#include <map>
#include <set>
#include <string>
//----------------------------------------------------------------------------

namespace aims {

  //==========================================================================
  // ELEMENT FILTERING ALGORITHM: FACTORY
  //==========================================================================
  /// Factory class for non linear filtering image algorithms
  /// Here are the registered algorithms:
  /// \code
  /// registerFilter( "min", MinFilter<T>() );
  /// registerFilter( "max", MaxFilter<T>() );
  /// registerFilter( "med", MedianFilter<T>() );
  /// registerFilter( "median", MedianFilter<T>() );
  /// registerFilter( "notnullmedian", NotNullMedianFilter<T>() );
  /// registerFilter( "mea", MeanFilter<T>() );
  /// registerFilter( "mean", MeanFilter<T>() );
  /// registerFilter( "notnullmean", NotNullMeanFilter<T>() );
  /// registerFilter( "maj", MajorityFilter<T>() );
  /// registerFilter( "majority", MajorityFilter<T>() );
  /// registerFilter( "dif", ExtremaDifferenceFilter<T>() );
  /// registerFilter( "difference", ExtremaDifferenceFilter<T>() );
  /// registerFilter( "sum", SumFilter<T>() );
  /// registerFilter( "var", VarFilter<T>() );
  /// registerFilter( "variance", VarFilter<T>() );
  /// registerFilter( "sd", StDevFilter<T>() );
  /// \code
  template <typename T>
  class ElementFilterFactory
  {
    public:
      static ElementFilteringImageAlgorithm<T> * create( const std::string & name,
                                                         const StructuringElement & strel,
                                                         carto::Object options = carto::none() );
      static ElementFilteringImageAlgorithm<T> * create( const std::string & name,
                                                         carto::Object options = carto::none() );
      static void registerFilter( const std::string & name,
                                  const ElementFilteringImageAlgorithm<T> & func );
      static std::set<std::string> names();
    protected:
      static void init();
      static std::map<std::string,carto::rc_ptr<ElementFilteringImageAlgorithm<T> > > & _map();
  };

} // namespace aims

#endif
