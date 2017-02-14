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

#ifndef CARTODATA_VOLUME_VOLUMEREF_D_OPERATORS_H
#define CARTODATA_VOLUME_VOLUMEREF_D_OPERATORS_H


//--- cartodata --------------------------------------------------------------
#include <cartodata/volume/volumeref.h>
#include <cartodata/volume/volumeutil.h>
#include <cartodata/volume/functional.h>
//----------------------------------------------------------------------------

// Extern operators should be defined inside the library namespace because of
// Koenig's lookup:
// - https://en.wikipedia.org/wiki/Argument-dependent_name_lookup
namespace carto {

#ifdef CARTO_VOLUME_AUTO_DEREFERENCE

  //==========================================================================
  //   BOOLEANS
  //==========================================================================

  template <typename T>
  inline
  bool VolumeRef<T>::all() const
  {
    return (*this)->all();
  }

  template <typename T>
  inline
  bool VolumeRef<T>::any() const
  {
    return (*this)->any();
  }

#if 0 // would change rc_ptr cast operator (equivalent to get())
  template <typename T>
  inline
  operator VolumeRef<T>::bool() const
  {
    return (bool)(**this);
  }
#endif

  template <typename T>
  inline
  T VolumeRef<T>::min() const
  {
    return (*this)->min();
  }

  template <typename T>
  inline
  T VolumeRef<T>::max() const
  {
    return (*this)->max();
  }

  template <typename T>
  inline
  typename DataTypeTraits<T>::LongType VolumeRef<T>::sum() const
  {
    return (*this)->sum();
  }


  //==========================================================================
  //   FILL / REPLACE
  //==========================================================================

  template <typename T>
  inline
  void VolumeRef<T>::fill( const T & value )
  {
    (*this)->fill(value);
  }

  template <typename T>
  inline
  VolumeRef<T> & VolumeRef<T>::operator= ( const T & value )
  {
    (**this) = value;
    return *this;
  }

  //==========================================================================
  //   COPY
  //==========================================================================
  // This is defined with the operators because it needs volumeutil.h

  template <typename T>
  inline
  VolumeRef<T> VolumeRef<T>::copy() const
  {
    return ::carto::copy<T,T>( *this );
  }

  template <typename T>
  template <typename OUTP>
  inline
  VolumeRef<OUTP> VolumeRef<T>::copy() const
  {
    return ::carto::copy<OUTP,T>( *this );
  }

  template <typename T>
  inline
  VolumeRef<T> VolumeRef<T>::deepcopy() const
  {
    return ::carto::deepcopy<T,T>( *this );
  }

  template <typename T>
  template <typename OUTP>
  inline
  VolumeRef<OUTP> VolumeRef<T>::deepcopy() const
  {
    return ::carto::deepcopy<OUTP,T>( *this );
  }

  template <typename T>
  inline
  VolumeRef<T> VolumeRef<T>::copyStructure() const
  {
    return ::carto::copyStructure<T,T>( *this );
  }

  template <typename T>
  template <typename OUTP>
  inline
  VolumeRef<OUTP> VolumeRef<T>::copyStructure() const
  {
    return ::carto::copyStructure<OUTP,T>( *this );
  }

  template <typename T>
  template <typename OUTP>
  inline
  VolumeRef<T>::operator VolumeRef<OUTP>() const
  {
    return ::carto::deepcopy<OUTP,T>( *this );
  }

  //============================================================================
  //   EXTERN OPERATORS : COMPARISONS
  //============================================================================

  //--- VolumeRef [op] Scalar --------------------------------------------------

