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


#define DECLARE_CHANNEL_SELECTOR_SPEC( T, U, C ) \
{ \
    public: \
      ChannelSelector() {} \
      virtual ~ChannelSelector() {} \
\
      U select( T input, uint8_t channel ); \
      void set( T & input, uint8_t channel, U value ); \
\
    private: \
      ShallowConverter<C, U> channelconv; \
      ShallowConverter<T, U> dataconv; \
}; \


#define DECLARE_CHANNEL_SELECTOR_CLASS() \
template <class T, class U, class C = byte > \
DECLARE_CHANNEL_SELECTOR_NAME() \
DECLARE_CHANNEL_SELECTOR_SPEC( T, U, C ) \


#define DECLARE_CHANNEL_SELECTOR_SPECIALIZED( T ) \
template <class U> \
DECLARE_CHANNEL_SELECTOR_NAME() < T, U > \
DECLARE_CHANNEL_SELECTOR_SPEC( T, U, byte ) \


#define DECLARE_CHANNEL_SELECTOR_VOLUME_SPECIALIZED() \
template <class T, class U> \
DECLARE_CHANNEL_SELECTOR_NAME() < AimsData<T>, AimsData<U> > \
DECLARE_CHANNEL_SELECTOR_SPEC( AimsData<T>, AimsData<U>, Void ) \


DECLARE_CHANNEL_SELECTOR_CLASS()
DECLARE_CHANNEL_SELECTOR_VOLUME_SPECIALIZED()
DECLARE_CHANNEL_SELECTOR_SPECIALIZED( AimsRGB )
DECLARE_CHANNEL_SELECTOR_SPECIALIZED( AimsRGBA )
DECLARE_CHANNEL_SELECTOR_SPECIALIZED( AimsHSV )


template <class T, class U, class C> inline
U ChannelSelector< T, U, C >::select( T input, uint8_t )
{
  return input;
}

template <class T, class U, class C> inline
void ChannelSelector< T, U, C >::set( T& input, uint8_t, U value )
{
  input = value;
}

template <class U> inline
U ChannelSelector< AimsRGB, U >::select( AimsRGB input, uint8_t channel )
{
  U output;

  switch(channel) {
    case RedChannel :
      channelconv.convert( input.red(), output );
      break;
    case GreenChannel :
      channelconv.convert( input.green(), output );
      break;
    case BlueChannel :
      channelconv.convert( input.blue(), output );
      break;
    default :
      dataconv.convert( input, output );
  }
  return output;
}

template <class U> inline
void ChannelSelector< AimsRGB, U >::set( AimsRGB& input, uint8_t channel, U value )
{
  switch(channel) {
    case RedChannel :
      channelconv.convert( value, input.red() );
      break;
    case GreenChannel :
      channelconv.convert( value, input.green() );
      break;
    case BlueChannel :
      channelconv.convert( value, input.blue() );
      break;
  }
}

template <class U> inline
U ChannelSelector< AimsRGBA, U >::select( AimsRGBA input, uint8_t channel )
{
  U output;

  switch(channel) {
    case RedChannel :
      channelconv.convert( input.red(), output );
      break;
    case GreenChannel :
      channelconv.convert( input.green(), output );
      break;
    case BlueChannel :
      channelconv.convert( input.blue(), output );
      break;
    case AlphaChannel :
      channelconv.convert( input.alpha(), output );
      break;
    default :
      dataconv.convert( input, output );
  }

  return output;
}

template <class U> inline
void ChannelSelector< AimsRGBA, U >::set( AimsRGBA& input, uint8_t channel, U value )
{
  switch(channel) {
    case RedChannel :
      channelconv.convert( value, input.red() );
      break;
    case GreenChannel :
      channelconv.convert( value, input.green() );
      break;
    case BlueChannel :
      channelconv.convert( value, input.blue() );
      break;
    case AlphaChannel :
      dataconv.convert( value, input.alpha() );
      break;
  }
}

template <class U> inline
U ChannelSelector< AimsHSV, U >::select( AimsHSV input, uint8_t channel )
{
  U output;

  switch(channel) {
    case HueChannel :
      channelconv.convert( input.hue(), output );
      break;
    case SaturationChannel :
      channelconv.convert( input.saturation(), output );
      break;
    case ValueChannel :
      channelconv.convert( input.value(), output );
      break;
    default :
      dataconv.convert( input, output );
      break;
  }

  return output;
}


template <class U> inline
void ChannelSelector< AimsHSV, U >::set( AimsHSV& input, uint8_t channel, U value )
{
  U output;

  switch(channel) {
    case HueChannel :
      channelconv.convert( value, input.hue() );
      break;
    case SaturationChannel :
      channelconv.convert( value, input.saturation() );
      break;
    case ValueChannel :
      channelconv.convert( value, input.value() );
      break;
  }
}

template<class T, class U> inline
AimsData< U > ChannelSelector< AimsData< T >, AimsData< U > >::select( AimsData< T > input, uint8_t channel )
{
  ChannelSelector< T, U > selector;
  int	x, y, z, t, 
        dx = input.dimX(), 
        dy = input.dimY(), 
        dz = input.dimZ(), 
        dt = input.dimT();

  AimsData< U > output = AimsData< U >( dx, dy, dz, dt );

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


template<class T, class U> inline
void ChannelSelector< AimsData< T >, AimsData< U > >::set( AimsData< T >& input, uint8_t channel, AimsData< U > value )
{
  ChannelSelector< T, U > selector;

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
