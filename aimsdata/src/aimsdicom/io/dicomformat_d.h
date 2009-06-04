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

#ifndef AIMS_IO_DICOMFORMAT_D_H
#define AIMS_IO_DICOMFORMAT_D_H

#include <aims/io/dicomformat.h>
#include <aims/io/dicomR.h>
#include <aims/data/data.h>
#include <cartobase/stream/directory.h>
#include <cartobase/stream/fileutil.h>

namespace aims
{

  template<class T>
  bool DicomFormat<T>::read( const std::string & filename, AimsData<T> & vol, 
                             const carto::AllocatorContext & context, 
                             carto::Object options )
  {
    DicomReader<T>	r( filename );
    r.read( vol, context, options );
    return( true );
  }


  template<class T>
  bool DicomFormat< T >::write( const std::string & filename, 
                                const AimsData<T> & vol, 
                                bool /* ascii */ )
  {
    int dx = vol.dimX();
    int dy = vol.dimY();
    unsigned long n = dx * dy * sizeof( T );
    char uid[ 100 ];
    Uint8* data = new Uint8[ n ];

    if ( !data )
    {
      return false;
    }

    std::string filename_noext( filename );
    std::string::size_type pos = filename_noext.rfind( '.' );

    if ( pos != std::string::npos )
    {
      filename_noext = filename_noext.substr( 0, pos );
    }

    carto::Directory directory( carto::FileUtil::dirname( filename_noext ) );

    if ( !directory.isValid() )
    {
      directory.makedirs();
    }

    DcmFileFormat fileFormat;
    DcmDataset* dataset = fileFormat.getDataset();

    // SOP common
    dataset->putAndInsertString( DCM_SOPClassUID, 
                                 UID_SecondaryCaptureImageStorage );
    dataset->putAndInsertString( DCM_SOPInstanceUID,
                                 dcmGenerateUniqueIdentifier(
                                                     uid,
                                                     SITE_INSTANCE_UID_ROOT ) );

    // Patient 
    dataset->insertEmptyElement( DCM_PatientsName );
    dataset->insertEmptyElement( DCM_PatientID );
    dataset->insertEmptyElement( DCM_PatientsBirthDate );
    dataset->insertEmptyElement( DCM_PatientsSex );

    // Study
    dataset->putAndInsertString( DCM_StudyInstanceUID,
                                 dcmGenerateUniqueIdentifier(
                                                        uid,
                                                        SITE_STUDY_UID_ROOT ) );
    dataset->insertEmptyElement( DCM_StudyDate );
    dataset->insertEmptyElement( DCM_StudyTime );
    dataset->insertEmptyElement( DCM_ReferringPhysiciansName );

    // Series
    dataset->putAndInsertString( DCM_Modality,
                                 dcmSOPClassUIDToModality( 
                                           UID_SecondaryCaptureImageStorage ) );
    dataset->putAndInsertString( DCM_SeriesInstanceUID,
                                 dcmGenerateUniqueIdentifier(
                                                       uid,
                                                       SITE_SERIES_UID_ROOT ) );
    dataset->insertEmptyElement( DCM_SeriesNumber );

    // Equipment
    dataset->putAndInsertString( DCM_ConversionType, "WSD" );

    // Image
    int nBits = 8 * (int)sizeof( T );

    dataset->putAndInsertUint16( DCM_SamplesPerPixel, 1 );
    dataset->putAndInsertString( DCM_PhotometricInterpretation, "MONOCHROME2" );
    dataset->putAndInsertUint16( DCM_Rows, (Uint16)dy );
    dataset->putAndInsertUint16( DCM_Columns, (Uint16)dx );
    dataset->putAndInsertUint16( DCM_BitsAllocated, (Uint16)nBits );
    dataset->putAndInsertUint16( DCM_BitsStored, (Uint16)nBits );
    dataset->putAndInsertUint16( DCM_HighBit, (Uint16)( nBits - 1 ) );
    dataset->putAndInsertUint16( DCM_PixelRepresentation, 0 );

    std::ostringstream pixelSpacingX, pixelSpacingY;
    pixelSpacingX << vol.sizeX();
    std::string str = pixelSpacingX.str();

    if ( str.length() > 16 )
    {
      str.resize( 16 );
    }

    str += "\\";
    pixelSpacingY << vol.sizeY();
    std::string str2 = pixelSpacingY.str();

    if ( str2.length() > 16 )
    {
      str2.resize( 16 );
    }

    str += str2;
    dataset->putAndInsertString( DCM_PixelSpacing, str.c_str() );

    int x, y, z, t, iN = 1;
    int dz = vol.dimZ();
    int dt = vol.dimT();
    float sz = vol.sizeZ();
    typename AimsData<T>::const_iterator it = vol.begin() + vol.oFirstPoint();
    Float64 sliceLocation;

    for ( t = 0; t < dt; t++ )
    {
      sliceLocation = 0.0;

      for ( z = 0; z < dz; z++, iN++ )
      {
        std::ostringstream instanceNumber;
        std::ostringstream sliceLocationStr;
        T* dptr = (T*)data;

        for ( y = 0; y < dy; y++ )
        {
          for ( x = 0; x < dx; x++ )
          {
            *dptr++ = *it++;
          }

          it += vol.oPointBetweenLine();
        }

        it += vol.oLineBetweenSlice();
        instanceNumber << iN;
        dataset->putAndInsertString( DCM_InstanceNumber, 
                                     instanceNumber.str().c_str() );

        sliceLocationStr << sliceLocation;
        std::string slStr = sliceLocationStr.str();

        if ( slStr.length() > 16 )
        {
          slStr.resize( 16 );
        }

        dataset->putAndInsertString( DCM_SliceLocation, slStr.c_str() );
        sliceLocation += sz;

        dataset->putAndInsertUint8Array( DCM_PixelData, data, n );

        std::string imageName = filename_noext + 
                                instanceNumber.str().c_str() + ".dcm";

        if ( fileFormat.saveFile( imageName.c_str(), 
                                  EXS_LittleEndianExplicit ).bad() )
        {
          delete[] data;
          return false;
        } 
      }

      it += vol.oSliceBetweenVolume();
    }

    delete[] data;
    return true;
  }

}

#endif
