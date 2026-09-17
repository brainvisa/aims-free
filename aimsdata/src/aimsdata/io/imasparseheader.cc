// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

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

