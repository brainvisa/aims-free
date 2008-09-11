/* Copyright (c) 1995-2005 CEA
 *
 *  This software and supporting documentation were developed by
 *      CEA/DSV/SHFJ
 *      4 place du General Leclerc
 *      91401 Orsay cedex
 *      France
 *
 * This software is governed by the CeCILL license version 2 under 
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the 
 * terms of the CeCILL license version 2 as circulated by CEA, CNRS
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
 * knowledge of the CeCILL license version 2 and that you accept its terms.
 */

#ifndef AIMS_IO_FILEFORMAT_D_H
#define AIMS_IO_FILEFORMAT_D_H


#include <aims/io/fileFormat.h>

namespace aims
{

  template<typename T> 
  FileFormat<T>::~FileFormat()
  {
  }


  template<class T> T* 
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
    std::string::size_type	pos = filename.rfind( '.' );
    std::string::size_type	pos1 = filename.rfind( '/' );
    std::string			ext;

    if( pos != std::string::npos 
	&& ( pos1 == std::string::npos || pos1 < pos ) )
      ext = filename.substr( pos+1, filename.length() - pos - 1 );
    return( ext );
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
