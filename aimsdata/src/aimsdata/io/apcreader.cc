// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/io/apcreader.h>
#include <cartobase/exception/ioexcept.h>
#include <soma-io/datasource/filedatasource.h>
#include <soma-io/datasource/bufferdatasource.h>
#include <soma-io/utilities/asciidatasourcetraits.h>

using namespace aims;
using namespace carto;
using namespace std;

APCReader::APCReader( const string & fname )
  : _filename( fname )
{
}


APCReader::~APCReader()
{
}


Object APCReader::read() const
{
  Object	o = Object::value( Dictionary() );

  FileDataSource	ds( _filename );
  if( !ds.open( DataSource::Read ) )
    io_error::launchErrnoExcept( _filename );

  string		line;
  string::size_type	pos;
  static set<string>	ids;
  if( ids.empty() )
    {
      ids.insert( "AC" );
      ids.insert( "PC" );
      ids.insert( "IH" );
      ids.insert( "ACmm" );
      ids.insert( "PCmm" );
      ids.insert( "IHmm" );
    }

  while( !ds.eof() )
    {
      StreamUtil::getline( ds, line );
      pos = line.find( ':' );
      if( pos != string::npos && pos < line.length() - 1 )
        {
          string	id = line.substr( 0, pos );
          if( ids.find( id ) != ids.end() )
            {
              ++pos;
              while( pos < line.length() 
                     && ( line[pos] == ' ' || line[pos] == '\t' ) )
                ++pos;
              string		val = line.substr( pos, line.length() - pos );
              BufferDataSource	bds( (char *) val.c_str(), val.length() + 1 );
              vector<float>	c(3);
              if( !AsciiDataSourceTraits<float>::read( bds, c[0] ) 
                  || !StreamUtil::skip( bds ) 
                  || !AsciiDataSourceTraits<float>::read( bds, c[1] ) 
                  || !StreamUtil::skip( bds ) 
                  || !AsciiDataSourceTraits<float>::read( bds, c[2] ) )
                throw invalid_format_error
                  ( string( "cannot interpret value for " ) + id 
                    + ", value found: " + val, _filename  );
              o->setProperty( id, c );
            }
        }
    }

  if( !o->hasProperty( "AC" ) || !o->hasProperty( "PC" ) 
      || !o->hasProperty( "IH" ) )
    throw syntax_check_error( "missing mandatory field in .APC", _filename );

  return o;
}


string APCReader::filename() const
{
  return _filename;
}


void APCReader::setFilename( const string & filename )
{
  _filename = filename;
}


