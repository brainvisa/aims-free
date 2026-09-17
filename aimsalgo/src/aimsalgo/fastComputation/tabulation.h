
#ifndef AIMS_FASTCOMPUTATION_TABULATION_H
#define AIMS_FASTCOMPUTATION_TABULATION_H

#include <math.h>
#include <vector>
#include <aims/def/general.h>

namespace aims {

class RealToRealFunctionTabulation
{
public:

  typedef double (*function_type)( const double & );

  RealToRealFunctionTabulation( const function_type function,
                                const double &min,
                                const double &max,
                                const ulong tableSize );

  ulong getIndex( const double & ) const;
  const double &operator ()( const double & ) const;
  const double &operator []( ulong ) const;

protected:

  double _minimum, _sampling;
  std::vector<double> _table;
};


inline const double &
RealToRealFunctionTabulation::operator []( ulong i ) const
{
  return _table[ i ];
}

inline ulong RealToRealFunctionTabulation::getIndex( const double &v ) const
{
  return ulong( rint( ( v - _minimum ) / _sampling ) );
}


inline const double &
RealToRealFunctionTabulation::operator ()( const double &v ) const
{
  return _table[ getIndex( v ) ];
}

} // namespace aims


#endif
