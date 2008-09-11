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

/*
 *  lecture de fichiers DICOM avec DCMTK
 */
#include <aims/io/dicomheader.h>
#include <aims/def/general.h>
#include <aims/def/assert.h>
#include <cartobase/stream/fileutil.h>
#include <cartobase/stream/directory.h>
#include <vector>

#ifndef HAVE_CONFIG_H
#define HAVE_CONFIG_H 1
#endif
#include <dctk.h>
// We recommend that applications should use the new loadFile() and saveFile()
// methods now defined in DcmFileFormat and DcmDataset, thus completely
// removing the need to cope with the stream classes.  The new interface for
// the stream classes can be a bit tricky (especially when handling
// zlib-compressed data with the buffer stream classes used in the network
// module), so we'd prefer people let the toolkit handle things whenever
// possible.
// If you need access to the file stream classes or the zlib codec,
// include dc[io]strmf.h and dc[io]strmz.h manually.
#ifdef UID_RawDataStorage // #if (OFFIS_DCMTK_VERSION_NUMBER-0 > 351)
#include <dcistrmf.h>
#endif

using namespace aims;
using namespace carto;
using namespace std;


DicomHeader::DicomHeader( const string& name ) :
  PythonHeader(), 
  _name( name ),
  _reverseZ( false )
{
}


DicomHeader::~DicomHeader()
{
}


string DicomHeader::name() const
{
  return _name;
}


int DicomHeader::dimX() const
{
  vector< int > vd;
  ASSERT( getProperty("volume_dimension", vd) );

  return( vd[0] );
}

int DicomHeader::dimY() const
{
  vector< int > vd;
  ASSERT( getProperty("volume_dimension", vd) );

  return( vd[1] );
}

int DicomHeader::dimZ() const
{
  vector< int > vd;
  ASSERT( getProperty("volume_dimension", vd) );

  return( vd[2] );
}

int DicomHeader::dimT() const
{
  vector< int > vd;
  ASSERT( getProperty("volume_dimension", vd) );

  return( vd[3] );
}

float DicomHeader::sizeX() const
{
  vector< float > vs;
  ASSERT( getProperty("voxel_size", vs) );

  return( vs[0] );
}

float DicomHeader::sizeY() const
{
  vector< float > vs;
  ASSERT( getProperty("voxel_size", vs) );

  return( vs[1] );
}

float DicomHeader::sizeZ() const
{
  vector< float > vs;
  ASSERT( getProperty("voxel_size", vs) );

  return( vs[2] );
}

float DicomHeader::sizeT() const
{
  float tr;
  ASSERT( getProperty( "tr", tr ) );

  return( tr );
}


int DicomHeader::read()
{
  //cout << "DicomHeader::read " << _name << endl;
  int slice1 = readFirst();
  if ( slice1 < 0 )  return -1;

  // file name elements
  string filebasename( FileUtil::basename( _name ) );
  string directoryname( FileUtil::dirname( _name ) );

  // load the header of the next files and check against the first one
  Directory	dir( directoryname );
  set<string>	files = dir.files();
  set<string>::iterator	is, es = files.end();
  string	fullname;
  char		sep = FileUtil::separator();

  for( is=files.begin(); is!=es; ++is )
    {
      const string	&s = *is;
      if ( s == "." || s == ".." )  continue;

      fullname = directoryname + sep + s;

      FileElement slice = readNext( fullname );
      if ( slice.instance() < 0 )  continue;

      _slices[ slice.instance() ] = slice;
    }


  map< int, FileElement >::const_iterator i = _slices.begin();
  int dimT;
  bool status = getProperty( "nb_t_pos", dimT );
  if ( !status )
    {
      // look for slice arrangement I -> S or S -> I
      bool sensInverse = false;
      i = _slices.begin();
      double loc1 = i->second.location();
      ++i;
      
      if ( i != _slices.end() && i->second.location() > loc1 ) 
        sensInverse = true;

      // look for dimT
      int nbTrans = 0;
      i = _slices.begin();
      ++i;
      while( i != _slices.end() )
        {
          if ( !sensInverse && i->second.location() > loc1 )  nbTrans++;
          else if ( sensInverse && i->second.location() < loc1 )  nbTrans++;

          loc1 = i->second.location();
          ++i;
        }

      dimT = nbTrans + 1;
    } 
 
  int dimZ = _slices.size() / dimT;
  
  string manufac;
  status = getProperty( "manufacturer", manufac );
  if ( status ) 
    { 
      if ( manufac == "GE MEDICAL SYSTEMS" )  _reverseZ = true;
/*
      // Removed because it is a mistake to inverse Z on Philips
      // diffusion images and nobody knows anybody who use Philips
      // DICOM image.

      else if ( manufac ==  "Philips Medical Systems" )
        {
          _reverseZ = true;
	  
          if ( dimT > 1 )
            {
              std::map< int, FileElement > slTmp;
              int k, l;
              std::map< int, FileElement >::const_iterator it=_slices.begin();
	  
              for ( l=0; l<dimZ; l++ )
                for ( k=0; k<dimT; k++, ++it )
                  slTmp[ k * dimZ + l + 1 ] = it->second;
	      
              _slices.clear();
              _slices = slTmp;
            }
        }
*/
    }

  vector< int > volDim;
  getProperty( "volume_dimension", volDim );
  volDim.push_back( dimZ );
  volDim.push_back( dimT );
  setProperty( "volume_dimension", volDim );
  setProperty( "file_type", string( "DICOM" ) );

  // time resolution
  float sizeT = 1.0f;
  float tr;
  status = getProperty( "tr", tr ); // in ms
  if ( dimT > 1 && status )  sizeT = tr / 1000.0f; // in s
  setProperty( "time_resolution", sizeT );

  // add meta-info to header
  readMinf( removeExtension( _name ) + extension() + ".minf" );

  return slice1;
}


