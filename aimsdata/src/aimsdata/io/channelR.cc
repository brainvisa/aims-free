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

// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/io/finder.h>
#include <aims/io/reader.h>
#include <aims/io/channelR.h>

#include <stdexcept>

using namespace aims;
using namespace carto;
using namespace std;

namespace aims
{
  // Internal class for volume conversion
  template<class INP, class OUTP>
  class VolumeConverter
  {
  public:
      static bool convert(Process & p, const string & fname, Finder & f);
  };
  
  template<class INP, class OUTP>
  bool VolumeConverter<INP, OUTP>::convert(
                                   Process & p, 
                                   const string & fname, 
                                   Finder & f)
  {
     throw std::runtime_error("Volume converter not implemented between " 
                              + carto::DataTypeCode<INP>::name() + " and " 
                              + carto::DataTypeCode<OUTP>::name());
  }
  
  // 
  // Partial specialization for carto::VolumeRef
  //
  template<class INP, class OUTP>
  class VolumeConverter<carto::VolumeRef<INP>, carto::VolumeRef<OUTP> >
  {
  public:
      static bool convert(Process & p, const string & fname, Finder & f);
  };
  
  template<class INP, class OUTP>
  bool VolumeConverter<carto::VolumeRef<INP>, 
                       carto::VolumeRef<OUTP> >::convert(
                                   Process & p, 
                                   const string & fname, 
                                   Finder & f)
  {
    string fmt = f.format();
    Reader<carto::VolumeRef<INP> > r( fname );
    InternalConverter<carto::VolumeRef<OUTP> > & ic
        = (InternalConverter<carto::VolumeRef<OUTP> > &) p;
    carto::VolumeRef<INP> input;
    
    r.read(input, ic.border, &fmt, ic.frame);

    ChannelSelector<carto::VolumeRef<INP>, carto::VolumeRef<OUTP> > selector;
    ic.data = selector.select(input, ic.channel);

    return( true );
  }
  
  //
  // ChangeVoxelTypeTrait allows to keep container type while changing
  // voxel type
  //
  template<class T, class V>
  class ChangeVoxelTypeTrait
  {};
  
  template<class T, class V>
  class ChangeVoxelTypeTrait<VolumeRef<T>, V>
  {
  public:
      typedef VolumeRef<V> ContainerType;
  };

