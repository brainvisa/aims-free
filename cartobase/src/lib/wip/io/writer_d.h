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

#ifndef CARTOBASE_IO_WRITER_D_H
#define CARTOBASE_IO_WRITER_D_H


#include <cartobase/wip/io/writer.h>
#include <cartobase/wip/io/formatwriter.h>
#include <cartobase/wip/io/formatdictionary.h>
#include <cartobase/exception/ioexcept.h>
#include <cartobase/datasource/datasource.h>
#include <cartobase/object/property.h>
#include <cartobase/stream/fileutil.h>
#include <set>


namespace carto
{

  template <typename T>
  bool GenericWriter::write( const T & obj, Object options )
  {
    Writer<T> *writer = dynamic_cast< Writer<T> * >( this );
    if ( ! writer )
      {
        std::string	filename;
        filename = _datasource->url();

        throw carto::format_mismatch_error( filename );
      }
    return writer->write( obj, options );
  }



  template<class T>
  std::string Writer<T>::writtenObjectType() const
  {
    return carto::DataTypeCode<T>::name();
  }


  template<class T>
  bool Writer<T>::write( const T & obj, Object options )
  {
    std::set<std::string>		tried;
    FormatWriter<T>			*writer;
    std::set<std::string>::iterator	notyet = tried.end();
    int					excp = 0;
    int					exct = -1;
    std::string				excm;

    if( !options.get() )
      options = Object::value( PropertySet() );
    std::string	format;
    options->getProperty( "format", format );
    std::string	filename;
    filename = _datasource->url();

    if( !format.empty() )	// priority to format hint
      {
	writer = FormatDictionary<T>::writeFormat( format );
	if( writer )
	  {
	    try
	      {
		if( writer->write( _datasource, obj, options ) )
		  return true;
	      }
	    catch( std::exception & e )
	      {
		carto::io_error::keepExceptionPriority( e, excp, exct, excm, 
							5 );
	      }
	    tried.insert( format );
	  }
      }

    std::string	ext = FileUtil::extension( filename );

    const std::multimap<std::string, std::string>	& extensions 
      = FormatDictionary<T>::writeExtensions();

    std::pair<std::multimap<std::string, std::string>::const_iterator, 
      std::multimap<std::string, std::string>::const_iterator>	iext 
      = extensions.equal_range( ext );
    std::multimap<std::string, std::string>::const_iterator 
      ie, ee = iext.second;

    // try every matching format until one works
    for( ie=iext.first; ie!=ee; ++ie )
      if( tried.find( ie->second ) == notyet )
	{
	  writer = FormatDictionary<T>::writeFormat( ie->second );
	  if( writer )
	    {
	      try
		{
		  if( writer->write( _datasource, obj, options ) )
		    return true;
		}
	      catch( std::exception & e )
		{
		  carto::io_error::keepExceptionPriority( e, excp, exct, 
							  excm );
		}
	      tried.insert( ie->second );
	    }
	}

    if( !ext.empty() )
      {
	// not found or none works: try writers with no extension
	iext = extensions.equal_range( "" );

	for( ie=iext.first, ee=iext.second; ie!=ee; ++ie )
	  if( tried.find( ie->second ) == notyet )
	    {
	      writer = FormatDictionary<T>::writeFormat( ie->second );
	      if( writer )
		{
		  try
		    {
		      if( writer->write( _datasource, obj, options ) )
			return true;
		    }
		  catch( std::exception & e )
		    {
		      carto::io_error::keepExceptionPriority( e, excp, exct, 
							      excm );
		    }
		  tried.insert( ie->second );
		}
	    }
      }

    // still not found ? well, try EVERY format this time...
    iext.first = extensions.begin();
    iext.second = extensions.end();

    for( ie=iext.first, ee=iext.second; ie!=ee; ++ie )
      if( tried.find( ie->second ) == notyet )
	{
	  writer = FormatDictionary<T>::writeFormat( ie->second );
	  if( writer )
	    {
	      try
		{
		  if( writer->write( _datasource, obj, options ) )
		    return true;
		}
	      catch( std::exception & e )
		{
		  carto::io_error::keepExceptionPriority( e, excp, exct, 
							  excm );
		}
	      tried.insert( ie->second );
	    }
	}

    // still not succeeded, it's hopeless...
    carto::io_error::launchExcept( exct, excm, 
                                   filename + " : no matching format" );
    return false;
  }

}

#endif

