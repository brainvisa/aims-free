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


