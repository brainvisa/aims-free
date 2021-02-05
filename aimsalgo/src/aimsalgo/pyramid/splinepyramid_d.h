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

#ifndef AIMS_INTERPOLATION_SPLINEPYRAMID_D_H
#define AIMS_INTERPOLATION_SPLINEPYRAMID_D_H

//--- aims -------------------------------------------------------------------
#include <aims/pyramid/splinepyramid.h>
#include <aims/pyramid/convolutionsubsampler_d.h>
#include <aims/signalfilter/splinefilter.h>  // aims::DirectBSplineFilter/InverseBSplineFitler
#include <aims/resampling/mediansubsampling.h>
//--- cartobase --------------------------------------------------------------
#include <cartobase/config/verbose.h>                        // carto::verbose
//--- std --------------------------------------------------------------------
#include <algorithm>
#include <cmath>
#include <limits>
#include <iostream>
//----------------------------------------------------------------------------

namespace aims {

  //==========================================================================
  //
  //                         MOVING AVERAGE KERNEL
  //
  //==========================================================================
  // u_m^n in Unser et al paper.
  // It can be used either to efficiently compute an expanded B-Spline kernel
  // or to downsample coefficients in the dual space

  class DiscreteU
  {
  public:
    DiscreteU( unsigned m, unsigned n );
    DiscreteU( const DiscreteU & other );
    virtual ~DiscreteU();
    DiscreteU & operator= ( const DiscreteU & other );

    double operator() ( int x ) const;
    double at( int x ) const;
    const Point2di & support() const;

  protected:
    Point2di _support;
    std::vector<double> _values;
  };

  DiscreteU::DiscreteU( unsigned m, unsigned n )
  {
    if( m == 1 )
    {
      _values.reserve(1);
      _values.push_back(1.);
      _support[0] = 0;
      _support[1] = 0;
    }

    if( n == 1 && m == 2 )
    {
      _values.reserve(3);
      _values.push_back(.5);
      _values.push_back(1.);
      _values.push_back(.5);
      _support[0] = -1;
      _support[1] = 1;
    }

    if( n == 3 && m == 2 )
    {
      _values.reserve(5);
      _values.push_back(1./8);
      _values.push_back(.5);
      _values.push_back(3./4);
      _values.push_back(.5);
      _values.push_back(1./8);
      _support[0] = -2;
      _support[1] = 2;
    }
  }

  DiscreteU::DiscreteU( const DiscreteU & other ):
    _support( other._support ),
    _values( other._values )
  {}

  DiscreteU::~DiscreteU()
  {}

  DiscreteU & DiscreteU::operator= ( const DiscreteU & other )
  {
    if( this != &other )
    {
      _values = other._values;
      _support = other._support;
    }
    return *this;
  }

  inline
  double DiscreteU::operator() ( int x ) const
  {
    return _values[ x - _support[0] ];
  }

  inline
  double DiscreteU::at( int x ) const
  {
    return _values[ x - _support[0] ];
  }

  inline
  const Point2di & DiscreteU::support() const
  {
    return _support;
  }

  //==========================================================================
  //
  //                             U(2,n) * B(2n+1)
  //
  //==========================================================================
  // To save computation time, the first level dual is not saved and the
  // second level dual is computed at once by a U*B subsampling

  class DiscreteUB
  {
  public:
    DiscreteUB( unsigned m, unsigned n );
    DiscreteUB( const DiscreteUB & other );
    virtual ~DiscreteUB();
    DiscreteUB & operator= ( const DiscreteUB & other );

    double operator() ( int x ) const;
    double at( int x ) const;
    const Point2di & support() const;

  protected:
    Point2di _support;
    std::vector<double> _values;
  };

