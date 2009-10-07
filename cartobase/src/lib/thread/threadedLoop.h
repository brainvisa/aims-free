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


#ifndef CARTOBASE_THREAD_THREADEDLOOP_H
#define CARTOBASE_THREAD_THREADEDLOOP_H

#include <cartobase/config/cartobase_config.h>

#ifndef CARTO_NO_THREAD

namespace carto
{

  class LoopContext;


  class ThreadedLoop
  {

  public:

    ThreadedLoop( LoopContext* loopContext,
                  int grain, int startIndex, int count,
                  float threadsByCpu );
    virtual ~ThreadedLoop();

    LoopContext* getLoopContext() const;
    /** This function is only useful in multiple run mode (endThreadsWhenDone 
        false). Call it only between two runs, from the control thread (the 
        thread running the loop).
    */
    void setLoopContext( LoopContext* );
    bool getStartIndexAndCount( int& startIndex, int& count );

    int getTodo() const;

    void increaseWorkerCount();
    void decreaseWorkerCount();
    int getWorkerCount() const;

    bool launch( bool resetGauge = true, bool resetCancel = true );

    /** Warning: it's possibly not safe to change this value from false to true
        after launch() has been called */
    void setEndThreadsWhenDone( bool x );
    /** The loop can work in two modes: single run (endThreadsWhenDone() is 
        true) or multiple runs (endThreadsWhenDone is false).
        - In single run mode, launch() creates the needed number of threads, 
        runs them to complete the loop, then destroys every thread.
        - In multiple runs mode, the first call to launch() creates the needed 
        threads, but doesn't destroy them after the loop is finished. Each 
        thread stays alive and blocks on a semaphore. When you call launch() 
        again later for another run (possibly after changing the LoopContext), 
        the existing threads will be re-used for the next loop, so the cost of 
        destroying and creating threads again is avoided. All threads will be 
        actually destroyed when the ThreadedLoop is deleted.
     */
    bool endThreadsWhenDone() const;
    void setLoopCount( int n );

  private:
    struct Private;

    Private	*d;

    LoopContext* _loopContext;
    int _grain;
    int _startIndex;
    int _count;
    float _threadsByCpu;

    int _currentIndex;
    int _todo;

    int _currentWorkerCount;
  };

}


#endif
#endif
