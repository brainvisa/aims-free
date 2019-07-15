#ifndef PYAIMS_VECTOR_NUMPYTYPES
#define PYAIMS_VECTOR_NUMPYTYPES


#include <pyaims/numpyarray.h>

PyArray_Descr* NPY_AimsRGB_Descr();
PyArray_Descr* NPY_AimsRGBA_Descr();
PyArray_Descr* NPY_AimsHSV_Descr();
int NPY_AimsRGB();
int NPY_AimsRGBA();
int NPY_AimsHSV();

template <typename T, int D> PyArray_Descr* NPY_AimsVector_Descr();
template <typename T, int D> int NPY_AimsVector();


extern template PyArray_Descr* NPY_AimsVector_Descr<uint8_t, 2>();
extern template PyArray_Descr* NPY_AimsVector_Descr<uint8_t, 3>();
extern template PyArray_Descr* NPY_AimsVector_Descr<uint8_t, 4>();
extern template PyArray_Descr* NPY_AimsVector_Descr<int8_t, 2>();
extern template PyArray_Descr* NPY_AimsVector_Descr<int8_t, 3>();
extern template PyArray_Descr* NPY_AimsVector_Descr<int8_t, 4>();
extern template PyArray_Descr* NPY_AimsVector_Descr<uint16_t, 2>();
extern template PyArray_Descr* NPY_AimsVector_Descr<uint16_t, 3>();
extern template PyArray_Descr* NPY_AimsVector_Descr<uint16_t, 4>();
extern template PyArray_Descr* NPY_AimsVector_Descr<int16_t, 2>();
extern template PyArray_Descr* NPY_AimsVector_Descr<int16_t, 3>();
extern template PyArray_Descr* NPY_AimsVector_Descr<int16_t, 4>();
extern template PyArray_Descr* NPY_AimsVector_Descr<uint32_t, 2>();
extern template PyArray_Descr* NPY_AimsVector_Descr<uint32_t, 3>();
extern template PyArray_Descr* NPY_AimsVector_Descr<uint32_t, 4>();
extern template PyArray_Descr* NPY_AimsVector_Descr<int32_t, 2>();
extern template PyArray_Descr* NPY_AimsVector_Descr<int32_t, 3>();
extern template PyArray_Descr* NPY_AimsVector_Descr<int32_t, 4>();
extern template PyArray_Descr* NPY_AimsVector_Descr<uint64_t, 2>();
extern template PyArray_Descr* NPY_AimsVector_Descr<uint64_t, 3>();
extern template PyArray_Descr* NPY_AimsVector_Descr<uint64_t, 4>();
extern template PyArray_Descr* NPY_AimsVector_Descr<int64_t, 2>();
extern template PyArray_Descr* NPY_AimsVector_Descr<int64_t, 3>();
extern template PyArray_Descr* NPY_AimsVector_Descr<int64_t, 4>();
extern template PyArray_Descr* NPY_AimsVector_Descr<float, 2>();
extern template PyArray_Descr* NPY_AimsVector_Descr<float, 3>();
extern template PyArray_Descr* NPY_AimsVector_Descr<float, 4>();
extern template PyArray_Descr* NPY_AimsVector_Descr<double, 2>();
extern template PyArray_Descr* NPY_AimsVector_Descr<double, 3>();
extern template PyArray_Descr* NPY_AimsVector_Descr<double, 4>();
extern template int NPY_AimsVector<uint8_t, 2>();
extern template int NPY_AimsVector<uint8_t, 3>();
extern template int NPY_AimsVector<uint8_t, 4>();
extern template int NPY_AimsVector<int8_t, 2>();
extern template int NPY_AimsVector<int8_t, 3>();
extern template int NPY_AimsVector<int8_t, 4>();
extern template int NPY_AimsVector<uint16_t, 2>();
extern template int NPY_AimsVector<uint16_t, 3>();
extern template int NPY_AimsVector<uint16_t, 4>();
extern template int NPY_AimsVector<int16_t, 2>();
extern template int NPY_AimsVector<int16_t, 3>();
extern template int NPY_AimsVector<int16_t, 4>();
extern template int NPY_AimsVector<uint32_t, 2>();
extern template int NPY_AimsVector<uint32_t, 3>();
extern template int NPY_AimsVector<uint32_t, 4>();
extern template int NPY_AimsVector<int32_t, 2>();
extern template int NPY_AimsVector<int32_t, 3>();
extern template int NPY_AimsVector<int32_t, 4>();
extern template int NPY_AimsVector<uint64_t, 2>();
extern template int NPY_AimsVector<uint64_t, 3>();
extern template int NPY_AimsVector<uint64_t, 4>();
extern template int NPY_AimsVector<int64_t, 2>();
extern template int NPY_AimsVector<int64_t, 3>();
extern template int NPY_AimsVector<int64_t, 4>();
extern template int NPY_AimsVector<float, 2>();
extern template int NPY_AimsVector<float, 3>();
extern template int NPY_AimsVector<float, 4>();
extern template int NPY_AimsVector<double, 2>();
extern template int NPY_AimsVector<double, 3>();
extern template int NPY_AimsVector<double, 4>();

#endif

