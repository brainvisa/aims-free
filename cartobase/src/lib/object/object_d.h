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

#ifndef CARTOBASE_OBJECT_OBJECT_D_H
#define CARTOBASE_OBJECT_OBJECT_D_H

#include <cartobase/object/object.h>

#define INSTANTIATE_GENERIC_OBJECT_TYPE( T )    \
template class TypedObject< T >; \
template class ValueObject< T >; \
template class ReferenceObject< T >; \
template class PointerObject< T >; \
template \
T const &GenericObject::value< T >() const; \
template \
T &GenericObject::value< T >(); \
template \
bool GenericObject::value( T &dest ) const; \
template \
void GenericObject::setValue( T const & x ); \
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


namespace carto
{

// For lisibility, it has been decided to put a part of object
// implementation in a separate header.
#include <cartobase/object/object_internal.h>


  //------------------------//
 //  DictionaryInterface   //
//------------------------//
/*

//---------------------------------------------------------------------------
template <typename T>
void DictionaryInterface::setProperty( const std::string &key, 
                                       const T &value )
{
  this->setProperty( key, Object::value( value ) );
}


//---------------------------------------------------------------------------
template <typename T> 
bool DictionaryInterface::getProperty( const std::string &key, T &value ) const
{
  Object	o;
  if( getProperty( key, o ) && o.get() )
    {
      try
        {
          value = o->GenericObject::value<T>();
          return true;
        }
      catch( ... )
        {
        }
    }
  return false;
}
*/


  //-----------------//
 //  GenericObject  //
//-----------------//

//-----------------------------------------------------------------------------
template<typename T> 
const T &GenericObject::value() const
{
  const TypedObject<T> *u = dynamic_cast<const TypedObject<T> *>( this );
  if( u ) return( u->getValue() );
  throw std::invalid_argument( std::string( "cannot convert object from " )
                               + type() + " to " + DataTypeCode<T>::name() 
                               + " (dynamic cast failed from " 
                               + typeid(*this).name() + " to " 
                               + typeid(u).name() + ")" );
}


//-----------------------------------------------------------------------------
template<typename T>
T &GenericObject::value()
{
  TypedObject<T> *u = dynamic_cast<TypedObject<T> *>( this );
  if( u ) return( u->getValue() );
  throw std::invalid_argument( std::string( "cannot convert object from " ) 
                               + type() + " to " + DataTypeCode<T>::name()
                               + " (dynamic cast failed from " 
                               + typeid(*this).name() + " to " 
                               + typeid(u).name() + ")" );
}


//-----------------------------------------------------------------------------
template <typename T> 
bool GenericObject::value( T &dest ) const
{
  const TypedObject<T> *u = dynamic_cast<const TypedObject<T> *>( this );
  if( u ) {
    dest = u->getValue();
    return true;
  }
  return false;
}


//-----------------------------------------------------------------------------
template<typename T> 
void GenericObject::setValue( const T & x )
{
  TypedObject<T> *u = dynamic_cast<TypedObject<T> *>( this );
  if( u ) {
    u->getValue() = x;
  } else {
    throw std::invalid_argument( std::string( "cannot write value of type " ) 
                                 + DataTypeCode<T>::name() 
                                 + " to object of type " + type() );
  }
}


