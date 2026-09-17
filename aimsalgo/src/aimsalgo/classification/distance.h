

#ifndef DISTANCE_H
#define DISTANCE_H

#include <vector>
#include <aims/vector/vector.h>

namespace aims{
  template <class T>
    class Distance{
    public:
    Distance() ;
    ~Distance() ;
    
    static float norm1( const std::vector<T>& ind1, const std::vector<T>& ind2, 
			unsigned int beginIndex, unsigned int endIndex ) ;
    static float norm2( const std::vector<T>& ind1, const std::vector<T>& ind2, 
			unsigned int beginIndex, unsigned int endIndex ) ;
    static float norm2sqr( const std::vector<T>& ind1, const std::vector<T>& ind2, 
			   unsigned int beginIndex, unsigned int endIndex ) ;
    static float infiniteNorm( const std::vector<T>& ind1, const std::vector<T>& ind2, 
			       unsigned int beginIndex, unsigned int endIndex ) ;
  } ;
}

#endif
