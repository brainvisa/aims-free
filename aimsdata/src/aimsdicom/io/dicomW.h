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

#ifndef AIMS_IO_DICOMW_H
#define AIMS_IO_DICOMW_H

#include <cartobase/stream/directory.h>
#include <cartobase/stream/fileutil.h>
#include <aims/data/data.h>
#include <aims/io/scaledcoding.h>
#include <string>

// #ifndef HAVE_CONFIG_H // needed by osconfig.h to include cfunix.h???
// #define HAVE_CONFIG_H 1
// #endif
#include <dctk.h>
#ifdef UID_RawDataStorage // #if (OFFIS_DCMTK_VERSION_NUMBER-0 > 351)
#include <dcistrmf.h>
#endif

namespace aims
{
  template <class T> class DicomWriter;

  template <class T>
  DicomWriter<T>& operator << ( DicomWriter<T>& writer, const AimsData<T>& thing );

  template <class T>
  class DicomWriter
  {
  public:
    DicomWriter( const std::string& name ) 
      : _name( name ) { }
    virtual ~DicomWriter() { }

    /** Write the data with "name" file name to disk
        @param thing const reference to the data to write
    */
    bool write( const AimsData<T>& thing );

    /// Output stream operator
    friend DicomWriter<T>& operator << <> (DicomWriter<T>& writer, const AimsData<T>& thing);

  private:
    std::string   _name;

  };

