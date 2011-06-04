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

#ifndef CARTOBASE_OBJECT_OBJECT_H
#define CARTOBASE_OBJECT_OBJECT_H

#include <cartobase/config/cartobase_config.h>
#include <cartobase/type/types.h>
#include <cartobase/type/conversion.h>
#include <cartobase/type/string_conversion.h>
#include <cartobase/smart/rcptr.h>
#include <cartobase/exception/format.h>
#include <stdexcept>
#include <string>
#include <set>
#include <vector>
#include <typeinfo>
#ifdef CARTO_DEBUG
#include <cartobase/config/verbose.h>
#include <iostream>
#endif // #ifdef CARTO_DEBUG
#include <cartobase/object/syntax.h>


#define DECLARE_GENERIC_OBJECT_TYPE( T ) \
template <> \
struct GenericObjectTypeDeclared< T > \
{ \
    static inline void check() {}; \
}; \
template <> \
void DictionaryInterface::setProperty( const std::string &key, \
                                       T const &value ); \
template <> \
bool DictionaryInterface::getProperty( const std::string &key, T &value ) const; \
extern template class TypedObject< T >; \
extern template class ValueObject< T >; \
extern template class ReferenceObject< T >; \
extern template class PointerObject< T >; \
extern template \
T const &GenericObject::value< T >() const; \
extern template \
T &GenericObject::value< T >(); \
extern template \
bool GenericObject::value( T &dest ) const; \
extern template \
void GenericObject::setValue( T const & x ); \
extern template bool DictionaryInterface:: \
getProperty( const std::string &, T &  ) const; \
extern template void DictionaryInterface:: \
setProperty( const std::string &, T const & ); \


namespace carto
{
class GenericObject;
template <typename T> class TypedObject;
class Object;


    //-------------//
   //  Interface  //
  //-------------//

  class Interface
  {
  public:
    virtual ~Interface();
  };


    //-------------------//
   //  ScalarInterface  //
  //-------------------//

  /** All scalar numbers implement the ScalarInterface (all ints, float, 
      double...) */
  class ScalarInterface : public virtual Interface
  {
  public:

    virtual ~ScalarInterface();

    /** Returns false if the stored object doesn't actually implement 
        the ScalarInterface API (needed since all GenericObject inherit 
        this interface whatever they actually contain)
    */
    virtual bool isScalar() const;
    /** Obtain a scalar value, possibly after a conversion.
        This method may throw an exception if the stored object has not a 
        scalar-compatible interface
    */
    virtual double getScalar() const = 0;
    /** The double value will be converted to the actual storage type before 
        it is set in the contained object.
        This method may throw an exception if the stored object has not a 
        scalar-compatible interface
    */
    virtual void setScalar( double ) = 0;
  };


    //-------------------//
   //  StringInterface  //
  //-------------------//

  /// Objects whose value may be represented as a character string
  class StringInterface : public virtual Interface
  {
  public:

    virtual ~StringInterface();

    /** Returns false if the stored object doesn't actually implement 
        the StringInterface API (needed since all GenericObject inherit 
        this interface whatever they actually contain)
    */
    virtual bool isString() const;
    /** Obtain a string value, possibly after a conversion.
        This method may throw an exception if the stored object has not a 
        string-compatible interface
    */
    virtual std::string getString() const = 0;
    /** The string value may be converted to the actual storage type before 
        it is set in the contained object.
        This method may throw an exception if the stored object has not a 
        string-compatible interface
    */
    virtual void setString( const std::string & ) = 0;
  };


    //------------------//
   //  SizeInterface   //
  //------------------//

  /// All container objects inherit the SizeInterface
  class SizeInterface : public virtual Interface
  {
  public:

    virtual ~SizeInterface();

    /** Number of sub-elements.
        This method may throw an exception if the stored object has not a 
        size-compatible interface
    */
    virtual size_t size() const = 0;
  };


    //----------------------//
   //  IterableInterface   //
  //----------------------//

  /// Container objects which can be iterated
  class IterableInterface : public virtual Interface
  {
  public:

    virtual ~IterableInterface();

    /** Returns false if the stored object doesn't actually implement 
        the IterableInterface API (needed since all GenericObject inherit 
        this interface whatever they actually contain)
    */
    virtual bool isIterable() const;
    /// returns an object implementing the IteratorIntrerface
    virtual Object objectIterator() const = 0;
  };


    //----------------------//
   //  IteratorInterface   //
  //----------------------//

  /** An iterator object is a reference to another object.
      They are used to iterate on a container Iterable object (like a 
      pointer in an array). 

      Example:
      \code
      Object iterable = Object::value( vector<int>( 10 ) );
      Object iterator = iterable->objectIterator();
      while( iterator->isValid() )
      {
        Object value = iterator->currentValue();
        cout << value->getScalar() << endl;
      }
      \endcode
   */
  class IteratorInterface : public virtual Interface
  {
  public:

    virtual ~IteratorInterface();

    /** Returns false if the stored object doesn't actually implement 
        the IteratorInterface API (needed since all GenericObject inherit 
        this interface whatever they actually contain)
    */
    virtual bool isIterator() const;
    /** \c true if the iterator points to a valid value, \c false when the 
        end of the iterable container has been reached */
    virtual bool isValid() const = 0;
    /// Access the value of the element pointed to by the iterator
    virtual Object currentValue() const = 0;
    /// Point to the next element of the iterable container
    virtual void next() = 0;
  };


    //--------------------------------//
   //  DictionaryIteratorInterface   //
  //--------------------------------//

