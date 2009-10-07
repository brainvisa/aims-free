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

#include <aims/io/giftiutil.h>
#include <aims/resampling/standardreferentials.h>

using namespace carto;
using namespace std;

namespace aims
{

  string niftiDataType( int dt )
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


  int niftiIntDataType( const string & typecode )
  {
    if( typecode == "U8" )
      return NIFTI_TYPE_UINT8;
    else if( typecode == "S16" )
      return NIFTI_TYPE_INT16;
    else if( typecode == "S32" )
      return NIFTI_TYPE_INT32;
    else if( typecode == "FLOAT" )
      return NIFTI_TYPE_FLOAT32;
    else if( typecode == "CFLOAT" )
      return NIFTI_TYPE_COMPLEX64;
    else if( typecode == "DOUBLE" )
      return NIFTI_TYPE_FLOAT64;
    else if( typecode == "RGB" )
      return NIFTI_TYPE_RGB24;
    else if( typecode == "S8" )
      return NIFTI_TYPE_INT8;
    else if( typecode == "U16" )
      return NIFTI_TYPE_UINT16;
    else if( typecode == "U32" )
      return NIFTI_TYPE_UINT32;
    else if( typecode == "S64" )
      return NIFTI_TYPE_INT64;
    else if( typecode == "U64" )
      return NIFTI_TYPE_UINT64;
    else if( typecode == "FLOAT128" )
      return NIFTI_TYPE_FLOAT128;
    else if( typecode == "CDOUBLE" )
      return NIFTI_TYPE_COMPLEX128;
    else if( typecode == "CFLOAT128" )
      return NIFTI_TYPE_COMPLEX256;
    else if( typecode == "RGBA" )
      return NIFTI_TYPE_RGBA32;
    else
      return DT_NONE;
  }


  string giftiTextureDataType( int dtype, int & ndim, int* dims, int intent,
                               int & nt )
  {
    string elemtype = niftiDataType( dtype );
    string gdtype = elemtype;
    nt = 1;

    if( ndim < 2 )
      ndim = 1;
    else
    {
      if( intent == NIFTI_INTENT_TIME_SERIES )
      {
        nt = dims[ ndim-1 ];
        --ndim;
        if( ndim == 2 )
        {
          nt = dims[1];
          ndim = 1;
        }
      }
      if( ndim == 2 )
        ndim = dims[1];
      else if( ndim >= 3 )
      {
        gdtype = "volume of " + elemtype;
        ndim = 1;
      }
    }
    switch( ndim )
    {
      case 1:
        break;
      case 2:
        if( intent != NIFTI_INTENT_TIME_SERIES )
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

} // namespace aims

