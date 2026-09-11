#ifndef CARTODATA_VOLUME_FUNCTIONAL_H
#define CARTODATA_VOLUME_FUNCTIONAL_H

#include <cartobase/type/types.h>
#include <cartobase/type/datatypetraits.h>
#include <exception>
#include <functional>
#include <limits>

 // Reimplements std functional with more flexibility on left and right
 // operands types.

namespace carto {
namespace volumeutil {


  //==========================================================================
  //   Arithmetic return type detectors
  //==========================================================================

  // With builtin types, arithmetic operations are performed in the most
  // precise type possible with a combination of integer promotion,
  // signness and floating point conversion.
  // The following structures automatically detect the preferred output
  // type.
  // However, with volume, we often want to store the result in a datatype
  // similar to the input type. To counter the effects of integer promotion,
  // these structures are then specialized for 8b and 16b types.
  //
  // Implicit conversion mechanisms are nicely explained here:
  // https://www.safaribooksonline.com/library/view/c-in-a/0596006977/ch04.html

  // plus

  template <typename LEFT, typename RIGHT>
  struct plus_result
  {
#if __cplusplus >= 201100
    typedef decltype( std::declval<LEFT>() + std::declval<RIGHT>() )
      result_type;
#else
    typedef typeof( LEFT() + RIGHT() ) result_type;
#endif
  };

  template <typename LEFT, typename RIGHT, template<typename> class CONTAINER >
  struct plus_result<CONTAINER<LEFT>, RIGHT>
  {
    typedef CONTAINER<typename plus_result<LEFT,RIGHT>::result_type>
      result_type;
  };

  template <typename LEFT, typename RIGHT, template<typename> class CONTAINER >
  struct plus_result<LEFT, CONTAINER<RIGHT> >
  {
    typedef CONTAINER<typename plus_result<LEFT,RIGHT>::result_type>
      result_type;
  };

  template <typename LEFT, typename RIGHT,
            template<typename> class CONTAINER1,
            template <typename> class CONTAINER2>
  struct plus_result<CONTAINER1<LEFT>, CONTAINER2<RIGHT> >
  {
    typedef CONTAINER1<typename plus_result<LEFT,RIGHT>::result_type>
      result_type;
  };

  // minus

  template <typename LEFT, typename RIGHT>
  struct minus_result
  {
#if __cplusplus >= 201100
    typedef decltype( std::declval<LEFT>() - std::declval<RIGHT>() )
      result_type;
#else
    typedef typeof( LEFT() - RIGHT() ) result_type;
#endif
  };

  template <typename LEFT, typename RIGHT, template<typename> class CONTAINER >
  struct minus_result<CONTAINER<LEFT>, RIGHT>
  {
    typedef CONTAINER<typename minus_result<LEFT,RIGHT>::result_type>
      result_type;
  };

  template <typename LEFT, typename RIGHT, template<typename> class CONTAINER >
  struct minus_result<LEFT, CONTAINER<RIGHT> >
  {
    typedef CONTAINER<typename minus_result<LEFT,RIGHT>::result_type>
      result_type;
  };

  template <typename LEFT, typename RIGHT,
            template<typename> class CONTAINER1,
            template <typename> class CONTAINER2>
  struct minus_result<CONTAINER1<LEFT>, CONTAINER2<RIGHT> >
  {
    typedef CONTAINER1<typename minus_result<LEFT,RIGHT>::result_type>
      result_type;
  };

  // multiplies

  template <typename LEFT, typename RIGHT>
  struct multiplies_result
  {
#if __cplusplus >= 201100
    typedef decltype( std::declval<LEFT>() * std::declval<RIGHT>() )
      result_type;
#else
    typedef typeof( LEFT() * RIGHT() ) result_type;
#endif
  };

  template <typename LEFT, typename RIGHT, template<typename> class CONTAINER >
  struct multiplies_result<CONTAINER<LEFT>, RIGHT>
  {
    typedef CONTAINER<typename multiplies_result<LEFT,RIGHT>::result_type>
      result_type;
  };

  template <typename LEFT, typename RIGHT, template<typename> class CONTAINER >
  struct multiplies_result<LEFT, CONTAINER<RIGHT> >
  {
    typedef CONTAINER<typename multiplies_result<LEFT,RIGHT>::result_type>
      result_type;
  };

