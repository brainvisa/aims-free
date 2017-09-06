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

#ifndef AIMS_SIGNALFILTER_CONVOLUTIONFILTER_D_H
#define AIMS_SIGNALFILTER_CONVOLUTIONFILTER_D_H

#include <aims/signalfilter/convolutionfilter.h>
#include <aims/utility/progress.h>
#include <aims/vector/vector.h>
#include <cartobase/config/verbose.h>

namespace aims {

  //==========================================================================
  // CONSTRUCTOR/DESTRUCTOR/COPY
  //==========================================================================

  template <typename F>
  ConvolutionFilter<F>::ConvolutionFilter( const F & f ):
    _func( std::vector<F>( 1, f ) ),
    _verbose( carto::verbose),
    _dir( 4, true )
  {
    _dir[3] = false;
  }

  template <typename F>
  ConvolutionFilter<F>::ConvolutionFilter( const std::vector<F> & f ):
    _func( f ),
    _verbose( carto::verbose),
    _dir( 4, true )
  {
    _dir[3] = false;
  }

  template <typename F>
  ConvolutionFilter<F>::ConvolutionFilter( const ConvolutionFilter<F> & other ):
    _func( other._func ),
    _verbose( other._verbose ),
    _dir( other._dir )
  {}

  template <typename F>
  ConvolutionFilter<F>::~ConvolutionFilter()
  {}

  template <typename F>
  ConvolutionFilter<F> & ConvolutionFilter<F>::operator=( const ConvolutionFilter<F> & other )
  {
    if( this != &other )
    {
      _func = other._func;
      _verbose = other._verbose;
      _dir = other._dir;
    }
    return *this;
  }

  //==========================================================================
  // HELPERS
  //==========================================================================

  namespace convolutionfilter {

    inline
    long mirrorCoeff( long i, long size )
    {
      i = std::abs( i );

      if ( i < size )
        return i;
      if ( size == 1 )
        return 0;

      long size2 = ( size << 1 ) - 2;
      ldiv_t modOp = std::ldiv( i, size2 );
      return ( modOp.rem < size ) ? modOp.rem : ( size2 - modOp.rem );
    }

    inline
    long selectCoeff( long i, long fullsize, long binf )
    {
      return mirrorCoeff( i + binf, fullsize ) - binf;
    }

    template <typename T>
    inline
    const T & at( const carto::VolumeRef<T> & vol,
                  const Point4dl & p,
                  const Point4dl & fullsize,
                  const Point4dl & binf )
    {
      return vol( selectCoeff( p[0], fullsize[0], binf[0] ),
                  selectCoeff( p[1], fullsize[1], binf[1] ),
                  selectCoeff( p[2], fullsize[2], binf[2] ),
                  selectCoeff( p[3], fullsize[3], binf[3] ) );
    }

  } // namespace convolutionfilter

  //==========================================================================
  // EXECUTION
  //==========================================================================

  template <typename F>
  template <typename OUT, typename IN>
  carto::VolumeRef<OUT> ConvolutionFilter<F>::execute( const carto::VolumeRef<IN> & in ) const
  {
    carto::VolumeRef<OUT> out = carto::copyStructure<OUT,IN>( in );
    return execute( in, out );
  }

