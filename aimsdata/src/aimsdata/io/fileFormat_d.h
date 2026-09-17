#ifndef AIMS_IO_FILEFORMAT_D_H
#define AIMS_IO_FILEFORMAT_D_H


#include <aims/io/fileFormat.h>
#include <cartobase/thread/mutex.h>
#include <cartobase/stream/fileutil.h>

namespace aims
{

  template<typename T> 
  FileFormat<T>::~FileFormat()
  {
  }


  template<typename  T> T*
  FileFormat<T>::read( const std::string & filename, 
                       const carto::AllocatorContext & context, 
                       carto::Object options )
  {
    T	*object = new T;
    try
      {
	if( read( filename, *object, context, options ) )
	  return object;
      }
    catch( std::exception & )
      {
	delete object;
	throw;
      }
    delete object;
    return 0;
  }


  template <typename T>
  bool FileFormat<T>::write( const std::string &, const T &, carto::Object )
  {
    return false;
  }


  template<class T> std::map<std::string, FileFormat<T>*> 
  & FileFormatDictionary<T>::_formats()
  {
    static std::map<std::string, FileFormat<T>*> form;
    return form;
  }

  template<class T> std::map<std::string, std::list<std::string> >
  & FileFormatDictionary<T>::_extensions()
  {
    static std::map<std::string, std::list<std::string> >	ext;
    return ext;
  }


  template<class T>
  carto::Mutex & FileFormatDictionary<T>::mutex()
  {
    // Must be initialized (generally in main thread) before using concurrently
    static carto::Mutex mutex( carto::Mutex::Recursive );
    return mutex;
  }


  template<class T>
      const std::map<std::string, std::list<std::string> > &
  FileFormatDictionary<T>::extensions()
  {
    init();
    return _extensions();
  }

  template<class T>
  void FileFormatDictionary<T>::init()
  {
    static bool initialized = false;
    if( !initialized )
    {
      initialized = true;
      carto::DataTypeCode<T>	dtc;
      IOObjectTypesDictionary::registerType( dtc.objectType(),
                                              dtc.dataType(), &formats );
      registerBaseFormats();
    }
  }

  template<class T> std::string 
  FileFormatDictionary<T>::fileExtension( const std::string & filename )
  {
    return( carto::FileUtil::extension(filename) );
  }

  template<class T> void 
  FileFormatDictionary<T>::registerFormat( const std::string & format, 
                                           FileFormat<T>* formatObj,
                                           const std::vector<std::string>
                                               & extensions,
                                           const std::string & before )
  {
    init();

    FileFormat<T>	*oldr = fileFormat( format );
    delete oldr;
    _formats()[ format ] = formatObj;

    std::vector<std::string>::const_iterator	ie, ee = extensions.end();
    std::list<std::string>::iterator  ie2, ee2;
    for( ie=extensions.begin(); ie!=ee; ++ie )
    {
      std::list<std::string> & ext = _extensions()[ *ie ];
      if( before.empty() )
        ext.push_back( format );
      else
      {
        for( ie2=ext.begin(), ee2=ext.end(); ie2!=ee2; ++ie2 )
          if( *ie2 == before )
            break;
        ext.insert( ie2, format );
      }
    }
  }


  template<class T> void
  FileFormatDictionary<T>::unregisterFormat( const std::string & format )
  {
    typename std::map<std::string, FileFormat<T>*>::iterator
      ir = _formats().find( format);

    if( ir != _formats().end() )
      _formats().erase( ir );

    std::map<std::string, std::list<std::string> >::iterator
      ie = _extensions().begin(), je, ee = _extensions().end();
    std::list<std::string>::iterator il, jl, el;

    while( ie != ee )
    {
      je = ie;
      ++ie;
      il = je->second.begin();
      el = je->second.end();
      while( il != el )
      {
        jl = il;
        ++il;
        
        if( (il != el)  && (*il == format ) )
        {
          je->second.erase( il );
          el = je->second.end();
          if( je->second.empty() )
            _extensions().erase( je );
        }
      }
    }
  }


  template<class T> FileFormat<T> *
  FileFormatDictionary<T>::fileFormat( const std::string & format )
  {
    init();

    typename std::map<std::string, FileFormat<T>*>::const_iterator	i 
      = _formats().find( format );
    if( i == _formats().end() )
      return( 0 );
    return( (*i).second );
  }


  template<class T> std::set<std::string> FileFormatDictionary<T>::formats()
  {
    std::set<std::string>	f;
    typename std::map<std::string, FileFormat<T>*>::const_iterator 
      i, e = _formats().end();
    for( i=_formats().begin(); i!=e; ++i )
      f.insert( i->first );
    return( f );
  }

}


#endif
