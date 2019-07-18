
#ifndef PYAIMS_VECTOR_NUMPYTYPES_D_H
#define PYAIMS_VECTOR_NUMPYTYPES_D_H

#include <pyaims/vector/numpytypes.h>
#include <iostream>
#include <sstream>
#include <cstdio>

namespace
{

  template <typename T> std::string npy_typecode()
  {
    return "u1";
  }


  template <> std::string npy_typecode<uint8_t>()
  {
    return "u1";
  }


  template <> std::string npy_typecode<int8_t>()
  {
    return "i1";
  }


  template <> std::string npy_typecode<uint16_t>()
  {
    return "u2";
  }


  template <> std::string npy_typecode<int16_t>()
  {
    return "i2";
  }


  template <> std::string npy_typecode<uint32_t>()
  {
    return "u4";
  }


  template <> std::string npy_typecode<int32_t>()
  {
    return "i4";
  }


  template <> std::string npy_typecode<uint64_t>()
  {
    return "u8";
  }


  template <> std::string npy_typecode<int64_t>()
  {
    return "i8";
  }


  template <> std::string npy_typecode<float>()
  {
    return "f4";
  }


  template <> std::string npy_typecode<double>()
  {
    return "f8";
  }

}


template <typename T, int D> PyArray_Descr* NPY_AimsVector_Descr()
{
  static PyArray_Descr* descr = 0;
  if( !descr )
  {
    PyObject *op;

    std::string tcode = npy_typecode<T>();
    std::stringstream tval;
    tval << D << tcode;

    op = Py_BuildValue( "[(s, s)]", "v", tval.str().c_str() );

    PyArray_DescrConverter( op, &descr );
    Py_DECREF( op );
  }

  return descr;
}


template <typename T, int D> int NPY_AimsVector()
{
  return NPY_AimsVector_Descr<T, D>()->type_num;
}


#endif
