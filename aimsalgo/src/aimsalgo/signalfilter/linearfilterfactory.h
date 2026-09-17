#ifndef AIMS_SIGNALFILTER_LINEARFILTERFACTORY_H
#define AIMS_SIGNALFILTER_LINEARFILTERFACTORY_H

//--- aims -------------------------------------------------------------------
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
  // LINEAR FILTERING ALGORITHM: FACTORY
  //==========================================================================
  /// Factory class for linear filtering image algorithms
  /// Here are the registered algorithms:
  /// \code
  /// registerFilter( "gab", GaborFilter<T>() );
  /// registerFilter( "gabor", GaborFilter<T>() );
  /// \code
  template <typename T>
  class LinearFilterFactory
  {
    public:
      static LinearFilteringImageAlgorithm<T> * create( const std::string & name,
                                                        carto::Object options = carto::none() );
      static void registerFilter( const std::string & name,
                                  const LinearFilteringImageAlgorithm<T> & func );
      static std::set<std::string> names();
    protected:
      static void init();
      static std::map<std::string,carto::rc_ptr<LinearFilteringImageAlgorithm<T> > > & _map();
  };

}

#endif
