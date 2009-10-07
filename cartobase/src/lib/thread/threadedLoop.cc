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

#include <cartobase/thread/threadedLoop.h>
#include <cartobase/thread/loopThread.h>
#include <cartobase/thread/loopContext.h>
#include <cartobase/thread/semaphore.h>
#include <cartobase/thread/cpu.h>
#include <list>
#include <iostream>
#include <math.h>


struct carto::ThreadedLoop::Private
{
  Private();
  ~Private();

  bool 				endThreads;
  std::list<LoopThread *>	threads;
};


carto::ThreadedLoop::Private::Private()
  : endThreads( true )
{
}


carto::ThreadedLoop::Private::~Private()
{
  std::list<LoopThread *>::iterator	i, e = threads.end();
  LoopThread				*lt;
  for( i=threads.begin(); i!=e; ++i )
    {
      lt = *i;
      lt->setFinished( true );
      lt->holdSemaphore()->post();
      lt->join();
      delete lt;
    }
}


carto::ThreadedLoop::ThreadedLoop( carto::LoopContext* loopContext,
                                   int grain, int startIndex, int count,
                                   float threadsByCpu )

  : d( new Private ), 
    _loopContext( loopContext ),
    _grain( grain ),
    _startIndex( startIndex ),
    _count( count ),
    _threadsByCpu( threadsByCpu ),
    _currentIndex( 0 ),
    _todo( count ),
    _currentWorkerCount( 0 )
{
}


carto::ThreadedLoop::~ThreadedLoop()
{
  delete d;
}


carto::LoopContext* carto::ThreadedLoop::getLoopContext() const
{

  return _loopContext;

}


void carto::ThreadedLoop::setLoopContext( LoopContext* lc )
{

  _loopContext = lc;

}


bool carto::ThreadedLoop::getStartIndexAndCount( int& startIndex,
                                                 int& count )
{

  _loopContext->lock();

  if ( _todo <= 0 || _loopContext->cancel() )
  {

    _loopContext->unlock();
    if ( _loopContext->cancel() )
      std::cerr << "=========================> aborted by cancel" << std::endl;

#ifdef CARTO_DEBUG

  std::cerr << "  ThreadedLoop::getStartIndexAndCount() "
            << "return false" << std::endl;

#endif

    return false;

  }

  _todo -= _grain;
  startIndex = _currentIndex;
  count = _grain;
  if ( _todo < 0 )
  {

    count += _todo;
    _todo = 0;

  }
  _currentIndex += count;
  startIndex += _startIndex;

#ifdef CARTO_DEBUG

  std::cerr << "  ThreadedLoop::getStartIndexAndCount() "
            << "{ start=" << startIndex << ", count=" << count << " }"
	    << std::endl;

#endif

  _loopContext->gaugeAdd( count );

  _loopContext->unlock();

#ifdef CARTO_DEBUG

  std::cerr << "  ThreadedLoop::getStartIndexAndCount() "
            << "return true" << std::endl;

#endif

  return true;

}


int carto::ThreadedLoop::getTodo() const
{

  return _todo;

}


void carto::ThreadedLoop::increaseWorkerCount()
{

  _loopContext->lock();
  _currentWorkerCount ++;
  _loopContext->unlock();

}


void carto::ThreadedLoop::decreaseWorkerCount()
{

  _loopContext->lock();
  _currentWorkerCount --;
  _loopContext->unlock();

}


int carto::ThreadedLoop::getWorkerCount() const
{

  return _currentWorkerCount;

}


bool carto::ThreadedLoop::launch( bool resetGauge, bool resetCancel )
{

  bool result = true;

  _loopContext->unlock();
  _loopContext->lock();

  _currentIndex = 0;
  _todo = _count;
  _currentWorkerCount = 0;

  unsigned nCpu = cpuCount();

#ifdef CARTO_DEBUG

  std::cerr << "CPU count     : " << nCpu << std::endl;
  std::cerr << "thread by CPU : " << _threadsByCpu << std::endl;

#endif

  int		nth = (int) rint( nCpu * _threadsByCpu ), i, 
    n = d->threads.size();
  LoopThread	*lt;
  Semaphore	*sem;

  if( nth < 1 )
    nth = 1;

  for( i=nth; i<n; ++i )
    {
      lt = d->threads.back();
      d->threads.pop_back();
      if( !endThreadsWhenDone() )
        {
          sem = lt->holdSemaphore();
          lt->setFinished( true );
          // release thread
          sem->post();
          // let is finish
          lt->join();
        }
      delete lt;
    }

  int countThread = 0;
  int t;
  n = d->threads.size();

  for ( t = n; ( t < nth ) &&  ( t * _grain < _count ); 
        ++t, ++countThread )
    {
      lt = new carto::LoopThread( this );
      d->threads.push_back( lt );
      if( !endThreadsWhenDone() )
        lt->launch();
    }

#ifdef CARTO_DEBUG

  std::cerr << "thread count  : " << countThread << std::endl;

#endif

  std::list<LoopThread *>::iterator	it, et = d->threads.end();

  for ( it = d->threads.begin(); it != et; ++it )
    {
      lt = *it;
      if( endThreadsWhenDone() )
        lt->launch();
      else
        lt->holdSemaphore()->post(); // start
    }

  _loopContext->unlock();

  if( endThreadsWhenDone() )
    {
      for ( it = d->threads.begin(); it != et; ++it )
        {
          lt = *it;
          lt->join();
          delete lt;
        }
      d->threads.clear();
    }
  else
    for ( it = d->threads.begin(); it != et; ++it )
      (*it)->endSemaphore()->wait();

  if ( _loopContext->cancel() )
    result = false;

  if ( resetGauge )
    _loopContext->resetGauge();
  if ( resetCancel )
    _loopContext->resetCancel();

  return result;

}


void carto::ThreadedLoop::setEndThreadsWhenDone( bool x )
{
  d->endThreads = x;
}


bool carto::ThreadedLoop::endThreadsWhenDone() const
{
  return d->endThreads;
}


void carto::ThreadedLoop::setLoopCount( int n )
{
  _count = n;
}

#endif
