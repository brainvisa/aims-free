#ifndef PYAIMS_DATA_SIPDATATYPES_H
#define PYAIMS_DATA_SIPDATATYPES_H

inline void* pyaimsConvertTo_Volume_S16( PyObject* obj )
{
  int sipIsErr = 0;
  void *ptr = sipForceConvertToType( obj, sipType_Volume_S16, 0, 0, 0,
                                     &sipIsErr );
  if( sipIsErr )
    return 0;
  return ptr;
}

#endif



