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
#include <aims/signalfilter/filteringfunction_nonlinear.h>
#include <aims/connectivity/structuring_element.h> // aims::StructuringElement
#include <aims/math/mathelem.h>                               // aims::absdiff
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
// NON LINEAR FILTERING FUNCTIONS: FACTORY
//============================================================================

  template <typename T>
  void NonLinFilterFuncFactory<T>::init()
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
    }
  }

  template <typename T>
  std::map<std::string,carto::rc_ptr<NonLinFilterFunc<T> > > & NonLinFilterFuncFactory<T>::_map()
  {
    static std::map<std::string,carto::rc_ptr<NonLinFilterFunc<T> > > m;
    return m;
  }

  template <typename T>
  void NonLinFilterFuncFactory<T>::registerFunction(
    const std::string & name,
    const NonLinFilterFunc<T> & func
  )
  {
    init();
    _map()[ name ] = carto::rc_ptr<NonLinFilterFunc<T> >( func.clone() );
  }

  template <typename T>
  std::set<std::string> NonLinFilterFuncFactory<T>::names()
  {
    init();
    std::set<std::string> s;
    typename std::map<std::string,carto::rc_ptr<NonLinFilterFunc<T> > >::const_iterator i, e = _map().end();
    for( i=_map().begin(); i!=e; ++i )
      s.insert( i->first );
    return( s );
  }

  template <typename T>
  NonLinFilterFunc<T>* NonLinFilterFuncFactory<T>::create(
    const std::string & name,
    carto::Object options
  )
  {
    init();
    typename std::map<std::string,carto::rc_ptr<NonLinFilterFunc<T> > >::const_iterator i;
    i = _map().find( name );
    if( i == _map().end() )
      return( 0 );
    NonLinFilterFunc<T> * new_func = i->second->clone();
    new_func->setOptions( options );
    return new_func;
  }

