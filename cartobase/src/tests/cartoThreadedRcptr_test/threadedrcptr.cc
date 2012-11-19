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
#include <cartobase/config/cartobase_config.h>
#ifndef CARTO_NO_BOOST

namespace boost
{

  template <typename T> void intrusive_ptr_add_ref( T *rc );
  template <typename T> void intrusive_ptr_release( T *rc );
}

#include <boost/smart_ptr.hpp>
using namespace boost;
#include <cartobase/smart/rcptrtrick.h>
#endif
#include <cartobase/smart/rcptr.h>
#include <cartobase/thread/loopContext.h>
#include <cartobase/thread/threadedLoop.h>
#include <cartobase/thread/thread.h>
#include <cartobase/thread/cpu.h>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <sys/time.h>

using namespace carto;
using namespace std;

class Type
{
public:
  Type( unsigned n )
  { x = n; }
  int x;
};


class RcType : public RCObject
{
public:
  RcType( unsigned n )
  { x = n; }
  int x;
};


template <typename rcptr> class rcptr_type
{
public:
  typedef rcptr type;
  static long use_count( type & x );
};

template <typename T> class rcptr_type<rc_ptr<T> >
{
public:
  typedef rc_ptr<T> type;
  typedef T itemtype;
  static long use_count( type & x ) { return x.refCount(); }
  static void reset( type & x ) { x.reset(); }
};

#ifndef CARTO_NO_BOOST
namespace boost
{

  template <>
  inline void intrusive_ptr_add_ref( RcType *rc )
  {
    ++rc_ptr_trick::refCount( *rc );
  }

  template <>
  inline void intrusive_ptr_release( RcType *rc )
  {
    if( !--rc_ptr_trick::refCount( *rc ) )
      delete rc;
  }

}

template <typename T> class rcptr_type<boost::shared_ptr<T> >
{
public:
  typedef boost::shared_ptr<T> type;
  typedef T itemtype;
  static long use_count( type & x ) { return x.use_count(); }
  static void reset( type & x ) { x.reset(); }
};

template <typename T> class rcptr_type<intrusive_ptr<T> >
{
public:
  typedef intrusive_ptr<T> type;
  typedef T itemtype;
  static long use_count( type & x )
  { return rc_ptr_trick::refCount( *x.get() ); }
  static void reset( type & x ) { x = 0; }
};
#endif


// global variables for thread access
unsigned      maxuse = 30*25, nptrs = 2, blocksz = 100;
int           testtime = 5;
template <typename rcptr>
vector<vector<typename rcptr_type<rcptr>::type > > & ptrs()
{
  static vector<vector<typename rcptr_type<rcptr>::type > > x( nptrs );
  return x;
}
// vector<long long> thcnt( cpuCount() * 2 );

template <typename rcptr>
class FooContext : public LoopContext
{
public:
  FooContext() : LoopContext() {}
  ~FooContext() {}

  void doIt( int startIndex, int count )
  {
    /* ++thcnt[cpuCount()*2*startIndex/maxuse];
    lock();
    cout << "startIndex: " << startIndex << ", count: " << count << endl 
         << flush;
    unlock(); */
    for( unsigned n=0; n<blocksz; ++n )
      for( int i = startIndex; i < startIndex + count; ++i )
      {
        unsigned j;
        for( j=0; j<nptrs; ++j )
          ptrs<rcptr>()[j][i] = ptrs<rcptr>()[j][0];
        for( j=0; j<nptrs; ++j )
          rcptr_type<rcptr>::reset( ptrs<rcptr>()[j][i] );
      }
    /* lock();
    cout << "end: " << startIndex << ", count: " << count << endl << flush;
    unlock(); */
  }
};

#ifdef _WIN32
// MingW32 doesn't implement gettimeofday or other precise timers
typedef clock_t TimerType;
#else
typedef struct timeval TimerType;
#endif

TimerType getTime()
{
#ifdef _WIN32
  return clock();
#else
  struct timeval tv;
  struct timezone tz;
  gettimeofday( &tv, &tz );
  return tv;
#endif
}

float elapsedTime( const TimerType & t0 )
{
#ifdef _WIN32
 return float( clock() - t0 ) / CLOCKS_PER_SEC;
#else
 struct timeval tv;
 struct timezone tz;
 gettimeofday( &tv, &tz );
 return float( tv.tv_sec - t0.tv_sec )
     + 0.000001 * ( tv.tv_usec - t0.tv_usec );
#endif
}


template <typename rcptr>
bool testthread( int tpcpu )
{
  long long     ntest = 0;
  TimerType  	t0;
  float			ck2;
  int           ncpu = cpuCount();
  unsigned      j;

  /* for( int i=0; i<ncpu*2; ++i )
    thcnt[i] = 0; */
  FooContext<rcptr> fooContext;
  ThreadedLoop
    threadedLoop( &fooContext,
                  maxuse / (ncpu * tpcpu),  // iterations per thread
                  1,  // the start index of the global loop
                  maxuse-1, // the count to be done
                  tpcpu   // 2 threads are launched on each CPU
                );
  threadedLoop.setEndThreadsWhenDone( false );

  t0 = getTime();

  do
  {
    threadedLoop.setLoopCount( maxuse-1 );
    threadedLoop.launch(); // start the loop process
    ++ntest;
    ck2 = elapsedTime( t0 );
  }
  while( ck2 < testtime );
  cout << ntest << " x " << nptrs << " elements x " << maxuse * blocksz
       << " copies (" << ncpu << " CPUs x " << tpcpu << " threads) in "
       << ck2 << "s : " << ntest * nptrs * maxuse * blocksz / ck2
       << " copies/s" << endl;
  /*cout << "(performance loss is normal here since we only do concurrent " 
          "access)" << endl; */
  /* cout << "counts by thread:" << endl;
  for( int i=0; i<ncpu*2; ++i )
    cout << thcnt[i] << " ";
  cout << endl; */

  cout << endl;

  for( j=0; j<nptrs; ++j )
    if( rcptr_type<rcptr>::use_count( ptrs<rcptr>()[j][0] ) != 1 )
    {
      cerr << "*** error in ref counter ***" << endl;
      cerr << "ptr " << j << ", count: "
           << rcptr_type<rcptr>::use_count( ptrs<rcptr>()[j][0] ) << endl;
      return false;
    }
  return true;
}


