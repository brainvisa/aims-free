#ifndef PYAIMS_DATA_VOLUMEOPERATORS_H
#define PYAIMS_DATA_VOLUMEOPERATORS_H

#include <cartodata/volume/volume.h>

namespace
{

  // Helper template to deduce result type and apply operation
  template <typename Op, typename L, typename R>
  auto apply_operation(L& l, R& r, Op op)
    -> decltype(op(l, r))
  {
    return op(l, r);
  }

  // Specialization for Volume types to handle VolumeRef
  template <typename Op, typename L, typename R>
  auto apply_volume_operation(L& l, R& r, Op op)
    -> decltype(op(l, r))
  {
    carto::VolumeRef<typename std::decay<decltype(l)>::type::datatype> rl(&l);
    carto::VolumeRef<typename std::decay<decltype(r)>::type::datatype> rr(&r);
    auto rres = op(rl, rr);
    auto res = rres.get();
    rres.release();
    return res;
  }

  // __add__ implementation
  template <typename L, typename R>
  inline
  auto __add__( L & l, R & r )
  {
    return apply_operation(l, r, [](auto& left, auto& right) {
      return left + right;
    });
  }

  template <>
  inline
  carto::Volume<float> * __add__<carto::Volume<float>, carto::Volume<float>>( 
    carto::Volume<float> & l, carto::Volume<float> & r )
  {
    auto op = [](auto& left, auto& right) { return left + right; };
    carto::VolumeRef<float> rl(&l);
    carto::VolumeRef<float> rr(&r);
    auto rres = op(rl, rr);
    auto res = rres.get();
    rres.release();
    return res;
  }

  // __iadd__ implementation
  template <typename L, typename R>
  inline
  L & __iadd__( L & l, R & r )
  {
    l += r;
    return l;
  }

  // __sub__ implementation
  template <typename L, typename R>
  inline
  auto __sub__( L & l, R & r )
  {
    return apply_operation(l, r, [](auto& left, auto& right) {
      return left - right;
    });
  }

  template <>
  inline
  carto::Volume<float> * __sub__<carto::Volume<float>, carto::Volume<float>>( 
    carto::Volume<float> & l, carto::Volume<float> & r )
  {
    auto op = [](auto& left, auto& right) { return left - right; };
    carto::VolumeRef<float> rl(&l);
    carto::VolumeRef<float> rr(&r);
    auto rres = op(rl, rr);
    auto res = rres.get();
    rres.release();
    return res;
  }

  // __isub__ implementation
  template <typename L, typename R>
  inline
  L & __isub__( L & l, R & r )
  {
    l -= r;
    return l;
  }

  // __mul__ implementation
  template <typename L, typename R>
  inline
  auto __mul__( L & l, R & r )
  {
    return apply_operation(l, r, [](auto& left, auto& right) {
      return left * right;
    });
  }

  template <>
  inline
  carto::Volume<float> * __mul__<carto::Volume<float>, carto::Volume<float>>( 
    carto::Volume<float> & l, carto::Volume<float> & r )
  {
    auto op = [](auto& left, auto& right) { return left * right; };
    carto::VolumeRef<float> rl(&l);
    carto::VolumeRef<float> rr(&r);
    auto rres = op(rl, rr);
    auto res = rres.get();
    rres.release();
    return res;
  }

  // __imul__ implementation
  template <typename L, typename R>
  inline
  L & __imul__( L & l, R & r )
  {
    l *= r;
    return l;
  }

  // __div__ implementation
  template <typename L, typename R>
  inline
  auto __div__( L & l, R & r )
  {
    return apply_operation(l, r, [](auto& left, auto& right) {
      return left / right;
    });
  }

  template <>
  inline
  carto::Volume<float> * __div__<carto::Volume<float>, carto::Volume<float>>( 
    carto::Volume<float> & l, carto::Volume<float> & r )
  {
    auto op = [](auto& left, auto& right) { return left / right; };
    carto::VolumeRef<float> rl(&l);
    carto::VolumeRef<float> rr(&r);
    auto rres = op(rl, rr);
    auto res = rres.get();
    rres.release();
    return res;
  }

  // __idiv__ implementation
  template <typename L, typename R>
  inline
  L & __idiv__( L & l, R & r )
  {
    l /= r;
    return l;
  }

  // __mod__ implementation
  template <typename L, typename R>
  inline
  auto __mod__( L & l, R & r )
  {
    return apply_operation(l, r, [](auto& left, auto& right) {
      return left % right;
    });
  }

  template <>
  inline
  carto::Volume<float> * __mod__<carto::Volume<float>, carto::Volume<float>>( 
    carto::Volume<float> & l, carto::Volume<float> & r )
  {
    auto op = [](auto& left, auto& right) { return left % right; };
    carto::VolumeRef<float> rl(&l);
    carto::VolumeRef<float> rr(&r);
    auto rres = op(rl, rr);
    auto res = rres.get();
    rres.release();
    return res;
  }

  // __imod__ implementation
  template <typename L, typename R>
  inline
  L & __imod__( L & l, R & r )
  {
    l %= r;
    return l;
  }

  // __and__ implementation
  template <typename L, typename R>
  inline
  auto __and__( L & l, R & r )
  {
    return apply_operation(l, r, [](auto& left, auto& right) {
      return left & right;
    });
  }

  template <>
  inline
  carto::Volume<int> * __and__<carto::Volume<int>, carto::Volume<int>>( 
    carto::Volume<int> & l, carto::Volume<int> & r )
  {
    auto op = [](auto& left, auto& right) { return left & right; };
    carto::VolumeRef<int> rl(&l);
    carto::VolumeRef<int> rr(&r);
    auto rres = op(rl, rr);
    auto res = rres.get();
    rres.release();
    return res;
  }

  // __iand__ implementation
  template <typename L, typename R>
  inline
  L & __iand__( L & l, R & r )
  {
    l &= r;
    return l;
  }

  // __or__ implementation
  template <typename L, typename R>
  inline
  auto __or__( L & l, R & r )
  {
    return apply_operation(l, r, [](auto& left, auto& right) {
      return left | right;
    });
  }

  template <>
  inline
  carto::Volume<int> * __or__<carto::Volume<int>, carto::Volume<int>>( 
    carto::Volume<int> & l, carto::Volume<int> & r )
  {
    auto op = [](auto& left, auto& right) { return left | right; };
    carto::VolumeRef<int> rl(&l);
    carto::VolumeRef<int> rr(&r);
    auto rres = op(rl, rr);
    auto res = rres.get();
    rres.release();
    return res;
  }

  // __ior__ implementation
  template <typename L, typename R>
  inline
  L & __ior__( L & l, R & r )
  {
    l |= r;
    return l;
  }

  // __xor__ implementation
  template <typename L, typename R>
  inline
  auto __xor__( L & l, R & r )
  {
    return apply_operation(l, r, [](auto& left, auto& right) {
      return left ^ right;
    });
  }

  template <>
  inline
  carto::Volume<int> * __xor__<carto::Volume<int>, carto::Volume<int>>( 
    carto::Volume<int> & l, carto::Volume<int> & r )
  {
    auto op = [](auto& left, auto& right) { return left ^ right; };
    carto::VolumeRef<int> rl(&l);
    carto::VolumeRef<int> rr(&r);
    auto rres = op(rl, rr);
    auto res = rres.get();
    rres.release();
    return res;
  }

  // __ixor__ implementation
  template <typename L, typename R>
  inline
  L & __ixor__( L & l, R & r )
  {
    l ^= r;
    return l;
  }

}

#endif
