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

//--- aims -------------------------------------------------------------------
#include <aims/signalfilter/iirfilter.h>
#include <aims/vector/vector.h>                   // Point4dd
#include <aims/utility/progress.h>                // aims::Progression
//--- cartobase --------------------------------------------------------------
#include <cartobase/config/verbose.h>             // carto::verbose
//--- std --------------------------------------------------------------------
#include <cmath>                                  // std::log10/fabs
#include <cstdlib>                                // std::ldiv
#include <limits>                                 // std::numeric_limits
#include <iostream>                               // std::cout/endl
#include <string>
//----------------------------------------------------------------------------

using namespace carto;
using namespace std;

namespace aims {

  //==========================================================================
  //
  //   BASE CLASS FOR INFINITE IMPULSE RESPONSE FILTER
  //
  //==========================================================================

  //--------------------------------------------------------------------------
  //   CONSTRUCTOR / DESTRUCTOR
  //--------------------------------------------------------------------------

  IIRFilterBase::IIRFilterBase():
    _verbose(verbose),
    _dir(4, true),
    _boundary(FilterType::Mirror),
    _copy(FilterType::Copy)
  {
    _dir[3] = false;
  }

  IIRFilterBase::IIRFilterBase( const IIRFilterBase & other ):
    _verbose(other._verbose),
    _dir(other._dir),
    _boundary( other._boundary ),
    _copy(other._copy)
  {}

  IIRFilterBase::~IIRFilterBase()
  {}

  IIRFilterBase & IIRFilterBase::operator=( const IIRFilterBase & other )
  {
    if( this != &other )
    {
      _boundary = other._boundary;
      _verbose = other._verbose;
      _dir = other._dir;
      _copy = other._copy;
    }
    return *this;
  }

  //--------------------------------------------------------------------------
  //   HELPERS
  //--------------------------------------------------------------------------

  inline
  long IIRFilterBase::mirrorCoeff( long i, long size ) const
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
  long IIRFilterBase::selectCoeff( long i, long fullsize, int binf ) const
  {
    return mirrorCoeff( i + binf, fullsize ) - binf;
  }


  inline
  double & IIRFilterBase::at( VolumeRef<double> & vector,
                              long k, int dir ) const
  {
    return vector( dir == 0 ? k : 0,
                   dir == 1 ? k : 0,
                   dir == 2 ? k : 0,
                   dir == 3 ? k : 0 );
  }

  inline
  const double & IIRFilterBase::at( const VolumeRef<double> & vector,
                                    long k,  int dir ) const
  {
    return vector( dir == 0 ? k : 0,
                   dir == 1 ? k : 0,
                   dir == 2 ? k : 0,
                   dir == 3 ? k : 0 );
  }

  inline
  double IIRFilterBase::at( const VolumeRef<double> & vector,
                            long k, int dir,
                            long fullsize, int binf ) const
  {
    if( _boundary == FilterType::Mirror )
      return vector( dir == 0 ? selectCoeff(k, fullsize, binf) : 0,
                     dir == 1 ? selectCoeff(k, fullsize, binf) : 0,
                     dir == 2 ? selectCoeff(k, fullsize, binf) : 0,
                     dir == 3 ? selectCoeff(k, fullsize, binf) : 0 );

    else if( _boundary == FilterType::Zero )
      return ( -binf <= k && k < fullsize
               ? vector( dir == 0 ? k : 0,
                         dir == 1 ? k : 0,
                         dir == 2 ? k : 0,
                         dir == 3 ? k : 0 )
               : 0. );

    else if( _boundary == FilterType::Nearest )
    {
      if( k < -binf )
        k = -binf;
      else if( k >= fullsize )
        k = fullsize;
      return vector( dir == 0 ? k : 0,
                     dir == 1 ? k : 0,
                     dir == 2 ? k : 0,
                     dir == 3 ? k : 0 );
    }
    else
      throw runtime_error("IIRFilterBase::at, unknown boundary type");

  }

  long IIRFilterBase::computeK0( double pole, double tolerance ) const
  {
    if( tolerance < 0 )
      tolerance = std::log10( numeric_limits<double>::epsilon() );
    return (long)std::ceil( tolerance / std::log10( std::fabs( pole ) ) );
  }

