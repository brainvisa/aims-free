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

#ifndef AIMS_SIGNALFILTER_ELEMENTFILTERFACTORY_D_H
#define AIMS_SIGNALFILTER_ELEMENTFILTERFACTORY_D_H

//--- aims -------------------------------------------------------------------
#include <aims/signalfilter/elementfilterfactory.h>
//--- registered algo --------------------------------------------------------
#include <aims/signalfilter/differencefilter.h>
#include <aims/signalfilter/majorityfilter.h>
#include <aims/signalfilter/maxfilter.h>
#include <aims/signalfilter/meanfilter.h>
#include <aims/signalfilter/medianfilter.h>
#include <aims/signalfilter/minfilter.h>
#include <aims/signalfilter/sdfilter.h>
#include <aims/signalfilter/sumfilter.h>
#include <aims/signalfilter/variancefilter.h>
//----------------------------------------------------------------------------

namespace aims {

  //==========================================================================
  // NON LINEAR FILTERING ALGORITHM: FACTORY
  //==========================================================================

  template <typename T>
  void ElementFilterFactory<T>::init()
  {
    static bool initialized = false;
    if( !initialized )
    {
      initialized = true;
      registerFilter( "min", MinFilter<T>() );
      registerFilter( "max", MaxFilter<T>() );
      registerFilter( "med", MedianFilter<T>() );
      registerFilter( "median", MedianFilter<T>() );
      registerFilter( "notnullmedian", NotNullMedianFilter<T>() );
      registerFilter( "mea", MeanFilter<T>() );
      registerFilter( "mean", MeanFilter<T>() );
      registerFilter( "notnullmean", NotNullMeanFilter<T>() );
      registerFilter( "maj", MajorityFilter<T>() );
      registerFilter( "majority", MajorityFilter<T>() );
      registerFilter( "dif", ExtremaDifferenceFilter<T>() );
      registerFilter( "difference", ExtremaDifferenceFilter<T>() );
      registerFilter( "sum", SumFilter<T>() );
      registerFilter( "var", VarFilter<T>() );
      registerFilter( "variance", VarFilter<T>() );
      registerFilter( "sd", StDevFilter<T>() );
    }
  }

  template <typename T>
  std::map<std::string,carto::rc_ptr<ElementFilteringImageAlgorithm<T> > > & ElementFilterFactory<T>::_map()
  {
    static std::map<std::string,carto::rc_ptr<ElementFilteringImageAlgorithm<T> > > m;
    return m;
  }

  template <typename T>
  void ElementFilterFactory<T>::registerFilter(
    const std::string & name,
    const ElementFilteringImageAlgorithm<T> & func
  )
  {
    init();
    _map()[ name ] = carto::rc_ptr<ElementFilteringImageAlgorithm<T> >( func.clone() );
  }

  template <typename T>
  std::set<std::string> ElementFilterFactory<T>::names()
  {
    init();
    std::set<std::string> s;
    typename std::map<std::string,carto::rc_ptr<ElementFilteringImageAlgorithm<T> > >::const_iterator i, e = _map().end();
    for( i=_map().begin(); i!=e; ++i )
      s.insert( i->first );
    return( s );
  }

  template <typename T>
  ElementFilteringImageAlgorithm<T>* ElementFilterFactory<T>::create(
    const std::string & name,
    const StructuringElement & strel,
    carto::Object options
  )
  {
    init();
    typename std::map<std::string,carto::rc_ptr<ElementFilteringImageAlgorithm<T> > >::const_iterator i;
    i = _map().find( name );
    if( i == _map().end() )
      return( 0 );
    ElementFilteringImageAlgorithm<T> * new_func = i->second->clone();
    new_func->setOptions( options );
    new_func->setStructuringElement( strel );
    return new_func;
  }

  template <typename T>
  ElementFilteringImageAlgorithm<T>* ElementFilterFactory<T>::create(
    const std::string & name,
    carto::Object options
  )
  {
    return create( name, strel::Cube(1.0), options );
  }


} // namespace aims

#endif
