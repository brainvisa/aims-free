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

#ifndef CARTOBASE_IO_FORMATDICTIONARY_D_H
#define CARTOBASE_IO_FORMATDICTIONARY_D_H

#include <cartobase/wip/io/formatdictionary.h>
#include <cartobase/wip/io/formatreader.h>
#include <cartobase/wip/io/formatwriter.h>
#include <cartobase/type/types.h>

namespace carto
{

  template <typename T> 
  std::map<std::string, FormatReader<T>*> & 
  FormatDictionary<T>::_readformats()
  {
    static std::map<std::string, FormatReader<T>*> form;
    return form;
  }

  template <typename T> 
  std::map<std::string, FormatWriter<T>*> & 
  FormatDictionary<T>::_writeformats()
  {
    static std::map<std::string, FormatWriter<T>*> form;
    return form;
  }

  template <typename T> 
  std::multimap<std::string, std::string> & 
  FormatDictionary<T>::_readextensions()
  {
    static std::multimap<std::string, std::string>	ext;
    return ext;
  }

  template <typename T> 
  std::multimap<std::string, std::string> & 
  FormatDictionary<T>::_writeextensions()
  {
    static std::multimap<std::string, std::string>	ext;
    return ext;
  }

  template <typename T>
  const std::multimap<std::string, std::string> & 
  FormatDictionary<T>::readExtensions()
  {
    init();
    return _readextensions();
  }

  template <typename T>
  const std::multimap<std::string, std::string> & 
  FormatDictionary<T>::writeExtensions()
  {
    init();
    return _writeextensions();
  }

  template <typename T>
  void FormatDictionary<T>::init()
  {
    static bool initialized = false;
    if( !initialized )
      {
	initialized = true;
	carto::DataTypeCode<T>	dtc;
	IOObjectTypesDictionary::registerReadType( dtc.name(), 
                                                   &readFormats );
	IOObjectTypesDictionary::registerWriteType( dtc.name(), 
                                                    &writeFormats );
	registerBaseFormats();
      }
  }


  template <typename T>
  void FormatDictionary<T>::registerBaseFormats()
  {
  }


  template <typename T> 
  void 
  FormatDictionary<T>::registerFormat( const std::string & format, 
                                       FormatReader<T>* formatObj, 
                                       const std::vector<std::string> 
                                       & extensions )
  {
    init();

    FormatReader<T>	*oldr = readFormat( format );
    delete oldr;
    _readformats()[ format ] = formatObj;

    std::vector<std::string>::const_iterator	ie, ee = extensions.end();
    for( ie=extensions.begin(); ie!=ee; ++ie )
      _readextensions().insert
        ( std::pair<std::string, std::string>( *ie, format ) );
  }


  template <typename T> 
  void 
  FormatDictionary<T>::registerFormat( const std::string & format, 
                                       FormatWriter<T>* formatObj, 
                                       const std::vector<std::string> 
                                       & extensions )
  {
    init();

    FormatWriter<T>	*oldr = writeFormat( format );
    delete oldr;
    _writeformats()[ format ] = formatObj;

    std::vector<std::string>::const_iterator	ie, ee = extensions.end();
    for( ie=extensions.begin(); ie!=ee; ++ie )
      _writeextensions().insert
        ( std::pair<std::string, std::string>( *ie, format ) );
  }


  template <typename T> 
  FormatReader<T> * 
  FormatDictionary<T>::readFormat( const std::string & format )
  {
    init();

    typename std::map<std::string, FormatReader<T>*>::const_iterator	i 
      = _readformats().find( format );
    if( i == _readformats().end() )
      return( 0 );
    return( (*i).second );
  }


  template <typename T> 
  FormatWriter<T> * 
  FormatDictionary<T>::writeFormat( const std::string & format )
  {
    init();

    typename std::map<std::string, FormatWriter<T>*>::const_iterator	i 
      = _writeformats().find( format );
    if( i == _writeformats().end() )
      return( 0 );
    return( (*i).second );
  }


  template <typename T> 
  std::set<std::string> FormatDictionary<T>::readFormats()
  {
    std::set<std::string>	f;
    typename std::map<std::string, FormatReader<T>*>::const_iterator 
      i, e = _readformats().end();
    for( i=_readformats().begin(); i!=e; ++i )
      f.insert( i->first );
    return( f );
  }


  template <typename T> 
  std::set<std::string> FormatDictionary<T>::writeFormats()
  {
    std::set<std::string>	f;
    typename std::map<std::string, FormatWriter<T>*>::const_iterator 
      i, e = _writeformats().end();
    for( i=_writeformats().begin(); i!=e; ++i )
      f.insert( i->first );
    return( f );
  }

}


#endif

