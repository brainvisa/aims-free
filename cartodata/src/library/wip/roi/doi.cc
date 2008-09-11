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

#include <stdexcept>
#include <cartodata/wip/roi/doi.h>

#define INSTANTIATE_VIRTUAL_GENERIC_OBJECT_TYPE( T )    \
template class TypedObject< T >; \
template class ReferenceObject< T >; \
template class PointerObject< T >; \
template \
T const &GenericObject::value< T >() const; \
template \
T &GenericObject::value< T >(); \
template <>\
bool GenericObject::value( T & ) const \
{ return false; } \
template <> \
void GenericObject::setValue( T const & ) \
{ throw std::runtime_error( "cannot use setValue() with virtual class" ); } \
template bool DictionaryInterface:: \
getProperty( const std::string &, T &  ) const; \
template void DictionaryInterface:: \
setProperty( const std::string &, T const & ); \
template <> \
void DictionaryInterface::setProperty( const std::string &key, \
                                       T const &value ) \
{ \
  this->setProperty( key, Object::value( value ) ); \
} \
template <> \
bool DictionaryInterface::getProperty( const std::string &key, T &value ) const \
{ \
  Object	o; \
  if( getProperty( key, o ) && o.get() ) \
    { \
      try \
        { \
          value = o->GenericObject::value< T >(); \
          return true; \
        } \
      catch( ... ) \
        { \
        } \
    } \
  return false; \
} \



using namespace std;

namespace carto
{

  //------------------//
 //  RcptrObject<T>  //
//------------------//

//-----------------------------------------------------------------------------
template <typename T>
RcptrObject<T>::~RcptrObject() {}


//-----------------------------------------------------------------------------
template <typename T>
T &RcptrObject<T>::getValue()
{
  return *_pvalue;
}

//-----------------------------------------------------------------------------
template <typename T>
Object RcptrObject<T>::clone() const
{
  return Object( new RcptrObject<T>( *_pvalue ) );
}



  //----------------//
 //  DiscreteDOI   //
//----------------//

//------------------------------------------------------------------------------
DiscreteDOI::~DiscreteDOI() {}


//------------------------------------------------------------------------------
Object DiscreteDOI::valueAt( const Site & ) const
{
  throw runtime_error( "valueAt( site ) method not implemented" );
}


//------------------------------------------------------------------------------
// INSTANTIATE_VIRTUAL_GENERIC_OBJECT_TYPE( DiscreteDOI );

#if 0
#define T DiscreteDOI
template class TypedObject< T >;
template class ReferenceObject< T >;
template class RcptrObject< T >;
template
T const &GenericObject::value< T >() const;
template
T &GenericObject::value< T >();
template <>
bool GenericObject::value( T & ) const
{
  throw std::invalid_argument( std::string( "value( x ) method is "
                                            "not allowed for abstact class " )
                               +  DataTypeCode<T>::name()  );
}
template <>
void GenericObject::setValue( T const & )
{
  throw std::invalid_argument( std::string( "setValue( x ) method is "
                                            "not allowed for abstact class " )
                               +  DataTypeCode<T>::name()  );
}
//template bool DictionaryInterface::
//getProperty( const std::string &, T & ) const;
template <> bool DictionaryInterface::
getProperty( const std::string &, T & ) const
{
  throw std::invalid_argument( std::string( "getProperty( name, x ) method is "
                                            "not allowed for abstact class " )
                               +  DataTypeCode<T>::name()  );
}
//template
//void DictionaryInterface::setProperty( const std::string &key,
//                                       T const &value );
template <>
void DictionaryInterface::setProperty( const std::string &key,
                                       T const &value )
{
  this->setProperty( key, Object::value( value ) );
}
#undef T
#endif

  //-------------------//
 //  NumberIterator   //
//-------------------//

//------------------------------------------------------------------------------
inline Object NumberIterator::currentValue() const
{
  // TODO
  return Object();
}


//------------------------------------------------------------------------------
// INSTANTIATE_VIRTUAL_GENERIC_OBJECT_TYPE( NumberIterator );


  //----------------------//
 //  DiscreteNumberDOI   //
//----------------------//

//------------------------------------------------------------------------------
Object DiscreteNumberDOI::objectIterator() const
{
  // TODO
  return Object();
  //return Object::value( numberIterator() );
}

//------------------------------------------------------------------------------
Object DiscreteNumberDOI::valueAt( const Site &site ) const
{
  return Object::value( numberAt( site ) );
}


//------------------------------------------------------------------------------
double DiscreteNumberDOI::numberAt( const Site & ) const
{
  throw runtime_error( "numberAt( site ) method not implemented" );
}


//------------------------------------------------------------------------------
// INSTANTIATE_VIRTUAL_GENERIC_OBJECT_TYPE( DiscreteNumberDOI );

} //namespace carto
