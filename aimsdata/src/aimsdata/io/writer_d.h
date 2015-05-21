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

/*
 *  writer class
 */
#ifndef AIMS_IO_WRITER_D_H
#define AIMS_IO_WRITER_D_H


#include <aims/io/writer.h>
#include <aims/io/fileFormat.h>
#include <aims/def/settings.h>
#include <cartobase/exception/ioexcept.h>
#include <cartobase/stream/fileutil.h>
#include <cartobase/plugin/plugin.h>
#include <set>
#include <soma-io/io/writer.h>
#include <soma-io/io/formatdictionary.h>

#define AIMS_INSTANTIATE_WRITER( T ) \
  namespace aims { \
    template class aims::Writer< T >; \
    template bool \
    GenericWriter::write< T >( const T &, bool, const std::string * ); \
  }

  #define AIMS_INSTANTIATE_ONLY_WRITER( T ) \
  namespace aims { \
    template class aims::Writer< T >; \
  }

#include <iostream>


/* WARNING: be careful to include <aims/io/datatypecode.h> before 
   including this writer_d.h header
*/

namespace aims
{

  template <typename T>
  bool GenericWriter::write( const T & obj, bool ascii, 
			     const std::string* format )
  {
    Writer<T> *writer = dynamic_cast< Writer<T> * >( this );
    if ( ! writer ) {
      throw carto::format_mismatch_error( fileName() );
    }
    return writer->write( obj, ascii, format );
  }



  template<class T>
  std::string Writer<T>::writtenObjectType() const
  {
    return carto::DataTypeCode<T>().objectType();
  }


  template<class T>
  std::string Writer<T>::writtenObjectDataType() const
  {
    return carto::DataTypeCode<T>().dataType();
  }
  

  template<class T>
  std::string Writer<T>::writtenObjectFullType() const
  {
    return carto::DataTypeCode<T>::name();
  }



