#ifndef AIMS_IO_GIFTIUTIL_H
#define AIMS_IO_GIFTIUTIL_H

#include <aims/vector/vector.h>
#include <aims/io/gifti.h>

namespace aims
{

  template <typename U>
  class _convertedNiftiValue // functor to allow partial specialization
  {
  public:
    inline U operator () ( void* data, int index, int dtype )
    {
      switch( dtype )
      {
        case NIFTI_TYPE_UINT8:
          return (U) reinterpret_cast<uint8_t *>(data)[index];
        case NIFTI_TYPE_INT16:
          return (U) reinterpret_cast<int16_t *>(data)[index];
        case NIFTI_TYPE_INT32:
          return (U) reinterpret_cast<int32_t *>(data)[index];
        case NIFTI_TYPE_FLOAT32:
          return (U) reinterpret_cast<float *>(data)[index];
        case NIFTI_TYPE_FLOAT64:
          return (U) reinterpret_cast<double *>(data)[index];
        case NIFTI_TYPE_INT8:
          return (U) reinterpret_cast<int8_t *>(data)[index];
        case NIFTI_TYPE_UINT16:
          return (U) reinterpret_cast<uint16_t *>(data)[index];
        case NIFTI_TYPE_UINT32:
          return (U) reinterpret_cast<uint32_t *>(data)[index];
        case NIFTI_TYPE_INT64:
          return (U) reinterpret_cast<int64_t *>(data)[index];
        case NIFTI_TYPE_UINT64:
          return (U) reinterpret_cast<uint64_t *>(data)[index];
        default:
          return U();
      }
    }
  };


  template <typename U, int D>
  class _convertedNiftiValue<AimsVector<U,D> >
  {
  public:
    inline AimsVector<U,D> operator () ( void* data, int index, int dtype )
    {
      AimsVector<U,D> v;
      int i;
      for( i=0; i<D; ++i )
        v[i] = _convertedNiftiValue<U>()( data, index * D + i, dtype );
      return v;
    }
  };


  template <typename U>
  class _convertedNiftiArrayValue // functor to allow partial specialization
  {
  public:
    inline U operator () ( void* data, int index, int dtype, size_t )
    {
      return _convertedNiftiValue<U>()( data, index, dtype );
    }
  };


  template <typename U, int D>
  class _convertedNiftiArrayValue<AimsVector<U,D> >
  {
  public:
    inline AimsVector<U,D> operator () ( void* data, int index, int dtype,
                                         size_t size
    )
    {
      AimsVector<U,D> v;
      int i;
      for( i=0; i<D; ++i )
        v[i] = _convertedNiftiValue<U>()( data, index + size * i, dtype );
      return v;
    }
  };


  template <typename U> inline
  U convertedNiftiValue( void* data, int index, int dtype )
  {
    return _convertedNiftiValue<U>()( data, index, dtype );
  }


  template <typename U> inline
  U convertedNiftiArrayValue( void* data, int index, int dtype, size_t size )
  {
    return _convertedNiftiArrayValue<U>()( data, index, dtype, size );
  }


  std::string niftiDataType( int dt );
  int niftiIntDataType( const std::string & typecode );
  std::string giftiTextureDataType( int dtype, int & ndim, int* dims,
                                    int intent, int & ntime );

}

#endif
