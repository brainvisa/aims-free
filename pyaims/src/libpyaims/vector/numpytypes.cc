
#include <pyaims/vector/numpytypes_d.h>
#include <iostream>
#include <cstdio>


PyArray_Descr* NPY_AimsRGB_Descr()
{
  static PyArray_Descr* descr = 0;
  if( !descr )
  {
    PyObject *op;

    op = Py_BuildValue( "[(s, s)]", "v", "3u1" );
    PyArray_DescrConverter( op, &descr );
    Py_DECREF( op );
  }

  return descr;
}


PyArray_Descr* NPY_AimsRGBA_Descr()
{
  static PyArray_Descr* descr = 0;
  if( !descr )
  {
    PyObject *op;

    op = Py_BuildValue( "[(s, s)", "v", "4u1" );
    PyArray_DescrConverter( op, &descr );
    Py_DECREF( op );
  }

  return descr;
}


PyArray_Descr* NPY_AimsHSV_Descr()
{
  static PyArray_Descr* descr = 0;
  if( !descr )
  {
    PyObject *op;

    op = Py_BuildValue( "[(s, s)]", "v", "3u1" );
    PyArray_DescrConverter( op, &descr );
    Py_DECREF( op );
  }

  return descr;
}


int NPY_AimsRGB()
{
  return NPY_AimsRGB_Descr()->type_num;
}


int NPY_AimsRGBA()
{
  return NPY_AimsRGBA_Descr()->type_num;
}


int NPY_AimsHSV()
{
  return NPY_AimsHSV_Descr()->type_num;
}


template PyArray_Descr* NPY_AimsVector_Descr<uint8_t, 2>();
template PyArray_Descr* NPY_AimsVector_Descr<uint8_t, 3>();
template PyArray_Descr* NPY_AimsVector_Descr<uint8_t, 4>();
template PyArray_Descr* NPY_AimsVector_Descr<int8_t, 2>();
template PyArray_Descr* NPY_AimsVector_Descr<int8_t, 3>();
template PyArray_Descr* NPY_AimsVector_Descr<int8_t, 4>();
template PyArray_Descr* NPY_AimsVector_Descr<uint16_t, 2>();
template PyArray_Descr* NPY_AimsVector_Descr<uint16_t, 3>();
template PyArray_Descr* NPY_AimsVector_Descr<uint16_t, 4>();
template PyArray_Descr* NPY_AimsVector_Descr<int16_t, 2>();
template PyArray_Descr* NPY_AimsVector_Descr<int16_t, 3>();
template PyArray_Descr* NPY_AimsVector_Descr<int16_t, 4>();
template PyArray_Descr* NPY_AimsVector_Descr<uint32_t, 2>();
template PyArray_Descr* NPY_AimsVector_Descr<uint32_t, 3>();
template PyArray_Descr* NPY_AimsVector_Descr<uint32_t, 4>();
template PyArray_Descr* NPY_AimsVector_Descr<int32_t, 2>();
template PyArray_Descr* NPY_AimsVector_Descr<int32_t, 3>();
template PyArray_Descr* NPY_AimsVector_Descr<int32_t, 4>();
template PyArray_Descr* NPY_AimsVector_Descr<uint64_t, 2>();
template PyArray_Descr* NPY_AimsVector_Descr<uint64_t, 3>();
template PyArray_Descr* NPY_AimsVector_Descr<uint64_t, 4>();
template PyArray_Descr* NPY_AimsVector_Descr<int64_t, 2>();
template PyArray_Descr* NPY_AimsVector_Descr<int64_t, 3>();
template PyArray_Descr* NPY_AimsVector_Descr<int64_t, 4>();
template PyArray_Descr* NPY_AimsVector_Descr<float, 2>();
template PyArray_Descr* NPY_AimsVector_Descr<float, 3>();
template PyArray_Descr* NPY_AimsVector_Descr<float, 4>();
template PyArray_Descr* NPY_AimsVector_Descr<double, 2>();
template PyArray_Descr* NPY_AimsVector_Descr<double, 3>();
template PyArray_Descr* NPY_AimsVector_Descr<double, 4>();
template int NPY_AimsVector<uint8_t, 2>();
template int NPY_AimsVector<uint8_t, 3>();
template int NPY_AimsVector<uint8_t, 4>();
template int NPY_AimsVector<int8_t, 2>();
template int NPY_AimsVector<int8_t, 3>();
template int NPY_AimsVector<int8_t, 4>();
template int NPY_AimsVector<uint16_t, 2>();
template int NPY_AimsVector<uint16_t, 3>();
template int NPY_AimsVector<uint16_t, 4>();
template int NPY_AimsVector<int16_t, 2>();
template int NPY_AimsVector<int16_t, 3>();
template int NPY_AimsVector<int16_t, 4>();
template int NPY_AimsVector<uint32_t, 2>();
template int NPY_AimsVector<uint32_t, 3>();
template int NPY_AimsVector<uint32_t, 4>();
template int NPY_AimsVector<int32_t, 2>();
template int NPY_AimsVector<int32_t, 3>();
template int NPY_AimsVector<int32_t, 4>();
template int NPY_AimsVector<uint64_t, 2>();
template int NPY_AimsVector<uint64_t, 3>();
template int NPY_AimsVector<uint64_t, 4>();
template int NPY_AimsVector<int64_t, 2>();
template int NPY_AimsVector<int64_t, 3>();
template int NPY_AimsVector<int64_t, 4>();
template int NPY_AimsVector<float, 2>();
template int NPY_AimsVector<float, 3>();
template int NPY_AimsVector<float, 4>();
template int NPY_AimsVector<double, 2>();
template int NPY_AimsVector<double, 3>();
template int NPY_AimsVector<double, 4>();


