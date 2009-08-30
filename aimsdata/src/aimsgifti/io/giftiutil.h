/* Copyright (c) 2009 CEA
 *
 *  This software and supporting documentation were developed by
 *      CEA/DSV/SHFJ
 *      4 place du General Leclerc
 *      91401 Orsay cedex
 *      France
 *
 * This software is governed by the CeCILL license version 2 under
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the
 * terms of the CeCILL license version 2 as circulated by CEA, CNRS
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
 * knowledge of the CeCILL license version 2 and that you accept its terms.
 */

#ifndef AIMS_IO_GIFTIUTIL_H
#define AIMS_IO_GIFTIUTIL_H

#include <aims/vector/vector.h>
#include <nifti1_io.h>

namespace aims
{

  template <typename U> inline
  U convertedNiftiValue( void* data, int index, int dtype )
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

  template <typename U, int D> inline
  AimsVector<U,D> convertedNiftiValue( void* data, int index, int dtype )
  {
    AimsVector<U,D> v;
    int i;
    for( i=0; i<D; ++i )
      v[i] = convertedNiftiValue<U>( data, index * D + i, dtype );
  }


  inline std::string niftiDataType( int dt )
  {
    switch( dt )
    {
      case NIFTI_TYPE_UINT8:
        return "U8";
      case NIFTI_TYPE_INT16:
        return "S16";
      case NIFTI_TYPE_INT32:
        return "S32";
      case NIFTI_TYPE_FLOAT32:
        return "FLOAT";
      case NIFTI_TYPE_COMPLEX64:
        return "CFLOAT";
      case NIFTI_TYPE_FLOAT64:
        return "DOUBLE";
      case NIFTI_TYPE_RGB24:
        return "RGB";
      case NIFTI_TYPE_INT8:
        return "S8";
      case NIFTI_TYPE_UINT16:
        return "U16";
      case NIFTI_TYPE_UINT32:
        return "U32";
      case NIFTI_TYPE_INT64:
        return "S64";
      case NIFTI_TYPE_UINT64:
        return "U64";
      case NIFTI_TYPE_FLOAT128:
        return "FLOAT128";
      case NIFTI_TYPE_COMPLEX128:
        return "CDOUBLE";
      case NIFTI_TYPE_COMPLEX256:
        return "CFLOAT128";
      case NIFTI_TYPE_RGBA32:
        return "RGBA";
      default:
        return "VOID";
    }
  }


  inline std::string giftiTextureDataType( int dtype, int & ndim, int* dims )
  {
    std::string elemtype = niftiDataType( dtype );
    std::string gdtype = elemtype;

    if( ndim < 2 )
      ndim = 1;
    else if( ndim == 2 )
      ndim = dims[1];
    else if( ndim >= 3 )
    {
      gdtype = "volume of " + elemtype;
      ndim = 1;
    }
    switch( ndim )
    {
      case 1:
        break;
      case 2:
        if( elemtype == "FLOAT" )
          gdtype = "POINT2DF";
        else
          gdtype = "VECTOR_OF_2_" + elemtype;
        break;
      case 3:
        if( elemtype == "FLOAT" )
          gdtype = "POINT3DF";
        else
          gdtype = "VECTOR_OF_3_" + elemtype;
        break;
      default:
      {
        std::ostringstream os;
        os << "VECTOR_OF_" << ndim << "_" << elemtype;
        gdtype = os.str();
      }
    }

    return gdtype;
  }

}

#endif