  /** Specialized IteratorInterface for dictionaries.
      A dictionary element stores both a key (string) and a value (generic 
      Object) (see DictionaryInterface)
  */
  class DictionaryIteratorInterface : public virtual IteratorInterface
  {
  public:

    virtual ~DictionaryIteratorInterface();

    /** Returns false if the stored object doesn't actually implement 
        the DictionaryIteratorInterface API (needed since all GenericObject 
        inherit this interface whatever they actually contain)
    */
    virtual bool isDictionaryIterator() const;
    /// Access the key of the current dictionary element
    virtual std::string key() const = 0;
  };


    //------------------//
   //  ArrayInterface  //
  //------------------//

  /** ArrayInterface represents any container whose elements can be accessed 
      via an integer index */
  class ArrayInterface : public virtual SizeInterface, 
                         public virtual IterableInterface
  {
  public:

    virtual ~ArrayInterface();

    /** Returns false if the stored object doesn't actually implement 
        the ArrayInterface API (needed since all GenericObject 
        inherit this interface whatever they actually contain)
    */
    virtual bool isArray() const;
    /** Get the element of index \c index. An exception will be thrown if 
        the stored object doesn't actually implement the ArrayInterface. But 
        if it does, access to an element of index greater or equal to 
        size() should not be attempted: depending on the exact storage type 
        and array implementation, it can either throw an exception, or 
        simply crash the program.
     */
    virtual Object getArrayItem( int index ) const = 0;
    virtual void setArrayItem( int, Object ) = 0;
    virtual size_t size() const = 0;
  };


    //---------------------//
   //  DynArrayInterface  //
  //---------------------//

  /// A dynamic array has resize and insertion capabilities (like a STL vector)
  class DynArrayInterface : public virtual ArrayInterface
  {
  public:

    virtual ~DynArrayInterface();

    /** Returns false if the stored object doesn't actually implement 
        the DynArrayInterface API (needed since all GenericObject 
        inherit this interface whatever they actually contain)
    */
    virtual bool isDynArray() const;
    /** like the STL vector::reserve(), memory is reserved but no element is 
        stored */
    virtual void reserveArray( size_t ) = 0;
    /** resize the array. This may need to copy all existing elements, so 
        all iterators may become invalid after this operation
     */
    virtual void resizeArray( size_t ) = 0;
    /** removes an element from the array. All elements after the one removed 
        may be moved, so iterators pointing after it may become invalid */
    virtual void removeArrayItem( int ) = 0;
    /** inserts an element into the array. All elements after this one 
        may be moved, so iterators pointing after it may become invalid */
    virtual void insertArrayItem( int, Object ) = 0;
  };


    //------------------------//
   //  DictionaryInterface   //
  //------------------------//

  /** Interface for dictionary-like objects

  DictionaryInterface is the generic interface for dictionary-like objects.
  This includes wrappers to STL maps with a string key, or the more 
  specialized PropertySet.

  Dictionary items are called "properties" and are stored using a string key.
  */
  class DictionaryInterface : public virtual SizeInterface, 
                              public virtual IterableInterface
  {
  public:

    virtual ~DictionaryInterface();

    /** Returns false if the stored object doesn't actually implement 
        the DictionaryInterface API (needed since all GenericObject 
        inherit this interface whatever they actually contain)
    */
    virtual bool isDictionary() const;
    /** Access the element ok key \c key. \c value is filled with a wrapper to 
        the selected element. If it fails (key not found for instance), an 
        exception is thrown.
    */
    virtual bool getProperty( const std::string & key, 
                              Object & value ) const = 0;
    /** same as the other getProperty() functions except that the value object 
        is returned. An exception is thrown in case of failure. */
    Object getProperty( const std::string & ) const;
    /** same as the other getProperty() functions except that the key is 
        contained in the \c key argument (which must implement the 
        StringInterface). The value object is returned. An exception is 
        thrown in case of failure. */
    Object getProperty( Object key ) const;
    /** same as the other getProperty() functions except that the value object 
        must be of exact type \c T and is copied to the \c value argument. 
        No exception is thrown if the key is not found or the selected element 
        is not of type \c T, but the return value is \c false if it fails. 
        An exception is still thrown if this function is called on an object 
        that doesn't actually implement the DictionaryInterface API.
    */
    template <typename T> bool getProperty( const std::string & key, 
                                            T & value ) const;

    /** Set (insert or replace) the element of key \c key with the \c value 
        object. On some specific dictionary implementations (such as 
        PropertySet with builtin properties, or maps of specific, typed 
        values), an existing property cannot change type. In this case 
        \c value must hold an object of the correct type, otherwise an 
        exception will be thrown.
    */
    virtual void setProperty( const std::string & key, Object value ) = 0;
    /// specific specialization: C strings are stored as std::string objects
    void setProperty( const std::string &, const char * );
    /** same as the other setProperty() functions except that the value to 
        be set is of the arbitrary type \c T and will possibly be stored in 
        an Object. The same type restrictions apply as for the other 
        setProperty() functions */
    template <typename T> void setProperty( const std::string &, const T & );

    /** remove an element. Note that on some dictionary implementations 
        (PropertySet with builtin properties for instance), some properties 
        cannot be removed.
    */
    virtual bool removeProperty( const std::string & ) = 0;

    /// check if an element exists under the key \c key
    virtual bool hasProperty( const std::string & key ) const = 0;
    /// clear the dictionary
    virtual void clearProperties();
    /** copy all properties of the \c source object to \c this object. 
        \c source must implement DictionaryInterface */
    virtual void copyProperties( Object source );
  };


    //----------------------//
   //  SyntaxedInterface   //
  //----------------------//

