/* Copyright (c) 1995-2005 CEA
 *
 *  This software and supporting documentation were developed by
 *      CEA/DSV/SHFJ
 *      4 place du General Leclerc
 *      91401 Orsay cedex
 *      France
 *
 * This software is governed by the CeCILL license version 2 under 
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the 
 * terms of the CeCILL license version 2 as circulated by CEA, CNRS
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
 * knowledge of the CeCILL license version 2 and that you accept its terms.
 */

#ifndef CARTOBASE_TYPE_CONVERTER_H
#define CARTOBASE_TYPE_CONVERTER_H

#include <cartobase/config/cartobase_config.h>
#include <cartobase/type/types.h>
#include <cartobase/type/limits.h>

#define CARTO_SPECIALIZED_RAWCONVERTER_ROUNDED_CONVERT( IN , OUT ) \
template<> \
inline \
void RawConverter< IN , OUT >::convert( const IN &in, OUT & out ) const \
{ \
  out = ( OUT )rint( in ); \
} \

namespace carto
{
  /// Low level rescaler info used for rescaling
  class RescalerInfo
  {
  public:
    RescalerInfo()
      : usevtypelimits(false),
        vmin( std::numeric_limits<double>::quiet_NaN() ),
        vmax( std::numeric_limits<double>::quiet_NaN() ),
        omin( std::numeric_limits<double>::quiet_NaN() ),
        omax( std::numeric_limits<double>::quiet_NaN() )
    {}

    bool    usevtypelimits; 
    double  vmin;
    double  vmax;
    double  omin;
    double  omax;
  };

  /// Low-level raw converter (not rescaling)
  template<typename INP, typename OUTP>
  class RawConverter
  {
  public:
    void convert( const INP &in, OUTP & out ) const;
  };

  // Conversion with Void
  template <typename OUTP>
  class RawConverter<Void, OUTP>
  {
  public:
    void convert( const Void &, OUTP & out ) const
    {
      out = OUTP( 1 );
    }
  };


  template <typename INP>
  class RawConverter<INP, Void>
  {
  public:
    void convert( const INP &, Void & ) const
    {
    }
  };


  template<> 
  class RawConverter<Void, Void>
  {
    public:
    void convert( const Void &, Void & ) const
    {
    }
  };

  template<typename INP, typename OUTP>
  class DefaultedRescalerInfo
  {
  public:
    DefaultedRescalerInfo( const RescalerInfo & info );

    double getScale();
    OUTP getScaledValue( INP value );

    void setvmin( double vmin );
    void setvmax( double vmax );
    void setomin( double omin );
    void setomax( double omax );

  private:
    DefaultedRescalerInfo();
    double getscale();
    INP _defaultedvmin;
    INP _defaultedvmax;
    OUTP _defaultedomin;
    OUTP _defaultedomax;

    const RescalerInfo & _info;
    double _scale;
  };

  /// Low-level rescaling converter
  template<typename INP, typename OUTP>
  class Rescaler
  {
  public:
    Rescaler();
    Rescaler( const RescalerInfo & info );
    void convert( const INP &in, OUTP & out ) const;

  private:
    RescalerInfo _info;
  };


  /** This converter is a bit higher-level than RawConverter and Rescaler, 
      it switches to the one or the other depending on the bool template 
      parameter: still not very user-friendly */
  template<typename INP, typename OUTP, bool MODE>
  class ConverterSwitch
  {
  public:
    void convert( const INP &in, OUTP & out ) const;
  };


  /** This allocator is used by the top-level Converter to allocate a new 
      object depending on the input: it is responsible for dimensioning 
      the output object (ie image dimensions etc) */
  template<typename INP, typename OUTP>
  class ConverterAllocator
  {
  public:
    static OUTP* alloc( const INP &in );
  };


  /// Specialization of rescaling ConverterSwitch
  template<typename INP, typename OUTP>
  class ConverterSwitch<INP,OUTP,true> : public Rescaler<INP,OUTP>
  {
  public:
    ConverterSwitch( const RescalerInfo & info )
      : Rescaler<INP,OUTP>( info ) {}
    ConverterSwitch()	: Rescaler<INP,OUTP>() {}
  };


  /// Specialization of non-rescaling ConverterSwitch
  template<typename INP, typename OUTP>
  class ConverterSwitch<INP,OUTP,false> : public RawConverter<INP,OUTP>
  {
  public:
    ConverterSwitch( const RescalerInfo & )
      : RawConverter<INP,OUTP>() {}
    ConverterSwitch() : RawConverter<INP,OUTP>() {}
  };


