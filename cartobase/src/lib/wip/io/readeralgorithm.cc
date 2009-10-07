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

#include <cartobase/wip/io/readeralgorithm.h>
#include <cartobase/wip/io/datasourceinfo.h>
#include <cartobase/datasource/filedatasource.h>
#include <cartobase/exception/format.h>
#include <iostream>

using namespace carto;
using namespace std;


ReaderAlgorithm::ReaderAlgorithm( const string & name ) : Algorithm( name )
{
}


ReaderAlgorithm::~ReaderAlgorithm()
{
}


void ReaderAlgorithm::registerAlgorithmType( const string & objType, 
                                             ProcFunc procFunc )
{
  _execs[ objType ] = procFunc;
}


bool ReaderAlgorithm::execute( const string & filename )
{
  return execute( rc_ptr<DataSource>( new FileDataSource( filename ) ) );
}


bool ReaderAlgorithm::execute( rc_ptr<DataSource> source )
{
  //cout << "ReaderAlgorithm::execute( " << source->url() << " )\n";
  DataSourceInfo	f;
  Object		hdr = f.check( *source );
  if( hdr.isNone() )
    {
      f.launchException();
      // throw format_error( source->url() );
      return false;
    }
  return execute( hdr, source );
}


bool ReaderAlgorithm::execute( Object header, rc_ptr<DataSource> source )
{
  string	otype;

  header->getProperty( "object_type", otype );

  map<string, ProcFunc>::const_iterator	ip = _execs.find( otype );
  if( ip == _execs.end() )
    {
      // Try alternate data types
      vector<string>	posstypes;
      try
        {
          header->getProperty( "possible_types", posstypes );
        }
      catch( ... )
        {
        }

      unsigned		i, n = posstypes.size();

      for( i=0; i<n; ++i )
	if( posstypes[i] != otype )
	  {
	    ip = _execs.find( posstypes[i] );
	    if( ip != _execs.end() )
	      {
		// force new datatype into header
                header->setProperty( "object_type", posstypes[i] );
		break;
	      }
	  }
      if( i == n )
	{
	  throw datatype_format_error( string( "unsupported object type " ) 
				       + otype, source->url() );
	  return false;
	}
    }

  //	execute algo function
return ip->second( *this, header, source );
}


