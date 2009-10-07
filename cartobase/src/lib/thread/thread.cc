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

#include <cartobase/thread/thread.h>
#include <iostream>
#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

using namespace std;

#ifdef _WIN32
static DWORD mainThreadId;
#else
static pthread_t mainThreadId;
#endif
static bool mainThreadInitialized = false;

namespace carto
{

  struct Thread::Private
  {
    Private() : suicideSafe( false ) {}

#ifdef _WIN32
    HANDLE		thread;
    DWORD		threadId;
#else
    pthread_t		thread;
    pthread_attr_t	attributes;
#endif
    bool		suicideSafe;
  };

}


carto::Thread::Thread()
             : _detached( false ),
               _cancellable( false ),
               _callStop( false ), 
               d( new carto::Thread::Private )
{
  if( !mainThreadInitialized )
    {
      /* the first time we are called, we can be called only from the 
	 main thread (unless the system API has been used directly) */
#ifdef _WIN32
      mainThreadId = GetCurrentThreadId();
#else
      mainThreadId = pthread_self();
#endif
      mainThreadInitialized = true;
    }



#ifdef CARTO_DEBUG

  std::cerr << "Thread::Thread()" << std::endl;

#endif

  reset();
#ifndef _WIN32
  if ( pthread_attr_init( &d->attributes ) )
    std::cerr << "Thread attributes initialization failed"
              << std::endl;
  // Windows: nothing to be done here
#endif
}


carto::Thread::~Thread()
{

#ifdef CARTO_DEBUG

  std::cerr << "Thread::~Thread()" << std::endl;

#endif
#ifndef _WIN32
  pthread_attr_destroy( &d->attributes );
#endif
  delete d;
}


void carto::Thread::start()
{

#ifdef CARTO_DEBUG

  std::cerr << "Thread::start()" << std::endl;

#endif

  doStart();

}


void carto::Thread::run()
{

#ifdef CARTO_DEBUG

  std::cerr << "Thread::run()" << std::endl;

#endif

  doRun();

}


void carto::Thread::stop()
{

#ifdef CARTO_DEBUG

  std::cerr << "Thread::stop()" << std::endl;

#endif

  doStop();

}


void carto::Thread::reset()
{

#ifdef CARTO_DEBUG

  std::cerr << "Thread::reset()" << std::endl;

#endif

  d->thread = 0;
#ifdef _WIN32
  d->threadId = 0;
#endif
}


#ifdef _WIN32
namespace
{
  DWORD WINAPI _bootstrap( PVOID thread )
  {
    void *x = carto::Thread::bootstrap( thread );
    return (DWORD) x;
  }
}
#endif


void carto::Thread::launch()
{
 
#ifdef CARTO_DEBUG

  std::cerr << "Thread::launch()" << std::endl;

#endif

  doPreLaunch();
  bool suic = d->suicideSafe;

#ifdef _WIN32
  // TODO: take attributes into account...
  d->thread = CreateThread( NULL, 0, _bootstrap, (PVOID) this, 0, &d->threadId );
  if( !d->thread )
    {
      cerr << "Thread::launch() : failed." << endl;
      return;
    }
  if( _detached )
    CloseHandle( d->thread );
#else
  int	err = pthread_create( &d->thread, &d->attributes, bootstrap, this );
  if ( err )
    std::cerr << "Thread::launch() : failed. code: " << err << ", thread: " 
              << d->thread << std::endl;
#endif
  if( !suic )
    doPostLaunch();

}


void carto::Thread::join()
{

#ifdef CARTO_DEBUG

  std::cerr << "Thread::join()" << std::endl;

#endif

#ifdef _WIN32
  if( WaitForSingleObject( d->thread, INFINITE ) != WAIT_OBJECT_0 )
    cerr << "Thread::join() failed!" << endl;
  else
    CloseHandle( d->thread );
#else
  pthread_join( d->thread, 0 );
#endif

  reset();
}


void carto::Thread::detach()
{

#ifdef CARTO_DEBUG

  std::cerr << "Thread::detach()" << std::endl;

#endif

#ifdef _WIN32
  if( d->thread && !_detached )
    CloseHandle( d->thread );
#else
  if ( pthread_attr_setdetachstate( &d->attributes, PTHREAD_CREATE_DETACHED ) )
  {

    _detached = false;
    std::cerr << "  Failed to set thread detached mode"
              << std::endl;

  }
  else
    _detached = true; 
#endif
}


void carto::Thread::setCancellable( bool value, bool callStop )
{

#ifdef CARTO_DEBUG

  std::cerr << "Thread::setCancellable()" << std::endl;

#endif

  _cancellable = value;
  _callStop = callStop;
#ifdef _WIN32
  //if( d->thread )
  //  SetHandleInformation( d->thread, THREAD_TERMINATE, );
  // TODO
  // It seems cancel state does not exist on Windows
#else
  if ( value )
    pthread_setcancelstate( PTHREAD_CANCEL_ENABLE, 0 );
#endif
}


void carto::Thread::cancel()
{

#ifdef CARTO_DEBUG

  std::cerr << "Thread::cancel()" << std::endl;

#endif

#ifdef _WIN32
  if( !TerminateThread( d->thread, 0 ) )
#else
  if ( pthread_cancel( d->thread ) )
#endif
    std::cerr << "  Thread failed in cancellation"
              << std::endl;
  else
  {

    if ( _callStop )
      stop(); 
    reset();

  }

}


bool carto::Thread::isActive() const
{

  return d->thread != 0;

}


void carto::Thread::setSuicideSafe( bool x )
{
  d->suicideSafe = x;
}


bool carto::Thread::isSuicideSafe() const
{
  return d->suicideSafe;
}


void* carto::Thread::bootstrap( void* thread )
{

#ifdef CARTO_DEBUG

  std::cerr << "Thread::bootstrap()" << std::endl;

#endif

  carto::Thread* theThread = static_cast< carto::Thread* >( thread );

  if ( theThread )
  {

    theThread->start();
    bool	suic = theThread->isSuicideSafe();
    theThread->run();
    if( !suic )
      theThread->stop();

  }

  return 0;

}


void carto::Thread::testCancel()
{

#ifdef CARTO_DEBUG

  std::cerr << "Thread::testCancel()" << std::endl;

#endif
#ifndef _WIN32
  if ( _cancellable )
    pthread_testcancel();
#endif
}


void carto::Thread::doStart()
{
}


void carto::Thread::doStop()
{
}


void carto::Thread::doPreLaunch()
{
}


void carto::Thread::doPostLaunch()
{
}


bool carto::Thread::isCurrent() const
{
#ifdef _WIN32
  return GetCurrentThreadId() == d->threadId;
#else
  return pthread_equal(pthread_self(), d->thread) ? true : false;
#endif
}


bool carto::Thread::currentIsMainThread()
{
#ifdef _WIN32
  return GetCurrentThreadId() == mainThreadId;
#else
  return pthread_equal(pthread_self(), mainThreadId) ? true : false;
#endif
}


#endif
