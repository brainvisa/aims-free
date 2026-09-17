
// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/math/random.h>
#include <cartodata/volume/volume.h>
#include <set>
#include <stack>
#include <stdlib.h>
#include <math.h>

using namespace std;
using namespace carto;

VolumeRef<int> AimsRandomList( int size )
{
  VolumeRef<int> data( size, 1, 1, 1, AllocatorContext::fast() );

  set<int> stack;
  int tmp=0;

  while ( int( stack.size() ) < size )
  {
    tmp = UniformRandom( 0, 2 * size ) % size;
    if ( stack.find( tmp ) == stack.end() )
    {
      data( stack.size() ) = tmp;
      stack.insert( tmp ); 
    }
  }
  return data;
}
