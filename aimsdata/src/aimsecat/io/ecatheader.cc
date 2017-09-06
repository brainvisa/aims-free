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
 *  ECAT image header
 */
#include <aims/io/ecatheader.h>
#include <aims/def/general.h>
#include <ecat+/io/io.h>
#include <cartobase/exception/file.h>
#include <vector>
#include <fstream>

using aims::EcatHeader;
using namespace carto;
using namespace std;

EcatHeader::EcatHeader(const string& name ) :
  PythonHeader(), 
  _name(name), _type( _typeINIT )
{
}


EcatHeader::EcatHeader( int dimx, int dimy, int dimz, int dimt, float sx, 
		      float sy, float sz, float st, int type, 
			const string & name ) :
  PythonHeader(), 
  _name( name ), _type( type )
{
  vector<int>	dims( 4 );
  dims[0] = dimx;
  dims[1] = dimy;
  dims[2] = dimz;
  dims[3] = dimt;
  setProperty( "volume_dimension", dims );
  vector<float>	vs( 4 );
  vs[0] = sx;
  vs[1] = sy;
  vs[2] = sz;
  vs[3] = st;
  setProperty( "voxel_size", vs );
}


EcatHeader::~EcatHeader()
{
}


set<string> EcatHeader::extensions() const
{
  set<string>	exts;
  exts.insert( ".v" );
  exts.insert( ".i" );
  return exts;
}


bool EcatHeader::imageIsProbablyLabel() const
{
  return _type == ECAT_LABEL;
}



int EcatHeader::dimX() const
{
  vector< int > vd;
  if( getProperty("volume_dimension", vd) )
    return vd[0];
  return 1;
}

int EcatHeader::dimY() const
{
  vector< int > vd;
  if( getProperty("volume_dimension", vd) )
    return vd[1];
  return 1;
}

int EcatHeader::dimZ() const
{
  vector< int > vd;
  if( getProperty("volume_dimension", vd) )
    return vd[2];
  return 1;
}

int EcatHeader::dimT() const
{
  vector< int > vd;
  if( getProperty("volume_dimension", vd) && vd.size() >= 4 )
    return vd[3];
  return 1;
}

float EcatHeader::sizeX() const
{
  vector< float > vs;
  if( getProperty("voxel_size", vs) )
    return vs[0];
  return 1;
}

float EcatHeader::sizeY() const
{
  vector< float > vs;
  if( getProperty("voxel_size", vs) )
    return vs[1];
  return 1;
}

float EcatHeader::sizeZ() const
{
  vector< float > vs;
  if( getProperty("voxel_size", vs) )
    return vs[2];
  return 1;
}

float EcatHeader::sizeT() const
{
  float tr;
  if( getProperty( "time_resolution", tr ) )
    return tr;
  vector<float>	vs;
  if( getProperty( "voxel_size", vs ) )
    return vs[3];
  return 1;
}


string EcatHeader::imageUnit() const
{
  string unit;
  if( getProperty( "image_unit", unit ) )
    return unit;
  return "";
}


