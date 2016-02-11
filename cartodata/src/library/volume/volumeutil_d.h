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

#ifndef CARTODATA_VOLUME_VOLUMEUTIL_D_H
#define CARTODATA_VOLUME_VOLUMEUTIL_D_H

#include <cartodata/volume/volumeutil.h>
#include <cartobase/type/datatypetraits.h>
#include <cartobase/exception/assert.h>
#include <limits>

/* bug in gcc 4.2, accumulate doesn't accept std::max<T> or std::min<T> as
    argument, but accepts a function without a namespace */
/* in C++11 (g++ --std=c++11) std::min and std::max<T> cannot be passed to a
   function also - because there are other variants, like
   max( std::initializer_list<T> ilist ). */

namespace
{
  template <typename T>
  inline
  T internal_max( const T x, const T y )
  { return std::max<T>( x, y ); }

  template <typename T>
  inline
  T internal_min( const T x, const T y )
  { return std::min<T>( x, y ); }
}


namespace carto
{

  template <typename T> template <class UnaryFunction>
  VolumeRef<T> VolumeUtil<T>::apply( UnaryFunction f, const VolumeRef<T> & o )
  {
    VolumeRef<T>	res
      ( new Volume<T>( o->getSizeX(), o->getSizeY(), o->getSizeZ(),
                       o->getSizeT() ) );
    res->header() = o->header();

    unsigned	x, nx = o->getSizeX(), y, ny = o->getSizeY(),
      z, nz = o->getSizeZ(), t, nt = o->getSizeT();
    T	*op, *rp;
    for( t=0; t<nt; ++t )
      for( z=0; z<nz; ++z )
        for( y=0; y<ny; ++y )
          {
            op = &o->at( 0, y, z, t );
            rp = &res->at( 0, y, z, t );
            for( x=0; x<nx; ++x )
              *rp++ = f( *op++ );
          }
    return res;
  }


  namespace internal
  {

    template <typename T> inline T _neutral()
    {
      return T();
    }

    template<> inline int8_t _neutral<int8_t>()
    {
      return 0;
    }

    template<> inline uint8_t _neutral<uint8_t>()
    {
      return 0;
    }

    template<> inline int16_t _neutral<int16_t>()
    {
      return 0;
    }

    template<> inline uint16_t _neutral<uint16_t>()
    {
      return 0;
    }

    template<> inline int32_t _neutral<int32_t>()
    {
      return 0;
    }

    template<> inline uint32_t _neutral<uint32_t>()
    {
      return 0;
    }

    template<> inline float _neutral<float>()
    {
      return 0;
    }

    template<> inline double _neutral<double>()
    {
      return 0;
    }

  }


  template <typename T> template <class BinaryFunction>
  VolumeRef<T> VolumeUtil<T>::apply( BinaryFunction f,
                                     const VolumeRef<T> & o1,
                                     const VolumeRef<T> & o2 )
  {
    int	x, nx = o1->getSizeX(), y, ny = o1->getSizeY(),
      z, nz = o1->getSizeZ(), t, nt = o1->getSizeT(), nx2, ny2, nz2, nt2, nx3;
    if( o2->getSizeX() < nx )
      {
        nx2 = nx;
        nx = o2->getSizeX();
      }
    else
      nx2 = o2->getSizeX();
    if( o2->getSizeY() < ny )
      {
        ny2 = ny;
        ny = o2->getSizeY();
      }
    else
      ny2 = o2->getSizeY();
    if( o2->getSizeZ() < nz )
      {
        nz2 = nz;
        nz = o2->getSizeZ();
      }
    else
      nz2 = o2->getSizeZ();
    if( o2->getSizeT() < nt )
      {
        nt2 = nt;
        nt = o2->getSizeT();
      }
    else
      nt2 = o2->getSizeT();

    VolumeRef<T>	res( new Volume<T>( nx2, ny2, nz2, nt2 ) );
    res->header() = o1->header();

    bool	x1, y1, z1, x2, y2, z2;
    T	*o1p, *o2p, *rp;
    for( t=0; t<nt2; ++t )
      {
        if( t >= nt )
          if( nt == o1->getSizeT() )
            {
              z1 = false;
              z2 = true;
            }
          else
            {
              z1 = true;
              z2 = false;
            }
        else
          {
            z1 = true;
            z2 = true;
          }

        for( z=0; z<nz2; ++z )
          {
            y1 = z1;
            y2 = z2;
            if( z >= nz )
            {
              if( nz == o1->getSizeZ() )
                y1 = false;
              else
                y2 = false;
            }

            for( y=0; y<ny2; ++y )
              {
                x1 = y1;
                x2 = y2;
                if( y >= ny )
                {
                  if( ny == o1->getSizeY() )
                    x1 = false;
                  else
                    x2 = false;
                }
                if( x1 )
                  o1p = &o1->at( 0, y, z, t );
                else
                  o1p = 0;
                if( x2 )
                  o2p = &o2->at( 0, y, z, t );
                else
                  o2p = 0;
                rp = &res->at( 0, y, z, t );

                /*
                std::cout << "pos: " << t << ", " << z << ", " << y
                          << ", x1: " << x1 << ", " << y1 << ", " << z1
                          << ", x2: " << x2 << ", "  << y2 << ", " << z2
                          << std::endl;
                */

                x = 0;
                if( x1 && x2 )
                  {
                    for( ; x<nx; ++x )
                      *rp++ = f( *o1p++, *o2p++ );
                    nx3 = nx2;
                    if( nx == o1->getSizeX() )
                      x1 = false;
                    else
                      x2 = false;
                  }
                else if( x1 )
                  nx3 = o1->getSizeX();
                else if( x2 )
                  nx3 = o2->getSizeX();
                else
                  nx3 = 0;
                if( !x1 && x2 )
                  for( ; x<nx3; ++x )
                    *rp++ = f( internal::_neutral<T>(), *o2p++ );
                else if( x1 && !x2 )
                  for( ; x<nx3; ++x )
                    *rp++ = f( *o1p++, internal::_neutral<T>() );
                for( ; x<nx2; ++x )
                  *rp++ = f( internal::_neutral<T>(),
                             internal::_neutral<T>() );
              }
          }
      }
    return res;
  }


