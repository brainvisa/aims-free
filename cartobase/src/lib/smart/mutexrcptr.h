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


#ifndef CARTOBASE_SMART_MUTEXRCPTR_H
#define CARTOBASE_SMART_MUTEXRCPTR_H

#include <cartobase/smart/rcptr.h>
#include <cartobase/thread/mutex.h>

namespace carto
{

  class MutexedObject : public RCObject
  {
  public:
#ifndef CARTO_NO_THREAD
    typedef carto::Mutex Mutex;
#else
    // fake, null mutex
    class Mutex
    {
    public:
      enum Type { Recursive=2 };
      Mutex( int = Recursive ) {}
      void lock() {}
      void unlock() {}
    };
#endif

    MutexedObject();
    MutexedObject( const MutexedObject & );
    virtual ~MutexedObject();
    MutexedObject & operator = ( const MutexedObject & );

    Mutex* mutex() const;

  private:
    mutable Mutex	*_mutex;
  };


  /** Mutex-protected rc_ptr

      This class is a "temporary" solution to have thread-safe rc_ptr.
      In the future version, the normal rc_ptr class should be thread-safe 
      using atomic operations.

      Here we are using a mutex to access the pointer and ref-counter.
      The API is willingly not related to the rc_ptr API (no inheritance)
   */
  template <typename T>
  class MutexRcPtr
  {
  public:
    MutexRcPtr();
    MutexRcPtr( T* ptr );
    MutexRcPtr( MutexRcPtr<T> & ptr );
    ~MutexRcPtr();

    void reset( T *p=0 );
    T * get();
    T * release();
    MutexRcPtr<T> & operator = ( MutexRcPtr<T> &other );
    T * operator -> ();
    T & operator * ();
    bool operator < ( const MutexRcPtr<T> &other ) const;
    bool operator == ( const MutexRcPtr< T > &other ) const;
    bool operator != ( const MutexRcPtr< T > &other ) const;
    int refCount() const;

    /// not thread-safe, almost private, for low-level operation only
    rc_ptr<T> rcptr();
    MutexedObject::Mutex* mutex();
    void lock();
    void unlock();

  private:
    rc_ptr<T>	_rcptr;
  };


  // ----------------

  inline MutexedObject::MutexedObject()
    : _mutex( new MutexedObject::Mutex( Mutex::Recursive ) )
  {
  }


  inline MutexedObject::MutexedObject( const MutexedObject & other )
    : RCObject( other ), _mutex( new MutexedObject::Mutex( Mutex::Recursive ) )
  {
  }


  inline MutexedObject & 
  MutexedObject::operator = ( const MutexedObject & other )
  {
    if( this != &other )
      {
        RCObject::operator = ( other );
        // _mutex stays distinct
      }
    return *this;
  }


  inline MutexedObject::Mutex* MutexedObject::mutex() const
  {
    return _mutex;
  }

  // -----------------

  template <typename T>
  inline MutexRcPtr<T>::MutexRcPtr()
  {
  }


  template <typename T>
  inline MutexRcPtr<T>::MutexRcPtr( T* ptr )
  {
    if( ptr )
      {
        ptr->mutex()->lock();
        _rcptr.reset( ptr );
        ptr->mutex()->unlock();
      }
  }


  template <typename T>
  inline MutexRcPtr<T>::MutexRcPtr( MutexRcPtr<T> & ptr )
  {
    ptr.lock();
    _rcptr = ptr._rcptr;
    ptr.unlock();
  }


  template <typename T>
  inline MutexRcPtr<T>::~MutexRcPtr()
  {
    reset( 0 );
  }


  template <typename T>
  inline void MutexRcPtr<T>::reset( T* ptr )
  {
    lock();
    T	*t = _rcptr.get();
    if( t )
      {
        if( _rcptr.refCount() == 1 )
          {
            _rcptr.release();
            t->mutex()->unlock();
            delete t;
          }
        else
          {
            _rcptr.release();
            t->mutex()->unlock();
          }
      }

    if( ptr )
      {
        ptr->mutex()->lock();
        _rcptr.reset( ptr );
        ptr->mutex()->unlock();
      }
  }


  template <typename T>
  inline T* MutexRcPtr<T>::get()
  {
    /* lock();
    T	*t = _rcptr.get();
    unlock();
    return t; */
    return _rcptr.get();
  }


  template <typename T>
  inline T* MutexRcPtr<T>::release()
  {
    lock();
    T	*t = _rcptr.get();
    _rcptr.release();
    if( t )
      t->mutex()->unlock();
    return t;
  }

  template <typename T>
  inline MutexRcPtr<T> & MutexRcPtr<T>::operator = ( MutexRcPtr<T> &other )
  {
    if( this == &other )
      return *this;
    reset( 0 );
    other.lock();
    _rcptr = other._rcptr;
    other.unlock();
  }


  template <typename T>
  inline T* MutexRcPtr<T>::operator -> ()
  {
    return get();
  }


  template <typename T>
  inline T & MutexRcPtr<T>::operator * ()
  {
    return *get();
  }


  template <typename T>
  inline bool MutexRcPtr<T>::operator < ( const MutexRcPtr<T> &other ) const
  {
    return get() < other.get();
  }


  template <typename T>
  inline bool MutexRcPtr<T>::operator == ( const MutexRcPtr<T> &other ) const
  {
    return get() == other.get();
  }


  template <typename T>
  inline bool MutexRcPtr<T>::operator != ( const MutexRcPtr<T> &other ) const
  {
    return get() != other.get();
  }


  template <typename T>
  inline int MutexRcPtr<T>::refCount() const
  {
    lock();
    int x = _rcptr.refCount();
    unlock();
    return x;
  }


  template <typename T>
  inline rc_ptr<T> MutexRcPtr<T>::rcptr()
  {
    return _rcptr;
  }


  template <typename T>
  inline MutexedObject::Mutex* MutexRcPtr<T>::mutex()
  {
    T	*t = _rcptr.get();
    return t ? t->mutex() : 0;
  }


  template <typename T>
  inline void MutexRcPtr<T>::lock()
  {
    MutexedObject::Mutex	*m = mutex();
    if( m )
      m->lock();
  }


  template <typename T>
  inline void MutexRcPtr<T>::unlock()
  {
    MutexedObject::Mutex	*m = mutex();
    if( m )
      m->unlock();
  }

}

#endif

