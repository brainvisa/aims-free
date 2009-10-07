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

#ifndef CARTOBASE_SMART_SHAREDPTR_H
#define CARTOBASE_SMART_SHAREDPTR_H

#include <cartobase/smart/weakobject.h>
#include <cartobase/smart/rcptr.h>

namespace carto
{

  /** SharedObject allows to work with combined reference counting pointers
      and weak (Observer) pointers.
  \see weak_shared_ptr, rc_ptr, weak_ptr
  */
  class SharedObject : public WeakObject, public RCObject
  {
  public:
    SharedObject();
    SharedObject( const SharedObject & x );
    /// notifies observers
    virtual ~SharedObject();

    SharedObject & operator = ( const SharedObject & ) { return *this; }

    /** tests if the shared object can be deleted (no strong pointers to it).
        The test also returns false if no smart pointer at all points to the
        object (counters are 0), because it may mean the SharedObject has been
        allocated on the heap and must not be deleted by the C++ delete
        operator.
    */
    bool testDeletable();
    /** tests if the shared object can be deleted. If possible, it is actually
        deleted (suicide is commited and the object is not valid any more
        after this call)
    */
    bool tryDelete();

  protected:
    /** called by destructors to avoid double deletion. Sets the reference
        counter to -1, so any additional existing smart pointer will not
        delete the SharedObject another time.
    */
    void disableRefCount();

  private:
    template <typename T> friend class weak_shared_ptr;
    template <typename T> friend class shared_ptr;
    friend class rc_ptr_trick;

    mutable RCObject::RefCounterType weak_count;
  };


  /** weak_shared_ptr: increments a reference count, is told and becomes
      null whenever the shared object is deleted externally.
      When the shared counter reaches 0, the shared object is deleted
      (standard rc_ptr behaviour).
      T must inherit SharedObject.
  \see rc_ptr, weak_ptr, WeakObject
   */
  template <typename T>
  class weak_shared_ptr : public rc_ptr<T>, public weak_ptr<T>
  {
  public:
    explicit weak_shared_ptr( T* x = 0, bool externalowner = false );
    weak_shared_ptr( const weak_shared_ptr<T> & x );
    weak_shared_ptr( const rc_ptr<T> & x );
    weak_shared_ptr( const weak_ptr<T> & x );
    ~weak_shared_ptr();

    T* get() const { return weak_ptr<T>::get(); }

    bool operator == ( const T* x ) const
    { return get() == x; }
    bool operator == ( const weak_shared_ptr<T> & x ) const
    { return weak_ptr<T>::operator == (x); }
    bool operator == ( const rc_ptr<T> & x ) const
    { return get() == x.get(); }
    bool operator == ( const weak_ptr<T> & x ) const
    { return get() == x.get(); }
    bool operator < ( const weak_shared_ptr<T> & x ) const
    { return weak_ptr<T>::operator < (x); }
    bool operator < ( const rc_ptr<T> & x ) const
    { return get() < x.get(); }
    bool operator < ( const weak_ptr<T> & x ) const
    { return get() < x.get(); }
    T& operator * () const
    { return weak_ptr<T>::operator * (); }
    T* operator -> () const
    { return weak_ptr<T>::operator -> (); }

    weak_shared_ptr<T> & operator = ( const weak_shared_ptr<T> & x );
    weak_shared_ptr<T> & operator = ( const rc_ptr<T> & x );
    weak_shared_ptr<T> & operator = ( const weak_ptr<T> & x );
    T* release();
    void reset( T* r = 0 );

  private:
    friend class WeakObject;
    friend class rc_ptr_trick;
    void update();
  };


  /** A multi-purpose general smart pointer, which can act as either a
      rc_ptr, a weak_ptr or a weak_shared_ptr.
      As the behaviour is stored as an instance variable, this pointer
      is a bit less light-weight than the lower-level smart pointers it
      emulates.
  */
  template <typename T>
  class shared_ptr : public weak_shared_ptr<T>
  {
  public:
    enum ReferenceType
    {
      Strong,
      WeakShared,
      Weak,
    };

    explicit shared_ptr( ReferenceType t = Weak, T* x = 0,
                         bool externalowner = false );
    shared_ptr( const shared_ptr<T> & x );
    shared_ptr( const weak_shared_ptr<T> & x );
    shared_ptr( const rc_ptr<T> & x );
    shared_ptr( const weak_ptr<T> & x );
    ~shared_ptr();

