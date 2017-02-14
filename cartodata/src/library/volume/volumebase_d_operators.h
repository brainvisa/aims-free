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

#ifndef CARTODATA_VOLUME_VOLUMEBASE_D_OPERATORS_H
#define CARTODATA_VOLUME_VOLUMEBASE_D_OPERATORS_H


//--- cartodata --------------------------------------------------------------
#include <cartodata/volume/volumebase.h>
#include <cartodata/volume/volumeutil.h>
#include <cartodata/volume/functional.h>
//----------------------------------------------------------------------------

// Methods that need volumeutil are defined here

// Extern operators should be defined inside the library namespace because of
// Koenig's lookup:
// - https://en.wikipedia.org/wiki/Argument-dependent_name_lookup
namespace carto {

  namespace volumebaseinternal {
    // Since non template methods risk being instantiated even if
    // they are not actually called, it is necessary to enable them only
    // when we are sure they won't cause a compilation error.
    // This is the aim of this helper.
    // all(), any(), min(), max(), sum() are concerned
    template <typename T, bool is_scalar = DataTypeTraits<T>::is_scalar>
    struct select
    {
      static bool all( const Volume<T> & vol )
      {
        throw std::logic_error( "Member function all() is only enabled for "
          "volumes of scalar. Try to use the non-member function "
          "carto::all(Volume<T>) instead." );
      }

      static bool any( const Volume<T> & vol )
      {
        throw std::logic_error( "Member function any() is only enabled for "
          "volumes of scalar. Try to use the non-member function "
          "carto::any(Volume<T>) instead." );
      }

      static T min( const Volume<T> & vol )
      {
        throw std::logic_error( "Member function min() is only enabled for "
          "volumes of scalar. Try to use the non-member function "
          "carto::min(Volume<T>) instead." );
      }

      static T max( const Volume<T> & vol )
      {
        throw std::logic_error( "Member function max() is only enabled for "
          "volumes of scalar. Try to use the non-member function "
          "carto::max(Volume<T>) instead." );
      }

      static typename DataTypeTraits<T>::LongType
      sum( const Volume<T> & vol )
      {
        throw std::logic_error( "Member function sum() is only enabled for "
          "volumes of scalar. Try to use the non-member function "
          "carto::sum(Volume<T>) instead." );
      }
    };


    template <typename T>
    struct select<T, true>
    {
      static bool all( const Volume<T> & vol )
      {
        return carto::all(vol);
      }

      static bool any( const Volume<T> & vol )
      {
        return carto::any(vol);
      }

      static T min( const Volume<T> & vol )
      {
        return carto::min(vol);
      }

      static T max( const Volume<T> & vol )
      {
        return carto::max(vol);
      }

      static typename DataTypeTraits<T>::LongType
      sum( const Volume<T> & vol )
      {
        return carto::sum(vol);
      }
    };
  }

  //==========================================================================
  //   BOOLEANS/MIN/MAX...
  //==========================================================================
  template <typename T>
  inline
  bool Volume<T>::all() const
  {
    return volumebaseinternal::select<T>::all( *this );
  }

  template <typename T>
  inline
  bool Volume<T>::any() const
  {
    return volumebaseinternal::select<T>::any( *this );
  }

  template <typename T>
  inline
  Volume<T>::operator bool() const
  {
    return all();
  }

  template <typename T>
  inline
  T Volume<T>::min() const
  {
    return volumebaseinternal::select<T>::min( *this );
  }

  template <typename T>
  inline
  T Volume<T>::max() const
  {
    return volumebaseinternal::select<T>::max( *this );
  }

  template <typename T>
  inline
  typename DataTypeTraits<T>::LongType Volume<T>::sum() const
  {
    return volumebaseinternal::select<T>::sum( *this );
  }


  //==========================================================================
  //   FILL / REPLACE
  //==========================================================================

  template <typename T>
  inline
  void Volume<T>::fill( const T & value )
  {
    volumeutil::selfApply( *this, std::bind2nd( volumeutil::select_right<T>(), value ) );
  }

  template <typename T>
  inline
  Volume<T> & Volume<T>::operator= ( const T & value )
  {
    this->fill( value );
    return *this;
  }

  //==========================================================================
  //   COPY
  //==========================================================================

  template <typename T>
  inline
  Volume<T> Volume<T>::copy() const
  {
    return ::carto::copy<T,T>( *this );
  }

  template <typename T>
  template <typename OUTP>
  inline
  Volume<OUTP> Volume<T>::copy() const
  {
    return ::carto::copy<OUTP,T>( *this );
  }