  DiscreteUB::DiscreteUB( unsigned m, unsigned n )
  {
    if( m == 1 )
    {
      _values.reserve(7);
      _values.push_back(    1. / 5040. );
      _values.push_back(  120. / 5040. );
      _values.push_back( 1191. / 5040. );
      _values.push_back( 2416. / 5040. );
      _values.push_back( 1191. / 5040. );
      _values.push_back(  120. / 5040. );
      _values.push_back(    1. / 5040. );
      _support[0] = -3;
      _support[1] = 3;
    }

    if( n == 1 && m == 2 )
    {
      _values.reserve(9);
      _values.push_back(    .5 / 5040. );
      _values.push_back(   61. / 5040. );
      _values.push_back(  716. / 5040. );
      _values.push_back( 2459. / 5040. );
      _values.push_back( 3607. / 5040. );
      _values.push_back( 2459. / 5040. );
      _values.push_back(  716. / 5040. );
      _values.push_back(   61. / 5040. );
      _values.push_back(    .5 / 5040. );
      _support[0] = -4;
      _support[1] = 4;
    }

    if( n == 3 && m == 2 )
    {
      _values.reserve(11);
      _values.push_back(     1. / 40320. );
      _values.push_back(   124. / 40320. );
      _values.push_back(  1677. / 40320. );
      _values.push_back(  7904. / 40320. );
      _values.push_back( 18482. / 40320. );
      _values.push_back( 24264. / 40320. );
      _values.push_back( 18482. / 40320. );
      _values.push_back(  7904. / 40320. );
      _values.push_back(  1677. / 40320. );
      _values.push_back(   124. / 40320. );
      _values.push_back(     1. / 40320. );
      _support[0] = -5;
      _support[1] = 5;

    }
  }

  DiscreteUB::DiscreteUB( const DiscreteUB & other ):
    _support( other._support ),
    _values( other._values )
  {}

  DiscreteUB::~DiscreteUB()
  {}

  DiscreteUB & DiscreteUB::operator= ( const DiscreteUB & other )
  {
    if( this != &other )
    {
      _values = other._values;
      _support = other._support;
    }
    return *this;
  }

  inline
  double DiscreteUB::operator() ( int x ) const
  {
    return _values[ x - _support[0] ];
  }

  inline
  double DiscreteUB::at( int x ) const
  {
    return _values[ x - _support[0] ];
  }

  inline
  const Point2di & DiscreteUB::support() const
  {
    return _support;
  }

  //==========================================================================
  //
  //                         SPLINE PYRAMID BUILDER
  //
  //==========================================================================

  //--------------------------------------------------------------------------
  /// Constructors / Destructors / Copy
  //--------------------------------------------------------------------------

  SplinePyramidBuilder::SplinePyramidBuilder( unsigned factor, unsigned n ):
    _order( n ),
    _factor( 1, Point4du(factor, factor, factor, factor) ),
    _dir( 4, true ),
    _verbose( carto::verbose )
  {
    _dir[3] = false;
  }

  SplinePyramidBuilder::SplinePyramidBuilder( const Point4du & factor, unsigned n ):
    _order( n ),
    _factor( 1, factor ),
    _dir( 4, true ),
    _verbose( carto::verbose )
  {
    _dir[3] = false;
  }

  SplinePyramidBuilder::SplinePyramidBuilder( const std::vector<Point4du> & factor, unsigned n ):
    _order( n ),
    _factor( factor ),
    _dir( 4, true ),
    _verbose( carto::verbose )
  {
    _dir[3] = false;
  }

  SplinePyramidBuilder::SplinePyramidBuilder( const SplinePyramidBuilder & other ):
    _order( other._order ),
    _factor( other._factor ),
    _dir( other._dir ),
    _verbose( other._verbose )
  {}

  SplinePyramidBuilder::~SplinePyramidBuilder()
  {}

  SplinePyramidBuilder & SplinePyramidBuilder::operator=( const SplinePyramidBuilder & other )
  {
    if( this != &other )
    {
      _order = other._order;
      _factor = other._factor;
      _dir = other._dir;
      _verbose = other._verbose;
    }
    return *this;
  }


  //--------------------------------------------------------------------------
  /// Execution
  //--------------------------------------------------------------------------

  // helpers

