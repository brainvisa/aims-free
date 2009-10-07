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


#ifndef CARTOBASE_SMART_RCPTR_H
#define CARTOBASE_SMART_RCPTR_H

#include <cartobase/config/cartobase_config.h>
#include <memory>
#include <iostream>

// uncomment this line to get debugging information
// #define DEBUG_REF

// comment this line to disable thread-safe code using ZooLib atomic operations
#define CARTO_THREADED_RCPTR

#ifdef CARTO_THREADED_RCPTR
#ifndef CARTO_NO_BOOST
#include <boost/version.hpp>
#if BOOST_VERSION >= 103300 && \
  ( !defined( linux ) || ( defined( __GNUC__ ) && __GNUC__ >= 4 ) )
#include <boost/detail/atomic_count.hpp>
#ifndef BOOST_AC_USE_PTHREADS
#define CARTO_RCPTR_USE_BOOST
#endif
#endif // BOOST_VERSION
#endif // CARTO_NO_BOOST

#ifndef CARTO_RCPTR_USE_BOOST
// try using ZooLib-based implementation
#include <cartobase/zoolib/atomiccount.h>
#endif

#if !defined( CARTO_RCPTR_USE_BOOST ) && !defined( CARTO_RCPTR_USE_ZOOLIB )
#undef CARTO_THREADED_RCPTR
#endif

#endif // CARTO_THREADED_RCPTR

namespace carto {

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template <class T>
class const_ref;
template <class T>
class ref;


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#ifndef DOXYGEN_HIDE_INTERNAL_CLASSES
/*!
  Contains the default constructor and destructor for \c const_ref<T> (see
  \c RCObject and \c DefaultRefConstruction.
*/
class DefaultRefConstruction
{
  template <class T>  friend class ref;
  template <class T>  friend class const_ref;

  template <class T>
  static void destroy( const_ref<T> * );
  template <class T>
  static void construct( const_ref<T> *, const T *, bool externalowner );
  template <class T>
  static const T *release( const_ref<T> * );
};

#endif // DOXYGEN_HIDE_INTERNAL_CLASSES

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class RCObject 
#ifndef DOXYGEN_HIDE_INTERNAL_CLASSES
: public virtual DefaultRefConstruction
#endif // DOXYGEN_HIDE_INTERNAL_CLASSES
{
  template <class T>  friend class ref;
  template <class T>  friend class const_ref;
  template <class T>  friend class shared_ptr;
  friend class rc_ptr_trick;

  template <class T>
  static void destroy( const_ref<T> * );
  template <class T>
  static void construct( const_ref<T> *, const T *, bool externalowner );
  template <class T>
  static const T *release( const_ref<T> * );

public:
#ifdef CARTO_THREADED_RCPTR
  typedef boost::detail::atomic_count RefCounterType;
#else
  typedef int RefCounterType;
#endif

private:
  mutable RefCounterType _refCounter;

public:
  
  inline RCObject() : _refCounter( 0 ) {}

  inline RCObject( const RCObject & )
    : _refCounter( 0 ) {}

  inline RCObject & operator = ( const RCObject & ) { return *this; }

#ifdef CARTO_DEBUG
  inline virtual ~RCObject()
  {
    int refCounter = _refCounter;
    if( refCounter > 0 )
      std::cerr << "RCObject destructor called while " << refCounter 
                << " references are still alive on object " << this 
                << std::endl;
  }
#endif
};


class rc_ptr_trick;


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#ifndef DOXYGEN_HIDE_INTERNAL_CLASSES
/*! The following class is used to get the appropriate const_ref<T> 
    constructor and destructor at compile time. A call to 
    RefConstruction<T>::construct() (resp. RefConstruction<T>::destroy())
    calls RCObject::construct<T>() (resp. RCObject::destroy<T>()) if T
    inherits from RCObject, in other cases, it calls
    DefaultRefConstruction::construct<T>() (resp. 
    DefaultRefConstruction::destroy<T>()).
*/ 
template <class T>
class RefConstruction : T, virtual DefaultRefConstruction
{
  template <class R>  friend class ref;
  template <class R>  friend class const_ref;