  //------------------//
 //  TypedObject<T>  //
//------------------//

//-----------------------------------------------------------------------------
template <typename T>
TypedObject<T>::TypedObject()
{
  GenericObjectTypeDeclared<T>::check();
#ifdef CARTO_DEBUG
  _debugInstantiation = _debugInstantiation;
#endif
}


//-----------------------------------------------------------------------------
template <typename T>
TypedObject<T>::~TypedObject()
{
}


//-----------------------------------------------------------------------------
template <typename T>
const T &TypedObject<T>::getValue() const
{
  return const_cast< TypedObject<T> & >( *this ).getValue(); 
}


//-----------------------------------------------------------------------------
template <typename T>
void TypedObject<T>::setValue( Object o )
{
  object_to( o, getValue() );
}


//-----------------------------------------------------------------------------
template<typename T>
std::string TypedObject<T>::type() const
{
  return DataTypeCode<T>::name();
}


//-----------------------------------------------------------------------------
template <typename T>
Interface *TypedObject<T>::_getGenericInterface()
{
  return interface_internal::
    GenericInterface< T, SUPERSUBCLASS(Interface,T) >::get( *this );
}


//-----------------------------------------------------------------------------
template <typename T>
const void *TypedObject<T>::_getAddressOfValue() const
{
  return &getValue();
}

//-----------------------------------------------------------------------------
template <typename T>
T &TypedObject<T>::getValue()
{
  throw std::runtime_error( "pure virtual function called" );
}


//-----------------------------------------------------------------------------
template <typename T>
Object TypedObject<T>::clone() const
{
  throw std::runtime_error( "pure virtual function called" );
}


#ifdef CARTO_DEBUG
//-----------------------------------------------------------------------------
template <typename T>
bool TypedObject<T>::_debugInstantiation = 
  PrintInstantiation<T>::doIt( &TypedObject<T>::_debugInstantiation );


//-----------------------------------------------------------------------------
template <typename T>
bool PrintInstantiation<T>::doIt( bool *address )
{
  if ( verbose ) {
    std::cerr << "!instantiation! " << typeid(T).name() << " "
              << "mutable "
              << address << " "
              <<  DataTypeCode<T>::name() << std::endl;
  }
  return true;
}

//-----------------------------------------------------------------------------
template <typename T>
bool PrintInstantiation<const T>::doIt( bool *address)
{
  if ( verbose ) {
    std::cerr << "!instantiation! " << typeid(T).name() << " "
              << "const "
              << (void *) address << " "
              <<  DataTypeCode<T>::name() << std::endl;
  }
  return true;
}

#endif // ifdef CARTO_DEBUG 


//-----------------------------------------------------------------------------
// ScalarInterface methods
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template <typename T>
bool TypedObject<T>::isScalar() const
{
  return interface_internal::
    ScalarImpl< T, SUPERSUBCLASS(ScalarInterface,T) >::isScalar( *this );
}


//-----------------------------------------------------------------------------
template <typename T>
double TypedObject<T>::getScalar() const
{
  return interface_internal::
    ScalarImpl< T, SUPERSUBCLASS(ScalarInterface,T) >::getScalar( *this );
}


//-----------------------------------------------------------------------------
template <typename T>
void TypedObject<T>::setScalar( double v )
{
  return interface_internal::
    ScalarImpl< T, SUPERSUBCLASS(ScalarInterface,T) >::setScalar( *this, v );
}


//-----------------------------------------------------------------------------
// StringInterface methods
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template <typename T>
bool TypedObject<T>::isString() const
{
  return interface_internal::
    StringImpl< T, SUPERSUBCLASS(StringInterface,T) >::isString( *this );
}


//-----------------------------------------------------------------------------
template <typename T>
std::string TypedObject<T>::getString() const
{
  return interface_internal::
    StringImpl< T, SUPERSUBCLASS(StringInterface,T) >::getString( *this );
}


//-----------------------------------------------------------------------------
template <typename T>
void TypedObject<T>::setString( const std::string &v )
{
  return interface_internal::
    StringImpl< T, SUPERSUBCLASS(StringInterface,T) >::setString( *this, v );
}


//-----------------------------------------------------------------------------
// ArrayInterface methods
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template <typename T>
bool TypedObject<T>::isArray() const
{
  return interface_internal::
    ArrayImpl< T, SUPERSUBCLASS(ArrayInterface,T) >::isArray( *this );
}


//-----------------------------------------------------------------------------
template <typename T>
Object TypedObject<T>::getArrayItem( int index ) const
{
  return interface_internal::
    ArrayImpl< T, SUPERSUBCLASS(ArrayInterface,T) >::
    getArrayItem( *this, index );
}


//-----------------------------------------------------------------------------
template <typename T>
void TypedObject<T>::setArrayItem( int index, Object value )
{
  interface_internal::
    ArrayImpl< T, SUPERSUBCLASS(ArrayInterface,T) >::
    setArrayItem( *this, index, value );
}


//-----------------------------------------------------------------------------
// DynArrayInterface methods
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template <typename T>
bool TypedObject<T>::isDynArray() const
{
  return interface_internal::
    DynArrayImpl< T, SUPERSUBCLASS(DynArrayInterface,T) >::isDynArray( *this );
}


//-----------------------------------------------------------------------------
template <typename T>
void TypedObject<T>::reserveArray( size_t size )
{
  interface_internal::
    DynArrayImpl< T, SUPERSUBCLASS(DynArrayInterface,T) >::
    reserveArray( *this, size );
}


//-----------------------------------------------------------------------------
template <typename T>
void TypedObject<T>::resizeArray( size_t size )
{
  interface_internal::
    DynArrayImpl< T, SUPERSUBCLASS(DynArrayInterface,T) >::
    resizeArray( *this, size );
}


//-----------------------------------------------------------------------------
template <typename T>
void TypedObject<T>::removeArrayItem( int index )
{
  interface_internal::
    DynArrayImpl< T, SUPERSUBCLASS(DynArrayInterface,T) >::
    removeArrayItem( *this, index );
}


//-----------------------------------------------------------------------------
template <typename T>
void TypedObject<T>::insertArrayItem( int index, Object value )
{
  interface_internal::
    DynArrayImpl< T, SUPERSUBCLASS(DynArrayInterface,T) >::
    insertArrayItem( *this, index, value );
}



//-----------------------------------------------------------------------------
// SizeInterface methods
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template <typename T>
size_t TypedObject<T>::size() const
{
  return interface_internal::SizeImpl< T, SUPERSUBCLASS(SizeInterface,T) >::
    size( *this );
}


//-----------------------------------------------------------------------------
// DictionaryInterface methods
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template <typename T>
bool TypedObject<T>::isDictionary() const
{
  return interface_internal::
    DictionaryImpl< T, SUPERSUBCLASS(DictionaryInterface,T) >
    ::isDictionary( *this );
}


//-----------------------------------------------------------------------------
template <typename T>
bool TypedObject<T>::getProperty( const std::string &key, 
                                  Object & result ) const
{
  return interface_internal::
    DictionaryImpl< T, SUPERSUBCLASS(DictionaryInterface,T) >::
    getProperty( *this, key, result );
}


//-----------------------------------------------------------------------------
template <typename T>
void TypedObject<T>::setProperty( const std::string &key, Object value )
{
  interface_internal::
    DictionaryImpl< T, SUPERSUBCLASS(DictionaryInterface,T) >::
    setProperty( *this, key, value );
}


//-----------------------------------------------------------------------------
template <typename T>
bool TypedObject<T>::removeProperty( const std::string &key )
{
  return interface_internal::
    DictionaryImpl< T, SUPERSUBCLASS(DictionaryInterface,T) >::
    removeProperty( *this,key );
}


//-----------------------------------------------------------------------------
template <typename T>
void TypedObject<T>::clearProperties()
{
  interface_internal::
    DictionaryImpl< T, SUPERSUBCLASS(DictionaryInterface,T) >::
    clearProperties( *this );
}


//-----------------------------------------------------------------------------
template <typename T>
bool TypedObject<T>::hasProperty( const std::string &key ) const
{
  return interface_internal::
    DictionaryImpl< T, SUPERSUBCLASS(DictionaryInterface,T) >::
    hasProperty( *this,key );
}


//-----------------------------------------------------------------------------
// IterableInterface methods
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template <typename T>
bool TypedObject<T>::isIterable() const
{
  return interface_internal::
    IterableImpl< T, SUPERSUBCLASS(IterableInterface,T) >::isIterable( *this );
}


//-----------------------------------------------------------------------------
template <typename T>
Object TypedObject<T>::objectIterator() const
{
  return interface_internal::
    IterableImpl< T, SUPERSUBCLASS(IterableInterface,T) >::
    objectIterator( *this );
}


//-----------------------------------------------------------------------------
// IteratorInterface methods
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template <typename T>
bool TypedObject<T>::isIterator() const
{
  return interface_internal::
    IteratorImpl< T, SUPERSUBCLASS(IteratorInterface,T) >::isIterator( *this );
}


//-----------------------------------------------------------------------------
template <typename T>
bool TypedObject<T>::isValid() const
{
  return interface_internal::
    IteratorImpl< T, SUPERSUBCLASS(IteratorInterface,T) >::
    isValid( *this );
}


//-----------------------------------------------------------------------------
template <typename T>
Object TypedObject<T>::currentValue() const
{
  return interface_internal::
    IteratorImpl< T, SUPERSUBCLASS(IteratorInterface,T) >::
    currentValue( *this );
}


//-----------------------------------------------------------------------------
template <typename T>
void TypedObject<T>::next()
{
  interface_internal::
    IteratorImpl< T, SUPERSUBCLASS(IteratorInterface,T) >::
    next( *this );
}


//-----------------------------------------------------------------------------
// DictionaryIteratorInterface methods
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template <typename T>
bool TypedObject<T>::isDictionaryIterator() const
{
  return interface_internal::
    DictionaryIteratorImpl< T, SUPERSUBCLASS(DictionaryIteratorInterface,T) >
    ::isDictionaryIterator( *this );
}


//-----------------------------------------------------------------------------
template <typename T>
std::string TypedObject<T>::key() const
{
  return interface_internal::
    DictionaryIteratorImpl< T, SUPERSUBCLASS(DictionaryIteratorInterface,T) >::
    key( *this );
}