  //--------------------------------------------------------------------------
  //   FUNCTION CALL
  //--------------------------------------------------------------------------

  carto::VolumeRef<double> IIRFilterBase::execute( carto::VolumeRef<double> & in, bool inplace ) const
  {
    carto::VolumeRef<double> tmp = ( inplace ? in : in.deepcopy() );
    filter( tmp );
    return tmp;
  }

  template <typename T>
  carto::VolumeRef<double> IIRFilterBase::execute( const carto::VolumeRef<T> & in ) const
  {
    carto::VolumeRef<double> tmp = in;
    return execute( tmp, true );
  }

  //--------------------------------------------------------------------------
  //   FILTERING
  //--------------------------------------------------------------------------

  void IIRFilterBase::filter( carto::VolumeRef<double> vol ) const
  {
    if( _verbose > 1 )
      std::cout << ( _dir[0] ? "X " : "" ) << ( _dir[1] ? "Y " : "" )
                << ( _dir[2] ? "Z " : "" ) << ( _dir[3] ? "T " : "" )
                << std::flush;

    aims::Progression prg( ( vol.getSizeX() > 1 && _dir[0] ? vol.getSizeT() * vol.getSizeZ() * vol.getSizeY() : 0L ) +
                           ( vol.getSizeY() > 1 && _dir[1] ? vol.getSizeT() * vol.getSizeZ() * vol.getSizeX() : 0L ) +
                           ( vol.getSizeZ() > 1 && _dir[2] ? vol.getSizeT() * vol.getSizeY() * vol.getSizeX() : 0L ) +
                           ( vol.getSizeT() > 1 && _dir[3] ? vol.getSizeZ() * vol.getSizeY() * vol.getSizeX() : 0L ) );
    if( _verbose > 0 )
      prg.print();

    carto::VolumeRef<double> tmp( (carto::Volume<double> *)0 );

    if( vol.getSizeX() > 1 && _dir[0] )
    {
      if( _copy == FilterType::Copy )
        tmp = carto::VolumeRef<double>( vol.getSizeX(), 1, 1, 1 );
      for( long t = 0; t < vol.getSizeT(); t++ )
      for( long z = 0; z < vol.getSizeZ(); z++ )
      for( long y = 0; y < vol.getSizeY(); y++ )
      {
        if( _verbose > 0 )
          (++prg).print();
        filter1d( vol.view( Point4dl( 0, y, z, t ), Point4dl( -1, 1, 1, 1 ) ), 0, tmp );
      }
    }

    if( vol.getSizeY() > 1 && _dir[1] )
    {
      if( _copy == FilterType::Copy )
        tmp = carto::VolumeRef<double>( 1, vol.getSizeY(), 1, 1 );
      for( long t = 0; t < vol.getSizeT(); t++ )
      for( long z = 0; z < vol.getSizeZ(); z++ )
      for( long x = 0; x < vol.getSizeX(); x++ )
      {
        if( _verbose > 0 )
          (++prg).print();
        filter1d( vol.view( Point4dl( x, 0, z, t ), Point4dl( 1, -1, 1, 1 ) ), 1, tmp );
      }
    }

    if( vol.getSizeZ() > 1 && _dir[2] )
    {
      if( _copy == FilterType::Copy )
        tmp = carto::VolumeRef<double>( 1, 1, vol.getSizeZ(), 1 );
      for( long t = 0; t < vol.getSizeT(); t++ )
      for( long y = 0; y < vol.getSizeY(); y++ )
      for( long x = 0; x < vol.getSizeX(); x++ )
      {
        if( _verbose > 0 )
          (++prg).print();
        filter1d( vol.view( Point4dl( x, y, 0, t ), Point4dl( 1, 1, -1, 1 ) ), 2, tmp );
      }
    }

    if( vol.getSizeT() > 1 && _dir[3] )
    {
      if( _copy == FilterType::Copy )
        tmp = carto::VolumeRef<double>( 1, 1, 1, vol.getSizeT() );
      for( long z = 0; z < vol.getSizeZ(); z++ )
      for( long y = 0; y < vol.getSizeY(); y++ )
      for( long x = 0; x < vol.getSizeX(); x++ )
      {
        if( _verbose > 0 )
          (++prg).print();
        filter1d( vol.view( Point4dl( x, y, z, 0 ), Point4dl( 1, 1, 1, -1 ) ), 3, tmp );
      }
    }

    if( _verbose > 0 )
      std::cout << std::endl;

  }

