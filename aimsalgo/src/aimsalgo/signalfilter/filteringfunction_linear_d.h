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


#ifndef AIMS_SIGNALFILTER_FILTERINGFUNCTION_LINEAR_D_H
#define AIMS_SIGNALFILTER_FILTERINGFUNCTION_LINEAR_D_H

//--- aims -------------------------------------------------------------------
#include <aims/signalfilter/filteringfunction_linear.h>
#include <aims/connectivity/structuring_element.h> // aims::StructuringElement
//--- cartodata --------------------------------------------------------------
#include <cartodata/volume/volume.h>                       // carto::VolumeRef
//--- cartobase --------------------------------------------------------------
#include <cartobase/config/verbose.h>                        // carto::verbose
#include <cartobase/smart/rcptr.h>                            // carto::rc_ptr
//--- std --------------------------------------------------------------------
#include <cmath>                                                  // std::fabs
#include <algorithm>                                               // std::max
#include <map>
#include <set>
#include <string>
//----------------------------------------------------------------------------

namespace aims {

//============================================================================
// LINEAR FILTERING FUNCTIONS: FACTORY
//============================================================================

  template <typename T>
  void LinFilterFuncFactory<T>::init()
  {
    static bool initialized = false;
    if( !initialized )
    {
      initialized = true;
      registerFunction( "gab", GaborFilterFunc<T>() );
      registerFunction( "gabor", GaborFilterFunc<T>() );
    }
  }

  template <typename T>
  std::map<std::string,carto::rc_ptr<LinFilterFunc<T> > > & LinFilterFuncFactory<T>::_map()
  {
    static std::map<std::string,carto::rc_ptr<LinFilterFunc<T> > > m;
    return m;
  }

  template <typename T>
  void LinFilterFuncFactory<T>::registerFunction(
    const std::string & name,
    const LinFilterFunc<T> & func
  )
  {
    init();
    _map()[ name ] = carto::rc_ptr<LinFilterFunc<T> >( func.clone() );
  }

  template <typename T>
  std::set<std::string> LinFilterFuncFactory<T>::names()
  {
    init();
    std::set<std::string> s;
    typename std::map<std::string,carto::rc_ptr<LinFilterFunc<T> > >::const_iterator i, e = _map().end();
    for( i=_map().begin(); i!=e; ++i )
      s.insert( i->first );
    return( s );
  }

  template <typename T>
  LinFilterFunc<T>* LinFilterFuncFactory<T>::create(
    const std::string & name,
    carto::Object options
  )
  {
    init();
    typename std::map<std::string,carto::rc_ptr<LinFilterFunc<T> > >::const_iterator i;
    i = _map().find( name );
    if( i == _map().end() )
      return( 0 );
    LinFilterFunc<T> * new_func = i->second->clone();
    new_func->setOptions( options );
    return new_func;
  }

//============================================================================
// LINEAR FILTERING FUNCTIONS: DERIVED CLASSES
//============================================================================
  //--------------------------------------------------------------------------
  // GaborFilterFunc
  //--------------------------------------------------------------------------
  template <typename T>
  void GaborFilterFunc<T>::setOptions( carto::Object options )
  {
    _sigma = 1.0;
    _theta = 0.0;
    _lambda = 1.0;
    _psi = 0.0;
    _gamma = 1.0;
    _real = true;

    if( options )
    {
      if( options->hasProperty( "sigma" ) )
        _sigma = (double)options->getProperty( "sigma" )->getScalar();
      if( options->hasProperty( "theta" ) )
        _theta = (double)options->getProperty( "theta" )->getScalar();
      if( options->hasProperty( "lambda" ) )
        _lambda = (double)options->getProperty( "lambda" )->getScalar();
      if( options->hasProperty( "psi" ) )
        _psi = (double)options->getProperty( "psi" )->getScalar();
      if( options->hasProperty( "gamma" ) )
        _gamma = (double)options->getProperty( "gamma" )->getScalar();
      if( options->hasProperty( "real" ) )
        _real = (bool)options->getProperty( "real" )->getScalar();
    }

    if( carto::verbose )
    {
      std::cout << "Gabor Filter created with parameters:" << std::endl
                << "- sigma: "  << _sigma  << " mm" << std::endl
                << "- theta: "  << _theta  << "°"   << std::endl
                << "- lambda: " << _lambda << " mm" << std::endl
                << "- psi: "    << _psi    << "°"   << std::endl
                << "- gamma: "  << _gamma           << std::endl
                << "- return "  << ( _real ? "real part" : "imaginary part" )
                << std::endl;
    }
  }

