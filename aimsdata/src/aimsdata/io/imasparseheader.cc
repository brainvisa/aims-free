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

#include <aims/io/imasparseheader.h>
#include <aims/def/general.h>
#include <aims/io/defaultItemR.h>
#include <cartobase/exception/ioexcept.h>
#include <cartobase/stream/fileutil.h>
#include <soma-io/utilities/asciidatasourcetraits.h>
#include <fstream>

using namespace aims;
using namespace carto;
using namespace std;


ImasHeader::ImasHeader( const string & filename ) :
  PythonHeader(),
  _filename( filename )
{
}


ImasHeader::~ImasHeader()
{
}


namespace
{

  bool testBinFormat( const string & fname, uint32_t & lines, uint32_t & cols,
    uint32_t & count )
  {
    ifstream  is( fname.c_str(), ios::in | ios::binary );
    if( !is )
      io_error::launchErrnoExcept( fname );
    is.unsetf( ios::skipws );


    static DefaultItemReader<uint32_t> itemR1;

    uint32_t size1 = 0U;
    uint32_t size2 = 0U;
    uint32_t nonZeroElementCount = 0U;

    itemR1.read( is, size1 );
    if( !is )
      return false;
    itemR1.read( is, size2 );
    if( !is )
      return false;
    itemR1.read( is, nonZeroElementCount );
    if( !is )
      return false;

    streampos p = is.tellg(), e;
    is.seekg( 0, ios_base::end );
    e = is.tellg();
    is.seekg( p, ios_base::beg );
    if( nonZeroElementCount > size1 * size2
        || nonZeroElementCount * 16 != e - p )
      return false;
    lines = size1;
    cols = size2;
    count = nonZeroElementCount;
    return true;
  }


  bool testBswapFormat( const string & fname, uint32_t & lines,
                        uint32_t & cols, uint32_t & count )
  {
    ifstream  is( fname.c_str(), ios::in | ios::binary );
    if( !is )
      io_error::launchErrnoExcept( fname );
    is.unsetf( ios::skipws );


    static DefaultBSwapItemReader<uint32_t> itemR1;

    uint32_t size1 = 0U;
    uint32_t size2 = 0U;
    uint32_t nonZeroElementCount = 0U;

    itemR1.read( is, size1 );
    if( !is )
      return false;
    itemR1.read( is, size2 );
    if( !is )
      return false;
    itemR1.read( is, nonZeroElementCount );
    if( !is )
      return false;

    streampos p = is.tellg(), e;
    is.seekg( 0, ios_base::end );
    e = is.tellg();
    is.seekg( p, ios_base::beg );
    if( nonZeroElementCount > size1 * size2
        || nonZeroElementCount * 16 != e - p )
      return false;
    lines = size1;
    cols = size2;
    count = nonZeroElementCount;
    return true;
  }


  bool testAsciiFormat( const string & fname, uint32_t & lines,
                        uint32_t & cols, uint32_t & count )
  {
    ifstream  is( fname.c_str(), ios::in | ios::binary );
    if( !is )
      io_error::launchErrnoExcept( fname );
    is.unsetf( ios::skipws );


    static DefaultAsciiItemReader<uint32_t> itemR1;

    uint32_t size1 = 0U;
    uint32_t size2 = 0U;
    uint32_t nonZeroElementCount = 0U;

    itemR1.read( is, size1 );
    if( !is )
      return false;
    itemR1.read( is, size2 );
    if( !is )
      return false;
    itemR1.read( is, nonZeroElementCount );
    if( !is )
      return false;

    streampos p = is.tellg(), e;
    is.seekg( 0, ios_base::end );
    e = is.tellg();
    is.seekg( p, ios_base::beg );
    if( nonZeroElementCount > size1 * size2
        || nonZeroElementCount * 16 != e - p )
      return false;
    lines = size1;
    cols = size2;
    count = nonZeroElementCount;
    return true;
  }

}


bool ImasHeader::read( uint32_t* )
{
  string    fname = filename();

  if( FileUtil::fileStat( fname ).find( '+' ) == string::npos )
    throw file_not_found_error( fname );

  bool ascii = false;
  bool bswap = false;
  uint32_t lines = 0, cols = 0, count = 0;

  if( testBinFormat( fname, lines, cols, count ) )
  {
    ascii = false;
    bswap = false;
  }
  else if( testBswapFormat( fname, lines, cols, count ) )
  {
    ascii = false;
    bswap = true;
  }
  else if( testAsciiFormat( fname, lines, cols, count ) )
  {
    ascii = true;
    bswap = false;
  }
  else
    throw wrong_format_error( "Wrong format or corrupted .imas format",
                              "<?>" );

  setProperty( "file_type", string( "IMASPARSE" ) );
  setProperty( "object_type", "SparseMatrix" );
  setProperty( "data_type", "DOUBLE" );
  setProperty( "ascii", (int) ascii );
  if( !ascii )
    setProperty( "byte_swapping", (int) bswap );
  vector<int> dims(2);
  dims[0] = (int) lines;
  dims[1] = (int) cols;
  setProperty( "dimensions", dims );
  setProperty( "non_zero_elements", (int) count );

  // add meta-info to header
  readMinf( removeExtension( fname ) + extension() + ".minf" );

  return true;
}


string ImasHeader::openMode() const
{
  int om = 0;
  getProperty( "ascii", om );
  if ( om )
    return string("ascii");
  return string("binar");
}


bool ImasHeader::byteSwapping() const
{
  int bswap = 0;
  getProperty( "byte_swapping", bswap );
  return bswap ? true : false;
}


string ImasHeader::filename() const
{
  if( _filename.length() > 5
      && _filename.substr( _filename.length()-5, 5 ) == ".imas" )
    return _filename;
  else
  {
    if( FileUtil::fileStat( _filename ).find( 'r' ) != string::npos )
      return _filename;
    return _filename + ".imas";
  }
}


set<string> ImasHeader::extensions() const
{
  set<string> exts;
  exts.insert( ".imas" );
  return exts;
}

