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
 *  Bucket header class members
 */
#include <aims/def/assert.h>
#include <aims/io/bckheader.h>
#include <aims/def/general.h>
#include <aims/io/defaultItemR.h>
#include <aims/io/defaultItemW.h>
#include <cartobase/exception/ioexcept.h>
#include <cartobase/wip/io/asciidatasourcetraits.h>

using namespace aims;
using namespace carto;
using namespace std;


BckHeader::BckHeader( const string& name, const string & type,
                      float sizex, float sizey, float sizez,
                      float sizet, uint byteorder, const string & openmode )
  : PythonHeader(), _name( name ), _type( type ), _ntime( 0 ),
    _byteOrder( byteorder ), _openMode( openmode )
{
  vector<float> vs(4);
  vs[0] = sizex;
  vs[1] = sizey;
  vs[2] = sizez;
  vs[3] = sizet;
  setProperty( "voxel_size", vs );
}

string BckHeader::filename() const
{
  if( _name.length() >= 4 && _name.substr( _name.length() - 4, 4 ) == ".bck" )
    return( _name );
  return( _name + ".bck" );
}


float BckHeader::sizeX() const
{
  std::vector<float> vs;
  getProperty( "voxel_size", vs );
  if( vs.empty() )
    return 1.;
  return vs[0];
}


float BckHeader::sizeY() const
{
  std::vector<float> vs;
  getProperty( "voxel_size", vs );
  if( vs.size() < 2 )
    return 1.;
  return vs[1];
}


float BckHeader::sizeZ() const
{
  std::vector<float> vs;
  getProperty( "voxel_size", vs );
  if( vs.size() < 3 )
    return 1.;
  return vs[2];
}


float BckHeader::sizeT() const
{
  std::vector<float> vs;
  getProperty( "voxel_size", vs );
  if( vs.size() < 4 )
    return 1.;
  return vs[3];
}


void BckHeader::read( uint32_t* offset )
{
  string fileName = filename();
  ifstream is( fileName.c_str(), ios::in | ios::binary );
  if( !is )
    io_error::launchErrnoExcept( fileName );
  is.unsetf( ios::skipws );

  char openModeC[6];
  is.read(openModeC,5);
  openModeC[5] = '\0';

  _openMode = string( openModeC );

  bool	ascii = ( _openMode == "ascii" );
  bool	bswap = false;
  vector<float> vs( 4, 1. );

  if ( !ascii )
    {
      if ( _openMode != "binar" )
	throw wrong_format_error( fileName );
      uint32_t tmp1;
      is.read( (char *) &tmp1, sizeof( uint32_t ) );
      _byteOrder = (uint)tmp1;
      bswap = ( _byteOrder == AIMS_MAGIC_NUMBER ? false : true);

      DefaultItemReader< uint32_t > itemb1;
      ItemReader< uint32_t > *itemR2 = itemb1.reader( _openMode, bswap );
      uint32_t typeSize=0;
      itemR2->read( is, typeSize );
      if( !is || typeSize > 50 )
	{
	  delete itemR2;
	  if( !is )
	    io_error::launchErrnoExcept( fileName );
	  throw wrong_format_error( fileName );
	}

      char *typeC = new char[ typeSize + 1 ];
      is.read( typeC, typeSize );
      typeC[ typeSize ] = '\0';

      _type = string( typeC );
      delete[] typeC;

      DefaultItemReader< float > itemb3;
      ItemReader< float >	*itemR3 = itemb3.reader( _openMode, bswap );
      itemR3->read( is, vs[0] );
      itemR3->read( is, vs[1] );
      itemR3->read( is, vs[2] );
      itemR3->read( is, vs[3] );

      uint32_t buf;
      itemR2->read( is, buf );
      _ntime = (int)buf;

      if( offset )
	*offset = is.tellg();

      delete itemR3;
      delete itemR2;
    }
  else
    {
      StreamUtil::skip( is );
      string tmp;
      is >> tmp;
      if( tmp != "-type" )
	throw wrong_format_error( fileName );
      StreamUtil::skip( is );
      is >> _type;
      StreamUtil::skip( is );
      is >> tmp;
      if( tmp != "-dx" )
	throw wrong_format_error( fileName );
      StreamUtil::skip( is );
      is >> vs[0];
      StreamUtil::skip( is );
      is >> tmp;
      if( tmp != "-dy" )
	throw wrong_format_error( fileName );
      StreamUtil::skip( is );
      is >> vs[1];
      StreamUtil::skip( is );
      is >> tmp;
      if( tmp != "-dz" )
	throw wrong_format_error( fileName );
      StreamUtil::skip( is );
      is >> vs[2];
      StreamUtil::skip( is );
      is >> tmp;
      if( tmp != "-dt" )
	throw wrong_format_error( fileName );
      StreamUtil::skip( is );
      is >> vs[3];

      StreamUtil::skip( is );
      is >> tmp;
      if( tmp != "-dimt" )
	throw wrong_format_error( fileName );
      StreamUtil::skip( is );
      is >> _ntime;

      if( offset )
	*offset = is.tellg();
    }

  setProperty( "file_type", string( "BCK" ) );
  setProperty( "data_type", _type );
  setProperty( "ascii", (int) ascii );
  if( !ascii )
    setProperty( "byte_swapping", (int) bswap );
  setProperty( "nb_t_pos", (int) _ntime );
  setProperty( "voxel_size", vs );
  setProperty( "object_type", string( "Bucket" ) );

  // add meta-info to header
  readMinf( removeExtension( _name ) + extension() + ".minf" );
}


void BckHeader::write()
{
  string fileName = filename();
  bool	ascii = ( _openMode == "ascii" );
  bool	bswap = ( _byteOrder == AIMS_MAGIC_NUMBER ? false : true);

  ios::openmode	omd = ios::out;
  if( !ascii )
    omd |= ios::binary;
  ofstream os( fileName.c_str(), omd );
  if( !os )
    io_error::launchErrnoExcept( fileName );

  os << _openMode;

  if( !ascii )
    {
      if( _openMode != "binar" )
	throw wrong_format_error( fileName );
      uint32_t tmp1 = (uint32_t)_byteOrder;
      os.write( (char *) &tmp1, sizeof( uint32_t ) );

      DefaultItemWriter<uint32_t>	sw1;
      ItemWriter<uint32_t>	*sw = sw1.writer( _openMode, bswap );

      sw->write( os, _type.size() );
      os.write( _type.c_str(), _type.size() );

      DefaultItemWriter<float>	fw1;
      ItemWriter<float>		*fw = fw1.writer( _openMode, bswap );
      fw->write( os, sizeX() );
      fw->write( os, sizeY() );
      fw->write( os, sizeZ() );
      fw->write( os, sizeT() );
      delete fw;
      delete sw;
    }
  else
    {
      os << "\n-type " << _type << endl;
      os << "-dx " << sizeX() << "\n-dy " << sizeY() << "\n-dz " << sizeZ()
	 << "\n-dt " << sizeT() << endl;
    }

  // write meta-info header
  setProperty( "file_type", string( "BCK" ) );
  vector<float>	vs;
  setProperty( "data_type", _type );
  setProperty( "object_type", string( "Bucket" ) );

  writeMinf( removeExtension( _name ) + extension() + ".minf" );
}


set<string> BckHeader::extensions() const
{
  set<string>	exts;
  exts.insert( ".bck" );
  return exts;
}
