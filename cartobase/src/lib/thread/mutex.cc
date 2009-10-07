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

#ifndef CARTO_NO_THREAD

#include <cartobase/thread/mutex.h>
#include <iostream>
#ifdef _WIN32
#include <cartobase/thread/mutexprivatewindows.h>
#else
#include <cartobase/thread/mutexprivatepthread.h>
#endif


carto::Mutex::Mutex( int type ) : d( new Mutex::Private )
{
#ifdef _WIN32

  // windows mutexes are always recursive as far as I understand
  d->mutex = CreateMutex( 0, true, 0 );
  // on windows mutexes are initially locked
  ReleaseMutex( d->mutex );

#else

  pthread_mutexattr_init( &d->attributes );
  setType( type );

  pthread_mutex_init( &d->mutex, &d->attributes );

#endif
}


carto::Mutex::~Mutex()
{
#ifdef _WIN32

  CloseHandle( d->mutex );

#else

  pthread_mutexattr_destroy( &d->attributes );
  pthread_mutex_destroy( &d->mutex );

#endif

  delete d;
}


void carto::Mutex::lock()
{
#ifdef _WIN32

  WaitForSingleObject( d->mutex, INFINITE );

#else

  if ( pthread_mutex_trylock( &d->mutex ) )
    pthread_mutex_lock( &d->mutex );

#endif
}


void carto::Mutex::unlock()
{
#ifdef _WIN32

  ReleaseMutex( d->mutex );

#else

  pthread_mutex_unlock( &d->mutex );

#endif
}

void carto::Mutex::setProcessPrivate()
{
#ifdef _WIN32

  // TODO

#else
#if !defined( __GLIBC__ ) || __GLIBC_PREREQ(2,2)
  if ( pthread_mutexattr_setpshared( &d->attributes, 
                                     PTHREAD_PROCESS_PRIVATE ) )
    std::cerr << "carto::Mutex : failed in setting private mode" << std::endl;
#else
#warning "carto::Mutex::setProcessPrivate() won't work, needs glibc >= 2.2"
#endif
#endif
}


void carto::Mutex::setProcessShared()
{
#ifdef _WIN32

  // TODO

#else
#if (!defined( __GLIBC__ ) || ( __GLIBC_PREREQ(2,2) ) )

  if ( pthread_mutexattr_setpshared( &d->attributes, PTHREAD_PROCESS_SHARED ) )
    std::cerr << "carto::Mutex : failed in setting shared mode" << std::endl;

#else
#warning "carto::Mutex::setProcessShared() won't work, needs glibc >= 2.2"
#endif
#endif
}


void carto::Mutex::setRecursive()
{
#ifdef _WIN32

#else
//#if !defined( __GNUC__ ) || defined( __USE_UNIX98 )
  pthread_mutexattr_settype( &d->attributes, PTHREAD_MUTEX_RECURSIVE );
//#else
//#warning setRecursive is not working
//#endif
#endif
}


void carto::Mutex::setFast()
{
#ifdef _WIN32

#else
//#if !defined( __GNUC__ ) || defined( __USE_UNIX98 )
  pthread_mutexattr_settype( &d->attributes, PTHREAD_MUTEX_NORMAL );
//#endif
#endif
}


void carto::Mutex::setErrorChecking()
{
#ifdef _WIN32

#else
//#if !defined( __GNUC__ ) || defined( __USE_UNIX98 )
  pthread_mutexattr_settype( &d->attributes, PTHREAD_MUTEX_ERRORCHECK );
//#endif
#endif
}


void carto::Mutex::setType( int x )
{
  switch( x & TypeMask )
    {
    case ErrorChecking:
      setErrorChecking();
      break;
    case Recursive:
      setRecursive();
      break;
    default:
      setFast();
      break;
    }
  if( x & ProcessShared )
    setProcessShared();
  else
    setProcessPrivate();
}


carto::Mutex::Private* carto::Mutex::priv()
{
  return d;
}


#endif
