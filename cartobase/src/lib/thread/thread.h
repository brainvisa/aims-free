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


#ifndef CARTOBASE_THREAD_THREAD_H
#define CARTOBASE_THREAD_THREAD_H

#include <cartobase/config/cartobase_config.h>

#ifndef CARTO_NO_THREAD


namespace carto
{


  /**	Short usage:
        - inherit Thread
        - define the doRun() function: this is the threaded job
        - run it by calling launch()
   */
  class Thread
  {

  public:

    Thread();
    virtual ~Thread();

    /// This is the function which should actually called to run a thread
    virtual void launch();

    /// Call launch(), not start()
    virtual void start();
    /// Call launch(), not run()
    virtual void run();
    virtual void stop();
    virtual void reset();

    virtual void join();
    virtual void detach();
    /** In suicide-safe mode, Thread can be deleted (or auto-deleted) while the
        threaded job is still running: doPostLaunch() and stop() are not 
        executed.
        Warning: it doesn't imply the thread is detached (if not, you may have
        to join() it to cleanup).
    */
    void setSuicideSafe( bool x = true );
    bool isSuicideSafe() const;

    virtual void setCancellable( bool value, bool callStop );
    virtual void cancel();

    virtual bool isActive() const;
    virtual bool isCurrent() const;
    static bool currentIsMainThread();

    static void* bootstrap( void* thread );

  protected:

    virtual void testCancel();
    virtual void doStart();
    virtual void doRun() = 0;
    virtual void doStop();
    virtual void doPreLaunch();
    virtual void doPostLaunch();

    bool _detached;
    bool _cancellable;
    bool _callStop;

  private:

    struct Private;
    Private *d;
  };

}


#endif
#endif
