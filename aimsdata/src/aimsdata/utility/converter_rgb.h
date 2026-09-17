#ifndef AIMS_UTILITY_CONVERTER_RGB_H
#define AIMS_UTILITY_CONVERTER_RGB_H

#include <cartobase/type/converter.h>
#include <aims/rgb/rgb.h>

namespace carto
{

  template <class INP>
  class RawConverter<INP, AimsRGB>
  {
  public :
    void convert( const INP &in, AimsRGB & out ) const;
  };

  template<class INP>
  inline void
  RawConverter<INP,AimsRGB>::convert( const INP &in, AimsRGB & out ) const
  {
    out = AimsRGB( (::byte) in, (::byte) in, (::byte) in );
  }

  template <class INP>
  class RawConverter<INP, AimsRGBA>
  {
  public :
    void convert( const INP &in, AimsRGBA & out ) const;
  };

  template<class INP>
  inline void
  RawConverter<INP,AimsRGBA>::convert( const INP &in, AimsRGBA & out ) const
  {
    out = AimsRGBA( (::byte) in, (::byte) in, (::byte) in, 255 );
  }

  template <class OUTP>
  class RawConverter<AimsRGB, OUTP>
  {
  public :
    void convert( const AimsRGB &in, OUTP & out ) const;
  };

  template<class OUTP>
  inline void
  RawConverter<AimsRGB,OUTP>::convert( const AimsRGB &in, OUTP & out ) const
  {
    out = (OUTP) ( sqrt( ( (double) in.red() ) * in.red()
			+ ( (double) in.green() ) * in.green()
			+ ( (double) in.blue() ) * in.blue() ) );
  }

  template <class OUTP>
  class RawConverter<AimsRGBA, OUTP>
  {
  public :
    void convert( const AimsRGBA &in, OUTP & out ) const;
  };

  template<class OUTP>
  inline void
  RawConverter<AimsRGBA,OUTP>::convert( const AimsRGBA &in, OUTP & out ) const
  {
    out = (OUTP) ( sqrt( ( (double) in.red() ) * in.red()
			+ ( (double) in.green() ) * in.green()
			+ ( (double) in.blue() ) * in.blue() ) );
  }

  template <>
  class RawConverter<AimsRGB, AimsRGBA>
  {
  public :
    void convert( const AimsRGB &in, AimsRGBA & out ) const
    {
      out = in;
    }
  };


  template <>
  class RawConverter<AimsRGBA, AimsRGB>
  {
  public :
    void convert( const AimsRGBA &in, AimsRGB & out ) const
    {
      out = in;
    }
  };

}

//--- Vector/RGB conversion --------------------------------------------------
#include <aims/vector/vector.h>
#include <limits>

namespace carto {

  template <typename T, int D>
  class RawConverter<AimsVector<T,D>, AimsRGB>
  {
  public:
    void convert( const AimsVector<T,D> &in, AimsRGB &out ) const
    {
      for( uint8_t i=0; i<3 && i<in.size(); ++i )
        out[i] = in[i];
    }
  };


  template <typename T, int D>
  class RawConverter<AimsRGB, AimsVector<T,D> >
  {
  public:
    void convert( const AimsRGB &in, AimsVector<T,D> &out ) const
    {
      for( uint8_t i=0; i<3 && i<out.size(); ++i )
        out[i] = in[i];
    }
  };


  template <typename T, int D>
  class RawConverter<AimsVector<T,D>, AimsRGBA>
  {
  public:
    void convert( const AimsVector<T,D> &in, AimsRGBA &out ) const
    {
      for( uint8_t i=0; i<4 && i<in.size(); ++i )
        out[i] = in[i];
    }
  };


  template <typename T, int D>
  class RawConverter<AimsRGBA, AimsVector<T,D> >
  {
  public:
    void convert( const AimsRGBA &in, AimsVector<T,D> &out ) const
    {
      for( uint8_t i=0; i<4 && i<out.size(); ++i )
        out[i] = in[i];
    }
  };

}


#endif


