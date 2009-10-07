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

#include <cartobase/thread/threadCondition.h>
#include <cartobase/thread/mutex.h>
#ifdef _WIN32
#include <cartobase/thread/mutexprivatewindows.h>
#else
#include <cartobase/thread/mutexprivatepthread.h>
#endif
#include <iostream>

using namespace std;

namespace carto
{
  struct ThreadCondition::Private
  {
#ifdef _WIN32

    HANDLE		condition;

#else

    pthread_cond_t	condition;
    pthread_condattr_t	attributes;

#endif
 };
}


carto::ThreadCondition::ThreadCondition() : d( new ThreadCondition::Private )
{
#ifdef _WIN32

  // TODO
  cerr << "ThreadCondition: not implemented for Windows" << endl;

#else

  pthread_cond_init( &d->condition, 0 );
  pthread_condattr_init( &d->attributes );

#endif
}


carto::ThreadCondition::~ThreadCondition()
{
#ifdef _WIN32

  // TODO

#else

  pthread_condattr_destroy( &d->attributes );
  pthread_cond_destroy( &d->condition );

#endif

  delete d;
}


void carto::ThreadCondition::wait( carto::Mutex& mutex )
{

  _mutex.lock();

#ifdef _WIN32
  // TODO
#else
  pthread_cond_wait( &d->condition, &mutex.priv()->mutex );
#endif

  _mutex.unlock();

}


void carto::ThreadCondition::timedWait( carto::Mutex& mutex,
                                       const struct timespec* t )
{

  _mutex.lock();

#ifdef _WIN32
  // TODO
#else
  pthread_cond_timedwait( &d->condition, &mutex.priv()->mutex, t );
#endif

  _mutex.unlock();

}


void carto::ThreadCondition::signal()
{
#ifdef _WIN32
  // TODO
#else
  pthread_cond_signal( &d->condition );
#endif
}


void carto::ThreadCondition::broadcast()
{
#ifdef _WIN32
  // TODO
#else
  pthread_cond_broadcast( &d->condition );
#endif
}


void carto::ThreadCondition::setProcessPrivate()
{
#ifdef _WIN32
  // TODO
#else
#if !defined( __GLIBC__ ) || __GLIBC_PREREQ(2,2)
  if ( pthread_condattr_setpshared( &d->attributes, PTHREAD_PROCESS_PRIVATE ) )
    std::cerr << "carto::Condition : failed in setting private mode"
              << std::endl;
#else
#warning "carto::Mutex::setProcessPrivate() won't work, needs glibc >= 2.2"
#endif
#endif
}


void carto::ThreadCondition::setProcessShared()
{
#ifdef _WIN32
  // TODO
#else
#if !defined( __GLIBC__ ) || __GLIBC_PREREQ(2,2)
  if ( pthread_condattr_setpshared( &d->attributes, PTHREAD_PROCESS_SHARED ) )
    std::cerr << "carto::Condition : failed in setting shared mode"
              << std::endl;
#else
#warning "carto::Mutex::setProcessPrivate() won't work, needs glibc >= 2.2"
#endif
#endif
}

#endif
