
#ifndef AIMS_UTILITY_STL_CONVERSION
#define AIMS_UTILITY_STL_CONVERSION

#include <map>

namespace aims
{

  /** convert a vector or list into a sparse toMap
  */
  template <typename Vector, typename IndexType>
  inline void toMap(
    const Vector & vector,
    std::map<IndexType, typename Vector::value_type> & map,
    const typename Vector::value_type & background = Vector::value_type( 0 ) )
  {
    typename Vector::const_iterator iv, ev = vector.end();
    IndexType n = 0;
    for( iv=vector.begin(); iv!=ev; ++iv, ++n )
      if( *iv != background )
        map[ n ] = *iv;
  }

}

#endif