  std::vector<Point4du> SplinePyramidBuilder::computeFactors( const std::vector<int> & size ) const
  {
    std::vector<Point4du> factors;
    if( _factor.size() > 1 )
    {
      factors = _factor;
    }
    else
    {
      unsigned nlevels = 0;
      Point4dl level_size( size[0], size[1], size[2], size[3] );
      bool ok = true;
      while( ok )
      {
        ++nlevels;
        for( int i = 0; i < 4; ++i )
        {
          if( _dir[i] ) {
            level_size[i] /= _factor[0][i];
            if( level_size[i] == 1 )
              ok = false;
          }
        }
      }
      factors.assign( nlevels, _factor[0] );
    }

    Point4dl level_size( size[0], size[1], size[2], size[3] );
    for( std::vector<Point4du>::iterator f = factors.begin();
         f != factors.end(); ++f )
    {
      for( int i = 0; i < 4; ++i )
      {
        if( _dir[i] )
        {
          if( level_size[i] / (*f)[i] == 0 )
            (*f)[i] = 1;
          else
            level_size[i] /= (*f)[i];
        }
        else
          (*f)[i] = 1;
      }
      if( *f == Point4du(1, 1, 1, 1) )
      {
        factors.erase( f, factors.end() );
        break;
      }
    }

    return factors;
  }

  // doer

  template <typename T>
  SplinePyramid SplinePyramidBuilder::execute( const carto::VolumeRef<T> & vol, bool is_coeff ) const
  {
    std::vector<Point4du> factors = computeFactors( vol.getSize() );
    std::vector<carto::VolumeRef<double> > pyramid;
    pyramid.reserve( factors.size() + 1 );
    std::vector<InterpolatedVolume> interpol( factors.size() + 1 );

    // ROOT LEVEL
    if( is_coeff )
    {
      if( _verbose > 1 )
        std::cout << "Copy root level spline coefficients... " << std::endl;
      pyramid.push_back( carto::copy<double, T>(vol) );
    }
    else
    {
      if( _verbose > 1 )
        std::cout << "Compute root level spline coefficients from image... " << std::endl;
      pyramid.push_back( carto::copy<double, T>(vol) );

      if( _verbose > 0 )
        std::cout << "BSpline inverse filter: " << std::flush;
      InverseBSplineFilter invfilter( _order );
      invfilter.setDirections( directions() );
      invfilter.setVerbose( _verbose - 1 );
      invfilter.execute( pyramid[0], true );
    }
    interpol[0].setCoeff( pyramid[0] );

    // FIRST LEVEL
    if( _verbose > 1 )
      std::cout << "Compute dual coefficients for level 1... " << std::endl;

    if( _verbose > 0 )
      std::cout << "Moving average * Direct BSpline filter/subsampling: " << std::flush;
    std::vector<DiscreteUB> bufunc;
    bufunc.reserve(4);
    bufunc.push_back( DiscreteUB( factors[0][0], _order ) );
    bufunc.push_back( DiscreteUB( factors[0][1], _order ) );
    bufunc.push_back( DiscreteUB( factors[0][2], _order ) );
    bufunc.push_back( DiscreteUB( factors[0][3], _order ) );
    ConvolutionSubSampler<DiscreteUB> busub( bufunc,factors[0] );
    busub.setDirections( directions() );
    busub.setVerbose( _verbose - 1 );
    pyramid.push_back( busub.execute<double, double>( pyramid[0] ) );
    pyramid[1] /= factors[0][0] * factors[0][1] * factors[0][2] * factors[0][3];

    // OTHER LEVELS
    std::vector<Point4du>::iterator f = ( factors.begin() == factors.end()
                                          ? factors.end()
                                          : ++(factors.begin()) );
    int l = 2;
    for( ; f != factors.end(); ++f, ++l )
    {
      if( _verbose > 0 )
        std::cout << "Compute dual coefficients for level " << l << "... " << std::endl;

      if( _verbose > 1 )
        std::cout << "Moving average filter/subsampling: " << std::flush;
      std::vector<DiscreteU> bfunc;
      bfunc.reserve(4);
      bfunc.push_back( DiscreteU( (*f)[0], _order ) );
      bfunc.push_back( DiscreteU( (*f)[1], _order ) );
      bfunc.push_back( DiscreteU( (*f)[2], _order ) );
      bfunc.push_back( DiscreteU( (*f)[3], _order ) );
      ConvolutionSubSampler<DiscreteU> sub( bfunc, *f );
      sub.setDirections( directions()[0] && (*f)[0] > 1,
                         directions()[1] && (*f)[1] > 1,
                         directions()[2] && (*f)[2] > 1,
                         directions()[3] && (*f)[3] > 1 );
      sub.setVerbose( _verbose - 1 );
      pyramid.push_back( sub.execute<double, double>( pyramid[l-1] ) );
      pyramid[l] /= (*f)[0] * (*f)[1] * (*f)[2] * (*f)[3];
      interpol[l].setCoeff( pyramid[l] );

      if( _verbose > 0 )
        std::cout << "Compute spline coefficients for level " << l-1 << "... " << std::endl;
      if( _verbose > 1 )
        std::cout << "BSpline inverse filter: " << std::flush;
      InverseBSplineFilter invfilter( 2 * _order + 1 );
      invfilter.setDirections( directions() );
      invfilter.setVerbose( _verbose - 1 );
      invfilter.execute( pyramid[l-1], true );
      if( _verbose > 1 )
        std::cout << std::endl;
      interpol[l-1].setCoeff( pyramid[l-1] );
    }

    // LAST LEVEL
    if( _verbose > 0 )
      std::cout << "Compute spline coefficients for level " << l-1 << "... " << std::endl;
    if( _verbose > 1 )
      std::cout << "BSpline inverse filter: " << std::flush;
    InverseBSplineFilter invfilter( 2 * _order + 1 );
    invfilter.setDirections( directions() );
    invfilter.setVerbose( _verbose - 1 );
    invfilter.execute( pyramid[l-1], true );
    if( _verbose > 1 )
      std::cout << std::endl;
    interpol[l-1].setCoeff( pyramid[l-1] );

    return SplinePyramid( interpol );
  }


