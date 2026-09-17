#ifndef AIMS_SIGNALFILTER_FILTERFUNCTION_H
#define AIMS_SIGNALFILTER_FILTERFUNCTION_H

//--- cartobase --------------------------------------------------------------
#include <cartobase/object/object.h>                          // carto::Object
//--- std --------------------------------------------------------------------
#include <vector>
//--- forward declarations ---------------------------------------------------
namespace carto {
  template <typename T> class VolumeRef;
}
//----------------------------------------------------------------------------

namespace aims {

  //==========================================================================
  // FILTERING FUNCTON INTERFACE
  //==========================================================================
  /// \brief Pure virtual class: interface for filtering functions
  ///        called by aims::FilteringImageAlgorithm and
  ///        aims::SubSamplingImageAlgorithm
  template <typename T>
  class FilteringFunctionInterface
  {
    public:
      virtual ~FilteringFunctionInterface() {};
      //----------------------------------------------------------------------
      // interface
      //----------------------------------------------------------------------
      /// Basic execute method that all derived class must implement (in
      /// the case where more parameters are needed, this methods runs
      /// with their default value)
      virtual T execute( const carto::VolumeRef<T> & in ) const = 0;
      /// Set the parameters of the filters
      /// If a parameter value is not set in the options object, a default
      /// value must be assigned.
      /// This method may do nothing if no options are available.
      virtual void setOptions( const carto::Object & /*options*/ ) {}
      /// Update the parameters of the filters
      /// If a parameter value is not set in the options object, the current
      /// value must be kept.
      /// This method may do nothing if no options are available.
      virtual void updateOptions( const carto::Object & /*options*/ ) {}
      /// clone method. It is better to implement it using a copy constructor.
      virtual FilteringFunctionInterface *clone() const = 0;

    protected:
      //----------------------------------------------------------------------
      //   magic 3
      //----------------------------------------------------------------------
      FilteringFunctionInterface() {}
      FilteringFunctionInterface( const FilteringFunctionInterface<T> & );
      FilteringFunctionInterface<T> & operator= ( const FilteringFunctionInterface<T> & );
  };

} // namespace aims

#endif