  inline
  void IIRFilterBase::filter1d( carto::VolumeRef<double> y, int dir, carto::VolumeRef<double> tmp ) const
  {
    if( _copy == FilterType::InPlace )
      filter1d( y, y, dir );
    else if( _copy == FilterType::Copy ) {
      filter1d( y, tmp );
      carto::transfer( tmp, y );
    }
  }

  //--------------------------------------------------------------------------
  //   PARAMETERS
  //--------------------------------------------------------------------------

  const std::vector<bool> & IIRFilterBase::directions() const
  {
    return _dir;
  }

  const FilterType::Boundary & IIRFilterBase::boundary() const
  {
    return _boundary;
  }

  void IIRFilterBase::setBoundary( const FilterType::Boundary & boundary )
  {
    _boundary = boundary;
  }

  void IIRFilterBase::setDirections( const std::vector<bool> & dir )
  {
    _dir.assign( 4, true );
    _dir[3] = false;
    for( size_t i = 0; (i < dir.size()) && (i < _dir.size()); ++i )
      _dir[i] = dir[i];
  }

  void IIRFilterBase::setDirections( bool dirx, bool diry, bool dirz, bool dirt )
  {
    _dir[0] = dirx;
    _dir[1] = diry;
    _dir[2] = dirz;
    _dir[3] = dirt;
  }

  void IIRFilterBase::setDirectionX( bool dirx )
  {
    _dir[0] = dirx;
  }

  void IIRFilterBase::setDirectionY( bool diry )
  {
    _dir[1] = diry;
  }

  void IIRFilterBase::setDirectionZ( bool dirz )
  {
    _dir[2] = dirz;
  }

  void IIRFilterBase::setDirectionT( bool dirt )
  {
    _dir[3] = dirt;
  }

  void IIRFilterBase::setVerbose( int verbose )
  {
    _verbose = verbose;
  }

  void IIRFilterBase::setQuiet()
  {
    _verbose = 0;
  }

  //--------------------------------------------------------------------------
  //   INSTANTIATE
  //--------------------------------------------------------------------------

  #define iir_instantiate_methods( T ) \
    template VolumeRef<double> IIRFilterBase::execute( const VolumeRef<T> & ) const;

  iir_instantiate_methods( uint8_t )
  iir_instantiate_methods( int8_t )
  iir_instantiate_methods( uint16_t )
  iir_instantiate_methods( int16_t )
  iir_instantiate_methods( uint32_t )
  iir_instantiate_methods( int32_t )
  iir_instantiate_methods( uint64_t )
  iir_instantiate_methods( int64_t )
  iir_instantiate_methods( float )
  iir_instantiate_methods( double )

  //==========================================================================
  //
  //   CAUSAL SINGLE POLE INFINITE IMPULSE RESPONSE FILTER
  //
  //==========================================================================

  //--------------------------------------------------------------------------
  //   CONSTRUCTOR / DESTRUCTOR
  //--------------------------------------------------------------------------

  CausalSinglePoleIIRFilter::CausalSinglePoleIIRFilter( double pole, double gain ):
    IIRFilterBase(),
    _gain(gain),
    _pole(pole),
    _k0()
  {
    computeK0();
    IIRFilterBase::_copy = FilterType::InPlace;
  }

  CausalSinglePoleIIRFilter::CausalSinglePoleIIRFilter( const CausalSinglePoleIIRFilter & other ):
    IIRFilterBase(other),
    _gain(other._gain),
    _pole(other._pole),
    _k0(other._k0)
  {}

  CausalSinglePoleIIRFilter::~CausalSinglePoleIIRFilter()
  {}

  CausalSinglePoleIIRFilter & CausalSinglePoleIIRFilter::operator=( const CausalSinglePoleIIRFilter & other )
  {
    if( this != &other )
    {
      IIRFilterBase::operator= ( other ),
      _gain = other._gain;
      _pole = other._pole;
      _k0 = other._k0;
    }
    return *this;
  }

  //--------------------------------------------------------------------------
  //   HELPERS
  //--------------------------------------------------------------------------

