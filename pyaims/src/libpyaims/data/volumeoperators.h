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

#ifndef PYAIMS_DATA_VOLUMEOPERATORS_H
#define PYAIMS_DATA_VOLUMEOPERATORS_H

#include <cartodata/volume/volume.h>

namespace
{

  template <typename L, typename R>
  struct __add_struct__ : public carto::volumeutil::plus_result<L, R>
  {
    static typename __add_struct__::result_type* doit( L & l, R & r )
    {
      return new typename __add_struct__::result_type( l + r );
    }
  };


  template <typename L, typename R>
  struct __add_struct__<carto::Volume<L>, R>
    : public carto::volumeutil::plus_result<carto::Volume<L>, R>
  {
    static typename __add_struct__::result_type* doit( carto::Volume<L> & l,
                                                       R & r )
    {
      typedef typename __add_struct__::result_type P;
      carto::VolumeRef<L> rl( &l );
      carto::VolumeRef<typename P::datatype> rres = rl + r;
      P *res = rres.get();
      rres.release();
      return res;
    }
  };


  template <typename L, typename R>
  struct __add_struct__<L, carto::Volume<R> >
    : public carto::volumeutil::plus_result<L, carto::Volume<R> >
  {
    static typename __add_struct__::result_type* doit( L & l,
                                                       carto::Volume<R> & r )
    {
      typedef typename __add_struct__::result_type P;
      carto::VolumeRef<R> rr( &r );
      carto::VolumeRef<typename P::datatype> rres = l + rr;
      P *res = rres.get();
      rres.release();
      return res;
    }
  };


  template <typename L, typename R>
  struct __add_struct__<carto::Volume<L>, carto::Volume<R> >
    : public carto::volumeutil::plus_result<carto::Volume<L>,
                                            carto::Volume<R> >
  {
    static typename __add_struct__::result_type* doit( carto::Volume<L> & l,
                                                       carto::Volume<R> & r )
    {
      typedef typename __add_struct__::result_type P;
      carto::VolumeRef<L> rl( &l );
      carto::VolumeRef<R> rr( &r );
      carto::VolumeRef<typename P::datatype> rres = rl + rr;
      P *res = rres.get();
      rres.release();
      return res;
    }
  };


  template <typename L, typename R>
  typename carto::volumeutil::plus_result<L, R>::result_type *
  __add__( L & l, R & r )
  {
    return __add_struct__<L, R>::doit( l, r );
  }


  template <typename L, typename R>
  L & __iadd__( L & l, R & r )
  {
    l += r;
    return l;
  }


  template <typename L, typename R>
  struct __sub_struct__ : public carto::volumeutil::minus_result<L, R>
  {
    static typename __sub_struct__::result_type* doit( L & l, R & r )
    {
      return new typename __sub_struct__::result_type( l - r );
    }
  };


  template <typename L, typename R>
  struct __sub_struct__<carto::Volume<L>, R>
    : public carto::volumeutil::minus_result<carto::Volume<L>, R>
  {
    static typename __sub_struct__::result_type* doit( carto::Volume<L> & l,
                                                       R & r )
    {
      typedef typename __sub_struct__::result_type P;
      carto::VolumeRef<L> rl( &l );
      carto::VolumeRef<typename P::datatype> rres = rl - r;
      P *res = rres.get();
      rres.release();
      return res;
    }
  };


  template <typename L, typename R>
  struct __sub_struct__<L, carto::Volume<R> >
    : public carto::volumeutil::minus_result<L, carto::Volume<R> >
  {
    static typename __sub_struct__::result_type* doit( L & l,
                                                       carto::Volume<R> & r )
    {
      typedef typename __sub_struct__::result_type P;
      carto::VolumeRef<R> rr( &r );
      carto::VolumeRef<typename P::datatype> rres = l - rr;
      P *res = rres.get();
      rres.release();
      return res;
    }
  };


  template <typename L, typename R>
  struct __sub_struct__<carto::Volume<L>, carto::Volume<R> >
    : public carto::volumeutil::minus_result<carto::Volume<L>,
                                             carto::Volume<R> >
  {
    static typename __sub_struct__::result_type* doit( carto::Volume<L> & l,
                                                       carto::Volume<R> & r )
    {
      typedef typename __sub_struct__::result_type P;
      carto::VolumeRef<L> rl( &l );
      carto::VolumeRef<R> rr( &r );
      carto::VolumeRef<typename P::datatype> rres = rl - rr;
      P *res = rres.get();
      rres.release();
      return res;
    }
  };


  template <typename L, typename R>
  typename carto::volumeutil::minus_result<L, R>::result_type *
  __sub__( L & l, R & r )
  {
    return __sub_struct__<L, R>::doit( l, r );
  }


