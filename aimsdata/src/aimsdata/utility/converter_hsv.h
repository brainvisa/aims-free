#ifndef AIMS_UTILITY_CONVERTER_HSV_H
#define AIMS_UTILITY_CONVERTER_HSV_H

#include <cartobase/type/string_conversion.h>
#include <cartobase/type/converter.h>
#include <aims/rgb/rgb.h>
#include <aims/utility/converter_rgb.h>
#include <aims/hsv/hsv.h>

#define MIN3(x,y,z)  ((y) <= (z) ? \
                         ((x) <= (y) ? (x) : (y)) \
                     : \
                         ((x) <= (z) ? (x) : (z)))

#define MAX3(x,y,z)  ((y) >= (z) ? \
                         ((x) >= (y) ? (x) : (y)) \
                     : \
                         ((x) >= (z) ? (x) : (z)))


namespace carto
{
  template <class T> inline
  void rgbtohsv( const T &in, AimsHSV & out )
  {
    ::byte rgb_min, rgb_max;
    rgb_min = MIN3(in.red(), in.green(), in.blue());
    rgb_max = MAX3(in.red(), in.green(), in.blue());

    /* Compute value */
    out.value() = rgb_max;
    if (out.value() == 0) {
        out.hue() = out.saturation() = 0;
    }
    else {
      /* Compute saturation */
      out.saturation() = uint8_t( rint(255.0 * double(rgb_max - rgb_min)
        / out.value()) );
      if (out.saturation() == 0) {
          out.hue() = 0;
      }
      else {  
        /* Compute hue */
        if (rgb_max == in.red()) {
          out.hue() = uint8_t( rint(43.0 * double(in.green()
            - in.blue())/(rgb_max - rgb_min)) );
        } else if (rgb_max == in.green()) {
          out.hue() = 85 + uint8_t( rint(43.0 * double(in.blue()
            - in.red())/(rgb_max - rgb_min)) );
        } else /* rgb_max == rgb.b */ {
          out.hue() = 171 + uint8_t( rint(43.0 * double(in.red()
            - in.green())/(rgb_max - rgb_min)) );
        }
      }
    }
  }

  template <class T> inline
  void hsvtorgb( const AimsHSV &in, T & out )
  {
    double f, h, s, v, r0 = 360.0 / 255, r1 = 1.0 / 255;
    long i, p, q, t;

    if( in.saturation() == 0 )
    {
      out.red() = out.green() = out.blue() = in.value();
      return;
    }

    h = double( in.hue() ) * r0;
    s = double( in.saturation() ) * r1;
    v = double( in.value() ) * r1;
    i = long( floor( h / 60 ) ) % 6;
    f = h / 60 - i;
    p = ::byte( v * ( 1.0 - s ) * 255 );
    q = ::byte( v * ( 1.0 - f * s ) * 255 );
    t = ::byte( v * ( 1.0 - ( 1.0 - f ) * s ) * 255 );
    
    switch( i ) {
      case 0:
        out.red() = in.value();
        out.green() = t;
        out.blue() = p;
        break;
      case 1:
        out.red() = q;
        out.green() = in.value();
        out.blue() = p;
        break;
      case 2:
        out.red() = p;
        out.green() = in.value();
        out.blue() = t;
        break;
      case 3:
        out.red() = p;
        out.green() = q;
        out.blue() = in.value();
        break;
      case 4:
        out.red() = t;
        out.green() = p;
        out.blue() = in.value();
        break;
      default:
        out.red() = in.value();
        out.green() = p;
        out.blue() = q;
        break;
    }
  }

  template <class INP>
  class RawConverter<INP, AimsHSV>
  {
  public :
    void convert( const INP &in, AimsHSV & out ) const;
  };

  template<class INP>
  inline void 
  RawConverter<INP,AimsHSV>::convert( const INP &in, AimsHSV & out ) const
  {
    AimsRGB tmp( (::byte) in, (::byte) in, (::byte) in );
    rgbtohsv<AimsRGB>( tmp, out );
  }

  template <class OUTP>
  class RawConverter<AimsHSV, OUTP>
  {
  public :
    void convert( const AimsHSV &in, OUTP & out ) const;
  };

  template<class OUTP>
  inline void 
  RawConverter<AimsHSV,OUTP>::convert( const AimsHSV &in, OUTP & out ) const
  {
    // First convert to RGB
    AimsRGB tmp;
    hsvtorgb<AimsRGB>( in, tmp );
    out = (OUTP) ( sqrt( ( (double) tmp.red() ) * tmp.red() 
			+ ( (double) tmp.green() ) * tmp.green() 
			+ ( (double) tmp.blue() ) * tmp.blue() ) );
  }

  template <>
  class RawConverter<AimsHSV, AimsRGB>
  {
  public :
    void convert( const AimsHSV &in, AimsRGB & out ) const
    {
      hsvtorgb<AimsRGB>( in, out );
    }
  };

  template <>
  class RawConverter<AimsHSV, AimsRGBA>
  {
  public :
    void convert( const AimsHSV &in, AimsRGBA & out ) const
    {
      hsvtorgb<AimsRGBA>( in, out );
    }
  };


  template <>
  class RawConverter<AimsRGB, AimsHSV>
  {
  public :
    void convert( const AimsRGB &in, AimsHSV & out ) const
    {
      rgbtohsv<AimsRGB>( in, out );
    }
  };

  template <>
  class RawConverter<AimsRGBA, AimsHSV>
  {
  public :
    void convert( const AimsRGBA &in, AimsHSV & out ) const
    {
      rgbtohsv<AimsRGBA>( in, out );
    }
  };

}

#endif