  void CausalSinglePoleIIRFilter::computeK0()
  {
    _k0 = IIRFilterBase::computeK0( _pole );
  }


  //--------------------------------------------------------------------------
  //   FILTERING
  //--------------------------------------------------------------------------

  void CausalSinglePoleIIRFilter::filter1d( const carto::VolumeRef<double> in,
                                            carto::VolumeRef<double> out,
                                            int dir ) const
  {
    if( in.get() != out.get() )
      carto::transfer( in, out );

    if( _gain != 1. )
      out *= _gain;

    long size = (long)in.getSizeT() * (long)in.getSizeZ() *
                (long)in.getSizeY() * (long)in.getSizeX();
    if ( size == 1 )
      return;

    std::vector<int> b = in.getBorders();
    int binf = b[dir*2];
    long fullsize = binf + size + b[dir*2+1];

    // init y[0]
    if( this->_boundary != FilterType::Zero )
    {
      double pow = _pole;
      for( long k = 1; k < _k0; ++k, pow *= _pole )
        at(out, 0, dir) += pow * at(in, -k, dir, fullsize, binf);
    }

    // forward sweep
    for( long k = 1; k < size; ++k )
      at(out, k, dir) += _pole * at(out, k-1, dir);
  }

  //--------------------------------------------------------------------------
  //   PARAMETERS
  //--------------------------------------------------------------------------

  const double & CausalSinglePoleIIRFilter::gain() const
  {
    return _gain;
  }

  const double & CausalSinglePoleIIRFilter::pole() const
  {
    return _pole;
  }

  void CausalSinglePoleIIRFilter::setPole( const double & pole )
  {
    _pole = pole;
    computeK0();
  }

  void CausalSinglePoleIIRFilter::setGain( const double & gain )
  {
    _gain = gain;
  }

  // Commenting this out.
  // See iirfilter.h for explanations
  #if 0
  //==========================================================================
  //
  //   ALL POLE CAUSAL INFINITE IMPULSE RESPONSE FILTER
  //
  //==========================================================================

  //--------------------------------------------------------------------------
  //   CONSTRUCTOR / DESTRUCTOR
  //--------------------------------------------------------------------------

  CausalAllPoleIIRFilter::CausalAllPoleIIRFilter( double coeff, const double gain, bool is_pole ):
    IIRFilterBase(),
    _gain(gain),
    _coeff(1, coeff),
    _k0(),
    _is_pole(is_pole)
  {
    computeK0();
    IIRFilterBase::_copy = FilterType::InPlace;
  }

  CausalAllPoleIIRFilter::CausalAllPoleIIRFilter( const vector<double> & coeffs, const double gain, bool is_pole ):
    IIRFilterBase(),
    _gain(gain),
    _coeff(coeffs),
    _k0(),
    _is_pole(is_pole)
  {
    computeK0();
  }

  CausalAllPoleIIRFilter::CausalAllPoleIIRFilter( const CausalAllPoleIIRFilter & other ):
    IIRFilterBase(other),
    _gain(other._gain),
    _coeff(other._coeff),
    _is_pole(other._is_pole),
    _k0(other._k0)
  {}

  CausalAllPoleIIRFilter::~CausalAllPoleIIRFilter()
  {}

  CausalAllPoleIIRFilter & CausalAllPoleIIRFilter::operator=( const CausalAllPoleIIRFilter & other )
  {
    if( this != &other )
    {
      IIRFilterBase::operator= ( other ),
      _gain = other._gain;
      _coeff = other._coeff;
      _k0 = other._k0;
      _is_pole = other._is_pole;
    }
    return *this;
  }

  //--------------------------------------------------------------------------
  //   HELPERS
  //--------------------------------------------------------------------------

  void CausalAllPoleIIRFilter::computeK0()
  {
    _k0.clear();
    double tolerance = std::log10( numeric_limits<double>::epsilon() );
    std::vector<double>::const_iterator coeff;

    if( _is_pole )
    {
      _k0.reserve( _coeff.size() );
      for( coeff = _coeff.begin(); coeff != _coeff.end(); ++coeff )
        _k0.push_back( IIRFilterBase::computeK0( *coeff, tolerance ) );
    }
    else
    {
      _k0.assign(1, 0);
      for( coeff = _coeff.begin(); coeff != _coeff.end(); ++coeff )
        _k0[0] = std::max( _k0[0], IIRFilterBase::computeK0( *coeff, tolerance ) );
    }

  }


