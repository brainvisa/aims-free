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

#include <aims/io/triheader.h>
#include <aims/def/general.h>
#include <cartobase/exception/ioexcept.h>
#include <fstream>

using namespace aims;
using namespace carto;
using namespace std;


TriHeader::TriHeader( const string & filename ) : 
  PythonHeader(), 
  _filename( filename )
{
}


TriHeader::~TriHeader()
{
}


bool TriHeader::read( size_t* offset )
{
  string fname = filename();

  ifstream is( fname.c_str() );
  if ( !is )
    io_error::launchErrnoExcept( fname );

  string str;
  is >> str;
  if ( !is )
    io_error::launchErrnoExcept( fname );
  if ( str != "-" )
    throw wrong_format_error( fname );

  int nvertex;
  is >> nvertex;
  if ( !is )
    io_error::launchErrnoExcept( fname );

  if ( offset ) {
    ifstream::pos_type tmp = is.tellg();
    *offset = (size_t)tmp; // ## does not support large files!
  }

  setProperty( "file_type", string( "TRI" ) );
  setProperty( "data_type", string( "VOID" ) );
  setProperty( "ascii", 1 );
  setProperty( "polygon_dimension", 3 );
  setProperty( "nb_t_pos", 1 );
  setProperty( "vertex_number", nvertex );

  // add meta-info to header
  readMinf( removeExtension( fname ) + extension() + ".minf" );

  return true;
}


string TriHeader::openMode() const
{
  int	om = 0;
  getProperty( "ascii", om );
  if ( om )
    return string("ascii");
  return string("binar");
}


bool TriHeader::byteSwapping() const
{
  int	bswap = 0;
  getProperty( "byte_swapping", bswap );
  return (bool)bswap;
}


string TriHeader::filename() const
{
  if ( _filename.length() > 4 
      && _filename.substr( _filename.length() - 4, 4 ) == ".tri" )
    return _filename;
  else
    return _filename + ".tri";
}


set<string> TriHeader::extensions() const
{
  set<string>	exts;
  exts.insert( ".tri" );
  return exts;
}