  template <typename T>
  inline
  Volume<T> Volume<T>::deepcopy() const
  {
    return ::carto::deepcopy<T,T>( *this );
  }

  template <typename T>
  template <typename OUTP>
  inline
  Volume<OUTP> Volume<T>::deepcopy() const
  {
    return ::carto::deepcopy<OUTP,T>( *this );
  }

  template <typename T>
  inline
  Volume<T> Volume<T>::copyStructure() const
  {
    return ::carto::copyStructure<T,T>( *this );
  }

  template <typename T>
  template <typename OUTP>
  inline
  Volume<OUTP> Volume<T>::copyStructure() const
  {
    return ::carto::copyStructure<OUTP,T>( *this );
  }

  template <typename T>
  template <typename OUTP>
  inline
  Volume<T>::operator Volume<OUTP>() const
  {
    return ::carto::deepcopy<OUTP,T>( *this );
  }

  //============================================================================
  //   OPERATORS : COMPARISONS
  //============================================================================

  //--- Volume [op] Scalar -----------------------------------------------------

  template <typename T, typename U>
  inline
  carto::Volume<bool> operator== ( const carto::Volume<T> & vol, const U & value )
  {
    carto::Volume<bool> output = carto::copyStructure<bool, T>( vol );
    return carto::volumeutil::applyTowards( vol, output, std::bind2nd( carto::volumeutil::equal_to<T,U>(), value ) );
  }

  template <typename T, typename U>
  inline
  carto::Volume<bool> operator!= ( const carto::Volume<T> & vol, const U & value )
  {
    carto::Volume<bool> output = carto::copyStructure<bool, T>( vol );
    return carto::volumeutil::applyTowards( vol, output, std::bind2nd( carto::volumeutil::not_equal_to<T,U>(), value ) );
  }

  template <typename T, typename U>
  inline
  carto::Volume<bool> operator>= ( const carto::Volume<T> & vol, const U & value )
  {
    carto::Volume<bool> output = carto::copyStructure<bool, T>( vol );
    return carto::volumeutil::applyTowards( vol, output, std::bind2nd( carto::volumeutil::greater_equal<T,U>(), value ) );
  }

  template <typename T, typename U>
  inline
  carto::Volume<bool> operator<= ( const carto::Volume<T> & vol, const U & value )
  {
    carto::Volume<bool> output = carto::copyStructure<bool, T>( vol );
    return carto::volumeutil::applyTowards( vol, output, std::bind2nd( carto::volumeutil::less_equal<T,U>(), value ) );
  }

  template <typename T, typename U>
  inline
  carto::Volume<bool> operator>  ( const carto::Volume<T> & vol, const U & value )
  {
    carto::Volume<bool> output = carto::copyStructure<bool, T>( vol );
    return carto::volumeutil::applyTowards( vol, output, std::bind2nd( carto::volumeutil::greater<T,U>(), value ) );
  }

  template <typename T, typename U>
  inline
  carto::Volume<bool> operator<  ( const carto::Volume<T> & vol, const U & value )
  {
    carto::Volume<bool> output = carto::copyStructure<bool, T>( vol );
    return carto::volumeutil::applyTowards( vol, output, std::bind2nd( carto::volumeutil::less<T,U>(), value ) );
  }

  //--- Scalar [op] Volume -----------------------------------------------------

  template <typename T, typename U>
  inline
  carto::Volume<bool> operator== ( const U & value, const carto::Volume<T> & vol )
  {
    carto::Volume<bool> output = carto::copyStructure<bool, T>( vol );
    return carto::volumeutil::applyTowards( vol, output, std::bind1st( carto::volumeutil::equal_to<U,T>(), value ) );
  }

  template <typename T, typename U>
  inline
  carto::Volume<bool> operator!= ( const U & value, const carto::Volume<T> & vol )
  {
    carto::Volume<bool> output = carto::copyStructure<bool, T>( vol );
    return carto::volumeutil::applyTowards( vol, output, std::bind1st( carto::volumeutil::not_equal_to<U,T>(), value ) );
  }

  template <typename T, typename U>
  inline
  carto::Volume<bool> operator>= ( const U & value, const carto::Volume<T> & vol )
  {
    carto::Volume<bool> output = carto::copyStructure<bool, T>( vol );
    return carto::volumeutil::applyTowards( vol, output, std::bind1st( carto::volumeutil::greater_equal<U,T>(), value ) );
  }

