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
 *  DICOM data reader class
 */

#include <aims/io/dicomR.h>
#include <aims/config/aimsdata_config.h>
#include <cartobase/exception/assert.h>
#include <cartobase/allocator/mappingcopy.h>
#include <cartobase/allocator/mappingro.h>
#include <cartobase/allocator/mappingrw.h>
#include <cartobase/exception/format.h>
#include <cartobase/exception/ioexcept.h>
#include <cartobase/datasource/filedatasource.h>
#include <memory>
#include <dcerror.h>

using namespace std;

namespace aims
{
  template< class T >
    void DicomReader< T >::read( AimsData< T >& thing, 
                                 const carto::AllocatorContext & context, 
                                 carto::Object options )
    {
      // std::cout << "Reading image" << std::endl ;
      int	frame = -1, borderWidth = 0;
      options->getProperty( "frame", frame );
      options->getProperty( "border", borderWidth );

      if( frame >= 0 )
        std::cerr << "Warning : DICOM single frame reading not implemented - " 
                  << "reading whole volume\n";

      // make sure data dictionary is loaded
      if ( !dcmDataDict.isDictionaryLoaded() ) 
        {
          std::cerr << "No data dictionary loaded, check environment " 
                    << "variable: " << DCM_DICT_ENVIRONMENT_VARIABLE 
                    << std::endl;
        }

      // load the header of the first file
      std::auto_ptr<aims::DicomHeader> hdr
        = std::auto_ptr<aims::DicomHeader>( new aims::DicomHeader( _name ) );
      int num = hdr->read();

      if ( num < 0 )
        {
          throw carto::format_error( _name );
        }

      // check if data type is compatible with type T
      int nbits;
      bool status;
      status = hdr->getProperty( "bits_allocated", nbits );
      if ( status && ( nbits / 8 ) > (int)sizeof( T ) )
        {
          /*std::cerr << "File bits allocated is : " << nbits;
            std::cerr << ". Incompatible with your data type" << std::endl;*/
          throw carto::format_error( _name );
        }

      // create an AimsData
      std::vector< int > volDim;
      hdr->getProperty( "volume_dimension", volDim );

      carto::AllocatorContext 
        cont2( context.accessMode(), 
               carto::rc_ptr<carto::DataSource>
               ( new carto::FileDataSource
                 ( _name, 0, carto::DataSource::Read ) ), false, 
               context.useFactor() );

      AimsData< T > data( volDim[ 0 ], volDim[ 1 ], volDim[ 2 ], volDim[ 3 ], 
                          borderWidth, cont2 );
      std::vector< float > voxSize;
      hdr->getProperty( "voxel_size", voxSize );
      float sizeT;
      hdr->getProperty( "time_resolution", sizeT );
      data.setSizeXYZT( voxSize[ 0 ], voxSize[ 1 ], voxSize[ 2 ], sizeT );

      // read each file and fill data
      //int relative = hdr->slices().begin()->first;
      int slice = 0;
      for ( std::map< int, aims::DicomHeader::FileElement >::const_iterator i =
              hdr->slices().begin(); i != hdr->slices().end(); ++i )
        {
//           std::cerr << "DICOM: readData: slice = " << slice
//                << " instance_number = "  << i->first << std::endl;
          readData( i->second.name(), data, slice, hdr.get() );
          ++slice;
        }
        thing = data;

      thing.setHeader( hdr.release() );
    }


