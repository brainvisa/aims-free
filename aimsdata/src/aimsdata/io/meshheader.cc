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

#include <aims/io/meshheader.h>
#include <aims/def/general.h>
#include <aims/io/byteswap.h>
#include <cartobase/exception/ioexcept.h>
#include <fstream>

using namespace aims;
using namespace carto;
using namespace std;


MeshHeader::MeshHeader( const string & filename ) : 
  PythonHeader(), 
  _filename( filename )
{
}


MeshHeader::~MeshHeader()
{
}


bool MeshHeader::read( uint32_t* offset )
{
  const unsigned	MaxPolyDim = 100;
  string		fname = filename();

  ifstream	is( fname.c_str(), ios::in | ios::binary );
  if( !is )
    io_error::launchErrnoExcept( fname );
  is.unsetf( ios::skipws );

  uint32_t	magicNumber;
  char		openModeC[6];
  string	type;
  int32_t	polygonDim;
  uint32_t	ntime;
  uint32_t	nvertex = 0;
  bool		ascii;

  is.read( openModeC, 5 );
  if( !is )
    io_error::launchErrnoExcept( fname );
  openModeC[5] = '\0';
  string openMode( openModeC );

  ascii = ( openMode == "ascii" );

  if( !ascii )
    {
      if ( openMode != "binar" )
	throw wrong_format_error( fname );

      is.read( (char *) &magicNumber, sizeof(uint32_t) );
      if ( !is )
	io_error::launchErrnoExcept( fname );

      uint32_t	typeSize = 0;
      is.read( (char *) &typeSize, sizeof(uint32_t) );
      if ( !is )
	io_error::launchErrnoExcept( fname );

      bool swap = false;
      if( magicNumber != AIMS_MAGIC_NUMBER )
	swap = true;

      if ( swap )
	typeSize = byteswap32( typeSize );

      if ( typeSize > 50 )
	throw wrong_format_error( fname );

      char *typeC = new char[ typeSize + 1 ];
      is.read( typeC, typeSize );
      if( !is )
	io_error::launchErrnoExcept( fname );
      typeC[ typeSize ] = '\0';
      type = typeC;
      delete[] typeC;

      is.read( (char *) &polygonDim, sizeof(int32_t) );
      if( !is )
	io_error::launchErrnoExcept( fname );
      if ( swap )
	polygonDim = byteswap32( polygonDim );
      if ( (unsigned) polygonDim > MaxPolyDim )
	throw wrong_format_error( fname );

      is.read( (char *) &ntime, sizeof(uint32_t) );
      if ( !is )
	io_error::launchErrnoExcept( fname );
      if ( swap )
	ntime = byteswap32( ntime );

      if ( offset )
	*offset = (uint32_t)is.tellg(); // ## does not support large files!

      if ( ntime > 0 )
	{
	  /* remainder is not necessary (vertex num depends on each 
	     time position) */
	  is.read( (char *) &nvertex, sizeof(uint32_t) ); // 1st time position
	  if ( !is )
	    io_error::launchErrnoExcept( fname );
	  is.read( (char *) &nvertex, sizeof(uint32_t) );
	  if ( !is )
	    io_error::launchErrnoExcept( fname );
	  if ( swap )
	    nvertex = byteswap32( nvertex );
	}
    }
  else	// ascii
    {
      is.setf( ios::skipws );
      is >> type;
      if ( !is )
	io_error::launchErrnoExcept( fname );
      is >> polygonDim;
      if ( !is )
	io_error::launchErrnoExcept( fname );
      if ( (unsigned) polygonDim > MaxPolyDim )
	throw wrong_format_error( fname );
      is >> ntime;
      if ( !is )
	io_error::launchErrnoExcept( fname );

      if ( offset )
	*offset = is.tellg();

      if ( ntime > 0 )
	{
	  is >> nvertex;	// 1st time position
	  if ( !is )
	    io_error::launchErrnoExcept( fname );
	  is >> nvertex;
	  if ( !is )
	    io_error::launchErrnoExcept( fname );
	}
    }

  is.close();

  setProperty( "file_type", string( "MESH" ) );
  setProperty( "data_type", type );
  setProperty( "ascii", (int) ascii );
  if( !ascii )
    setProperty( "byte_swapping", 
		  (int) ( magicNumber != AIMS_MAGIC_NUMBER ) );
  setProperty( "polygon_dimension", (int) polygonDim );
  setProperty( "nb_t_pos", (int) ntime );
  setProperty( "vertex_number", (int) nvertex );

  // add meta-info to header
  readMinf( removeExtension( fname ) + extension() + ".minf" );

  return true;
}


string MeshHeader::openMode() const
{
  int om = 0;
  getProperty( "ascii", om );
  if ( om )
    return string("ascii");
  return string("binar");
}


bool MeshHeader::byteSwapping() const
{
  int	bswap = 0;
  getProperty( "byte_swapping", bswap );
  return bswap ? true : false;
}


string MeshHeader::filename() const
{
  if( _filename.length() > 5 
      && _filename.substr( _filename.length()-5, 5 ) == ".mesh" )
    return _filename;
  else
    return _filename + ".mesh";
}


set<string> MeshHeader::extensions() const
{
  set<string> exts;
  exts.insert( ".mesh" );
  return exts;
}
