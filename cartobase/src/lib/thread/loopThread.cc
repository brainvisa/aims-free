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

#include <cartobase/thread/loopThread.h>
#include <cartobase/thread/loopContext.h>
#include <cartobase/thread/threadedLoop.h>
#include <cartobase/thread/semaphore.h>
#include <iostream>


struct carto::LoopThread::Private
{
  Private() : finished( false ), holdsem( 0 ), endsem( 0 ) {}
  ~Private();
  bool		finished;
  Semaphore	*holdsem;
  Semaphore	*endsem;
};


carto::LoopThread::Private::~Private()
{
  if( holdsem )
    {
      holdsem->post();
      delete holdsem;
    }
  if( endsem )
    {
      endsem->post();
      delete endsem;
    }
}



carto::LoopThread::LoopThread( carto::ThreadedLoop* parent )
                 : carto::Thread(),
                   _parent( parent ), 
                   d( 0 )
{
  if( !parent->endThreadsWhenDone() )
    {
      d = new Private;
      d->holdsem = new Semaphore( 0 );
      d->endsem = new Semaphore( 0 );
    }
}


carto::LoopThread::~LoopThread()
{

#ifdef CARTO_DEBUG

  std::cerr << "LoopThread::~LoopThread()" << std::endl;

#endif

  delete d;
}


void carto::LoopThread::doStart()
{

  _parent->increaseWorkerCount();

}


void carto::LoopThread::doRun()
{

  int startIndex = 0;
  int countIndex = 0;
  bool single = _parent->endThreadsWhenDone();

  do
    {
      if( !single )
        {
          d->holdsem->wait(); // wait for start signal
          if( finished() )
            break;
        }

      // regular loop (one run)
      while ( _parent->getStartIndexAndCount( startIndex, countIndex ) )
        {

          _parent->getLoopContext()->doIt( startIndex, countIndex );

        }
      if( !single )
        d->endsem->post(); // signal the loop I'm done
    }
  while( !single );

}


void carto::LoopThread::doStop()
{

  _parent->decreaseWorkerCount();

}


bool carto::LoopThread::finished() const
{
  if( d )
    return d->finished;
  return true;
}


void carto::LoopThread::setFinished( bool x )
{
  if( d )
    d->finished = x;
}


carto::Semaphore* carto::LoopThread::holdSemaphore()
{
  if( d )
    return d->holdsem;
  return 0;
}


carto::Semaphore* carto::LoopThread::endSemaphore()
{
  if( d )
    return d->endsem;
  return 0;
}

#endif