  template <typename T, typename U>
  inline
  carto::Volume<bool> operator<= ( const U & value, const carto::Volume<T> & vol )
  {
    carto::Volume<bool> output = carto::copyStructure<bool, T>( vol );
    return carto::volumeutil::applyTowards( vol, output, std::bind1st( carto::volumeutil::less_equal<U,T>(), value ) );
  }

  template <typename T, typename U>
  inline
  carto::Volume<bool> operator>  ( const U & value, const carto::Volume<T> & vol )
  {
    carto::Volume<bool> output = carto::copyStructure<bool, T>( vol );
    return carto::volumeutil::applyTowards( vol, output, std::bind1st( carto::volumeutil::greater<U,T>(), value ) );
  }

  template <typename T, typename U>
  inline
  carto::Volume<bool> operator<  ( const U & value, const carto::Volume<T> & vol )
  {
    carto::Volume<bool> output = carto::copyStructure<bool, T>( vol );
    return carto::volumeutil::applyTowards( vol, output, std::bind1st( carto::volumeutil::less<U,T>(), value ) );
  }

  //--- Volume [op] Volume -----------------------------------------------------

  template <typename T, typename U>
  inline
  carto::Volume<bool> operator== ( const carto::Volume<T> & vol, const carto::Volume<U> & other )
  {
    carto::Volume<bool> output = carto::copyStructure<bool, T>( vol );
    return carto::volumeutil::applyTowards( vol, other, output, carto::volumeutil::equal_to<T,U>() );
  }

  template <typename T, typename U>
  inline
  carto::Volume<bool> operator!= ( const carto::Volume<T> & vol, const carto::Volume<U> & other )
  {
    carto::Volume<bool> output = carto::copyStructure<bool, T>( vol );
    return carto::volumeutil::applyTowards( vol, other, output, carto::volumeutil::not_equal_to<T,U>() );
  }

  template <typename T, typename U>
  inline
  carto::Volume<bool> operator>= ( const carto::Volume<T> & vol, const carto::Volume<U> & other )
  {
    carto::Volume<bool> output = carto::copyStructure<bool, T>( vol );
    return carto::volumeutil::applyTowards( vol, other, output, carto::volumeutil::greater_equal<T,U>() );
  }

  template <typename T, typename U>
  inline
  carto::Volume<bool> operator<= ( const carto::Volume<T> & vol, const carto::Volume<U> & other )
  {
    carto::Volume<bool> output = carto::copyStructure<bool, T>( vol );
    return carto::volumeutil::applyTowards( vol, other, output, carto::volumeutil::less_equal<T,U>() );
  }

  template <typename T, typename U>
  inline
  carto::Volume<bool> operator>  ( const carto::Volume<T> & vol, const carto::Volume<U> & other )
  {
    carto::Volume<bool> output = carto::copyStructure<bool, T>( vol );
    return carto::volumeutil::applyTowards( vol, other, output, carto::volumeutil::greater<T,U>() );
  }

  template <typename T, typename U>
  inline
  carto::Volume<bool> operator<  ( const carto::Volume<T> & vol, const carto::Volume<U> & other )
  {
    carto::Volume<bool> output = carto::copyStructure<bool, T>( vol );
    return carto::volumeutil::applyTowards( vol, other, output, carto::volumeutil::less<T,U>() );
  }

  //============================================================================
  //   OPERATORS : ARITHMETICS
  //============================================================================

  //--- [op] Volume ------------------------------------------------------------

  template <typename T>
  inline
  carto::Volume<T> operator- ( const carto::Volume<T> & vol )
  {
    return carto::volumeutil::apply( vol, carto::volumeutil::negate<T>() );
  }

  template <typename T>
  inline
  carto::Volume<T> operator~ ( const carto::Volume<T> & vol )
  {
    return carto::volumeutil::apply( vol, carto::volumeutil::bitwise_not<T>() );
  }

  template <typename T>
  inline
  carto::Volume<bool> operator! ( const carto::Volume<T> & vol )
  {
    carto::Volume<bool> output = carto::copyStructure<bool,T>( vol );
    return carto::volumeutil::applyTowards( vol, output, carto::volumeutil::logical_not<T>() );
  }

  //--- Volume [op] Scalar -----------------------------------------------------

  template <typename T, typename U>
  inline
  carto::Volume<typename carto::volumeutil::plus<T,U>::result_type >
  operator+ ( const carto::Volume<T> & vol, const U & value )
  {
    return carto::volumeutil::apply( vol, std::bind2nd( carto::volumeutil::plus<T,U>(), value ) );
  }

