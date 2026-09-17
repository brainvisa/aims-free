
#ifndef AIMS_SIGNALFILTER_FILTERINGFUNCTION_ELEMENT_H
#define AIMS_SIGNALFILTER_FILTERINGFUNCTION_ELEMENT_H

//--- aims -------------------------------------------------------------------
#include <aims/signalfilter/filteringfunction.h>    // aims::FilteringFunction
//--- std --------------------------------------------------------------------
#include <set>
#include <string>
#include <map>
//--- forward declarations ---------------------------------------------------
namespace carto {
  template <typename T> class VolumeRef;
  template <typename T> class rc_ptr;
  class Object;
  Object none();
}
namespace aims {
  class StructuringElement;
}
//----------------------------------------------------------------------------

namespace aims {

  //==========================================================================
  // ELEMENT FILTERING FUNCTON
  //==========================================================================
  /// \brief Base class for filtering functions applied in a structuring
  /// element.
  /// \sa MedianFilterFunc, MeanFilterFunc
  template <typename T>
  class ElementFilteringFunction: public FilteringFunctionInterface<T>
  {
    public:
      virtual ~ElementFilteringFunction() {};
      //----------------------------------------------------------------------
      // interface
      //----------------------------------------------------------------------
      /// \param in  window volume of size 1 voxel with enough border so that
      /// the structuring element can be used.
      virtual T execute( const carto::VolumeRef<T> & in,
                         const carto::rc_ptr<StructuringElement> & se ) const = 0;
      /// \param in  window volume of any given size. It is used as a cubic
      /// structuring element.
      virtual T execute( const carto::VolumeRef<T> & in ) const = 0;
      /// clone
      virtual ElementFilteringFunction<T> *clone() const = 0;

    protected:
      //----------------------------------------------------------------------
      //   magic 3
      //----------------------------------------------------------------------
      ElementFilteringFunction() {}
      ElementFilteringFunction( const ElementFilteringFunction<T> & );
      ElementFilteringFunction<T> & operator= ( const ElementFilteringFunction<T> & );
  };

  //==========================================================================
  // ELEMENT FILTERING FUNCTIONS: FACTORY
  //==========================================================================
  /// \brief Associates a string with a ElementFilteringFunction
  template <typename T>
  class ElementFilteringFunctionFactory
  {
    public:
      /// Build and return the ElementFilteringFunction associated with
      /// the id \c name.
      static ElementFilteringFunction<T> * create( const std::string & name,
                                                   carto::Object options = carto::none() );
      /// Register a ElementFilterigFunction into the factory
      static void registerFunction( const std::string & name,
                                    const ElementFilteringFunction<T> & func );
      /// List all function ids registered in the factory
      static std::set<std::string> names();
    protected:
      static void init();
      static std::map<std::string,carto::rc_ptr<ElementFilteringFunction<T> > > & _map();
  };

} // namespace aims


//============================================================================
// ELEMENT FILTERING FUNCTIONS: DERIVED CLASSES
//============================================================================

#define AIMS_DECLARE_ELEMENTFILTERFUNC_CUSTOMIZED( NAME )                    \
  template <typename T>                                                      \
  class NAME: public ElementFilteringFunction<T>                             \
  {                                                                          \
    public:                                                                  \
      NAME( carto::Object options = carto::none() )                          \
        { setOptions( options ); }                                           \
      NAME( const NAME<T> & other );                                         \
      virtual ~NAME();                                                       \
      NAME<T> & operator=( const NAME<T> & other );                          \
      virtual NAME<T> *clone() const { return new NAME<T>(*this); }          \
      virtual void setOptions( const carto::Object & options );              \
      virtual T execute( const carto::VolumeRef<T> & in ) const;             \
      virtual T execute( const carto::VolumeRef<T> & in,                     \
                         const carto::rc_ptr<StructuringElement> & se ) const;

#define AIMS_DEFINE_ELEMENTFILTERFUNC_BASIC( NAME )                          \
  template <typename T>                                                      \
  NAME<T>::NAME( const NAME<T> & /*other*/ ) : ElementFilteringFunction<T>() {} \
  template <typename T>                                                      \
  NAME<T>::~NAME() {}                                                        \
  template <typename T>                                                      \
  NAME<T> & NAME<T>::operator=( const NAME<T> & /*other*/ ) { return *this; } \
  template <typename T>                                                      \
  void NAME<T>::setOptions( const carto::Object & /*options*/ ) {}

#define AIMS_DECLARE_ELEMENTFILTERFUNC_BASIC( NAME )                         \
  AIMS_DECLARE_ELEMENTFILTERFUNC_CUSTOMIZED(NAME)                            \
  };                                                                         \
  AIMS_DEFINE_ELEMENTFILTERFUNC_BASIC(NAME)


namespace aims {
  AIMS_DECLARE_ELEMENTFILTERFUNC_BASIC(MinFilterFunc)
  AIMS_DECLARE_ELEMENTFILTERFUNC_BASIC(MaxFilterFunc)
  AIMS_DECLARE_ELEMENTFILTERFUNC_BASIC(MajorityFilterFunc)
  AIMS_DECLARE_ELEMENTFILTERFUNC_BASIC(ExtremaDifferenceFilterFunc)
  AIMS_DECLARE_ELEMENTFILTERFUNC_BASIC(SumFilterFunc)
  AIMS_DECLARE_ELEMENTFILTERFUNC_BASIC(VarFilterFunc)
  AIMS_DECLARE_ELEMENTFILTERFUNC_BASIC(StDevFilterFunc)
  AIMS_DECLARE_ELEMENTFILTERFUNC_BASIC(MedianFilterFunc)
  AIMS_DECLARE_ELEMENTFILTERFUNC_BASIC(MeanFilterFunc)
  AIMS_DECLARE_ELEMENTFILTERFUNC_BASIC(NotNullMeanFilterFunc)
  AIMS_DECLARE_ELEMENTFILTERFUNC_BASIC(NotNullMedianFilterFunc)
  AIMS_DECLARE_ELEMENTFILTERFUNC_BASIC(NotNullMajorityFilterFunc)
} // namespace aims

#endif
