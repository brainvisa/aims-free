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

/*
 *  Data reader class
 */

#include <aims/io/finder.h>
#include <aims/io/reader.h>
#include <aims/io/channelR.h>

using namespace aims;
using namespace carto;
using namespace std;

namespace aims
{
  template<class INP, class OUTP>
  bool internalconvert( Process & p, const string & fname, Finder & f )
  {
    string			fmt = f.format();
    Reader<INP>			r( fname );
    InternalConverter<OUTP>     & ic = (InternalConverter<OUTP> &) p;
    INP				input;
    r.read( input, ic.border, &fmt, ic.frame );

    ic.data = OUTP( input.dimX(), 
                    input.dimY(),
                    input.dimZ(), 
                    input.dimT() );
    ic.data.volume()->header() = input.volume()->header();

    ChannelSelector< INP, OUTP > selector;
    ic.data = selector.select( input, ic.channel );

    return( true );
  }

  template<class OUTP>
  ChannelReader<OUTP>::ChannelReader( const std::string & f )
    : _filename( f )
  {
    setAllowedInputDataTypes( map<string, set<string> >() );
  }

  template<class OUTP>
  ChannelReader<OUTP>::~ChannelReader()
  {
  }

  template<class OUTP>
  void ChannelReader<OUTP>::read( OUTP & data, uint8_t channel, int border, 
                                   const std::string*, int frame )
  {
    InternalConverter<OUTP>	ic( channel, border, frame, data );

    map<std::string, set<string> >::const_iterator idt
      = _allowedTypes.find( "Volume" );
    if( idt != _allowedTypes.end() )
    {
      const set<string> &vt = idt->second;
      set<string>::const_iterator e = vt.end();
      if( vt.find( "S8" ) != e )
        ic.registerProcessType( "Volume", "S8",
                                &internalconvert< AimsData<int8_t>, OUTP > );
      if( vt.find( "U8" ) != e )
        ic.registerProcessType( "Volume", "U8",
                                &internalconvert< AimsData<uint8_t>, OUTP > );
      if( vt.find( "S16" ) != e )
        ic.registerProcessType( "Volume", "S16",
                                &internalconvert< AimsData<int16_t>, OUTP > );
      if( vt.find( "U16" ) != e )
        ic.registerProcessType( "Volume", "U16",
                                &internalconvert< AimsData<uint16_t>, OUTP > );
      if( vt.find( "S32" ) != e )
        ic.registerProcessType( "Volume", "S32",
                                &internalconvert< AimsData<int32_t>, OUTP > );
      if( vt.find( "U32" ) != e )
        ic.registerProcessType( "Volume", "U32",
                                &internalconvert< AimsData<uint32_t>, OUTP > );
      if( vt.find( "FLOAT" ) != e )
        ic.registerProcessType( "Volume", "FLOAT",
                                &internalconvert< AimsData<float>, OUTP > );
      if( vt.find( "DOUBLE" ) != e )
        ic.registerProcessType( "Volume", "DOUBLE",
                                &internalconvert< AimsData<double>, OUTP > );
      if( vt.find( "RGB" ) != e )
        ic.registerProcessType( "Volume", "RGB",
                                &internalconvert< AimsData<AimsRGB>, OUTP > );
      if( vt.find( "RGBA" ) != e )
        ic.registerProcessType( "Volume", "RGBA",
                                &internalconvert< AimsData<AimsRGBA>, OUTP > );
      if( vt.find( "HSV" ) != e )
        ic.registerProcessType( "Volume", "HSV",
                                &internalconvert< AimsData<AimsHSV>, OUTP > );
    }

    ic.execute( _filename );
  }

  template <typename OUTP>
  void ChannelReader<OUTP>::setAllowedInputDataTypes(
    const map<std::string, set<string> > & types )
  {
    _allowedTypes = types;
    if( _allowedTypes.empty() )
    {
      // fill in standard data types
      set<string> & vt = _allowedTypes[ "Volume" ];
      vt.insert( "S8" );
      vt.insert( "U8" );
      vt.insert( "S16" );
      vt.insert( "U16" );
      vt.insert( "S32" );
      vt.insert( "U32" );
      vt.insert( "FLOAT" );
      vt.insert( "DOUBLE" );
      vt.insert( "RGB" );
      vt.insert( "RGBA" );
      vt.insert( "HSV" );
    }
  }

  // Instanciates classes
  #define INSTANCIATE_INTERNAL_CONVERT( INP, OUTP )    \
  template bool internalconvert< INP, OUTP >( Process & p, const string & fname, Finder & f );
/*

  #define INSTANCIATE_EXTENDED_READER( OUTP )    \
  INSTANCIATE_INTERNAL_CONVERT( AimsData<int8_t>, AimsData<OUTP> ) \
  INSTANCIATE_INTERNAL_CONVERT( AimsData<uint8_t>, AimsData<OUTP> ) \
  INSTANCIATE_INTERNAL_CONVERT( AimsData<int16_t>, AimsData<OUTP> ) \
  INSTANCIATE_INTERNAL_CONVERT( AimsData<uint16_t>, AimsData<OUTP> ) \
  INSTANCIATE_INTERNAL_CONVERT( AimsData<int32_t>, AimsData<OUTP> ) \
  INSTANCIATE_INTERNAL_CONVERT( AimsData<uint32_t>, AimsData<OUTP> ) \
  INSTANCIATE_INTERNAL_CONVERT( AimsData<float>, AimsData<OUTP> ) \
  INSTANCIATE_INTERNAL_CONVERT( AimsData<double>, AimsData<OUTP> ) \
  INSTANCIATE_INTERNAL_CONVERT( AimsData<AimsRGB>, AimsData<OUTP> ) \
  INSTANCIATE_INTERNAL_CONVERT( AimsData<AimsRGBA>, AimsData<OUTP> ) \
  INSTANCIATE_INTERNAL_CONVERT( AimsData<AimsHSV>, AimsData<OUTP> ) \*/