  /** A Syntaxed object is an object containing an additional character string 
      giving it a kind of type (a syntactic attribute).
      Syntaxed objects (SyntaxedObject) are used to implement graphs (Graph, 
      Vertex, Edge and other classes in the graph library).
  */
  class SyntaxedInterface : public virtual Interface
  {
  public:

    virtual ~SyntaxedInterface();

    virtual std::string getSyntax() const = 0;
    virtual bool hasSyntax() const = 0;
    virtual void setSyntax( const std::string& syntactic ) = 0;
  };


  //-----------------//
 //  GenericObject  //
//-----------------//
  
class CARTOBASE_API GenericObject :
  public virtual RCObject,
  public virtual StringInterface,
  public virtual ScalarInterface,
  public virtual DynArrayInterface,
  public virtual DictionaryInterface,
  public virtual IterableInterface,
  public virtual IteratorInterface,
  public virtual DictionaryIteratorInterface
{
public:
  virtual ~GenericObject();

  /// cloning copy
  virtual Object clone() const = 0;
  /// type() returns the DataTypeCode::name() of the underlying object type
  virtual std::string type() const = 0;  
  
  /** Retreive value in object, const reference.
      This function only works if type T matches the actual value type 
      stored in the object, otherwise it throws an exception */
  template<typename T> const T & value() const;
  /** Retreive value in object, by non-const reference.
      This function only works if type T matches the actual value type 
      stored in the object, otherwise it throws an exception */
  template<typename T> T & value();
  /** Retreive value in object, and store it in a variable.
      This function only works if type T matches the actual value type 
      stored in the object, otherwise it throws an exception */
  template <typename T> bool value( T & ) const;
  /** Store value in object by copying it.
      This function only works if type T matches the actual value type 
      stored in the object, otherwise it throws an exception */
  template<typename T> void setValue( const T &val );
  virtual void setValue( Object val ) = 0;
  
  virtual size_t size() const = 0;

  /** Obtain a specific Interface subclass.
      GenericObject inherits all "standard" interfaces, but may hold an object 
      which implements additional custom interfaces. getInterface() provides 
      access to such additional interfaces.

      Moreover an interface may be implemented either in the stored object 
      itself if it inherits the interface, or in the container GenericObject, 
      so getInterface() may return a pointer to either object.

      In case of failure (interface not implemented), getInterface() returns 
      0 (null pointer).

      As GenericObject inherits all standard interfaces but doesn't 
      necessarily really implement them, there is still an ambiguity for them.
      Up to now, getInterface() returns a non-null pointer, and functions 
      are provided in each standard interface to check if it is actually 
      implemented or not (ScalarInterface::isScalar() for instance). In a 
      future release, this function may return a null pointer.
   */
  template <typename T>
  T *getInterface();
  /// const access to an interface
  template <typename T>
  const T *getInterface() const;

protected:
  friend class Object;

  virtual Interface *_getGenericInterface() = 0;  
  virtual const void *_getAddressOfValue() const = 0;
};


  //----------//
 //  Object  //
//----------//

class Object : public rc_ptr<GenericObject>
{
public:
  inline Object();
  inline Object( const Object & );
  inline Object( GenericObject* data, bool externalowner = false );
  inline ~Object();
  /// uses the reference-counting so the underlying GenericObject is shared
  inline Object & operator = ( const Object & );

  /** factory function: builds an Object by using the default constructor */
  template <typename T>
  static inline Object value();

  /** factory function: builds an Object by copying the value into a 
      ValueObject storage wrapper */
  template <typename T>
  static inline Object value( const T & );

  /** factory function: builds an Object by referencing the value from a 
      ReferenceObject storage wrapper. The original \c value object must not 
      be destroyed while the new object is still in use.
  */
  template <typename T>
  static inline Object reference( T & value );

  /** This method remove "constness" of it argument and call its
      non const equivalent.
  */
  template <typename T>
  static inline Object reference( const T & value );

  /// compares the addresses of the underlying GenericObjects
  bool isSameObject( const Object & ) const;
  /** check if we hold a "none" object.
      At the moment, a "none" object is null: the reference counter holds a 
      null pointer, so you must check it before accessing any GenericObject 
      method via the \c operator->() . In the future, we may define a None type
      which will not be a null pointer, allowing the use of GenericObject 
      methods, which will throw exceptions rather than crash the program.

      Note that a singleton None object is defined as a global variable: 
      carto::none()
  */
  bool isNone() const;
};
 

  //------------------//
 //  TypedObject<T>  //
//------------------//

template <typename T>
class TypedObject : public GenericObject
{
public:
  TypedObject();
  virtual ~TypedObject();
  
  // This function should be pure virtual but it is mandatory to
  // do explicit instanciation of TypedObject and it is not possible
  // with pure virtual classes.
  virtual T &getValue();
  inline const T &getValue() const;
  virtual void setValue( Object val );
  
  // This function should be pure virtual but it is mandatory to
  // do explicit instanciation of TypedObject and it is not possible
  // with pure virtual classes.
  virtual Object clone() const;

  virtual std::string type() const;

  // ScalarInterface methods
  virtual bool isScalar() const;
  virtual double getScalar() const;
  virtual void setScalar( double );

  // StringInterface methods
  virtual bool isString() const;
  virtual std::string getString() const;
  virtual void setString( const std::string & );

  // ArrayInterface methods
  virtual bool isArray() const;
  virtual Object getArrayItem( int index ) const;
  virtual void setArrayItem( int, Object );
  virtual size_t size() const;

  // DynArrayInterface methods
  virtual bool isDynArray() const;
  virtual void reserveArray( size_t );
  virtual void resizeArray( size_t );
  virtual void removeArrayItem( int );
  virtual void insertArrayItem( int, Object );