void EcatHeader::read()
{
  //cout << "EcatHeader::read(), filename : " << _name << "...\n";

  string            fileName;
  UnifiedEcatInfo  *uei;

  //fileName = removeExtension(_name) + ".v";
  fileName = _name;   // .i, .v, .p are different ECAT volumes
  if( (uei = EcatOpen( const_cast<char*>( fileName.c_str() ),
    const_cast<char*>( "r" ) )) == ECATSHFJ_FAIL )
  {
    if( (uei = EcatOpen( const_cast<char*>( (fileName+".v").c_str() ),
      const_cast<char*>( "r" ) )) != ECATSHFJ_FAIL )
      _name = fileName + ".v";
    else
      if( (uei = EcatOpen( const_cast<char*>( (fileName+".i").c_str() ),
        const_cast<char*>( "r" ) )) != ECATSHFJ_FAIL )
        _name = fileName + ".i";
      else
        if( (uei = EcatOpen( const_cast<char*>( (fileName+".p").c_str() ),
          const_cast<char*>( "r" ) )) != ECATSHFJ_FAIL )
          _name = fileName + ".p";
        else
          throw file_error( fileName );
  }

  setProperty( "file_type", string( "ECAT" ) );

  vector< int > volDim;
  volDim.push_back( (int) EcatSizeX( uei ) );
  volDim.push_back( (int) EcatSizeY( uei ) );
  volDim.push_back( (int) EcatSizeZ( uei ) );
  volDim.push_back( (int) EcatSizeT( uei ) );
  setProperty( "volume_dimension", volDim );
  
  vector< float > voxSize;
  voxSize.push_back( (float)  EcatVoxSizeX( uei ));
  voxSize.push_back( (float)  EcatVoxSizeY( uei ));
  voxSize.push_back( (float)  EcatVoxSizeZ( uei ));
  voxSize.push_back( (float)  EcatVoxSizeT( uei ));
  setProperty( "voxel_size", voxSize );

  setProperty( "time_resolution", (float) 1.0);

  vector< int > startTime, durTime;
  for (int t = 0; t < EcatSizeT( uei ); t++)
    {
      startTime.push_back( (int) EcatVolStartTime( uei, t ) );
      durTime.push_back( (int) EcatVolDurTime( uei, t ) );
    }
  setProperty( "start_time",    startTime);
  setProperty( "duration_time", durTime);

  setProperty( "image_unit", string(EcatCalibUnit(uei)) );


  setProperty( "radiopharmaceutical",  string( uei->radiopharmaceutical ) );
  setProperty( "isotope_halflife",  uei->isotope_halflife);
  if (uei->proc_code)
    {
      setProperty( "process_code", uei->proc_code );
      setProperty( "process_list", string( uei->proc_list ));
    }
  // Ici postionner une informatin + sophistiquee LABEL/QUANTITATIVE
  int flag = 1;
  for (int fr=0;fr<EcatSizeT( uei );fr++)
    { 
      flag = flag && EcatVolScale( uei, fr ) ==  1.0;
    }
  if (flag) _type = (int)ECAT_LABEL;
  else      _type = (int)ECAT_QUANT;

  // Cat�gorie de donn�es : Image Projection Sinogramme .....
  setProperty("category", string(uei->category) );

  // 
  setProperty("zero_start_time", (int32_t) uei->zero_start_time);

  // Sauvegarde de nom original file name
  setProperty("original_filename", string(uei->original_filename) );

  EcatClose( uei );

  vector<string>	vt;
  string		datatype;
  if( imageIsProbablyLabel() )
    {	// prefers to be read as short
      datatype = "S16";
      vt.push_back( "S16" );
      vt.push_back( "FLOAT" );
    }
  else
    {	// prefers to be read as float
      datatype = "FLOAT";
      vt.push_back( "FLOAT" );
      vt.push_back( "S16" );
    }
  setProperty( "data_type", datatype );
  setProperty( "possible_data_types", vt );

  // add meta-info to header
  readMinf( removeExtension( _name ) + extension() + ".minf" );
}



void EcatHeader::write()
{
  setProperty( "file_type", string( "ECAT" ) );
  setProperty( "object_type", string( "Volume" ) );
  if( imageIsProbablyLabel() )
    setProperty( "data_type", string( "S16" ) );
  else
    setProperty( "data_type", string( "FLOAT" ) );
  vector<int>	dims;
  dims.push_back( dimX() );
  dims.push_back( dimY() );
  dims.push_back( dimZ() );
  dims.push_back( dimT() );
  setProperty( "volume_dimension", dims );
  vector<float>	vs;
  vs.push_back( sizeX() );
  vs.push_back( sizeY() );
  vs.push_back( sizeZ() );
  vs.push_back( sizeT() );
  setProperty( "voxel_size", vs );

  setProperty( "image_unit", imageUnit() );  

  writeMinf( removeExtension( _name ) + extension() + ".minf" );
}