  template<class T>
  bool Writer<T>::write( const T & obj, bool ascii, const std::string* format )
  {
    // try first soma-io writer (since 2013)
    // try first 3 passes
    try{
      // building uri
      std::string uri = _filename;
      if( ascii || format )
        uri += "?";
      if( ascii )
        uri += "ascii";
      if( ascii && format )
        uri += "&";
      if ( format )
        uri += ( "format=" + *format );

      soma::Writer<T> writer( uri );
      return writer.write( obj, _options, 1, 3 );
    } catch( ... ) {}
    // if it failed, continue with aims writer.

    // force loading plugins if it has not been done already
    carto::PluginLoader::load();

    std::set<std::string>		tried;
    FileFormat<T>			*writer;
    std::set<std::string>::iterator	notyet = tried.end();
    int					excp = 0;
    int					exct = -1;
    std::string				excm;

    bool exactformat = false;
    carto::Object options = carto::Object::value( carto::Dictionary() );
    if( !_options.isNull() )
    {
      try
      {
        carto::Object exact = _options->getProperty( "exact_format" );
        exactformat = (bool) exact->getScalar();
      }
      catch( ... )
      {
      }
      options->copyProperties( _options );
    }

    if( !options->hasProperty( "ascii" ) )
      options->setProperty( "ascii", ascii );

    if( format )	// priority to format hint
    {
      writer = FileFormatDictionary<T>::fileFormat( *format );
      if( writer )
      {
#ifdef AIMS_DEBUG_IO
        std::cout << "1. try writer " << *format << std::endl;
#endif
        try
        {
          if( writer->write( _filename, obj, options ) )
            return true;
          else if( exactformat )
            throw carto::wrong_format_error( std::string( "Cannot write "
              "object in format " ) + *format, _filename );
        }
        catch( std::exception & e )
        {
          if( exactformat )
            throw;
          carto::io_error::keepExceptionPriority( e, excp, exct, excm,
                                                  5 );
        }
        tried.insert( *format );
      }
    }

    std::string                bname = carto::FileUtil::basename( _filename );
    std::string::size_type     pos = bname.find( '.' );
    std::string::size_type     dlen = _filename.length() - bname.length();
    std::string                ext;

    if( pos != std::string::npos )
      ext = _filename.substr( dlen+pos+1, _filename.length() - pos - 1 );

    const std::map<std::string, std::list<std::string> >        & extensions
        = FileFormatDictionary<T>::extensions();

    std::map<std::string, std::list<std::string> >::const_iterator iext
        = extensions.find( ext ),
    eext = extensions.end();
    std::list<std::string>::const_iterator ie, ee;

    if( ext.empty() )
    {
      // no extension: use settings for default_writer if one is specified
      const Settings      &sett = Settings::settings();
      try
      {
        carto::Object defwriters = sett.getProperty( "default_writers" );
        carto::Object defformato;
        try
        {
          defformato
            = defwriters->getProperty( carto::DataTypeCode<T>::name() );
        }
        catch( std::exception & )
        {
          // exact type not found, try just the object type
          defformato
            = defwriters->getProperty( carto::DataTypeCode<T>::objectType() );
        }
        if( defformato )
        {
          std::string defformat = defformato->getString();
          if( tried.find( defformat ) == notyet )
          {
            writer = FileFormatDictionary<T>::fileFormat( defformat );
            if( writer )
            {
#ifdef AIMS_DEBUG_IO
              std::cout << "2. try writer " << defformat << std::endl;
#endif
              try
              {
                if( writer->write( _filename, obj, options ) )
                  return true;
                else if( exactformat )
                  throw carto::wrong_format_error( std::string( "Cannot write "
                    "object in format " ) + defformat, _filename );
              }
              catch( std::exception & e )
              {
                if( exactformat )
                  throw;
                carto::io_error::keepExceptionPriority( e, excp, exct, excm,
                                                        5 );
              }
              tried.insert( defformat );
            }
          }
        }
      }
      catch( std::exception & )
      {
      }
    }

    while( iext == eext && (pos=bname.find( '.', pos+1 ))!=std::string::npos )
    {
      ext = _filename.substr( dlen+pos+1, _filename.length() - pos - 1 );
      iext = extensions.find( ext );
    }

    // try every matching format until one works
    if( iext != eext )
      for( ie=iext->second.begin(), ee=iext->second.end(); ie!=ee; ++ie )
        if( tried.find( *ie ) == notyet )
        {
          writer = FileFormatDictionary<T>::fileFormat( *ie );
          if( writer )
          {
#ifdef AIMS_DEBUG_IO
            std::cout << "3. try writer " << *ie << std::endl;
#endif
            try
            {
              if( writer->write( _filename, obj, options ) )
                return true;
              else if( exactformat )
                throw carto::wrong_format_error( std::string( "Cannot write "
                  "object in format " ) + *ie, _filename );
            }
            catch( std::exception & e )
            {
#ifdef AIMS_DEBUG_IO
              std::cout << "3. failed for " << *ie << ": " << e.what()
                << std::endl;
#endif
              if( exactformat )
                throw;
              carto::io_error::keepExceptionPriority( e, excp, exct,
                                                      excm );
            }
            tried.insert( *ie );
          }
        }

    if( !ext.empty() )
    {
      // not found or none works: try writers with no extension
      iext = extensions.find( "" );

      if( iext != eext )
        for( ie=iext->second.begin(), ee=iext->second.end(); ie!=ee; ++ie )
          if( tried.find( *ie ) == notyet )
          {
            writer = FileFormatDictionary<T>::fileFormat( *ie );
            if( writer )
            {
#ifdef AIMS_DEBUG_IO
              std::cout << "4. try writer " << *ie << std::endl;
#endif
              try
              {
                if( writer->write( _filename, obj, options ) )
                  return true;
                else if( exactformat )
                  throw carto::wrong_format_error( std::string( "Cannot write "
                    "object in format " ) + *ie, _filename );
              }
              catch( std::exception & e )
              {
                if( exactformat )
                  throw;
                carto::io_error::keepExceptionPriority( e, excp, exct,
                                                        excm );
              }
              tried.insert( *ie );
            }
          }
    }

    // still not found ? well, try EVERY format this time...
    for( iext=extensions.begin(); iext!=eext; ++iext )
      for( ie=iext->second.begin(), ee=iext->second.end(); ie!=ee; ++ie )
        if( tried.find( *ie ) == notyet )
        {
          writer = FileFormatDictionary<T>::fileFormat( *ie );
          if( writer )
          {
#ifdef AIMS_DEBUG_IO
            std::cout << "5. try writer " << *ie << std::endl;
#endif
            try
            {
              if( writer->write( _filename, obj, options ) )
                return true;
              else if( exactformat )
                throw carto::wrong_format_error( std::string( "Cannot write "
                "object in format " ) + *ie, _filename );
            }
            catch( std::exception & e )
            {
              if( exactformat )
                throw;
              carto::io_error::keepExceptionPriority( e, excp, exct,
                                                      excm );
            }
            tried.insert( *ie );
          }
        }

    // try first soma-io writer (since 2013)
    // try pass 4
    try{
      // building uri
      std::string uri = _filename;
      if( ascii || format )
        uri += "?";
      if( ascii )
        uri += "ascii";
      if( ascii && format )
        uri += "&";
      if ( format )
        uri += ( "format=" + *format );

      soma::Writer<T> writer( uri );
      return writer.write( obj, _options, 4, 4 );
    } catch( ... ) {}
    // if it failed, it's hopeless

    // still not succeeded, it's hopeless...
    carto::io_error::launchExcept( exct, excm,
                                   _filename + " : no matching format" );
    return( false );
  }

}


#endif