    bool operator == ( const shared_ptr<T> & x ) const
    { return weak_ptr<T>::operator == (x); }
    bool operator == ( const T* x ) const
    { return this->weak_ptr<T>::get() == x; }
    bool operator == ( const rc_ptr<T> & x ) const
    { return this->weak_ptr<T>::get() == x.get(); }
    bool operator == ( const weak_ptr<T> & x ) const
    { return this->weak_ptr<T>::get() == x.get(); }
    bool operator == ( const weak_shared_ptr<T> & x ) const
    { return this->weak_ptr<T>::get() == x.get(); }
    bool operator < ( const shared_ptr<T> & x ) const
    { return weak_ptr<T>::operator < (x); }
    bool operator < ( const rc_ptr<T> & x ) const
    { return this->weak_ptr<T>::get() < x.get(); }
    bool operator < ( const weak_ptr<T> & x ) const
    { return this->weak_ptr<T>::get() < x.get(); }
    bool operator < ( const weak_shared_ptr<T> & x ) const
    { return this->weak_ptr<T>::get() < x.get(); }

    shared_ptr<T> & operator = ( const shared_ptr<T> & x );
    shared_ptr<T> & operator = ( const weak_shared_ptr<T> & x );
    shared_ptr<T> & operator = ( const rc_ptr<T> & x );
    shared_ptr<T> & operator = ( const weak_ptr<T> & x );
    T* release();
    void reset( ReferenceType t, T* r = 0 );

    ReferenceType referenceType() const;

  private:
    ReferenceType _reftype;
  };


  // ----------------------------

  template <typename T>
  inline
  weak_shared_ptr<T>::weak_shared_ptr( T* x, bool externalowner )
  : rc_ptr<T>( x, externalowner ), weak_ptr<T>( x )
  {
    // force re-attaching with the correct type for this
    if( x )
    {
      ++x->weak_count;
      x->attachWeakPtr( *this );
    }
  }


  template <typename T>
  inline
  weak_shared_ptr<T>::weak_shared_ptr( const weak_shared_ptr<T> & x )
  : rc_ptr<T>( x ), weak_ptr<T>( x )
  {
    // force re-attaching with the correct type for this
    if( x.get() )
    {
      ++x->weak_count;
      x->attachWeakPtr( *this );
    }
  }


  template <typename T>
  inline
  weak_shared_ptr<T>::weak_shared_ptr( const rc_ptr<T> & x )
  : rc_ptr<T>( x ), weak_ptr<T>( x.get() )
  {
    // force re-attaching with the correct type for this
    if( x.get() )
    {
      ++x->weak_count;
      x->attachWeakPtr( *this );
    }
  }


  template <typename T>
  inline
  weak_shared_ptr<T>::weak_shared_ptr( const weak_ptr<T> & x )
  : rc_ptr<T>( x ), weak_ptr<T>( x )
  {
    // force re-attaching with the correct type for this
    if( x.get() )
    {
      ++x->weak_count;
      x->attachWeakPtr( *this );
    }
  }


  template <typename T>
  inline
  weak_shared_ptr<T>::~weak_shared_ptr()
  {
    if( get() )
      --get()->weak_count;
  }


  template <typename T>
  inline
  void weak_shared_ptr<T>::update()
  {
    // weak_ptr<T>::update(); is done by WeakObject
    rc_ptr<T>::reset( 0 );
  }


  template <typename T>
  inline
  weak_shared_ptr<T> & weak_shared_ptr<T>::operator =
    ( const weak_shared_ptr<T> & x )
  {
    if( &x == this )
      return *this;

    if( get() )
      --get()->weak_count;
    weak_ptr<T>::operator = ( x );
    // force re-attaching with the correct type for this
    if( x.get() )
    {
      ++x->weak_count;
      x->attachWeakPtr( *this );
    }
    rc_ptr<T>::operator = ( x );
    return *this;
  }


  template <typename T>
  inline
  weak_shared_ptr<T> & weak_shared_ptr<T>::operator =
    ( const rc_ptr<T> & x )
  {
    if( this != &x )
      reset( x.get() );
    return *this;
  }


  template <typename T>
  inline
  T* weak_shared_ptr<T>::release()
  {
    if( get() )
      --get()->weak_count;
    weak_ptr<T>::release();
    return rc_ptr<T>::release();
  }


  template <typename T>
  inline
  void weak_shared_ptr<T>::reset( T* r )
  {
    if( get() )
      --get()->weak_count;
    weak_ptr<T>::reset( r );
    // force re-attaching with the correct type for this
    if( r )
    {
      ++r->weak_count;
      r->attachWeakPtr( *this );
    }
    rc_ptr<T>::reset( r );
  }

  // ----------------


  template <typename T>
  inline
  shared_ptr<T>::shared_ptr( ReferenceType t, T* x, bool externalowner )
  : weak_shared_ptr<T>( x, externalowner ), _reftype( t )
  {
    switch( t )
    {
    case Strong:
      if( x )
        --x->weak_count;
      break;
    case WeakShared:
      break;
    case Weak:
      weak_shared_ptr<T>::release();
      weak_ptr<T>::reset( x );
      break;
    }
  }


