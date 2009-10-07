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

#ifndef CARTOBASE_IO_READER_D_H
#define CARTOBASE_IO_READER_D_H


#include <cartobase/wip/io/reader.h>
#include <cartobase/wip/io/formatreader.h>
#include <cartobase/wip/io/formatdictionary.h>
#include <cartobase/wip/io/datasourceinfo.h>
#include <cartobase/exception/ioexcept.h>
#include <cartobase/datasource/filedatasource.h>
#include <cartobase/datasource/streamdatasource.h>
#include <cartobase/object/property.h>
#include <cartobase/stream/fileutil.h>
#include <set>
#ifdef CARTO_DEBUG_IO
#include <iostream>
#endif


namespace carto
{

  template<class T> Reader<T>::Reader()
  {
  }

  template<class T> Reader<T>::Reader( rc_ptr<DataSource> ds )
    : _datasource( ds )
  {
  }

  template<class T> Reader<T>::Reader( const std::string& filename )
    : _datasource( new FileDataSource( filename ) )
  {
  }

  template<class T> Reader<T>::Reader( std::istream & stream )
    : _datasource( new IStreamDataSource( stream ) )
  {
  }

  template<class T> Reader<T>::~Reader()
  {
  }

  template <typename T>
  void Reader<T>::setAllocatorContext( const AllocatorContext & ac )
  {
    _alloccontext = ac;
  }

  template <typename T>
  const AllocatorContext & Reader<T>::allocatorContext() const
  {
    return _alloccontext;
  }

  template<class T>
  void Reader<T>::setOptions( Object options )
  {
    _options = options;
  }

  template<class T>
  Object Reader<T>::options() const
  {
    return _options;
  }

  template<class T>
  Object & Reader<T>::options()
  {
    return _options;
  }

  template<class T>
  void Reader<T>::attach( rc_ptr<DataSource> ds )
  {
    _datasource = ds;
  }

  template<class T>
  void Reader<T>::attach( const std::string & filename, offset_t offset )
  {
    _datasource.reset( new FileDataSource( filename, offset ) );
  }

  template<class T>
  void Reader<T>::attach( std::istream & stream )
  {
    _datasource.reset( new IStreamDataSource( stream ) );
  }

  template<class T>
  const rc_ptr<DataSource> Reader<T>::dataSource() const
  {
    return _datasource;
  }

  template<class T>
  rc_ptr<DataSource> Reader<T>::dataSource()
  {
    return _datasource;
  }