  template <typename LEFT, typename RIGHT, template<typename> class CONTAINER1, template <typename> class CONTAINER2>
  struct multiplies_result<CONTAINER1<LEFT>, CONTAINER2<RIGHT> >
  {
    typedef CONTAINER1<typename multiplies_result<LEFT,RIGHT>::result_type>
      result_type;
  };

  // divides

  template <typename LEFT, typename RIGHT>
  struct divides_result
  {
#if __cplusplus >= 201100
    typedef decltype( std::declval<LEFT>() / std::declval<RIGHT>() )
      result_type;
#else
    typedef typeof( LEFT() / RIGHT() ) result_type;
#endif
  };

  template <typename LEFT, typename RIGHT, template<typename> class CONTAINER >
  struct divides_result<CONTAINER<LEFT>, RIGHT>
  {
    typedef CONTAINER<typename divides_result<LEFT,RIGHT>::result_type>
      result_type;
  };

  template <typename LEFT, typename RIGHT, template<typename> class CONTAINER >
  struct divides_result<LEFT, CONTAINER<RIGHT> >
  {
    typedef CONTAINER<typename divides_result<LEFT,RIGHT>::result_type>
      result_type;
  };

  template <typename LEFT, typename RIGHT,
            template<typename> class CONTAINER1,
            template <typename> class CONTAINER2>
  struct divides_result<CONTAINER1<LEFT>, CONTAINER2<RIGHT> >
  {
    typedef CONTAINER1<typename divides_result<LEFT,RIGHT>::result_type>
      result_type;
  };

  // modulus

  template <typename LEFT, typename RIGHT>
  struct modulus_result
  {
#if __cplusplus >= 201100
    typedef decltype( std::declval<LEFT>() % std::declval<RIGHT>() )
      result_type;
#else
    typedef typeof( LEFT() % RIGHT() ) result_type;
#endif
  };

  template <typename LEFT, typename RIGHT,
            template<typename> class CONTAINER >
  struct modulus_result<CONTAINER<LEFT>, RIGHT>
  {
    typedef CONTAINER<typename modulus_result<LEFT,RIGHT>::result_type>
      result_type;
  };

  template <typename LEFT, typename RIGHT,
            template<typename> class CONTAINER >
  struct modulus_result<LEFT, CONTAINER<RIGHT> >
  {
    typedef CONTAINER<typename modulus_result<LEFT,RIGHT>::result_type>
      result_type;
  };

  template <typename LEFT, typename RIGHT,
            template<typename> class CONTAINER1,
            template <typename> class CONTAINER2>
  struct modulus_result<CONTAINER1<LEFT>, CONTAINER2<RIGHT> >
  {
    typedef CONTAINER1<typename modulus_result<LEFT,RIGHT>::result_type>
      result_type;
  };

  // bitwise_and

  template <typename LEFT, typename RIGHT>
  struct bitwise_and_result
  {
#if __cplusplus >= 201100
    typedef decltype( std::declval<LEFT>() & std::declval<RIGHT>() )
      result_type;
#else
    typedef typeof( LEFT() & RIGHT() ) result_type;
#endif
  };

  template <typename LEFT, typename RIGHT, template<typename> class CONTAINER >
  struct bitwise_and_result<CONTAINER<LEFT>, RIGHT>
  {
    typedef CONTAINER<typename bitwise_and_result<LEFT,RIGHT>::result_type>
      result_type;
  };

  template <typename LEFT, typename RIGHT, template<typename> class CONTAINER >
  struct bitwise_and_result<LEFT, CONTAINER<RIGHT> >
  {
    typedef CONTAINER<typename bitwise_and_result<LEFT,RIGHT>::result_type>
      result_type;
  };

  template <typename LEFT, typename RIGHT,
            template<typename> class CONTAINER1,
            template <typename> class CONTAINER2>
  struct bitwise_and_result<CONTAINER1<LEFT>, CONTAINER2<RIGHT> >
  {
    typedef CONTAINER1<typename bitwise_and_result<LEFT,RIGHT>::result_type>
      result_type;
  };

  // bitwise_or

  template <typename LEFT, typename RIGHT>
  struct bitwise_or_result
  {
#if __cplusplus >= 201100
    typedef decltype( std::declval<LEFT>() | std::declval<RIGHT>() )
      result_type;
#else
    typedef typeof( LEFT() | RIGHT() ) result_type;
#endif
  };