  template <typename T, typename U>
  inline
  carto::Volume<typename carto::volumeutil::minus<T,U>::result_type >
  operator- ( const carto::Volume<T> & vol, const U & value )
  {
    return carto::volumeutil::apply( vol, std::bind2nd( carto::volumeutil::minus<T,U>(), value ) );
  }

  template <typename T, typename U>
  inline
  carto::Volume<typename carto::volumeutil::multiplies<T,U>::result_type >
  operator* ( const carto::Volume<T> & vol, const U & value )
  {
    return carto::volumeutil::apply( vol, std::bind2nd( carto::volumeutil::multiplies<T,U>(), value ) );
  }

  template <typename T, typename U>
  inline
  carto::Volume<typename carto::volumeutil::divides<T,U>::result_type >
  operator/ ( const carto::Volume<T> & vol, const U & value )
  {
    return carto::volumeutil::apply( vol, std::bind2nd( carto::volumeutil::divides<T,U>(), value ) );
  }

  template <typename T, typename U>
  inline
  carto::Volume<typename carto::volumeutil::modulus<T,U>::result_type >
  operator% ( const carto::Volume<T> & vol, const U & value )
  {
    return carto::volumeutil::apply( vol, std::bind2nd( carto::volumeutil::modulus<T,U>(), value ) );
  }

  template <typename T, typename U>
  inline
  carto::Volume<typename carto::volumeutil::bitwise_and<T,U>::result_type >
  operator& ( const carto::Volume<T> & vol, const U & value )
  {
    return carto::volumeutil::apply( vol, std::bind2nd( carto::volumeutil::bitwise_and<T,U>(), value ) );
  }

  template <typename T, typename U>
  inline
  carto::Volume<typename carto::volumeutil::bitwise_or<T,U>::result_type >
  operator| ( const carto::Volume<T> & vol, const U & value )
  {
    return carto::volumeutil::apply( vol, std::bind2nd( carto::volumeutil::bitwise_or<T,U>(), value ) );
  }

  template <typename T, typename U>
  inline
  carto::Volume<typename carto::volumeutil::bitwise_xor<T,U>::result_type >
  operator^ ( const carto::Volume<T> & vol, const U & value )
  {
    return carto::volumeutil::apply( vol, std::bind2nd( carto::volumeutil::bitwise_xor<T,U>(), value ) );
  }

  template <typename T, typename U>
  inline
  carto::Volume<bool> operator&& ( const carto::Volume<T> & vol, const U & value )
  {
    carto::Volume<bool> output = carto::copyStructure<bool,T>( vol );
    return carto::volumeutil::applyTowards( vol, output, std::bind2nd( carto::volumeutil::logical_and<T,U>(), value ) );
  }

  template <typename T, typename U>
  inline
  carto::Volume<bool> operator|| ( const carto::Volume<T> & vol, const U & value )
  {
    carto::Volume<bool> output = carto::copyStructure<bool,T>( vol );
    return carto::volumeutil::applyTowards( vol, output, std::bind2nd( carto::volumeutil::logical_or<T,U>(), value ) );
  }

  //--- Scalar [op] Volume -----------------------------------------------------

  template <typename T, typename U>
  inline
  carto::Volume<typename carto::volumeutil::plus<U,T>::result_type >
  operator+ ( const U & value, const carto::Volume<T> & vol )
  {
    return carto::volumeutil::apply( vol, std::bind1st( carto::volumeutil::plus<U,T>(), value ) );
  }

  template <typename T, typename U>
  inline
  carto::Volume<typename carto::volumeutil::minus<U,T>::result_type >
  operator- ( const U & value, const carto::Volume<T> & vol )
  {
    return carto::volumeutil::apply( vol, std::bind1st( carto::volumeutil::minus<U,T>(), value ) );
  }

  template <typename T, typename U>
  inline
  carto::Volume<typename carto::volumeutil::multiplies<U,T>::result_type >
  operator* ( const U & value, const carto::Volume<T> & vol )
  {
    return carto::volumeutil::apply( vol, std::bind1st( carto::volumeutil::multiplies<U,T>(), value ) );
  }

  template <typename T, typename U>
  inline
  carto::Volume<typename carto::volumeutil::divides<U,T>::result_type >
  operator/ ( const U & value, const carto::Volume<T> & vol )
  {
    return carto::volumeutil::apply( vol, std::bind1st( carto::volumeutil::divides<U,T>(), value ) );
  }

  //--- Volume [op] Volume -----------------------------------------------------

