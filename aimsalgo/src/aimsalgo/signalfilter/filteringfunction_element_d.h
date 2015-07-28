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


#ifndef AIMS_SIGNALFILTER_FILTERINGFUNCTION_NONLINEAR_D_H
#define AIMS_SIGNALFILTER_FILTERINGFUNCTION_NONLINEAR_D_H

//--- aims -------------------------------------------------------------------
#include <aims/signalfilter/filteringfunction_element.h>
#include <aims/connectivity/structuring_element.h> // aims::StructuringElement
#include <aims/math/mathelem.h>              // absdiff + all iterator methods
//--- cartodata --------------------------------------------------------------
#include <cartodata/volume/volumeutil_d.h>
#include <cartodata/volume/volume.h>                       // carto::VolumeRef
//--- cartobase --------------------------------------------------------------
#include <cartobase/config/verbose.h>                        // carto::verbose
#include <cartobase/smart/rcptr.h>                            // carto::rc_ptr
//--- std --------------------------------------------------------------------
#include <cmath>
#include <map>
#include <set>
#include <string>
#include <algorithm>                                       // std::max_element
#include <limits>                                       // std::numeric_limits
//----------------------------------------------------------------------------

namespace aims {

//============================================================================
// ELEMENT FILTERING FUNCTIONS: FACTORY
//============================================================================

  template <typename T>
  void ElementFilteringFunctionFactory<T>::init()
  {
    static bool initialized = false;
    if( !initialized )
    {
      initialized = true;
      registerFunction( "min", MinFilterFunc<T>() );
      registerFunction( "max", MaxFilterFunc<T>() );
      registerFunction( "med", MedianFilterFunc<T>() );
      registerFunction( "median", MedianFilterFunc<T>() );
      registerFunction( "notnullmedian", NotNullMedianFilterFunc<T>() );
      registerFunction( "mea", MeanFilterFunc<T>() );
      registerFunction( "mean", MeanFilterFunc<T>() );
      registerFunction( "notnullmean", NotNullMeanFilterFunc<T>() );
      registerFunction( "maj", MajorityFilterFunc<T>() );
      registerFunction( "majority", MajorityFilterFunc<T>() );
      registerFunction( "dif", ExtremaDifferenceFilterFunc<T>() );
      registerFunction( "difference", ExtremaDifferenceFilterFunc<T>() );
      registerFunction( "sum", SumFilterFunc<T>() );
      registerFunction( "var", VarFilterFunc<T>() );
      registerFunction( "variance", VarFilterFunc<T>() );
      registerFunction( "sd", StDevFilterFunc<T>() );
    }
  }

  template <typename T>
  std::map<std::string,carto::rc_ptr<ElementFilteringFunction<T> > > &
  ElementFilteringFunctionFactory<T>::_map()
  {
    static std::map<std::string,carto::rc_ptr<ElementFilteringFunction<T> > > m;
    return m;
  }

  template <typename T>
  void ElementFilteringFunctionFactory<T>::registerFunction(
    const std::string & name,
    const ElementFilteringFunction<T> & func
  )
  {
    init();
    _map()[ name ] = carto::rc_ptr<ElementFilteringFunction<T> >( func.clone() );
  }

  template <typename T>
  std::set<std::string> ElementFilteringFunctionFactory<T>::names()
  {
    init();
    std::set<std::string> s;
    typename std::map<std::string,carto::rc_ptr<ElementFilteringFunction<T> > >::const_iterator i, e = _map().end();
    for( i=_map().begin(); i!=e; ++i )
      s.insert( i->first );
    return( s );
  }

  template <typename T>
  ElementFilteringFunction<T>* ElementFilteringFunctionFactory<T>::create(
    const std::string & name,
    carto::Object options
  )
  {
    init();
    typename std::map<std::string,carto::rc_ptr<ElementFilteringFunction<T> > >::const_iterator i;
    i = _map().find( name );
    if( i == _map().end() )
      return( 0 );
    ElementFilteringFunction<T> * new_func = i->second->clone();
    new_func->setOptions( options );
    return new_func;
  }

//============================================================================
// ELEMENT FILTERING FUNCTIONS: DERIVED CLASSES
//============================================================================
  //--------------------------------------------------------------------------
  // MaxFilterFunc
  //--------------------------------------------------------------------------
  template <class T> inline
  T MaxFilterFunc<T>::execute( const carto::VolumeRef<T> & volume ) const
  {
    return carto::VolumeUtil<T>::max(volume);
  }

  template <class T> inline
  T MaxFilterFunc<T>::execute(
    const carto::VolumeRef<T> & volume,
    const carto::rc_ptr<StructuringElement> & se
  ) const
  {
    StructuredConstVolume<T> volse( *volume, *se );
    return MathUtil<T>::max( volse.begin(), volse.end() );
  }

  //--------------------------------------------------------------------------
  // MinFilterFunc
  //--------------------------------------------------------------------------
  template <class T> inline
  T MinFilterFunc<T>::execute( const carto::VolumeRef<T>& volume ) const
  {
    return carto::VolumeUtil<T>::min(volume);
  }

  template <class T> inline
  T MinFilterFunc<T>::execute(
    const carto::VolumeRef<T> & volume,
    const carto::rc_ptr<StructuringElement> & se
  ) const
  {
    StructuredConstVolume<T> volse( *volume, *se );
    return MathUtil<T>::min( volse.begin(), volse.end() );
  }

  //--------------------------------------------------------------------------
  // MedianFilterFunc
  //--------------------------------------------------------------------------
  template <class T> inline
  T MedianFilterFunc<T>::execute( const carto::VolumeRef<T> & volume ) const
  {
    return MathUtil<T>::median( volume.begin(), volume.end() );
  }