  /** Mid-level converter.
      This converter automatically switches to raw or rescaling converter 
      depending on the ability to use rescaling on the data types */
  template<typename INP, typename OUTP>
  class SmartConverter
    : public ConverterSwitch<INP,OUTP, 
                             std::numeric_limits<INP>::is_specialized 
  && std::numeric_limits<OUTP>::is_bounded >
  {
  public:
    SmartConverter( const RescalerInfo & info )
      : ConverterSwitch<INP,OUTP,
                             std::numeric_limits<INP>::is_specialized
  && std::numeric_limits<OUTP>::is_bounded >( info ) {}
  };


  /// High-level converter, reimplementation of aims::Converter
  template <class INP,class OUTP>
  class Converter
  {
  public :
    Converter( bool rescale = false ) : _rescale( rescale ), _info() {}
    Converter( bool rescale, RescalerInfo & info ) : _rescale( rescale ), _info(info) {}
    ~Converter() {}

    /** converts an INP type data to an OUTP data.
        If rescale mode is on, the conversion tries to use the whole dynamics 
        of the OUTP type (int types), so all quantification info will be lost, 
        unless put as a scale factor in the header (used for AimsData).
    */
    OUTP* operator () ( const INP &in ) const;
    ///	converts INP into an existing OUTP object
    void convert( const INP &in, OUTP & out ) const;

  protected:
    bool	_rescale;
    RescalerInfo _info;

  };


  /** ShallowConverter only differs from the "standard" Converter in the 
      way that it may make shallow copies of objects if input and output types 
      are identical. When INP and OUTP types are different, it is the same as 
      Converter.
   */
  template <typename INP, typename OUTP> 
  class ShallowConverter : public Converter<INP, OUTP>
  {
  public:
    ShallowConverter( bool rescale = false ) 
      : Converter<INP, OUTP>( rescale ) {}
    ShallowConverter( bool rescale, RescalerInfo & info )
      : Converter<INP, OUTP>( rescale, info ) {}
  };

  // implementation
  template<typename INP, typename OUTP>
  inline
  Rescaler<INP,OUTP>::Rescaler() : _info()
  {
  }

  template<typename INP, typename OUTP>
  inline
  Rescaler<INP,OUTP>::Rescaler(const RescalerInfo & info) : _info(info)
  {
  }

  template<typename INP, typename OUTP>
  inline
  void Rescaler<INP,OUTP>::convert( const INP &in, OUTP & out ) const
  {
    out = this->_info->getScaledValue( in );
  }

  // Specialization for integer conversions rawconverter methods
  // In the case of conversion to integer types, we must round values, not only convert.
  CARTO_SPECIALIZED_RAWCONVERTER_ROUNDED_CONVERT( double, int16_t )
  CARTO_SPECIALIZED_RAWCONVERTER_ROUNDED_CONVERT( float, int16_t )
  CARTO_SPECIALIZED_RAWCONVERTER_ROUNDED_CONVERT( double, uint16_t )
  CARTO_SPECIALIZED_RAWCONVERTER_ROUNDED_CONVERT( float, uint16_t )
  CARTO_SPECIALIZED_RAWCONVERTER_ROUNDED_CONVERT( double, int32_t )
  CARTO_SPECIALIZED_RAWCONVERTER_ROUNDED_CONVERT( float, int32_t )
  CARTO_SPECIALIZED_RAWCONVERTER_ROUNDED_CONVERT( double, uint32_t )
  CARTO_SPECIALIZED_RAWCONVERTER_ROUNDED_CONVERT( float, uint32_t )
  CARTO_SPECIALIZED_RAWCONVERTER_ROUNDED_CONVERT( double, int64_t )
  CARTO_SPECIALIZED_RAWCONVERTER_ROUNDED_CONVERT( float, int64_t )
  CARTO_SPECIALIZED_RAWCONVERTER_ROUNDED_CONVERT( double, uint64_t )
  CARTO_SPECIALIZED_RAWCONVERTER_ROUNDED_CONVERT( float, uint64_t )

  template<typename INP, typename OUTP>
  inline
  void RawConverter<INP,OUTP>::convert( const INP &in, OUTP & out ) const
  {
    out = OUTP( in );
  }

  template<class INP, class OUTP>
  inline OUTP* ConverterAllocator<INP,OUTP>::alloc( const INP & )
  {
    return new OUTP;
  }

