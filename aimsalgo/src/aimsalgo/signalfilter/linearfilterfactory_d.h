#ifndef AIMS_SIGNALFILTER_LINEARFILTERFACTORY_D_H
#define AIMS_SIGNALFILTER_LINEARFILTERFACTORY_D_H

//--- aims -------------------------------------------------------------------
#include <aims/signalfilter/linearfilterfactory.h>
//--- registered algo --------------------------------------------------------
#include <aims/signalfilter/gaborfilter.h>
//----------------------------------------------------------------------------

namespace aims {

  template <typename T>
  void LinearFilterFactory<T>::init()
  {
    static bool initialized = false;
    if( !initialized )
    {
      initialized = true;
      registerFilter( "gab", GaborFilter<T>() );
      registerFilter( "gabor", GaborFilter<T>() );
    }
  }

  template <typename T>
  std::map<std::string,carto::rc_ptr<LinearFilteringImageAlgorithm<T> > > & LinearFilterFactory<T>::_map()
  {
    static std::map<std::string,carto::rc_ptr<LinearFilteringImageAlgorithm<T> > > m;
    return m;
  }

  template <typename T>
  void LinearFilterFactory<T>::registerFilter(
    const std::string & name,
    const LinearFilteringImageAlgorithm<T> & func
  )
  {
    init();
    _map()[ name ] = carto::rc_ptr<LinearFilteringImageAlgorithm<T> >( func.clone() );
  }

  template <typename T>
  std::set<std::string> LinearFilterFactory<T>::names()
  {
    init();
    std::set<std::string> s;
    typename std::map<std::string,carto::rc_ptr<LinearFilteringImageAlgorithm<T> > >::const_iterator i, e = _map().end();
    for( i=_map().begin(); i!=e; ++i )
      s.insert( i->first );
    return( s );
  }

  template <typename T>
  LinearFilteringImageAlgorithm<T>* LinearFilterFactory<T>::create(
    const std::string & name,
    carto::Object options
  )
  {
    init();
    typename std::map<std::string,carto::rc_ptr<LinearFilteringImageAlgorithm<T> > >::const_iterator i;
    i = _map().find( name );
    if( i == _map().end() )
      return( 0 );
    LinearFilteringImageAlgorithm<T> * new_func = i->second->clone();
    new_func->setOptions( options );
    return new_func;
  }

} // namespace aims

#endif