  template< class T >
    void DicomReader< T >::readData( const std::string& filename, 
                                     AimsData< T >& thing, int instance,
                                     const aims::DicomHeader *hdr )
    {
      // open file
#ifdef UID_RawDataStorage // #if (OFFIS_DCMTK_VERSION_NUMBER-0 > 351)
      DcmInputFileStream  stream( filename.c_str() );
      if ( ! stream.good() )
#else
        DcmFileStream stream( filename.c_str(), DCM_ReadMode );
      if ( stream.GetError() != EC_Normal ) 
#endif
        {
          throw carto::open_error( "Not a Dicom file", filename );
        }
	
      // allocate file
      DcmFileFormat dfile;

      // read file
      dfile.transferInit();
      dfile.read( stream, EXS_Unknown, EGL_noChange );
      dfile.transferEnd();
      if ( dfile.error() != EC_Normal )
        throw carto::wrong_format_error( dcmErrorConditionToString
                                         ( dfile.error() ), filename );

      DcmStack stack;

      // get the DICOM pixel data pointer and fill the AimsData
      if ( dfile.search( DCM_PixelData, stack ) == EC_Normal ) 
        {
          ASSERT( stack.top()->ident() == EVR_PixelData );
          DcmPixelData *object = (DcmPixelData *)stack.top();

          // Dicom pixel data is only encoded in byte or short
          Uint8  *byteValues = NULL;
          Uint16 *shortValues = NULL;
          DcmEVR evr = object->getVR();

          if ( evr == EVR_OW )
            object->getUint16Array( shortValues );
          else // evr == EVR_OB
            object->getUint8Array( byteValues );

          if ( dfile.search( DCM_BitsAllocated, stack ) == EC_Normal )
            {
              ASSERT( stack.top()->ident() == EVR_US );
              DcmUnsignedShort *object = (DcmUnsignedShort *)stack.top();
              Uint16 nbits;
              object->getUint16( nbits );
              if ( nbits == 16 && byteValues )
                {
                  shortValues = (Uint16 *)byteValues;
                  byteValues = 0;
                }
            }

	  double scale_factor = 1.0;
          std::string modality;
          hdr->getProperty( "modality", modality );
          if ( modality == "PT" )
          {
            std::string unit;
            hdr->getProperty( "image_unit", unit );

            if ( unit == "Counts" )
            {
              std::string manufacturer;
              hdr->getProperty( "manufacturer", manufacturer );

              if ( manufacturer == "Philips Medical Systems" )
              {
                Float64 tmpScale;

                if ( dfile.search( DcmTagKey( 0x7053, 0x1009 ), stack ) == 
                     EC_Normal )
                {
		  ASSERT( stack.top()->ident() == EVR_DS );
		  DcmDecimalString *object = (DcmDecimalString *)stack.top();
		  object->getFloat64( tmpScale );
		  scale_factor = (double)tmpScale;
	        }
              }
            }
          }

          if ( shortValues )
            {
              std::string tr_syntax;
              hdr->getProperty( "transfer_syntax", tr_syntax );
              if ( tr_syntax == "1.2.840.113619.5.2" )
                { // This is a private GE Implicit VR Big Endian transfer syntax
                    std::cout << "3.\n";

                  Uint32 byteLength = object->getLength();
                  swapBytes( shortValues, byteLength, sizeof( Uint16 ) );
                }
	      
	      Uint16 *sptr = shortValues;
              int z0, ze, frame;

              if ( modality == "NM" )
              {
                z0 = 0;
                ze = thing.dimZ();
                frame = 0;
              }
              else
              {
	        z0 = instance % thing.dimZ();
                ze = z0 + 1;
	        frame = instance / thing.dimZ();
                if( frame >= thing.dimT() )
                {
                  std::cerr << "DICOM reader error: size/slice number mismatch"
                            << std::endl;
                  return;
                }

	        if ( hdr->reverseZ() )  {
		  z0 = thing.dimZ() - z0 - 1;
                  ze = z0 + 1;
	  	  //if ( slice == 0 ) std::cerr << "DICOM Z axis reversed" 
		  // << std::endl;
	        }
              }
		
	      bool useScaleAndIntercept = true ;
	      if( useScaleAndIntercept ){
		double wc = -1.0;
		double ww = -1.0;
		Float64 gtmp;
		
		if ( dfile.search( DCM_WindowCenter, stack ) == EC_Normal )
		  {
		    ASSERT( stack.top()->ident() == EVR_DS );
		    DcmDecimalString *object = (DcmDecimalString *)stack.top();
		    object->getFloat64( gtmp );
		    wc = (double)gtmp;
		  }
		
		if ( dfile.search( DCM_WindowWidth, stack ) == EC_Normal )
		  {
		    ASSERT( stack.top()->ident() == EVR_DS );
		    DcmDecimalString *object = (DcmDecimalString *)stack.top();
		    object->getFloat64( gtmp );
		    ww = (double)gtmp;
		  }
		
		double slope = 1.0;
		double inter = 0.0;
		
		if ( dfile.search( DCM_RescaleSlope, stack ) == EC_Normal )
		  {
		    ASSERT( stack.top()->ident() == EVR_DS );
		    DcmDecimalString *object = (DcmDecimalString *)stack.top();
		    object->getFloat64( gtmp );
		    slope = (double)gtmp;
		  }
		
		if ( dfile.search( DCM_RescaleIntercept, stack ) == EC_Normal )
		  {
		    ASSERT( stack.top()->ident() == EVR_DS );
		    DcmDecimalString *object = (DcmDecimalString *)stack.top();
		    object->getFloat64( gtmp );
		    inter = (double)gtmp;
		  }
		
/* 		double min = -32767.; */
/* 		double max = 32767; */
		
/* 		if ( wc >= 0.0 && ww >= 0.0 ) */
/* 		  { */
/* 		    min = wc - ww / 2.0; */
/* 		    max = wc + ww / 2.0; */
/* 		  } */
		
		int i, j, k;
		int dx = thing.dimX();
		int dy = thing.dimY();
		double signal ;
                for ( k=z0; k<ze; k++ )
  		  for ( j=0; j<dy; j++ )
		    for ( i=0; i<dx; i++ )
		    {
                      // Data is stored as int16_t so we must read it as int16_t otherwise 
                      // signed values will be lost
		      signal = ((double)(int16_t)*sptr++) * slope + inter;
                      signal *= scale_factor;
		      thing( i, j, k, frame ) = (T)signal;
		    }
		
	      } else {
		int i, j, k;
		int dx = thing.dimX();
		int dy = thing.dimY();
                double signal;
                for ( k=z0; k<ze; k++ )
		  for ( j=0; j<dy; j++ )
		    for ( i=0; i<dx; i++ )
                    {
		      signal = (double)(int16_t)*sptr++;
                      signal *= scale_factor;
		      thing( i, j, k, frame ) = (T)signal;
                    }
	      }
            }
          else if ( byteValues )
            {
              Uint8 *bptr = byteValues;
              int z0, ze, frame;

              if ( modality == "NM" )
              {
                z0 = 0;
                ze = thing.dimZ();
                frame = 0;
              }
              else
              {
	        z0 = instance % thing.dimZ();
                ze = z0 + 1;
                frame = instance / thing.dimZ();

                if ( hdr->reverseZ() ) {
                  z0 = thing.dimZ() - z0 - 1;
                  ze = z0 + 1;
                  //if ( slice == 0 ) std::cerr << "DICOM Z axis reversed" 
                  // << std::endl;
                }
              }

              double wc = -1.0;
              double ww = -1.0;
              Float64 gtmp;

              if ( dfile.search( DCM_WindowCenter, stack ) == EC_Normal )
                {
                  ASSERT( stack.top()->ident() == EVR_DS );
                  DcmDecimalString *object = (DcmDecimalString *)stack.top();
                  object->getFloat64( gtmp );
                  wc = (double)gtmp;
                }

              if ( dfile.search( DCM_WindowWidth, stack ) == EC_Normal )
                {
                  ASSERT( stack.top()->ident() == EVR_DS );
                  DcmDecimalString *object = (DcmDecimalString *)stack.top();
                  object->getFloat64( gtmp );
                  ww = (double)gtmp;
                }

              double slope = 1.0;
              double inter = 0.0;

              if ( dfile.search( DCM_RescaleSlope, stack ) == EC_Normal )
                {
                  ASSERT( stack.top()->ident() == EVR_DS );
                  DcmDecimalString *object = (DcmDecimalString *)stack.top();
                  object->getFloat64( gtmp );
                  slope = (double)gtmp;
                }

              if ( dfile.search( DCM_RescaleIntercept, stack ) == EC_Normal )
                {
                  ASSERT( stack.top()->ident() == EVR_DS );
                  DcmDecimalString *object = (DcmDecimalString *)stack.top();
                  object->getFloat64( gtmp );
                  inter = (double)gtmp;
                }
	      
/*              double min = 0.0;
              double max = 255.0;
	      
              if ( wc >= 0.0 && ww >= 0.0 )
                {
                  min = wc - ww / 2.0;
                  max = wc + ww / 2.0;
                }
*/
	      
              int i, j, k;
              int dx = thing.dimX();
              int dy = thing.dimY();
              double signal; //, coef = 255.0 / ( max - min );
              for ( k=z0; k<ze; k++ )
                for ( j=0; j<dy; j++ )
                  for ( i=0; i<dx; i++ )
                  {
		    std::cout << "slope = " << slope << " & intercept = " << inter << std::endl ;
                    signal = ((double)(int8_t)*bptr++) * slope + inter;
                    signal *= scale_factor;

/*                    if ( signal <= min )  signal = 0.0;
                    else if ( signal >= max )  signal = 255.0;
                    else signal = ( signal - min ) * coef;
*/

                    thing( i, j, k, frame ) = (T)signal;
                  }
            }
        }
    }


  template class DicomReader< uint8_t >;
  template class DicomReader< int8_t >;
  template class DicomReader< uint16_t >;
  template class DicomReader< int16_t >;
  template class DicomReader< uint32_t >;
  template class DicomReader< int32_t >;
  template class DicomReader< float >;
  template class DicomReader< double >;
  
  // ### remove after everything has been moved to intN_t/uintN_t
  template class DicomReader< char >;
  template class DicomReader< long >;
  template class DicomReader< unsigned long >;
}
