#ifndef PYAIMS_SIPSET_H
#define PYAIMS_SIPSET_H

#include <set>

template <typename T>
inline sipWrapperType* sipClass_set();

template <typename T>
inline const sipTypeDef* sipType_set();


template <typename T>
inline PyObject *pyaimsConvertFrom_set( std::set<T> * p )
{
  return sipConvertFromType( p, sipType_set<T>(), 0 );
}


template <typename T>
inline std::set<T> *pyaimsConvertTo_set( PyObject* p )
{
  int	iserr = 0;
  return (std::set<T> *)
    sipConvertToType( p, sipType_set<T>(), 0,
                      SIP_NO_CONVERTORS, 0, &iserr );
}


template <typename T>
inline bool pyaimsCheck_set( PyObject* o )
{
  return sipCanConvertToType( o, (sipType_set<T>()),
                              SIP_NOT_NONE | SIP_NO_CONVERTORS );
}

#endif

