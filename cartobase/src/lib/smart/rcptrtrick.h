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


#ifndef CARTOBASE_SMART_RCPTRTRICK_H
#define CARTOBASE_SMART_RCPTRTRICK_H

#include <cartobase/smart/rcptr.h>
#include <cartobase/smart/sharedptr.h>

namespace carto
{

  /** rc_ptr_trick merely makes a public access to the reference counter 
      inside a rc_ptr. It is intended to be used only on low-level, hardly 
      legal dirty operations, as its name tells. It is *not* intended to be 
      used by normal programs, so just DON'T USE IT.
   */
  class rc_ptr_trick
  {
  public:
    /** just increments the ref-counter of the underlying rc_ptr so that 
        the rc_ptr will never delete the object in it (it doesn't own 
        the object anymore) */
    template <typename T> 
    static void releaseOwnership( rc_ptr<T> & );
    static void releaseOwnership( RCObject & );
    /// decrements the ref-counter
    template <typename T> 
    static void restoreOwnership( rc_ptr<T> & );
    static void restoreOwnership( RCObject & );
    /// allows complete read-write access to the counter
    template <typename T>
    static RCObject::RefCounterType & refCount( shared_ptr<T> & );
    template <typename T>
    static RCObject::RefCounterType & refCount( rc_ptr<T> & );
    static RCObject::RefCounterType & refCount( RCObject & );
    template <typename T>
    static RCObject::RefCounterType & weakCount( weak_shared_ptr<T> & );
    static RCObject::RefCounterType & weakCount( SharedObject & );
  };


  template <typename T> inline
  void rc_ptr_trick::releaseOwnership( rc_ptr<T> & x )
  {
    if( x._pcount && *x._pcount > 0 )
      ++(*x._pcount);
  }


  inline void rc_ptr_trick::releaseOwnership( RCObject & x )
  {
    if( x._refCounter > 0 )
      ++x._refCounter;
  }


  template <typename T> inline
  void rc_ptr_trick::restoreOwnership( rc_ptr<T> & x )
  {
    if( x._pcount && *x._pcount > 0 )
      --(*x._pcount);
  }


  inline void rc_ptr_trick::restoreOwnership( RCObject & x )
  {
    if( x._refCounter > 0 )
      --x._refCounter;
  }


  template <typename T> inline
  RCObject::RefCounterType & rc_ptr_trick::refCount( shared_ptr<T> & x )
  {
    return x->_refCounter;
  }


  template <typename T> inline
  RCObject::RefCounterType & rc_ptr_trick::refCount( rc_ptr<T> & x )
  {
    return *x._pcount;
  }


  inline RCObject::RefCounterType & rc_ptr_trick::refCount( RCObject & x )
  {
    return x._refCounter;
  }


  template <typename T> inline
  RCObject::RefCounterType &
  rc_ptr_trick::weakCount( weak_shared_ptr<T> & x )
  {
    return x->weak_count;
  }


  inline RCObject::RefCounterType &
  rc_ptr_trick::weakCount( SharedObject & x )
  {
    return x.weak_count;
  }

}

#endif