  template <typename LEFT, typename RIGHT, template<typename> class CONTAINER >
  struct bitwise_or_result<CONTAINER<LEFT>, RIGHT>
  {
    typedef CONTAINER<typename bitwise_or_result<LEFT,RIGHT>::result_type>
      result_type;
  };

  template <typename LEFT, typename RIGHT, template<typename> class CONTAINER >
  struct bitwise_or_result<LEFT, CONTAINER<RIGHT> >
  {
    typedef CONTAINER<typename bitwise_or_result<LEFT,RIGHT>::result_type>
      result_type;
  };

  template <typename LEFT, typename RIGHT,
            template<typename> class CONTAINER1,
            template <typename> class CONTAINER2>
  struct bitwise_or_result<CONTAINER1<LEFT>, CONTAINER2<RIGHT> >
  {
    typedef CONTAINER1<typename bitwise_or_result<LEFT,RIGHT>::result_type>
      result_type;
  };

  // bitwise_xor

  template <typename LEFT, typename RIGHT>
  struct bitwise_xor_result
  {
#if __cplusplus >= 201100
    typedef decltype( std::declval<LEFT>() ^ std::declval<RIGHT>() )
      result_type;
#else
    typedef typeof( LEFT() ^ RIGHT() ) result_type;
#endif
  };

  template <typename LEFT, typename RIGHT, template<typename> class CONTAINER >
  struct bitwise_xor_result<CONTAINER<LEFT>, RIGHT>
  {
    typedef CONTAINER<typename bitwise_xor_result<LEFT,RIGHT>::result_type>
      result_type;
  };

  template <typename LEFT, typename RIGHT, template<typename> class CONTAINER >
  struct bitwise_xor_result<LEFT, CONTAINER<RIGHT> >
  {
    typedef CONTAINER<typename bitwise_xor_result<LEFT,RIGHT>::result_type>
      result_type;
  };

  template <typename LEFT, typename RIGHT,
            template<typename> class CONTAINER1,
            template <typename> class CONTAINER2>
  struct bitwise_xor_result<CONTAINER1<LEFT>, CONTAINER2<RIGHT> >
  {
    typedef CONTAINER1<typename bitwise_xor_result<LEFT,RIGHT>::result_type>
      result_type;
  };

  // select

  template <typename LEFT, typename RIGHT>
  struct select_result
  {
#if __cplusplus >= 201100
    typedef decltype( true ? std::declval<LEFT>() : std::declval<RIGHT>() )
      ref_result_type;
    typedef typename std::remove_reference<ref_result_type>::type result_type;
#else
    typedef typeof( true ? LEFT() : RIGHT() ) result_type;
#endif
  };

  template <typename LEFT, typename RIGHT, template<typename> class CONTAINER >
  struct select_result<CONTAINER<LEFT>, RIGHT>
  {
    typedef CONTAINER<typename select_result<LEFT,RIGHT>::result_type>
      result_type;
  };

  template <typename LEFT, typename RIGHT, template<typename> class CONTAINER >
  struct select_result<LEFT, CONTAINER<RIGHT> >
  {
    typedef CONTAINER<typename select_result<LEFT,RIGHT>::result_type>
      result_type;
  };

  template <typename LEFT, typename RIGHT,
            template<typename> class CONTAINER1,
            template <typename> class CONTAINER2>
  struct select_result<CONTAINER1<LEFT>, CONTAINER2<RIGHT> >
  {
    typedef CONTAINER1<typename select_result<LEFT,RIGHT>::result_type>
      result_type;
  };

  //==========================================================================
  //   Builtin type specialization to take into account integer promotion
  //==========================================================================

  // helper structure that actual *_result structures will inherit from.

  template <typename LEFT, typename RIGHT, typename OPERATION>
  struct basic_type_result { using type = OPERATION; };

  template <typename OPERATION>
  struct basic_type_result<uint8_t, uint8_t, OPERATION> { using type = uint8_t; };
  template <typename OPERATION>
  struct basic_type_result<uint8_t, int8_t, OPERATION> { using type = uint8_t; };
  template <typename OPERATION>
  struct basic_type_result<uint8_t, uint16_t, OPERATION> { using type = uint16_t; };
  template <typename OPERATION>
  struct basic_type_result<uint8_t, int16_t, OPERATION> { using type = int16_t; };