template <typename rcptr>
bool monothreadedTest()
{
  unsigned      i, j;
  long long     ntest = 0;
  for( i=0; i<nptrs; ++i )
    ptrs<rcptr>()[i] = vector<rcptr>( maxuse );

  for( i=0; i<nptrs; ++i )
    ptrs<rcptr>()[i][0]
        = rcptr( new typename rcptr_type<rcptr>::itemtype( i ) );

  TimerType	t0;
  float		ck2;

  t0 = getTime();

  do
  {
    for( unsigned n=0; n<blocksz; ++n )
    {
      for( i=1; i<maxuse; ++i )
        for( j=0; j<nptrs; ++j )
          ptrs<rcptr>()[j][i] = ptrs<rcptr>()[j][0];
      for( i=1; i<maxuse; ++i )
        for( j=0; j<nptrs; ++j )
          rcptr_type<rcptr>::reset( ptrs<rcptr>()[j][i] );
    }
    ++ntest;
	ck2 = elapsedTime( t0 );
  }
  while( ck2 < testtime );
  cout << ntest << " x " << nptrs << " elements x " << maxuse * blocksz
       << " copies in " << ck2 << "s : " 
       << ntest * nptrs * maxuse * blocksz / ck2 << " copies/s" << endl;
  cout << endl;

  for( j=0; j<nptrs; ++j )
    if( rcptr_type<rcptr>::use_count( ptrs<rcptr>()[j][0] ) != 1 )
    {
      cerr << "*** error in ref counter ***" << endl;
      cerr << "ptr " << j << ", count: "
           << rcptr_type<rcptr>::use_count( ptrs<rcptr>()[j][0] ) << endl;
      return false;
    }
  return true;
}


int main()
{
#ifndef CARTO_THREADED_RCPTR
  cout << "Warning: thread-safe support for carto::rc_ptr is disabled" << endl;
  cout << "Expect problems and crashes in the threaded tests." << endl;
  cout << endl;
#endif

  int retcode = EXIT_SUCCESS;

  cout << "/-- carto::rc_ptr (external counter) tests --/" << endl;
  cout << "-- Test 1: mono-threaded speed test --" << endl;
  if( !monothreadedTest<rc_ptr<Type> >() )
    retcode = EXIT_FAILURE;

  cout << "-- Test 2: 2 threads/CPU --" << endl;
  if( !testthread<rc_ptr<Type> >( 2 ) )
    retcode = EXIT_FAILURE;

  cout << "-- Test 3: 1 thread/CPU --" << endl;
  if( !testthread<rc_ptr<Type> >( 1 ) )
    retcode = EXIT_FAILURE;

  cout << "/-- carto::rc_ptr (intrusive counter) tests --/" << endl;
  cout << "-- Test 4: mono-threaded speed test --" << endl;
  if( !monothreadedTest<rc_ptr<RcType> >() )
    retcode = EXIT_FAILURE;

  cout << "-- Test 5: 2 threads/CPU --" << endl;
  if( !testthread<rc_ptr<RcType> >( 2 ) )
    retcode = EXIT_FAILURE;

  cout << "-- Test 6: 1 thread/CPU --" << endl;
  if( !testthread<rc_ptr<RcType> >( 1 ) )
    retcode = EXIT_FAILURE;

#ifndef CARTO_NO_BOOST
  cout << "/-- boost::shared_ptr (external counter) tests --/" << endl;
  cout << "-- Test 7: mono-threaded speed test --" << endl;
  if( !monothreadedTest<boost::shared_ptr<Type> >() )
    retcode = EXIT_FAILURE;

  cout << "-- Test 8: 2 threads/CPU --" << endl;
  if( !testthread<boost::shared_ptr<Type> >( 2 ) )
    retcode = EXIT_FAILURE;

  cout << "-- Test 9: 1 thread/CPU --" << endl;
  if( !testthread<boost::shared_ptr<Type> >( 1 ) )
    retcode = EXIT_FAILURE;

  cout << "/-- boost::intrusive_ptr (intrusive counter) tests --/" << endl;
  cout << "-- Test 10: mono-threaded speed test --" << endl;
  if( !monothreadedTest<intrusive_ptr<RcType> >() )
    retcode = EXIT_FAILURE;

  cout << "-- Test 11: 2 threads/CPU --" << endl;
  if( !testthread<intrusive_ptr<RcType> >( 2 ) )
    retcode = EXIT_FAILURE;

  cout << "-- Test 12: 1 thread/CPU --" << endl;
  if( !testthread<intrusive_ptr<RcType> >( 1 ) )
    retcode = EXIT_FAILURE;
#endif

  return retcode;
}