  //
  // Channel reader
  //
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
  void ChannelReader<OUTP>::read( OUTP & data, 
                                  uint8_t channel, int border, 
                                  const std::string*, int frame )
  {
    InternalConverter<OUTP> ic( channel, border, frame, data );

    map<std::string, set<string> >::const_iterator idt
      = _allowedTypes.find( "Volume" );
    if( idt != _allowedTypes.end() )
    {
      const set<string> &vt = idt->second;
      set<string>::const_iterator e = vt.end();
      if( vt.find( "S8" ) != e )
        ic.registerProcessType( "Volume", "S8",
                                &VolumeConverter<
                                    typename 
                                    ChangeVoxelTypeTrait<OUTP, int8_t>
                                        ::ContainerType, 
                                    OUTP >::convert);
      if( vt.find( "U8" ) != e )
        ic.registerProcessType( "Volume", "U8",
                                &VolumeConverter<
                                    typename 
                                    ChangeVoxelTypeTrait<OUTP, uint8_t>
                                        ::ContainerType, 
                                    OUTP >::convert );
      if( vt.find( "S16" ) != e )
        ic.registerProcessType( "Volume", "S16",
                                &VolumeConverter<
                                    typename 
                                    ChangeVoxelTypeTrait<OUTP, int16_t>
                                        ::ContainerType, 
                                    OUTP >::convert );
      if( vt.find( "U16" ) != e )
        ic.registerProcessType( "Volume", "U16",
                                &VolumeConverter<
                                    typename 
                                    ChangeVoxelTypeTrait<OUTP, uint16_t>
                                        ::ContainerType, 
                                    OUTP >::convert );
      if( vt.find( "S32" ) != e )
        ic.registerProcessType( "Volume", "S32",
                                &VolumeConverter<
                                    typename 
                                    ChangeVoxelTypeTrait<OUTP, int32_t>
                                        ::ContainerType, 
                                    OUTP >::convert );
      if( vt.find( "U32" ) != e )
        ic.registerProcessType( "Volume", "U32",
                                &VolumeConverter<
                                    typename 
                                    ChangeVoxelTypeTrait<OUTP, uint32_t>
                                        ::ContainerType, 
                                    OUTP >::convert );
      if( vt.find( "FLOAT" ) != e )
        ic.registerProcessType( "Volume", "FLOAT",
                                &VolumeConverter<
                                    typename 
                                    ChangeVoxelTypeTrait<OUTP, float>
                                        ::ContainerType, 
                                    OUTP >::convert );
      if( vt.find( "DOUBLE" ) != e )
        ic.registerProcessType( "Volume", "DOUBLE",
                                &VolumeConverter<
                                    typename 
                                    ChangeVoxelTypeTrait<OUTP, double>
                                        ::ContainerType, 
                                    OUTP >::convert );
      if( vt.find( "RGB" ) != e )
        ic.registerProcessType( "Volume", "RGB",
                                &VolumeConverter<
                                    typename 
                                    ChangeVoxelTypeTrait<OUTP, AimsRGB>
                                        ::ContainerType, 
                                    OUTP >::convert );
      if( vt.find( "RGBA" ) != e )
        ic.registerProcessType( "Volume", "RGBA",
                                &VolumeConverter<
                                    typename 
                                    ChangeVoxelTypeTrait<OUTP, AimsRGBA>
                                        ::ContainerType, 
                                    OUTP >::convert );
      if( vt.find( "HSV" ) != e )
        ic.registerProcessType( "Volume", "HSV",
                                &VolumeConverter<
                                    typename 
                                    ChangeVoxelTypeTrait<OUTP, AimsHSV>
                                        ::ContainerType, 
                                    OUTP >::convert );
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
  template class VolumeConverter<carto::VolumeRef<INP >, \
                                 carto::VolumeRef<OUTP > >;
      

  INSTANCIATE_INTERNAL_CONVERT( int8_t, int8_t )
  INSTANCIATE_INTERNAL_CONVERT( uint8_t, int8_t )
  INSTANCIATE_INTERNAL_CONVERT( int16_t, int8_t )
  INSTANCIATE_INTERNAL_CONVERT( uint16_t, int8_t )
  INSTANCIATE_INTERNAL_CONVERT( int32_t, int8_t )
  INSTANCIATE_INTERNAL_CONVERT( uint32_t, int8_t )
  INSTANCIATE_INTERNAL_CONVERT( float, int8_t )
  INSTANCIATE_INTERNAL_CONVERT( double, int8_t )
  INSTANCIATE_INTERNAL_CONVERT( AimsRGB, int8_t )
  INSTANCIATE_INTERNAL_CONVERT( AimsRGBA, int8_t )
  INSTANCIATE_INTERNAL_CONVERT( AimsHSV, int8_t )

  INSTANCIATE_INTERNAL_CONVERT( int8_t, uint8_t )
  INSTANCIATE_INTERNAL_CONVERT( uint8_t, uint8_t )
  INSTANCIATE_INTERNAL_CONVERT( int16_t, uint8_t )
  INSTANCIATE_INTERNAL_CONVERT( uint16_t, uint8_t )
  INSTANCIATE_INTERNAL_CONVERT( int32_t, uint8_t )
  INSTANCIATE_INTERNAL_CONVERT( uint32_t, uint8_t )
  INSTANCIATE_INTERNAL_CONVERT( float, uint8_t )
  INSTANCIATE_INTERNAL_CONVERT( double, uint8_t )
  INSTANCIATE_INTERNAL_CONVERT( AimsRGB, uint8_t )
  INSTANCIATE_INTERNAL_CONVERT( AimsRGBA, uint8_t )
  INSTANCIATE_INTERNAL_CONVERT( AimsHSV, uint8_t )

  INSTANCIATE_INTERNAL_CONVERT( int8_t, int16_t )
  INSTANCIATE_INTERNAL_CONVERT( uint8_t, int16_t )
  INSTANCIATE_INTERNAL_CONVERT( int16_t, int16_t )
  INSTANCIATE_INTERNAL_CONVERT( uint16_t, int16_t )
  INSTANCIATE_INTERNAL_CONVERT( int32_t, int16_t )
  INSTANCIATE_INTERNAL_CONVERT( uint32_t, int16_t )
  INSTANCIATE_INTERNAL_CONVERT( float, int16_t )
  INSTANCIATE_INTERNAL_CONVERT( double, int16_t )
  INSTANCIATE_INTERNAL_CONVERT( AimsRGB, int16_t )
  INSTANCIATE_INTERNAL_CONVERT( AimsRGBA, int16_t )
  INSTANCIATE_INTERNAL_CONVERT( AimsHSV, int16_t )

  INSTANCIATE_INTERNAL_CONVERT( int8_t, uint16_t )
  INSTANCIATE_INTERNAL_CONVERT( uint8_t, uint16_t )
  INSTANCIATE_INTERNAL_CONVERT( int16_t, uint16_t )
  INSTANCIATE_INTERNAL_CONVERT( uint16_t, uint16_t )
  INSTANCIATE_INTERNAL_CONVERT( int32_t, uint16_t )
  INSTANCIATE_INTERNAL_CONVERT( uint32_t, uint16_t )
  INSTANCIATE_INTERNAL_CONVERT( float, uint16_t )
  INSTANCIATE_INTERNAL_CONVERT( double, uint16_t )
  INSTANCIATE_INTERNAL_CONVERT( AimsRGB, uint16_t )
  INSTANCIATE_INTERNAL_CONVERT( AimsRGBA, uint16_t )
  INSTANCIATE_INTERNAL_CONVERT( AimsHSV, uint16_t )

  INSTANCIATE_INTERNAL_CONVERT( int8_t, int32_t )
  INSTANCIATE_INTERNAL_CONVERT( uint8_t, int32_t )
  INSTANCIATE_INTERNAL_CONVERT( int16_t, int32_t )
  INSTANCIATE_INTERNAL_CONVERT( uint16_t, int32_t )
  INSTANCIATE_INTERNAL_CONVERT( int32_t, int32_t )
  INSTANCIATE_INTERNAL_CONVERT( uint32_t, int32_t )
  INSTANCIATE_INTERNAL_CONVERT( float, int32_t )
  INSTANCIATE_INTERNAL_CONVERT( double, int32_t )
  INSTANCIATE_INTERNAL_CONVERT( AimsRGB, int32_t )
  INSTANCIATE_INTERNAL_CONVERT( AimsRGBA, int32_t )
  INSTANCIATE_INTERNAL_CONVERT( AimsHSV, int32_t )

  INSTANCIATE_INTERNAL_CONVERT( int8_t, uint32_t )
  INSTANCIATE_INTERNAL_CONVERT( uint8_t, uint32_t )
  INSTANCIATE_INTERNAL_CONVERT( int16_t, uint32_t )
  INSTANCIATE_INTERNAL_CONVERT( uint16_t, uint32_t )
  INSTANCIATE_INTERNAL_CONVERT( int32_t, uint32_t )
  INSTANCIATE_INTERNAL_CONVERT( uint32_t, uint32_t )
  INSTANCIATE_INTERNAL_CONVERT( float, uint32_t )
  INSTANCIATE_INTERNAL_CONVERT( double, uint32_t )
  INSTANCIATE_INTERNAL_CONVERT( AimsRGB, uint32_t )
  INSTANCIATE_INTERNAL_CONVERT( AimsRGBA, uint32_t )
  INSTANCIATE_INTERNAL_CONVERT( AimsHSV, uint32_t )

  INSTANCIATE_INTERNAL_CONVERT( int8_t, float )
  INSTANCIATE_INTERNAL_CONVERT( uint8_t, float )
  INSTANCIATE_INTERNAL_CONVERT( int16_t, float )
  INSTANCIATE_INTERNAL_CONVERT( uint16_t, float )
  INSTANCIATE_INTERNAL_CONVERT( int32_t, float )
  INSTANCIATE_INTERNAL_CONVERT( uint32_t, float )
  INSTANCIATE_INTERNAL_CONVERT( float, float )
  INSTANCIATE_INTERNAL_CONVERT( double, float )
  INSTANCIATE_INTERNAL_CONVERT( AimsRGB, float )
  INSTANCIATE_INTERNAL_CONVERT( AimsRGBA, float )
  INSTANCIATE_INTERNAL_CONVERT( AimsHSV, float )

  INSTANCIATE_INTERNAL_CONVERT( int8_t, double )
  INSTANCIATE_INTERNAL_CONVERT( uint8_t, double )
  INSTANCIATE_INTERNAL_CONVERT( int16_t, double )
  INSTANCIATE_INTERNAL_CONVERT( uint16_t, double )
  INSTANCIATE_INTERNAL_CONVERT( int32_t, double )
  INSTANCIATE_INTERNAL_CONVERT( uint32_t, double )
  INSTANCIATE_INTERNAL_CONVERT( float, double )
  INSTANCIATE_INTERNAL_CONVERT( double, double )
  INSTANCIATE_INTERNAL_CONVERT( AimsRGB, double )
  INSTANCIATE_INTERNAL_CONVERT( AimsRGBA, double )
  INSTANCIATE_INTERNAL_CONVERT( AimsHSV, double )

  template class ChannelReader< VolumeRef<int8_t> >;
  template class ChannelReader< VolumeRef<uint8_t> >;
  template class ChannelReader< VolumeRef<int16_t> >;
  template class ChannelReader< VolumeRef<uint16_t> >;
  template class ChannelReader< VolumeRef<int32_t> >;
  template class ChannelReader< VolumeRef<uint32_t> >;
  template class ChannelReader< VolumeRef<float> >;
  template class ChannelReader< VolumeRef<double> >;
}