  INSTANCIATE_INTERNAL_CONVERT( AimsData<int8_t>, AimsData<int8_t> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<uint8_t>, AimsData<int8_t> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<int16_t>, AimsData<int8_t> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<uint16_t>, AimsData<int8_t> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<int32_t>, AimsData<int8_t> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<uint32_t>, AimsData<int8_t> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<float>, AimsData<int8_t> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<double>, AimsData<int8_t> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<AimsRGB>, AimsData<int8_t> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<AimsRGBA>, AimsData<int8_t> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<AimsHSV>, AimsData<int8_t> )

  INSTANCIATE_INTERNAL_CONVERT( AimsData<int8_t>, AimsData<uint8_t> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<uint8_t>, AimsData<uint8_t> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<int16_t>, AimsData<uint8_t> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<uint16_t>, AimsData<uint8_t> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<int32_t>, AimsData<uint8_t> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<uint32_t>, AimsData<uint8_t> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<float>, AimsData<uint8_t> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<double>, AimsData<uint8_t> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<AimsRGB>, AimsData<uint8_t> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<AimsRGBA>, AimsData<uint8_t> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<AimsHSV>, AimsData<uint8_t> )

  INSTANCIATE_INTERNAL_CONVERT( AimsData<int8_t>, AimsData<int16_t> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<uint8_t>, AimsData<int16_t> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<int16_t>, AimsData<int16_t> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<uint16_t>, AimsData<int16_t> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<int32_t>, AimsData<int16_t> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<uint32_t>, AimsData<int16_t> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<float>, AimsData<int16_t> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<double>, AimsData<int16_t> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<AimsRGB>, AimsData<int16_t> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<AimsRGBA>, AimsData<int16_t> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<AimsHSV>, AimsData<int16_t> )

  INSTANCIATE_INTERNAL_CONVERT( AimsData<int8_t>, AimsData<uint16_t> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<uint8_t>, AimsData<uint16_t> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<int16_t>, AimsData<uint16_t> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<uint16_t>, AimsData<uint16_t> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<int32_t>, AimsData<uint16_t> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<uint32_t>, AimsData<uint16_t> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<float>, AimsData<uint16_t> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<double>, AimsData<uint16_t> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<AimsRGB>, AimsData<uint16_t> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<AimsRGBA>, AimsData<uint16_t> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<AimsHSV>, AimsData<uint16_t> )

  INSTANCIATE_INTERNAL_CONVERT( AimsData<int8_t>, AimsData<int32_t> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<uint8_t>, AimsData<int32_t> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<int16_t>, AimsData<int32_t> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<uint16_t>, AimsData<int32_t> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<int32_t>, AimsData<int32_t> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<uint32_t>, AimsData<int32_t> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<float>, AimsData<int32_t> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<double>, AimsData<int32_t> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<AimsRGB>, AimsData<int32_t> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<AimsRGBA>, AimsData<int32_t> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<AimsHSV>, AimsData<int32_t> )

  INSTANCIATE_INTERNAL_CONVERT( AimsData<int8_t>, AimsData<uint32_t> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<uint8_t>, AimsData<uint32_t> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<int16_t>, AimsData<uint32_t> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<uint16_t>, AimsData<uint32_t> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<int32_t>, AimsData<uint32_t> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<uint32_t>, AimsData<uint32_t> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<float>, AimsData<uint32_t> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<double>, AimsData<uint32_t> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<AimsRGB>, AimsData<uint32_t> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<AimsRGBA>, AimsData<uint32_t> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<AimsHSV>, AimsData<uint32_t> )

  INSTANCIATE_INTERNAL_CONVERT( AimsData<int8_t>, AimsData<float> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<uint8_t>, AimsData<float> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<int16_t>, AimsData<float> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<uint16_t>, AimsData<float> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<int32_t>, AimsData<float> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<uint32_t>, AimsData<float> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<float>, AimsData<float> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<double>, AimsData<float> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<AimsRGB>, AimsData<float> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<AimsRGBA>, AimsData<float> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<AimsHSV>, AimsData<float> )

  INSTANCIATE_INTERNAL_CONVERT( AimsData<int8_t>, AimsData<double> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<uint8_t>, AimsData<double> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<int16_t>, AimsData<double> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<uint16_t>, AimsData<double> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<int32_t>, AimsData<double> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<uint32_t>, AimsData<double> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<float>, AimsData<double> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<double>, AimsData<double> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<AimsRGB>, AimsData<double> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<AimsRGBA>, AimsData<double> )
  INSTANCIATE_INTERNAL_CONVERT( AimsData<AimsHSV>, AimsData<double> )


  template class ChannelReader< AimsData<int8_t> >;
  template class ChannelReader< AimsData<uint8_t> >;
  template class ChannelReader< AimsData<int16_t> >;
  template class ChannelReader< AimsData<uint16_t> >;
  template class ChannelReader< AimsData<int32_t> >;
  template class ChannelReader< AimsData<uint32_t> >;
  template class ChannelReader< AimsData<float> >;
  template class ChannelReader< AimsData<double> >;

}
