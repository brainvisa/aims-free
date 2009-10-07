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

#ifndef CARTOBASE_OBJECT_OBJECT_FACTORY_H
#define CARTOBASE_OBJECT_OBJECT_FACTORY_H

#include <cartobase/object/object.h>

namespace carto {

class ObjectFactory : public RCObject
{
public:

  typedef Object (*ObjectFactoryFunction)();

  static void registerFactory( const std::string &type, ObjectFactoryFunction );
  template <typename T>
  inline static void registerDefaultFactory( const std::string &type );
  static Object createObject( const std::string &type );


private:

  typedef std::map< std::string, ObjectFactoryFunction > FactoriesType;

  template <typename T>
  static Object _defaultFactoryFunction();

  ObjectFactory();

  void _registerFactory( const std::string &type, ObjectFactoryFunction );
  template <typename T>
  inline void _registerDefaultFactory( const std::string &type );
  Object _createObject( const std::string &type ) const;

  static ObjectFactory &_singleton();

  FactoriesType _factories;
};


template <typename T>
inline void ObjectFactory::registerDefaultFactory( const std::string &type )
{
  _singleton()._registerDefaultFactory<T>( type );
}


template <typename T>
Object ObjectFactory::_defaultFactoryFunction()
{
  return Object::value( T() );
}

// template <>
// Object ObjectFactory::_defaultFactoryFunction<int>()
// {
//   return Object::value( int() );
// }

template <typename T>
inline void ObjectFactory::_registerDefaultFactory( const std::string &type )
{
  _registerFactory( type, &ObjectFactory::_defaultFactoryFunction<T> );
}


} // namespace carto

#endif // ifndef CARTOBASE_OBJECT_OBJECT_FACTORY_H