  template <typename F>
  template <typename OUT, typename IN>
  carto::VolumeRef<OUT> ConvolutionFilter<F>::execute( const carto::VolumeRef<IN> & in, carto::VolumeRef<OUT> & out ) const
  {
    carto::VolumeRef<IN> tmp = in;

    std::vector<int> b = tmp.getBorders();
    std::vector<int> s = tmp.getSize();
    Point4dl binf( b[0], b[2], b[4], b[6] );
    Point4dl fullsize( s[0] + b[0] + b[1], s[1] + b[2] + b[3],
                       s[2] + b[4] + b[5], s[3] + b[6] + b[7] );

    const F & fx = _func[0];
    const F & fy = ( _func.size() > 1 ? _func[1] : _func[0] );
    const F & fz = ( _func.size() > 2 ? _func[2] : _func[0] );
    const F & ft = ( _func.size() > 3 ? _func[3] : _func[0] );

    bool doT = tmp.getSizeT() > 1 && _dir[3];
    bool doZ = tmp.getSizeZ() > 1 && _dir[2];
    bool doY = tmp.getSizeY() > 1 && _dir[1];
    bool doX = tmp.getSizeX() > 1 && _dir[0];

    long linf = ( doT ? (long)ft.support()[0] : 0 );
    long lsup = ( doT ? (long)ft.support()[1] : 0 );
    long kinf = ( doZ ? (long)fz.support()[0] : 0 );
    long ksup = ( doZ ? (long)fz.support()[1] : 0 );
    long jinf = ( doY ? (long)fy.support()[0] : 0 );
    long jsup = ( doY ? (long)fy.support()[1] : 0 );
    long iinf = ( doX ? (long)fx.support()[0] : 0 );
    long isup = ( doX ? (long)fx.support()[1] : 0 );

    if( _verbose > 1 )
    {
      std::cout << ( _dir[0] ? std::string("X (") + carto::toString(isup - iinf + 1) + std::string(") ") : "" )
                << ( _dir[1] ? std::string("Y (") + carto::toString(jsup - jinf + 1) + std::string(") ") : "" )
                << ( _dir[2] ? std::string("Z (") + carto::toString(ksup - kinf + 1) + std::string(") ") : "" )
                << ( _dir[3] ? std::string("T (") + carto::toString(lsup - linf + 1) + std::string(") ") : "" )
                << std::flush;
    }


    Progression prg( (long)out.getSizeT() * (long)out.getSizeZ() *
                     (long)out.getSizeY() * (long)out.getSizeX() );
    if( _verbose > 0 )
      prg.print();

    for( long t = 0; t < out.getSizeT(); ++t )
    for( long z = 0; z < out.getSizeZ(); ++z )
    for( long y = 0; y < out.getSizeY(); ++y )
    for( long x = 0; x < out.getSizeX(); ++x )
    {
      if( _verbose > 0 )
        (++prg).print();

      double value = 0;
      for( long l = linf; l <= lsup; ++l )
      for( long k = kinf; k <= ksup; ++k )
      for( long j = jinf; j <= jsup; ++j )
      for( long i = iinf; i <= isup; ++i )
      {
        Point4dl p( x+i, y+j, z+k, t+l );
        double ct = doT ? ft( l ) : 1.;
        double cz = doZ ? fz( k ) : 1.;
        double cy = doY ? fy( j ) : 1.;
        double cx = doX ? fx( i ) : 1.;
        value += double( convolutionfilter::at( tmp, p, fullsize, binf ) )
                 * cx * cy * cz * ct;
      }
      out( x, y, z, t ) = value;
    }

    if( _verbose > 0 )
      std::cout << std::endl;

    return out;
  }

  //==========================================================================
  // PARAMETERS
  //==========================================================================

  template <typename F>
  const std::vector<bool> & ConvolutionFilter<F>::directions() const
  {
    return _dir;
  }

  template <typename F>
  void ConvolutionFilter<F>::setBasisFunction( const F & func )
  {
    _func.assign( 4, func );
  }

  template <typename F>
  void ConvolutionFilter<F>::setBasisFunction( const std::vector<F> & func )
  {
    _func = func;
  }

  template <typename F>
  void ConvolutionFilter<F>::setDirections( const std::vector<bool> & dir )
  {
    _dir.assign( 4, true );
    _dir[3] = false;
    for( int i = 0; i < dir.size() && i < _dir.size(); ++i )
      _dir[i] = dir[i];
  }

  template <typename F>
  void ConvolutionFilter<F>::setDirections( bool dirx, bool diry, bool dirz, bool dirt )
  {
    _dir[0] = dirx;
    _dir[1] = diry;
    _dir[2] = dirz;
    _dir[3] = dirt;
  }

  template <typename F>
  void ConvolutionFilter<F>::setVerbose( int verbose )
  {
    _verbose = verbose;
  }

  template <typename F>
  void ConvolutionFilter<F>::setQuiet()
  {
    _verbose = 0;
  }

} // namespace aims

#endif // AIMS_SIGNALFILTER_CONVOLUTIONFILTER_D_H