//============================================================================
// NON LINEAR FILTERING FUNCTIONS: DERIVED CLASSES
//============================================================================
  //--------------------------------------------------------------------------
  // MaxFilterFunc
  //--------------------------------------------------------------------------
  namespace {
    template <typename T>
    bool is_min( T a, T b )
    {
      return (int)a < (int)b;
    }

    template<typename Iterator, typename T>
    T max_iterate( Iterator b, Iterator e, T init )
    {
      Iterator m;
      m = std::max_element( b, e, is_min<T> );
      if( m == e )
        return init;
      else
        return T(*m);
    }
  } // namespace

  template <class T> inline
  T MaxFilterFunc<T>::execute( const carto::VolumeRef<T> & volume ) const
  {
    return carto::VolumeUtil<T>::max(volume);
  }

  template <class T> inline
  T MaxFilterFunc<T>::execute(
    const carto::VolumeRef<T> & volume,
    const StructuringElementRef & se
  ) const
  {
    StructuredConstVolume<T> volse( *volume, *se );
    return max_iterate( volse.begin(), volse.end(), std::numeric_limits<T>::max() );
  }

  //--------------------------------------------------------------------------
  // MinFilterFunc
  //--------------------------------------------------------------------------
  namespace {
    template <typename Iterator, typename T>
    T min_iterate( Iterator b, Iterator e, T init )
    {
      Iterator m;
      m = std::min_element( b, e, is_min<T> );
      if( m == e )
        return init;
      else
        return T(*m);
    }
  } // namespace

  template <class T> inline
  T MinFilterFunc<T>::execute( const carto::VolumeRef<T>& volume ) const
  {
    return carto::VolumeUtil<T>::min(volume);
  }

  template <class T> inline
  T MinFilterFunc<T>::execute(
    const carto::VolumeRef<T> & volume,
    const StructuringElementRef & se
  ) const
  {
    StructuredConstVolume<T> volse( *volume, *se );
    return min_iterate( volse.begin(), volse.end(), std::numeric_limits<T>::max() );
  }

  //--------------------------------------------------------------------------
  // MedianFilterFunc
  //--------------------------------------------------------------------------
  namespace {
    template <typename Iterator, typename T>
    T median( Iterator b, Iterator e, T init )
    {
      Iterator i;
      std::vector<T> values;
      for( i=b; i!=e; ++i )
        values.push_back( (T) *i );

      if( values.size() == 0 )
        return init;
      std::sort( values.begin(), values.end() );
      return values[ values.size() / 2 ];
    }
  } // namespace

  template <class T> inline
  T MedianFilterFunc<T>::execute( const carto::VolumeRef<T> & volume ) const
  {
    return median( volume.begin(), volume.end(), (T)0 );
  }

  template <class T> inline
  T MedianFilterFunc<T>::execute(
    const carto::VolumeRef<T> & volume,
    const StructuringElementRef & se
  ) const
  {
    StructuredConstVolume<T> volse( *volume, *se );
    return median( volse.begin(), volse.end(), (T)0 );
  }

  //--------------------------------------------------------------------------
  // NotNullMedianFilterFunc
  //--------------------------------------------------------------------------
  namespace {
    template<typename Iterator, typename T>
    T notNullMedian( Iterator b, Iterator e, T init )
    {
      Iterator i;
      std::vector<T> values;
      for( i=b; i!=e; ++i )
        if( *i != 0 )
          values.push_back( *i );

      if( values.size() == 0 )
        return init;
      std::sort( values.begin(), values.end() );
      return values[ values.size() / 2 ];
    }
  } // namespace

  template <class T> inline
  T NotNullMedianFilterFunc<T>::execute( const carto::VolumeRef<T>& volume ) const
  {
    return notNullMedian( volume.begin(), volume.end(), (T)0 );
  }

  template <class T> inline
  T NotNullMedianFilterFunc<T>::execute(
    const carto::VolumeRef<T> & volume,
    const StructuringElementRef & se
  ) const
  {
    StructuredConstVolume<T> volse( *volume, *se );
    return notNullMedian( volse.begin(), volse.end(), (T)0 );
  }

  //--------------------------------------------------------------------------
  // MeanFilterFunc
  //--------------------------------------------------------------------------
  namespace {
    template <typename Iterator, typename T>
    T mean( Iterator b, Iterator e, T init )
    {
      Iterator i = b;
      double sum = (double) init;
      uint32_t count = 0;

      // Sum values and count values
      for( ; i != e; ++i) {
        sum += (double)(*i);
        ++count;
      }
      return (count != 0 ? (T)( sum / count ) : (T)0);
    }
  } // namespace

  template <class T> inline
  T MeanFilterFunc<T>::execute( const carto::VolumeRef<T>& volume ) const
  {
    return mean( volume.begin(), volume.end(), (T)0 );
  }

  template <class T> inline
  T MeanFilterFunc<T>::execute(
    const carto::VolumeRef<T> & volume,
    const StructuringElementRef & se
  ) const
  {
    StructuredConstVolume<T> volse( *volume, *se );
    return mean( volse.begin(), volse.end(), (T)0 );
  }

  //--------------------------------------------------------------------------
  // NotNullMeanFilterFunc
  //--------------------------------------------------------------------------
  namespace {
    template <typename Iterator, typename T>
    T notNullMean( Iterator b, Iterator e, T init )
    {
      Iterator i;
      double sum = (double) init;
      uint32_t count = 0;

      // Sum values and count values
      for( i=b; i!=e; ++i )
        if( *i != 0 ) {
          sum += (double)(*i);
          ++count;
        }
      return (count != 0 ? (T)( sum / count ) : (T)0);
    }
  } // namespace

  template <class T> inline
  T NotNullMeanFilterFunc<T>::execute( const carto::VolumeRef<T>& volume ) const
  {
    return notNullMean( volume.begin(), volume.end(), (T)0 );
  }

  template <class T> inline
  T NotNullMeanFilterFunc<T>::execute(
    const carto::VolumeRef<T> & volume,
    const StructuringElementRef & se
  ) const
  {
    StructuredConstVolume<T> volse( *volume, *se );
    return notNullMean( volse.begin(), volse.end(), (T)0 );
  }

  //--------------------------------------------------------------------------
  // MajorityFilterFunc
  //--------------------------------------------------------------------------
  namespace {
    template <typename Iterator, typename T>
    T majority( Iterator b, Iterator e, T init )
    {
      Iterator i;
      T currentclass, majorityclass = init;
      uint32_t currentclasscases = 0, majoritycases = 0;
      std::map<T, uint32_t> classcases;

      // Goes through the data and count number of values for each class
      for( i=b; i!=e; ++i )
      {
        currentclass = (*i);

        if ( !classcases[ currentclass ] )
        {
            classcases[ currentclass ] = 1;
            currentclasscases = 1;
        }
        else
        {
            currentclasscases = classcases[ currentclass ] + 1;
            classcases[ currentclass ] = currentclasscases;
        }

        if (currentclasscases > majoritycases)
        {
            // Set the new majority cases and class for which it occurs
            majorityclass = currentclass;
            majoritycases = currentclasscases;
        }
      }

      return majorityclass;
    }
  } // namespace

  template <class T> inline
  T MajorityFilterFunc<T>::execute( const carto::VolumeRef<T>& volume ) const
  {
    return majority( volume.begin(), volume.end(), (T)0 );
  }

  template <class T> inline
  T MajorityFilterFunc<T>::execute(
    const carto::VolumeRef<T> & volume,
    const StructuringElementRef & se
  ) const
  {
    StructuredConstVolume<T> volse( *volume, *se );
    return majority( volse.begin(), volse.end(), (T)0 );
  }

  //--------------------------------------------------------------------------
  // ExtremaDifferenceFilterFunc
  //--------------------------------------------------------------------------
  template <class T> inline
  T ExtremaDifferenceFilterFunc<T>::execute( const carto::VolumeRef<T>& volume ) const
  {
    T min, max;
    min = carto::VolumeUtil<T>::min( volume );
    max = carto::VolumeUtil<T>::max( volume );
    return aims::absdiff<T>(max, min);
  }

  template <class T> inline
  T ExtremaDifferenceFilterFunc<T>::execute(
    const carto::VolumeRef<T> & volume,
    const StructuringElementRef & se
  ) const
  {
    T min, max;
    StructuredConstVolume<T> volse( *volume, *se );
    min = min_iterate( volse.begin(), volse.end(), std::numeric_limits<T>::max() );
    max = max_iterate( volse.begin(), volse.end(), std::numeric_limits<T>::min() );
    return aims::absdiff<T>(max, min);
  }

  //--------------------------------------------------------------------------
  // SumFilterFunc
  //--------------------------------------------------------------------------
  namespace {
    template <typename Iterator, typename T>
    T sum( Iterator b, Iterator e, T init )
    {
      Iterator i;
      double accumumate = (double) init;

      // Goes through the data and sum values
      for( i=b; i!=e; ++i )
        accumumate += (double)(*i);
      return (T)accumumate;
    }
  } // namespace

  template <class T> inline
  T SumFilterFunc<T>::execute( const carto::VolumeRef<T>& volume ) const
  {
    return sum( volume.begin(), volume.end(), (T) 0 );
  }

  template <class T> inline
  T SumFilterFunc<T>::execute(
    const carto::VolumeRef<T> & volume,
    const StructuringElementRef & se
  ) const
  {
    StructuredConstVolume<T> volse( *volume, *se );
    return sum( volse.begin(), volse.end(), (T)0 );
  }

} // namespace aims

#endif
