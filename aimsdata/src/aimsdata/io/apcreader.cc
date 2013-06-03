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

#include <aims/io/apcreader.h>
#include <cartobase/exception/ioexcept.h>
#ifdef USE_SOMA_IO
  #include <soma-io/datasource/filedatasource.h>
  #include <soma-io/datasource/bufferdatasource.h>
  #include <soma-io/utilities/asciidatasourcetraits.h>
#else
  #include <cartobase/datasource/filedatasource.h>
  #include <cartobase/datasource/bufferdatasource.h>
  #include <cartobase/io/asciidatasourcetraits.h>
#endif

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


