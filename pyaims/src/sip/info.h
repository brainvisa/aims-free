#ifndef PYAIMS_INFO_H
#define PYAIMS_INFO_H

inline PyObject* pyaimsConvertFrom_InfoP( void * a )
{
  return sipConvertFromType( a, sipType_carto_Info, 0 );
}


inline void* pyaimsConvertTo_InfoP( PyObject * o )
{
  int isErr = 0;
  return sipConvertToType( o, sipType_carto_Info, 0, 0, 0, &isErr );
}


inline int pyaimsInfoP_Check( PyObject* o )
{
  return sipCanConvertToType( o, sipType_carto_Info,
                              SIP_NOT_NONE | SIP_NO_CONVERTORS );
}

#endif