namespace
{

  // could move to cartobase / general utility mute streambuf
  class null_streambuf : public streambuf
  {
  public:
    null_streambuf() {}
  };

}


int DicomHeader::readFirst()
{
  // open file
#ifdef UID_RawDataStorage // #if (OFFIS_DCMTK_VERSION_NUMBER-0 > 351)
  DcmInputFileStream  stream( _name.c_str() );
  if ( ! stream.good() )
#else
    DcmFileStream stream( _name.c_str(), DCM_ReadMode );
  if ( stream.GetError() != EC_Normal )
#endif
    {
      //perror( 0 );
      //cout << "could not open stream " << _name << endl;
      return -1;
    }

  // allocate header
  DcmFileFormat header;

  // read header
  null_streambuf	nsb;
  streambuf	*sb = cerr.rdbuf( &nsb );
  header.transferInit();
  header.read( stream, EXS_Unknown, EGL_noChange );
  header.transferEnd();
  cerr.rdbuf( sb );
  if ( header.error() != EC_Normal ) 
    {
      //cerr << dcmErrorConditionToString( header.error() ) << endl;
      return -1;
    }

  DcmStack stack;

  //cout << "header read OK\n";
  if ( header.search( DCM_TransferSyntaxUID, stack ) == EC_Normal )
    {
      if( stack.top()->ident() != EVR_UI )
        { 
          //cerr << "fail 1, id: " << stack.top()->ident() << "\n";
          return( -1 );
        }
      DcmUniqueIdentifier *object = (DcmUniqueIdentifier *)stack.top();
      OFString syntax;
      object->getOFString( syntax, 0 );
      setProperty( "transfer_syntax", string( syntax.c_str() ) );
    }

  string manufacturer;
  if ( header.search( DCM_Manufacturer, stack ) == EC_Normal )
    {
      if( stack.top()->ident() != EVR_LO )
        return( -1 );
      DcmLongString *object = (DcmLongString *)stack.top();
      OFString manufac;
      object->getOFString( manufac, 0 );
      manufacturer = manufac.c_str();
      setProperty( "manufacturer",  manufacturer );
    }

  if ( header.search( DCM_Modality, stack ) == EC_Normal )
    {
      if( stack.top()->ident() != EVR_CS )
        return( -1 );
      DcmCharString *object = (DcmCharString *)stack.top();
      OFString moda;
      object->getOFString( moda, 0 );
      setProperty( "modality", string( moda.c_str() ) );
    }
  if ( header.search( DCM_PatientID, stack ) == EC_Normal )
    {
      if( stack.top()->ident() != EVR_LO )
        return( -1 );
      DcmLongString *object = (DcmLongString *)stack.top();
      OFString patId;
      object->getOFString( patId, 0 );
      setProperty( "patient_id", string( patId.c_str() ) );
    }

  if ( header.search( DCM_StudyID, stack ) == EC_Normal )
    {
      if( stack.top()->ident() != EVR_SH )
        return( -1 );
      DcmShortString *object = (DcmShortString *)stack.top();
      OFString studyId;
      object->getOFString( studyId, 0 );
      setProperty( "study_id", string( studyId.c_str() ) );
    }

  if ( header.search( DCM_SeriesNumber, stack ) == EC_Normal )
    {
      if( stack.top()->ident() != EVR_IS )
        return( -1 );
      DcmIntegerString *object = (DcmIntegerString *)stack.top();
      Sint32 seriesNum;
      object->getSint32( seriesNum );
      setProperty( "series_number", (int)seriesNum );
    }

  vector< int > volDim;
  if ( header.search( DCM_Columns, stack ) == EC_Normal )
    {
      if( stack.top()->ident() != EVR_US )
        return( -1 );
      DcmUnsignedShort *object = (DcmUnsignedShort *)stack.top();
      Uint16 col;
      object->getUint16( col );
      volDim.push_back( (int)col );
    }

  if ( header.search( DCM_Rows, stack ) == EC_Normal )
    {
      if( stack.top()->ident() != EVR_US )
        return( -1 );
      DcmUnsignedShort *object = (DcmUnsignedShort *)stack.top();
      Uint16 row;
      object->getUint16( row );
      volDim.push_back( (int)row );
    }
  setProperty( "volume_dimension", volDim );

  vector< float > voxSize;
  if ( header.search( DCM_PixelSpacing, stack ) == EC_Normal )
    {
      if( stack.top()->ident() != EVR_DS )
        return( -1 );
      DcmDecimalString *object = (DcmDecimalString *)stack.top();
      Float64 pixSpace;
      object->getFloat64( pixSpace, 0 );
      voxSize.push_back( (float)pixSpace );
      object->getFloat64( pixSpace, 1 );
      voxSize.push_back( (float)pixSpace );
    }

  Float64 thickness = 0.0f;
  if ( header.search( DCM_SliceThickness, stack ) == EC_Normal )
    {
      if( stack.top()->ident() != EVR_DS )
        return( -1 );
      DcmDecimalString *object = (DcmDecimalString *)stack.top();
      object->getFloat64( thickness );
      setProperty( "slice_thickness", (float)thickness );
    }

  Float64 spaceSlice = 0.0f;
  if ( header.search( DCM_SpacingBetweenSlices, stack ) == EC_Normal )
    {
      if( stack.top()->ident() != EVR_DS )
        return( -1 );
      DcmDecimalString *object = (DcmDecimalString *)stack.top();
      object->getFloat64( spaceSlice );
      setProperty( "inter_slices", (float)spaceSlice );
    }

  if ( spaceSlice > 0.0f )  voxSize.push_back( (float)spaceSlice );
  else if ( thickness > 0.0f )  voxSize.push_back( (float)thickness );
  else  voxSize.push_back( 1.0f );
  voxSize.push_back( 1.0f ); // T dimension
  setProperty( "voxel_size", voxSize );

  if ( header.search( DCM_NumberOfTemporalPositions, stack ) == EC_Normal )
    {
      if( stack.top()->ident() != EVR_IS )
        return( -1 );
      DcmIntegerString *object = (DcmIntegerString *)stack.top();
      Sint32 nbTempPos;
      object->getSint32( nbTempPos );
      setProperty( "nb_t_pos", (int)nbTempPos );
    }

  if ( header.search( DCM_BitsAllocated, stack ) == EC_Normal )
    {
      if( stack.top()->ident() != EVR_US )
        return( -1 );
      DcmUnsignedShort *object = (DcmUnsignedShort *)stack.top();
      Uint16 nbits;
      object->getUint16( nbits );
      setProperty( "bits_allocated", (int)nbits );

      if ( nbits == 16 )  setProperty( "data_type",  string( "S16" ) );
      else setProperty( "data_type", string( "U8" ) );
    }

  if ( header.search( DCM_RepetitionTime, stack ) == EC_Normal )
    {
      if( stack.top()->ident() != EVR_DS )
        return( -1 );
      DcmDecimalString *object = (DcmDecimalString *)stack.top();
      Float64 tr;
      object->getFloat64( tr );
      setProperty( "tr", (float)tr );
    }

  if ( header.search( DCM_EchoTime, stack ) == EC_Normal )
    {
      if( stack.top()->ident() != EVR_DS )
        return( -1 );
      DcmDecimalString *object = (DcmDecimalString *)stack.top();
      Float64 te;
      object->getFloat64( te );
      setProperty( "te", (float)te );
    }

  if ( header.search( DCM_FlipAngle, stack ) == EC_Normal )
    {
      if ( stack.top()->ident() != EVR_DS )
        return -1;
      DcmDecimalString *object = (DcmDecimalString *)stack.top();
      Float64 flipAngle;
      object->getFloat64( flipAngle );
      setProperty( "flip_angle", (float)flipAngle );
    }

  Sint32 instance_number = -1;
  if ( header.search( DCM_InstanceNumber, stack ) == EC_Normal )
    {
      if( stack.top()->ident() != EVR_IS )
        return( -1 );
      DcmIntegerString *object = (DcmIntegerString *)stack.top();
      object->getSint32( instance_number );
      //cerr << "readFirst: " << _name << ": instance_number = " 
      //     << instance_number << endl;
    }


  if ( manufacturer == "GE MEDICAL SYSTEMS" )
    {
      if ( header.search( DcmTagKey( 0x0019, 0x10B9 ), stack ) == EC_Normal )
        {
          DcmDecimalString *object = (DcmDecimalString *)stack.top();
          Float64 bvalue;
          object->getFloat64( bvalue );
          setProperty( "b_value", (float)bvalue );
        }
    }

  //cout << "readFirst OK\n";
  return (int)instance_number;
}


