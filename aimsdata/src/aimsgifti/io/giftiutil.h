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

#ifndef AIMS_IO_GIFTIUTIL_H
#define AIMS_IO_GIFTIUTIL_H

#include <aims/vector/vector.h>
#include <nifti1_io.h>
extern "C"
{
#include <gifti_io.h>
}

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


  template <typename U> inline
  U convertedNiftiValue( void* data, int index, int dtype )
  {
    return _convertedNiftiValue<U>()( data, index, dtype );
  }


  std::string niftiDataType( int dt );
  int niftiIntDataType( const std::string & typecode );
  std::string giftiTextureDataType( int dtype, int & ndim, int* dims,
                                    int intent, int & ntime );

}

#endif