  //--------------------------------------------------------------------------
  /// Parameters
  //--------------------------------------------------------------------------

  unsigned SplinePyramidBuilder::order() const
  {
    return _order;
  }

  const std::vector<bool> & SplinePyramidBuilder::directions() const
  {
    return _dir;
  }

  const std::vector<Point4du> & SplinePyramidBuilder::factor() const
  {
    return _factor;
  }

  void SplinePyramidBuilder::setOrder( unsigned n )
  {
    _order = n;
  }

  void SplinePyramidBuilder::setFactor( unsigned r )
  {
    _factor.assign( 1, Point4du( r, r, r, r ) );
  }

  void SplinePyramidBuilder::setFactor( const Point4du & r )
  {
    _factor.assign( 1, r );
  }

  void SplinePyramidBuilder::setFactor( const std::vector<Point4du> & r )
  {
    _factor = r;
  }

  void SplinePyramidBuilder::setDirections( const std::vector<bool> & dir )
  {
    _dir.assign( 4, true );
    _dir[3] = false;
    for( std::vector<bool>::size_type i = 0; i < dir.size() && i < _dir.size(); ++i )
      _dir[i] = dir[i];
  }

  void SplinePyramidBuilder::setDirections( bool dirx, bool diry, bool dirz, bool dirt )
  {
    _dir[0] = dirx;
    _dir[1] = diry;
    _dir[2] = dirz;
    _dir[3] = dirt;
  }

  void SplinePyramidBuilder::setVerbose( int verbose )
  {
    _verbose = verbose;
  }

  void SplinePyramidBuilder::setQuiet()
  {
    _verbose = 0;
  }

//============================================================================
//
//                            SPLINE PYRAMID
//
//============================================================================

  template <typename T>
  SplinePyramid::SplinePyramid( const std::vector<carto::VolumeRef<T> > & pyramid,
                                unsigned order,
                                bool is_coeff ):
    _pyramid()
  {
    if( is_coeff )
      _pyramid.assign( pyramid.size(), InterpolatedVolume() );
    else
      _pyramid.reserve( pyramid.size() );

    for( size_t i = 0; i < pyramid.size(); ++i )
    {
      if( is_coeff )
        _pyramid[i].setCoeff( pyramid[i] );
      else
        _pyramid.push_back( InterpolatedVolume( pyramid[i], order ) );
    }
  }

} // namespace aims

#endif // AIMS_INTERPOLATION_SPLINEPYRAMID_D_H