  template <typename T, typename U>
  inline
  carto::VolumeRef<bool> operator== ( const carto::VolumeRef<T> & vol, const U & value )
  {
    carto::VolumeRef<bool> output = carto::copyStructure<bool, T>( vol );
    carto::volumeutil::applyTowards( *vol, *output, std::bind2nd( carto::volumeutil::equal_to<T,U>(), value ) );
    return output;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<bool> operator!= ( const carto::VolumeRef<T> & vol, const U & value )
  {
    carto::VolumeRef<bool> output = carto::copyStructure<bool, T>( vol );
    carto::volumeutil::applyTowards( *vol, *output, std::bind2nd( carto::volumeutil::not_equal_to<T,U>(), value ) );
    return output;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<bool> operator>= ( const carto::VolumeRef<T> & vol, const U & value )
  {
    carto::VolumeRef<bool> output = carto::copyStructure<bool, T>( vol );
    carto::volumeutil::applyTowards( *vol, *output, std::bind2nd( carto::volumeutil::greater_equal<T,U>(), value ) );
    return output;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<bool> operator<= ( const carto::VolumeRef<T> & vol, const U & value )
  {
    carto::VolumeRef<bool> output = carto::copyStructure<bool, T>( vol );
    carto::volumeutil::applyTowards( *vol, *output, std::bind2nd( carto::volumeutil::less_equal<T,U>(), value ) );
    return output;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<bool> operator>  ( const carto::VolumeRef<T> & vol, const U & value )
  {
    carto::VolumeRef<bool> output = carto::copyStructure<bool, T>( vol );
    carto::volumeutil::applyTowards( *vol, *output, std::bind2nd( carto::volumeutil::greater<T,U>(), value ) );
    return output;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<bool> operator<  ( const carto::VolumeRef<T> & vol, const U & value )
  {
    carto::VolumeRef<bool> output = carto::copyStructure<bool, T>( vol );
    carto::volumeutil::applyTowards( *vol, *output, std::bind2nd( carto::volumeutil::less<T,U>(), value ) );
    return output;
  }

  //--- Scalar [op] VolumeRef --------------------------------------------------

  template <typename T, typename U>
  inline
  carto::VolumeRef<bool> operator== ( const U & value, const carto::VolumeRef<T> & vol )
  {
    carto::VolumeRef<bool> output = carto::copyStructure<bool, T>( vol );
    carto::volumeutil::applyTowards( *vol, *output, std::bind1st( carto::volumeutil::equal_to<U,T>(), value ) );
    return output;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<bool> operator!= ( const U & value, const carto::VolumeRef<T> & vol )
  {
    carto::VolumeRef<bool> output = carto::copyStructure<bool, T>( vol );
    carto::volumeutil::applyTowards( *vol, *output, std::bind1st( carto::volumeutil::not_equal_to<U,T>(), value ) );
    return output;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<bool> operator>= ( const U & value, const carto::VolumeRef<T> & vol )
  {
    carto::VolumeRef<bool> output = carto::copyStructure<bool, T>( vol );
    carto::volumeutil::applyTowards( *vol, *output, std::bind1st( carto::volumeutil::greater_equal<U,T>(), value ) );
    return output;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<bool> operator<= ( const U & value, const carto::VolumeRef<T> & vol )
  {
    carto::VolumeRef<bool> output = carto::copyStructure<bool, T>( vol );
    carto::volumeutil::applyTowards( *vol, *output, std::bind1st( carto::volumeutil::less_equal<U,T>(), value ) );
    return output;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<bool> operator>  ( const U & value, const carto::VolumeRef<T> & vol )
  {
    carto::VolumeRef<bool> output = carto::copyStructure<bool, T>( vol );
    carto::volumeutil::applyTowards( *vol, *output, std::bind1st( carto::volumeutil::greater<U,T>(), value ) );
    return output;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<bool> operator<  ( const U & value, const carto::VolumeRef<T> & vol )
  {
    carto::VolumeRef<bool> output = carto::copyStructure<bool, T>( vol );
    carto::volumeutil::applyTowards( *vol, *output, std::bind1st( carto::volumeutil::less<U,T>(), value ) );
    return output;
  }

  //--- VolumeRef [op] Volume --------------------------------------------------

  template <typename T, typename U>
  inline
  carto::VolumeRef<bool> operator== ( const carto::VolumeRef<T> & vol, const carto::Volume<U> & other )
  {
    carto::VolumeRef<bool> output = carto::copyStructure<bool, T>( vol );
    carto::volumeutil::applyTowards( *vol, other, *output, carto::volumeutil::equal_to<T,U>() );
    return output;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<bool> operator!= ( const carto::VolumeRef<T> & vol, const carto::Volume<U> & other )
  {
    carto::VolumeRef<bool> output = carto::copyStructure<bool, T>( vol );
    carto::volumeutil::applyTowards( *vol, other, *output, carto::volumeutil::not_equal_to<T,U>() );
    return output;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<bool> operator>= ( const carto::VolumeRef<T> & vol, const carto::Volume<U> & other )
  {
    carto::VolumeRef<bool> output = carto::copyStructure<bool, T>( vol );
    carto::volumeutil::applyTowards( *vol, other, *output, carto::volumeutil::greater_equal<T,U>() );
    return output;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<bool> operator<= ( const carto::VolumeRef<T> & vol, const carto::Volume<U> & other )
  {
    carto::VolumeRef<bool> output = carto::copyStructure<bool, T>( vol );
    carto::volumeutil::applyTowards( *vol, other, *output, carto::volumeutil::less_equal<T,U>() );
    return output;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<bool> operator>  ( const carto::VolumeRef<T> & vol, const carto::Volume<U> & other )
  {
    carto::VolumeRef<bool> output = carto::copyStructure<bool, T>( vol );
    carto::volumeutil::applyTowards( *vol, other, *output, carto::volumeutil::greater<T,U>() );
    return output;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<bool> operator<  ( const carto::VolumeRef<T> & vol, const carto::Volume<U> & other )
  {
    carto::VolumeRef<bool> output = carto::copyStructure<bool, T>( vol );
    carto::volumeutil::applyTowards( *vol, other, *output, carto::volumeutil::less<T,U>() );
    return output;
  }

  //--- Volume [op] VolumeRef --------------------------------------------------

  template <typename T, typename U>
  inline
  carto::Volume<bool> operator== ( const carto::Volume<T> & vol, const carto::VolumeRef<U> & other )
  {
    carto::Volume<bool> output = carto::copyStructure<bool, T>( vol );
    return carto::volumeutil::applyTowards( vol, *other, output, carto::volumeutil::equal_to<T,U>() );
  }

  template <typename T, typename U>
  inline
  carto::Volume<bool> operator!= ( const carto::Volume<T> & vol, const carto::VolumeRef<U> & other )
  {
    carto::Volume<bool> output = carto::copyStructure<bool, T>( vol );
    return carto::volumeutil::applyTowards( vol, *other, output, carto::volumeutil::not_equal_to<T,U>() );
  }

  template <typename T, typename U>
  inline
  carto::Volume<bool> operator>= ( const carto::Volume<T> & vol, const carto::VolumeRef<U> & other )
  {
    carto::Volume<bool> output = carto::copyStructure<bool, T>( vol );
    return carto::volumeutil::applyTowards( vol, *other, output, carto::volumeutil::greater_equal<T,U>() );
  }

  template <typename T, typename U>
  inline
  carto::Volume<bool> operator<= ( const carto::Volume<T> & vol, const carto::VolumeRef<U> & other )
  {
    carto::Volume<bool> output = carto::copyStructure<bool, T>( vol );
    return carto::volumeutil::applyTowards( vol, *other, output, carto::volumeutil::less_equal<T,U>() );
  }

  template <typename T, typename U>
  inline
  carto::Volume<bool> operator>  ( const carto::Volume<T> & vol, const carto::VolumeRef<U> & other )
  {
    carto::Volume<bool> output = carto::copyStructure<bool, T>( vol );
    return carto::volumeutil::applyTowards( vol, *other, output, carto::volumeutil::greater<T,U>() );
  }

  template <typename T, typename U>
  inline
  carto::Volume<bool> operator<  ( const carto::Volume<T> & vol, const carto::VolumeRef<U> & other )
  {
    carto::Volume<bool> output = carto::copyStructure<bool, T>( vol );
    return carto::volumeutil::applyTowards( vol, *other, output, carto::volumeutil::less<T,U>() );
  }

  //--- VolumeRef [op] VolumeRef -----------------------------------------------

  #if 0 // Would change rc_ptr behaviour
  template <typename T, typename U>
  inline
  carto::VolumeRef<bool> operator== ( const carto::VolumeRef<T> & vol, const carto::VolumeRef<U> & other )
  {
    carto::VolumeRef<bool> output = carto::copyStructure<bool, T>( vol );
    carto::volumeutil::applyTowards( *vol, *other, *output, carto::volumeutil::equal_to<T,U>() );
    return output;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<bool> operator!= ( const carto::VolumeRef<T> & vol, const carto::VolumeRef<U> & other )
  {
    carto::VolumeRef<bool> output = carto::copyStructure<bool, T>( vol );
    carto::volumeutil::applyTowards( *vol, *other, *output, carto::volumeutil::not_equal_to<T,U>() );
    return output;
  }
  #endif

  template <typename T, typename U>
  inline
  carto::VolumeRef<bool> operator>= ( const carto::VolumeRef<T> & vol, const carto::VolumeRef<U> & other )
  {
    carto::VolumeRef<bool> output = carto::copyStructure<bool, T>( vol );
    carto::volumeutil::applyTowards( *vol, *other, *output, carto::volumeutil::greater_equal<T,U>() );
    return output;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<bool> operator<= ( const carto::VolumeRef<T> & vol, const carto::VolumeRef<U> & other )
  {
    carto::VolumeRef<bool> output = carto::copyStructure<bool, T>( vol );
    carto::volumeutil::applyTowards( *vol, *other, *output, carto::volumeutil::less_equal<T,U>() );
    return output;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<bool> operator>  ( const carto::VolumeRef<T> & vol, const carto::VolumeRef<U> & other )
  {
    carto::VolumeRef<bool> output = carto::copyStructure<bool, T>( vol );
    carto::volumeutil::applyTowards( *vol, *other, *output, carto::volumeutil::greater<T,U>() );
    return output;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<bool> operator<  ( const carto::VolumeRef<T> & vol, const carto::VolumeRef<U> & other )
  {
    carto::VolumeRef<bool> output = carto::copyStructure<bool, T>( vol );
    carto::volumeutil::applyTowards( *vol, *other, *output, carto::volumeutil::less<T,U>() );
    return output;
  }

  //============================================================================
  //   EXTERN OPERATORS : ARITHMETICS
  //============================================================================

  //--- [op] VolumeRef ---------------------------------------------------------

  template <typename T>
  inline
  carto::VolumeRef<T> operator- ( const carto::VolumeRef<T> & vol )
  {
    return carto::volumeutil::apply( vol, carto::volumeutil::negate<T>() );
  }

  template <typename T>
  inline
  carto::VolumeRef<T> operator~ ( const carto::VolumeRef<T> & vol )
  {
    return carto::volumeutil::apply( vol, carto::volumeutil::bitwise_not<T>() );
  }

  template <typename T>
  inline
  carto::VolumeRef<bool> operator! ( const carto::VolumeRef<T> & vol )
  {
    carto::VolumeRef<bool> output = carto::copyStructure<bool,T>( vol );
    carto::volumeutil::applyTowards( *vol, *output, carto::volumeutil::logical_not<T>() );
    return output;
  }

  //--- VolumeRef [op] Scalar --------------------------------------------------

  template <typename T, typename U>
  inline
  carto::VolumeRef<typename carto::volumeutil::plus<T,U>::result_type >
  operator+ ( const carto::VolumeRef<T> & vol, const U & value )
  {
    return carto::volumeutil::apply( vol, std::bind2nd( carto::volumeutil::plus<T,U>(), value ) );
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<typename carto::volumeutil::minus<T,U>::result_type >
  operator- ( const carto::VolumeRef<T> & vol, const U & value )
  {
    return carto::volumeutil::apply( vol, std::bind2nd( carto::volumeutil::minus<T,U>(), value ) );
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<typename carto::volumeutil::multiplies<T,U>::result_type >
  operator* ( const carto::VolumeRef<T> & vol, const U & value )
  {
    return carto::volumeutil::apply( vol, std::bind2nd( carto::volumeutil::multiplies<T,U>(), value ) );
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<typename carto::volumeutil::divides<T,U>::result_type >
  operator/ ( const carto::VolumeRef<T> & vol, const U & value )
  {
    return carto::volumeutil::apply( vol, std::bind2nd( carto::volumeutil::divides<T,U>(), value ) );
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<typename carto::volumeutil::modulus<T,U>::result_type >
  operator% ( const carto::VolumeRef<T> & vol, const U & value )
  {
    return carto::volumeutil::apply( vol, std::bind2nd( carto::volumeutil::modulus<T,U>(), value ) );
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<typename carto::volumeutil::bitwise_and<T,U>::result_type >
  operator& ( const carto::VolumeRef<T> & vol, const U & value )
  {
    return carto::volumeutil::apply( vol, std::bind2nd( carto::volumeutil::bitwise_and<T,U>(), value ) );
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<typename carto::volumeutil::bitwise_or<T,U>::result_type >
  operator| ( const carto::VolumeRef<T> & vol, const U & value )
  {
    return carto::volumeutil::apply( vol, std::bind2nd( carto::volumeutil::bitwise_or<T,U>(), value ) );
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<typename carto::volumeutil::bitwise_xor<T,U>::result_type >
  operator^ ( const carto::VolumeRef<T> & vol, const U & value )
  {
    return carto::volumeutil::apply( vol, std::bind2nd( carto::volumeutil::bitwise_xor<T,U>(), value ) );
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<bool> operator&& ( const carto::VolumeRef<T> & vol, const U & value )
  {
    carto::VolumeRef<bool> output = carto::copyStructure<bool,T>( vol );
    carto::volumeutil::applyTowards( *vol, *output, std::bind2nd( carto::volumeutil::logical_and<T,U>(), value ) );
    return output;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<bool> operator|| ( const carto::VolumeRef<T> & vol, const U & value )
  {
    carto::VolumeRef<bool> output = carto::copyStructure<bool,T>( vol );
    carto::volumeutil::applyTowards( *vol, *output, std::bind2nd( carto::volumeutil::logical_or<T,U>(), value ) );
    return output;
  }

  //--- Scalar [op] VolumeRef --------------------------------------------------

  template <typename T, typename U>
  inline
  carto::VolumeRef<typename carto::volumeutil::plus<U,T>::result_type >
  operator+ ( const U & value, const carto::VolumeRef<T> & vol )
  {
    return carto::volumeutil::apply( vol, std::bind1st( carto::volumeutil::plus<U,T>(), value ) );
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<typename carto::volumeutil::minus<U,T>::result_type >
  operator- ( const U & value, const carto::VolumeRef<T> & vol )
  {
    return carto::volumeutil::apply( vol, std::bind1st( carto::volumeutil::minus<U,T>(), value ) );
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<typename carto::volumeutil::multiplies<U,T>::result_type >
  operator* ( const U & value, const carto::VolumeRef<T> & vol )
  {
    return carto::volumeutil::apply( vol, std::bind1st( carto::volumeutil::multiplies<U,T>(), value ) );
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<typename carto::volumeutil::divides<U,T>::result_type >
  operator/ ( const U & value, const carto::VolumeRef<T> & vol )
  {
    return carto::volumeutil::apply( vol, std::bind1st( carto::volumeutil::divides<U,T>(), value ) );
  }

  //--- VolumeRef [op] Volume --------------------------------------------------

  template <typename T, typename U>
  inline
  carto::VolumeRef<typename carto::volumeutil::plus<T,U>::result_type >
  operator+ ( const carto::VolumeRef<T> & vol, const carto::Volume<U> & other )
  {
    return carto::volumeutil::apply( vol, other, carto::volumeutil::plus<T,U>() );
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<typename carto::volumeutil::minus<T,U>::result_type >
  operator- ( const carto::VolumeRef<T> & vol, const carto::Volume<U> & other )
  {
    return carto::volumeutil::apply( vol, other, carto::volumeutil::minus<T,U>() );
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<typename carto::volumeutil::multiplies<T,U>::result_type >
  operator* ( const carto::VolumeRef<T> & vol, const carto::Volume<U> & other )
  {
    return carto::volumeutil::apply( vol, other, carto::volumeutil::multiplies<T,U>() );
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<typename carto::volumeutil::divides<T,U>::result_type >
  operator/ ( const carto::VolumeRef<T> & vol, const carto::Volume<U> & other )
  {
    return carto::volumeutil::apply( vol, other, carto::volumeutil::divides<T,U>() );
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<typename carto::volumeutil::modulus<T,U>::result_type >
  operator% ( const carto::VolumeRef<T> & vol, const carto::Volume<U> & other )
  {
    return carto::volumeutil::apply( vol, other, carto::volumeutil::modulus<T,U>() );
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<typename carto::volumeutil::bitwise_and<T,U>::result_type >
  operator& ( const carto::VolumeRef<T> & vol, const carto::Volume<U> & other )
  {
    return carto::volumeutil::apply( vol, other, carto::volumeutil::bitwise_and<T,U>() );
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<typename carto::volumeutil::bitwise_or<T,U>::result_type >
  operator| ( const carto::VolumeRef<T> & vol, const carto::Volume<U> & other )
  {
    return carto::volumeutil::apply( vol, other, carto::volumeutil::bitwise_or<T,U>() );
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<typename carto::volumeutil::bitwise_xor<T,U>::result_type >
  operator^ ( const carto::VolumeRef<T> & vol, const carto::Volume<U> & other )
  {
    return carto::volumeutil::apply( vol, other, carto::volumeutil::bitwise_xor<T,U>() );
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<bool> operator&& ( const carto::VolumeRef<T> & vol, const carto::Volume<U> & other )
  {
    carto::VolumeRef<bool> output = carto::copyStructure<bool,T>( vol );
    carto::volumeutil::applyTowards( *vol, other, *output, carto::volumeutil::logical_and<T,U>() );
    return output;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<bool> operator|| ( const carto::VolumeRef<T> & vol, const carto::Volume<U> & other )
  {
    carto::VolumeRef<bool> output = carto::copyStructure<bool,T>( vol );
    carto::volumeutil::applyTowards( *vol, other, *output, carto::volumeutil::logical_or<T,U>() );
    return output;
  }

  //--- Volume [op] VolumeRef --------------------------------------------------

  template <typename T, typename U>
  inline
  carto::VolumeRef<typename carto::volumeutil::plus<T,U>::result_type >
  operator+ ( const carto::Volume<T> & vol, const carto::VolumeRef<U> & other )
  {
    return carto::volumeutil::apply( vol, *other, carto::volumeutil::plus<T,U>() );
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<typename carto::volumeutil::minus<T,U>::result_type >
  operator- ( const carto::Volume<T> & vol, const carto::VolumeRef<U> & other )
  {
    return carto::volumeutil::apply( vol, *other, carto::volumeutil::minus<T,U>() );
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<typename carto::volumeutil::multiplies<T,U>::result_type >
  operator* ( const carto::Volume<T> & vol, const carto::VolumeRef<U> & other )
  {
    return carto::volumeutil::apply( vol, *other, carto::volumeutil::multiplies<T,U>() );
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<typename carto::volumeutil::divides<T,U>::result_type >
  operator/ ( const carto::Volume<T> & vol, const carto::VolumeRef<U> & other )
  {
    return carto::volumeutil::apply( vol, *other, carto::volumeutil::divides<T,U>() );
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<typename carto::volumeutil::modulus<T,U>::result_type >
  operator% ( const carto::Volume<T> & vol, const carto::VolumeRef<U> & other )
  {
    return carto::volumeutil::apply( vol, *other, carto::volumeutil::modulus<T,U>() );
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<typename carto::volumeutil::bitwise_and<T,U>::result_type >
  operator& ( const carto::Volume<T> & vol, const carto::VolumeRef<U> & other )
  {
    return carto::volumeutil::apply( vol, *other, carto::volumeutil::bitwise_and<T,U>() );
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<typename carto::volumeutil::bitwise_or<T,U>::result_type >
  operator| ( const carto::Volume<T> & vol, const carto::VolumeRef<U> & other )
  {
    return carto::volumeutil::apply( vol, *other, carto::volumeutil::bitwise_or<T,U>() );
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<typename carto::volumeutil::bitwise_xor<T,U>::result_type >
  operator^ ( const carto::Volume<T> & vol, const carto::VolumeRef<U> & other )
  {
    return carto::volumeutil::apply( vol, *other, carto::volumeutil::bitwise_xor<T,U>() );
  }

  template <typename T, typename U>
  inline
  carto::Volume<bool> operator&& ( const carto::Volume<T> & vol, const carto::VolumeRef<U> & other )
  {
    carto::Volume<bool> output = carto::copyStructure<bool,T>( vol );
    return carto::volumeutil::applyTowards( vol, *other, output, carto::volumeutil::logical_and<T,U>() );
  }
  template <typename T, typename U>
  inline
  carto::Volume<bool> operator|| ( const carto::Volume<T> & vol, const carto::VolumeRef<U> & other )
  {
    carto::Volume<bool> output = carto::copyStructure<bool,T>( vol );
    return carto::volumeutil::applyTowards( vol, *other, output, carto::volumeutil::logical_or<T,U>() );
  }

  //--- VolumeRef [op] VolumeRef -----------------------------------------------

  template <typename T, typename U>
  inline
  carto::VolumeRef<typename carto::volumeutil::plus<T,U>::result_type >
  operator+ ( const carto::VolumeRef<T> & vol, const carto::VolumeRef<U> & other )
  {
    return carto::volumeutil::apply( vol, *other, carto::volumeutil::plus<T,U>() );
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<typename carto::volumeutil::minus<T,U>::result_type >
  operator- ( const carto::VolumeRef<T> & vol, const carto::VolumeRef<U> & other )
  {
    return carto::volumeutil::apply( vol, *other, carto::volumeutil::minus<T,U>() );
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<typename carto::volumeutil::multiplies<T,U>::result_type >
  operator* ( const carto::VolumeRef<T> & vol, const carto::VolumeRef<U> & other )
  {
    return carto::volumeutil::apply( vol, *other, carto::volumeutil::multiplies<T,U>() );
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<typename carto::volumeutil::divides<T,U>::result_type >
  operator/ ( const carto::VolumeRef<T> & vol, const carto::VolumeRef<U> & other )
  {
    return carto::volumeutil::apply( vol, *other, carto::volumeutil::divides<T,U>() );
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<typename carto::volumeutil::modulus<T,U>::result_type >
  operator% ( const carto::VolumeRef<T> & vol, const carto::VolumeRef<U> & other )
  {
    return carto::volumeutil::apply( vol, *other, carto::volumeutil::modulus<T,U>() );
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<typename carto::volumeutil::bitwise_and<T,U>::result_type >
  operator& ( const carto::VolumeRef<T> & vol, const carto::VolumeRef<U> & other )
  {
    return carto::volumeutil::apply( vol, *other, carto::volumeutil::bitwise_and<T,U>() );
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<typename carto::volumeutil::bitwise_or<T,U>::result_type >
  operator| ( const carto::VolumeRef<T> & vol, const carto::VolumeRef<U> & other )
  {
    return carto::volumeutil::apply( vol, *other, carto::volumeutil::bitwise_or<T,U>() );
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<typename carto::volumeutil::bitwise_xor<T,U>::result_type >
  operator^ ( const carto::VolumeRef<T> & vol, const carto::VolumeRef<U> & other )
  {
    return carto::volumeutil::apply( vol, *other, carto::volumeutil::bitwise_xor<T,U>() );
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<bool> operator&& ( const carto::VolumeRef<T> & vol, const carto::VolumeRef<U> & other )
  {
    carto::VolumeRef<bool> output = carto::copyStructure<bool,T>( vol );
    carto::volumeutil::applyTowards( *vol, *other, *output, carto::volumeutil::logical_and<T,U>() );
    return output;
  }
  template <typename T, typename U>
  inline
  carto::VolumeRef<bool> operator|| ( const carto::VolumeRef<T> & vol, const carto::VolumeRef<U> & other )
  {
    carto::VolumeRef<bool> output = carto::copyStructure<bool,T>( vol );
    carto::volumeutil::applyTowards( *vol, *other, *output, carto::volumeutil::logical_or<T,U>() );
    return output;
  }

  //============================================================================
  //   OPERATORS: ARITHMETIC MODIFIERS
  //============================================================================

  //--- VolumeRef [op] Scalar --------------------------------------------------

  template <typename T, typename U>
  inline
  carto::VolumeRef<T> & operator+= ( carto::VolumeRef<T> & vol, const U & value )
  {
    carto::volumeutil::selfApply( vol, std::bind2nd( carto::volumeutil::plus<T,U>(), value ) );
    return vol;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<T> & operator-= ( carto::VolumeRef<T> & vol, const U & value )
  {
    carto::volumeutil::selfApply( vol, std::bind2nd( carto::volumeutil::minus<T,U>(), value ) );
    return vol;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<T> & operator*= ( carto::VolumeRef<T> & vol, const U & value )
  {
    carto::volumeutil::selfApply( vol, std::bind2nd( carto::volumeutil::multiplies<T,U>(), value ) );
    return vol;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<T> & operator/= ( carto::VolumeRef<T> & vol, const U & value )
  {
    carto::volumeutil::selfApply( vol, std::bind2nd( carto::volumeutil::divides<T,U>(), value ) );
    return vol;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<T> & operator%= ( carto::VolumeRef<T> & vol, const U & value )
  {
    carto::volumeutil::selfApply( vol, std::bind2nd( carto::volumeutil::modulus<T,U>(), value ) );
    return vol;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<T> & operator&= ( carto::VolumeRef<T> & vol, const U & value )
  {
    carto::volumeutil::selfApply( vol, std::bind2nd( carto::volumeutil::bitwise_and<T,U>(), value ) );
    return vol;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<T> & operator|= ( carto::VolumeRef<T> & vol, const U & value )
  {
    carto::volumeutil::selfApply( vol, std::bind2nd( carto::volumeutil::bitwise_or<T,U>(), value ) );
    return vol;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<T> & operator^= ( carto::VolumeRef<T> & vol, const U & value )
  {
    carto::volumeutil::selfApply( vol, std::bind2nd( carto::volumeutil::bitwise_xor<T,U>(), value ) );
    return vol;
  }

  //--- VolumeRef [op] Volume --------------------------------------------------

  template <typename T, typename U>
  inline
  carto::VolumeRef<T> & operator+= ( carto::VolumeRef<T> & vol, const carto::Volume<U> & other )
  {
    carto::volumeutil::selfApply( vol, other, carto::volumeutil::plus<T,U>() );
    return vol;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<T> & operator-= ( carto::VolumeRef<T> & vol, const carto::Volume<U> & other )
  {
    carto::volumeutil::selfApply( vol, other, carto::volumeutil::minus<T,U>() );
    return vol;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<T> & operator*= ( carto::VolumeRef<T> & vol, const carto::Volume<U> & other )
  {
    carto::volumeutil::selfApply( vol, other, carto::volumeutil::multiplies<T,U>() );
    return vol;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<T> & operator/= ( carto::VolumeRef<T> & vol, const carto::Volume<U> & other )
  {
    carto::volumeutil::selfApply( vol, other, carto::volumeutil::divides<T,U>() );
    return vol;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<T> & operator%= ( carto::VolumeRef<T> & vol, const carto::Volume<U> & other )
  {
    carto::volumeutil::selfApply( vol, other, carto::volumeutil::modulus<T,U>() );
    return vol;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<T> & operator&= ( carto::VolumeRef<T> & vol, const carto::Volume<U> & other )
  {
    carto::volumeutil::selfApply( vol, other, carto::volumeutil::bitwise_and<T,U>() );
    return vol;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<T> & operator|= ( carto::VolumeRef<T> & vol, const carto::Volume<U> & other )
  {
    carto::volumeutil::selfApply( vol, other, carto::volumeutil::bitwise_or<T,U>() );
    return vol;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<T> & operator^= ( carto::VolumeRef<T> & vol, const carto::Volume<U> & other )
  {
    carto::volumeutil::selfApply( vol, other, carto::volumeutil::bitwise_xor<T,U>() );
    return vol;
  }

  //--- Volume [op]= VolumeRef -------------------------------------------------

  template <typename T, typename U>
  inline
  carto::Volume<T> & operator+= ( carto::Volume<T> & vol, const carto::VolumeRef<U> & other )
  {
    return carto::volumeutil::selfApply( vol, *other, carto::volumeutil::plus<T,U>() );
  }

  template <typename T, typename U>
  inline
  carto::Volume<T> & operator-= ( carto::Volume<T> & vol, const carto::VolumeRef<U> & other )
  {
    return carto::volumeutil::selfApply( vol, *other, carto::volumeutil::minus<T,U>() );
  }

  template <typename T, typename U>
  inline
  carto::Volume<T> & operator*= ( carto::Volume<T> & vol, const carto::VolumeRef<U> & other )
  {
    return carto::volumeutil::selfApply( vol, *other, carto::volumeutil::multiplies<T,U>() );
  }

  template <typename T, typename U>
  inline
  carto::Volume<T> & operator/= ( carto::Volume<T> & vol, const carto::VolumeRef<U> & other )
  {
    return carto::volumeutil::selfApply( vol, *other, carto::volumeutil::divides<T,U>() );
  }

  template <typename T, typename U>
  inline
  carto::Volume<T> & operator%= ( carto::Volume<T> & vol, const carto::VolumeRef<U> & other )
  {
    return carto::volumeutil::selfApply( vol, *other, carto::volumeutil::modulus<T,U>() );
  }

  template <typename T, typename U>
  inline
  carto::Volume<T> & operator&= ( carto::Volume<T> & vol, const carto::VolumeRef<U> & other )
  {
    return carto::volumeutil::selfApply( vol, *other, carto::volumeutil::bitwise_and<T,U>() );
  }

  template <typename T, typename U>
  inline
  carto::Volume<T> & operator|= ( carto::Volume<T> & vol, const carto::VolumeRef<U> & other )
  {
    return carto::volumeutil::selfApply( vol, *other, carto::volumeutil::bitwise_or<T,U>() );
  }

  template <typename T, typename U>
  inline
  carto::Volume<T> & operator^= ( carto::Volume<T> & vol, const carto::VolumeRef<U> & other )
  {
    return carto::volumeutil::selfApply( vol, *other, carto::volumeutil::bitwise_xor<T,U>() );
  }

  //--- VolumeRef [op] VolumeRef -----------------------------------------------

  template <typename T, typename U>
  inline
  carto::VolumeRef<T> & operator+= ( carto::VolumeRef<T> & vol, const carto::VolumeRef<U> & other )
  {
    carto::volumeutil::selfApply( vol, *other, carto::volumeutil::plus<T,U>() );
    return vol;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<T> & operator-= ( carto::VolumeRef<T> & vol, const carto::VolumeRef<U> & other )
  {
    carto::volumeutil::selfApply( vol, *other, carto::volumeutil::minus<T,U>() );
    return vol;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<T> & operator*= ( carto::VolumeRef<T> & vol, const carto::VolumeRef<U> & other )
  {
    carto::volumeutil::selfApply( vol, *other, carto::volumeutil::multiplies<T,U>() );
    return vol;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<T> & operator/= ( carto::VolumeRef<T> & vol, const carto::VolumeRef<U> & other )
  {
    carto::volumeutil::selfApply( vol, *other, carto::volumeutil::divides<T,U>() );
    return vol;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<T> & operator%= ( carto::VolumeRef<T> & vol, const carto::VolumeRef<U> & other )
  {
    carto::volumeutil::selfApply( vol, *other, carto::volumeutil::modulus<T,U>() );
    return vol;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<T> & operator&= ( carto::VolumeRef<T> & vol, const carto::VolumeRef<U> & other )
  {
    carto::volumeutil::selfApply( vol, *other, carto::volumeutil::bitwise_and<T,U>() );
    return vol;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<T> & operator|= ( carto::VolumeRef<T> & vol, const carto::VolumeRef<U> & other )
  {
    carto::volumeutil::selfApply( vol, *other, carto::volumeutil::bitwise_or<T,U>() );
    return vol;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<T> & operator^= ( carto::VolumeRef<T> & vol, const carto::VolumeRef<U> & other )
  {
    carto::volumeutil::selfApply( vol, *other, carto::volumeutil::bitwise_xor<T,U>() );
    return vol;
  }


  //============================================================================
  //   INCREMENTS
  //============================================================================

  template <typename T>
  inline
  carto::VolumeRef<T> operator++ ( carto::VolumeRef<T> & vol, int )
  {
    carto::VolumeRef<T> output = carto::deepcopy(vol);
    ++vol;
    return output;
  }

  template <typename T>
  inline
  carto::VolumeRef<T> & operator++ ( carto::VolumeRef<T> & vol )
  {
    carto::volumeutil::selfApply( vol, carto::volumeutil::increment<T>() );
    return vol;
  }

  template <typename T>
  inline
  carto::VolumeRef<T> operator-- ( carto::VolumeRef<T> & vol, int )
  {
    carto::VolumeRef<T> output = carto::deepcopy(vol);
    --vol;
    return output;
  }

  template <typename T>
  inline
  carto::VolumeRef<T> & operator-- ( carto::VolumeRef<T> & vol )
  {
    carto::volumeutil::selfApply( vol, carto::volumeutil::decrement<T>() );
    return vol;
  }

#endif // CARTO_VOLUME_AUTO_DEREFERENCE

} // namespace carto

#endif // CARTODATA_VOLUME_VOLUMEREF_D_OPERATORS_H
