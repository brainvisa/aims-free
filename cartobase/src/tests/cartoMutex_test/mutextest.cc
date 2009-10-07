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


#include <cstdlib>
#include <cartobase/thread/thread.h>
#include <cartobase/thread/mutex.h>
#include <iostream>
#include <unistd.h>
#include <stdlib.h>

using namespace carto;
using namespace std;

Mutex mutex;

#ifdef _WIN32
void sleep( unsigned ns )
{
  _sleep( ns * 1000 );
}
#endif

class MyThread : public Thread
{
public:
  MyThread( int id ) : Thread(), _id( id ) {}
  virtual ~MyThread() {}
  virtual void doRun();
  int _id;
};

void MyThread::doRun()
{
  cout << "doRun in thread " << _id << endl << flush;
  sleep( 1 );
  mutex.lock();
  cout << "mutex locked for thread " << _id << endl << flush;
  sleep( 1 );
  cout << "mutex unlocked for thread " << _id << endl << flush;
  mutex.unlock();
}


int main( int /*argc*/, char* /*argv*/[] )
{
  MyThread    t1( 1 ), t2( 2 ), t3( 3 );
  cout << "running threads\n" << flush;
  t1.launch();
  t2.launch();
  t3.launch();
  cout << "(main) threads running...\n" << flush;
  t1.join();
  t2.join();
  t3.join();
  cout << "threads finished\n" << flush;
  return EXIT_SUCCESS;
}