  template <typename L, typename R>
  L & __isub__( L & l, R & r )
  {
    l -= r;
    return l;
  }


  template <typename L, typename R>
  struct __mul_struct__ : public carto::volumeutil::multiplies_result<L, R>
  {
    static typename __mul_struct__::result_type* doit( L & l, R & r )
    {
      return new typename __mul_struct__::result_type( l * r );
    }
  };


  template <typename L, typename R>
  struct __mul_struct__<carto::Volume<L>, R>
    : public carto::volumeutil::multiplies_result<carto::Volume<L>, R>
  {
    static typename __mul_struct__::result_type* doit( carto::Volume<L> & l,
                                                       R & r )
    {
      typedef typename __mul_struct__::result_type P;
      carto::VolumeRef<L> rl( &l );
      carto::VolumeRef<typename P::datatype> rres = rl * r;
      P *res = rres.get();
      rres.release();
      return res;
    }
  };


  template <typename L, typename R>
  struct __mul_struct__<L, carto::Volume<R> >
    : public carto::volumeutil::multiplies_result<L, carto::Volume<R> >
  {
    static typename __mul_struct__::result_type* doit( L & l,
                                                       carto::Volume<R> & r )
    {
      typedef typename __mul_struct__::result_type P;
      carto::VolumeRef<R> rr( &r );
      carto::VolumeRef<typename P::datatype> rres = l * rr;
      P *res = rres.get();
      rres.release();
      return res;
    }
  };


  template <typename L, typename R>
  struct __mul_struct__<carto::Volume<L>, carto::Volume<R> >
    : public carto::volumeutil::multiplies_result<carto::Volume<L>,
                                                  carto::Volume<R> >
  {
    static typename __mul_struct__::result_type* doit( carto::Volume<L> & l,
                                                       carto::Volume<R> & r )
    {
      typedef typename __mul_struct__::result_type P;
      carto::VolumeRef<L> rl( &l );
      carto::VolumeRef<R> rr( &r );
      carto::VolumeRef<typename P::datatype> rres = rl * rr;
      P *res = rres.get();
      rres.release();
      return res;
    }
  };


  template <typename L, typename R>
  typename carto::volumeutil::multiplies_result<L, R>::result_type *
  __mul__( L & l, R & r )
  {
    return __mul_struct__<L, R>::doit( l, r );
  }


  template <typename L, typename R>
  L & __imul__( L & l, R & r )
  {
    l *= r;
    return l;
  }


  template <typename L, typename R>
  struct __div_struct__ : public carto::volumeutil::divides_result<L, R>
  {
    static typename __div_struct__::result_type* doit( L & l, R & r )
    {
      return new typename __div_struct__::result_type( l / r );
    }
  };


  template <typename L, typename R>
  struct __div_struct__<carto::Volume<L>, R>
    : public carto::volumeutil::divides_result<carto::Volume<L>, R>
  {
    static typename __div_struct__::result_type* doit( carto::Volume<L> & l,
                                                       R & r )
    {
      typedef typename __div_struct__::result_type P;
      carto::VolumeRef<L> rl( &l );
      carto::VolumeRef<typename P::datatype> rres = rl / r;
      P *res = rres.get();
      rres.release();
      return res;
    }
  };


  template <typename L, typename R>
  struct __div_struct__<L, carto::Volume<R> >
    : public carto::volumeutil::divides_result<L, carto::Volume<R> >
  {
    static typename __div_struct__::result_type* doit( L & l,
                                                       carto::Volume<R> & r )
    {
      typedef typename __div_struct__::result_type P;
      carto::VolumeRef<R> rr( &r );
      carto::VolumeRef<typename P::datatype> rres = l / rr;
      P *res = rres.get();
      rres.release();
      return res;
    }
  };


  template <typename L, typename R>
  struct __div_struct__<carto::Volume<L>, carto::Volume<R> >
    : public carto::volumeutil::divides_result<carto::Volume<L>,
                                               carto::Volume<R> >
  {
    static typename __div_struct__::result_type* doit( carto::Volume<L> & l,
                                                       carto::Volume<R> & r )
    {
      typedef typename __div_struct__::result_type P;
      carto::VolumeRef<L> rl( &l );
      carto::VolumeRef<R> rr( &r );
      carto::VolumeRef<typename P::datatype> rres = rl / rr;
      P *res = rres.get();
      rres.release();
      return res;
    }
  };


  template <typename L, typename R>
  typename carto::volumeutil::divides_result<L, R>::result_type *
  __div__( L & l, R & r )
  {
    return __div_struct__<L, R>::doit( l, r );
  }


  template <typename L, typename R>
  L & __idiv__( L & l, R & r )
  {
    l /= r;
    return l;
  }


