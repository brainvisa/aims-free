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

#include <cartobase/smart/sharedptr.h>
#include <cartobase/smart/rcptrtrick.h>

using namespace carto;


SharedObject::SharedObject()
  : WeakObject(), RCObject(), weak_count( 0 )
{
}


SharedObject::SharedObject( const SharedObject & x )
  : WeakObject( x ), RCObject( x ), weak_count( 0 )
{
}


SharedObject::~SharedObject()
{
  disableRefCount();
  // notifyUnregisterObservers(); // done in WeakObject
}


bool SharedObject::testDeletable()
{
  return weak_count != 0 && weak_count == rc_ptr_trick::refCount( *this );
}


bool SharedObject::tryDelete()
{
  if( testDeletable() )
  {
    // force the counter to -1 before deleting to prevent multiple deletion
    disableRefCount();
    delete this;
    return true;
  }
  return false;
}


void SharedObject::disableRefCount()
{
#if defined( CARTO_RCPTR_USE_BOOST )
  // boost doesn't allow assignation of atomic counters
  while( rc_ptr_trick::refCount( *this ) >= 0 )
    --rc_ptr_trick::refCount( *this );
#else
  rc_ptr_trick::refCount( *this ) = -1;
#endif
}

