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


#include <string>
#include <vector>
#include <float.h>
#include <cartobase/object/object_d.h>
#include <limits.h>

using namespace std;

namespace carto {


  //--------//
 //  none  //
//--------//

Object none()
{
  static Object n;
  return n;
}


  //-----------------//
 //  GenericObject  //
//-----------------//

//-----------------------------------------------------------------------------
GenericObject::~GenericObject()
{
}


//-----------------------------------------------------------------------------
bool Object::isSameObject( const Object &other ) const
{
  if ( isNull() ) return other.isNull();
  return get()->_getAddressOfValue() == other->_getAddressOfValue();
}


  //----------------//
 //  object_to<T>  //
//----------------//

//-----------------------------------------------------------------------------
template <>
void object_to( Object o, bool &r )
{
  double v = o->getScalar();
  if ( v < 0 || v > 1 ) {
    throw out_of_range_error( toString( v ) );    
  }
  r = (bool) v;
}

//-----------------------------------------------------------------------------
template <>
void object_to( Object o, unsigned char &r )
{
  double v = o->getScalar();
  if ( v < 0 || v > UCHAR_MAX ) {
    throw out_of_range_error( toString( v ) );    
  }
  r = (unsigned char) v;
}

//-----------------------------------------------------------------------------
template <>
void object_to( Object o, signed char &r )
{
  double v = o->getScalar();
  if ( v < SCHAR_MIN || v > SCHAR_MAX ) {
    throw out_of_range_error( toString( v ) );    
  }
  r = (signed char) v;
}


//-----------------------------------------------------------------------------
template <>
void object_to( Object o, unsigned short &r )
{
  double v = o->getScalar();
  if ( v < 0 || v > USHRT_MAX ) {
    throw out_of_range_error( toString( v ) );    
  }
  r = (unsigned short) v;
}


//-----------------------------------------------------------------------------
template <>
void object_to( Object o, short &r )
{
  double v = o->getScalar();
  if ( v < SHRT_MIN || v > SHRT_MAX ) {
    throw out_of_range_error( toString( v ) );    
  }
  r = (short) v;
}


//-----------------------------------------------------------------------------
template <>
void object_to( Object o, unsigned &r )
{
  double v = o->getScalar();
  if ( v < 0 || v > UINT_MAX ) {
    throw out_of_range_error( toString( v ) );    
  }
  r = (unsigned) v;
}


//-----------------------------------------------------------------------------
template <>
void object_to( Object o, int &r )
{
  double v = o->getScalar();
  if ( v < INT_MIN || v > INT_MAX ) {
    throw out_of_range_error( toString( v ) );    
  }
  r = (int) v;
}



//-----------------------------------------------------------------------------
template <>
void object_to( Object o, float &r )
{
  double v = o->getScalar();
  if ( v < -FLT_MAX || v > FLT_MAX ) {
    throw out_of_range_error( toString( v ) );    
  }
  r = (float) v;
}

//-----------------------------------------------------------------------------
template <>
void object_to( Object o, string &r )
{
  string v = o->getString();
  r = v;
}


  //-------------//
 //  Interface  //
//-------------//


//---------------------------------------------------------------------------
Interface::~Interface() {}


    //-------------------//
   //  ScalarInterface  //
  //-------------------//


//---------------------------------------------------------------------------
ScalarInterface::~ScalarInterface()
{
}

//---------------------------------------------------------------------------
bool ScalarInterface::isScalar() const
{
  return true;
}


    //-------------------//
   //  StringInterface  //
  //-------------------//


//---------------------------------------------------------------------------
StringInterface::~StringInterface()
{
}

//---------------------------------------------------------------------------
bool StringInterface::isString() const
{
  return true;
}


    //-----------------//
   //  SizeInterface  //
  //-----------------//


//---------------------------------------------------------------------------
SizeInterface::~SizeInterface()
{
}

    //----------------------//
   //  IterableInterface   //
  //----------------------//


//---------------------------------------------------------------------------
IterableInterface::~IterableInterface()
{
}

//---------------------------------------------------------------------------
bool IterableInterface::isIterable() const
{
  return true;
}


    //----------------------//
   //  IteratorInterface   //
  //----------------------//


//---------------------------------------------------------------------------
IteratorInterface::~IteratorInterface()
{
}

//---------------------------------------------------------------------------
bool IteratorInterface::isIterator() const
{
  return true;
}


    //--------------------------------//
   //  DictionaryIteratorInterface   //
  //--------------------------------//


//---------------------------------------------------------------------------
DictionaryIteratorInterface::~DictionaryIteratorInterface()
{
}

//---------------------------------------------------------------------------
bool DictionaryIteratorInterface::isDictionaryIterator() const
{
  return true;
}


    //------------------//
   //  ArrayInterface  //
  //------------------//


//---------------------------------------------------------------------------
ArrayInterface::~ArrayInterface()
{
}

//---------------------------------------------------------------------------
bool ArrayInterface::isArray() const
{
  return true;
}


    //---------------------//
   //  DynArrayInterface  //
  //---------------------//


//---------------------------------------------------------------------------
DynArrayInterface::~DynArrayInterface()
{
}

//---------------------------------------------------------------------------
bool DynArrayInterface::isDynArray() const
{
  return true;
}


  //-----------------------//
 //  DictionaryInterface  //
//-----------------------//


//---------------------------------------------------------------------------
DictionaryInterface::~DictionaryInterface()
{
}

//---------------------------------------------------------------------------
bool DictionaryInterface::isDictionary() const
{
  return true;
}


//---------------------------------------------------------------------------
Object  DictionaryInterface::
getProperty( Object key ) const
{
  return this->getProperty( key->getString() );
}


//---------------------------------------------------------------------------
Object DictionaryInterface::getProperty( const std::string &key ) const
{
  Object	o;
  if( !getProperty( key, o ) )
    throw std::runtime_error( std::string( "key " ) + key 
                              + " not found in Dictionary" );
  return o;
}

//---------------------------------------------------------------------------
void DictionaryInterface::setProperty( const std::string &key, 
                                       const char *value )
{
  this->setProperty( key, Object::value( std::string( value ) ) );
}


//---------------------------------------------------------------------------
void DictionaryInterface::clearProperties()
{
  Object		i;
  set<string>		keys;
  set<string>::iterator	ik, ek = keys.end();

  for( i=objectIterator(); i->isValid(); i->next() )
    keys.insert( i->key() );
  for( ik=keys.begin(); ik!=ek; ++ik )
    removeProperty( *ik );
}


//---------------------------------------------------------------------------
void DictionaryInterface::copyProperties( Object other )
{
  Object		i;

  for( i=other->objectIterator(); i->isValid(); i->next() )
    setProperty( i->key(), i->currentValue() );
}

  //---------------------//
 //  SyntaxedInterface  //
//---------------------//


//---------------------------------------------------------------------------
SyntaxedInterface::~SyntaxedInterface()
{
}

} // namespace carto