  template<class T>
  bool Reader<T>::read( T & obj, Object header )
  {
#ifdef CARTO_DEBUG_IO
    std::cout << "Reader<" << DataTypeCode<T>::name() << ">\n";
#endif

    if( !_datasource )
      throw std::runtime_error( "Reader with no source of data" );
    if( !_options.get() )
      _options = Object::value( PropertySet() );

    if( !header.get() )	// no header: check it no
      {
        DataSourceInfo	dsi;
        header = dsi.check( *_datasource );
        if( !header.get() )
          dsi.launchException();
      }
    std::string	format;
    if( !_options->getProperty( "format", format )  // user options first
        && !header->getProperty( "format", format ) ) // new style
      header->getProperty( "file_type" ); // old style

    std::string				filename;
    filename = _datasource->url();
    std::set<std::string>		tried;
    std::set<FormatReader<T> *>		triedf;
    FormatReader<T>			*reader;
    std::set<std::string>::iterator	notyet = tried.end();
    typename std::set<FormatReader<T> *>::iterator	notyetf = triedf.end();
    int					excp = 0;
    int					exct = -1;
    std::string				excm;

    if( !format.empty() )	// priority to format hint
      {
	reader = FormatDictionary<T>::readFormat( format );
	if( reader )
	  {
	    try
	      {
#ifdef CARTO_DEBUG_IO
                std::cout << "1. try reader " << format << std::endl;
#endif
		reader->setupAndRead( obj, header, _datasource, _alloccontext, 
                                      _options );
#ifdef CARTO_DEBUG_IO
                std::cout << "1. " << format << " OK\n";
#endif
                return true;
	      }
	    catch( std::exception & e )
	      {
#ifdef CARTO_DEBUG_IO
                std::cout << "1. failed\n";
#endif
		io_error::keepExceptionPriority( e, excp, exct, excm, 5 );
	      }
	    tried.insert( format );
	    triedf.insert( reader );
	  }
      }

    std::string	ext = FileUtil::extension( filename );

    const std::multimap<std::string, std::string>	& extensions 
      = FormatDictionary<T>::readExtensions();

    std::pair<std::multimap<std::string, std::string>::const_iterator, 
      std::multimap<std::string, std::string>::const_iterator>	iext 
      = extensions.equal_range( ext );
    std::multimap<std::string, std::string>::const_iterator	
      ie, ee = iext.second;

    // try every matching format until one works
    for( ie=iext.first; ie!=ee; ++ie )
      if( tried.find( (*ie).second ) == notyet )
	{
	  reader = FormatDictionary<T>::readFormat( ie->second );
	  if( reader && triedf.find( reader ) == notyetf )
	    {
	      try
		{
#ifdef CARTO_DEBUG_IO
                  std::cout << "2. try reader " << ie->second << std::endl;
#endif
		  reader->setupAndRead( obj, header, _datasource, 
                                        _alloccontext, _options );
#ifdef CARTO_DEBUG_IO
                  std::cout << "2. " << ie->second << " OK\n";
#endif
                  return true;
		}
	      catch( std::exception & e )
		{
#ifdef CARTO_DEBUG_IO
                  std::cout << "2. failed\n";
#endif
		  io_error::keepExceptionPriority( e, excp, exct, excm );
		}
#ifdef CARTO_DEBUG_IO
              std::cout << "2. unsuccessfully tried " << ie->second 
                        << std::endl;
#endif
	      tried.insert( ie->second );
	      triedf.insert( reader );
	    }
	}

    if( !ext.empty() )
      {
	// not found or none works: try readers with no extension
	iext = extensions.equal_range( "" );

	for( ie=iext.first, ee=iext.second; ie!=ee; ++ie )
	  if( tried.find( (*ie).second ) == notyet )
	    {
	      reader = FormatDictionary<T>::readFormat( ie->second );
	      if( reader && triedf.find( reader ) == notyetf )
		{
		  try
		    {
#ifdef CARTO_DEBUG_IO
                      std::cout << "3. try reader " << ie->second << std::endl;
#endif
		      reader->setupAndRead( obj, header, _datasource, 
                                           _alloccontext, _options );
#ifdef CARTO_DEBUG_IO
                      std::cout << "3. " << ie->second << " OK\n";
#endif
                      return true;
		    }
		  catch( std::exception & e )
		    {
#ifdef CARTO_DEBUG_IO
                      std::cout << "3. failed\n";
#endif
		      io_error::keepExceptionPriority( e, excp, exct, excm );
		    }
		  tried.insert( ie->second );
		  triedf.insert( reader );
		}
	    }
      }

    // still not found ? well, try EVERY format this time...
    iext.first = extensions.begin();
    iext.second = extensions.end();

    for( ie=iext.first, ee=iext.second; ie!=ee; ++ie )
      if( tried.find( (*ie).second ) == notyet )
	{
	  reader = FormatDictionary<T>::readFormat( ie->second );
	  if( reader && triedf.find( reader ) == notyetf )
	    {
	      try
		{
#ifdef CARTO_DEBUG_IO
                  std::cout << "4. try reader " << ie->second << std::endl;
#endif
		  reader->setupAndRead( obj, header, _datasource, 
                                        _alloccontext, _options );
#ifdef CARTO_DEBUG_IO
                  std::cout << "4. " << ie->second << " OK\n";
#endif
                  return true;
		}
	      catch( std::exception & e )
		{
#ifdef CARTO_DEBUG_IO
                  std::cout << "4. failed\n";
#endif
		  io_error::keepExceptionPriority( e, excp, exct, excm );
		}
	      tried.insert( ie->second );
	      triedf.insert( reader );
	    }
	}

    // still not succeeded, it's hopeless...
    io_error::launchExcept( exct, excm, filename + " : no matching format" );
    return false;
  }