  template <typename T, typename U>
  inline
  carto::Volume<typename carto::volumeutil::plus<T,U>::result_type >
  operator+ ( const carto::Volume<T> & vol, const carto::Volume<U> & other )
  {
    return carto::volumeutil::apply( vol, other, carto::volumeutil::plus<T,U>() );
  }

  template <typename T, typename U>
  inline
  carto::Volume<typename carto::volumeutil::minus<T,U>::result_type >
  operator- ( const carto::Volume<T> & vol, const carto::Volume<U> & other )
  {
    return carto::volumeutil::apply( vol, other, carto::volumeutil::minus<T,U>() );
  }

  template <typename T, typename U>
  inline
  carto::Volume<typename carto::volumeutil::multiplies<T,U>::result_type >
  operator* ( const carto::Volume<T> & vol, const carto::Volume<U> & other )
  {
    return carto::volumeutil::apply( vol, other, carto::volumeutil::multiplies<T,U>() );
  }

  template <typename T, typename U>
  inline
  carto::Volume<typename carto::volumeutil::divides<T,U>::result_type >
  operator/ ( const carto::Volume<T> & vol, const carto::Volume<U> & other )
  {
    return carto::volumeutil::apply( vol, other, carto::volumeutil::divides<T,U>() );
  }

  template <typename T, typename U>
  inline
  carto::Volume<typename carto::volumeutil::modulus<T,U>::result_type >
  operator% ( const carto::Volume<T> & vol, const carto::Volume<U> & other )
  {
    return carto::volumeutil::apply( vol, other, carto::volumeutil::modulus<T,U>() );
  }

  template <typename T, typename U>
  inline
  carto::Volume<typename carto::volumeutil::bitwise_and<T,U>::result_type >
  operator& ( const carto::Volume<T> & vol, const carto::Volume<U> & other )
  {
    return carto::volumeutil::apply( vol, other, carto::volumeutil::bitwise_and<T,U>() );
  }

  template <typename T, typename U>
  inline
  carto::Volume<typename carto::volumeutil::bitwise_or<T,U>::result_type >
  operator| ( const carto::Volume<T> & vol, const carto::Volume<U> & other )
  {
    return carto::volumeutil::apply( vol, other, carto::volumeutil::bitwise_or<T,U>() );
  }

  template <typename T, typename U>
  inline
  carto::Volume<typename carto::volumeutil::bitwise_xor<T,U>::result_type >
  operator^ ( const carto::Volume<T> & vol, const carto::Volume<U> & other )
  {
    return carto::volumeutil::apply( vol, other, carto::volumeutil::bitwise_xor<T,U>() );
  }

  template <typename T, typename U>
  inline
  carto::Volume<bool> operator&& ( const carto::Volume<T> & vol, const carto::Volume<U> & other )
  {
    carto::Volume<bool> output = carto::copyStructure<bool,T>( vol );
    return carto::volumeutil::applyTowards( vol, other, output, carto::volumeutil::logical_and<T,U>() );
  }

  template <typename T, typename U>
  inline
  carto::Volume<bool> operator|| ( const carto::Volume<T> & vol, const carto::Volume<U> & other )
  {
    carto::Volume<bool> output = carto::copyStructure<bool,T>( vol );
    return carto::volumeutil::applyTowards( vol, other, output, carto::volumeutil::logical_or<T,U>() );
  }

  //============================================================================
  //   OPERATORS: ARITHMETIC MODIFIERS
  //============================================================================

  //--- Volume [op]= Scalar ----------------------------------------------------

  template <typename T, typename U>
  inline
  carto::Volume<T> & operator+= ( carto::Volume<T> & vol, const U & value )
  {
    return carto::volumeutil::selfApply( vol, std::bind2nd( carto::volumeutil::plus<T,U>(), value ) );
  }

  template <typename T, typename U>
  inline
  carto::Volume<T> & operator-= ( carto::Volume<T> & vol, const U & value )
  {
    return carto::volumeutil::selfApply( vol, std::bind2nd( carto::volumeutil::minus<T,U>(), value ) );
  }

  template <typename T, typename U>
  inline
  carto::Volume<T> & operator*= ( carto::Volume<T> & vol, const U & value )
  {
    return carto::volumeutil::selfApply( vol, std::bind2nd( carto::volumeutil::multiplies<T,U>(), value ) );
  }

  template <typename T, typename U>
  inline
  carto::Volume<T> & operator/= ( carto::Volume<T> & vol, const U & value )
  {
    return carto::volumeutil::selfApply( vol, std::bind2nd( carto::volumeutil::divides<T,U>(), value ) );
  }