  template <typename T> template <class UnaryFunction>
  void VolumeUtil<T>::selfApply( UnaryFunction f, VolumeRef<T> & o )
  {
    int	x, nx = o->getSizeX(), y, ny = o->getSizeY(),
      z, nz = o->getSizeZ(), t, nt = o->getSizeT();
    T	*op;
    for( t=0; t<nt; ++t )
      for( z=0; z<nz; ++z )
        for( y=0; y<ny; ++y )
          {
            op = &o->at( 0, y, z, t );
            for( x=0; x<nx; ++x, ++op )
              *op = f( *op );
          }
  }


  template <typename T> template <class BinaryFunction>
  void VolumeUtil<T>::selfApply( BinaryFunction f, VolumeRef<T> & o1,
                                 const VolumeRef<T> & o2 )
  {
    int	x, nx = o1->getSizeX(), y, ny = o1->getSizeY(),
      z, nz = o1->getSizeZ(), t, nt = o1->getSizeT();

    if( o2->getSizeX() < nx )
      nx = o2->getSizeX();
    if( o2->getSizeY() < ny )
      ny = o2->getSizeY();
    if( o2->getSizeZ() < nz )
      nz = o2->getSizeZ();
    if( o2->getSizeT() < nt )
      nt = o2->getSizeT();

    T	*o1p, *o2p;
    for( t=0; t<nt; ++t )
      for( z=0; z<nz; ++z )
        for( y=0; y<ny; ++y )
          {
            o1p = &o1->at( 0, y, z, t );
            o2p = &o2->at( 0, y, z, t );
            for( x=0; x<nx; ++x, ++o1p )
              *o1p = f( *o1p, *o2p++ );
          }
  }


  template <typename T> template <class BinaryFunction>
  T VolumeUtil<T>::accumulate( BinaryFunction f,
                               const Volume<T> & o, T initial )
  {
    T res = initial;

    unsigned    x, nx = o.getSizeX(), y, ny = o.getSizeY(),
      z, nz = o.getSizeZ(), t, nt = o.getSizeT();
    const T   *op;
    for( t=0; t<nt; ++t )
      for( z=0; z<nz; ++z )
        for( y=0; y<ny; ++y )
          {
            op = &o.at( 0, y, z, t );
            for( x=0; x<nx; ++x )
              res = f( *op++, res );
          }
    return res;
  }

  template <typename T>
  T VolumeUtilBase<T, true>::min( const Volume<T> & o )
  {
    return carto::VolumeUtil<T>::accumulate(
              internal_min<T>, o, std::numeric_limits<T>::max() );
  }

  template <typename T>
  T VolumeUtilBase<T, true>::max( const Volume<T> & o )
  {
    return carto::VolumeUtil<T>::accumulate(
              internal_max<T>, o, -std::numeric_limits<T>::max() );
  }


  //--------------------------------------------------------------------------
  // Copy functions
  //--------------------------------------------------------------------------

  template <typename T>
  void transfer( const carto::VolumeRef<T> & src,
                 carto::VolumeRef<T> & dst )
  {
    ASSERT( ( src.getSizeX() * src.getSizeY() *
              src.getSizeZ() * src.getSizeT() ) ==
            ( dst.getSizeX() * dst.getSizeY() *
              dst.getSizeZ() * dst.getSizeT() ) );

    typename carto::VolumeRef<T>::const_iterator i;
    typename carto::VolumeRef<T>::iterator j;

    for( i = src.begin(), j = dst.begin(); i != src.end(); ++i, ++j )
      *j = *i;
  }

  template <typename T>
  carto::VolumeRef<T> deepcopy( const carto::VolumeRef<T> & src,
                                bool copy_full_structure )
  {
    carto::VolumeRef<T> dst( (carto::Volume<T>*)0 );
    if( copy_full_structure )
    {
      carto::VolumeRef<T> src_cur = src;
      carto::VolumeRef<T> src_prv = src;
      carto::VolumeRef<T> dst_cur = dst;
      carto::VolumeRef<T> dst_prv = dst;
      while( src_cur )
      {
        dst_cur.reset( new carto::Volume<T>(
          src_cur.getSizeX(),
          src_cur.getSizeY(),
          src_cur.getSizeZ(),
          src_cur.getSizeT(),
          carto::AllocatorContext(),
          src_cur->allocatorContext().isAllocated() ) );
        dst_cur->copyHeaderFrom( src_cur.header() );
        if( src_cur->allocatorContext().isAllocated() )
          transfer( src_cur, dst_cur );

        if( dst_prv )
        {
          dst_prv->setRefVolume( dst_cur );
          dst_prv->setPosInRefVolume( src_prv->posInRefVolume() );
        }
        else
          dst = dst_cur;

        dst_prv = dst_cur;
        src_prv = src_cur;
        src_cur = src_cur->refVolume();
      }
    }
    else
    {
      dst.reset( new carto::Volume<T>(
        src.getSizeX(), src.getSizeY(),
        src.getSizeZ(), src.getSizeT() ) );
      dst->copyHeaderFrom( src.header() );
      transfer( src, dst );
    }

    return dst;
  }

  template <typename T>
  carto::VolumeRef<T> copy( const carto::VolumeRef<T> & src )
  {
    return deepcopy( src, false );
  }


}

#endif