  //--------------------------------------------------------------------------
  //   FILTERING
  //--------------------------------------------------------------------------

  void CausalAllPoleIIRFilter::filter1d( const carto::VolumeRef<double> in,
                                         carto::VolumeRef<double> out,
                                         int dir ) const
  {
    if( in.get() != out.get() )
      carto::transfer( in, out );

    out *= _gain;

    long size = (long)in.getSizeT() * (long)in.getSizeZ() *
                (long)in.getSizeY() * (long)in.getSizeX();
    if ( size == 1 )
      return;

    std::vector<int> b = in.getBorders();
    int binf = b[dir*2];
    long fullsize = binf + size + b[dir*2+1];

    if( _is_pole )
    {
      std::vector<double>::const_reverse_iterator pole;
      std::vector<long>::const_reverse_iterator k0;

      for( pole = _coeff.rbegin(), k0 = _k0.rbegin();
           pole != _coeff.rend(); ++pole, ++k0 )
      {
        // init y[0]
        if( this->_boundary != Zero )
        {
          double pow = *pole;
          for( long k = 1; k < *k0; ++k, pow *= *pole )
            at(out, 0, dir) += pow * at(in, -k, dir, fullsize, binf);
        }

        // forward sweep
        for( long k = 1; k < size; ++k )
          at(out, k, dir) += *pole * at(out, k-1, dir);
      }

    }
    else
    {
      // init y[0]
      if( this->_boundary != Zero )
      {
        std::vector<double> pow( _coeff.size(), 0 );
        for( long k = 1; k < _k0[0]; ++k )
        {
          double sum = 0.;
          for( long p = 0; p < pow.size(); ++p )
          {
            if( k-1 == p )
              pow[p] = _coeff[p];
            else if( k-1 > p )
              pow[p] *= _coeff[p];
            sum += pow[p];
          }
          at(out, 0, dir) += sum * at(in, -k, dir, fullsize, binf);
        }
      }

      // WARNING 
      // Ce que je fais est faux !

      // forward sweep with mirroring
      for( long k = 1; k < _coeff.size(); ++k )
        for( long p = 0; p < _coeff.size(); ++p )
          if( k < p+1 )
            at(out, k, dir) += _coeff[p] * at(in, k-1-p, dir, fullsize, binf);
          else
            at(out, k, dir) += _coeff[p] * at(out, k-1-p, dir);

      // forward sweep
      for( long k = _coeff.size(); k < size; ++k )
        for( long p = 0; p < _coeff.size(); ++p )
          at(out, k, dir) += _coeff[p] * at(out, k-1-p, dir);

    }

  }

  //--------------------------------------------------------------------------
  //   PARAMETERS
  //--------------------------------------------------------------------------

  const double & CausalAllPoleIIRFilter::gain() const
  {
    return _gain;
  }

  const std::vector<double> & CausalAllPoleIIRFilter::poles() const
  {
    return _coeff;
  }

  const std::vector<double> & CausalAllPoleIIRFilter::coeffs() const
  {
    return _coeff;
  }

  void CausalAllPoleIIRFilter::setPole( const double & pole, bool is_pole )
  {
    _coeff.assign( 1, pole );
    _is_pole = is_pole;
    this->computeK0();
  }

  void CausalAllPoleIIRFilter::setPoles( const std::vector<double> & poles,
                                         bool is_pole )
  {
    _coeff = poles;
    _is_pole = is_pole;
    this->computeK0();
  }

  void CausalAllPoleIIRFilter::setCoeff( const double & coeff, bool is_pole )
  {
    _coeff.assign( 1, coeff );
    _is_pole = is_pole;
    this->computeK0();
  }

  void CausalAllPoleIIRFilter::setCoeffs( const std::vector<double> & coeffs,
                                         bool is_pole )
  {
    _coeff = coeffs;
    _is_pole = is_pole;
    this->computeK0();
  }

  void CausalAllPoleIIRFilter::setGain( const double & gain )
  {
    _gain = gain;
  }

