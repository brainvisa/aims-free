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


#ifndef AIMS_SIGNALFILTER_NONLINFILT_FUNC_H
#define AIMS_SIGNALFILTER_NONLINFILT_FUNC_H

#include <map>
#include <aims/data/data.h>
#include <algorithm>

template <class T> class AimsData;

//
// class FilterFunc
//
template <class T>
class NonLinFilterFunc
{
  public:

    NonLinFilterFunc() { }
    virtual ~NonLinFilterFunc() { }

    virtual T doit( AimsData<T>& data ) const = 0;    
};


template <class T>
class MaxFilterFunc : public NonLinFilterFunc< T >
{
  public:
    MaxFilterFunc () { }
    virtual ~MaxFilterFunc () { }

    T doit( AimsData<T>& data ) const; 
  
};
 
template <class T> inline
T MaxFilterFunc<T>::doit( AimsData<T>& data ) const
{
  return data.maximum();
}


template <class T>
class MedianFilterFunc : public NonLinFilterFunc< T >
{
  public:
    MedianFilterFunc () { }
    virtual ~MedianFilterFunc () { }

    T doit( AimsData<T>& data ) const; 
  
};
 
template <class T> inline
T MedianFilterFunc<T>::doit( AimsData<T>& data ) const
{
  std::sort( data.begin(), data.end());
  return data( data.dimX() / 2 );
}


template <class T>
class MinFilterFunc : public NonLinFilterFunc< T >
{
  public:
    MinFilterFunc () { }
    virtual ~MinFilterFunc () { }

    T doit( AimsData<T>& data ) const; 
  
};
 
template <class T> inline
T MinFilterFunc<T>::doit( AimsData<T>& data ) const
{
  return data.minimum();
}

template <class T>
class MajorityFilterFunc : public NonLinFilterFunc< T >
{
  public:
    MajorityFilterFunc() { }
    virtual ~MajorityFilterFunc() {}

    T doit( AimsData<T>& data ) const;
};

template <class T> inline
T MajorityFilterFunc<T>::doit( AimsData<T>& data ) const
{
  T currentclass, majorityclass = 0;
  unsigned currentclasscases = 0, majoritycases = 0;
  std::map<T, unsigned> classcases;
  typename AimsData<T>::iterator it;

  // Goes through the data and count number of values for each class
  for (it = data.begin(); it != data.end(); it++)
  {
    currentclass = (T)(*it);

    if ( !classcases[currentclass] )
    {
        classcases[ currentclass ] = 1;
        currentclasscases = 1;
    }
    else
    {
        currentclasscases = classcases[ currentclass ] + 1;
        classcases[ currentclass ] = currentclasscases;
    }

    if (currentclasscases > majoritycases)
    {
        // Set the new majority cases and class for which it occurs
        majorityclass = currentclass;
        majoritycases = currentclasscases;
    }
  }

  return majorityclass;
}

#endif

