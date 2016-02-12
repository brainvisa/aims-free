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
    typedef typeof( LEFT() + RIGHT() ) result_type;
  };

  template <typename LEFT, typename RIGHT, template<typename> class CONTAINER >
  struct plus_result<CONTAINER<LEFT>, RIGHT>
  {
    typedef CONTAINER<typename plus_result<LEFT,RIGHT>::result_type> result_type;
  };

  template <typename LEFT, typename RIGHT, template<typename> class CONTAINER >
  struct plus_result<LEFT, CONTAINER<RIGHT> >
  {
    typedef CONTAINER<typename plus_result<LEFT,RIGHT>::result_type> result_type;
  };

  template <typename LEFT, typename RIGHT, template<typename> class CONTAINER1, template <typename> class CONTAINER2>
  struct plus_result<CONTAINER1<LEFT>, CONTAINER2<RIGHT> >
  {
    typedef CONTAINER1<typename plus_result<LEFT,RIGHT>::result_type> result_type;
  };

  // minus

  template <typename LEFT, typename RIGHT>
  struct minus_result
  {
    typedef typeof( LEFT() - RIGHT() ) result_type;
  };

  template <typename LEFT, typename RIGHT, template<typename> class CONTAINER >
  struct minus_result<CONTAINER<LEFT>, RIGHT>
  {
    typedef CONTAINER<typename minus_result<LEFT,RIGHT>::result_type> result_type;
  };

  template <typename LEFT, typename RIGHT, template<typename> class CONTAINER >
  struct minus_result<LEFT, CONTAINER<RIGHT> >
  {
    typedef CONTAINER<typename minus_result<LEFT,RIGHT>::result_type> result_type;
  };

  template <typename LEFT, typename RIGHT, template<typename> class CONTAINER1, template <typename> class CONTAINER2>
  struct minus_result<CONTAINER1<LEFT>, CONTAINER2<RIGHT> >
  {
    typedef CONTAINER1<typename minus_result<LEFT,RIGHT>::result_type> result_type;
  };

  // multiplies

  template <typename LEFT, typename RIGHT>
  struct multiplies_result
  {
    typedef typeof( LEFT() * RIGHT() ) result_type;
  };

  template <typename LEFT, typename RIGHT, template<typename> class CONTAINER >
  struct multiplies_result<CONTAINER<LEFT>, RIGHT>
  {
    typedef CONTAINER<typename multiplies_result<LEFT,RIGHT>::result_type> result_type;
  };

  template <typename LEFT, typename RIGHT, template<typename> class CONTAINER >
  struct multiplies_result<LEFT, CONTAINER<RIGHT> >
  {
    typedef CONTAINER<typename multiplies_result<LEFT,RIGHT>::result_type> result_type;
  };

  template <typename LEFT, typename RIGHT, template<typename> class CONTAINER1, template <typename> class CONTAINER2>
  struct multiplies_result<CONTAINER1<LEFT>, CONTAINER2<RIGHT> >
  {
    typedef CONTAINER1<typename multiplies_result<LEFT,RIGHT>::result_type> result_type;
  };

  // divides

  template <typename LEFT, typename RIGHT>
  struct divides_result
  {
    typedef typeof( LEFT() / RIGHT() ) result_type;
  };

  template <typename LEFT, typename RIGHT, template<typename> class CONTAINER >
  struct divides_result<CONTAINER<LEFT>, RIGHT>
  {
    typedef CONTAINER<typename divides_result<LEFT,RIGHT>::result_type> result_type;
  };

  template <typename LEFT, typename RIGHT, template<typename> class CONTAINER >
  struct divides_result<LEFT, CONTAINER<RIGHT> >
  {
    typedef CONTAINER<typename divides_result<LEFT,RIGHT>::result_type> result_type;
  };

  template <typename LEFT, typename RIGHT, template<typename> class CONTAINER1, template <typename> class CONTAINER2>
  struct divides_result<CONTAINER1<LEFT>, CONTAINER2<RIGHT> >
  {
    typedef CONTAINER1<typename divides_result<LEFT,RIGHT>::result_type> result_type;
  };

  // modulus

  template <typename LEFT, typename RIGHT>
  struct modulus_result
  {
    typedef typeof( LEFT() % RIGHT() ) result_type;
  };

  template <typename LEFT, typename RIGHT, template<typename> class CONTAINER >
  struct modulus_result<CONTAINER<LEFT>, RIGHT>
  {
    typedef CONTAINER<typename modulus_result<LEFT,RIGHT>::result_type> result_type;
  };

  template <typename LEFT, typename RIGHT, template<typename> class CONTAINER >
  struct modulus_result<LEFT, CONTAINER<RIGHT> >
  {
    typedef CONTAINER<typename modulus_result<LEFT,RIGHT>::result_type> result_type;
  };

  template <typename LEFT, typename RIGHT, template<typename> class CONTAINER1, template <typename> class CONTAINER2>
  struct modulus_result<CONTAINER1<LEFT>, CONTAINER2<RIGHT> >
  {
    typedef CONTAINER1<typename modulus_result<LEFT,RIGHT>::result_type> result_type;
  };

  // bitwise_and

  template <typename LEFT, typename RIGHT>
  struct bitwise_and_result
  {
    typedef typeof( LEFT() & RIGHT() ) result_type;
  };

  template <typename LEFT, typename RIGHT, template<typename> class CONTAINER >
  struct bitwise_and_result<CONTAINER<LEFT>, RIGHT>
  {
    typedef CONTAINER<typename bitwise_and_result<LEFT,RIGHT>::result_type> result_type;
  };

  template <typename LEFT, typename RIGHT, template<typename> class CONTAINER >
  struct bitwise_and_result<LEFT, CONTAINER<RIGHT> >
  {
    typedef CONTAINER<typename bitwise_and_result<LEFT,RIGHT>::result_type> result_type;
  };

  template <typename LEFT, typename RIGHT, template<typename> class CONTAINER1, template <typename> class CONTAINER2>
  struct bitwise_and_result<CONTAINER1<LEFT>, CONTAINER2<RIGHT> >
  {
    typedef CONTAINER1<typename bitwise_and_result<LEFT,RIGHT>::result_type> result_type;
  };

  // bitwise_or

  template <typename LEFT, typename RIGHT>
  struct bitwise_or_result
  {
    typedef typeof( LEFT() | RIGHT() ) result_type;
  };

  template <typename LEFT, typename RIGHT, template<typename> class CONTAINER >
  struct bitwise_or_result<CONTAINER<LEFT>, RIGHT>
  {
    typedef CONTAINER<typename bitwise_or_result<LEFT,RIGHT>::result_type> result_type;
  };

  template <typename LEFT, typename RIGHT, template<typename> class CONTAINER >
  struct bitwise_or_result<LEFT, CONTAINER<RIGHT> >
  {
    typedef CONTAINER<typename bitwise_or_result<LEFT,RIGHT>::result_type> result_type;
  };

  template <typename LEFT, typename RIGHT, template<typename> class CONTAINER1, template <typename> class CONTAINER2>
  struct bitwise_or_result<CONTAINER1<LEFT>, CONTAINER2<RIGHT> >
  {
    typedef CONTAINER1<typename bitwise_or_result<LEFT,RIGHT>::result_type> result_type;
  };

  // bitwise_xor

  template <typename LEFT, typename RIGHT>
  struct bitwise_xor_result
  {
    typedef typeof( LEFT() ^ RIGHT() ) result_type;
  };

  template <typename LEFT, typename RIGHT, template<typename> class CONTAINER >
  struct bitwise_xor_result<CONTAINER<LEFT>, RIGHT>
  {
    typedef CONTAINER<typename bitwise_xor_result<LEFT,RIGHT>::result_type> result_type;
  };

  template <typename LEFT, typename RIGHT, template<typename> class CONTAINER >
  struct bitwise_xor_result<LEFT, CONTAINER<RIGHT> >
  {
    typedef CONTAINER<typename bitwise_xor_result<LEFT,RIGHT>::result_type> result_type;
  };

  template <typename LEFT, typename RIGHT, template<typename> class CONTAINER1, template <typename> class CONTAINER2>
  struct bitwise_xor_result<CONTAINER1<LEFT>, CONTAINER2<RIGHT> >
  {
    typedef CONTAINER1<typename bitwise_xor_result<LEFT,RIGHT>::result_type> result_type;
  };

  // select

  template <typename LEFT, typename RIGHT>
  struct select_result
  {
    typedef typeof( true ? LEFT() : RIGHT() ) result_type;
  };

  template <typename LEFT, typename RIGHT, template<typename> class CONTAINER >
  struct select_result<CONTAINER<LEFT>, RIGHT>
  {
    typedef CONTAINER<typename select_result<LEFT,RIGHT>::result_type> result_type;
  };

  template <typename LEFT, typename RIGHT, template<typename> class CONTAINER >
  struct select_result<LEFT, CONTAINER<RIGHT> >
  {
    typedef CONTAINER<typename select_result<LEFT,RIGHT>::result_type> result_type;
  };

  template <typename LEFT, typename RIGHT, template<typename> class CONTAINER1, template <typename> class CONTAINER2>
  struct select_result<CONTAINER1<LEFT>, CONTAINER2<RIGHT> >
  {
    typedef CONTAINER1<typename select_result<LEFT,RIGHT>::result_type> result_type;
  };

  //==========================================================================
  //   Builtin type specialization to take into account integer promotion
  //==========================================================================

  // helper structure that actual *_result structures will inherit from.

  template <typename LEFT, typename RIGHT>
  struct basic_type_result;

  template <>
  struct basic_type_result<uint8_t, uint8_t> { typedef uint8_t result_type; };
  template <>
  struct basic_type_result<uint8_t, int8_t> { typedef uint8_t result_type; };
  template <>
  struct basic_type_result<uint8_t, uint16_t> { typedef uint16_t result_type; };
  template <>
  struct basic_type_result<uint8_t, int16_t> { typedef int16_t result_type; };

  template <>
  struct basic_type_result<int8_t, uint8_t> { typedef uint8_t result_type; };
  template <>
  struct basic_type_result<int8_t, int8_t> { typedef int8_t result_type; };
  template <>
  struct basic_type_result<int8_t, uint16_t> { typedef uint16_t result_type; };
  template <>
  struct basic_type_result<int8_t, int16_t> { typedef int16_t result_type; };

  template <>
  struct basic_type_result<uint16_t, uint8_t> { typedef uint16_t result_type; };
  template <>
  struct basic_type_result<uint16_t, int8_t> { typedef uint16_t result_type; };
  template <>
  struct basic_type_result<uint16_t, uint16_t> { typedef uint16_t result_type; };
  template <>
  struct basic_type_result<uint16_t, int16_t> { typedef uint16_t result_type; };

  template <>
  struct basic_type_result<int16_t, uint8_t> { typedef int16_t result_type; };
  template <>
  struct basic_type_result<int16_t, int8_t> { typedef int16_t result_type; };
  template <>
  struct basic_type_result<int16_t, uint16_t> { typedef uint16_t result_type; };
  template <>
  struct basic_type_result<int16_t, int16_t> { typedef int16_t result_type; };

  // plus

  template <>
  struct plus_result<uint8_t, uint8_t>: public basic_type_result<uint8_t, uint8_t> {};
  template <>
  struct plus_result<uint8_t, int8_t>: public basic_type_result<uint8_t, int8_t> {};
  template <>
  struct plus_result<uint8_t, uint16_t>: public basic_type_result<uint8_t, uint16_t> {};
  template <>
  struct plus_result<uint8_t, int16_t>: public basic_type_result<uint8_t, int16_t> {};
  template <>
  struct plus_result<int8_t, uint8_t>: public basic_type_result<uint8_t, uint8_t> {};
  template <>
  struct plus_result<int8_t, int8_t>: public basic_type_result<uint8_t, int8_t> {};
  template <>
  struct plus_result<int8_t, uint16_t>: public basic_type_result<uint8_t, uint16_t> {};
  template <>
  struct plus_result<int8_t, int16_t>: public basic_type_result<uint8_t, int16_t> {};
  template <>
  struct plus_result<uint16_t, uint8_t>: public basic_type_result<uint8_t, uint8_t> {};
  template <>
  struct plus_result<uint16_t, int8_t>: public basic_type_result<uint8_t, int8_t> {};
  template <>
  struct plus_result<uint16_t, uint16_t>: public basic_type_result<uint8_t, uint16_t> {};
  template <>
  struct plus_result<uint16_t, int16_t>: public basic_type_result<uint8_t, int16_t> {};
  template <>
  struct plus_result<int16_t, uint8_t>: public basic_type_result<uint8_t, uint8_t> {};
  template <>
  struct plus_result<int16_t, int8_t>: public basic_type_result<uint8_t, int8_t> {};
  template <>
  struct plus_result<int16_t, uint16_t>: public basic_type_result<uint8_t, uint16_t> {};
  template <>
  struct plus_result<int16_t, int16_t>: public basic_type_result<uint8_t, int16_t> {};

  // minus

  template <>
  struct minus_result<uint8_t, uint8_t>: public basic_type_result<uint8_t, uint8_t> {};
  template <>
  struct minus_result<uint8_t, int8_t>: public basic_type_result<uint8_t, int8_t> {};
  template <>
  struct minus_result<uint8_t, uint16_t>: public basic_type_result<uint8_t, uint16_t> {};
  template <>
  struct minus_result<uint8_t, int16_t>: public basic_type_result<uint8_t, int16_t> {};
  template <>
  struct minus_result<int8_t, uint8_t>: public basic_type_result<uint8_t, uint8_t> {};
  template <>
  struct minus_result<int8_t, int8_t>: public basic_type_result<uint8_t, int8_t> {};
  template <>
  struct minus_result<int8_t, uint16_t>: public basic_type_result<uint8_t, uint16_t> {};
  template <>
  struct minus_result<int8_t, int16_t>: public basic_type_result<uint8_t, int16_t> {};
  template <>
  struct minus_result<uint16_t, uint8_t>: public basic_type_result<uint8_t, uint8_t> {};
  template <>
  struct minus_result<uint16_t, int8_t>: public basic_type_result<uint8_t, int8_t> {};
  template <>
  struct minus_result<uint16_t, uint16_t>: public basic_type_result<uint8_t, uint16_t> {};
  template <>
  struct minus_result<uint16_t, int16_t>: public basic_type_result<uint8_t, int16_t> {};
  template <>
  struct minus_result<int16_t, uint8_t>: public basic_type_result<uint8_t, uint8_t> {};
  template <>
  struct minus_result<int16_t, int8_t>: public basic_type_result<uint8_t, int8_t> {};
  template <>
  struct minus_result<int16_t, uint16_t>: public basic_type_result<uint8_t, uint16_t> {};
  template <>
  struct minus_result<int16_t, int16_t>: public basic_type_result<uint8_t, int16_t> {};

  // multiplies

  template <>
  struct multiplies_result<uint8_t, uint8_t>: public basic_type_result<uint8_t, uint8_t> {};
  template <>
  struct multiplies_result<uint8_t, int8_t>: public basic_type_result<uint8_t, int8_t> {};
  template <>
  struct multiplies_result<uint8_t, uint16_t>: public basic_type_result<uint8_t, uint16_t> {};
  template <>
  struct multiplies_result<uint8_t, int16_t>: public basic_type_result<uint8_t, int16_t> {};
  template <>
  struct multiplies_result<int8_t, uint8_t>: public basic_type_result<uint8_t, uint8_t> {};
  template <>
  struct multiplies_result<int8_t, int8_t>: public basic_type_result<uint8_t, int8_t> {};
  template <>
  struct multiplies_result<int8_t, uint16_t>: public basic_type_result<uint8_t, uint16_t> {};
  template <>
  struct multiplies_result<int8_t, int16_t>: public basic_type_result<uint8_t, int16_t> {};
  template <>
  struct multiplies_result<uint16_t, uint8_t>: public basic_type_result<uint8_t, uint8_t> {};
  template <>
  struct multiplies_result<uint16_t, int8_t>: public basic_type_result<uint8_t, int8_t> {};
  template <>
  struct multiplies_result<uint16_t, uint16_t>: public basic_type_result<uint8_t, uint16_t> {};
  template <>
  struct multiplies_result<uint16_t, int16_t>: public basic_type_result<uint8_t, int16_t> {};
  template <>
  struct multiplies_result<int16_t, uint8_t>: public basic_type_result<uint8_t, uint8_t> {};
  template <>
  struct multiplies_result<int16_t, int8_t>: public basic_type_result<uint8_t, int8_t> {};
  template <>
  struct multiplies_result<int16_t, uint16_t>: public basic_type_result<uint8_t, uint16_t> {};
  template <>
  struct multiplies_result<int16_t, int16_t>: public basic_type_result<uint8_t, int16_t> {};

  // divides

  template <>
  struct divides_result<uint8_t, uint8_t>: public basic_type_result<uint8_t, uint8_t> {};
  template <>
  struct divides_result<uint8_t, int8_t>: public basic_type_result<uint8_t, int8_t> {};
  template <>
  struct divides_result<uint8_t, uint16_t>: public basic_type_result<uint8_t, uint16_t> {};
  template <>
  struct divides_result<uint8_t, int16_t>: public basic_type_result<uint8_t, int16_t> {};
  template <>
  struct divides_result<int8_t, uint8_t>: public basic_type_result<uint8_t, uint8_t> {};
  template <>
  struct divides_result<int8_t, int8_t>: public basic_type_result<uint8_t, int8_t> {};
  template <>
  struct divides_result<int8_t, uint16_t>: public basic_type_result<uint8_t, uint16_t> {};
  template <>
  struct divides_result<int8_t, int16_t>: public basic_type_result<uint8_t, int16_t> {};
  template <>
  struct divides_result<uint16_t, uint8_t>: public basic_type_result<uint8_t, uint8_t> {};
  template <>
  struct divides_result<uint16_t, int8_t>: public basic_type_result<uint8_t, int8_t> {};
  template <>
  struct divides_result<uint16_t, uint16_t>: public basic_type_result<uint8_t, uint16_t> {};
  template <>
  struct divides_result<uint16_t, int16_t>: public basic_type_result<uint8_t, int16_t> {};
  template <>
  struct divides_result<int16_t, uint8_t>: public basic_type_result<uint8_t, uint8_t> {};
  template <>
  struct divides_result<int16_t, int8_t>: public basic_type_result<uint8_t, int8_t> {};
  template <>
  struct divides_result<int16_t, uint16_t>: public basic_type_result<uint8_t, uint16_t> {};
  template <>
  struct divides_result<int16_t, int16_t>: public basic_type_result<uint8_t, int16_t> {};

  // modulus

  template <>
  struct modulus_result<uint8_t, uint8_t>: public basic_type_result<uint8_t, uint8_t> {};
  template <>
  struct modulus_result<uint8_t, int8_t>: public basic_type_result<uint8_t, int8_t> {};
  template <>
  struct modulus_result<uint8_t, uint16_t>: public basic_type_result<uint8_t, uint16_t> {};
  template <>
  struct modulus_result<uint8_t, int16_t>: public basic_type_result<uint8_t, int16_t> {};
  template <>
  struct modulus_result<int8_t, uint8_t>: public basic_type_result<uint8_t, uint8_t> {};
  template <>
  struct modulus_result<int8_t, int8_t>: public basic_type_result<uint8_t, int8_t> {};
  template <>
  struct modulus_result<int8_t, uint16_t>: public basic_type_result<uint8_t, uint16_t> {};
  template <>
  struct modulus_result<int8_t, int16_t>: public basic_type_result<uint8_t, int16_t> {};
  template <>
  struct modulus_result<uint16_t, uint8_t>: public basic_type_result<uint8_t, uint8_t> {};
  template <>
  struct modulus_result<uint16_t, int8_t>: public basic_type_result<uint8_t, int8_t> {};
  template <>
  struct modulus_result<uint16_t, uint16_t>: public basic_type_result<uint8_t, uint16_t> {};
  template <>
  struct modulus_result<uint16_t, int16_t>: public basic_type_result<uint8_t, int16_t> {};
  template <>
  struct modulus_result<int16_t, uint8_t>: public basic_type_result<uint8_t, uint8_t> {};
  template <>
  struct modulus_result<int16_t, int8_t>: public basic_type_result<uint8_t, int8_t> {};
  template <>
  struct modulus_result<int16_t, uint16_t>: public basic_type_result<uint8_t, uint16_t> {};
  template <>
  struct modulus_result<int16_t, int16_t>: public basic_type_result<uint8_t, int16_t> {};

  // bitwise_and

  template <>
  struct bitwise_and_result<uint8_t, uint8_t>: public basic_type_result<uint8_t, uint8_t> {};
  template <>
  struct bitwise_and_result<uint8_t, int8_t>: public basic_type_result<uint8_t, int8_t> {};
  template <>
  struct bitwise_and_result<uint8_t, uint16_t>: public basic_type_result<uint8_t, uint16_t> {};
  template <>
  struct bitwise_and_result<uint8_t, int16_t>: public basic_type_result<uint8_t, int16_t> {};
  template <>
  struct bitwise_and_result<int8_t, uint8_t>: public basic_type_result<uint8_t, uint8_t> {};
  template <>
  struct bitwise_and_result<int8_t, int8_t>: public basic_type_result<uint8_t, int8_t> {};
  template <>
  struct bitwise_and_result<int8_t, uint16_t>: public basic_type_result<uint8_t, uint16_t> {};
  template <>
  struct bitwise_and_result<int8_t, int16_t>: public basic_type_result<uint8_t, int16_t> {};
  template <>
  struct bitwise_and_result<uint16_t, uint8_t>: public basic_type_result<uint8_t, uint8_t> {};
  template <>
  struct bitwise_and_result<uint16_t, int8_t>: public basic_type_result<uint8_t, int8_t> {};
  template <>
  struct bitwise_and_result<uint16_t, uint16_t>: public basic_type_result<uint8_t, uint16_t> {};
  template <>
  struct bitwise_and_result<uint16_t, int16_t>: public basic_type_result<uint8_t, int16_t> {};
  template <>
  struct bitwise_and_result<int16_t, uint8_t>: public basic_type_result<uint8_t, uint8_t> {};
  template <>
  struct bitwise_and_result<int16_t, int8_t>: public basic_type_result<uint8_t, int8_t> {};
  template <>
  struct bitwise_and_result<int16_t, uint16_t>: public basic_type_result<uint8_t, uint16_t> {};
  template <>
  struct bitwise_and_result<int16_t, int16_t>: public basic_type_result<uint8_t, int16_t> {};

  // bitwise_or

  template <>
  struct bitwise_or_result<uint8_t, uint8_t>: public basic_type_result<uint8_t, uint8_t> {};
  template <>
  struct bitwise_or_result<uint8_t, int8_t>: public basic_type_result<uint8_t, int8_t> {};
  template <>
  struct bitwise_or_result<uint8_t, uint16_t>: public basic_type_result<uint8_t, uint16_t> {};
  template <>
  struct bitwise_or_result<uint8_t, int16_t>: public basic_type_result<uint8_t, int16_t> {};
  template <>
  struct bitwise_or_result<int8_t, uint8_t>: public basic_type_result<uint8_t, uint8_t> {};
  template <>
  struct bitwise_or_result<int8_t, int8_t>: public basic_type_result<uint8_t, int8_t> {};
  template <>
  struct bitwise_or_result<int8_t, uint16_t>: public basic_type_result<uint8_t, uint16_t> {};
  template <>
  struct bitwise_or_result<int8_t, int16_t>: public basic_type_result<uint8_t, int16_t> {};
  template <>
  struct bitwise_or_result<uint16_t, uint8_t>: public basic_type_result<uint8_t, uint8_t> {};
  template <>
  struct bitwise_or_result<uint16_t, int8_t>: public basic_type_result<uint8_t, int8_t> {};
  template <>
  struct bitwise_or_result<uint16_t, uint16_t>: public basic_type_result<uint8_t, uint16_t> {};
  template <>
  struct bitwise_or_result<uint16_t, int16_t>: public basic_type_result<uint8_t, int16_t> {};
  template <>
  struct bitwise_or_result<int16_t, uint8_t>: public basic_type_result<uint8_t, uint8_t> {};
  template <>
  struct bitwise_or_result<int16_t, int8_t>: public basic_type_result<uint8_t, int8_t> {};
  template <>
  struct bitwise_or_result<int16_t, uint16_t>: public basic_type_result<uint8_t, uint16_t> {};
  template <>
  struct bitwise_or_result<int16_t, int16_t>: public basic_type_result<uint8_t, int16_t> {};

  // bitwise_xor

  template <>
  struct bitwise_xor_result<uint8_t, uint8_t>: public basic_type_result<uint8_t, uint8_t> {};
  template <>
  struct bitwise_xor_result<uint8_t, int8_t>: public basic_type_result<uint8_t, int8_t> {};
  template <>
  struct bitwise_xor_result<uint8_t, uint16_t>: public basic_type_result<uint8_t, uint16_t> {};
  template <>
  struct bitwise_xor_result<uint8_t, int16_t>: public basic_type_result<uint8_t, int16_t> {};
  template <>
  struct bitwise_xor_result<int8_t, uint8_t>: public basic_type_result<uint8_t, uint8_t> {};
  template <>
  struct bitwise_xor_result<int8_t, int8_t>: public basic_type_result<uint8_t, int8_t> {};
  template <>
  struct bitwise_xor_result<int8_t, uint16_t>: public basic_type_result<uint8_t, uint16_t> {};
  template <>
  struct bitwise_xor_result<int8_t, int16_t>: public basic_type_result<uint8_t, int16_t> {};
  template <>
  struct bitwise_xor_result<uint16_t, uint8_t>: public basic_type_result<uint8_t, uint8_t> {};
  template <>
  struct bitwise_xor_result<uint16_t, int8_t>: public basic_type_result<uint8_t, int8_t> {};
  template <>
  struct bitwise_xor_result<uint16_t, uint16_t>: public basic_type_result<uint8_t, uint16_t> {};
  template <>
  struct bitwise_xor_result<uint16_t, int16_t>: public basic_type_result<uint8_t, int16_t> {};
  template <>
  struct bitwise_xor_result<int16_t, uint8_t>: public basic_type_result<uint8_t, uint8_t> {};
  template <>
  struct bitwise_xor_result<int16_t, int8_t>: public basic_type_result<uint8_t, int8_t> {};
  template <>
  struct bitwise_xor_result<int16_t, uint16_t>: public basic_type_result<uint8_t, uint16_t> {};
  template <>
  struct bitwise_xor_result<int16_t, int16_t>: public basic_type_result<uint8_t, int16_t> {};

  // select

  template <>
  struct select_result<uint8_t, uint8_t>: public basic_type_result<uint8_t, uint8_t> {};
  template <>
  struct select_result<uint8_t, int8_t>: public basic_type_result<uint8_t, int8_t> {};
  template <>
  struct select_result<uint8_t, uint16_t>: public basic_type_result<uint8_t, uint16_t> {};
  template <>
  struct select_result<uint8_t, int16_t>: public basic_type_result<uint8_t, int16_t> {};
  template <>
  struct select_result<int8_t, uint8_t>: public basic_type_result<uint8_t, uint8_t> {};
  template <>
  struct select_result<int8_t, int8_t>: public basic_type_result<uint8_t, int8_t> {};
  template <>
  struct select_result<int8_t, uint16_t>: public basic_type_result<uint8_t, uint16_t> {};
  template <>
  struct select_result<int8_t, int16_t>: public basic_type_result<uint8_t, int16_t> {};
  template <>
  struct select_result<uint16_t, uint8_t>: public basic_type_result<uint8_t, uint8_t> {};
  template <>
  struct select_result<uint16_t, int8_t>: public basic_type_result<uint8_t, int8_t> {};
  template <>
  struct select_result<uint16_t, uint16_t>: public basic_type_result<uint8_t, uint16_t> {};
  template <>
  struct select_result<uint16_t, int16_t>: public basic_type_result<uint8_t, int16_t> {};
  template <>
  struct select_result<int16_t, uint8_t>: public basic_type_result<uint8_t, uint8_t> {};
  template <>
  struct select_result<int16_t, int8_t>: public basic_type_result<uint8_t, int8_t> {};
  template <>
  struct select_result<int16_t, uint16_t>: public basic_type_result<uint8_t, uint16_t> {};
  template <>
  struct select_result<int16_t, int16_t>: public basic_type_result<uint8_t, int16_t> {};

  //==========================================================================
  //   Generic operators
  //==========================================================================

  // Function objects similar to those of the standard library, with the
  // difference that left and right operand types can differ.
  // These function objects are then used to perform basic operations
  // between containers (volumes).

  template <typename LEFT, typename RIGHT = LEFT>
  struct select_left: public std::binary_function<LEFT, RIGHT, LEFT>
  {
    const LEFT & operator() ( const LEFT & x, const RIGHT & y ) const
    {
      return x;
    }
  };

  template <typename LEFT, typename RIGHT = LEFT>
  struct select_right: public std::binary_function<LEFT, RIGHT, RIGHT>
  {
    const RIGHT & operator() ( const LEFT & x, const RIGHT & y ) const
    {
      return y;
    }
  };

  template <typename T>
  struct identity: public std::unary_function<T, T>
  {
    const T & operator() ( const T & x ) const
    {
      return x;
    }
  };

  template <typename LEFT, typename RIGHT = LEFT>
  struct plus: public std::binary_function<LEFT, RIGHT, typename plus_result<LEFT,RIGHT>::result_type>
  {
    typename plus_result<LEFT,RIGHT>::result_type
    operator() (const LEFT & x, const RIGHT & y) const
    {
      return x + y;
    }
  };

  template <typename LEFT, typename RIGHT = LEFT>
  struct minus: public std::binary_function<LEFT, RIGHT, typename minus_result<LEFT,RIGHT>::result_type>
  {
    typename minus_result<LEFT,RIGHT>::result_type
    operator() (const LEFT & x, const RIGHT & y) const
    {
      return x - y;
    }
  };

  template <typename LEFT, typename RIGHT = LEFT>
  struct multiplies: public std::binary_function<LEFT, RIGHT, typename multiplies_result<LEFT,RIGHT>::result_type>
  {
    typename multiplies_result<LEFT,RIGHT>::result_type
    operator() (const LEFT & x, const RIGHT & y) const
    {
      return x * y;
    }
  };

  template <typename LEFT, typename RIGHT = LEFT>
  struct divides: public std::binary_function<LEFT, RIGHT, typename divides_result<LEFT,RIGHT>::result_type>
  {
    typename divides_result<LEFT,RIGHT>::result_type
    operator() (const LEFT & x, const RIGHT & y) const
    {
      return x / y;
    }
  };

  template <typename LEFT, typename RIGHT = LEFT>
  struct modulus: public std::binary_function<LEFT, RIGHT, typename modulus_result<LEFT,RIGHT>::result_type>
  {
    typename modulus_result<LEFT,RIGHT>::result_type
    operator() (const LEFT & x, const RIGHT & y) const
    {
      return x % y;
    }
  };

  template <typename T>
  struct negate: public std::unary_function<T, T>
  {
    T operator() (const T & x) const
    {
      return -x;
    }
  };

  template <typename T>
  struct increment: public std::unary_function<T, T>
  {
    T operator() (T x) const
    {
      return ++x;
    }
  };

  template <typename T>
  struct decrement: public std::unary_function<T, T>
  {
    T operator() (T x) const
    {
      return --x;
    }
  };

  template <typename LEFT, typename RIGHT = LEFT>
  struct equal_to: public std::binary_function<LEFT, RIGHT, bool>
  {
    bool operator() (const LEFT & x, const RIGHT & y) const
    {
      return x == y;
    }
  };

  template <typename LEFT, typename RIGHT = LEFT>
  struct not_equal_to: public std::binary_function<LEFT, RIGHT, bool>
  {
    bool operator() (const LEFT & x, const RIGHT & y) const
    {
      return x != y;
    }
  };

  template <typename LEFT, typename RIGHT = LEFT>
  struct greater: public std::binary_function<LEFT, RIGHT, bool>
  {
    bool operator() (const LEFT & x, const RIGHT & y) const
    {
      return x > y;
    }
  };

  template <typename LEFT, typename RIGHT = LEFT>
  struct less: public std::binary_function<LEFT, RIGHT, bool>
  {
    bool operator() (const LEFT & x, const RIGHT & y) const
    {
      return x < y;
    }
  };

  template <typename LEFT, typename RIGHT = LEFT>
  struct greater_equal: public std::binary_function<LEFT, RIGHT, bool>
  {
    bool operator() (const LEFT & x, const RIGHT & y) const
    {
      return x >= y;
    }
  };

  template <typename LEFT, typename RIGHT = LEFT>
  struct less_equal: public std::binary_function<LEFT, RIGHT, bool>
  {
    bool operator() (const LEFT & x, const RIGHT & y) const
    {
      return x <= y;
    }
  };

  template <typename LEFT, typename RIGHT = LEFT>
  struct logical_and: public std::binary_function<LEFT, RIGHT, bool>
  {
    bool operator() (const LEFT & x, const RIGHT & y) const
    {
      return x && y;
    }
  };

  template <typename LEFT, typename RIGHT = LEFT>
  struct logical_or: public std::binary_function<LEFT, RIGHT, bool>
  {
    bool operator() (const LEFT & x, const RIGHT & y) const
    {
      return x || y;
    }
  };

  template <typename T>
  struct logical_not: public std::unary_function<T, bool>
  {
    bool operator() (const T & x) const
    {
      return !x;
    }
  };

  template <typename LEFT, typename RIGHT = LEFT>
  struct bitwise_and: public std::binary_function<LEFT, RIGHT, typename bitwise_and_result<LEFT,RIGHT>::result_type>
  {
    typename bitwise_and_result<LEFT,RIGHT>::result_type
    operator() (const LEFT & x, const RIGHT & y) const
    {
      return x & y;
    }
  };

  template <typename LEFT, typename RIGHT = LEFT>
  struct bitwise_or: public std::binary_function<LEFT, RIGHT, typename bitwise_or_result<LEFT,RIGHT>::result_type>
  {
    typename bitwise_or_result<LEFT,RIGHT>::result_type
    operator() (const LEFT & x, const RIGHT & y) const
    {
      return x | y;
    }
  };

  template <typename LEFT, typename RIGHT = LEFT>
  struct bitwise_xor: public std::binary_function<LEFT, RIGHT, typename bitwise_xor_result<LEFT,RIGHT>::result_type>
  {
    typename bitwise_xor_result<LEFT,RIGHT>::result_type
    operator() (const LEFT & x, const RIGHT & y) const
    {
      return x ^ y;
    }
  };

  template <typename T>
  struct bitwise_not: public std::unary_function<T, T>
  {
    T operator() (const T & x) const
    {
      return ~x;
    }
  };

  template <typename LEFT, typename RIGHT = LEFT>
  struct bitwise_left_shift: public std::binary_function<LEFT, RIGHT, LEFT>
  {
    LEFT operator() (const LEFT & x, const RIGHT & y) const
    {
      return x << y;
    }
  };

  template <typename LEFT, typename RIGHT = LEFT>
  struct bitwise_right_shift: public std::binary_function<LEFT, RIGHT, LEFT>
  {
    LEFT operator() (const LEFT & x, const RIGHT & y) const
    {
      return x >> y;
    }
  };

  template <typename LEFT, typename RIGHT = LEFT>
  struct select_min: public std::binary_function<LEFT, RIGHT, typename select_result<LEFT,RIGHT>::result_type>
  {
    typename select_result<LEFT,RIGHT>::result_type
    operator() (const LEFT & x, const RIGHT & y) const
    {
      return ( x <= y ? x : y );
    }
  };

  template <typename LEFT, typename RIGHT = LEFT>
  struct select_max: public std::binary_function<LEFT, RIGHT, typename select_result<LEFT,RIGHT>::result_type>
  {
    typename select_result<LEFT,RIGHT>::result_type
    operator() (const LEFT & x, const RIGHT & y) const
    {
      return ( x >= y ? x : y );
    }
  };

  //==========================================================================
  //   Specialization: cfloat/cdouble
  //==========================================================================

  // By default, cfloat values can only be multiplied/divided by floats
  // and cdouble by doubles.

  template <>
  struct multiplies<cfloat, double>: public std::binary_function<cfloat, double, cfloat>
  {
    cfloat operator() (const cfloat & x, const double & y) const
    {
      return x * (float)y;
    }
  };

  template <>
  struct multiplies<cfloat, long>: public std::binary_function<cfloat, long, cfloat>
  {
    cfloat operator() (const cfloat & x, const long & y) const
    {
      return x * (float)y;
    }
  };

  template <>
  struct multiplies<cdouble, float>: public std::binary_function<cdouble, float, cdouble>
  {
    cdouble operator() (const cdouble & x, const float & y) const
    {
      return x * (double)y;
    }
  };

  template <>
  struct multiplies<cdouble, long>: public std::binary_function<cdouble, long, cdouble>
  {
    cdouble operator() (const cdouble & x, const long & y) const
    {
      return x * (double)y;
    }
  };

  template <>
  struct divides<cfloat, double>: public std::binary_function<cfloat, double, cfloat>
  {
    cfloat operator() (const cfloat & x, const double & y) const
    {
      return x * (float)( 1. / y );
    }
  };

  template <>
  struct divides<cfloat, long>: public std::binary_function<cfloat, long, cfloat>
  {
    cfloat operator() (const cfloat & x, const long & y) const
    {
      return x * (float)( 1. / (double)y );
    }
  };

  template <>
  struct divides<cdouble, float>: public std::binary_function<cdouble, float, cdouble>
  {
    cdouble operator() (const cdouble & x, const float & y) const
    {
      return x * (double)( 1. / y );
    }
  };

  template <>
  struct divides<cdouble, double>: public std::binary_function<cdouble, double, cdouble>
  {
    cdouble operator() (const cdouble & x, const double & y) const
    {
      return x * ( 1. / y );
    }
  };

  template <>
  struct divides<cdouble, long>: public std::binary_function<cdouble, long, cdouble>
  {
    cdouble operator() (const cdouble & x, const long & y) const
    {
      return x * (double)( 1. / (double)y );
    }
  };

  template <>
  struct logical_and<bool, cfloat>: public std::binary_function<bool, cfloat, bool>
  {
    bool operator() (const bool & x, const cfloat & y) const
    {
      return x && ( y.imag() || y.real() );
    }
  };

  template <>
  struct logical_and<bool, cdouble>: public std::binary_function<bool, cdouble, bool>
  {
    bool operator() (const bool & x, const cdouble & y) const
    {
      return x && ( y.imag() || y.real() );
    }
  };

  template <>
  struct logical_or<bool, cfloat>: public std::binary_function<bool, cfloat, bool>
  {
    bool operator() (const bool & x, const cfloat & y) const
    {
      return x || y.imag() || y.real();
    }
  };

  template <>
  struct logical_or<bool, cdouble>: public std::binary_function<bool, cdouble, bool>
  {
    bool operator() (const bool & x, const cdouble & y) const
    {
      return x || y.imag() || y.real();
    }
  };

  template <>
  struct logical_not<cfloat>: public std::unary_function<cfloat, bool>
  {
    bool operator() (const cfloat & x) const
    {
      return !( x.imag() || x.real() );
    }
  };

  template <>
  struct logical_not<cdouble>: public std::unary_function<cdouble, bool>
  {
    bool operator() (const cdouble & x) const
    {
      return !( x.imag() || x.real() );
    }
  };

  //==========================================================================
  //   Specialization: bool
  //==========================================================================

  template <>
  struct increment<bool>: public std::unary_function<bool, bool>
  {
    bool operator() ( bool x )
    {
      return true;
    }
  };

  template <>
  struct decrement<bool>: public std::unary_function<bool, bool>
  {
    bool operator() ( bool x )
    {
      return false;
    }
  };

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

  template <>
  struct negate<VoxelRGB>: public std::unary_function<VoxelRGB, VoxelRGB>
  {
    VoxelRGB operator() ( const VoxelRGB & x )
    {
      return x * -1.f;
    }
  };

  template <>
  struct negate<VoxelRGBA>: public std::unary_function<VoxelRGBA, VoxelRGBA>
  {
    VoxelRGBA operator() ( const VoxelRGBA & x )
    {
      return x * -1.f;
    }
  };

  template <>
  struct negate<VoxelHSV>: public std::unary_function<VoxelHSV, VoxelHSV>
  {
    VoxelHSV operator() ( const VoxelHSV & x )
    {
      return x * -1.f;
    }
  };

} // namespace volumeutil
} // namespace carto

#endif // CARTODATA_VOLUME_FUNCTIONAL_H
