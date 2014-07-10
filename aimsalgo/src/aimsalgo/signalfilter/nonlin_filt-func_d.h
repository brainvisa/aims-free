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


#ifndef AIMS_SIGNALFILTER_NONLINFILT_FUNC_D_H
#define AIMS_SIGNALFILTER_NONLINFILT_FUNC_D_H

#include <map>
#include <algorithm>
#include <aims/math/mathelem.h>
#include <aims/signalfilter/nonlin_filt-func.h>
#include <cartodata/volume/volumeutil_d.h>

//
// Definitions for class NonLinFilterFunc
//
template <class T>
NonLinFilterFunc<T>::NonLinFilterFunc() {}

template <class T>
NonLinFilterFunc<T>::~NonLinFilterFunc() {}

template <class T> inline
T NonLinFilterFunc<T>::doit( const AimsData<T>& data ) const {
  return this->execute( carto::VolumeRef<T>(data.volume()) );
}
    
template <class T> inline
T NonLinFilterFunc<T>::doit( const carto::VolumeRef<T>& volume ) const {
  return this->execute( volume );
}

//
// Definitions for class MaxFilterFunc
//
template <class T>
MaxFilterFunc<T>::MaxFilterFunc(): NonLinFilterFunc< T >() {}

template <class T>
MaxFilterFunc<T>::~MaxFilterFunc() {}

template <class T> inline
T MaxFilterFunc<T>::execute( const carto::VolumeRef<T>& volume ) const
{
  return carto::VolumeUtil<T>::max(volume);
}

//
// Definitions for class MedianFilterFunc
//
template <class T>
MedianFilterFunc<T>::MedianFilterFunc(): NonLinFilterFunc<T>() {}

template <class T>
MedianFilterFunc<T>::~MedianFilterFunc () { }

template <class T> inline
T MedianFilterFunc<T>::execute( const carto::VolumeRef<T>& volume ) const
{
  std::vector<T> values( volume.getSizeX()
                         * volume.getSizeY()
                         * volume.getSizeZ()
                         * volume.getSizeT() );
  typename carto::VolumeRef<T>::const_iterator it, ie = volume.end();
  typename std::vector<T>::iterator vit = values.begin();

  // Copy volume window to a vector of values
  for (it = volume.begin(); it != ie; ++it, ++vit) {
    (*vit) = (*it);
  }
  
  // Sort values
  std::sort( values.begin(), values.end() );
  
  return values[ values.size() / 2 ];
}

//
// Definitions for class MinFilterFunc
//
template <class T>
MinFilterFunc<T>::MinFilterFunc(): NonLinFilterFunc<T>() {}

template <class T>
MinFilterFunc<T>::~MinFilterFunc () {}

template <class T> inline
T MinFilterFunc<T>::execute( const carto::VolumeRef<T>& volume ) const
{
  return carto::VolumeUtil<T>::min(volume);
}

//
// Definitions for class MeanFilterFunc
//
template <class T>
MeanFilterFunc<T>::MeanFilterFunc(): NonLinFilterFunc<T>() {}

template <class T>
MeanFilterFunc<T>::~MeanFilterFunc () {}

template <class T> inline
T MeanFilterFunc<T>::execute( const carto::VolumeRef<T>& volume ) const
{
  // Sum is declared as double to avoid overflow issue
  double sum = 0.;
  uint32_t count = 0;
  typename carto::VolumeRef<T>::const_iterator it, ie = volume.end();

  // Sum values and count values
  for (it = volume.begin(); it != ie; ++it) {
    sum += (double)(*it);
    ++count;
  }

  return (count != 0 ? (T)( sum / count ) : (T)0);
}

//
// Definitions for class NotNullMeanFilterFunc
//
template <class T>
NotNullMeanFilterFunc<T>::NotNullMeanFilterFunc(): NonLinFilterFunc<T>() {}

template <class T>
NotNullMeanFilterFunc<T>::~NotNullMeanFilterFunc() {}

template <class T> inline
T NotNullMeanFilterFunc<T>::execute( const carto::VolumeRef<T>& volume ) const
{
  // Sum is declared as double to avoid overflow issue
  double sum = 0.;
  uint32_t count = 0;
  typename carto::VolumeRef<T>::const_iterator it, ie = volume.end();

  // Sum and count not null values
  for (it = volume.begin(); it != ie; ++it) {
    if ((*it) != (T)0) {
      sum += (double)(*it);
      ++count;
    }
  }

  return (count != 0 ? (T)( sum / count ) : (T)0);
}

//
// Definitions for class MajorityFilterFunc
//
template <class T>
MajorityFilterFunc<T>::MajorityFilterFunc(): NonLinFilterFunc<T>() {}

template <class T>
MajorityFilterFunc<T>::~MajorityFilterFunc() {}

template <class T> inline
T MajorityFilterFunc<T>::execute( const carto::VolumeRef<T>& volume ) const
{
  T currentclass, majorityclass = 0;
  uint32_t currentclasscases = 0, majoritycases = 0;
  std::map<T, uint32_t> classcases;
  typename carto::VolumeRef<T>::const_iterator it, ie = volume.end();

  // Goes through the data and count number of values for each class
  for (it = volume.begin(); it != ie; ++it)
  {
    currentclass = (*it);

    if ( !classcases[ currentclass ] )
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

//
// Definitions for class ExtremaDifferenceFilterFunc
//
template <class T>
ExtremaDifferenceFilterFunc<T>::ExtremaDifferenceFilterFunc(): NonLinFilterFunc< T >() {}

template <class T>
ExtremaDifferenceFilterFunc<T>::~ExtremaDifferenceFilterFunc() {}

template <class T> inline
T ExtremaDifferenceFilterFunc<T>::execute( const carto::VolumeRef<T>& volume ) const
{
  T min, max;
  min = carto::VolumeUtil<T>::min( volume );
  max = carto::VolumeUtil<T>::max( volume );
  return aims::absdiff<T>(max, min);
}

//
// Definitions for class SumFilterFunc
//
template <class T>
SumFilterFunc<T>::SumFilterFunc(): NonLinFilterFunc< T >() {}

template <class T>
SumFilterFunc<T>::~SumFilterFunc () { }

template <class T> inline
T SumFilterFunc<T>::execute( const carto::VolumeRef<T>& volume ) const
{
  T sum = (T)0;
  typename carto::VolumeRef<T>::const_iterator it, ie = volume.end();

  // Goes through the data and sum values
  for (it = volume.begin(); it != ie; ++it)
    sum += (*it);

  return sum;
}

#endif