  template <typename T, typename U>
  inline
  carto::Volume<T> & operator%= ( carto::Volume<T> & vol, const U & value )
  {
    return carto::volumeutil::selfApply( vol, std::bind2nd( carto::volumeutil::modulus<T,U>(), value ) );
  }

  template <typename T, typename U>
  inline
  carto::Volume<T> & operator&= ( carto::Volume<T> & vol, const U & value )
  {
    return carto::volumeutil::selfApply( vol, std::bind2nd( carto::volumeutil::bitwise_and<T,U>(), value ) );
  }

  template <typename T, typename U>
  inline
  carto::Volume<T> & operator|= ( carto::Volume<T> & vol, const U & value )
  {
    return carto::volumeutil::selfApply( vol, std::bind2nd( carto::volumeutil::bitwise_or<T,U>(), value ) );
  }

  template <typename T, typename U>
  inline
  carto::Volume<T> & operator^= ( carto::Volume<T> & vol, const U & value )
  {
    return carto::volumeutil::selfApply( vol, std::bind2nd( carto::volumeutil::bitwise_xor<T,U>(), value ) );
  }

  //--- Volume [op]= Volume ----------------------------------------------------

  template <typename T, typename U>
  inline
  carto::Volume<T> & operator+= ( carto::Volume<T> & vol, const carto::Volume<U> & other )
  {
    return carto::volumeutil::selfApply( vol, other, carto::volumeutil::plus<T,U>() );
  }

  template <typename T, typename U>
  inline
  carto::Volume<T> & operator-= ( carto::Volume<T> & vol, const carto::Volume<U> & other )
  {
    return carto::volumeutil::selfApply( vol, other, carto::volumeutil::minus<T,U>() );
  }

  template <typename T, typename U>
  inline
  carto::Volume<T> & operator*= ( carto::Volume<T> & vol, const carto::Volume<U> & other )
  {
    return carto::volumeutil::selfApply( vol, other, carto::volumeutil::multiplies<T,U>() );
  }

  template <typename T, typename U>
  inline
  carto::Volume<T> & operator/= ( carto::Volume<T> & vol, const carto::Volume<U> & other )
  {
    return carto::volumeutil::selfApply( vol, other, carto::volumeutil::divides<T,U>() );
  }

  template <typename T, typename U>
  inline
  carto::Volume<T> & operator%= ( carto::Volume<T> & vol, const carto::Volume<U> & other )
  {
    return carto::volumeutil::selfApply( vol, other, carto::volumeutil::modulus<T,U>() );
  }

  template <typename T, typename U>
  inline
  carto::Volume<T> & operator&= ( carto::Volume<T> & vol, const carto::Volume<U> & other )
  {
    return carto::volumeutil::selfApply( vol, other, carto::volumeutil::bitwise_and<T,U>() );
  }

  template <typename T, typename U>
  inline
  carto::Volume<T> & operator|= ( carto::Volume<T> & vol, const carto::Volume<U> & other )
  {
    return carto::volumeutil::selfApply( vol, other, carto::volumeutil::bitwise_or<T,U>() );
  }

  template <typename T, typename U>
  inline
  carto::Volume<T> & operator^= ( carto::Volume<T> & vol, const carto::Volume<U> & other )
  {
    return carto::volumeutil::selfApply( vol, other, carto::volumeutil::bitwise_xor<T,U>() );
  }

  //============================================================================
  //   INCREMENTS
  //============================================================================

  template <typename T>
  inline
  carto::Volume<T> operator++ ( carto::Volume<T> & vol, int)
  {
    carto::Volume<T> output = vol.deepcopy();
    ++vol;
    return output;
  }

  template <typename T>
  inline
  carto::Volume<T> & operator++ ( carto::Volume<T> & vol )
  {
    return carto::volumeutil::selfApply( vol, carto::volumeutil::increment<T>() );
  }

  template <typename T>
  inline
  carto::Volume<T> operator-- ( carto::Volume<T> & vol, int)
  {
    carto::Volume<T> output = vol.deepcopy();
    --vol;
    return output;
  }

  template <typename T>
  inline
  carto::Volume<T> & operator-- ( carto::Volume<T> & vol )
  {
    return carto::volumeutil::selfApply( vol, carto::volumeutil::decrement<T>() );
  }

} // namespace carto

#endif // CARTODATA_VOLUME_VOLUMEBASE_D_OPERATORS_H