  template <class T> inline
  T MedianFilterFunc<T>::execute(
    const carto::VolumeRef<T> & volume,
    const carto::rc_ptr<StructuringElement> & se
  ) const
  {
    StructuredConstVolume<T> volse( *volume, *se );
    return MathUtil<T>::median( volse.begin(), volse.end() );
  }

  //--------------------------------------------------------------------------
  // NotNullMedianFilterFunc
  //--------------------------------------------------------------------------
  template <class T> inline
  T NotNullMedianFilterFunc<T>::execute( const carto::VolumeRef<T>& volume ) const
  {
    return MathUtil<T>::notnull_median( volume.begin(), volume.end() );
  }

  template <class T> inline
  T NotNullMedianFilterFunc<T>::execute(
    const carto::VolumeRef<T> & volume,
    const carto::rc_ptr<StructuringElement> & se
  ) const
  {
    StructuredConstVolume<T> volse( *volume, *se );
    return MathUtil<T>::notnull_median( volse.begin(), volse.end() );
  }

  //--------------------------------------------------------------------------
  // MeanFilterFunc
  //--------------------------------------------------------------------------
  template <class T> inline
  T MeanFilterFunc<T>::execute( const carto::VolumeRef<T>& volume ) const
  {
    return MathUtil<T>::mean( volume.begin(), volume.end() );
  }

  template <class T> inline
  T MeanFilterFunc<T>::execute(
    const carto::VolumeRef<T> & volume,
    const carto::rc_ptr<StructuringElement> & se
  ) const
  {
    StructuredConstVolume<T> volse( *volume, *se );
    return MathUtil<T>::mean( volse.begin(), volse.end() );
  }

  //--------------------------------------------------------------------------
  // NotNullMeanFilterFunc
  //--------------------------------------------------------------------------
  template <class T> inline
  T NotNullMeanFilterFunc<T>::execute( const carto::VolumeRef<T>& volume ) const
  {
    return MathUtil<T>::notnull_mean( volume.begin(), volume.end() );
  }

  template <class T> inline
  T NotNullMeanFilterFunc<T>::execute(
    const carto::VolumeRef<T> & volume,
    const carto::rc_ptr<StructuringElement> & se
  ) const
  {
    StructuredConstVolume<T> volse( *volume, *se );
    return MathUtil<T>::notnull_mean( volse.begin(), volse.end() );
  }

  //--------------------------------------------------------------------------
  // MajorityFilterFunc
  //--------------------------------------------------------------------------
  template <class T> inline
  T MajorityFilterFunc<T>::execute( const carto::VolumeRef<T>& volume ) const
  {
    return MathUtil<T>::majority( volume.begin(), volume.end() );
  }

  template <class T> inline
  T MajorityFilterFunc<T>::execute(
    const carto::VolumeRef<T> & volume,
    const carto::rc_ptr<StructuringElement> & se
  ) const
  {
    StructuredConstVolume<T> volse( *volume, *se );
    return MathUtil<T>::majority( volse.begin(), volse.end() );
  }

  //--------------------------------------------------------------------------
  // ExtremaDifferenceFilterFunc
  //--------------------------------------------------------------------------
  template <class T> inline
  T ExtremaDifferenceFilterFunc<T>::execute( const carto::VolumeRef<T>& volume ) const
  {
    return MathUtil<T>::extrema_difference( volume.begin(), volume.end() );
  }

  template <class T> inline
  T ExtremaDifferenceFilterFunc<T>::execute(
    const carto::VolumeRef<T> & volume,
    const carto::rc_ptr<StructuringElement> & se
  ) const
  {
    StructuredConstVolume<T> volse( *volume, *se );
    return MathUtil<T>::extrema_difference( volse.begin(), volse.end() );
  }

  //--------------------------------------------------------------------------
  // SumFilterFunc
  //--------------------------------------------------------------------------
  template <class T> inline
  T SumFilterFunc<T>::execute( const carto::VolumeRef<T>& volume ) const
  {
    return MathUtil<T>::sum( volume.begin(), volume.end() );
  }

  template <class T> inline
  T SumFilterFunc<T>::execute(
    const carto::VolumeRef<T> & volume,
    const carto::rc_ptr<StructuringElement> & se
  ) const
  {
    StructuredConstVolume<T> volse( *volume, *se );
    return MathUtil<T>::sum( volse.begin(), volse.end() );
  }

  //--------------------------------------------------------------------------
  // VarFilterFunc
  //--------------------------------------------------------------------------
  template <class T> inline
  T VarFilterFunc<T>::execute( const carto::VolumeRef<T>& volume ) const
  {
    return MathUtil<T>::variance( volume.begin(), volume.end() );
  }

  template <class T> inline
  T VarFilterFunc<T>::execute(
    const carto::VolumeRef<T> & volume,
    const carto::rc_ptr<StructuringElement> & se
  ) const
  {
    StructuredConstVolume<T> volse( *volume, *se );
    return MathUtil<T>::variance( volse.begin(), volse.end() );
  }

  //--------------------------------------------------------------------------
  // StDevFilterFunc
  //--------------------------------------------------------------------------
  template <class T> inline
  T StDevFilterFunc<T>::execute( const carto::VolumeRef<T>& volume ) const
  {
    return MathUtil<T>::stdev( volume.begin(), volume.end() );
  }

  template <class T> inline
  T StDevFilterFunc<T>::execute(
    const carto::VolumeRef<T> & volume,
    const carto::rc_ptr<StructuringElement> & se
  ) const
  {
    StructuredConstVolume<T> volse( *volume, *se );
    return MathUtil<T>::stdev( volse.begin(), volse.end() );
  }

} // namespace aims

#endif
