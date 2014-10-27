/* This software and supporting documentation are distributed by
 *     Institut Federatif de Recherche 49
 *     CEA/NeuroSpin, Batiment 145,
 *     91191 Gif-sur-Yvette cedex
 *     France
 *
 * This software is governed by the CeCILL-B license under
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the
 * terms of the CeCILL-B license as circulated by CEA, CNRS
 * and INRIA at the following URL "http://www.cecill.info".
 *
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability.
 *
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or
 * data to be ensured and,  more generally, to use and operate it in the
 * same conditions as regards security.
 *
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL-B license and that you accept its terms.
 */


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
  // LINEAR FILTERING FUNCTIONS: INTERFACE
  //==========================================================================
  /// Pure virtual class: interface for non linear filtering functions.
  template <typename T>
  class LinFilterFunc: public FilteringFunction<T>
  {
    public:
      virtual ~LinFilterFunc() {};
      virtual bool isLinear() const { return true; }
      virtual StructuringElementRef getStructuringElement(
        const std::vector<float> & voxel_size = std::vector<float>(4,1.)
      ) = 0;
      virtual LinFilterFunc<T> * clone() const = 0;
  };

  //==========================================================================
  // LINEAR FILTERING FUNCTIONS: FACTORY
  //==========================================================================
  template <typename T>
  class LinFilterFuncFactory
  {
    public:
      static LinFilterFunc<T> * create( const std::string & name,
                                     carto::Object options = carto::none() );
      static void registerFunction( const std::string & name,
                                    const LinFilterFunc<T> & func );
      static std::set<std::string> names();
    protected:
      static void init();
      static std::map<std::string,carto::rc_ptr<LinFilterFunc<T> > > & _map();
  };

} // namespace aims


//============================================================================
// LINEAR FILTERING FUNCTIONS: DERIVED CLASSES
//============================================================================
#define AIMS_DECLARE_LINFILTERUNC_CUSTOMIZED( NAME )                         \
  template <typename T>                                                      \
  class NAME: public LinFilterFunc<T>                                        \
  {                                                                          \
    public:                                                                  \
      NAME( carto::Object options = carto::none() )                          \
        { setOptions( options ); }                                           \
      virtual ~NAME() {};                                                    \
      virtual void setOptions( carto::Object );                              \
      virtual T execute( const carto::VolumeRef<T> & volume ) ;              \
      virtual T execute( const carto::VolumeRef<T> & volume,                 \
                         const StructuringElementRef & se );                 \
      virtual StructuringElementRef getStructuringElement(                   \
        const std::vector<float> & voxel_size = std::vector<float>(4,1.)     \
      );                                                                     \
      virtual void setKernel(                                                \
        const std::vector<float> & voxel_size = std::vector<float>(4,1.)     \
      );                                                                     \
      virtual NAME<T> * clone() const { return new NAME<T>(); }

#define AIMS_DECLARE_LINFILTERFUNC_BASIC( NAME )                             \
  AIMS_DECLARE_LINFILTERUNC_CUSTOMIZED(NAME)                                 \
  };                                                                         \
  template <typename T>                                                      \
  void NAME<T>::setOptions( carto::Object ) {}                               \
  template <typename T>                                                      \
  void NAME<T>::setKernel( const std::vector<float> & ) {}                   \
  (void)0


namespace aims
{
  AIMS_DECLARE_LINFILTERUNC_CUSTOMIZED(GaborFilterFunc)
    protected:
      double _sigma;
      double _theta;
      double _lambda;
      double _psi;
      double _gamma;
      bool   _real;
      int    _nstd;
      bool   _init;
      std::vector<double>       _amplitude;
      std::vector<float>        _voxelsize;
      carto::VolumeRef<double>  _kernel;
  };
}

#endif
