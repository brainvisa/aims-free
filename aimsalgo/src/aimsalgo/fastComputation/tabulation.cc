
#include <aims/fastComputation/tabulation.h>

using namespace std;

namespace aims {

RealToRealFunctionTabulation::RealToRealFunctionTabulation( 
  const RealToRealFunctionTabulation::function_type function,
  const double &minimum,
  const double &maximum,
  const ulong tableSize ) :
  _minimum( minimum ),
  _sampling( ( maximum - minimum ) / (tableSize-1) ),
  _table( tableSize )
{
  for( ulong i = 0; i < _table.size(); ++i ) {
    _table[ i ] = function( i * _sampling + _minimum );
  }
}

} // namespace aims
