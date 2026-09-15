#ifndef PYAIMS_DATA_VOLUMEOPERATORS_H
#define PYAIMS_DATA_VOLUMEOPERATORS_H

#include <stdexcept>
#include <cartodata/volume/volume.h>

namespace
{

  // // Helper template to deduce result type and apply operation
  // template <typename Op, typename L, typename R>
  // auto apply_operation(L& l, R& r, Op op)
  //   -> decltype(op(l, r))
  // {
  //   return op(l, r);
  // }

  // Specialization for Volume types to handle VolumeRef
  template <typename Op, typename L, typename R>
  inline auto apply_volume_operation(L& l, R& r, Op op)
    -> decltype(op(l, r)) *
  {
    carto::VolumeRef<typename std::decay<decltype(l)>::type::datatype> rl(&l);
    carto::VolumeRef<typename std::decay<decltype(r)>::type::datatype> rr(&r);
    auto rres = apply(rl, rr);
    auto res = rres.get();
    rres.release();
    return res;
  }


  template <typename T, typename U>
  inline
  auto __add__( 
    carto::Volume<T> & l, const U & r )
  {
    return apply_volume_operation(l, r, [](auto &x, auto &y) {return x + y;});
  }

  template <typename T, typename U>
  inline
  carto::Volume<T> &__iadd__( 
    carto::Volume<T> & l, const U & r )
  {
    l += r;
    return l;
  }


  template <typename T, typename U>
  inline
  auto __sub__( 
    carto::Volume<T> & l, const U & r )
    -> carto::Volume<decltype(std::declval<T>() - std::declval<U>())> *
  {
    return apply_volume_operation(l, r, [](auto &x, auto &y) {return x - y;});
  }

  template <typename T, typename U>
  inline
  auto __sub__( 
    const T & l,
    carto::Volume<U> & r )
    -> carto::Volume<decltype(std::declval<T>() - std::declval<U>())> *
  {
    return apply_volume_operation(l, r, [](auto &x, auto &y) {return x - y;});
  }

  template <typename T, typename U>
  inline
  auto __sub__( 
    carto::Volume<T> & l,
    carto::Volume<U> & r )
  {
    return apply_volume_operation(l, r, [](auto &x, auto &y) {return x - y;});
  }

  template <typename T, typename U>
  inline
  carto::Volume<T> &__isub__( 
    carto::Volume<T> & l, const U & r )
  {
    l -= r;
    return l;
  }


  template <typename T, typename U>
  inline
  auto __mul__( 
    carto::Volume<T> & l, const U & r )
  {
    return apply_volume_operation(l, r, [](auto &x, auto &y) {return x * y;});
  }

  template <typename T, typename U>
  inline
  carto::Volume<T> &__imul__( 
    carto::Volume<T> & l, const U & r )
  {
    l *= r;
    return l;
  }


  template <typename T, typename U>
  inline
  auto __div__( 
    carto::Volume<T> & l, const U & r )
    -> carto::Volume<decltype(std::declval<T>() / std::declval<U>())> *
  {
    return apply_volume_operation(l, r, [](auto &x, auto &y) {return x / y;});
  }

  template <typename T, typename U>
  inline
  auto __div__( 
    const T & l,
    carto::Volume<U> & r )
    -> carto::Volume<decltype(std::declval<T>() / std::declval<U>())> *
  {
    return apply_volume_operation(l, r, [](auto &x, auto &y) {return x / y;});
  }

  template <typename T, typename U>
  inline
  auto __div__( 
    carto::Volume<T> & l,
    carto::Volume<U> & r )
  {
    return apply_volume_operation(l, r, [](auto &x, auto &y) {return x / y;});
  }


  template <typename T, typename U>
  inline
  carto::Volume<T> &__idiv__( 
    carto::Volume<T> & l, const U & r )
  {
    l &= r;
    return l;
  }

  template <typename T, typename U>
  inline
  auto __mod__( 
    carto::Volume<T> & l, const U & r )
  {
    return apply_volume_operation(l, r, [](auto &x, auto &y) {return x % y;});
  }

  template <typename T, typename U>
  inline
  carto::Volume<T> &__imod__( 
    carto::Volume<T> & l, const U & r )
  {
    l %= r;
    return l;
  }

  template <typename T, typename U>
  inline
  auto __and__( 
    carto::Volume<T> & l, const U & r )
  {
    return apply_volume_operation(l, r, [](auto &x, auto &y) {return x & y;});
  }

  template <typename T, typename U>
  inline
  carto::Volume<T> &__iand__( 
    carto::Volume<T> & l, const U & r )
  {
    l &= r;
    return l;
  }


  template <typename T, typename U>
  inline
  auto __or__( 
    carto::Volume<T> & l, const U & r )
  {
    return apply_volume_operation(l, r, [](auto &x, auto &y) {return x | y;});
  }

  template <typename T, typename U>
  inline
  carto::Volume<T> &__ior__( 
    carto::Volume<T> & l, const U & r )
  {
    l |= r;
    return l;
  }


  template <typename T, typename U>
  inline
  auto __xor__( 
    carto::Volume<T> & l, const U & r )
  {
    return apply_volume_operation(l, r, [](auto &x, auto &y) {return x ^ y;});
  }

  template <typename T, typename U>
  inline
  carto::Volume<T> &__ixor__( 
    carto::Volume<T> & l, const U & r )
  {
    l  ^= r;
    return l;
  }

}

#endif