  template<class T>
  bool DicomWriter< T >::write( const AimsData<T> & thing )
  {
    /* fail if no extension has been specified, so that DICOM is not the
       default writer format when no ext is given
    */
    if( carto::FileUtil::extension( _name ).empty() )
      return false;

    int dx = thing.dimX();
    int dy = thing.dimY();
    unsigned long lineSize = dx * sizeof( T );
    unsigned long n = lineSize * dy;
    char uid[ 100 ];
    Uint8* data = new Uint8[ n ];

    if ( !data )
    {
      return false;
    }

    std::string filename_noext( _name );
    std::string::size_type pos = filename_noext.rfind( '.' );

    if ( pos != std::string::npos )
    {
      filename_noext = filename_noext.substr( 0, pos );
    }

    carto::Directory directory( carto::FileUtil::dirname( filename_noext ) );

    if ( !directory.isValid() )
    {
      /* It's difficult to say wheter the writer should create a directory
         or not. Sometimes we would like to create a dicom as a directory
         (maybe without a file extension, but this is currently forbidden
         about 20 lines earlier in this code), and sometimes we would like to
         write a series of files in an existing directory.
         Right now (2010/12/30) we disable making directories because if we
         do, using a Writer in a non-existing directory will always result in
         a Dicom file: writing a_directory/image.nii with a_directory not
         existing will fail for the nifti writer, and work for the dicom
         writer.
       */
      return false;
      // directory.makedirs();
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
#if OFFIS_DCMTK_VERSION_NUMBER >= 360
    dataset->insertEmptyElement( DCM_PatientName );
#else
    dataset->insertEmptyElement( DCM_PatientsName );
#endif
    dataset->insertEmptyElement( DCM_PatientID );
#if OFFIS_DCMTK_VERSION_NUMBER >= 360
    dataset->insertEmptyElement( DCM_PatientBirthDate );
    dataset->insertEmptyElement( DCM_PatientSex );
#else
    dataset->insertEmptyElement( DCM_PatientsBirthDate );
    dataset->insertEmptyElement( DCM_PatientsSex );
#endif

    // Study
    dataset->putAndInsertString( DCM_StudyInstanceUID,
                                 dcmGenerateUniqueIdentifier(
                                                        uid,
                                                        SITE_STUDY_UID_ROOT ) );
    dataset->insertEmptyElement( DCM_StudyDate );
    dataset->insertEmptyElement( DCM_StudyTime );
#if OFFIS_DCMTK_VERSION_NUMBER >= 360
    dataset->insertEmptyElement( DCM_ReferringPhysicianName );
#else
    dataset->insertEmptyElement( DCM_ReferringPhysiciansName );
#endif

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

    PythonHeader * hdr = (PythonHeader *)thing.header();
    bool usescale = false;
    if ( hdr->hasProperty( "scale_factor_applied" ) )
    {
      double slope, offset;
      hdr->getProperty( "scale_factor_applied", usescale );

      if (usescale)
      { 
        if ( hdr->hasProperty( "scale_factor" ) )
        {
          hdr->getProperty( "scale_factor", slope );
          std::ostringstream oslope;
          oslope << slope;
          dataset->putAndInsertString( DCM_RescaleSlope, oslope.str().c_str() );
        }
 
        if ( hdr->hasProperty( "scale_offset" ) )
        {
          hdr->getProperty( "scale_offset", offset );
          std::ostringstream ooffset;
          ooffset << offset;
          dataset->putAndInsertString( DCM_RescaleIntercept, ooffset.str().c_str() );
        }
      }
    }

    std::ostringstream pixelSpacingX, pixelSpacingY;
    pixelSpacingX << thing.sizeX();
    std::string str = pixelSpacingX.str();

    if ( str.length() > 16 )
    {
      str.resize( 16 );
    }

    str += "\\";
    pixelSpacingY << thing.sizeY();
    std::string str2 = pixelSpacingY.str();

    if ( str2.length() > 16 )
    {
      str2.resize( 16 );
    }

    str += str2;
    dataset->putAndInsertString( DCM_PixelSpacing, str.c_str() );

    int dz = thing.dimZ();
    float sz = thing.sizeZ();

    if ( dz > 1 )
    {
      std::ostringstream sliceThickness;
      sliceThickness << sz;

      std::string slThick = sliceThickness.str();

      if ( slThick.length() > 16 )
      {
        slThick.resize( 16 );
      }

      dataset->putAndInsertString( DCM_SliceThickness, slThick.c_str() );
    }

    int y, z, t, iN = 1;
    int dt = thing.dimT();
    typename AimsData<T>::const_iterator it = thing.begin() + thing.oFirstPoint();
    Float64 sliceLocation;
    for ( t = 0; t < dt; t++ )
    {
      sliceLocation = 0.0;

      for ( z = 0; z < dz; z++, iN++ )
      {
        std::ostringstream instanceNumber;
        std::ostringstream sliceLocationStr;
        Uint8* dptr = data;

        for ( y = 0; y < dy; y++ )
        {
          std::memcpy( dptr, (const void*)it, lineSize );
          dptr += lineSize;
          it += thing.oLine();
        }

        it += thing.oLineBetweenSlice();
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

      it += thing.oSliceBetweenVolume();
    }

    delete[] data;
    return true;
  }


  template <>
  bool DicomWriter<float>::write( const AimsData<float>& thing )
  {
    try
    {
        soma::ScaledEncodingInfo info;
        AimsData<int16_t> dcmdata( thing.dimX(), 
                                   thing.dimY(),
                                   thing.dimZ(),
                                   thing.dimT() );
        dcmdata.setSizeXYZT( thing.sizeX(), 
                             thing.sizeY(), 
                             thing.sizeZ(),
                             thing.sizeT() );
    
        if( thing.header() )
          dcmdata.setHeader( thing.header()->cloneHeader( true ) );
    
        info = ScaledEncoding<float, int16_t>::rescale( thing, dcmdata );
        PythonHeader * hdr = (PythonHeader *)dcmdata.header();
        hdr->setProperty( "scale_factor_applied", true );
        hdr->setProperty( "scale_factor", info.slope() );
        hdr->setProperty( "scale_offset", info.offset() - info.slope() * (double)std::numeric_limits<int16_t>::min() );

        DicomWriter<int16_t>	w( _name );
        return w.write( dcmdata );
    }
    catch( std::exception & e )
    {
	return false;
    }
  }



  template <class T> inline
  DicomWriter<T>& operator << ( DicomWriter<T>& writer, const AimsData<T>& thing )
  {
    writer.write(thing);
    return writer;
  }
}

#endif





