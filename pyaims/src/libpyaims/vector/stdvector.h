
#ifndef PYAIMS_VECTOR_STDVECTOR_H
#define PYAIMS_VECTOR_STDVECTOR_H

#include <vector>
#include <stdlib.h>

template <typename T>
inline std::vector<T> * pyaimsCopyFrom_Array( T * array, unsigned size )
{
  std::vector<T> * result = new std::vector<T>( array, array + size );

  return result;
}

#if defined( __APPLE__ ) || defined( __LP64__ )
//#pragma message( "===== Defining apple or linux 64 bits vector types" )
typedef std::vector<unsigned long> vector_SIZE_T;

#elif defined( _WIN64 )
//#pragma message( "===== Defining windows 64 bits vector types" )
typedef std::vector<unsigned long long int> vector_SIZE_T;

#else
//#pragma message( "===== Defining 32 bits vector types" )
typedef std::vector<unsigned> vector_SIZE_T;
#endif

#endif

