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

#include <pyaims/vector/numpyarrayfunc.h>
#include <vector>
#include <iostream>
using namespace std;

namespace
{

  PyObject* transposeNumpyArray( PyObject* a, bool xyzorder )
  {
    PyArrayObject *arr = (PyArrayObject *) a;
    int i, ndim = PyArray_NDIM( arr );
    if( ndim < 2 )
      return a;
    bool firstindexinc = ( PyArray_STRIDES( arr )[0]
        < PyArray_STRIDES( arr )[1] );
    if( xyzorder == firstindexinc )
      return a;
    PyArray_Dims  adims;
    vector<npy_intp> dims( ndim );
    for( i=0; i<ndim; ++i )
      dims[i] = ndim - 1 - i;
    adims.ptr = &dims[0];
    adims.len = ndim;
    PyObject *sipRes = PyArray_Transpose( arr, &adims );
    Py_DECREF( a );
    return sipRes;
  }

}

namespace aims
{

  PyObject* initNumpyArray( PyObject* sipSelf, NPY_TYPES numType, int ndim,
                            int* dims, char* buffer, bool xyzorder,
                            size_t *strides )
  {
    PyObject *sipRes = 0;
    // if the object has been built from an existing array, just return it
    if( PyObject_HasAttrString( sipSelf, "_arrayext" ) )
    {
      PyObject *arr = PyObject_GetAttrString( sipSelf, "_arrayext" );
      if( arr )
        sipRes = transposeNumpyArray( arr, xyzorder );
    }
    // else look if an array has already been built on the object
    else if( PyObject_HasAttrString( sipSelf, "_arrayref" ) )
    {
      PyObject *wr = PyObject_GetAttrString( sipSelf, "_arrayref" );
      if( wr )
      {
        PyObject *arr = PyWeakref_GetObject( wr );
        if( arr )
        {
          Py_INCREF( arr );
          sipRes = transposeNumpyArray( arr, xyzorder );
        }
        else
        {
          std::cerr << "weakref on deleted object" << std::endl;
          PyObject_DelAttrString( sipSelf, "_arrayref" );
        }
        Py_DECREF( wr );
      }
    }

    if( !sipRes )
    {
      std::vector<npy_intp> dimsp( ndim );
      for( int i=0; i<ndim; ++i )
        dimsp[i] = dims[i];
      sipRes = PyArray_SimpleNewFromData( ndim, &dimsp[0], numType, buffer );
      //sipRes = PyArray_FromDimsAndData( ndim, dims, numType, buffer );
      if( sipRes )
      {
        if( strides )
        {
          // if strides are specified, force them (volume views...)
          PyArray_STRIDES( sipRes )[0] = strides[0];
          PyArray_STRIDES( sipRes )[1] = strides[1];
          PyArray_STRIDES( sipRes )[2] = strides[2];
          PyArray_STRIDES( sipRes )[3] = strides[3];
        }
        sipRes = PyArray_Return( (PyArrayObject *) sipRes );
        // make a weakref to the array with a deletion callback
        PyObject *cbk = PyObject_GetAttrString( sipSelf,
            "_arrayDestroyedCallback" );
        if( !cbk )
        {
          std::cerr << "warning: callback method _arrayDestroyedCallback "
              "not found" << std::endl;
        }
        else
        {
          PyObject *wr = PyWeakref_NewRef( sipRes, cbk );
          if( wr )
          {
            if( PyObject_SetAttrString( sipSelf, "_arrayref", wr )
                == -1 )
            {
              std::cerr << "cannot set object ._arrayref" << std::endl;
            }
            // cbk holds a ref to self until the array is destroyed
            // so self will not be destroyed before the array
            Py_DECREF( wr );
            // increment refcount to self so it doesn't get destroyed until
            // the array is destroyed
            Py_INCREF( sipSelf );
          }
          else
            std::cerr << "could not make a weakref to the array"
                << std::endl;
          Py_DECREF( cbk );
        }
        sipRes = transposeNumpyArray( sipRes, xyzorder );
      }
    }

    return sipRes;
  }


  void resizeNumpyArray( PyObject* sipSelf, int ndim, int* dims, char* buffer,
    size_t *strides )
  {
    if( !PyObject_HasAttrString( sipSelf, "_arrayref" ) )
      return;
    PyObject *wr = PyObject_GetAttrString( sipSelf, "_arrayref" );
    if( !wr )
      return;
    PyObject *o = PyWeakref_GetObject( wr );
    if( !PyArray_Check( o ) )
      return;
    PyArrayObject *arr = (PyArrayObject *) o;
    // PyArray_NDIM( arr ) = ndim;
    PyArray_BYTES( arr ) = buffer;
    if( PyArray_NDIM( arr ) != ndim )
    {
      PyArray_NDIM( arr ) = ndim;
      free( PyArray_DIMS( arr ) );
      PyArray_DIMS( arr ) = PyDimMem_RENEW( PyArray_DIMS( arr ), ndim*2 );
    }
    int i;
    if( strides )
    {
      for( i=0; i<ndim; ++i )
      {
        PyArray_DIMS( arr )[i] = dims[i];
        PyArray_STRIDES( arr )[i] = strides[i];
      }
    }
    else
    {
      for( i=ndim-1; i>=0; --i )
      {
        PyArray_DIMS( arr )[i] = dims[i];
        if( i == ndim-1 )
          PyArray_STRIDES( arr )[i] = PyArray_ITEMSIZE( arr );
        else
          PyArray_STRIDES( arr )[i] = PyArray_STRIDES( arr )[i+1] * dims[i+1];
      }
    }
  }

}