  template <typename OPERATION>
  struct basic_type_result<int8_t, uint8_t, OPERATION> { using type = uint8_t; };
  template <typename OPERATION>
  struct basic_type_result<int8_t, int8_t, OPERATION> { using type = int8_t; };
  template <typename OPERATION>
  struct basic_type_result<int8_t, uint16_t, OPERATION> { using type = uint16_t; };
  template <typename OPERATION>
  struct basic_type_result<int8_t, int16_t, OPERATION> { using type = int16_t; };

  template <typename OPERATION>
  struct basic_type_result<uint16_t, uint8_t, OPERATION> { using type = uint16_t; };
  template <typename OPERATION>
  struct basic_type_result<uint16_t, int8_t, OPERATION> { using type = uint16_t; };
  template <typename OPERATION>
  struct basic_type_result<uint16_t, uint16_t, OPERATION> { using type = uint16_t; };
  template <typename OPERATION>
  struct basic_type_result<uint16_t, int16_t, OPERATION> { using type = uint16_t; };

  template <typename OPERATION>
  struct basic_type_result<int16_t, uint8_t, OPERATION> { using type = int16_t; };
  template <typename OPERATION>
  struct basic_type_result<int16_t, int8_t, OPERATION> { using type = int16_t; };
  template <typename OPERATION>
  struct basic_type_result<int16_t, uint16_t, OPERATION> { using type = uint16_t; };
  template <typename OPERATION>
  struct basic_type_result<int16_t, int16_t, OPERATION> { using type = int16_t; };


  template <typename LEFT, typename RIGHT, typename OPERATION>
  auto cast_operator_result(OPERATION&& x)
      -> typename basic_type_result<LEFT, RIGHT, OPERATION>::type
  {
      return static_cast<typename basic_type_result<LEFT, RIGHT, OPERATION>::type>(std::forward<OPERATION>(x));
  }


  //==========================================================================
  //   Generic operators
  //==========================================================================

  // Function objects similar to those of the standard library, with the
  // difference that left and right operand types can differ.
  // These function objects are then used to perform basic operations
  // between containers (volumes).

  // template <typename LEFT, typename RIGHT = LEFT>
  // struct select_left
  // {
  //   const LEFT & operator() ( const LEFT & x, const RIGHT & ) const
  //   {
  //     return x;
  //   }
  // };

  // template <typename T>
  // struct identity
  // {
  //   const T & operator() ( const T & x ) const
  //   {
  //     return x;
  //   }
  // };

  // template <typename LEFT, typename RIGHT = LEFT>
  // struct plus
  // {
  //   typename plus_result<LEFT,RIGHT>::result_type
  //   operator() (const LEFT & x, const RIGHT & y) const
  //   {
  //     return x + y;
  //   }
  // };

  // template <typename LEFT, typename RIGHT = LEFT>
  // struct minus
  // {
  //   typename minus_result<LEFT,RIGHT>::result_type
  //   operator() (const LEFT & x, const RIGHT & y) const
  //   {
  //     return x - y;
  //   }
  // };

  // template <typename LEFT, typename RIGHT = LEFT>
  // struct multiplies
  // {
  //   typename multiplies_result<LEFT,RIGHT>::result_type
  //   operator() (const LEFT & x, const RIGHT & y) const
  //   {
  //     return x * y;
  //   }
  // };

  // template <typename LEFT, typename RIGHT = LEFT>
  // struct divides
  // {
  //   typename divides_result<LEFT,RIGHT>::result_type
  //   operator() (const LEFT & x, const RIGHT & y) const
  //   {
  //     return x / y;
  //   }
  // };

  // template <typename LEFT, typename RIGHT = LEFT>
  // struct modulus
  // {
  //   typename modulus_result<LEFT,RIGHT>::result_type
  //   operator() (const LEFT & x, const RIGHT & y) const
  //   {
  //     return x % y;
  //   }
  // };

  // template <typename T>
  // struct negate
  // {
  //   T operator() (const T & x) const
  //   {
  //     return -x;
  //   }
  // };

  // template <typename T>
  // struct increment
  // {
  //   T operator() (T x) const
  //   {
  //     return ++x;
  //   }
  // };

  // template <typename T>
  // struct decrement
  // {
  //   T operator() (T x) const
  //   {
  //     return --x;
  //   }
  // };

  // template <typename LEFT, typename RIGHT = LEFT>
  // struct equal_to
  // {
  //   bool operator() (const LEFT & x, const RIGHT & y) const
  //   {
  //     return x == y;
  //   }
  // };

