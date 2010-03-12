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

#ifndef AIMS_UTILITY_CHANNEL_H
#define AIMS_UTILITY_CHANNEL_H

#include <iomanip>
#include <cartobase/type/string_conversion.h>
#include <aims/data/data_g.h>
#include <aims/utility/converter_g.h>

using namespace std;
using namespace carto;
using namespace aims; 

enum AimsRGBAChannel
{
  RedChannel = 0,
  GreenChannel = 1,
  BlueChannel = 2,
  AlphaChannel = 3
};

enum AimsHSVChannel
{
  HueChannel = 0,
  SaturationChannel = 1,
  ValueChannel = 2
};


/**	This class is just a hint to get data type information */
template<class T>
class DataTypeInfo
{
  public:
    DataTypeInfo() {};

  public:
    static inline uint8_t samples();
};


template<typename T>
inline uint8_t DataTypeInfo<T>::samples()
{ 
  return 1;
}

//	specializations
template<>
inline uint8_t DataTypeInfo<AimsRGB>::samples()
{
  return 3;
}

//---------------------------------------------------------------------------
template<>
inline uint8_t DataTypeInfo<AimsRGBA>::samples()
{
  return 4;
}

//---------------------------------------------------------------------------
template<>
inline uint8_t DataTypeInfo<AimsHSV>::samples()
{
  return 3;
}


#define DECLARE_CHANNEL_SELECTOR_NAME() \
class ChannelSelector \


#define DECLARE_CHANNEL_SELECTOR_SPEC( T, C ) \
{ \
    public: \
      ChannelSelector() {} \
      virtual ~ChannelSelector() {} \
\
      C select( T input, uint8_t channel ); \
      void set( T & input, uint8_t channel, C value ); \
\
    private: \
      ShallowConverter<T, C> conv; \
}; \


#define DECLARE_CHANNEL_SELECTOR_CLASS() \
template <class T, class C > \
DECLARE_CHANNEL_SELECTOR_NAME() \
DECLARE_CHANNEL_SELECTOR_SPEC( T, C ) \


#define DECLARE_CHANNEL_SELECTOR_SPECIALIZED( T ) \
template <class C> \
DECLARE_CHANNEL_SELECTOR_NAME() < T, C > \
DECLARE_CHANNEL_SELECTOR_SPEC( T, C ) \


#define DECLARE_CHANNEL_SELECTOR_VOLUME_SPECIALIZED() \
template <class T, class C> \
DECLARE_CHANNEL_SELECTOR_NAME() < AimsData<T>, AimsData<C> > \
DECLARE_CHANNEL_SELECTOR_SPEC( AimsData<T>, AimsData<C> ) \


DECLARE_CHANNEL_SELECTOR_CLASS()
DECLARE_CHANNEL_SELECTOR_VOLUME_SPECIALIZED()
DECLARE_CHANNEL_SELECTOR_SPECIALIZED( AimsRGB )
DECLARE_CHANNEL_SELECTOR_SPECIALIZED( AimsRGBA )
DECLARE_CHANNEL_SELECTOR_SPECIALIZED( AimsHSV )


template <class T, class C> inline
C ChannelSelector< T, C >::select( T input, uint8_t )
{
  return input;
}

template <class T, class C> inline
void ChannelSelector< T, C >::set( T& input, uint8_t, C value )
{
  input = value;
}

template <class C> inline
C ChannelSelector< AimsRGB, C >::select( AimsRGB input, uint8_t channel )
{
  C output;

  switch(channel) {
    case RedChannel :
      conv.convert( input.red(), output );
      break;
    case GreenChannel :
      conv.convert( input.green(), output );
      break;
    case BlueChannel :
      conv.convert( input.blue(), output );
      break;
    default :
      conv.convert( input, output );
  }
  return output;
}

template <class C> inline
void ChannelSelector< AimsRGB, C >::set( AimsRGB& input, uint8_t channel, C value )
{
  switch(channel) {
    case RedChannel :
      conv.convert( value, input.red() );
      break;
    case GreenChannel :
      conv.convert( value, input.green() );
      break;
    case BlueChannel :
      conv.convert( value, input.blue() );
      break;
  }
}

template <class C> inline
C ChannelSelector< AimsRGBA, C >::select( AimsRGBA input, uint8_t channel )
{
  C output;

  switch(channel) {
    case RedChannel :
      conv.convert( input.red(), output );
      break;
    case GreenChannel :
      conv.convert( input.green(), output );
      break;
    case BlueChannel :
      conv.convert( input.blue(), output );
      break;
    case AlphaChannel :
      conv.convert( input.alpha(), output );
      break;
    default :
      conv.convert( input, output );
  }

  return output;
}

template <class C> inline
void ChannelSelector< AimsRGBA, C >::set( AimsRGBA& input, uint8_t channel, C value )
{
  switch(channel) {
    case RedChannel :
      conv.convert( value, input.red() );
      break;
    case GreenChannel :
      conv.convert( value, input.green() );
      break;
    case BlueChannel :
      conv.convert( value, input.blue() );
      break;
    case AlphaChannel :
      conv.convert( value, input.alpha() );
      break;
  }
}

template <class C> inline
C ChannelSelector< AimsHSV, C >::select( AimsHSV input, uint8_t channel )
{
  C output;

  switch(channel) {
    case HueChannel :
      conv.convert( input.hue(), output );
      break;
    case SaturationChannel :
      conv.convert( input.saturation(), output );
      break;
    case ValueChannel :
      conv.convert( input.value(), output );
      break;
    default :
      conv.convert( input, output );
  }
  return output;
}


template <class C> inline
void ChannelSelector< AimsHSV, C >::set( AimsHSV& input, uint8_t channel, C value )
{
  C output;

  switch(channel) {
    case HueChannel :
      conv.convert( value, input.hue() );
      break;
    case SaturationChannel :
      conv.convert( value, input.saturation() );
      break;
    case ValueChannel :
      conv.convert( value, input.value() );
      break;
  }
}

template<class T, class C> inline
AimsData< C > ChannelSelector< AimsData< T >, AimsData< C > >::select( AimsData< T > input, uint8_t channel )
{
  ChannelSelector< T, C > selector;
  int	x, y, z, t, 
        dx = input.dimX(), 
        dy = input.dimY(), 
        dz = input.dimZ(), 
        dt = input.dimT();

  AimsData< C > output = AimsData< C >( dx, dy, dz, dt );

  output.setSizeXYZT( input.sizeX(), 
                      input.sizeY(), 
                      input.sizeZ(),
                      input.sizeT() );

  if( input.header() )
    output.setHeader( input.header()->cloneHeader( true ) );

  for( t=0; t<dt; ++t )
    {
      for( z=0; z<dz; ++z )
        {
          for( y=0; y<dy; ++y )
            {
              for( x=0; x<dx; ++x )
                output( x, y, z, t ) = selector.select( input( x, y, z, t ), channel );
            }
        }
    }

  return output;
}


template<class T, class C> inline
void ChannelSelector< AimsData< T >, AimsData< C > >::set( AimsData< T >& input, uint8_t channel, AimsData< C > value )
{
  ChannelSelector< T, C > selector;

  int	x, y, z, t, 
        dx = input.dimX(), 
        dy = input.dimY(), 
        dz = input.dimZ(), 
        dt = input.dimT();

  for( t=0; t<dt; ++t )
    {
      for( z=0; z<dz; ++z )
        {
          for( y=0; y<dy; ++y )
            {
              for( x=0; x<dx; ++x )
                 selector.set( input( x, y, z, t ), channel, value( x, y, z, t ) );
            }
        }
    }
}


#endif
