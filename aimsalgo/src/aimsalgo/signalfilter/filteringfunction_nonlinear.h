
#ifndef AIMS_SIGNALFILTER_FILTERINGFUNCTION_NONLINEAR_H
#define AIMS_SIGNALFILTER_FILTERINGFUNCTION_NONLINEAR_H

//--- aims -------------------------------------------------------------------
#include <aims/signalfilter/filteringfunction.h>    // aims::FilteringFunction
//--- std --------------------------------------------------------------------
#include <set>
#include <string>
#include <map>
//--- forward declarations ---------------------------------------------------
namespace carto {
  template <typename T> class VolumeRef;
  class Object;
  Object none();
}
namespace aims {
  class StructuringElementRef;
  namespace strel {
    StructuringElementRef none();
  }
}
//----------------------------------------------------------------------------

namespace aims {

  //==========================================================================
  // NON LINEAR FILTERING FUNCTIONS: INTERFACE
  //==========================================================================
  /// Pure virtual class: interface for non linear filtering functions.
  template <typename T>
  class NonLinFilterFunc: public FilteringFunction<T>
  {
    public:
      virtual ~NonLinFilterFunc() {};
      virtual bool isLinear() const { return false; }
      virtual NonLinFilterFunc<T> * clone() const = 0;
  };

  //==========================================================================
  // NON LINEAR FILTERING FUNCTIONS: FACTORY
  //==========================================================================
  template <typename T>
  class NonLinFilterFuncFactory
  {
    public:
      static NonLinFilterFunc<T> * create( const std::string & name,
                                        carto::Object options = carto::none() );
      static void registerFunction( const std::string & name,
                                    const NonLinFilterFunc<T> & func );
      static std::set<std::string> names();
    protected:
      static void init();
      static std::map<std::string,carto::rc_ptr<NonLinFilterFunc<T> > > & _map();
  };

} // namespace aims


//============================================================================
// NON LINEAR FILTERING FUNCTIONS: DERIVED CLASSES
//============================================================================
#define AIMS_DECLARE_NONLINFILTERFUNC_BASIC( NAME )                          \
  template <typename T>                                                      \
  class NAME: public NonLinFilterFunc<T>                                     \
  {                                                                          \
    public:                                                                  \
      NAME( carto::Object options = carto::none() )                          \
        { setOptions( options ); }                                           \
      virtual ~NAME() {};                                                    \
      virtual void setOptions( carto::Object ) {}                            \
      virtual T execute( const carto::VolumeRef<T> & volume ) ;              \
      virtual T execute( const carto::VolumeRef<T> & volume,                 \
                         const StructuringElementRef & se );                 \
      virtual NAME<T> * clone() const { return new NAME<T>(); }              \
  }

namespace aims {
  AIMS_DECLARE_NONLINFILTERFUNC_BASIC(MinFilterFunc);
  AIMS_DECLARE_NONLINFILTERFUNC_BASIC(MaxFilterFunc);
  AIMS_DECLARE_NONLINFILTERFUNC_BASIC(MedianFilterFunc);
  AIMS_DECLARE_NONLINFILTERFUNC_BASIC(NotNullMedianFilterFunc);
  AIMS_DECLARE_NONLINFILTERFUNC_BASIC(MeanFilterFunc);
  AIMS_DECLARE_NONLINFILTERFUNC_BASIC(NotNullMeanFilterFunc);
  AIMS_DECLARE_NONLINFILTERFUNC_BASIC(MajorityFilterFunc);
  AIMS_DECLARE_NONLINFILTERFUNC_BASIC(NotNullMajorityFilterFunc);
  AIMS_DECLARE_NONLINFILTERFUNC_BASIC(ExtremaDifferenceFilterFunc);
  AIMS_DECLARE_NONLINFILTERFUNC_BASIC(SumFilterFunc);
  AIMS_DECLARE_NONLINFILTERFUNC_BASIC(VarFilterFunc);
  AIMS_DECLARE_NONLINFILTERFUNC_BASIC(StDevFilterFunc);
} // namespace aims

#endif
