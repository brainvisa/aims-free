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

#ifndef CARTOBASE_SMART_WEAKOBJECT_H
#define CARTOBASE_SMART_WEAKOBJECT_H


#include <cartobase/config/cartobase_config.h>
#include <cartobase/smart/weakptr.h>
#include <map>


namespace carto
{

  template <typename T> class weak_shared_ptr;

class WeakObject
{

  typedef weak_ptr<WeakObject> WeakPtr;
  typedef weak_shared_ptr<WeakObject> WeakSharedPtr;

  public:

    template <typename T> void attachWeakPtr( weak_ptr<T>& ) const;
    template <typename T> void detachWeakPtr( weak_ptr<T>& ) const;
    template <typename T> void attachWeakPtr( weak_shared_ptr<T>& ) const;
    template <typename T> void detachWeakPtr( weak_shared_ptr<T>& ) const;
    virtual ~WeakObject();

  protected:

    WeakObject();
    WeakObject( const WeakObject& );
    WeakObject& operator=( const WeakObject& );

  private:

    void attachWeakPtr( WeakPtr* ) const;
    void detachWeakPtr( WeakPtr* ) const;
    void attachWeakPtr( WeakSharedPtr* ) const;
    void detachWeakPtr( WeakSharedPtr* ) const;
    void notifyObervers() const;

    mutable std::map<WeakPtr*, bool> observers;

};


template <typename T>
inline 
void WeakObject::attachWeakPtr( weak_ptr<T>& ptr ) const
{
  attachWeakPtr( reinterpret_cast<WeakPtr*>( &ptr ) );
}


template <typename T>
inline
void WeakObject::detachWeakPtr( weak_ptr<T>& ptr ) const
{
  detachWeakPtr( reinterpret_cast<WeakPtr*>( &ptr ) );
}


template <typename T>
inline
void WeakObject::attachWeakPtr( weak_shared_ptr<T>& ptr ) const
{
  attachWeakPtr( reinterpret_cast<WeakSharedPtr*>( &ptr ) );
}


template <typename T>
inline
void WeakObject::detachWeakPtr( weak_shared_ptr<T>& ptr ) const
{
  detachWeakPtr( reinterpret_cast<WeakSharedPtr*>( &ptr ) );
}


} // namespace carto


#endif