  // DictionaryInterface methods
  virtual bool getProperty( const std::string &, Object & ) const;
  virtual bool isDictionary() const;
  /** Reimplementation needed because of a bizarre "feature" of the C++ 
      language */
  inline Object getProperty( const std::string & key ) const;
  /** Reimplementation needed because of a bizarre "feature" of the C++ 
      language */
  inline Object getProperty( Object key ) const;

  /** Reimplementation needed because of a bizarre "feature" of the C++ 
      language */
  template <typename U> 
  inline bool getProperty( const std::string & key, 
                           U & value ) const;

  virtual void setProperty( const std::string &, Object );
  /** Reimplementation needed because of a bizarre "feature" of the C++ 
      language */
  inline void setProperty( const std::string & key, const char * value );
  /** Reimplementation needed because of a bizarre "feature" of the C++ 
      language */
  template <typename U> 
  inline void setProperty( const std::string & key, const U & value );

  virtual bool removeProperty( const std::string & );
  virtual bool hasProperty( const std::string & ) const;
  virtual void clearProperties();

  // IterableInterface methods
  virtual bool isIterable() const;
  virtual Object objectIterator() const;

  // IteratorInterface methods
  virtual bool isIterator() const;
  virtual bool isValid() const;
  virtual Object currentValue() const;
  virtual void next();

  // DictionaryIteratorInterface methods
  virtual bool isDictionaryIterator() const;
  virtual std::string key() const;

private:
  Interface *_getGenericInterface();
  const void *_getAddressOfValue() const;
#ifdef CARTO_DEBUG
  static bool _debugInstantiation;
#endif // ifdef CARTO_DEBUG 
};


//-----------------------------------------------------------------------------
/// Class to force a compilation error when a generic object
//  is automatically instanitiated on a non declared type.
template <typename T>
struct GenericObjectTypeDeclared
{
  static inline void check( T *x = NULL ) 
  {
    you_must_use_DECLARE_GENERIC_OBJECT_TYPE( x );
  }
};


//-----------------------------------------------------------------------------
template <typename T>
class TypedObject<const T> : public GenericObject
{
private:
  // Forbid TypedObject on constant type
  inline TypedObject() {};
};



#ifdef CARTO_DEBUG
template <typename T>
class PrintInstantiation
{
public:

  static bool doIt( bool * );
};


template <typename T>
class PrintInstantiation<const T>
{
public:

  static bool doIt( bool * );
};
#endif // ifdef CARTO_DEBUG 




 #if 0
 //--------------------------//
 //  ForbidInheritance<T,U>  //
//--------------------------//
template <typename T, bool B>
class CompileErrorIfFalse
{
  static inline void check() {}
};

template <typename T>
class CompileErrorIfFalse<T,false>
{
  static inline void check() 
  {
    T::cannot_compile_this_code();
  }
};

template <typename T, typename U>
class ForbidInheritance : CompileErrorIfFalse< T, SUPERSUBCLASS( T, U ) >
{
};
#endif


  //------------------//
 //  ValueObject<T>  //
//------------------//

template <typename T>
class ValueObject : public TypedObject<T>
{
public:
  inline ValueObject();
  inline ValueObject( const T & x );
  virtual ~ValueObject();
 
  virtual T &getValue();
  
  virtual Object clone() const;
  
private:
  
  // class _Test : ForbidInheritance< GenericObject, T > {};
  mutable T _value;
};


  //-----------------------//
 //  Useful common types  //
//-----------------------//

/** Common type used for DictionaryInterface implementation

Another DictionaryInterface implementation is commonly used: PropertySet
*/
typedef std::map<std::string, Object> Dictionary;
typedef std::map<int, Object> IntDictionary;
typedef std::map<Object, Object> ObjectDictionary;
/// Common type used for ArrayInterface implementation
typedef std::vector<Object> ObjectVector;


  //----------------------//
 //  ReferenceObject<T>  //
//----------------------//

template <typename T>
class ReferenceObject : public TypedObject<T>
{
public:
  inline ReferenceObject( T &x );
  virtual ~ReferenceObject();
  virtual T &getValue();
  
  virtual Object clone() const;
  
private:

  // class _Test : ForbidInheritance< GenericObject, T > {};
  T &_value;
};


  //--------------------//
 //  PointerObject<T>  //
//--------------------//

template <typename T>
class PointerObject : public TypedObject<T>
{
public:
  inline PointerObject( T &x, bool owner );
  virtual ~PointerObject();
  virtual T &getValue();
  
  virtual Object clone() const;
  
private:

  mutable T *_pvalue;
  bool _owner;
};


  //--------//
 //  none  //
//--------//

/// An empty singleton object (holds a null pointer)
extern Object none();


  //----------------//
 //  object_to<T>  //
//----------------//

//-----------------------------------------------------------------------------
template <typename T>
inline void object_to( Object o, T & r );

//-----------------------------------------------------------------------------
/* This specialization is a workaaround to avoid a bug in gcc 3.4.3 (at least).
   This function needs to be compiled and I really really don't know why. 
   Otherwise it seems not to be called at all (hopefully)
*/
template <typename T>
inline void object_to( Object, const T & );


