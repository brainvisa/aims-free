
#ifndef AIMS_SIGNALFILTER_FILTERINGFUNCTION_LINEAR_H
#define AIMS_SIGNALFILTER_FILTERINGFUNCTION_LINEAR_H

//--- aims -------------------------------------------------------------------
#include <aims/signalfilter/filteringfunction.h>    // aims::FilteringFunction
//--- cartodata --------------------------------------------------------------
#include <cartodata/volume/volume.h>
//--- std --------------------------------------------------------------------
#include <set>
#include <string>
#include <map>
#include <vector>
//--- forward declarations ---------------------------------------------------
namespace carto {
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
  // LINEAR FILTERING FUNCTON
  //==========================================================================
  /// \brief Base class for linear filtering functions.
  /// They are executed on a window (unallocated carto::Volume with an
  /// allocated parent volume) of size 1 voxel with enough border so that
  /// the linear kernel may be applied.
  template <typename T>
  class LinearFilteringFunction: public FilteringFunctionInterface<T>
  {
    public:
      virtual ~LinearFilteringFunction() {};
      //----------------------------------------------------------------------
      // interface
      //----------------------------------------------------------------------
      /// \param in  window volume with the same size as the kernel.
      /// Use \c getAmplitude to know the needed dimensions
      virtual T execute( const carto::VolumeRef<T> & in ) const = 0;
      /// Return a vector fo size 6 describing the border needed in each
      /// direction (x-, x+, y-, y+, z-, z+). The kernel size is then of
      /// dimension: \c ( v[0] + v[1] + 1, v[2] + v[3] + 1, v[4] + v[5] + 1 )
      virtual const std::vector<int> & getAmplitude() const = 0;
      /// clone
      virtual LinearFilteringFunction<T> *clone() const = 0;

    protected:
      //----------------------------------------------------------------------
      //   magic 3
      //----------------------------------------------------------------------
      LinearFilteringFunction() {}
      LinearFilteringFunction( const LinearFilteringFunction<T> & );
      LinearFilteringFunction<T> & operator= ( const LinearFilteringFunction<T> & );
  };

  //==========================================================================
  // LINEAR FILTERING FUNCTIONS: FACTORY
  //==========================================================================
  template <typename T>
  class LinearFilteringFunctionFactory
  {
    public:
      static LinearFilteringFunction<T> * create( const std::string & name,
                                     carto::Object options = carto::none() );
      static void registerFunction( const std::string & name,
                                    const LinearFilteringFunction<T> & func );
      static std::set<std::string> names();
    protected:
      static void init();
      static std::map<std::string,carto::rc_ptr<LinearFilteringFunction<T> > > & _map();
  };

} // namespace aims


//============================================================================
// LINEAR FILTERING FUNCTIONS: DERIVED CLASSES
//============================================================================
#define AIMS_DECLARE_LINFILTERUNC_CUSTOMIZED( NAME )                         \
  template <typename T>                                                      \
  class NAME: public LinearFilteringFunction<T>                              \
  {                                                                          \
    public:                                                                  \
      NAME( carto::Object options = carto::none() )                          \
        { setOptions( options ); }                                           \
      NAME( const NAME<T> & other );                                         \
      NAME<T> & operator=( const NAME<T> & );                                \
      virtual ~NAME();                                                       \
      virtual NAME<T> *clone() const { return new NAME<T>(*this); }          \
      virtual void setOptions( const carto::Object & options );              \
      virtual void updateOptions( const carto::Object & options );           \
      const std::vector<int> & getAmplitude() const;                         \
      virtual T execute( const carto::VolumeRef<T> & in ) const;

#define AIMS_DECLARE_LINFILTERFUNC_BASIC( NAME )                             \
  AIMS_DECLARE_LINFILTERUNC_CUSTOMIZED(NAME)                                 \
  };                                                                         \
  template <typename T>                                                      \
  NAME<T>::NAME( const NAME<T> & other ) {}                                  \
  template <typename T>                                                      \
  NAME<T>::~NAME() {}                                                        \
  template <typename T>                                                      \
  NAME<T> & NAME<T>::operator=( const NAME<T> & other ) {}


namespace aims
{
  AIMS_DECLARE_LINFILTERUNC_CUSTOMIZED(GaborFilterFunc)
    protected:
      void setKernel();
      double _sigma;
      double _theta;
      double _lambda;
      double _psi;
      double _gamma;
      bool   _real;
      int    _nstd;
      bool   _init;
      std::vector<int>          _amplitude;
      std::vector<float>        _voxelsize;
      carto::VolumeRef<double>  _kernel;
  };
} // namespace aims

#endif
