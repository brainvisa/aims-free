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
#include <aims/io/writer.h>
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
    _init = false;
    _sigma = 1.0;
    _theta = 0.0;
    _lambda = 1.0;
    _psi = 0.0;
    _gamma = 1.0;
    _real = true;
    _nstd = 2;
    _voxelsize = std::vector<float>(4,1.0);

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
      if( options->hasProperty( "nstd" ) )
        _real = (int)options->getProperty( "nstd" )->getScalar();
    }

    if( carto::verbose )
    {
      std::cout << "Gabor Filter created with parameters:" << std::endl
                << "- sigma: "  << _sigma  << " mm" << std::endl
                << "- theta: "  << _theta  << "°"   << std::endl
                << "- lambda: " << _lambda << " mm" << std::endl
                << "- psi: "    << _psi    << "°"   << std::endl
                << "- gamma: "  << _gamma           << std::endl
                << "- return "  << ( _real ? "real part" : "imaginary part" ) << std::endl
                << "- nstd: "   << _nstd            << std::endl
                << std::endl;
    }
  }

  template <typename T>
  StructuringElementRef GaborFilterFunc<T>::getStructuringElement(
    const std::vector<float> & voxel_size
  )
  {
    if( voxel_size[0] != _voxelsize[0] ||
        voxel_size[1] != _voxelsize[1] ||
        !_init )
      setKernel( voxel_size );

    return StructuringElementRef( new strel::SquareXY( _amplitude ) );
  }

  template <typename T> inline
  void GaborFilterFunc<T>::setKernel(
    const std::vector<float> & voxel_size
  )
  {
    std::cout << "setKernel" << std::endl;
    _voxelsize = voxel_size;
    std::cout << "- voxel size: "
              << _voxelsize[0] << ", "
              << _voxelsize[1] << std::endl;

    double pi = 3.1415926535897;
    _amplitude = std::vector<double>(2,0.);
    _amplitude[0] = std::max(
      std::fabs( _nstd * _sigma * std::cos(_theta*pi/180. ) ),
      std::fabs( _nstd * _sigma/_gamma * std::sin(_theta*pi/180. ) )
    );
    _amplitude[1] = std::max(
      std::fabs( _nstd * _sigma * std::sin(_theta*pi/180. ) ),
      std::fabs( _nstd * _sigma/_gamma * std::cos(_theta*pi/180. ) )
    );
    _amplitude[0] /= _voxelsize[0];
    _amplitude[1] /= _voxelsize[1];
    _kernel = carto::VolumeRef<double>( (int)_amplitude[0] * 2 + 1, (int)_amplitude[1] * 2 + 1 );
    std::cout << "- amplitude: "
              << _amplitude[0] << ", "
              << _amplitude[1] << std::endl;


    double offsetx = (double)(_kernel->getSizeX())/2;
    double offsety = (double)(_kernel->getSizeY())/2;
    double xp, yp;
    for( int32_t y=0; y<_kernel->getSizeY(); ++y )
      for( int32_t x=0; x<_kernel->getSizeX(); ++x )
      {
        xp = ( (double)x-offsetx ) * _voxelsize[0] * std::cos( _theta*pi/180. ) +
             ( (double)y-offsety ) * _voxelsize[1] * std::sin( _theta*pi/180. );
        yp = ( -(double)x+offsetx ) * _voxelsize[0] * std::sin( _theta*pi/180. ) +
             ( (double)y-offsety ) * _voxelsize[1] * std::cos( _theta*pi/180. );
        _kernel(x,y) = std::exp( ( std::pow(xp,2) +
                                   std::pow(_gamma,2) *
                                   std::pow(yp,2) ) /
                       ( -2. * std::pow(_sigma,2) ) );
        if( _real )
          _kernel(x,y) *= std::cos( 2. * pi * xp / _lambda + _psi * pi / 180. );
        else
          _kernel(x,y) *= std::sin( 2. * pi * xp / _lambda + _psi * pi / 180. );
      }

    _init = true;
    Writer<carto::Volume<double> > writer("kernel.ima");
    writer << *_kernel;
  }

  template <typename T> inline
  T GaborFilterFunc<T>::execute( const carto::VolumeRef<T> & volume )
  {
    ASSERT( volume->getSizeZ() == 1 );
    ASSERT( volume->getSizeT() == 1 );
    double result = 0;
    std::vector<float> voxel_size(4,1.0);
    volume->header().getProperty( "voxel_size", voxel_size );
    if( voxel_size[0] != _voxelsize[0] ||
        voxel_size[1] != _voxelsize[1] ||
        !_init )
      setKernel( voxel_size );

    for( int32_t y=0; y<volume->getSizeY(); ++y )
      for( int32_t x=0; x<volume->getSizeX(); ++x )
        result += _kernel(x,y) * volume(x,y);

    return (T) result;
  }

  template <typename T> inline
  T GaborFilterFunc<T>::execute( const carto::VolumeRef<T> & volume,
                                 const StructuringElementRef & se )
  {
    ASSERT( volume->getSizeZ() == 1 );
    ASSERT( volume->getSizeT() == 1 );
    ASSERT( se != strel::none() );
    int x, y;
    double result;
    std::vector<float> voxel_size(4,1.0);
    volume->header().getProperty( "voxel_size", voxel_size );
    if( voxel_size[0] != _voxelsize[0] ||
        voxel_size[1] != _voxelsize[1] ||
        !_init )
      setKernel( voxel_size );
    StructuringElement::const_iterator i, e = se->end();

    for( i=se->begin(); i!=e; ++i ) {
      x = (*i)[0];
      y = (*i)[1];
      result += _kernel( x + std::floor((double)(_kernel->getSizeX())/2),
                         y + std::floor((double)(_kernel->getSizeY())/2) )
                * volume(x,y);
    }

    return (T) result;
  }
} // namespace aims

#endif