  //------------------//
 //  instantiations  //
//------------------//

DECLARE_GENERIC_OBJECT_TYPE( int )
DECLARE_GENERIC_OBJECT_TYPE( unsigned )
DECLARE_GENERIC_OBJECT_TYPE( char )
DECLARE_GENERIC_OBJECT_TYPE( signed char )
DECLARE_GENERIC_OBJECT_TYPE( unsigned char )
DECLARE_GENERIC_OBJECT_TYPE( short )
DECLARE_GENERIC_OBJECT_TYPE( unsigned short )
DECLARE_GENERIC_OBJECT_TYPE( long )
DECLARE_GENERIC_OBJECT_TYPE( unsigned long )
DECLARE_GENERIC_OBJECT_TYPE( long long )
DECLARE_GENERIC_OBJECT_TYPE( unsigned long long )
DECLARE_GENERIC_OBJECT_TYPE( float )
DECLARE_GENERIC_OBJECT_TYPE( double )
DECLARE_GENERIC_OBJECT_TYPE( bool )
DECLARE_GENERIC_OBJECT_TYPE( std::string )
DECLARE_GENERIC_OBJECT_TYPE( Object )
DECLARE_GENERIC_OBJECT_TYPE( void * )
template <>
struct GenericObjectTypeDeclared< GenericObject >
{
  static inline void check() {};
};
extern template class TypedObject< GenericObject >;
extern template class ReferenceObject< GenericObject >;
extern template class PointerObject< GenericObject >;

DECLARE_GENERIC_OBJECT_TYPE( std::vector<int> )
DECLARE_GENERIC_OBJECT_TYPE( std::vector<unsigned> )
DECLARE_GENERIC_OBJECT_TYPE( std::vector<char> )
DECLARE_GENERIC_OBJECT_TYPE( std::vector<signed char> )
DECLARE_GENERIC_OBJECT_TYPE( std::vector<unsigned char> )
DECLARE_GENERIC_OBJECT_TYPE( std::vector<short> )
DECLARE_GENERIC_OBJECT_TYPE( std::vector<unsigned short> )
DECLARE_GENERIC_OBJECT_TYPE( std::vector<long> )
DECLARE_GENERIC_OBJECT_TYPE( std::vector<unsigned long> )
DECLARE_GENERIC_OBJECT_TYPE( std::vector<long long> )
DECLARE_GENERIC_OBJECT_TYPE( std::vector<unsigned long long> )
DECLARE_GENERIC_OBJECT_TYPE( std::vector<float> )
DECLARE_GENERIC_OBJECT_TYPE( std::vector<double> )
DECLARE_GENERIC_OBJECT_TYPE( std::vector<std::string> )
DECLARE_GENERIC_OBJECT_TYPE( std::vector<Object> )
DECLARE_GENERIC_OBJECT_TYPE( std::vector<std::string> * )
DECLARE_GENERIC_OBJECT_TYPE( std::vector<std::vector<float> > )
DECLARE_GENERIC_OBJECT_TYPE( std::vector<std::vector<char> > )
DECLARE_GENERIC_OBJECT_TYPE( std::vector<std::set<unsigned> > )

DECLARE_GENERIC_OBJECT_TYPE( std::set<int> )
DECLARE_GENERIC_OBJECT_TYPE( std::set<unsigned> )
DECLARE_GENERIC_OBJECT_TYPE( std::set<char> )
DECLARE_GENERIC_OBJECT_TYPE( std::set<signed char> )
DECLARE_GENERIC_OBJECT_TYPE( std::set<unsigned char> )
DECLARE_GENERIC_OBJECT_TYPE( std::set<short> )
DECLARE_GENERIC_OBJECT_TYPE( std::set<unsigned short> )
DECLARE_GENERIC_OBJECT_TYPE( std::set<long> )
DECLARE_GENERIC_OBJECT_TYPE( std::set<unsigned long> )
DECLARE_GENERIC_OBJECT_TYPE( std::set<long long> )
DECLARE_GENERIC_OBJECT_TYPE( std::set<unsigned long long> )
DECLARE_GENERIC_OBJECT_TYPE( std::set<float> )
DECLARE_GENERIC_OBJECT_TYPE( std::set<double> )
DECLARE_GENERIC_OBJECT_TYPE( std::set<std::string> )
DECLARE_GENERIC_OBJECT_TYPE( std::set<Object> )

#define _TMP_ std::map< std::string, int >
DECLARE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_
#define _TMP_ std::map< std::string, unsigned >
DECLARE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_
#define _TMP_ std::map< std::string, char >
DECLARE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_
#define _TMP_ std::map< std::string, signed char >
DECLARE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_
#define _TMP_ std::map< std::string, unsigned char >
DECLARE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_
#define _TMP_ std::map< std::string, short >
DECLARE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_
#define _TMP_ std::map< std::string, unsigned short >
DECLARE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_
#define _TMP_ std::map< std::string, long >
DECLARE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_
#define _TMP_ std::map< std::string, unsigned long >
DECLARE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_
#define _TMP_ std::map< std::string, long long >
DECLARE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_
#define _TMP_ std::map< std::string, unsigned long long >
DECLARE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_
#define _TMP_ std::map< std::string, float >
DECLARE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_
#define _TMP_ std::map< std::string, double >
DECLARE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_
#define _TMP_ std::map< std::string, bool >
DECLARE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_
#define _TMP_ std::map< std::string, std::string >
DECLARE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_
#define _TMP_ std::map< std::string, Object >
DECLARE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_
#define TMP std::map< int, Object >
DECLARE_GENERIC_OBJECT_TYPE( TMP )
#undef TMP
#define TMP std::map< float, Object >
DECLARE_GENERIC_OBJECT_TYPE( TMP )
#undef TMP
#define TMP std::map< Object, Object >
DECLARE_GENERIC_OBJECT_TYPE( TMP )
#undef TMP

#define _TMP_ std::map< std::string, std::vector<int> >
DECLARE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_
#define _TMP_ std::map< std::string, std::vector<unsigned> >
DECLARE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_
#define _TMP_ std::map< std::string, std::vector<char> >
DECLARE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_
#define _TMP_ std::map< std::string, std::vector<signed char> >
DECLARE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_
#define _TMP_ std::map< std::string, std::vector<unsigned char> >
DECLARE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_
#define _TMP_ std::map< std::string, std::vector<short> >
DECLARE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_
#define _TMP_ std::map< std::string, std::vector<unsigned short> >
DECLARE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_
#define _TMP_ std::map< std::string, std::vector<long> >
DECLARE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_
#define _TMP_ std::map< std::string, std::vector<unsigned long>  >
DECLARE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_
#define _TMP_ std::map< std::string, std::vector<long long> >
DECLARE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_
#define _TMP_ std::map< std::string, std::vector<unsigned long long> >
DECLARE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_
#define _TMP_ std::map< std::string, std::vector<float> >
DECLARE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_
#define _TMP_ std::map< std::string, std::vector<double> >
DECLARE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_
#define _TMP_ std::map< std::string, std::vector<std::string> >
DECLARE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_
#define _TMP_ std::map< std::string, std::vector<Object> >
DECLARE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_

DECLARE_GENERIC_OBJECT_TYPE( rc_ptr< std::vector<int> > )
DECLARE_GENERIC_OBJECT_TYPE( rc_ptr< std::vector<unsigned> > )
DECLARE_GENERIC_OBJECT_TYPE( rc_ptr< std::vector<char> > )
DECLARE_GENERIC_OBJECT_TYPE( rc_ptr< std::vector<signed char> > )
DECLARE_GENERIC_OBJECT_TYPE( rc_ptr< std::vector<unsigned char> > )
DECLARE_GENERIC_OBJECT_TYPE( rc_ptr< std::vector<short> > )
DECLARE_GENERIC_OBJECT_TYPE( rc_ptr< std::vector<unsigned short> > )
DECLARE_GENERIC_OBJECT_TYPE( rc_ptr< std::vector<long> > )
DECLARE_GENERIC_OBJECT_TYPE( rc_ptr< std::vector<unsigned long> > )
DECLARE_GENERIC_OBJECT_TYPE( rc_ptr< std::vector<long long> > )
DECLARE_GENERIC_OBJECT_TYPE( rc_ptr< std::vector<unsigned long long> > )
DECLARE_GENERIC_OBJECT_TYPE( rc_ptr< std::vector<float> > )
DECLARE_GENERIC_OBJECT_TYPE( rc_ptr< std::vector<double> > )
DECLARE_GENERIC_OBJECT_TYPE( rc_ptr< std::vector<Object> > )
DECLARE_GENERIC_OBJECT_TYPE( rc_ptr< std::vector<std::string> > )

#define _TMP_  rc_ptr< std::map< std::string, int > >
DECLARE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_
#define _TMP_  rc_ptr< std::map< std::string, unsigned > >
DECLARE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_
#define _TMP_  rc_ptr< std::map< std::string, char > >
DECLARE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_
#define _TMP_  rc_ptr< std::map< std::string, signed char > >
DECLARE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_
#define _TMP_  rc_ptr< std::map< std::string, unsigned char > >
DECLARE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_
#define _TMP_  rc_ptr< std::map< std::string, short > >
DECLARE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_
#define _TMP_  rc_ptr< std::map< std::string, unsigned short > >
DECLARE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_
#define _TMP_  rc_ptr< std::map< std::string, long > >
DECLARE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_
#define _TMP_  rc_ptr< std::map< std::string, unsigned long > >
DECLARE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_
#define _TMP_  rc_ptr< std::map< std::string, long long > >
DECLARE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_
#define _TMP_  rc_ptr< std::map< std::string, unsigned long long > >
DECLARE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_
#define _TMP_  rc_ptr< std::map< std::string, float > >
DECLARE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_
#define _TMP_  rc_ptr< std::map< std::string, double > >
DECLARE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_
#define _TMP_  rc_ptr< std::map< std::string, bool > >
DECLARE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_
#define _TMP_  rc_ptr< std::map< std::string, std::string > >
DECLARE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_
#define _TMP_  rc_ptr< std::map< std::string, Object > >
DECLARE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_
#define _TMP_ rc_ptr< std::map< int, Object > >
DECLARE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_
#define _TMP_ rc_ptr< std::map< float, Object > >
DECLARE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_
#define TMP rc_ptr< std::map< Object, Object > >
DECLARE_GENERIC_OBJECT_TYPE( TMP )
#undef TMP

#define _TMP_  rc_ptr< std::map< std::string, std::vector<int> > >
DECLARE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_
#define _TMP_  rc_ptr< std::map< std::string, std::vector<unsigned> > >
DECLARE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_
#define _TMP_  rc_ptr< std::map< std::string, std::vector<char> > >
DECLARE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_
#define _TMP_  rc_ptr< std::map< std::string, std::vector<signed char> > >
DECLARE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_
#define _TMP_  rc_ptr< std::map< std::string, std::vector<unsigned char> > >
DECLARE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_
#define _TMP_  rc_ptr< std::map< std::string, std::vector<short> > >
DECLARE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_
#define _TMP_  rc_ptr< std::map< std::string, std::vector<unsigned short> > >
DECLARE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_
#define _TMP_  rc_ptr< std::map< std::string, std::vector<long> > >
DECLARE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_
#define _TMP_  rc_ptr< std::map< std::string, std::vector<unsigned long>  > >
DECLARE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_
#define _TMP_  rc_ptr< std::map< std::string, std::vector<long long> > >
DECLARE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_
#define _TMP_  rc_ptr< std::map< std::string, std::vector<unsigned long long> > >
DECLARE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_
#define _TMP_  rc_ptr< std::map< std::string, std::vector<float> > >
DECLARE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_
#define _TMP_  rc_ptr< std::map< std::string, std::vector<double> > >
DECLARE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_
#define _TMP_  rc_ptr< std::map< std::string, std::vector<std::string> > >
DECLARE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_
#define _TMP_  rc_ptr< std::map< std::string, std::vector<Object> > >
DECLARE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_

DECLARE_GENERIC_OBJECT_TYPE( Syntax )
DECLARE_GENERIC_OBJECT_TYPE( Semantic )
DECLARE_GENERIC_OBJECT_TYPE( SyntaxSet )
DECLARE_GENERIC_OBJECT_TYPE( rc_ptr<SyntaxSet> )

template <typename T>
inline void object_to( Object o, T & r );
template <>
void object_to( Object o, bool &r );
template <>
void object_to( Object o, unsigned char &r );
template <>
void object_to( Object o, signed char &r );
template <>
inline void object_to( Object o, char &r );
template <>
void object_to( Object o, unsigned short &r );
template <>
void object_to( Object o, short &r );
template <>
void object_to( Object o, unsigned &r );
template <>
void object_to( Object o, int &r );
template <>
void object_to( Object o, float &r );
template <>
inline void object_to( Object o, double &r );
template <>
void object_to( Object o, std::string &r );
template <>
inline void object_to( Object o, Object & r );



