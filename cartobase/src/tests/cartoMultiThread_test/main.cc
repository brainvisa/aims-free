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


#include <cartobase/thread/loopContext.h>
#include <cartobase/thread/threadedLoop.h>
#include <cartobase/thread/thread.h>
#include <iostream>
#include <unistd.h>
#ifdef _WIN32
#include <windows.h>
#endif

using namespace carto;

class FooContext : public LoopContext
{

  public:

    FooContext( /* put here the parameters of the algorithm */ )
              : LoopContext() { }
    ~FooContext() { }

    void doIt( int startIndex, int count )
    {

      for ( int i = startIndex; i < startIndex + count; i++ )
      {
#ifdef _WIN32
        Sleep( 1000 );
#else
        sleep( 1 );
#endif
	lock();
        std::cerr << " index = " << i
                  << std::endl;
	unlock();

      }

    }

  private:

    // private parameters of the algorithm

};


int main( int /*argc*/, char* /*argv*/[] )
{


  FooContext fooContext/* (my parameters for the algorithm )*/;
  ThreadedLoop
    threadedLoop( &fooContext,
                  5,  // each thread does 5 iterations at once, if it can
                  3,  // the start index of the global loop
                  15, // the count to be done
                  2   // 2 threads are launched on each CPU
                );

  // That will give, for instance, on a single processor computer:
  // 1 thread does iterations i=3,4,5,6,7.
  // Parallely, 1 thread does iterations i=8,9,10,11,12.
  // The first of the threads that has finished realizes the 5 last iterations
  // i=13,14,15,16,17.
  // As a result, 2 threads are created on that single-processor computer.
  
  // As for a computer doted with 2 processors:
  // 3 threads are created; 2 threads on the processor I that deals with
  // iterations i=3,4,5,6,7 and i=13,14,15,16,17, and 1 thread on 
  // processor II that deals with iterations i=8,9,10,11,12
  
  // Typically, one should use 1 thread per CPU, and one should use a unitary
  // grain in image processing algorithms; so we often have to implement
  // a threaded loop of the following type:
  //   pguitk::ThreadedLoop loop( &myAlgoContext, 1, startIter, count, 1 );
		   
  threadedLoop.launch(); // start the loop process

  return 1;

}
