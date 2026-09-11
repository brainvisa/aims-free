#ifndef CARTODATA_VOLUME_VOLUMEBASE_D_OPERATORS_H
#define CARTODATA_VOLUME_VOLUMEBASE_D_OPERATORS_H


//--- cartodata --------------------------------------------------------------
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
    struct select_is_scalar
    {
      static T min( const Volume<T> & )
      {
        throw std::logic_error( "Member function min() is only enabled for "
          "volumes of scalar. Try to use the non-member function "
          "carto::min(Volume<T>) instead." );
      }

      static T max( const Volume<T> & )
      {
        throw std::logic_error( "Member function max() is only enabled for "
          "volumes of scalar. Try to use the non-member function "
          "carto::max(Volume<T>) instead." );
      }

      static typename DataTypeTraits<T>::LongType
      sum( const Volume<T> & )
      {
        throw std::logic_error( "Member function sum() is only enabled for "
          "volumes of scalar. Try to use the non-member function "
          "carto::sum(Volume<T>) instead." );
      }

    };

    template <typename T, bool has_bool_conversion = DataTypeTraits<T>::has_bool_conversion>
    struct select_has_bool_conversion
    {
      static bool all( const Volume<T> & )
      {
        throw std::logic_error( "Member function all() is only enabled for "
          "volumes of types that support boolean conversion. Try to use the "
          "non-member function carto::all(Volume<T>) instead." );
      }

      static bool any( const Volume<T> & )
      {
        throw std::logic_error( "Member function any() is only enabled for "
          "volumes of that support boolean conversion. Try to use the "
          "non-member function carto::any(Volume<T>) instead." );
      }
    };


    template <typename T>
    struct select_is_scalar<T, true>
    {
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

    template <typename T>
    struct select_has_bool_conversion<T, true>
    {
      static bool all( const Volume<T> & vol )
      {
        return carto::all(vol);
      }

      static bool any( const Volume<T> & vol )
      {
        return carto::any(vol);
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
    return volumebaseinternal::select_has_bool_conversion<T>::all( *this );
  }

  template <typename T>
  inline
  bool Volume<T>::any() const
  {
    return volumebaseinternal::select_has_bool_conversion<T>::any( *this );
  }

  template <typename T>
  inline
  Volume<T>::operator bool() const
  {
    return volumebaseinternal::select_has_bool_conversion<T>::all( *this );
  }

  template <typename T>
  inline
  T Volume<T>::min() const
  {
    return volumebaseinternal::select_is_scalar<T>::min( *this );
  }

  template <typename T>
  inline
  T Volume<T>::max() const
  {
    return volumebaseinternal::select_is_scalar<T>::max( *this );
  }

  template <typename T>
  inline
  typename DataTypeTraits<T>::LongType Volume<T>::sum() const
  {
    //return ::carto::sum( *this );
    return volumebaseinternal::select_is_scalar<T>::sum( *this );
  }


  //==========================================================================
  //   FILL / REPLACE
  //==========================================================================

  template <typename T>
  inline
  void Volume<T>::fill( const T & value )
  {
    volumeutil::selfApply( *this, [&value](const auto &p) { return value; } );
  }

  template <typename T>
  inline
  Volume<T> & Volume<T>::operator= ( const T & value )
  {
    this->fill( value );
    return *this;
  }

  /// Fill borders with a constant value
  template <typename T>
  inline
  void Volume<T>::fillBorder( const T & value )
  {
    rc_ptr<Volume<T> > ref = refVolume();
    if( !ref )
      return;
    typename Volume<T>::Position pos = posInRefVolume();
    std::vector<int> sz = ref->getSize();
    std::vector<int> vsz = this->getSize();
    int dim;
    int nd = sz.size();
    int vnd = vsz.size();
    int np = pos.size();
    std::vector<int> dmin( nd, 0 ), dmax( nd, 0 );
    std::vector<int> ppos( nd, 0 );
    std::vector<bool> is_inside( nd, true );
    int i;

    for( dim=nd - 1; dim>=0; --dim )
    {
      if( dim < vnd )
      {
        if( dim < np )
          dmin[dim] = pos[dim];
        else
          dmin[dim] = 0;
        dmax[dim] = vsz[dim] + dmin[dim];
      }
      else
      {
        dmin[dim] = 0;
        dmax[dim] = sz[dim];
      }
      ppos[dim] = 0;
      is_inside[dim] = (dmin[dim] == 0 );
      if( dim < nd - 1 )
        is_inside[dim] = is_inside[dim] && is_inside[dim + 1];
    }

    while( true )
    {
      // fill one line along dim0
      if( is_inside[1] )
      {
        for( i=0; i<dmin[0]; ++i )
        {
          ppos[0] = i;
          ref->at( ppos ) = value;
        }
        for( i=dmax[0]; i<sz[0]; ++i )
        {
          ppos[0] = i;
          ref->at( ppos ) = value;
        }
      }
      else
      {
        for( i=0; i<sz[0]; ++i )
        {
          ppos[0] = i;
          ref->at( ppos ) = value;
        }
      }
      // increment line
      ++ppos[1];
      dim = 1;
      while( ppos[dim] >= sz[dim] )
      {
        ppos[dim] = 0;
        ++dim;
        if( dim >= nd )
          break;
        ++ppos[dim];
      }
      if( dim >= nd )
        break;
      // check if we are inside the view
      for( ; dim > 0; --dim )
      {
        is_inside[dim] = ( dim < nd - 1 ? is_inside[dim + 1] : true  );
        is_inside[dim] = is_inside[dim] && ( ppos[dim] >= dmin[dim] )
                          && ( ppos[dim] < dmax[dim] );
      }
    }
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
    return carto::volumeutil::applyTowards( vol, output, [&value](const auto &p) {return p == value;} );
  }

  template <typename T, typename U>
  inline
  carto::Volume<bool> operator!= ( const carto::Volume<T> & vol, const U & value )
  {
    carto::Volume<bool> output = carto::copyStructure<bool, T>( vol );
    return carto::volumeutil::applyTowards( vol, output, [&value](const auto &p) {return p != value;} );
  }

  template <typename T, typename U>
  inline
  carto::Volume<bool> operator>= ( const carto::Volume<T> & vol, const U & value )
  {
    carto::Volume<bool> output = carto::copyStructure<bool, T>( vol );
    return carto::volumeutil::applyTowards( vol, output, [&value](const auto &p) {return p >= value;} );
  }

  template <typename T, typename U>
  inline
  carto::Volume<bool> operator<= ( const carto::Volume<T> & vol, const U & value )
  {
    carto::Volume<bool> output = carto::copyStructure<bool, T>( vol );
    return carto::volumeutil::applyTowards( vol, output, [&value](const auto &p) {return p <= value;} );
  }

  template <typename T, typename U>
  inline
  carto::Volume<bool> operator>  ( const carto::Volume<T> & vol, const U & value )
  {
    carto::Volume<bool> output = carto::copyStructure<bool, T>( vol );
    return carto::volumeutil::applyTowards( vol, output, [&value](const auto &p) {return p > value;} );
  }

  template <typename T, typename U>
  inline
  carto::Volume<bool> operator<  ( const carto::Volume<T> & vol, const U & value )
  {
    carto::Volume<bool> output = carto::copyStructure<bool, T>( vol );
    return carto::volumeutil::applyTowards( vol, output, [&value](const auto &p) {return p < value;} );
  }

  //--- Scalar [op] Volume -----------------------------------------------------

   template <typename T, typename U>
   inline
   carto::Volume<bool> operator== ( const U & value, const carto::Volume<T> & vol )
   {
     carto::Volume<bool> output = carto::copyStructure<bool, T>( vol );
     return carto::volumeutil::applyTowards( vol, output, [&value](const auto &p) {return value == p;} );
   }

   template <typename T, typename U>
   inline
   carto::Volume<bool> operator!= ( const U & value, const carto::Volume<T> & vol )
   {
     carto::Volume<bool> output = carto::copyStructure<bool, T>( vol );
     return carto::volumeutil::applyTowards( vol, output, [&value](const auto &p) {return value !=  p;} );
   }

   template <typename T, typename U>
   inline
   carto::Volume<bool> operator>= ( const U & value, const carto::Volume<T> & vol )
   {
     carto::Volume<bool> output = carto::copyStructure<bool, T>( vol );
     return carto::volumeutil::applyTowards( vol, output, [&value](const auto &p) {return value >= p;} );
   }

   template <typename T, typename U>
   inline
   carto::Volume<bool> operator<= ( const U & value, const carto::Volume<T> & vol )
   {
     carto::Volume<bool> output = carto::copyStructure<bool, T>( vol );
     return carto::volumeutil::applyTowards( vol, output, [&value](const auto &p) {return value <= p;} );
   }

   template <typename T, typename U>
   inline
   carto::Volume<bool> operator>  ( const U & value, const carto::Volume<T> & vol )
   {
     carto::Volume<bool> output = carto::copyStructure<bool, T>( vol );
     return carto::volumeutil::applyTowards( vol, output, [&value](const auto &p) {return value > p;} );
   }

   template <typename T, typename U>
   inline
   carto::Volume<bool> operator<  ( const U & value, const carto::Volume<T> & vol )
   {
     carto::Volume<bool> output = carto::copyStructure<bool, T>( vol );
     return carto::volumeutil::applyTowards( vol, output, [&value](const auto &p) {return value < p;} );
   }

  //--- Volume [op] Volume -----------------------------------------------------

  template <typename T, typename U>
  inline
  carto::Volume<bool> operator== ( const carto::Volume<T> & vol, const carto::Volume<U> & other )
  {
    carto::Volume<bool> output = carto::copyStructure<bool, T>( vol );
    return carto::volumeutil::applyTowards( vol, other, output, [](const T &x, const U &y) { return x == y; } );
  }

  template <typename T, typename U>
  inline
  carto::Volume<bool> operator!= ( const carto::Volume<T> & vol, const carto::Volume<U> & other )
  {
    carto::Volume<bool> output = carto::copyStructure<bool, T>( vol );
    return carto::volumeutil::applyTowards( vol, other, output, [](const T &x, const U &y) { return x != y; } );
  }

  template <typename T, typename U>
  inline
  carto::Volume<bool> operator>= ( const carto::Volume<T> & vol, const carto::Volume<U> & other )
  {
    carto::Volume<bool> output = carto::copyStructure<bool, T>( vol );
    return carto::volumeutil::applyTowards( vol, other, output, [](const T &x, const U &y) { return x >= y; } );
  }

  template <typename T, typename U>
  inline
  carto::Volume<bool> operator<= ( const carto::Volume<T> & vol, const carto::Volume<U> & other )
  {
    carto::Volume<bool> output = carto::copyStructure<bool, T>( vol );
    return carto::volumeutil::applyTowards( vol, other, output, [](const T &x, const U &y) { return x <= y; } );
  }

  template <typename T, typename U>
  inline
  carto::Volume<bool> operator>  ( const carto::Volume<T> & vol, const carto::Volume<U> & other )
  {
    carto::Volume<bool> output = carto::copyStructure<bool, T>( vol );
    return carto::volumeutil::applyTowards( vol, other, output, [](const T &x, const U &y) { return x > y; } );
  }

  template <typename T, typename U>
  inline
  carto::Volume<bool> operator<  ( const carto::Volume<T> & vol, const carto::Volume<U> & other )
  {
    carto::Volume<bool> output = carto::copyStructure<bool, T>( vol );
    return carto::volumeutil::applyTowards( vol, other, output, [](const T &x, const U &y) { return x < y; } );
  }

  //============================================================================
  //   OPERATORS : ARITHMETICS
  //============================================================================

  //--- [op] Volume ------------------------------------------------------------

  template <typename T>
  inline
  carto::Volume<T> operator- ( const carto::Volume<T> & vol )
  {
    return carto::volumeutil::apply( vol, [](const T &x) { return -x; } );
  }

  template <typename T>
  inline
  carto::Volume<T> operator~ ( const carto::Volume<T> & vol )
  {
    return carto::volumeutil::apply( vol, [](const T &x) { return ~x; } );
  }

  template <typename T>
  inline
  carto::Volume<bool> operator! ( const carto::Volume<T> & vol )
  {
    carto::Volume<bool> output = carto::copyStructure<bool,T>( vol );
    return carto::volumeutil::applyTowards( vol, output, [](const T &x) { return !x; } );
  }

  //--- Volume [op] Scalar -----------------------------------------------------

  template <typename T, typename U>
  inline
  auto operator+ ( const carto::Volume<T> & vol, const U & value )
  {
    return carto::volumeutil::apply( vol, [&value](const auto &p) {return p + value;} );
  }

  template <typename T, typename U>
  inline
  auto operator- ( const carto::Volume<T> & vol, const U & value )
  {
    return carto::volumeutil::apply( vol, [&value](const auto &p) {return p - value;} );
  }

  template <typename T, typename U>
  inline
  auto operator* ( const carto::Volume<T> & vol, const U & value )
  {
    return carto::volumeutil::apply( vol, [&value](const auto &p) {return p * value;} );
  }

  template <typename T, typename U>
  inline
  auto operator/ ( const carto::Volume<T> & vol, const U & value )
  {
    return carto::volumeutil::apply( vol, [&value](const auto &p) {return p / value;} );
  }

  template <typename T, typename U>
  inline
  auto operator% ( const carto::Volume<T> & vol, const U & value )
  {
    return carto::volumeutil::apply( vol, [&value](const auto &p) {return p % value;} );
  }

  template <typename T, typename U>
  inline
  auto operator& ( const carto::Volume<T> & vol, const U & value )
  {
    return carto::volumeutil::apply( vol, [&value](const auto &p) {return p & value;} );
  }

  template <typename T, typename U>
  inline
  auto operator| ( const carto::Volume<T> & vol, const U & value )
  {
    return carto::volumeutil::apply( vol, [&value](const auto &p) {return p | value;} );
  }

  template <typename T, typename U>
  inline
  auto operator^ ( const carto::Volume<T> & vol, const U & value )
  {
    return carto::volumeutil::apply( vol, [&value](const auto &p) {return p ^ value;} );
  }

  template <typename T, typename U>
  inline
  carto::Volume<bool> operator&& ( const carto::Volume<T> & vol, const U & value )
  {
    carto::Volume<bool> output = carto::copyStructure<bool,T>( vol );
    return carto::volumeutil::applyTowards( vol, output, [&value](const auto &p) {return p && value;} );
  }

  template <typename T, typename U>
  inline
  carto::Volume<bool> operator|| ( const carto::Volume<T> & vol, const U & value )
  {
    carto::Volume<bool> output = carto::copyStructure<bool,T>( vol );
    return carto::volumeutil::applyTowards( vol, output, [&value](const auto &p) {return p || value;} );
  }

  //--- Scalar [op] Volume -----------------------------------------------------

  template <typename T, typename U>
  inline
  auto operator+ ( const U & value, const carto::Volume<T> & vol )
  {
    return carto::volumeutil::apply( vol, [&value](const auto &p) {return value + p;} );
  }

  template <typename T, typename U>
  inline
  auto operator- ( const U & value, const carto::Volume<T> & vol )
  {
    return carto::volumeutil::apply( vol, [&value](const auto &p) {return value - p;} );
  }

  template <typename T, typename U>
  inline
  auto operator* ( const U & value, const carto::Volume<T> & vol )
  {
    return carto::volumeutil::apply( vol, [&value](const auto &p) {return value * p;} );
  }

  template <typename T, typename U>
  inline
  auto operator/ ( const U & value, const carto::Volume<T> & vol )
  {
    return carto::volumeutil::apply( vol, [&value](const auto &p) {return value / p;} );
  }

  //--- Volume [op] Volume -----------------------------------------------------

  template <typename T, typename U>
  inline
  auto operator+ ( const carto::Volume<T> & vol, const carto::Volume<U> & other )
  {
    return carto::volumeutil::apply( vol, other, [](const T &x, const U &y) { return x+y; } );
  }

  template <typename T, typename U>
  inline
  auto operator- ( const carto::Volume<T> & vol, const carto::Volume<U> & other )
  {
    return carto::volumeutil::apply( vol, other, [](const T &x, const U &y) { return x-y; } );
  }

  template <typename T, typename U>
  inline
  auto operator* ( const carto::Volume<T> & vol, const carto::Volume<U> & other )
  {
    return carto::volumeutil::apply( vol, other, [](const T &x, const U &y) { return x*y; } );
  }

  template <typename T, typename U>
  inline
  auto operator/ ( const carto::Volume<T> & vol, const carto::Volume<U> & other )
  {
    return carto::volumeutil::apply( vol, other, [](const T &x, const U &y) { return x/y; } );
  }

  template <typename T, typename U>
  inline
  auto operator% ( const carto::Volume<T> & vol, const carto::Volume<U> & other )
  {
    return carto::volumeutil::apply( vol, other, [](const T &x, const U &y) { return x%y; } );
  }

  template <typename T, typename U>
  inline
  auto operator& ( const carto::Volume<T> & vol, const carto::Volume<U> & other )
  {
    return carto::volumeutil::apply( vol, other, [](const T &x, const U &y) { return x&y; } );
  }

  template <typename T, typename U>
  inline
  auto operator| ( const carto::Volume<T> & vol, const carto::Volume<U> & other )
  {
    return carto::volumeutil::apply( vol, other, [](const T &x, const U &y) { return x|y; } );
  }

  template <typename T, typename U>
  inline
  auto operator^ ( const carto::Volume<T> & vol, const carto::Volume<U> & other )
  {
    return carto::volumeutil::apply( vol, other, [](const T &x, const U &y) { return x^y; } );
  }

  template <typename T, typename U>
  inline
  carto::Volume<bool> operator&& ( const carto::Volume<T> & vol, const carto::Volume<U> & other )
  {
    carto::Volume<bool> output = carto::copyStructure<bool,T>( vol );
    return carto::volumeutil::applyTowards( vol, other, output, [](const T &x, const U &y) { return x&&y; } );
  }

  template <typename T, typename U>
  inline
  carto::Volume<bool> operator|| ( const carto::Volume<T> & vol, const carto::Volume<U> & other )
  {
    carto::Volume<bool> output = carto::copyStructure<bool,T>( vol );
    return carto::volumeutil::applyTowards( vol, other, output, [](const T &x, const U &y) { return x||y; } );
  }

  //============================================================================
  //   OPERATORS: ARITHMETIC MODIFIERS
  //============================================================================

  //--- Volume [op]= Scalar ----------------------------------------------------

  template <typename T, typename U>
  inline
  carto::Volume<T> & operator+= ( carto::Volume<T> & vol, const U & value )
  {
    return carto::volumeutil::selfApply( vol, [&value](const auto &p) {return p + value;} );
  }

  template <typename T, typename U>
  inline
  carto::Volume<T> & operator-= ( carto::Volume<T> & vol, const U & value )
  {
    return carto::volumeutil::selfApply( vol, [&value](const auto &p) {return p - value;} );
  }

  template <typename T, typename U>
  inline
  carto::Volume<T> & operator*= ( carto::Volume<T> & vol, const U & value )
  {
    return carto::volumeutil::selfApply( vol, [&value](const auto &p) {return p * value;} );
  }

  template <typename T, typename U>
  inline
  carto::Volume<T> & operator/= ( carto::Volume<T> & vol, const U & value )
  {
    return carto::volumeutil::selfApply( vol, [&value](const auto &p) {return p / value;} );
  }

  template <typename T, typename U>
  inline
  carto::Volume<T> & operator%= ( carto::Volume<T> & vol, const U & value )
  {
    return carto::volumeutil::selfApply( vol, [&value](const auto &p) {return p % value;} );
  }

  template <typename T, typename U>
  inline
  carto::Volume<T> & operator&= ( carto::Volume<T> & vol, const U & value )
  {
    return carto::volumeutil::selfApply( vol, [&value](const auto &p) {return p & value;} );
  }

  template <typename T, typename U>
  inline
  carto::Volume<T> & operator|= ( carto::Volume<T> & vol, const U & value )
  {
    return carto::volumeutil::selfApply( vol, [&value](const auto &p) {return p | value;} );
  }

  template <typename T, typename U>
  inline
  carto::Volume<T> & operator^= ( carto::Volume<T> & vol, const U & value )
  {
    return carto::volumeutil::selfApply( vol, [&value](const auto &p) {return p ^ value;} );
  }

  //--- Volume [op]= Volume ----------------------------------------------------

  template <typename T, typename U>
  inline
  carto::Volume<T> & operator+= ( carto::Volume<T> & vol, const carto::Volume<U> & other )
  {
    return carto::volumeutil::selfApply( vol, other, [](const T &x, const U& y) { return volumeutil::cast_operator_result<T,U>(x + y); } );
  }

  template <typename T, typename U>
  inline
  carto::Volume<T> & operator-= ( carto::Volume<T> & vol, const carto::Volume<U> & other )
  {
    return carto::volumeutil::selfApply( vol, other, [](const T &x, const U& y) { return volumeutil::cast_operator_result<T,U>(x - y); } );
  }

  template <typename T, typename U>
  inline
  carto::Volume<T> & operator*= ( carto::Volume<T> & vol, const carto::Volume<U> & other )
  {
    return carto::volumeutil::selfApply( vol, other, [](const T &x, const U& y) { return volumeutil::cast_operator_result<T,U>(x * y); } );
  }

  template <typename T, typename U>
  inline
  carto::Volume<T> & operator/= ( carto::Volume<T> & vol, const carto::Volume<U> & other )
  {
    return carto::volumeutil::selfApply( vol, other, [](const T &x, const U& y) { return volumeutil::cast_operator_result<T,U>(x / y); } );
  }

  template <typename T, typename U>
  inline
  carto::Volume<T> & operator%= ( carto::Volume<T> & vol, const carto::Volume<U> & other )
  {
    return carto::volumeutil::selfApply( vol, other, [](const T &x, const U& y) { return volumeutil::cast_operator_result<T,U>(x % y); } );
  }

  template <typename T, typename U>
  inline
  carto::Volume<T> & operator&= ( carto::Volume<T> & vol, const carto::Volume<U> & other )
  {
    return carto::volumeutil::selfApply( vol, other, [](const T &x, const U& y) { return volumeutil::cast_operator_result<T,U>(x & y); } );
  }

  template <typename T, typename U>
  inline
  carto::Volume<T> & operator|= ( carto::Volume<T> & vol, const carto::Volume<U> & other )
  {
    return carto::volumeutil::selfApply( vol, other, [](const T &x, const U& y) { return volumeutil::cast_operator_result<T,U>(x | y); } );
  }

  template <typename T, typename U>
  inline
  carto::Volume<T> & operator^= ( carto::Volume<T> & vol, const carto::Volume<U> & other )
  {
    return carto::volumeutil::selfApply( vol, other, [](const T &x, const U& y) { return volumeutil::cast_operator_result<T,U>(x & y); } );
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
    return carto::volumeutil::selfApply( vol, [](T &x) { return ++x; } );
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
    return carto::volumeutil::selfApply( vol, [](T &x) { return --x; });
  }

} // namespace carto

#endif // CARTODATA_VOLUME_VOLUMEBASE_D_OPERATORS_H