  //----------------//
 //  object_to<T>  //
//----------------//

//-----------------------------------------------------------------------------
template <typename T>
inline void object_to( Object o, T & r )
{
  r = o->GenericObject::value<T>();
}

//-----------------------------------------------------------------------------
/* This specialization is a workaaround to avoid a bug in gcc 3.4.3 (at least).
   This function needs to be compiled and I really really don't know why. 
   Otherwise it seems not to be called at all (hopefully)
*/
template <typename T>
inline void object_to( Object, const T & )
{
  throw std::runtime_error( std::string( "cannot convert object to const " ) + 
                            DataTypeCode<T>::name() + " (this is probably a " 
                            "bug)" );
}

//-----------------------------------------------------------------------------
template <>
inline void object_to( Object o, Object & r )
{
  r = o;
}


//-----------------------------------------------------------------------------
template <>
inline void object_to( Object o, char &r )
{
#ifdef __CHAR_UNSIGNED__
  object_to<unsigned char>( o,  reinterpret_cast<unsigned char &>( r ) );
#else
  object_to<signed char>( o, reinterpret_cast<signed char &>( r ) );
#endif
}




//-----------------------------------------------------------------------------
template <>
inline void object_to( Object o, double &r )
{
  r = o->getScalar();
}


  //------------------------//
 //  DictionaryInterface   //
//------------------------//


//---------------------------------------------------------------------------
template <typename T>
void DictionaryInterface::setProperty( const std::string &, 
                                       const T &value )
{
  you_must_use_DECLARE_GENERIC_OBJECT_TYPE( value );
}


//---------------------------------------------------------------------------
template <typename T> 
bool DictionaryInterface::getProperty( const std::string &, T &value ) const
{
  return you_must_use_DECLARE_GENERIC_OBJECT_TYPE( value );
}