  /*! The following constructor is never called therefore it is not defined.
      However, its declaration must be present to avoid compilation error if
     class T has no default constructor.
  */
  RefConstruction();
};

/*!
 The following class has been build because of a problem in gcc 2.95.2 (I
 don't know if it is a bug) which forbid to access the private members of
 a const_ref<R> from a member of const_ref<T>. It is not possible to use
 a template friend declaration in this case because it leads to an internal
 compiler error.
*/
template <class T>
class RefData
{
  template <class R> friend class const_ref;
  template <class R> friend class ref;
  friend class DefaultRefConstruction;
  friend class RCObject;
  friend class rc_ptr_trick;

  RCObject::RefCounterType *_pcount;
  T *_ref;

public:

  inline int count() const
  {
    return ( _pcount ? *_pcount : 0 );
  }
};
#endif // DOXYGEN_HIDE_INTERNAL_CLASSES


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
/*! \brief Constant Ref to an object of class T.

    The purpose of this class is to provide a way to reference dynamically
    allocated objects without worrying about their destruction. To avoid
    confusion with build-in C++ references we call a \e Ref an
    object of class \a const_ref or \a ref.

    A Ref act as a pointer on a dynamicaly allocated object (with \c new).
    The main difference between a \e Ref an a pointer is the link that exists
    between the \e Ref lifetime and the pointed object lifetime. An object
    pointed to by a \e Ref is destroyed when the last \e Ref pointed to it is
    destroyed. Therefore a \e Ref contains two things: a pointer to an
    object and a reference counter. Each time a \e Ref is created,
    the counter is incremented. When a \e Ref is destroyed, the counter
    is decremented and if the counter is null, the object is destroyed
    (with \c delete).

    A \e Ref can be used to reference any object created by \c new. Therefore it can be used with
    existing classes.

    \warning It is the responssability to the programmer to avoid cyclic references or memory leak can occur.
    For example, in the following program, the objects of class \c A and \c B are never destroyed.
\code
#include <iostream>
#include <rcptr.h>

using namespace carto;

class A;
class B;

class A {
public:

  A();
  ~A();

  ref<B> rb;
};

class B
{
public:

  B();
  ~B();

  ref<A> ra;
};

A::A()  { cerr << "Construction of A" << endl; }
A::~A() { cerr << "Destruction of A" << endl; }

B::B() { cerr << "Construction of B" << endl; }
B::~B() { cerr << "Destruction of B" << endl; }

int main()
{
  ref<A> ra = new A;
  ref<B> rb = new B;

  ra->rb = rb;
  rb->ra = ra;
}
\endcode

\warning In the general case the pointer returned by \c new can be used to build only one \e Ref. If it is used to build several \e Ref, the object will be destroyed several times and the program will crash. See \c RCObject to avoid this problem.
*/
template <class T>
class const_ref 
#ifndef DOXYGEN_HIDE_INTERNAL_CLASSES
: public RefData<T>
#endif // DOXYGEN_HIDE_INTERNAL_CLASSES
{

#ifndef DOXYGEN_HIDE_INTERNAL_CLASSES
  friend class DefaultRefConstruction;
  friend class RCObject;
#endif // DOXYGEN_HIDE_INTERNAL_CLASSES
  template <class R> friend class ref;

public:

  typedef T referenced_type;
  inline const_ref() { this->_pcount = 0; this->_ref = 0; }

  inline explicit const_ref( const T *pObject )
  {
    RefConstruction<T>::construct( this, pObject, false );
  }

  inline explicit const_ref( const T *pObject, bool externalowner )
  {
    RefConstruction<T>::construct( this, pObject, externalowner );
  }

  inline const_ref( const ref<T> &other ) {
#ifdef DEBUG_REF
    std::cout << "New ref from other ref (" << other._ref << ")" << std::endl;
#endif
    if( other._pcount && *other._pcount >= 0 )
      ++(*other._pcount);
    this->_pcount = other._pcount;
    this->_ref = other._ref;
  }

  inline const_ref( const const_ref<T> &other ) {
#ifdef DEBUG_REF
    std::cout << "New ref from other ref (" << other._ref << ")" << std::endl;
#endif
    if( other._pcount && *other._pcount >= 0 )
      ++(*other._pcount);
    this->_pcount = other._pcount;
    this->_ref = other._ref;
  }

  template <class R>
  inline const_ref( const ref<R> &o ){
    const RefData<R> &other = o;
#ifdef DEBUG_REF
    std::cout << "New ref from other ref with static cast (" << other._ref
      << ")" << std::endl;
#endif
    if( other._pcount && *other._pcount >= 0 )
      ++(*other._pcount);
    this->_pcount = other._pcount;
    this->_ref = static_cast<T *>( other._ref );
  }

  template <class R>
  inline const_ref( const const_ref<R> &other ){
#ifdef DEBUG_REF
    std::cout << "New ref from other ref with static cast (" << other._ref << ")" << std::endl;
#endif
    if( other._pcount && *other._pcount >= 0 )
      ++(*other._pcount);
    this->_pcount = other._pcount;
    this->_ref = static_cast<T *>( other._ref );
  }

  inline ~const_ref() {  RefConstruction<T>::destroy( this ); }
  
  // const_ref<T> &operator =( const T *pObject );
  const_ref<T> &operator =( const ref<T> &other );
  const_ref<T> &operator =( const const_ref<T> &other );

  inline bool isNull() const { return this->_pcount == NULL; }
  inline bool operator ==( const ref<T> &other ) const { return this->_ref == other._ref; }
  inline bool operator ==( const T *pointer ) const { return this->_ref == pointer; }
  inline bool operator ==( const const_ref<T> &other ) const { return this->_ref == other._ref; }
  inline bool operator !=( const ref<T> &other ) const { return this->_ref != other._ref; }
  inline bool operator !=( const const_ref<T> &other ) const { return this->_ref != other._ref; }
  inline bool operator !=( const T *pointer ) const { return this->_ref != pointer; }

  inline const T *operator ->() const {
    return static_cast<const T *>( this->_ref );
  }

  inline const T &operator *() const {
    return *static_cast<const T *>( this->_ref );
  }

  inline const T *pointer() const {
    return static_cast<const T *>( this->_ref );
  }

  inline int refCount() const
  {
    return this->_pcount ? *this->_pcount : 0;
  }
};


///////////////////////////////////////////////////////////////////////////////
  /*
template <class T>
const_ref<T> &const_ref<T>::operator =( const T *pObject ) {
#ifdef DEBUG_REF
    std::cout << "ref (" << this->_ref << ") = " << pObject << std::endl;
#endif
  if ( this->_ref != pObject ) {
    RefConstruction<T>::destroy( this );
    RefConstruction<T>::construct( this, pObject, false );
  }
  return *this;
}
  */

///////////////////////////////////////////////////////////////////////////////
template <class T>
const_ref<T> &const_ref<T>::operator =( const ref<T> &other ) {
#ifdef DEBUG_REF
    std::cout << "ref (" << this->_ref << ") = ref (" << other._ref << ")"
      << std::endl;
#endif
  if ( this->_ref != other._ref )
    {
      if( other._pcount && *other._pcount >= 0 )
        ++(*other._pcount);
      RefConstruction<T>::destroy( this );
      this->_pcount = other._pcount;
      this->_ref = other._ref;
    }
  return *this;
}

///////////////////////////////////////////////////////////////////////////////
template <class T>
const_ref<T> &const_ref<T>::operator =( const const_ref<T> &other ) {
#ifdef DEBUG_REF
    std::cout << "ref (" << this->_ref << ") = ref (" << other._ref << ")"
      << std::endl;
#endif
  if ( this != &other && this->_ref != other._ref )
    {
      if( other._pcount && *other._pcount >= 0 )
        ++(*other._pcount);
      RefConstruction<T>::destroy( this );
      this->_pcount = other._pcount;
      this->_ref = other._ref;
    }
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//! A \a ref is a \a const_ref which can reference non constant objects.
template <class T>
class ref : public const_ref<T>
{
  template <class R> friend class const_ref;
  
public:
  
  typedef T referenced_type;

  inline ref() : const_ref<T>() {
#ifdef DEBUG_REF
    std::cout << "New empty ref" << std::endl;
#endif
  }

  inline explicit ref( T *pObject ) : const_ref<T>( pObject ) {}
  inline explicit ref( T *pObject, bool externalowner ) 
    : const_ref<T>( pObject, externalowner ) {}

  inline ref( const ref<T> &other ) : const_ref<T>( other ) {}
  
  template <class R>
  inline ref( const ref<R> &other ) : const_ref<T>( other ) {}

  inline ~ref() {}

  inline T *release() {
    return const_cast<T *>( RefConstruction<T>::release( this ) );
  }

  inline ref<T> &operator =( const ref<T> &other ) {
    this->const_ref<T>::operator =( other );
    return *this;
  }

  inline T *operator ->() const {
    return static_cast<T *>( this->_ref );
  }

  inline T &operator *() const {
    return *static_cast<T *>( this->_ref );
  }

  inline T *pointer() const {
    return static_cast<T *>( this->_ref );
  }

};



///////////////////////////////////////////////////////////////////////////////
#ifndef DOXYGEN_HIDE_INTERNAL_CLASSES
template <class T>
void DefaultRefConstruction::construct( const_ref<T> *r, const T *pObject, 
                                        bool externalowner )
{
#ifdef DEBUG_REF
    std::cout << "New ref (" << pObject << ")" << std::endl;
#endif
  if ( pObject && !externalowner )
    r->_pcount = new RCObject::RefCounterType( 1 );
  else r->_pcount = NULL;
  r->_ref = const_cast<T *>( pObject );
}

///////////////////////////////////////////////////////////////////////////////
template <class T>
void DefaultRefConstruction::destroy( const_ref<T> *r ) {
#ifdef DEBUG_REF
  std::cout << "Deleting ref (" << r->_ref << ")" << std::endl;
#endif
  if ( r->_pcount && --(*r->_pcount) == 0 )
    {
#ifdef DEBUG_REF
      std::cout << "Deleting ref object (" << r->_ref << ")" << std::endl;
#endif
      delete r->_pcount;
      delete static_cast<T *>( r->_ref );
    }
}

///////////////////////////////////////////////////////////////////////////////
template <class T>
const T *DefaultRefConstruction::release( const_ref<T> *r ) {
#ifdef DEBUG_REF
  std::cout << "Releasing ref (" << r->_ref << ")" << std::endl;
#endif
  const T * const result = r->_ref;
  if ( r->_pcount && --(*r->_pcount) == 0 )
    {
      delete r->_pcount;
      r->_pcount = 0;
    }
  r->_ref = NULL;
  return result;
}
#endif // DOXYGEN_HIDE_INTERNAL_CLASSES

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////




///////////////////////////////////////////////////////////////////////////////
template <class T>
void RCObject::construct( const_ref<T> *r, const T * pObject, 
                          bool externalowner )
{
#ifdef DEBUG_REF
  std::cout << "New RCObject (" << pObject << ")" << std::endl;
#endif
  if( pObject )
    {
      int x = pObject->_refCounter;
      if( externalowner && x == 0 )
      {
        --pObject->_refCounter;
        // r->_pcount = NULL;
      }
      else if( x >= 0 )
        ++pObject->_refCounter;
      r->_pcount = &pObject->_refCounter;
    }
  else
    r->_pcount = NULL;
  r->_ref = const_cast<T *>( pObject );
}

///////////////////////////////////////////////////////////////////////////////
template <class T>
void RCObject::destroy( const_ref<T> *r )
{
#ifdef DEBUG_REF
  std::cout << "RCObject::destroy (" << r->_ref << ")" << std::endl;
  if( r->_pcount )
    std::cout << "  count: " << *r->_pcount << std::endl;
  else
    std::cout << "  no counter\n";
#endif
  if( r->_pcount && --(*r->_pcount) == 0 )
    {
#ifdef DEBUG_REF
      std::cout << "Deleting RCObject object (" << r->_ref << ")" << std::endl;
#endif
      delete static_cast<T *>( r->_ref );
    }
}

///////////////////////////////////////////////////////////////////////////////
template <class T>
const T *RCObject::release( const_ref<T> *r ) {
#ifdef DEBUG_REF
  std::cout << "Releasing RCObject (" << r->_ref << ")" << std::endl;
#endif
  const T * const result = r->_ref;
  if ( r->_pcount && *r->_pcount >= 0 )
    --(*r->_pcount);
  r->_pcount = 0;
  r->_ref = 0;
  return result;
}


template <typename T>
class rc_ptr : public ref<T>
{
public:
#ifndef DOXYGEN_HIDE_INTERNAL_CLASSES
  class insipid
  {
  protected:
      void operator delete( void* );
      void operator delete[]( void* );
  };
#endif

  inline rc_ptr() {}
  inline explicit rc_ptr( T *p ) : ref<T>( p ) {}
  inline explicit rc_ptr( T *p, bool externalowner ) 
    : ref<T>( p, externalowner ) {}

  inline void reset( T *p = NULL )
  { *static_cast< ref<T> * >( this ) = ref<T>( p ); }
  inline T *get() const { return this->pointer(); }
  inline bool operator <( const rc_ptr<T> &other ) const {
    return this->pointer() < other.pointer();
  }

#ifndef DOXYGEN_HIDE_INTERNAL_CLASSES
  inline operator insipid*() const
  {
    return reinterpret_cast< insipid* >( this->pointer() );
  }
#endif
};



template <>
class RefConstruction< char > : DefaultRefConstruction
{

  private:

    template < class R > friend class ref;
    template < class R > friend class const_ref;

    RefConstruction();


};


template <>
class RefConstruction< unsigned char > : DefaultRefConstruction
{

  private:

    template < class R > friend class ref;
    template < class R > friend class const_ref;

    RefConstruction();


};


template <>
class RefConstruction< short > : DefaultRefConstruction
{

  private:

    template < class R > friend class ref;
    template < class R > friend class const_ref;

    RefConstruction();


};


template <>
class RefConstruction< unsigned short > : DefaultRefConstruction
{

  private:

    template < class R > friend class ref;
    template < class R > friend class const_ref;

    RefConstruction();


};


template <>
class RefConstruction< int > : DefaultRefConstruction
{

  private:

    template < class R > friend class ref;
    template < class R > friend class const_ref;

    RefConstruction();


};


template <>
class RefConstruction< unsigned int > : DefaultRefConstruction
{

  private:

    template < class R > friend class ref;
    template < class R > friend class const_ref;

    RefConstruction();


};


template <>
class RefConstruction< long > : DefaultRefConstruction
{

  private:

    template < class R > friend class ref;
    template < class R > friend class const_ref;

    RefConstruction();


};


template <>
class RefConstruction< unsigned long > : DefaultRefConstruction
{

  private:

    template < class R > friend class ref;
    template < class R > friend class const_ref;

    RefConstruction();


};


template <>
class RefConstruction< float > : DefaultRefConstruction
{

  private:

    template < class R > friend class ref;
    template < class R > friend class const_ref;

    RefConstruction();


};


template <>
class RefConstruction< double > : DefaultRefConstruction
{

  private:

    template < class R > friend class ref;
    template < class R > friend class const_ref;

    RefConstruction();


};


} // namespace carto


#endif
