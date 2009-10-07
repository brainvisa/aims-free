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

#include <cartobase/object/object_factory.h>

using namespace std;
using namespace carto;

ObjectFactory::ObjectFactory()
{
  _registerDefaultFactory< int >( "int" ); 
  _registerDefaultFactory< float >( "float" ); 
  _registerDefaultFactory< string >( "string" ); 
  _registerDefaultFactory< vector<int> >( "int_vector" ); 
  _registerDefaultFactory< vector<float> >( "float_vector" ); 
  _registerDefaultFactory< vector<string> >( "string_vector" ); 
}


void ObjectFactory::registerFactory( const std::string &type, ObjectFactoryFunction factory )
{
  _singleton()._registerFactory( type, factory );
}


void ObjectFactory::_registerFactory( const std::string &type, ObjectFactoryFunction factory )
{
  _factories[ type ] = factory;
}


Object ObjectFactory::createObject( const std::string &type )
{
  return _singleton()._createObject( type );
}

Object ObjectFactory::_createObject( const std::string &type ) const
{
  FactoriesType::const_iterator it = _factories.find( type );
  if ( it != _factories.end() ) {
    return it->second();
  }
  return Object();
}

ObjectFactory &ObjectFactory::_singleton()
{
  static ObjectFactory factory;
  return factory;
}