  //==========================================================================
  //
  //   CAUSAL INFINITE IMPULSE RESPONSE FILTER
  //
  //==========================================================================

  //--------------------------------------------------------------------------
  //   CONSTRUCTOR / DESTRUCTOR
  //--------------------------------------------------------------------------

  CausalIIRFilter::CausalIIRFilter( const vector<double> & num_coeffs,
                                    const vector<double> & den_coeffs,
                                    const double b0 ):
    IIRFilterBase(),
    _b0(b0),
    _num_coeff(_num_coeff),
    _den_coeff(_den_coeff)
  {}

  CausalIIRFilter::CausalIIRFilter( const CausalIIRFilter & other ):
    IIRFilterBase(other),
    _b0(other._b0),
    _num_coeff(other._num_coeff),
    _den_coeff(other._den_coeff)
  {}

  CausalIIRFilter::~CausalIIRFilter()
  {}

  CausalIIRFilter & CausalIIRFilter::operator=( const CausalIIRFilter & other )
  {
    if( this != &other )
    {
      IIRFilterBase::operator= ( other );
      _b0 = other._b0;
      _num_coeff = other._num_coeff;
      _den_coeff = other._den_coeff;
    }
    return *this;
  }

  //--------------------------------------------------------------------------
  //   FILTERING
  //--------------------------------------------------------------------------

  inline
  double CausalIIRFilter::recursiveHelper( const carto::VolumeRef<double> in,
                                           long k, int dir, long fullsize, int binf ) const
  {
    double result = _b0 * at( in, k, dir, fullsize, binf );
    double comp;
    for( int p = 0; p < _num_coeff.size(); ++p )
      result += _num_coeff[p] * at( in, k-p-1, dir, fullsize, binf );
    for( int p = 0; p < _den_coeff.size(); ++p ) {
      comp = _den_coeff[p] * recursiveHelper( in, k-p-1, dir, fullsize, binf );
      if( comp > std::numeric_limits<double>::epsilon() )
        result += _den_coeff[p] * recursiveHelper( in, k-p-1, dir, fullsize, binf );
    }
    return result;
  }

  // inline
  // void CausalIIRFilter::recursiveHelper( const carto::VolumeRef<double> in,
  //                                        double & out,
  //                                        std::vector<double> & cx,
  //                                        double gain;
  //                                        long k ) const
  // {
  //   cx[0] += gain * _b0;
  //   for( int p = 1; p < cx.size(); ++p )
  //     cx[ p ] += gain * _num_coeff[p-1];

  //   if( std::abs(cx[0]) < std::numeric_limits<double>::epsilon() )
  //   {
  //     out += cx[0] * at( in, k, fullsize, binf );
  //     cx.assign( ++cx.begin(), cx.end() );
  //     cx.back() = 0.;
  //     for( int q = 0; q < _den_coeff.size(); ++q )
  //       recursiveHelper( in, out, cx, _den_coeff[q], k-1 );
  //   }
  // }

  void CausalIIRFilter::filter1d( const carto::VolumeRef<double> in,
                                  carto::VolumeRef<double> out,
                                  int dir ) const
  {
    out.fill(0.);

    long size = (long)in.getSizeT() * (long)in.getSizeZ() *
                (long)in.getSizeY() * (long)in.getSizeX();
    if ( size == 1 )
      return;

    std::vector<int> b = in.getBorders();
    int binf = b[dir*2];
    long fullsize = binf + size + b[dir*2+1];


    // init y[0]
    // std::vector<double> cx( _num_coeff.size() + 1, 0. );
    for( int p = 0; p < _den_coeff.size(); ++p )
      at( out, p, dir ) = recursiveHelper( in, p, dir, fullsize, binf );

    // forward sweep
    for( long k = _den_coeff.size(); k < size; ++k )
    {
      at(out, k, dir) = _b0 * at(in, k, dir);
      for( int p = 0; p < _num_coeff.size(); ++p )
        at(out, k, dir) += _num_coeff[p] * at(in, k-p-1, dir);
      for( int p = 0; p < _den_coeff.size(); ++p )
        at(out, k, dir) += _den_coeff[p] * at(out, k-p-1, dir);
    }

  }

  //--------------------------------------------------------------------------
  //   PARAMETERS
  //--------------------------------------------------------------------------

