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

#ifndef AIMS_PYRAMID_SUBSAMPLINGPYRAMID_D_H
#define AIMS_PYRAMID_SUBSAMPLINGPYRAMID_D_H

#include <aims/pyramid/subsamplingpyramid.h>
#include <aims/io/writer.h>
#include <cartobase/config/verbose.h>

namespace aims {

  //--------------------------------------------------------------------------
  // Constructors / Destructors / Copy
  //--------------------------------------------------------------------------

  template <typename S>
  SubSamplingPyramidBuilder<S>::SubSamplingPyramidBuilder( const SubSampler & f, unsigned factor ):
    _subsampler( f ),
    _factor( 1, Point4du(factor, factor, factor, factor) ),
    _dir( 4, true ),
    _verbose( carto::verbose )
  {
    _dir[3] = false;
  }

  template <typename S>
  SubSamplingPyramidBuilder<S>::SubSamplingPyramidBuilder( const SubSampler & f, const Point4du & factor ):
    _subsampler( f ),
    _factor( 1, factor ),
    _dir( 4, true ),
    _verbose( carto::verbose )
  {
    _dir[3] = false;
  }

  template <typename S>
  SubSamplingPyramidBuilder<S>::SubSamplingPyramidBuilder( const SubSampler & f, const std::vector<Point4du> & factor ):
    _subsampler( f ),
    _factor( factor ),
    _dir( 4, true ),
    _verbose( carto::verbose )
  {
    _dir[3] = false;
  }

  template <typename S>
  SubSamplingPyramidBuilder<S>::SubSamplingPyramidBuilder( const SubSamplingPyramidBuilder & other ):
    _subsampler( other._subsampler ),
    _factor( other._factor ),
    _dir( other._dir ),
    _verbose( other._verbose )
  {}

  template <typename S>
  SubSamplingPyramidBuilder<S>::~SubSamplingPyramidBuilder()
  {}

  template <typename S>
  SubSamplingPyramidBuilder<S> &
  SubSamplingPyramidBuilder<S>::operator=( const SubSamplingPyramidBuilder & other )
  {
    if( this != &other )
    {
      _subsampler = other._subsampler;
      _factor = other._factor;
      _dir = other._dir;
      _verbose = other._verbose;
    }
    return *this;
  }


  //--------------------------------------------------------------------------
  // Parameters
  //--------------------------------------------------------------------------

  template <typename S>
  const std::vector<bool> & SubSamplingPyramidBuilder<S>::directions() const
  {
    return _dir;
  }

  template <typename S>
  const std::vector<Point4du> & SubSamplingPyramidBuilder<S>::factor() const
  {
    return _factor;
  }

  template <typename S>
  void SubSamplingPyramidBuilder<S>::setFactor( unsigned r )
  {
    _factor.assign( 1, Point4du( r, r, r, r ) );
  }

  template <typename S>
  void SubSamplingPyramidBuilder<S>::setFactor( const Point4du & r )
  {
    _factor.assign( 1, r );
  }

  template <typename S>
  void SubSamplingPyramidBuilder<S>::setFactor( const std::vector<Point4du> & r )
  {
    _factor = r;
  }

  template <typename S>
  void SubSamplingPyramidBuilder<S>::setSubSampler( const SubSampler & f )
  {
    _subsampler = f;
  }

  template <typename S>
  void SubSamplingPyramidBuilder<S>::setDirections( const std::vector<bool> & dir )
  {
    _dir.assign( 4, true );
    _dir[3] = false;
    for( int i = 0; i < dir.size() && i < _dir.size(); ++i )
      _dir[i] = dir[i];
  }

  template <typename S>
  void SubSamplingPyramidBuilder<S>::setDirections( bool dirx, bool diry, bool dirz, bool dirt )
  {
    _dir[0] = dirx;
    _dir[1] = diry;
    _dir[2] = dirz;
    _dir[3] = dirt;
  }

  template <typename S>
  void SubSamplingPyramidBuilder<S>::setVerbose( int verbose )
  {
    _verbose = verbose;
  }

  template <typename S>
  void SubSamplingPyramidBuilder<S>::setQuiet()
  {
    _verbose = 0;
  }

  //--------------------------------------------------------------------------
  // Execution
  //--------------------------------------------------------------------------

  template <typename S>
  template <typename T>
  std::vector<carto::VolumeRef<T> > SubSamplingPyramidBuilder<S>::execute( const carto::VolumeRef<T> & vol )
  {
    std::vector<Point4du> factors = computeFactors( vol.getSize() );
    std::vector<carto::VolumeRef<T> > pyramid;
    pyramid.reserve( factors.size() + 1 );
    _subsampler.setVerbose( _verbose - 1 );

    // FIRST LEVEL
    if( _verbose > 0 )
      std::cout << "Copy first level... " << std::endl;
    pyramid.push_back( carto::copy(vol) );

    // OTHER LEVELS
    std::vector<Point4du>::iterator f;
    int l = 1;
    for( f = factors.begin(); f != factors.end(); ++f, ++l )
    {
      if( _verbose > 0 )
        std::cout << "Compute subsampled level " << l << "... " << std::endl;

      if( _verbose > 1 )
        std::cout << "Moving average filter/subsampling: " << std::flush;
      _subsampler.setFactor( *f );
    _subsampler.setDirections( _dir[0] && (*f)[0] > 1,
                               _dir[1] && (*f)[1] > 1,
                               _dir[2] && (*f)[2] > 1,
                               _dir[3] && (*f)[3] > 1 );
      pyramid.push_back( _subsampler.execute<T,T>( pyramid[l-1] ) );
    }

    return pyramid;
  }

  //--------------------------------------------------------------------------
  // Helpers
  //--------------------------------------------------------------------------
  template <typename S>
  std::vector<Point4du> SubSamplingPyramidBuilder<S>::computeFactors( const std::vector<int> & size ) const
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

} // namespace aims

#endif // AIMS_PYRAMID_SUBSAMPLINGPYRAMID_D_H
