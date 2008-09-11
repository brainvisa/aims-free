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

#include <aims/io/finderFormats.h>
#include <aims/io/finder.h>
#include <aims/io/gisheader.h>
#include <aims/io/spmheader.h>
#ifdef VIDA_LIB
#include <aims/io/vidaheader.h>
#endif
#ifdef DICOM_LIB
#include <aims/io/dicomheader.h>
#endif
#ifdef ECAT_LIB
#include <aims/io/ecatheader.h>
#endif
#ifdef AIMS_JPEG
#include <aims/io/jpegheader.h>
#endif
#include <aims/io/genesisheader.h>
#include <aims/io/meshheader.h>
#include <aims/io/texheader.h>
#include <aims/io/triheader.h>
#include <aims/io/bckheader.h>
#include <aims/io/argheader.h>
#include <aims/io/trmheader.h>
#include <aims/io/byteswap.h>
#include <aims/def/assert.h>
#include <cartobase/exception/ioexcept.h>
#include <cartobase/stream/cuifstream.h>
#include <fstream>

using namespace aims;
using namespace carto;
using namespace std;


bool FinderGisFormat::check( const string & /*filename*/, Finder & /*f*/ ) const
{
  return false; // disable Aims version
}


bool FinderSpmFormat::check( const string & filename, Finder & f ) const
{
  string::size_type	pos = filename.rfind( '.' );
  string		ext, fname;

  if( pos == string::npos )
    fname = filename + ".img";
  else
    fname = filename;

  SpmHeader *hdr = new SpmHeader( fname );
  try
    {
      hdr->read();
    }
  catch( exception & )
    {
      delete hdr;
      throw;
    }

  string	dattp;
  hdr->getProperty( "data_type", dattp );
  f.setObjectType( "Volume" );
  f.setFormat( "SPM" );
  vector<string>	vt;
  hdr->getProperty( "possible_data_types", vt );
  if( vt.empty() )
    vt.push_back( dattp );
  f.setDataType( dattp );
  f.setPossibleDataTypes( vt );
  f.setHeader( hdr );

  return( true );
}


#ifdef VIDA_LIB
bool FinderVidaFormat::check( const string & filename, Finder & f ) const
{
  string::size_type	pos = filename.rfind( '.' );
  string		ext, fname;

  if( pos == string::npos )
    fname = filename + ".vimg";
  else
    fname = filename;

  VidaHeader *hdr = new VidaHeader( fname );
  try
    {
      hdr->read();
    }
  catch( exception & e )
    {
      delete hdr;
      return( false );
    }

  f.setObjectType( "Volume" );
  f.setFormat( "VIDA" );
  vector<string>	vt;
  if( hdr->imageIsProbablyLabel() )
    {	// prefers to be read as short
      f.setDataType( "S16" );
      vt.push_back( "S16" );
      vt.push_back( "FLOAT" );
    }
  else
    {	// prefers to be read as float
      f.setDataType( "FLOAT" );
      vt.push_back( "FLOAT" );
      vt.push_back( "S16" );
    }
  f.setPossibleDataTypes( vt );
  f.setHeader( hdr );

  return( true );
}
#endif


#ifdef DICOM_LIB
bool FinderDicomFormat::check( const string & filename, Finder & f ) const
{
  DicomHeader *hdr = new DicomHeader( filename );
  int	status;
  try
    {
      status = hdr->read();
    }
  catch( exception & e )
    {
      delete hdr;
      return( false );
    }
  if( status < 0 )
    {
      delete hdr;
      return( false );
    }

  string	dattp;
  hdr->getProperty( "data_type", dattp );
  f.setObjectType( "Volume" );
  f.setFormat( "DICOM" );
  f.setDataType( dattp );
  vector<string>	vt;
  vt.push_back( dattp );
  f.setPossibleDataTypes( vt );
  f.setHeader( hdr );

  return( true );
}
#endif


