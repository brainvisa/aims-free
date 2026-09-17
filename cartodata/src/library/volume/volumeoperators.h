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

