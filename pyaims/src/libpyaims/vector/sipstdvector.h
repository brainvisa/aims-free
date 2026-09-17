#ifndef PYAIMS_SIPSTDVECTOR_H
#define PYAIMS_SIPSTDVECTOR_H

#include <vector>
#include <list>

template <typename T>
inline sipWrapperType* sipClass_vector();

template <typename T>
inline const sipTypeDef* sipType_vector();

template <typename T>
inline sipWrapperType* sipClass_list();

template <typename T>
inline const sipTypeDef* sipType_list();


template <typename T>
inline PyObject *pyaimsConvertFrom_vector( std::vector<T> * p )
{
  return sipConvertFromType( p, sipType_vector<T>(), 0 );
}


template <typename T>
inline PyObject *pyaimsConvertFrom_list( std::list<T> * p )
{
  return sipConvertFromType( p, sipType_list<T>(), 0 );
}


template <typename T>
inline std::vector<T> *pyaimsConvertTo_vector( PyObject* p )
{
  int	iserr = 0;
  return (std::vector<T> *)
    sipConvertToType( p, sipType_vector<T>(), 0,
                      /*SIP_NO_CONVERTORS*/ 0, 0, &iserr );
}


template <typename T>
inline std::list<T> *pyaimsConvertTo_list( PyObject* p )
{
  int	iserr = 0;
  return (std::list<T> *)
    sipConvertToType( p, sipType_list<T>(), 0,
                      /*SIP_NO_CONVERTORS*/ 0, 0, &iserr );
}


template <typename T>
inline bool pyaimsCheck_vector( PyObject* o )
{
  return sipCanConvertToType( o, (sipType_vector<T>()),
                              SIP_NOT_NONE | SIP_NO_CONVERTORS );
}


template <typename T>
inline bool pyaimsCheck_list( PyObject* o )
{
  return sipCanConvertToType( o, (sipType_list<T>()),
                              SIP_NOT_NONE | SIP_NO_CONVERTORS );
}

#endif

