
#include <pyaims/vector/numpytypes.h>
#include <iostream>
#include <cstdio>

PyArray_Descr* NPY_AimsRGB_Descr()
{
  static PyArray_Descr* descr = 0;
  if( !descr )
  {
    PyObject *op;

    op = Py_BuildValue( "[(s, s), (s, s), (s, s)]",
                        "r", "u1", "g", "u1", "b", "u1" );
    PyObject_Print(op, stdout, 0);
    PyArray_DescrConverter( op, &descr );
    std::cout << "descr for aimsRGB: " << descr << ", type_num: " << descr->type_num << std::endl;
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

    op = Py_BuildValue( "[(s, s), (s, s), (s, s), (s, s)]",
                        "r", "u1", "g", "u1", "b", "u1", "a", "u1" );
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

    op = Py_BuildValue( "[(s, s), (s, s), (s, s)]",
                        "h", "u1", "s", "u1", "v", "u1" );
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


