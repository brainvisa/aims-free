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
  void VolumeRef<T>::fillBorder( const T & value )
  {
    (*this)->fillBorder(value);
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
    carto::volumeutil::applyTowards( *vol, *output, [&value](const auto &p) {return p == value;} );
    return output;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<bool> operator!= ( const carto::VolumeRef<T> & vol, const U & value )
  {
    carto::VolumeRef<bool> output = carto::copyStructure<bool, T>( vol );
    carto::volumeutil::applyTowards( *vol, *output, [&value](const auto &p) {return p != value;} );
    return output;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<bool> operator>= ( const carto::VolumeRef<T> & vol, const U & value )
  {
    carto::VolumeRef<bool> output = carto::copyStructure<bool, T>( vol );
    carto::volumeutil::applyTowards( *vol, *output, [&value](const auto &p) {return p >= value;} );
    return output;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<bool> operator<= ( const carto::VolumeRef<T> & vol, const U & value )
  {
    carto::VolumeRef<bool> output = carto::copyStructure<bool, T>( vol );
    carto::volumeutil::applyTowards( *vol, *output, [&value](const auto &p) {return p <= value;} );
    return output;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<bool> operator>  ( const carto::VolumeRef<T> & vol, const U & value )
  {
    carto::VolumeRef<bool> output = carto::copyStructure<bool, T>( vol );
    carto::volumeutil::applyTowards( *vol, *output, [&value](const auto &p) {return p > value;} );
    return output;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<bool> operator<  ( const carto::VolumeRef<T> & vol, const U & value )
  {
    carto::VolumeRef<bool> output = carto::copyStructure<bool, T>( vol );
    carto::volumeutil::applyTowards( *vol, *output, [&value](const auto &p) {return p < value;} );
    return output;
  }

  //--- Scalar [op] VolumeRef --------------------------------------------------

  template <typename T, typename U>
  inline
  carto::VolumeRef<bool> operator== ( const U & value, const carto::VolumeRef<T> & vol )
  {
    carto::VolumeRef<bool> output = carto::copyStructure<bool, T>( vol );
    carto::volumeutil::applyTowards( *vol, *output, [&value](const auto &p) {return value == p;} );
    return output;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<bool> operator!= ( const U & value, const carto::VolumeRef<T> & vol )
  {
    carto::VolumeRef<bool> output = carto::copyStructure<bool, T>( vol );
    carto::volumeutil::applyTowards( *vol, *output, [&value](const auto &p) {return value != p;} );
    return output;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<bool> operator>= ( const U & value, const carto::VolumeRef<T> & vol )
  {
    carto::VolumeRef<bool> output = carto::copyStructure<bool, T>( vol );
    carto::volumeutil::applyTowards( *vol, *output, [&value](const auto &p) {return value >= p;} );
    return output;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<bool> operator<= ( const U & value, const carto::VolumeRef<T> & vol )
  {
    carto::VolumeRef<bool> output = carto::copyStructure<bool, T>( vol );
    carto::volumeutil::applyTowards( *vol, *output, [&value](const auto &p) {return value <= p;} );
    return output;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<bool> operator>  ( const U & value, const carto::VolumeRef<T> & vol )
  {
    carto::VolumeRef<bool> output = carto::copyStructure<bool, T>( vol );
    carto::volumeutil::applyTowards( *vol, *output, [&value](const auto &p) {return value > p;} );
    return output;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<bool> operator<  ( const U & value, const carto::VolumeRef<T> & vol )
  {
    carto::VolumeRef<bool> output = carto::copyStructure<bool, T>( vol );
    carto::volumeutil::applyTowards( *vol, *output, [&value](const auto &p) {return value < p;} );
    return output;
  }

  //--- VolumeRef [op] Volume --------------------------------------------------

  template <typename T, typename U>
  inline
  carto::VolumeRef<bool> operator== ( const carto::VolumeRef<T> & vol, const carto::Volume<U> & other )
  {
    carto::VolumeRef<bool> output = carto::copyStructure<bool, T>( vol );
    carto::volumeutil::applyTowards( *vol, other, *output, [](const T &x, const U &y) { return x == y; } );
    return output;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<bool> operator!= ( const carto::VolumeRef<T> & vol, const carto::Volume<U> & other )
  {
    carto::VolumeRef<bool> output = carto::copyStructure<bool, T>( vol );
    carto::volumeutil::applyTowards( *vol, other, *output, [](const T &x, const U &y) { return x != y; } );
    return output;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<bool> operator>= ( const carto::VolumeRef<T> & vol, const carto::Volume<U> & other )
  {
    carto::VolumeRef<bool> output = carto::copyStructure<bool, T>( vol );
    carto::volumeutil::applyTowards( *vol, other, *output, [](const T &x, const U &y) { return x >= y; } );
    return output;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<bool> operator<= ( const carto::VolumeRef<T> & vol, const carto::Volume<U> & other )
  {
    carto::VolumeRef<bool> output = carto::copyStructure<bool, T>( vol );
    carto::volumeutil::applyTowards( *vol, other, *output, [](const T &x, const U &y) { return x <= y; } );
    return output;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<bool> operator>  ( const carto::VolumeRef<T> & vol, const carto::Volume<U> & other )
  {
    carto::VolumeRef<bool> output = carto::copyStructure<bool, T>( vol );
    carto::volumeutil::applyTowards( *vol, other, *output, [](const T &x, const U &y) { return x > y; } );
    return output;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<bool> operator<  ( const carto::VolumeRef<T> & vol, const carto::Volume<U> & other )
  {
    carto::VolumeRef<bool> output = carto::copyStructure<bool, T>( vol );
    carto::volumeutil::applyTowards( *vol, other, *output, [](const T &x, const U &y) { return x < y; } );
    return output;
  }

  //--- Volume [op] VolumeRef --------------------------------------------------

  template <typename T, typename U>
  inline
carto::VolumeRef<bool> operator== ( const carto::Volume<T> & vol, const carto::VolumeRef<U> & other )
  {
    carto::Volume<bool> output = carto::copyStructure<bool, T>( vol );
    return carto::volumeutil::applyTowards( vol, *other, output, [](const T &x, const U &y) { return x == y; } );
  }

  template <typename T, typename U>
  inline
carto::VolumeRef<bool> operator!= ( const carto::Volume<T> & vol, const carto::VolumeRef<U> & other )
  {
    carto::Volume<bool> output = carto::copyStructure<bool, T>( vol );
    return carto::volumeutil::applyTowards( vol, *other, output, [](const T &x, const U &y) { return x != y; } );
  }

  template <typename T, typename U>
  inline
carto::VolumeRef<bool> operator>= ( const carto::Volume<T> & vol, const carto::VolumeRef<U> & other )
  {
    carto::Volume<bool> output = carto::copyStructure<bool, T>( vol );
    return carto::volumeutil::applyTowards( vol, *other, output, [](const T &x, const U &y) { return x >= y; } );
  }

  template <typename T, typename U>
  inline
carto::VolumeRef<bool> operator<= ( const carto::Volume<T> & vol, const carto::VolumeRef<U> & other )
  {
    carto::Volume<bool> output = carto::copyStructure<bool, T>( vol );
    return carto::volumeutil::applyTowards( vol, *other, output, [](const T &x, const U &y) { return x <= y; } );
  }

  template <typename T, typename U>
  inline
carto::VolumeRef<bool> operator>  ( const carto::Volume<T> & vol, const carto::VolumeRef<U> & other )
  {
    carto::Volume<bool> output = carto::copyStructure<bool, T>( vol );
    return carto::volumeutil::applyTowards( vol, *other, output, [](const T &x, const U &y) { return x > y; } );
  }

  template <typename T, typename U>
  inline
carto::VolumeRef<bool> operator<  ( const carto::Volume<T> & vol, const carto::VolumeRef<U> & other )
  {
    carto::Volume<bool> output = carto::copyStructure<bool, T>( vol );
    return carto::volumeutil::applyTowards( vol, *other, output, [](const T &x, const U &y) { return x < y; } );
  }

  //--- VolumeRef [op] VolumeRef -----------------------------------------------

  #if 0 // Would change rc_ptr behaviour
  template <typename T, typename U>
  inline
  carto::VolumeRef<bool> operator== ( const carto::VolumeRef<T> & vol, const carto::VolumeRef<U> & other )
  {
    carto::VolumeRef<bool> output = carto::copyStructure<bool, T>( vol );
    carto::volumeutil::applyTowards( *vol, *other, *output, [](const T &x, const U &y) { return x == y; } );
    return output;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<bool> operator!= ( const carto::VolumeRef<T> & vol, const carto::VolumeRef<U> & other )
  {
    carto::VolumeRef<bool> output = carto::copyStructure<bool, T>( vol );
    carto::volumeutil::applyTowards( *vol, *other, *output, [](const T &x, const U &y) { return x != y; } );
    return output;
  }
  #endif

  template <typename T, typename U>
  inline
  carto::VolumeRef<bool> operator>= ( const carto::VolumeRef<T> & vol, const carto::VolumeRef<U> & other )
  {
    carto::VolumeRef<bool> output = carto::copyStructure<bool, T>( vol );
    carto::volumeutil::applyTowards( *vol, *other, *output, [](const T &x, const U &y) { return x >= y; } );
    return output;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<bool> operator<= ( const carto::VolumeRef<T> & vol, const carto::VolumeRef<U> & other )
  {
    carto::VolumeRef<bool> output = carto::copyStructure<bool, T>( vol );
    carto::volumeutil::applyTowards( *vol, *other, *output, [](const T &x, const U &y) { return x <= y; } );
    return output;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<bool> operator>  ( const carto::VolumeRef<T> & vol, const carto::VolumeRef<U> & other )
  {
    carto::VolumeRef<bool> output = carto::copyStructure<bool, T>( vol );
    carto::volumeutil::applyTowards( *vol, *other, *output, [](const T &x, const U &y) { return x > y; } );
    return output;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<bool> operator<  ( const carto::VolumeRef<T> & vol, const carto::VolumeRef<U> & other )
  {
    carto::VolumeRef<bool> output = carto::copyStructure<bool, T>( vol );
    carto::volumeutil::applyTowards( *vol, *other, *output, [](const T &x, const U &y) { return x < y; } );
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
    return carto::volumeutil::apply( vol, [](const T &x) { return -x; } );
  }

  template <typename T>
  inline
  carto::VolumeRef<T> operator~ ( const carto::VolumeRef<T> & vol )
  {
    return carto::volumeutil::apply( vol, [](const T &x) { return ~x; } );
  }

  /** The "not" operator on a VolumeRef does NOT behave like rc_ptr operator
      "not", but rather like Volume operator "not": it returns a boolean
      volume, with element-wise test.
  */
  template <typename T>
  inline
  carto::VolumeRef<bool> operator! ( const carto::VolumeRef<T> & vol )
  {
    // if the input vol is null, also return a null result.
    if( vol.isNull() )
      return carto::VolumeRef<bool>();

    carto::VolumeRef<bool> output = carto::copyStructure<bool,T>( vol );
    carto::volumeutil::applyTowards( *vol, *output, [](const T &x) { return !x; } );
    return output;
  }

  //--- VolumeRef [op] Scalar --------------------------------------------------

  template <typename T, typename U>
  inline
  auto operator+ ( const carto::VolumeRef<T> & vol, const U & value )
  {
    return carto::volumeutil::apply( vol, [&value](const auto &p) {return p + value;} );
  }

  template <typename T, typename U>
  inline
  auto operator- ( const carto::VolumeRef<T> & vol, const U & value )
  {
    return carto::volumeutil::apply( vol, [&value](const auto &p) {return p - value;} );
  }

  template <typename T, typename U>
  inline
  auto operator* ( const carto::VolumeRef<T> & vol, const U & value )
  {
    return carto::volumeutil::apply( vol, [&value](const auto &p) {return p * value;} );
  }

  template <typename T, typename U>
  inline
  auto operator/ ( const carto::VolumeRef<T> & vol, const U & value )
  {
    return carto::volumeutil::apply( vol, [&value](const auto &p) {return p / value;} );
  }

  template <typename T, typename U>
  inline
  auto operator% ( const carto::VolumeRef<T> & vol, const U & value )
  {
    return carto::volumeutil::apply( vol, [&value](const auto &p) {return p % value;} );
  }

  template <typename T, typename U>
  inline
  auto operator& ( const carto::VolumeRef<T> & vol, const U & value )
  {
    return carto::volumeutil::apply( vol, [&value](const auto &p) {return p & value;} );
  }

  template <typename T, typename U>
  inline
  auto operator| ( const carto::VolumeRef<T> & vol, const U & value )
  {
    return carto::volumeutil::apply( vol, [&value](const auto &p) {return p | value;} );
  }

  template <typename T, typename U>
  inline
  auto operator^ ( const carto::VolumeRef<T> & vol, const U & value )
  {
    return carto::volumeutil::apply( vol, [&value](const auto &p) {return p ^ value;} );
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<bool> operator&& ( const carto::VolumeRef<T> & vol, const U & value )
  {
    carto::VolumeRef<bool> output = carto::copyStructure<bool,T>( vol );
    carto::volumeutil::applyTowards( *vol, *output, [&value](const auto &p) {return p && value;} );
    return output;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<bool> operator|| ( const carto::VolumeRef<T> & vol, const U & value )
  {
    carto::VolumeRef<bool> output = carto::copyStructure<bool,T>( vol );
    carto::volumeutil::applyTowards( *vol, *output, [&value](const auto &p) {return p || value;} );
    return output;
  }

  //--- Scalar [op] VolumeRef --------------------------------------------------

  template <typename T, typename U>
  inline
  auto operator+ ( const U & value, const carto::VolumeRef<T> & vol )
  {
    return carto::volumeutil::apply( vol, [&value](const auto &p) {return value + p;} );
  }

  template <typename T, typename U>
  inline
  auto operator- ( const U & value, const carto::VolumeRef<T> & vol )
  {
    return carto::volumeutil::apply( vol, [&value](const auto &p) {return value - p;} );
  }

  template <typename T, typename U>
  inline
  auto operator* ( const U & value, const carto::VolumeRef<T> & vol )
  {
    return carto::volumeutil::apply( vol, [&value](const auto &p) {return value * p;} );
  }

  template <typename T, typename U>
  inline
  auto operator/ ( const U & value, const carto::VolumeRef<T> & vol )
  {
    return carto::volumeutil::apply( vol, [&value](const auto &p) {return value / p;} );
  }

  //--- VolumeRef [op] Volume --------------------------------------------------

  template <typename T, typename U>
  inline
  auto operator+ ( const carto::VolumeRef<T> & vol, const carto::Volume<U> & other )
  {
    return carto::volumeutil::apply( vol, other, [](const T &x, const U &y) { return x + y; } );
  }

  template <typename T, typename U>
  inline
  auto operator- ( const carto::VolumeRef<T> & vol, const carto::Volume<U> & other )
  {
    return carto::volumeutil::apply( vol, other, [](const T &x, const U &y) { return x - y; } );
  }

  template <typename T, typename U>
  inline
  auto operator* ( const carto::VolumeRef<T> & vol, const carto::Volume<U> & other )
  {
    return carto::volumeutil::apply( vol, other, [](const T &x, const U &y) { return x * y; } );
  }

  template <typename T, typename U>
  inline
  auto operator/ ( const carto::VolumeRef<T> & vol, const carto::Volume<U> & other )
  {
    return carto::volumeutil::apply( vol, other, [](const T &x, const U &y) { return x / y; } );
  }

  template <typename T, typename U>
  inline
  auto operator% ( const carto::VolumeRef<T> & vol, const carto::Volume<U> & other )
  {
    return carto::volumeutil::apply( vol, other, [](const T &x, const U &y) { return x % y; } );
  }

  template <typename T, typename U>
  inline
  auto operator& ( const carto::VolumeRef<T> & vol, const carto::Volume<U> & other )
  {
    return carto::volumeutil::apply( vol, other, [](const T &x, const U &y) { return x & y; } );
  }

  template <typename T, typename U>
  inline
  auto operator| ( const carto::VolumeRef<T> & vol, const carto::Volume<U> & other )
  {
    return carto::volumeutil::apply( vol, other, [](const T &x, const U &y) { return x | y; } );
  }

  template <typename T, typename U>
  inline
  auto operator^ ( const carto::VolumeRef<T> & vol, const carto::Volume<U> & other )
  {
    return carto::volumeutil::apply( vol, other, [](const T &x, const U &y) { return x ^ y; } );
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<bool> operator&& ( const carto::VolumeRef<T> & vol, const carto::Volume<U> & other )
  {
    carto::VolumeRef<bool> output = carto::copyStructure<bool,T>( vol );
    carto::volumeutil::applyTowards( *vol, other, *output, [](const T &x, const U &y) { return x && y; } );
    return output;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<bool> operator|| ( const carto::VolumeRef<T> & vol, const carto::Volume<U> & other )
  {
    carto::VolumeRef<bool> output = carto::copyStructure<bool,T>( vol );
    carto::volumeutil::applyTowards( *vol, other, *output, [](const T &x, const U &y) { return x || y; } );
    return output;
  }

  //--- Volume [op] VolumeRef --------------------------------------------------

  template <typename T, typename U>
  inline
  auto operator+ ( const carto::Volume<T> & vol, const carto::VolumeRef<U> & other )
  {
    return carto::volumeutil::apply( vol, *other, [](const T &x, const U &y) { return x + y; } );
  }

  template <typename T, typename U>
  inline
  auto operator- ( const carto::Volume<T> & vol, const carto::VolumeRef<U> & other )
  {
    return carto::volumeutil::apply( vol, *other, [](const T &x, const U &y) { return x - y; } );
  }

  template <typename T, typename U>
  inline
  auto operator* ( const carto::Volume<T> & vol, const carto::VolumeRef<U> & other )
  {
    return carto::volumeutil::apply( vol, *other, [](const T &x, const U &y) { return x * y; } );
  }

  template <typename T, typename U>
  inline
  auto operator/ ( const carto::Volume<T> & vol, const carto::VolumeRef<U> & other )
  {
    return carto::volumeutil::apply( vol, *other, [](const T &x, const U &y) { return x / y; } );
  }

  template <typename T, typename U>
  inline
  auto operator% ( const carto::Volume<T> & vol, const carto::VolumeRef<U> & other )
  {
    return carto::volumeutil::apply( vol, *other, [](const T &x, const U &y) { return x % y; });
  }

  template <typename T, typename U>
  inline
  auto operator& ( const carto::Volume<T> & vol, const carto::VolumeRef<U> & other )
  {
    return carto::volumeutil::apply( vol, *other, [](const T &x, const U &y) { return x & y; } );
  }

  template <typename T, typename U>
  inline
  auto operator| ( const carto::Volume<T> & vol, const carto::VolumeRef<U> & other )
  {
    return carto::volumeutil::apply( vol, *other, [](const T &x, const U &y) { return x | y; } );
  }

  template <typename T, typename U>
  inline
  auto operator^ ( const carto::Volume<T> & vol, const carto::VolumeRef<U> & other )
  {
    return carto::volumeutil::apply( vol, *other, [](const T &x, const U &y) { return x ^ y; } );
  }

  template <typename T, typename U>
  inline
  carto::Volume<bool> operator&& ( const carto::Volume<T> & vol, const carto::VolumeRef<U> & other )
  {
    carto::Volume<bool> output = carto::copyStructure<bool,T>( vol );
    return carto::volumeutil::applyTowards( vol, *other, output, [](const T &x, const U &y) { return x && y; } );
  }
  template <typename T, typename U>
  inline
  carto::Volume<bool> operator|| ( const carto::Volume<T> & vol, const carto::VolumeRef<U> & other )
  {
    carto::Volume<bool> output = carto::copyStructure<bool,T>( vol );
    return carto::volumeutil::applyTowards( vol, *other, output, [](const T &x, const U &y) { return x || y; } );
  }

  //--- VolumeRef [op] VolumeRef -----------------------------------------------

  template <typename T, typename U>
  inline
  auto operator+ ( const carto::VolumeRef<T> & vol, const carto::VolumeRef<U> & other )
  {
    return carto::volumeutil::apply( vol, *other, [](const T &x, const U &y) { return x + y; } );
  }

  template <typename T, typename U>
  inline
  auto operator- ( const carto::VolumeRef<T> & vol, const carto::VolumeRef<U> & other )
  {
    return carto::volumeutil::apply( vol, *other, [](const T &x, const U &y) { return x - y; } );
  }

  template <typename T, typename U>
  inline
  auto operator* ( const carto::VolumeRef<T> & vol, const carto::VolumeRef<U> & other )
  {
    return carto::volumeutil::apply( vol, *other, [](const T &x, const U &y) { return x * y; } );
  }

  template <typename T, typename U>
  inline
  auto operator/ ( const carto::VolumeRef<T> & vol, const carto::VolumeRef<U> & other )
  {
    return carto::volumeutil::apply( vol, *other, [](const T &x, const U &y) { return x / y; } );
  }

  template <typename T, typename U>
  inline
  auto operator% ( const carto::VolumeRef<T> & vol, const carto::VolumeRef<U> & other )
  {
    return carto::volumeutil::apply( vol, *other, [](const T &x, const U &y) { return x % y; } );
  }

  template <typename T, typename U>
  inline
  auto operator& ( const carto::VolumeRef<T> & vol, const carto::VolumeRef<U> & other )
  {
    return carto::volumeutil::apply( vol, *other, [](const T &x, const U &y) { return x & y; } );
  }

  template <typename T, typename U>
  inline
  auto operator| ( const carto::VolumeRef<T> & vol, const carto::VolumeRef<U> & other )
  {
    return carto::volumeutil::apply( vol, *other, [](const T &x, const U &y) { return x | y; } );
  }

  template <typename T, typename U>
  inline
  auto operator^ ( const carto::VolumeRef<T> & vol, const carto::VolumeRef<U> & other )
  {
    return carto::volumeutil::apply( vol, *other, [](const T &x, const U &y) { return x ^ y; } );
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<bool> operator&& ( const carto::VolumeRef<T> & vol, const carto::VolumeRef<U> & other )
  {
    carto::VolumeRef<bool> output = carto::copyStructure<bool,T>( vol );
    carto::volumeutil::applyTowards( *vol, *other, *output, [](const T &x, const U &y) { return x && y; } );
    return output;
  }
  template <typename T, typename U>
  inline
  carto::VolumeRef<bool> operator|| ( const carto::VolumeRef<T> & vol, const carto::VolumeRef<U> & other )
  {
    carto::VolumeRef<bool> output = carto::copyStructure<bool,T>( vol );
    carto::volumeutil::applyTowards( *vol, *other, *output, [](const T &x, const U &y) { return x || y; } );
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
    carto::volumeutil::selfApply( vol, [&value](const T &p) {return p + value;} );
    return vol;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<T> & operator-= ( carto::VolumeRef<T> & vol, const U & value )
  {
    carto::volumeutil::selfApply( vol, [&value](const T &p) {return p - value;} );
    return vol;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<T> & operator*= ( carto::VolumeRef<T> & vol, const U & value )
  {
    carto::volumeutil::selfApply( vol, [&value](const T &p) {return p * value;} );
    return vol;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<T> & operator/= ( carto::VolumeRef<T> & vol, const U & value )
  {
    carto::volumeutil::selfApply( vol, [&value](const T &p) {return p / value;} );
    return vol;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<T> & operator%= ( carto::VolumeRef<T> & vol, const U & value )
  {
    carto::volumeutil::selfApply( vol, [&value](const T &p) {return p % value;} );
    return vol;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<T> & operator&= ( carto::VolumeRef<T> & vol, const U & value )
  {
    carto::volumeutil::selfApply( vol, [&value](const T &p) {return p & value;} );
    return vol;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<T> & operator|= ( carto::VolumeRef<T> & vol, const U & value )
  {
    carto::volumeutil::selfApply( vol, [&value](const T &p) {return p | value;} );
    return vol;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<T> & operator^= ( carto::VolumeRef<T> & vol, const U & value )
  {
    carto::volumeutil::selfApply( vol, [&value](const T &p) {return p ^ value;} );
    return vol;
  }

  //--- VolumeRef [op] Volume --------------------------------------------------

  template <typename T, typename U>
  inline
  carto::VolumeRef<T> & operator+= ( carto::VolumeRef<T> & vol, const carto::Volume<U> & other )
  {
    carto::volumeutil::selfApply( vol, other, [](const T &x, const U &y) { return x + y; } );
    return vol;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<T> & operator-= ( carto::VolumeRef<T> & vol, const carto::Volume<U> & other )
  {
    carto::volumeutil::selfApply( vol, other, [](const T &x, const U &y) { return x - y; } );
    return vol;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<T> & operator*= ( carto::VolumeRef<T> & vol, const carto::Volume<U> & other )
  {
    carto::volumeutil::selfApply( vol, other, [](const T &x, const U &y) { return x * y; } );
    return vol;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<T> & operator/= ( carto::VolumeRef<T> & vol, const carto::Volume<U> & other )
  {
    carto::volumeutil::selfApply( vol, other, [](const T &x, const U &y) { return x / y; } );
    return vol;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<T> & operator%= ( carto::VolumeRef<T> & vol, const carto::Volume<U> & other )
  {
    carto::volumeutil::selfApply( vol, other, [](const T &x, const U &y) { return x % y; } );
    return vol;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<T> & operator&= ( carto::VolumeRef<T> & vol, const carto::Volume<U> & other )
  {
    carto::volumeutil::selfApply( vol, other, [](const T &x, const U &y) { return x & y; } );
    return vol;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<T> & operator|= ( carto::VolumeRef<T> & vol, const carto::Volume<U> & other )
  {
    carto::volumeutil::selfApply( vol, other, [](const T &x, const U &y) { return x | y; } );
    return vol;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<T> & operator^= ( carto::VolumeRef<T> & vol, const carto::Volume<U> & other )
  {
    carto::volumeutil::selfApply( vol, other, [](const T &x, const U &y) { return x ^ y; } );
    return vol;
  }

  //--- Volume [op]= VolumeRef -------------------------------------------------

  template <typename T, typename U>
  inline
  carto::Volume<T> & operator+= ( carto::Volume<T> & vol, const carto::VolumeRef<U> & other )
  {
    return carto::volumeutil::selfApply( vol, *other, [](const T &x, const U &y) { return x + y; } );
  }

  template <typename T, typename U>
  inline
  carto::Volume<T> & operator-= ( carto::Volume<T> & vol, const carto::VolumeRef<U> & other )
  {
    return carto::volumeutil::selfApply( vol, *other, [](const T &x, const U &y) { return x - y; } );
  }

  template <typename T, typename U>
  inline
  carto::Volume<T> & operator*= ( carto::Volume<T> & vol, const carto::VolumeRef<U> & other )
  {
    return carto::volumeutil::selfApply( vol, *other, [](const T &x, const U &y) { return x - y; } );
  }

  template <typename T, typename U>
  inline
  carto::Volume<T> & operator/= ( carto::Volume<T> & vol, const carto::VolumeRef<U> & other )
  {
    return carto::volumeutil::selfApply( vol, *other, [](const T &x, const U &y) { return x / y; } );
  }

  template <typename T, typename U>
  inline
  carto::Volume<T> & operator%= ( carto::Volume<T> & vol, const carto::VolumeRef<U> & other )
  {
    return carto::volumeutil::selfApply( vol, *other, [](const T &x, const U &y) { return x % y; } );
  }

  template <typename T, typename U>
  inline
  carto::Volume<T> & operator&= ( carto::Volume<T> & vol, const carto::VolumeRef<U> & other )
  {
    return carto::volumeutil::selfApply( vol, *other, [](const T &x, const U &y) { return x & y; } );
  }

  template <typename T, typename U>
  inline
  carto::Volume<T> & operator|= ( carto::Volume<T> & vol, const carto::VolumeRef<U> & other )
  {
    return carto::volumeutil::selfApply( vol, *other, [](const T &x, const U &y) { return x | y; } );
  }

  template <typename T, typename U>
  inline
  carto::Volume<T> & operator^= ( carto::Volume<T> & vol, const carto::VolumeRef<U> & other )
  {
    return carto::volumeutil::selfApply( vol, *other, [](const T &x, const U &y) { return x ^ y; } );
  }

  //--- VolumeRef [op] VolumeRef -----------------------------------------------

  template <typename T, typename U>
  inline
  carto::VolumeRef<T> & operator+= ( carto::VolumeRef<T> & vol, const carto::VolumeRef<U> & other )
  {
    carto::volumeutil::selfApply( vol, *other, [](const T &x, const U &y) { return x + y; } );
    return vol;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<T> & operator-= ( carto::VolumeRef<T> & vol, const carto::VolumeRef<U> & other )
  {
    carto::volumeutil::selfApply( vol, *other, [](const T &x, const U &y) { return x - y; } );
    return vol;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<T> & operator*= ( carto::VolumeRef<T> & vol, const carto::VolumeRef<U> & other )
  {
    carto::volumeutil::selfApply( vol, *other, [](const T &x, const U &y) { return x * y; } );
    return vol;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<T> & operator/= ( carto::VolumeRef<T> & vol, const carto::VolumeRef<U> & other )
  {
    carto::volumeutil::selfApply( vol, *other, [](const T &x, const U &y) { return x / y; } );
    return vol;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<T> & operator%= ( carto::VolumeRef<T> & vol, const carto::VolumeRef<U> & other )
  {
    carto::volumeutil::selfApply( vol, *other, [](const T &x, const U &y) { return x % y; } );
    return vol;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<T> & operator&= ( carto::VolumeRef<T> & vol, const carto::VolumeRef<U> & other )
  {
    carto::volumeutil::selfApply( vol, *other, [](const T &x, const U &y) { return x & y; } );
    return vol;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<T> & operator|= ( carto::VolumeRef<T> & vol, const carto::VolumeRef<U> & other )
  {
    carto::volumeutil::selfApply( vol, *other, [](const T &x, const U &y) { return x | y; } );
    return vol;
  }

  template <typename T, typename U>
  inline
  carto::VolumeRef<T> & operator^= ( carto::VolumeRef<T> & vol, const carto::VolumeRef<U> & other )
  {
    carto::volumeutil::selfApply( vol, *other, [](const T &x, const U &y) { return x ^ y; } );
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
    carto::volumeutil::selfApply( vol, [](const T &x) { return ++x; } );
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
    carto::volumeutil::selfApply( vol, [](const T &x) { return --x; } );
    return vol;
  }

#endif // CARTO_VOLUME_AUTO_DEREFERENCE

} // namespace carto

#endif // CARTODATA_VOLUME_VOLUMEREF_D_OPERATORS_H