  template <typename T>
  StructuringElementRef GaborFilterFunc<T>::getStructuringElement(
    const std::vector<double> & voxel_size
  ) const
  {
    double pi = 3.1415926535897;
    std::vector<double> amplitude(2,0.);
    double nstds = 3.; // should keep 99% of the filter
    amplitude[0] = std::max(
      std::fabs( nstds * _sigma * std::cos(_theta*pi/180. ) ),
      std::fabs( nstds * _sigma/_gamma * std::sin(_theta*pi/180. ) )
    );
    amplitude[1] = std::max(
      std::fabs( nstds * _sigma * std::sin(_theta*pi/180. ) ),
      std::fabs( nstds * _sigma/_gamma * std::cos(_theta*pi/180. ) )
    );
    amplitude[0] /= voxel_size[0];
    amplitude[1] /= voxel_size[1];
    return StructuringElementRef( new strel::SquareXY( amplitude ) );
  }

  template <typename T> inline
  T GaborFilterFunc<T>::execute( const carto::VolumeRef<T> & volume ) const
  {
    ASSERT( volume->getSizeZ() == 1 );
    ASSERT( volume->getSizeT() == 1 );
    double offsetx = (double)(volume->getSizeX())/2;
    double offsety = (double)(volume->getSizeY())/2;
    double result = 0;
    double gabor;
    int32_t xp, yp;
    double pi = 3.1415926535897;
    std::vector<double> vs(4,1.0);
    volume->header().getProperty( "voxel_size", vs );

    for( int32_t y=0; y<volume->getSizeY(); ++y )
      for( int32_t x=0; x<volume->getSizeX(); ++x )
      {
        xp = ( (double)x-offsetx ) * vs[0] * std::cos( _theta*pi/180. ) +
             ( (double)y-offsety ) * vs[1] * std::sin( _theta*pi/180. );
        yp = ( -(double)x+offsetx ) * vs[0] * std::sin( _theta*pi/180. ) +
             ( (double)y-offsety ) * vs[1] * std::cos( _theta*pi/180. );
        gabor = std::exp( ( std::pow((float)xp,2) + std::pow((float)_gamma,2) * std::pow((float)yp,2) ) /
                          ( -2. * std::pow((float)_sigma,2) ) );
        if( _real )
          gabor *= std::cos( 2. * pi * xp / _lambda + _psi * pi / 180. );
        else
          gabor *= std::sin( 2. * pi * xp / _lambda + _psi * pi / 180. );
        result += gabor * volume(x,y,0,0);
      }

    return (T) result;
  }

  template <typename T> inline
  T GaborFilterFunc<T>::execute( const carto::VolumeRef<T> & volume,
                                 const StructuringElementRef & se ) const
  {
    ASSERT( volume->getSizeZ() == 1 );
    ASSERT( volume->getSizeT() == 1 );
    ASSERT( se != strel::none() );
    double offsetx = (double)(volume->getSizeX())/2;
    double offsety = (double)(volume->getSizeY())/2;
    double result = 0.0;
    double gabor;
    int32_t xp, yp;
    double pi = 3.1415926535897;
    std::vector<double> vs(4,1.0);
    volume->header().getProperty( "voxel_size", vs );
    StructuringElement::const_iterator i, e = se->end();

    for( i=se->begin(); i!=e; ++i )
    {
      xp = ( (double)(*i)[0] ) * vs[0] * std::cos( _theta*pi/180. ) +
           ( (double)(*i)[1] ) * vs[1] * std::sin( _theta*pi/180. );
      yp = ( -(double)(*i)[0] ) * vs[0] * std::sin( _theta*pi/180. ) +
           ( (double)(*i)[1] ) * vs[1] * std::cos( _theta*pi/180. );
      gabor = std::exp( ( std::pow((float)xp,2) + std::pow((float)_gamma,2) * std::pow((float)yp,2) ) /
                        ( -2. * std::pow((float)_sigma,2) ) );
      if( _real )
        gabor *= std::cos( 2. * pi * xp / _lambda + _psi * pi / 180. );
      else
        gabor *= std::sin( 2. * pi * xp / _lambda + _psi * pi / 180. );
      result += gabor * volume((*i)[0],(*i)[1],0,0);
    }

    // if( result != 0.0)
      // std::cout << result << " -> " << (T) result << std::endl;
    return (T) result;
  }
} // namespace aims

#endif