  template<class T>
  T* Reader<T>::read( Object header )
  {
#ifdef CARTO_DEBUG_IO
    std::cout << "Reader<" << DataTypeCode<T>::name() << ">\n";
#endif

     if( !_datasource )
      throw std::runtime_error( "Reader with no source of data" );
    if( !_options.get() )
      _options = Object::value( PropertySet() );

    if( !header.get() )	// no header: check it no
      {
        DataSourceInfo	dsi;
        header = dsi.check( *_datasource );
        if( !header.get() )
          dsi.launchException();
#ifdef CARTO_DEBUG_IO
        std::cout << "source checked\n";
#endif
      }
    std::string	format;
    if( !_options->getProperty( "format", format )  // user options first
        && !header->getProperty( "format", format ) ) // new style
      header->getProperty( "file_type" ); // old style

#ifdef CARTO_DEBUG_IO
    std::cout << "format: " << format << std::endl;
#endif

    std::string				filename;
    filename = _datasource->url();
    std::set<std::string>		tried;
    std::set<FormatReader<T> *>		triedf;
    FormatReader<T>			*reader;
    std::set<std::string>::iterator	notyet = tried.end();
    typename std::set<FormatReader<T> *>::iterator	notyetf = triedf.end();
    T					*obj;
    int					excp = 0;
    int					exct = -1;
    std::string				excm;

    if( !format.empty() )	// priority to format hint
      {
	reader = FormatDictionary<T>::readFormat( format );
	if( reader )
	  {
	    try
	      {
#ifdef CARTO_DEBUG_IO
                std::cout << "1. try reader " << format << std::endl;
#endif
		obj = reader->createAndRead( header, _datasource, 
                                             _alloccontext, _options );
		if( obj )
                  {
#ifdef CARTO_DEBUG_IO
                    std::cout << "1. " << format << " OK\n";
#endif
                    return obj;
                  }
	      }
	    catch( std::exception & e )
	      {
#ifdef CARTO_DEBUG_IO
                std::cout << "1. failed\n";
#endif
		io_error::keepExceptionPriority( e, excp, exct, excm, 5 );
	      }
	    tried.insert( format );
	    triedf.insert( reader );
	  }
      }

    std::string	ext = FileUtil::extension( filename );

    const std::multimap<std::string, std::string>	& extensions 
      = FormatDictionary<T>::readExtensions();

    std::pair<std::multimap<std::string, std::string>::const_iterator, 
      std::multimap<std::string, std::string>::const_iterator>	iext 
      = extensions.equal_range( ext );
    std::multimap<std::string, std::string>::const_iterator	
      ie, ee = iext.second;

    // try every matching format until one works
    for( ie=iext.first; ie!=ee; ++ie )
      if( tried.find( (*ie).second ) == notyet )
	{
	  reader = FormatDictionary<T>::readFormat( (*ie).second );
	  if( reader && triedf.find( reader ) == notyetf )
	    {
	      try
		{
#ifdef CARTO_DEBUG_IO
                  std::cout << "2. try reader " << ie->second << std::endl;
#endif
		  obj = reader->createAndRead( header, _datasource, 
                                               _alloccontext, _options );
		  if( obj )
                    {
#ifdef CARTO_DEBUG_IO
                      std::cout << "2. " << ie->second << " OK\n";
#endif
                      return obj;
                    }
		}
	      catch( std::exception & e )
		{
#ifdef CARTO_DEBUG_IO
                  std::cout << "2. failed\n";
#endif
		  io_error::keepExceptionPriority( e, excp, exct, excm );
		}
#ifdef CARTO_DEBUG_IO
              std::cout << "2. unsuccessfully tried " << ie->second 
                        << std::endl;
#endif
	      tried.insert( ie->second );
	      triedf.insert( reader );
	    }
	}

    if( !ext.empty() )
      {
	// not found or none works: try readers with no extension
	iext = extensions.equal_range( "" );

	for( ie=iext.first, ee=iext.second; ie!=ee; ++ie )
	  if( tried.find( (*ie).second ) == notyet )
	    {
	      reader = FormatDictionary<T>::readFormat( (*ie).second );
	      if( reader && triedf.find( reader ) == notyetf )
		{
		  try
		    {
#ifdef CARTO_DEBUG_IO
                      std::cout << "3. try reader " << ie->second << std::endl;
#endif
		      obj = reader->createAndRead( header, _datasource, 
                                                   _alloccontext, _options );
		      if( obj )
                        {
#ifdef CARTO_DEBUG_IO
                          std::cout << "3. " << ie->second << " OK\n";
#endif
                          return obj;
                        }
		    }
		  catch( std::exception & e )
		    {
#ifdef CARTO_DEBUG_IO
                      std::cout << "3. failed\n";
#endif
		      io_error::keepExceptionPriority( e, excp, exct, excm );
		    }
		  tried.insert( ie->second );
		  triedf.insert( reader );
		}
	    }
      }

    // still not found ? well, try EVERY format this time...
    iext.first = extensions.begin();
    iext.second = extensions.end();

    for( ie=iext.first, ee=iext.second; ie!=ee; ++ie )
      if( tried.find( (*ie).second ) == notyet )
	{
	  reader = FormatDictionary<T>::readFormat( ie->second );
	  if( reader && triedf.find( reader ) == notyetf )
	    {
	      try
		{
#ifdef CARTO_DEBUG_IO
                  std::cout << "4. try reader " << ie->second << std::endl;
#endif
		  obj = reader->createAndRead( header, _datasource, 
                                               _alloccontext, _options );
		  if( obj )
                    {
#ifdef CARTO_DEBUG_IO
                      std::cout << "4. " << ie->second << " OK\n";
#endif
                      return obj;
                    }
		}
	      catch( std::exception & e )
		{
#ifdef CARTO_DEBUG_IO
                  std::cout << "4. failed\n";
#endif
		  io_error::keepExceptionPriority( e, excp, exct, excm );
		}
	      tried.insert( ie->second );
	      triedf.insert( reader );
	    }
	}

    // still not succeeded, it's hopeless...
    io_error::launchExcept( exct, excm, filename + " : no matching format" );
    return 0;
  }


  template <typename T>
  void Reader<T>::flush()
  {
    if( _datasource )
      _datasource->flush();
  }


  template <typename T>
  void Reader<T>::close()
  {
    if( _datasource )
      _datasource->close();
  }

}

#endif