  // template <typename LEFT, typename RIGHT = LEFT>
  // struct not_equal_to
  // {
  //   bool operator() (const LEFT & x, const RIGHT & y) const
  //   {
  //     return x != y;
  //   }
  // };

  // template <typename LEFT, typename RIGHT = LEFT>
  // struct greater
  // {
  //   bool operator() (const LEFT & x, const RIGHT & y) const
  //   {
  //     return x > y;
  //   }
  // };

  // template <typename LEFT, typename RIGHT = LEFT>
  // struct less
  // {
  //   bool operator() (const LEFT & x, const RIGHT & y) const
  //   {
  //     return x < y;
  //   }
  // };

  // template <typename LEFT, typename RIGHT = LEFT>
  // struct greater_equal
  // {
  //   bool operator() (const LEFT & x, const RIGHT & y) const
  //   {
  //     return x >= y;
  //   }
  // };

  // template <typename LEFT, typename RIGHT = LEFT>
  // struct less_equal
  // {
  //   bool operator() (const LEFT & x, const RIGHT & y) const
  //   {
  //     return x <= y;
  //   }
  // };

  // template <typename LEFT, typename RIGHT = LEFT>
  // struct logical_and
  // {
  //   bool operator() (const LEFT & x, const RIGHT & y) const
  //   {
  //     return x && y;
  //   }
  // };

  // template <typename LEFT, typename RIGHT = LEFT>
  // struct logical_or
  // {
  //   bool operator() (const LEFT & x, const RIGHT & y) const
  //   {
  //     return x || y;
  //   }
  // };

  // template <typename T>
  // struct logical_not
  // {
  //   bool operator() (const T & x) const
  //   {
  //     return !x;
  //   }
  // };

  // template <typename LEFT, typename RIGHT = LEFT>
  // struct bitwise_and
  // {
  //   typename bitwise_and_result<LEFT,RIGHT>::result_type
  //   operator() (const LEFT & x, const RIGHT & y) const
  //   {
  //     return x & y;
  //   }
  // };

  // template <typename LEFT, typename RIGHT = LEFT>
  // struct bitwise_or
  // {
  //   typename bitwise_or_result<LEFT,RIGHT>::result_type
  //   operator() (const LEFT & x, const RIGHT & y) const
  //   {
  //     return x | y;
  //   }
  // };

  // template <typename LEFT, typename RIGHT = LEFT>
  // struct bitwise_xor
  // {
  //   typename bitwise_xor_result<LEFT,RIGHT>::result_type
  //   operator() (const LEFT & x, const RIGHT & y) const
  //   {
  //     return x ^ y;
  //   }
  // };

  // template <typename T>
  // struct bitwise_not
  // {
  //   T operator() (const T & x) const
  //   {
  //     return ~x;
  //   }
  // };

  // template <typename LEFT, typename RIGHT = LEFT>
  // struct bitwise_left_shift
  // {
  //   LEFT operator() (const LEFT & x, const RIGHT & y) const
  //   {
  //     return x << y;
  //   }
  // };

  // template <typename LEFT, typename RIGHT = LEFT>
  // struct bitwise_right_shift
  // {
  //   LEFT operator() (const LEFT & x, const RIGHT & y) const
  //   {
  //     return x >> y;
  //   }
  // };

  // template <typename LEFT, typename RIGHT = LEFT>
  // struct select_min
  // {
  //   typename select_result<LEFT,RIGHT>::result_type
  //   operator() (const LEFT & x, const RIGHT & y) const
  //   {
  //     return ( x <= y ? x : y );
  //   }
  // };

  // template <typename LEFT, typename RIGHT = LEFT>
  // struct select_max
  // {
  //   typename select_result<LEFT,RIGHT>::result_type
  //   operator() (const LEFT & x, const RIGHT & y) const
  //   {
  //     return ( x >= y ? x : y );
  //   }
  // };

  // //==========================================================================
  // //   Specialization: cfloat/cdouble
  // //==========================================================================

  // // By default, cfloat values can only be multiplied/divided by floats
  // // and cdouble by doubles.

  // template <>
  // struct multiplies<cfloat, double>
  // {
  //   cfloat operator() (const cfloat & x, const double & y) const
  //   {
  //     return x * (float)y;
  //   }
  // };

  // template <>
  // struct multiplies<cfloat, long>
  // {
  //   cfloat operator() (const cfloat & x, const long & y) const
  //   {
  //     return x * (float)y;
  //   }
  // };