  const double & CausalIIRFilter::b0() const
  {
    return _b0;
  }

  const std::vector<double> & CausalIIRFilter::numCoeffs() const
  {
    return _num_coeff;
  }

  const std::vector<double> & CausalIIRFilter::denCoeffs() const
  {
    return _den_coeff;
  }

  void CausalIIRFilter::setNumCoeff( const double & coeff )
  {
    _num_coeff.assign( 1, coeff );
  }

  void CausalIIRFilter::setNumCoeffs( const std::vector<double> & coeffs )
  {
    _num_coeff = coeffs;
  }

  void CausalIIRFilter::setDenCoeff( const double & coeff )
  {
    _den_coeff.assign( 1, coeff );
  }

  void CausalIIRFilter::setDenCoeffs( const std::vector<double> & coeffs )
  {
    _den_coeff = coeffs;
  }

  void CausalIIRFilter::setB0( const double & b0 )
  {
    _b0 = b0;
  }


  //==========================================================================
  //
  //   WARPER FOR ANTICAUSAL FILTERS
  //
  //==========================================================================

  //--------------------------------------------------------------------------
  //   CONSTRUCTOR / DESTRUCTOR
  //--------------------------------------------------------------------------

  template <typename CausalFilter>
  AntiCausalWarperIIRFilter<CausalFilter>::AntiCausalWarperIIRFilter( const CausalFilter & other ):
    CausalFilter(other)
  {}

  template <typename CausalFilter>
  AntiCausalWarperIIRFilter<CausalFilter>::AntiCausalWarperIIRFilter( const AntiCausalWarperIIRFilter & other ):
    CausalFilter(other)
  {}

  template <typename CausalFilter>
  AntiCausalWarperIIRFilter<CausalFilter>::~AntiCausalWarperIIRFilter()
  {}

  template <typename CausalFilter>
  AntiCausalWarperIIRFilter<CausalFilter> & AntiCausalWarperIIRFilter<CausalFilter>::operator=( const AntiCausalWarperIIRFilter<CausalFilter> & other )
  {
    if( this != &other )
      CausalFilter::operator=( other );
    return *this;
  }

  //--------------------------------------------------------------------------
  //   HELPER
  //--------------------------------------------------------------------------

  template <typename CausalFilter>
  double & AntiCausalWarperIIRFilter<CausalFilter>::at( carto::VolumeRef<double> & vector, long k, int dir ) const
  {
    long size = vector.getSize()[dir];
    return CausalFilter::at( vector, size - 1 - k, dir );
  }

  template <typename CausalFilter>
  const double & AntiCausalWarperIIRFilter<CausalFilter>::at( const carto::VolumeRef<double> & vector, long k, int dir ) const
  {
    long size = vector.getSize()[dir];
    return CausalFilter::at( vector, size - 1 - k, dir );
  }


  template <typename CausalFilter>
  double AntiCausalWarperIIRFilter<CausalFilter>::at( const carto::VolumeRef<double> & vector, long k,
                                                int dir, long fullsize, int binf ) const
  {
    long size = vector.getSize()[dir];
    return CausalFilter::at( vector, size - 1 - k, dir, fullsize, binf );
  }

  //--------------------------------------------------------------------------
  //   INSTANTIATE
  //--------------------------------------------------------------------------

  template class AntiCausalWarperIIRFilter<CausalIIRFilter>;
  template class AntiCausalWarperIIRFilter<CausalAllPoleIIRFilter>;
  template class AntiCausalWarperIIRFilter<CausalSinglePoleIIRFilter>;
  #endif

  //==========================================================================
  //
  //   SYMMETRIC ALL-POLE INFINITE IMPULSE RESPONSE FILTER
  //
  //==========================================================================

  //--------------------------------------------------------------------------
  //   CONSTRUCTOR / DESTRUCTOR
  //--------------------------------------------------------------------------

  SymAllPoleIIRFilter::SymAllPoleIIRFilter( const double gain, const vector<double> & poles ):
    IIRFilterBase(),
    _poles(poles),
    _k0(),
    _gain(gain),
    _precisegain(),
    _precise(false)
  {
    computeK0();
    computePreciseGain();
    IIRFilterBase::_copy = FilterType::InPlace;
  }