  //------------------//
 //  ValueObject<T>  //
//------------------//

//-----------------------------------------------------------------------------
template <typename T>
ValueObject<T>::~ValueObject()
{
}


//-----------------------------------------------------------------------------
template <typename T>
T &ValueObject<T>::getValue()
{
  return _value;
}


//-----------------------------------------------------------------------------
template <typename T>
Object ValueObject<T>::clone() const
{
  return Object::value( _value );
}


  //----------------------//
 //  ReferenceObject<T>  //
//----------------------//

//-----------------------------------------------------------------------------
template <typename T>
ReferenceObject<T>::~ReferenceObject()
{
}


//-----------------------------------------------------------------------------
template <typename T>
T &ReferenceObject<T>::getValue()
{
  return _value;
}

//-----------------------------------------------------------------------------
template <typename T>
Object ReferenceObject<T>::clone() const
{
  return Object::reference( _value );
}


  //--------------------//
 //  PointerObject<T>  //
//--------------------//

//-----------------------------------------------------------------------------
template <typename T>
PointerObject<T>::~PointerObject()
{
  if ( _owner ) delete _pvalue;
}


//-----------------------------------------------------------------------------
template <typename T>
T &PointerObject<T>::getValue()
{
  return *_pvalue;
}

//-----------------------------------------------------------------------------
template <typename T>
Object PointerObject<T>::clone() const
{
  return Object( new PointerObject<T>( *_pvalue, false ) );
}


} // namespace carto

#endif // ifndef CARTOBASE_OBJECT_OBJECT_D_H