  // template <>
  // struct multiplies<cdouble, float>
  // {
  //   cdouble operator() (const cdouble & x, const float & y) const
  //   {
  //     return x * (double)y;
  //   }
  // };

  // template <>
  // struct multiplies<cdouble, long>
  // {
  //   cdouble operator() (const cdouble & x, const long & y) const
  //   {
  //     return x * (double)y;
  //   }
  // };

  // template <>
  // struct divides<cfloat, double>
  // {
  //   cfloat operator() (const cfloat & x, const double & y) const
  //   {
  //     return x * (float)( 1. / y );
  //   }
  // };

  // template <>
  // struct divides<cfloat, long>
  // {
  //   cfloat operator() (const cfloat & x, const long & y) const
  //   {
  //     return x * (float)( 1. / (double)y );
  //   }
  // };

  // template <>
  // struct divides<cdouble, float>
  // {
  //   cdouble operator() (const cdouble & x, const float & y) const
  //   {
  //     return x * (double)( 1. / y );
  //   }
  // };

  // template <>
  // struct divides<cdouble, double>
  // {
  //   cdouble operator() (const cdouble & x, const double & y) const
  //   {
  //     return x * ( 1. / y );
  //   }
  // };

  // template <>
  // struct divides<cdouble, long>
  // {
  //   cdouble operator() (const cdouble & x, const long & y) const
  //   {
  //     return x * (double)( 1. / (double)y );
  //   }
  // };

  // template <>
  // struct logical_and<bool, cfloat>
  // {
  //   bool operator() (const bool & x, const cfloat & y) const
  //   {
  //     return x && ( y.imag() || y.real() );
  //   }
  // };

  // template <>
  // struct logical_and<bool, cdouble>
  // {
  //   bool operator() (const bool & x, const cdouble & y) const
  //   {
  //     return x && ( y.imag() || y.real() );
  //   }
  // };

  // template <>
  // struct logical_or<bool, cfloat>
  // {
  //   bool operator() (const bool & x, const cfloat & y) const
  //   {
  //     return x || y.imag() || y.real();
  //   }
  // };

  // template <>
  // struct logical_or<bool, cdouble>
  // {
  //   bool operator() (const bool & x, const cdouble & y) const
  //   {
  //     return x || y.imag() || y.real();
  //   }
  // };

  // template <>
  // struct logical_not<cfloat>
  // {
  //   bool operator() (const cfloat & x) const
  //   {
  //     return !( x.imag() || x.real() );
  //   }
  // };

  // template <>
  // struct logical_not<cdouble>
  // {
  //   bool operator() (const cdouble & x) const
  //   {
  //     return !( x.imag() || x.real() );
  //   }
  // };

  // //==========================================================================
  // //   Specialization: bool
  // //==========================================================================

  // template <>
  // struct increment<bool>
  // {
  //   bool operator() ( bool )
  //   {
  //     return true;
  //   }
  // };

  // template <>
  // struct decrement<bool>
  // {
  //   bool operator() ( bool )
  //   {
  //     return false;
  //   }
  // };

} // namespace volumeutil
} // namespace carto

  //==========================================================================
  //   Specialization: VoxelRGB/VoxelRGBA/VoxelHSV
  //==========================================================================

  // operator- (VoxelRGB/RGBA/HSV) is called in pyaims (even though the
  // channel type is unsigned).
  // Thus, it needs to be defined to avoid a compilation error.

#include <cartobase/type/voxelrgb.h>
#include <cartobase/type/voxelrgba.h>
#include <cartobase/type/voxelhsv.h>

namespace carto {
namespace volumeutil {

  // template <>
  // struct negate<VoxelRGB>
  // {
  //   VoxelRGB operator() ( const VoxelRGB & x )
  //   {
  //     return x * -1.f;
  //   }
  // };

  // template <>
  // struct negate<VoxelRGBA>
  // {
  //   VoxelRGBA operator() ( const VoxelRGBA & x )
  //   {
  //     return x * -1.f;
  //   }
  // };

  // template <>
  // struct negate<VoxelHSV>
  // {
  //   VoxelHSV operator() ( const VoxelHSV & x )
  //   {
  //     return x * -1.f;
  //   }
  // };

} // namespace volumeutil
} // namespace carto

#endif // CARTODATA_VOLUME_FUNCTIONAL_H
