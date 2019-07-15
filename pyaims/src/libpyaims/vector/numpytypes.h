#ifndef PYAIMS_VECTOR_NUMPYTYPES
#define PYAIMS_VECTOR_NUMPYTYPES


#include <pyaims/numpyarray.h>

PyArray_Descr* NPY_AimsRGB_Descr();
PyArray_Descr* NPY_AimsRGBA_Descr();
PyArray_Descr* NPY_AimsHSV_Descr();
int NPY_AimsRGB();
int NPY_AimsRGBA();
int NPY_AimsHSV();


#endif