  //-----------------//
 //  GenericObject  //
//-----------------//

//-----------------------------------------------------------------------------
template <class T>
inline T *GenericObject::getInterface()
{
  return dynamic_cast<T *>( _getGenericInterface() );
}

//-----------------------------------------------------------------------------
template <class T>
inline const T *GenericObject::getInterface() const
{
  /* using const_cast is ugly, OK, right.
     But it avoids redefining a const version of _getGenericInterface() with 
     exactly the same contents. I'm lazy today.
  */
  return dynamic_cast<T *>( const_cast<GenericObject *>( this )
                            ->_getGenericInterface() );
}


  //----------//
 //  Object  //
//----------//


//-----------------------------------------------------------------------------
inline Object::Object() : rc_ptr<GenericObject>()
{
}

//-----------------------------------------------------------------------------
inline Object::Object( const Object &x ) : rc_ptr<GenericObject>( x )
{
}

//-----------------------------------------------------------------------------
inline Object::Object( GenericObject *x, bool externalowner )
  : rc_ptr<GenericObject>( x, externalowner )
{
}

//-----------------------------------------------------------------------------
inline Object::~Object()
{
}


//-----------------------------------------------------------------------------
inline Object & Object::operator = ( const Object &other )
{
  if ( this != &other ) {
    rc_ptr<GenericObject>::operator =
      ( static_cast< const rc_ptr<GenericObject> & >( other ) );
  }
  return *this;
}


#if 0
//-----------------------------------------------------------------------------
template <typename T, bool>
class ObjectValueTrait
{
public:
  static inline Object value()
  {
    return Object( static_cast<GenericObject *>( new ValueObject<T>() ) );
  }
  static inline Object value( const T &v )
  {
    return Object( static_cast<GenericObject *>( new ValueObject<T>( v ) ) );
  }
};

//-----------------------------------------------------------------------------
template <typename T>
class ObjectValueTrait<T,true>
{
public:
  static inline Object value()
  {
    return Object( (GenericObject *) new T );
  }
  static inline Object value( const T &v )
  {
    return v.clone();
  }
};
#endif

//-----------------------------------------------------------------------------
template <typename T>
inline Object Object::value()
{
  return Object( static_cast<GenericObject *>( new ValueObject<T>() ) );
  // return ObjectValueTrait< T, SUPERSUBCLASS(GenericObject,T) >::value();
}


//-----------------------------------------------------------------------------
template <typename T>
inline Object Object::value( const T &v )
{
  return Object( static_cast<GenericObject *>( new ValueObject<T>( v ) ) );
  // return ObjectValueTrait< T, SUPERSUBCLASS(GenericObject,T) >::value( v );
}


//-----------------------------------------------------------------------------
template <>
inline Object Object::value( const Object &value )
{
  return value;
}


#if 0
//-----------------------------------------------------------------------------
template <typename T, bool>
class ObjectReferenceTrait
{
public:
  static inline Object reference( T &v )
  {
    return Object( static_cast< GenericObject *>( new ReferenceObject<T>( v ) ) );
  }
};

//-----------------------------------------------------------------------------
template <typename T>
class ObjectReferenceTrait<T,true>
{
public:
  static inline Object reference( T &v )
  {
    return Object( &v );
  }
};
#endif

//-----------------------------------------------------------------------------
template <typename T>
inline Object Object::reference( T &value )
{
  return Object( static_cast< GenericObject *>( new ReferenceObject<T>( value ) ) );
  // return ObjectReferenceTrait< T, SUPERSUBCLASS( GenericObject, T ) >::reference( value );
}


//-----------------------------------------------------------------------------
template <typename T>
inline Object Object::reference( const T &value )
{
  return Object::reference( const_cast< T & >( value ) );
}


//-----------------------------------------------------------------------------
inline bool Object::isNone() const
{
  // may be replaced by a comparison to the global none object
  return get() == 0;
}