DicomHeader::FileElement DicomHeader::readNext( const std::string& filename )
{
  //cout << "readNext\n";
  // open file
#ifdef UID_RawDataStorage // #if (OFFIS_DCMTK_VERSION_NUMBER-0 > 351)
  DcmInputFileStream stream( filename.c_str() );
  if ( ! stream.good() )
#else
    DcmFileStream stream( filename.c_str(), DCM_ReadMode );
  if ( stream.GetError() != EC_Normal )
#endif
    {
      perror( 0 );
      return -1;
    }

  // allocate header
  DcmFileFormat header;

  // read header
  header.transferInit();
  header.read( stream, EXS_Unknown, EGL_noChange );
  header.transferEnd();
  if ( header.error() != EC_Normal )  return -1;

  DcmStack stack;

  bool status;

  if ( header.search( DCM_PatientID, stack ) == EC_Normal )
    {
      ASSERT( stack.top()->ident() == EVR_LO );
      DcmLongString *object = (DcmLongString *)stack.top();
      OFString patient_id;
      object->getOFString( patient_id, 0 );
      std::string patId;
      status = getProperty( "patient_id", patId );
      if ( !status || std::string( patient_id.c_str() ) != patId )
        return -1;
    }

  if ( header.search( DCM_StudyID, stack ) == EC_Normal ) 
    {
      ASSERT( stack.top()->ident() == EVR_SH );
      DcmShortString *object = (DcmShortString *)stack.top();
      OFString study_id;
      object->getOFString( study_id, 0 );
      std::string studId;
      status = getProperty( "study_id", studId );
      if ( !status || std::string( study_id.c_str() ) != studId )
        return -1;
    }

  if ( header.search( DCM_SeriesNumber, stack ) == EC_Normal ) 
    {
      ASSERT( stack.top()->ident() == EVR_IS );
      DcmIntegerString *object = (DcmIntegerString *)stack.top();
      Sint32 series_number;
      object->getSint32( series_number, 0 );
      int serNum;
      status = getProperty( "series_number", serNum );
      if ( !status || (int)series_number != serNum )
        return -1;
    }

  vector< int > volDim;
  status = getProperty( "volume_dimension", volDim );
  if ( header.search( DCM_Columns, stack ) == EC_Normal ) 
    {
      ASSERT( stack.top()->ident() == EVR_US );
      DcmUnsignedShort *object = (DcmUnsignedShort *)stack.top();
      Uint16 columns;
      object->getUint16( columns );
      if ( status )  ASSERT( (int)columns == volDim[ 0 ] );
    }

  if ( header.search( DCM_Rows, stack ) == EC_Normal ) 
    {
      ASSERT( stack.top()->ident() == EVR_US );
      DcmUnsignedShort *object = (DcmUnsignedShort *)stack.top();
      Uint16 rows;
      object->getUint16( rows );
      if ( status )  ASSERT( (int)rows == volDim[ 1 ] );
    }

  if ( header.search( DCM_BitsAllocated, stack ) == EC_Normal ) 
    {
      ASSERT( stack.top()->ident() == EVR_US );
      DcmUnsignedShort *object = (DcmUnsignedShort *)stack.top();
      Uint16 bits_allocated;
      object->getUint16( bits_allocated );
      int nbits;
      status = getProperty( "bits_allocated", nbits );
      if ( status )  ASSERT( (int)bits_allocated == nbits );
    }

  Float64 loc = 0.0;
  if ( header.search( DCM_SliceLocation, stack ) == EC_Normal )
    {
      ASSERT( stack.top()->ident() == EVR_DS );
      DcmDecimalString *object = (DcmDecimalString *)stack.top();
      object->getFloat64( loc );
    }

  Sint32 instance_number = -1;
  if ( header.search( DCM_InstanceNumber, stack ) == EC_Normal )
    {
      ASSERT( stack.top()->ident() == EVR_IS );
      DcmIntegerString *object = (DcmIntegerString *)stack.top();
      object->getSint32( instance_number );
      //cerr << "readNext: " << filename << ": instance_number = "
      //     << instance_number << endl;
    }

  FileElement fl( (int)instance_number, (double)loc, filename );

  return fl;
}