#ifdef ECAT_LIB
bool FinderEcatFormat::check( const string & filename, Finder & f ) const
{
  //cout << "FinderEcatFormat::check( " << filename << " , ...)\n";
  EcatHeader *hdr = new EcatHeader( filename );
  try
    {
      hdr->read();
    }
  catch( exception & e )
    {
      //cout << "ECAT failed, continuing\n";
      delete hdr;
      return( false );
    }

  //cout << "ECAT header read\n";
  f.setObjectType( "Volume" );
  f.setFormat( "ECAT" );
  vector<string>	vt;
  if( hdr->imageIsProbablyLabel() )
    {	// prefers to be read as short
      f.setDataType( "S16" );
      vt.push_back( "S16" );
      vt.push_back( "FLOAT" );
    }
  else
    {	// prefers to be read as float
      f.setDataType( "FLOAT" );
      vt.push_back( "FLOAT" );
      vt.push_back( "S16" );
    }
  f.setPossibleDataTypes( vt );
  f.setHeader( hdr );

  return( true );
}
#endif


#ifdef AIMS_JPEG
bool FinderJpegFormat::check( const string & filename, Finder & f ) const
{
  /*string::size_type	pos = filename.rfind( '.' );
  string	fname;

  if( pos == string::npos )
    fname = filename + ".jpg";
  else
  fname = filename;*/

  JpegHeader *hdr = new JpegHeader( filename );
  try
    {
      hdr->read();
    }
  catch( exception & e )
    {
      delete hdr;
      return( false );
    }
  f.setObjectType( "Volume" );
  f.setFormat( "JPEG" );
  f.setDataType( hdr->dataType() );
  f.setPossibleDataTypes( hdr->possibleDataTypes() );
  f.setHeader( hdr );

  return( true );
}
#endif


bool FinderGenesisFormat::check( const string & filename, Finder & f ) const
{
  //cout << "FinderGenesisFormat::check( " << filename << " , ...)\n";
  GenesisHeader *hdr = new GenesisHeader( filename );
  try
    {
      hdr->read();
    }
  catch( exception & )
    {
      delete hdr;
      throw;
    }

  //cout << "GENESIS header read\n";
  vector<string>	vt;
  string		dtype;
  if( hdr->getProperty( "data_type", dtype ) )
    vt.push_back( dtype );
  f.setDataType( dtype );
  f.setObjectType( "Volume" );
  f.setFormat( "GENESIS" );
  f.setPossibleDataTypes( vt );
  f.setHeader( hdr );

  return( true );
}


bool FinderMeshFormat::check( const string & filename, Finder & f ) const
{
  MeshHeader	*hdr = new MeshHeader( filename );
  try
    {
      if( !hdr->read() )
	{
	  delete hdr;
	  return( false );
	}
    }
  catch( exception & )
    {
      delete hdr;
      throw;
    }

  f.setHeader( hdr );
  f.setObjectType( "Mesh" );
  int	ps = 0;
  if( hdr->getProperty( "polygon_dimension", ps ) )
    switch( ps )
      {
      case 2:
        f.setObjectType( "Segments" );
        break;
      case 4:
        f.setObjectType( "Mesh4" );
        break;
      default:
        break;
      }
  string	type;
  hdr->getProperty( "data_type", type );
  f.setDataType( type );
  vector<string>	vt;
  vt.push_back( type );
  f.setPossibleDataTypes( vt );
  f.setFormat( "MESH" );

  return( true );
}


bool FinderTriFormat::check( const string & filename, Finder & f ) const
{
  TriHeader	*hdr = new TriHeader( filename );
  try
    {
      if( !hdr->read() )
	{
	  delete hdr;
	  return( false );
	}
    }
  catch( exception & )
    {
      delete hdr;
      throw;
    }

  f.setHeader( hdr );
  f.setObjectType( "Mesh" );
  string	type;
  hdr->getProperty( "data_type", type );
  f.setDataType( type );
  vector<string>	vt;
  vt.push_back( type );
  f.setPossibleDataTypes( vt );
  f.setFormat( "TRI" );

  return( true );
}


