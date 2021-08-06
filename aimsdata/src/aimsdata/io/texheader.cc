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

/*
 *  Texture header class members
 */
#include <aims/io/texheader.h>
#include <aims/def/general.h>
#include <aims/io/defaultItemR.h>
#include <aims/io/defaultItemW.h>
#include <cartobase/exception/format.h>
#include <cartobase/exception/file.h>
#include <cartobase/stream/fileutil.h>
#include <soma-io/utilities/asciidatasourcetraits.h>
#include <fstream>
#include <memory>

using namespace aims;
using namespace carto;
using namespace std;


TexHeader::TexHeader( const string& name, const string & type, 
		      size_t size, bool ascii )
  : PythonHeader(), 
    _name( name ), _type( type ), _size( size ), 
    _byteOrder( AIMS_MAGIC_NUMBER ), _openMode( ascii ? "ascii" : "binar" )
{
  setProperty( "nb_t_pos", (int) _size );
}


TexHeader::~TexHeader()
{
}


string TexHeader::name() const
{
  return _name;
}


string TexHeader::filename() const
{
  if( ( _name.length() > 4 && _name.substr( _name.length()-4, 4 ) == ".tex" )
      || !FileUtil::fileStat( _name ).empty() )
    return _name;
  else
    return _name + ".tex";
}


void TexHeader::read( size_t* offset )
{
  string fileName = filename();
  ifstream is( fileName.c_str(), ios::in | ios::binary );
  if ( !is )
    io_error::launchErrnoExcept( fileName );
  is.unsetf( ios::skipws );

  char openModeC[ 6 ];
  is.read( openModeC, 5 );
  if ( !is )
    io_error::launchErrnoExcept( fileName );
  openModeC[ 5 ] = '\0';

  _openMode = string( openModeC );
  bool	ascii = ( _openMode == "ascii" );
  if ( !ascii && _openMode != "binar" )
    throw wrong_format_error( fileName );

  bool		bswap = false;
  uint32_t	nvertex = 0;

  _byteOrder = 0;

  if ( !ascii )
    {
      is.read( (char *) &_byteOrder, sizeof( uint ) );
      if ( !is )
	io_error::launchErrnoExcept( fileName );
      bswap = ( _byteOrder == AIMS_MAGIC_NUMBER ? false : true);

      DefaultItemReader< uint32_t > ib1;
      unique_ptr<ItemReader<uint32_t> >
        itemR2( ib1.reader( _openMode, bswap ) );
      uint32_t typeSize=0;
      itemR2->read( is, typeSize );
      if ( !is )
	io_error::launchErrnoExcept( fileName );
      if ( typeSize > 50 )
	throw wrong_format_error( fileName );

      char *typeC = new char[ typeSize + 1 ];
      is.read( typeC, typeSize );
      if ( !is )
	io_error::launchErrnoExcept( fileName );
      typeC[ typeSize ] = '\0';

      _type = string( typeC );
      delete[] typeC;

      uint32_t tmp;
      itemR2->read( is, tmp );
      _size = (size_t)tmp;
      if ( !is )
	io_error::launchErrnoExcept( fileName );

      if ( offset ) {
        ifstream::pos_type tmp = is.tellg();
        *offset = (size_t)tmp; // ## does not support large files!
      }

      if ( _size > 0 )
	{
	  uint32_t	time0;
	  itemR2->read( is, time0 );
	  if ( !is )
	    io_error::launchErrnoExcept( fileName );
	  itemR2->read( is, nvertex );
	  if ( !is )
	    io_error::launchErrnoExcept( fileName );
	}
    }
  else
    {
      StreamUtil::skip( is );
      is >> _type;
      if ( !is )
	io_error::launchErrnoExcept( fileName );
      StreamUtil::skip( is );
      is >> _size;
      if ( !is )
	io_error::launchErrnoExcept( fileName );

      if ( offset ) {
        ifstream::pos_type tmp = is.tellg();
        *offset = (size_t)tmp; // ## does not support large files!
      }

      if ( _size > 0 )
	{
	  size_t	time0;
          StreamUtil::skip( is );
	  is >> time0;
	  if ( !is )
	    io_error::launchErrnoExcept( fileName );
          StreamUtil::skip( is );
	  is >> nvertex;
	  if ( !is )
	    io_error::launchErrnoExcept( fileName );
	}
    }

  is.close();

  setProperty( "file_type", string( "TEX" ) );
  setProperty( "data_type", _type );
  setProperty( "ascii", (int) ascii );
  if ( !ascii )
    setProperty( "byte_swapping", (int) bswap );
  setProperty( "nb_t_pos", (int) _size );
  setProperty( "vertex_number", (int) nvertex );

  // add meta-info to header
  readMinf( removeExtension( _name ) + extension() + ".minf" );
}


int TexHeader::dimT() const
{
  int dimt = 0;
  getProperty( "nb_t_pos", dimt );
  return dimt;
}


bool TexHeader::byteSwapping() const
{
  return _byteOrder != AIMS_MAGIC_NUMBER;
}


set<string> TexHeader::extensions() const
{
  set<string>	exts;
  exts.insert( ".tex" );
  return exts;
}


void TexHeader::write()
{
  string fileName = filename();
  ofstream os( fileName.c_str() );
  if ( !os )
    io_error::launchErrnoExcept( fileName );
  os << _openMode;
  bool	ascii = (_openMode == "ascii" );
  if ( ascii )
    {
      os << endl;
      os << _type << endl;
      os << _size << endl;
    }
  else
    {
      uint32_t magicNumber = (uint32_t) AIMS_MAGIC_NUMBER;
      os.write( (const char *) &magicNumber, sizeof(uint32_t) );
      DefaultItemWriter<uint32_t> sw;
      sw.write( os, (uint32_t)_type.length() );
      os.write( _type.c_str(), _type.length() );
      sw.write( os, (uint32_t)_size );
    }

  setProperty( "object_type", string( "Texture" ) );
  setProperty( "data_type", _type );
  writeMinf( fileName + ".minf" );
}