  template <typename L, typename R>
  struct __mod_struct__ : public carto::volumeutil::modulus_result<L, R>
  {
    static typename __mod_struct__::result_type* doit( L & l, R & r )
    {
      return new typename __mod_struct__::result_type( l % r );
    }
  };


  template <typename L, typename R>
  struct __mod_struct__<carto::Volume<L>, R>
    : public carto::volumeutil::modulus_result<carto::Volume<L>, R>
  {
    static typename __mod_struct__::result_type* doit( carto::Volume<L> & l,
                                                       R & r )
    {
      typedef typename __mod_struct__::result_type P;
      carto::VolumeRef<L> rl( &l );
      carto::VolumeRef<typename P::datatype> rres = rl % r;
      P *res = rres.get();
      rres.release();
      return res;
    }
  };


  template <typename L, typename R>
  struct __mod_struct__<L, carto::Volume<R> >
    : public carto::volumeutil::multiplies_result<L, carto::Volume<R> >
  {
    static typename __mod_struct__::result_type* doit( L & l,
                                                       carto::Volume<R> & r )
    {
      typedef typename __mod_struct__::result_type P;
      carto::VolumeRef<R> rr( &r );
      carto::VolumeRef<typename P::datatype> rres = l % rr;
      P *res = rres.get();
      rres.release();
      return res;
    }
  };


  template <typename L, typename R>
  struct __mod_struct__<carto::Volume<L>, carto::Volume<R> >
    : public carto::volumeutil::multiplies_result<carto::Volume<L>,
                                                  carto::Volume<R> >
  {
    static typename __mod_struct__::result_type* doit( carto::Volume<L> & l,
                                                       carto::Volume<R> & r )
    {
      typedef typename __mod_struct__::result_type P;
      carto::VolumeRef<L> rl( &l );
      carto::VolumeRef<R> rr( &r );
      carto::VolumeRef<typename P::datatype> rres = rl % rr;
      P *res = rres.get();
      rres.release();
      return res;
    }
  };


  template <typename L, typename R>
  typename carto::volumeutil::modulus_result<L, R>::result_type *
  __mod__( L & l, R & r )
  {
    return __mod_struct__<L, R>::doit( l, r );
  }


  template <typename L, typename R>
  L & __imod__( L & l, R & r )
  {
    l %= r;
    return l;
  }


  template <typename L, typename R>
  struct __and_struct__ : public carto::volumeutil::bitwise_and_result<L, R>
  {
    static typename __and_struct__::result_type* doit( L & l, R & r )
    {
      return new typename __and_struct__::result_type( l & r );
    }
  };


  template <typename L, typename R>
  struct __and_struct__<carto::Volume<L>, R>
    : public carto::volumeutil::bitwise_and_result<carto::Volume<L>, R>
  {
    static typename __and_struct__::result_type* doit( carto::Volume<L> & l,
                                                       R & r )
    {
      typedef typename __and_struct__::result_type P;
      carto::VolumeRef<L> rl( &l );
      carto::VolumeRef<typename P::datatype> rres = rl & r;
      P *res = rres.get();
      rres.release();
      return res;
    }
  };


  template <typename L, typename R>
  struct __and_struct__<L, carto::Volume<R> >
    : public carto::volumeutil::bitwise_and_result<L, carto::Volume<R> >
  {
    static typename __and_struct__::result_type* doit( L & l,
                                                       carto::Volume<R> & r )
    {
      typedef typename __and_struct__::result_type P;
      carto::VolumeRef<R> rr( &r );
      carto::VolumeRef<typename P::datatype> rres = l & rr;
      P *res = rres.get();
      rres.release();
      return res;
    }
  };


  template <typename L, typename R>
  struct __and_struct__<carto::Volume<L>, carto::Volume<R> >
    : public carto::volumeutil::bitwise_and_result<carto::Volume<L>,
                                                   carto::Volume<R> >
  {
    static typename __and_struct__::result_type* doit( carto::Volume<L> & l,
                                                       carto::Volume<R> & r )
    {
      typedef typename __and_struct__::result_type P;
      carto::VolumeRef<L> rl( &l );
      carto::VolumeRef<R> rr( &r );
      carto::VolumeRef<typename P::datatype> rres = rl & rr;
      P *res = rres.get();
      rres.release();
      return res;
    }
  };


  template <typename L, typename R>
  typename carto::volumeutil::bitwise_and_result<L, R>::result_type *
  __and__( L & l, R & r )
  {
    return __and_struct__<L, R>::doit( l, r );
  }


  template <typename L, typename R>
  L & __iand__( L & l, R & r )
  {
    l &= r;
    return l;
  }