  SymAllPoleIIRFilter::SymAllPoleIIRFilter( const SymAllPoleIIRFilter & other ):
    IIRFilterBase(other),
    _poles(other._poles),
    _k0(other._k0),
    _gain(other._gain),
    _precisegain(other._precisegain),
    _precise(other._precise)
  {}

  SymAllPoleIIRFilter::~SymAllPoleIIRFilter()
  {}

  SymAllPoleIIRFilter & SymAllPoleIIRFilter::operator=( const SymAllPoleIIRFilter & other )
  {
    if( this != &other )
    {
      IIRFilterBase::operator= ( other ),
      _gain = other._gain;
      _poles = other._poles;
      _k0 = other._k0;
      _precisegain = other._precisegain;
      _precise = other._precise;
    }
    return *this;
  }

  //--------------------------------------------------------------------------
  //   HELPERS
  //--------------------------------------------------------------------------

  void SymAllPoleIIRFilter::computeK0()
  {
    _k0.clear();
    _k0.reserve( _poles.size() );
    double tolerance = std::log10( numeric_limits<double>::epsilon() );
    std::vector<double>::const_iterator pole;

    for( pole = _poles.begin(); pole != _poles.end(); ++pole )
      _k0.push_back( IIRFilterBase::computeK0( *pole, tolerance ) );
  }

  void SymAllPoleIIRFilter::computePreciseGain()
  {
    _precisegain = _gain;
    if( !_precise )
    {
      std::vector<double>::const_iterator pole;
      for( pole = _poles.begin(); pole != _poles.end(); ++pole )
        _precisegain *= - *pole;
    }
  }

  //--------------------------------------------------------------------------
  //   FILTERING
  //--------------------------------------------------------------------------

  void SymAllPoleIIRFilter::filter1d( const carto::VolumeRef<double> in,
                                         carto::VolumeRef<double> out,
                                         int dir ) const
  {
    std::vector<double>::const_reverse_iterator pole;
    std::vector<long>::const_reverse_iterator k0;

    if( in.get() != out.get() )
      carto::transfer( in, out );

    out *= _precisegain;

    long size = (long)in.getSizeT() * (long)in.getSizeZ() *
                (long)in.getSizeY() * (long)in.getSizeX();
    if ( size == 1 )
      return;

    std::vector<int> b = in.getBorders();
    int binf = b[dir*2];
    long fullsize = binf + size + b[dir*2+1];

    for( pole = _poles.rbegin(), k0 = _k0.rbegin();
         pole != _poles.rend(); ++pole, ++k0 )
    {
      // init y[0]
      if( this->_boundary != FilterType::Zero )
      {
        double pow = *pole;
        for( long k = 1; k < *k0; ++k, pow *= *pole )
          at(out, 0, dir) += pow * at(in, -k, dir, fullsize, binf);
      }

      // forward sweep
      for( long k = 1; k < size; ++k )
        at(out, k, dir) += *pole * at(out, k-1, dir);

      // reinit y[last]
      at(out, size-1, dir) += *pole * at(out, size-2, dir);
      at(out, size-1, dir) /= 1. - *pole * *pole;

      // backward sweep
      for( long k = size - 2; k >= 0; --k ) {
        at(out, k, dir) += *pole * at(out, k+1, dir);
      }
    }

  }

  //--------------------------------------------------------------------------
  //   PARAMETERS
  //--------------------------------------------------------------------------

  const double & SymAllPoleIIRFilter::gain() const
  {
    return _gain;
  }

  const std::vector<double> & SymAllPoleIIRFilter::poles() const
  {
    return _poles;
  }

  void SymAllPoleIIRFilter::setPoles( const std::vector<double> & poles )
  {
    _poles = poles;
    computeK0();
    computePreciseGain();
  }

  void SymAllPoleIIRFilter::addPole( const double & pole )
  {
    _poles.push_back( pole );
    computeK0();
    computePreciseGain();
  }

  void SymAllPoleIIRFilter::setGain( const double & gain )
  {
    _gain = gain;
    computePreciseGain();
  }

  void SymAllPoleIIRFilter::setModePreciseGain( bool precise )
  {
    _precise = precise;
    computePreciseGain();
  }

} // namespace aims
