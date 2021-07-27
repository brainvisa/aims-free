/* This software and supporting documentation are distributed by
 *     Institut Federatif de Recherche 49
 *     CEA/NeuroSpin, Batiment 145,
 *     91191 Gif-sur-Yvette cedex
 *     France
 *
 * This software is governed by the CeCILL-B license under
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the
 * terms of the CeCILL-B license as circulated by CEA, CNRS
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
 * knowledge of the CeCILL-B license and that you accept its terms.
 */

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

