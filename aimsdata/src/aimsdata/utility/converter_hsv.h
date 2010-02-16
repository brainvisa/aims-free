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
    byte rgb_min, rgb_max;
    rgb_min = MIN3(in.red(), in.green(), in.blue());
    rgb_max = MAX3(in.red(), in.green(), in.blue());

    /* Compute value */
    out.value() = rgb_max;
    if (out.value() == 0) {
        out.hue() = out.saturation() = 0;
    }
    else {
      /* Compute saturation */
      out.saturation() = rint(255.0 * float(rgb_max - rgb_min) / out.value());
      if (out.saturation() == 0) {
          out.hue() = 0;
      }
      else {  
        /* Compute hue */
        if (rgb_max == in.red()) {
            out.hue() = 0 + rint(43.0 * float(in.green() - in.blue())/(rgb_max - rgb_min));
        } else if (rgb_max == in.green()) {
            out.hue() = 85 + rint(43.0 * float(in.blue() - in.red())/(rgb_max - rgb_min));
        } else /* rgb_max == rgb.b */ {
            out.hue() = 171 + rint(43.0 * float(in.red() - in.green())/(rgb_max - rgb_min));
        }
      }
    }
  }

  template <class T> inline
  void hsvtorgb( const AimsHSV &in, T & out )
  {
    float h, s, v;
    long i, p, q, t;

    if( in.saturation() == 0 )
    {
      out.red() = out.green() = out.blue() = in.value();
      return;
    }

    
    h = float( in.hue() ) * 6 / 255;
    s = float( in.saturation() ) / 255;
    v = float( in.value() ) / 255;
    i = floor( h );
    p = rint( v * ( 1 - s ) * 255 );
    q = rint( v * ( 1 - s * ( h - i ) ) * 255 );
    t = rint( v * ( 1 - s * ( 1 - ( h - i ) ) ) * 255 );

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
    AimsRGB tmp( (byte) in, (byte) in, (byte) in );
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