bool FinderTexFormat::check( const string & filename, Finder & f ) const
{
  TexHeader	*hdr = new TexHeader( filename );

  try
    {
      hdr->read();
    }
  catch( exception & )
    {
      delete hdr;
      throw;
    }

  f.setObjectType( "Texture" );
  string	dattp;
  hdr->getProperty( "data_type", dattp );
  f.setDataType( dattp );
  vector<string>	vt;
  vt.push_back( dattp );
  f.setPossibleDataTypes( vt );
  f.setFormat( "TEX" );
  f.setHeader( hdr );

  return true;
}


bool FinderBckFormat::check( const string & filename, Finder & f ) const
{
  BckHeader	*hdr = new BckHeader( filename );

  try
    {
      hdr->read();
    }
  catch( exception & )
    {
      delete hdr;
      throw;
    }

  f.setObjectType( "Bucket" );
  string	dattp;
  hdr->getProperty( "data_type", dattp );
  f.setDataType( dattp );
  vector<string>	vt;
  vt.push_back( dattp );
  f.setPossibleDataTypes( vt );
  f.setFormat( "BCK" );
  f.setHeader( hdr );

  return( true );
}


// Yann // A vérifier et compléter
bool FinderGraphFormat::check( const string & filename, Finder & f ) const
{
  ArgHeader	*hdr = new ArgHeader( filename );

  try
    {
      hdr->read();
    }
  catch( exception & )
    {
      delete hdr;
      throw;
    }

  f.setObjectType( "Graph" );
  string	dattp;
  hdr->getProperty( "data_type", dattp );
  f.setDataType( dattp );
  vector<string>	vt;
  vt.push_back( dattp );
  f.setPossibleDataTypes( vt );
  f.setFormat( "ARG" );
  f.setHeader( hdr );

  return true;
}


bool FinderTrmFormat::check( const string & filename, Finder & f ) const
{
  TrmHeader	*hdr = new TrmHeader( filename );

  try
    {
      hdr->read();
    }
  catch( exception & )
    {
      delete hdr;
      throw;
    }

  f.setObjectType( "Motion" );
  f.setDataType( "VOID" );
  vector<string>	vt;
  vt.push_back( "VOID" );
  f.setPossibleDataTypes( vt );
  f.setFormat( "TRM" );
  f.setHeader( hdr );

  return true;
}


bool FinderHieFormat::check( const string & filename, Finder & f ) const
{
  cuifstream  ff( filename.c_str() );
  if( !ff )
    io_error::launchErrnoExcept( filename );
  string  expected = "*BEGIN TREE";
  string::size_type checked = 0, n = expected.length();
  int  c = 0;
  if( !ff.eof() )
    do
    {
      c = ff.get();
    }
    while( !ff.eof() && ( c == ' ' || c == '\t' || c == '\r' || c == '\n' ) );
  while( !ff.eof() && checked < n )
  {
    if( c != expected[checked] )
      throw invalid_format_error( filename );
    c = ff.get();
    ++checked;
  }
  while( !ff.eof() && ( c == ' ' || c == '\t' || c == '\r' ) )
    c = ff.get();
  if( ff.eof() )
    throw invalid_format_error( filename );
  ostringstream synts;
  do
  {
    synts << (char) c;
    c = ff.get();
  }
  while( !ff.eof() && c != '\n' );
  if( c != '\n' )
    synts << c;
  string  synt = synts.str();
  PythonHeader *ph = new PythonHeader;
  ph->setProperty( "data_type", synt );
  f.setObjectType( "Tree" );
  f.setDataType( synt );
  vector<string>        vt;
  vt.push_back( synt );
  f.setPossibleDataTypes( vt );
  f.setFormat( "HIE" );
  f.setHeader( ph );
  return true;
}

