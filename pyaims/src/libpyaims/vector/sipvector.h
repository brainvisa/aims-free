#ifndef PYAIMS_VECTOR_SIPVECTOR_H
#define PYAIMS_VECTOR_SIPVECTOR_H

template <typename T, int D>
inline sipWrapperType* sipClass_AimsVector();

template <typename T, int D>
inline const sipTypeDef* sipType_AimsVector();

/*
template <>
inline sipWrapperType* sipClass_AimsVector<int16_t, 2>()
{
  return sipClass_AimsVector_S16_2;
}
template <>
inline sipWrapperType* sipClass_AimsVector<int16_t, 3>()
{
  return sipClass_AimsVector_S16_3;
}
template <>
inline sipWrapperType* sipClass_AimsVector<uint32_t, 2>()
{
  return sipClass_AimsVector_U32_2;
}
template <>
inline sipWrapperType* sipClass_AimsVector<uint32_t, 3>()
{
  return sipClass_AimsVector_U32_3;
}
template <>
inline sipWrapperType* sipClass_AimsVector<uint32_t, 4>()
{
  return sipClass_AimsVector_U32_4;
}
template <>
inline sipWrapperType* sipClass_AimsVector<float, 2>()
{
  return sipClass_AimsVector_FLOAT_2;
}
template <>
inline sipWrapperType* sipClass_AimsVector<float, 3>()
{
  return sipClass_AimsVector_FLOAT_3;
}
*/


template <typename T, int D>
inline PyObject *pyaimsConvertFrom_AimsVector( AimsVector<T,D> * p )
{
  return sipConvertFromType( p, (sipType_AimsVector<T,D>()), 0 );
}

/* inline PyObject *pyaimsConvertFrom_AimsVector_U32_4( AimsVector_U32_4 * p )
   { return pyaimsConvertFrom_AimsVector( p ); } */


template <typename T, int D>
inline AimsVector<T, D> *pyaimsConvertTo_AimsVector( PyObject* p )
{
  int	iserr = 0;
  return (AimsVector<T,D> *) 
    sipConvertToType( p, (sipType_AimsVector<T,D>()), 0,
                      0, 0, &iserr );
}


template <typename T, int D>
inline bool pyaimsCheck_AimsVector( PyObject* o )
{
  return sipCanConvertToType( o, (sipType_AimsVector<T,D>()),
                              SIP_NOT_NONE | SIP_NO_CONVERTORS );
}

#endif