  //------------------//
 //  TypedObject<T>  //
//------------------//


//-----------------------------------------------------------------------------
template <typename T>
inline Object TypedObject<T>::getProperty( const std::string & key ) const
{ return DictionaryInterface::getProperty( key ); }


//-----------------------------------------------------------------------------
template <typename T>
inline Object TypedObject<T>::getProperty( Object key ) const
{ return DictionaryInterface::getProperty( key ); }

//-----------------------------------------------------------------------------
template <typename T>
template <typename U> 
inline bool TypedObject<T>::getProperty( const std::string & key, 
                                         U & value ) const
{ return DictionaryInterface::getProperty( key, value ); }

//-----------------------------------------------------------------------------
template <typename T>
inline void TypedObject<T>::setProperty( const std::string & key, 
                                         const char * value )
{ DictionaryInterface::setProperty( key, value ); }

//-----------------------------------------------------------------------------
template <typename T>
template <typename U> 
inline void TypedObject<T>::setProperty( const std::string & key, 
                                         const U & value )
{ DictionaryInterface::setProperty( key, value ); }



  //--------------------------------//
 //  DataTypeCode<ValueObject<T>>  //
//--------------------------------//

#ifndef DOXYGEN_HIDE_INTERNAL_CLASSES

template<typename T> class DataTypeCode< ValueObject<T> >
{
 public:
  static inline std::string objectType()
  { return "ValueObject"; }
  static inline std::string dataType()
  { return DataTypeCode<T>::name(); }
  static inline std::string name() 
  { 
    return std::string("ValueObject of ") + DataTypeCode< T >::name();
  }
};

#endif


  //------------------//
 //  ValueObject<T>  //
//------------------//

//-----------------------------------------------------------------------------
template <typename T>
inline ValueObject<T>::ValueObject()
{
}



//-----------------------------------------------------------------------------
template <typename T>
inline ValueObject<T>::ValueObject( const T &x )
{
  _value = x;
}


  //----------------------//
 //  ReferenceObject<T>  //
//----------------------//

//-----------------------------------------------------------------------------
template <typename T>
inline ReferenceObject<T>::ReferenceObject( T &x )
                          : _value( x )
{
}

  //--------------------//
 //  PointerObject<T>  //
//--------------------//

//-----------------------------------------------------------------------------
template <typename T>
inline PointerObject<T>::PointerObject( T &x, bool owner )
                          : _pvalue( &x ), _owner( owner )
{
}


  //------------------------//
 //  DataTypeCode<Object>  //
//------------------------//

//-----------------------------------------------------------------------------
template<> inline std::string DataTypeCode<Object>::dataType()
{ 
  return "object";
}


//-----------------------------------------------------------------------------
template<> inline std::string DataTypeCode<Object>::objectType()
{ 
  return "object";
}


//-----------------------------------------------------------------------------
template<> inline std::string DataTypeCode<Object>::name()
{ 
  return "object";
}


  //-------------------------------//
 //  DataTypeCode<GenericObject>  //
//-------------------------------//

//-----------------------------------------------------------------------------
template<> inline std::string DataTypeCode<GenericObject>::dataType()
{ 
  return "any";
}


//-----------------------------------------------------------------------------
template<> inline std::string DataTypeCode<GenericObject>::objectType()
{ 
  return "genericobject";
}


//-----------------------------------------------------------------------------
template<> inline std::string DataTypeCode<GenericObject>::name()
{ 
  return "genericobject";
}



  //------------------------------------//
 //  DataTypeCode<ReferenceObject<T>>  //
//------------------------------------//

#ifndef DOXYGEN_HIDE_INTERNAL_CLASSES

template<typename T> class DataTypeCode< ReferenceObject<T> >
{
 public:
  static inline std::string objectType()
  { return "ReferenceObject"; }
  static inline std::string dataType()
  { return DataTypeCode<T>::name(); }
  static inline std::string name() 
  { 
    return std::string("ReferenceObject of ") + DataTypeCode< T >::name();
  }
};

#endif


//-----------------------------------------------------------------------------


} // namespace carto


#endif