  template <typename L, typename R>
  struct __or_struct__ : public carto::volumeutil::bitwise_or_result<L, R>
  {
    static typename __or_struct__::result_type* doit( L & l, R & r )
    {
      return new typename __or_struct__::result_type( l | r );
    }
  };


  template <typename L, typename R>
  struct __or_struct__<carto::Volume<L>, R>
    : public carto::volumeutil::bitwise_or_result<carto::Volume<L>, R>
  {
    static typename __or_struct__::result_type* doit( carto::Volume<L> & l,
                                                      R & r )
    {
      typedef typename __or_struct__::result_type P;
      carto::VolumeRef<L> rl( &l );
      carto::VolumeRef<typename P::datatype> rres = rl | r;
      P *res = rres.get();
      rres.release();
      return res;
    }
  };


  template <typename L, typename R>
  struct __or_struct__<L, carto::Volume<R> >
    : public carto::volumeutil::bitwise_or_result<L, carto::Volume<R> >
  {
    static typename __or_struct__::result_type* doit( L & l,
                                                      carto::Volume<R> & r )
    {
      typedef typename __or_struct__::result_type P;
      carto::VolumeRef<R> rr( &r );
      carto::VolumeRef<typename P::datatype> rres = l | rr;
      P *res = rres.get();
      rres.release();
      return res;
    }
  };


  template <typename L, typename R>
  struct __or_struct__<carto::Volume<L>, carto::Volume<R> >
    : public carto::volumeutil::bitwise_or_result<carto::Volume<L>,
                                                  carto::Volume<R> >
  {
    static typename __or_struct__::result_type* doit( carto::Volume<L> & l,
                                                      carto::Volume<R> & r )
    {
      typedef typename __or_struct__::result_type P;
      carto::VolumeRef<L> rl( &l );
      carto::VolumeRef<R> rr( &r );
      carto::VolumeRef<typename P::datatype> rres = rl | rr;
      P *res = rres.get();
      rres.release();
      return res;
    }
  };


  template <typename L, typename R>
  typename carto::volumeutil::bitwise_or_result<L, R>::result_type *
  __or__( L & l, R & r )
  {
    return __or_struct__<L, R>::doit( l, r );
  }


  template <typename L, typename R>
  L & __ior__( L & l, R & r )
  {
    l |= r;
    return l;
  }


  template <typename L, typename R>
  struct __xor_struct__ : public carto::volumeutil::bitwise_xor_result<L, R>
  {
    static typename __xor_struct__::result_type* doit( L & l, R & r )
    {
      return new typename __xor_struct__::result_type( l ^ r );
    }
  };


  template <typename L, typename R>
  struct __xor_struct__<carto::Volume<L>, R>
    : public carto::volumeutil::bitwise_xor_result<carto::Volume<L>, R>
  {
    static typename __xor_struct__::result_type* doit( carto::Volume<L> & l,
                                                       R & r )
    {
      typedef typename __xor_struct__::result_type P;
      carto::VolumeRef<L> rl( &l );
      carto::VolumeRef<typename P::datatype> rres = rl ^ r;
      P *res = rres.get();
      rres.release();
      return res;
    }
  };


  template <typename L, typename R>
  struct __xor_struct__<L, carto::Volume<R> >
    : public carto::volumeutil::bitwise_xor_result<L, carto::Volume<R> >
  {
    static typename __xor_struct__::result_type* doit( L & l,
                                                      carto::Volume<R> & r )
    {
      typedef typename __xor_struct__::result_type P;
      carto::VolumeRef<R> rr( &r );
      carto::VolumeRef<typename P::datatype> rres = l ^ rr;
      P *res = rres.get();
      rres.release();
      return res;
    }
  };


  template <typename L, typename R>
  struct __xor_struct__<carto::Volume<L>, carto::Volume<R> >
    : public carto::volumeutil::bitwise_xor_result<carto::Volume<L>,
                                                   carto::Volume<R> >
  {
    static typename __xor_struct__::result_type* doit( carto::Volume<L> & l,
                                                       carto::Volume<R> & r )
    {
      typedef typename __xor_struct__::result_type P;
      carto::VolumeRef<L> rl( &l );
      carto::VolumeRef<R> rr( &r );
      carto::VolumeRef<typename P::datatype> rres = rl ^ rr;
      P *res = rres.get();
      rres.release();
      return res;
    }
  };


  template <typename L, typename R>
  typename carto::volumeutil::bitwise_xor_result<L, R>::result_type *
  __xor__( L & l, R & r )
  {
    return __xor_struct__<L, R>::doit( l, r );
  }


  template <typename L, typename R>
  L & __ixor__( L & l, R & r )
  {
    l ^= r;
    return l;
  }

}

#endif

