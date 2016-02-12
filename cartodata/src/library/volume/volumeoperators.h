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

#ifndef CARTODATA_VOLUME_VOLUMEOPERATORS_H
#define CARTODATA_VOLUME_VOLUMEOPERATORS_H

#if 0

#include <cartodata/volume/volumeutil.h>
#include <functional>


namespace carto
{

  template <typename T> inline
  VolumeRef<T> operator - ( const VolumeRef<T> & o1 )
  {
    return VolumeUtil<T>::apply( std::negate<T>(), o1 );
  }


  template <typename T> inline
  VolumeRef<T> operator + ( const VolumeRef<T> & o1, const VolumeRef<T> & o2 )
  {
    return VolumeUtil<T>::apply( std::plus<T>(), o1, o2 );
  }


  template <typename T> inline
  VolumeRef<T> & operator += ( VolumeRef<T> & o1, const VolumeRef<T> & o2 )
  {
    VolumeUtil<T>::selfApply( std::plus<T>(), o1, o2 );
    return o1;
  }


  template <typename T> inline
  VolumeRef<T> operator - ( const VolumeRef<T> & o1, const VolumeRef<T> & o2 )
  {
    return VolumeUtil<T>::apply( std::minus<T>(), o1, o2 );
  }


  template <typename T> inline
  VolumeRef<T> & operator -= ( VolumeRef<T> & o1, const VolumeRef<T> & o2 )
  {
    VolumeUtil<T>::selfApply( std::minus<T>(), o1, o2 );
    return o1;
  }


  template <typename T> inline
  VolumeRef<T> operator * ( const VolumeRef<T> & o1, const VolumeRef<T> & o2 )
  {
    return VolumeUtil<T>::apply( std::multiplies<T>(), o1, o2 );
  }


  template <typename T> inline
  VolumeRef<T> & operator *= ( VolumeRef<T> & o1, const VolumeRef<T> & o2 )
  {
    VolumeUtil<T>::selfApply( std::multiplies<T>(), o1, o2 );
    return o1;
  }


  template <typename T> inline
  VolumeRef<T> operator / ( const VolumeRef<T> & o1, const VolumeRef<T> & o2 )
  {
    return VolumeUtil<T>::apply( std::divides<T>(), o1, o2 );
  }


  template <typename T> inline
  VolumeRef<T> & operator /= ( VolumeRef<T> & o1, const VolumeRef<T> & o2 )
  {
    VolumeUtil<T>::selfApply( std::divides<T>(), o1, o2 );
    return o1;
  }


  template <typename T> inline
  VolumeRef<T> operator + ( const VolumeRef<T> & o1, const T & val )
  {
    return VolumeUtil<T>::apply
      ( UnaryFromConstantBinaryFunctor<T, std::plus<T> >
        ( val, std::plus<T>() ), o1 );
  }


  template <typename T> inline
  VolumeRef<T> operator + ( const T & val, const VolumeRef<T> & o1 )
  {
    return VolumeUtil<T>::apply
      ( UnaryFromConstantBinaryFunctor2<T, std::plus<T> >
        ( val, std::plus<T>() ), o1 );
  }


  template <typename T> inline
  VolumeRef<T> & operator += ( VolumeRef<T> & o1, const T & val )
  {
    VolumeUtil<T>::selfApply( UnaryFromConstantBinaryFunctor<T, std::plus<T> >
                              ( val, std::plus<T>() ), o1 );
    return o1;
  }


  template <typename T> inline
  VolumeRef<T> operator - ( const VolumeRef<T> & o1, const T & val )
  {
    return VolumeUtil<T>::apply
      ( UnaryFromConstantBinaryFunctor<T, std::minus<T> >
        ( val, std::minus<T>() ), o1 );
  }


  template <typename T> inline
  VolumeRef<T> operator - ( const T & val, const VolumeRef<T> & o1 )
  {
    return VolumeUtil<T>::apply
      ( UnaryFromConstantBinaryFunctor2<T, std::minus<T> >
        ( val, std::minus<T>() ), o1 );
  }


  template <typename T> inline
  VolumeRef<T> & operator -= ( VolumeRef<T> & o1, const T & val )
  {
    VolumeUtil<T>::selfApply( UnaryFromConstantBinaryFunctor<T, std::minus<T> >
                              ( val, std::minus<T>() ), o1 );
    return o1;
  }


  template <typename T, typename U> inline
  VolumeRef<T> operator * ( const VolumeRef<T> & o1, U val )
  {
    return VolumeUtil<T>::apply( Scaler<T, U>( val ), o1 );
  }


  template <typename T, typename U> inline
  VolumeRef<T> operator * ( U val, const VolumeRef<T> & o1 )
  {
    return VolumeUtil<T>::apply( Scaler<T, U>( val ), o1 );
  }


  template <typename T, typename U> inline
  VolumeRef<T> & operator *= ( VolumeRef<T> & o1, U val )
  {
    VolumeUtil<T>::selfApply( Scaler<T, U>( val ), o1 );
    return o1;
  }


  template <typename T, typename U> inline
  VolumeRef<T> operator / ( const VolumeRef<T> & o1, U val )
  {
    return VolumeUtil<T>::apply( Divider<T, U>( val ), o1 );
  }


  template <typename T> inline
  VolumeRef<T> operator / ( double val, const VolumeRef<T> & o1 )
  {
    return VolumeUtil<T>::apply
      ( UnaryFromConstantBinaryFunctor2<T, std::divides<T> >
        ( val, std::divides<T>() ), o1 );
  }


  template <typename T, typename U> inline
  VolumeRef<T> & operator /= ( VolumeRef<T> & o1, U val )
  {
    VolumeUtil<T>::selfApply( Divider<T, U>( val ), o1 );
    return o1;
  }

}

#endif

#endif