  // Converter
  template<class INP, class OUTP>
  inline OUTP* Converter<INP,OUTP>::operator () ( const INP & in ) const
  {
    OUTP    *out = ConverterAllocator<INP,OUTP>::alloc( in );
    convert( in, *out );
    return out;
  }

  template<class INP, class OUTP>
  inline void 
  Converter<INP,OUTP>::convert( const INP &in, OUTP & out ) const
  {
    if( _rescale )
      {
        SmartConverter<INP,OUTP>    sc(this->_info);
        sc.convert( in, out );
      }
    else
      {
        RawConverter<INP,OUTP>  rc;
        rc.convert( in, out );
      }
  }

  template<typename T>
  inline T min_limit() {
    /// Returns the negative minimum value, because for some types (double and float)
    /// the min value is the nearest value to 0
    T result = std::numeric_limits<T>::min();

    if ( std::numeric_limits<T>::is_signed ) {
      if ( result > 0 ) {
        result = 0 - std::numeric_limits<T>::max();
      }
    }

    return result;
  }

  template<typename T>
  inline bool ismin_limit( T value ){
    return ( value == carto::min_limit<T>() );
  }

  template<typename T>
  inline bool ismax_limit( T value ){
    return ( value == std::numeric_limits<T>::max() );
  }

  template<typename TYPE, typename TEST>
  inline bool isvalidvalue( TYPE value ){
    /// Check that value is valid for the test type
    return ( ((TYPE)carto::min_limit<TEST>()) <= value )
      && ( value <= ((TYPE)std::numeric_limits<TEST>::max()) );
  }

  template<typename TYPE>
  inline TYPE getcheckedmin( double min ) {
    return ( carto::isvalidvalue<double, TYPE>( min ) ? min : carto::min_limit<TYPE>() );
  }

  template<typename TYPE>
  inline TYPE getcheckedmax( double max ) {
    return ( carto::isvalidvalue<double, TYPE>( max ) ? max : std::numeric_limits<TYPE>::max() );
  }

  /*
   * DefaultedRescalerInfo methods implementation
   */
  template<typename INP, typename OUTP>
  DefaultedRescalerInfo<INP,OUTP>::DefaultedRescalerInfo( const RescalerInfo & info )
    : _info( info )
  {
    // Initialize scale information
    setvmin( _info.vmin );
    setvmax( _info.vmax );
    setomin( _info.omin );
    setomax( _info.omax );

    _scale = this->getscale();
  }

  template<typename INP, typename OUTP>
  double DefaultedRescalerInfo<INP,OUTP>::getScale() {
    return _scale;
  }

  template<typename INP, typename OUTP>
  OUTP DefaultedRescalerInfo<INP,OUTP>::getScaledValue( INP value ) {
    OUTP result;
    RawConverter<double, OUTP> doubleconverter;
    double scaledvalue;

    scaledvalue = ( value - this->_defaultedvmin ) * _scale + this->_defaultedomin;

    if ( scaledvalue < ((double)carto::min_limit<OUTP>())) {
      result = carto::min_limit<OUTP>();
    } else if ( scaledvalue > ((double)std::numeric_limits<OUTP>::max()) ) {
      result = std::numeric_limits<OUTP>::max();
    } else {
      doubleconverter.convert( scaledvalue, result );
    }

    return result;
  }

  template<typename INP, typename OUTP>
  void DefaultedRescalerInfo<INP,OUTP>::setvmin( double vmin ) {
    _defaultedvmin = carto::getcheckedmin<INP>( vmin );
  }

  template<typename INP, typename OUTP>
  void DefaultedRescalerInfo<INP,OUTP>::setvmax( double vmax ) {
    _defaultedvmax = carto::getcheckedmax<INP>( vmax );
  }

  template<typename INP, typename OUTP>
  void DefaultedRescalerInfo<INP,OUTP>::setomin( double omin ) {
    _defaultedomin = carto::getcheckedmin<OUTP>( omin );
  }

  template<typename INP, typename OUTP>
  void DefaultedRescalerInfo<INP,OUTP>::setomax( double omax ) {
    _defaultedomax = carto::getcheckedmax<OUTP>( omax );
  }

  template<typename INP, typename OUTP>
  double DefaultedRescalerInfo<INP,OUTP>::getscale() {
    double vdiff, odiff;
    vdiff = (double)this->_defaultedvmax - (double)this->_defaultedvmin;
    vdiff = ( vdiff != 0 ? vdiff : 1 );
    odiff = (double)this->_defaultedomax - (double)this->_defaultedomin;

    return odiff / vdiff ;
  }
}

#endif
