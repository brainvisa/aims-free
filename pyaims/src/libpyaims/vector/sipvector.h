/* Copyright (c) 1995-2006 CEA
 *
 *  This software and supporting documentation were developed by
 *      CEA/DSV/SHFJ
 *      4 place du General Leclerc
 *      91401 Orsay cedex
 *      France
 *
 * This software is governed by the CeCILL license version 2 under 
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the 
 * terms of the CeCILL license version 2 as circulated by CEA, CNRS
 * and INRIA at the following URL "http://www.cecill.info". 
 * 
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability. 
 * 
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or 
 * data to be ensured and,  more generally, to use and operate it in the 
 * same conditions as regards security. 
 * 
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL license version 2 and that you accept its terms.
 */

#ifndef PYAIMS_VECTOR_SIPVECTOR_H
#define PYAIMS_VECTOR_SIPVECTOR_H

template <typename T, int D>
inline sipWrapperType* sipClass_AimsVector();

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
  return sipConvertFromInstance( p, (sipClass_AimsVector<T,D>()), 0 );
}

/* inline PyObject *pyaimsConvertFrom_AimsVector_U32_4( AimsVector_U32_4 * p )
   { return pyaimsConvertFrom_AimsVector( p ); } */


template <typename T, int D>
inline AimsVector<T, D> *pyaimsConvertTo_AimsVector( PyObject* p )
{
  int	iserr = 0;
  return (AimsVector<T,D> *) 
    sipConvertToInstance( p, (sipClass_AimsVector<T,D>()), 0, 
                          SIP_NO_CONVERTORS, 0, &iserr );
}


template <typename T, int D>
inline bool pyaimsCheck_AimsVector( PyObject* o )
{
  return sipCanConvertToInstance( o, (sipClass_AimsVector<T,D>()),
                                  SIP_NOT_NONE | SIP_NO_CONVERTORS );
}

#endif