  template <typename T>
  inline
  shared_ptr<T>::shared_ptr( const shared_ptr<T> & x )
  : weak_shared_ptr<T>( x ), _reftype( x.referenceType() )
  {
    switch( referenceType() )
    {
    case Strong:
      if( x.get() )
        --x->weak_count;
      break;
    case WeakShared:
      break;
    case Weak:
      weak_shared_ptr<T>::release();
      weak_ptr<T>::reset( x.get() );
      break;
    }
  }


  template <typename T>
  inline
  shared_ptr<T>::shared_ptr( const weak_shared_ptr<T> & x )
  : weak_shared_ptr<T>( x ), _reftype( WeakShared )
  {
  }


  template <typename T>
  inline
  shared_ptr<T>::shared_ptr( const rc_ptr<T> & x )
  : weak_shared_ptr<T>( x.get() ), _reftype( Strong )
  {
    if( x )
      --x->weak_count;
  }


  template <typename T>
  inline
  shared_ptr<T>::shared_ptr( const weak_ptr<T> & x )
  : weak_shared_ptr<T>( x )
  {
    weak_shared_ptr<T>::release();
    weak_ptr<T>::reset( x.get() );
  }


  template <typename T>
  inline
  shared_ptr<T>::~shared_ptr()
  {
    switch( referenceType() )
    {
    case Strong:
      if( this->get() )
        ++this->get()->weak_count; // as it will be decremented soon
      // WARNING: this is not thread-safe
      break;
    case WeakShared:
      break;
    case Weak:
      release();
      break;
    }
  }


  template <typename T>
  inline typename shared_ptr<T>::ReferenceType
  shared_ptr<T>::referenceType() const
  {
    return _reftype;
  }


  template <typename T>
  inline
  shared_ptr<T> & shared_ptr<T>::operator =
    ( const shared_ptr<T> & x )
  {
    reset( x.referenceType(), x.get() );
    return *this;
  }


  template <typename T>
  inline
  shared_ptr<T> & shared_ptr<T>::operator =
    ( const weak_shared_ptr<T> & x )
  {
    reset( WeakShared, x.get() );
    return *this;
  }


  template <typename T>
  inline
  shared_ptr<T> & shared_ptr<T>::operator = ( const rc_ptr<T> & x )
  {
    reset( Strong, x.get() );
    return *this;
  }


  template <typename T>
  inline
  shared_ptr<T> & shared_ptr<T>::operator = ( const weak_ptr<T> & x )
  {
    reset( Weak, x.get() );
    return *this;
  }


  template <typename T>
  inline
  T* shared_ptr<T>::release()
  {
    T* x = weak_ptr<T>::release();
    if( this->get() && referenceType() == WeakShared )
      --this->get()->weak_count;
    if( referenceType() != Weak )
      return rc_ptr<T>::release();
    return x;
  }


  template <typename T>
  inline
  void shared_ptr<T>::reset( ReferenceType t, T* r )
  {
    if( r ==  this->get() )
    {
      if( t != referenceType() )
      {
        switch( t )
        {
          case Strong:
            switch( referenceType() )
            {
              case WeakShared:
                --r->weak_count;
                break;
              case Weak:
                rc_ptr<T>::reset( r );
                break;
              default:
                break;
            }
            break;
          case WeakShared:
            switch( referenceType() )
            {
              case Strong:
                ++r->weak_count;
                break;
              case Weak:
                weak_shared_ptr<T>::reset( r );
                break;
              default:
                break;
            }
            break;
          case Weak:
            switch( referenceType() )
            {
              case Strong:
                rc_ptr<T>::reset( 0 );
                if( this->get() ) // r could have been destroyed by now
                  r->attachWeakPtr( static_cast<weak_ptr<T> &>( *this ) );
                break;
              case WeakShared:
                if( r->_refCounter == 1 )
                  weak_shared_ptr<T>::reset( 0 ); // should destroy r...
                else
                {
                  weak_shared_ptr<T>::reset( 0 );
                  weak_ptr<T>::reset( r );
                }
                break;
              default:
                break;
            }
        }
        _reftype = t;
      }
      return;
    }

    if( this->get() )
      switch( referenceType() )
      {
        case Strong:
          rc_ptr<T>::reset( 0 );
          break;
        case WeakShared:
          weak_shared_ptr<T>::reset( 0 );
          break;
        case Weak:
          weak_ptr<T>::reset( 0 );
      }

    _reftype = t;
    switch( t )
    {
      case Strong:
      // force re-attaching with the correct type for this
        if( r )
        {
          r->attachWeakPtr( *this );
          rc_ptr<T>::reset( r );
          weak_ptr<T>::reset( r );
        }
        break;
      case WeakShared:
      // force re-attaching with the correct type for this
        if( r )
          weak_shared_ptr<T>::reset( r );
        break;
      case Weak:
        weak_ptr<T>::reset( r );
        break;
    }
  }

}

#endif

