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
#include <cartodata/volume/volumebase.h>
#include <cartodata/volume/volumebase_d_inline.h>
#include <cartodata/volume/volumebase_d_instantiate.h>
#include <cartodata/volume/volumeref.h>
#include <cartodata/volume/volumeref_d_inline.h>
#include <cartodata/volume/volumeref_d_instantiate.h>
#include <cartobase/type/datatypetraits.h>
#include <cartobase/exception/assert.h>
#include <cartobase/containers/nditerator.h>
#include <cartobase/type/converter.h>  // carto::min_limit<T>()
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

    const_line_NDIterator<T> it( &o->at( 0 ), o->getSize(), o->getStrides() );
    line_NDIterator<T> rit( &res->at( 0 ), res->getSize(), res->getStrides() );
    const T *op, *pp;
    T *rp;

    for( ; !it.ended(); ++it, ++rit )
    {
      op = &*it;
      rp = &*rit;
      for( pp=op + it.line_length(); op!=pp;
           it.inc_line_ptr( op ), rit.inc_line_ptr( rp ) )
        *rp = f( *op );
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
                    res->at( x, y, z, t ) = f( o1->at( x, y, z, t ),
                                               o2->at( x, y, z, t ) );
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
                    res->at( x, y, z, t ) = f( internal::_neutral<T>(),
                                               o2->at( x, y, z, t ) );
                else if( x1 && !x2 )
                  for( ; x<nx3; ++x )
                    res->at( x, y, z, t ) = f( o1->at( x, y, z, t ),
                                               internal::_neutral<T>() );
                for( ; x<nx2; ++x )
                  res->at( x, y, z, t ) = f( internal::_neutral<T>(),
                                             internal::_neutral<T>() );
              }
          }
      }
    return res;
  }


  template <typename T> template <class UnaryFunction>
  void VolumeUtil<T>::selfApply( UnaryFunction f, VolumeRef<T> & o )
  {
    line_NDIterator<T> it( &o->at( 0 ), o->getSize(), o->getStrides(), true );
    T *op, *pp;

    for( ; !it.ended(); ++it )
    {
      op = &*it;
      for( pp=op + it.line_length(); op!=pp; it.inc_line_ptr( op ) )
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

    line_NDIterator<T> o1it( &o1->at( 0 ), o1->getSize(), o1->getStrides() );
    const_line_NDIterator<T> o2it( &o2->at( 0 ), o2->getSize(),
                                   o2->getStrides() );
    T *o1p, *pp;
    const T *o2p;

    for( ; !o1it.ended(); ++o1it, ++o2it )
    {
      o1p = &*o1it;
      o2p = &*o2it;
      for( pp=o1p + o1it.line_length(); o1p!=pp;
           o1it.inc_line_ptr( o1p ), o2it.inc_line_ptr( o2p ) )
        *o1p = f( *o1p, *o2p );
    }
  }


  template <typename T> template <class BinaryFunction>
  T VolumeUtil<T>::accumulate( BinaryFunction f,
                               const Volume<T> & o, T initial )
  {
    T res = initial;

    const_line_NDIterator<T> it( &o.at( 0 ), o.getSize(), o.getStrides(),
                                 true );
    const T *op, *pp;

    for( ; !it.ended(); ++it )
    {
      op = &*it;
      for( pp=op + it.line_length(); op!=pp; it.inc_line_ptr( op ) )
        res = f( *op, res );
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
              internal_max<T>, o, min_limit<T>() );
  }

#if 0
  // VolumeUtil declarations (needed on Mac)
  extern template VolumeRef<VoxelRGB>
  VolumeUtil<VoxelRGB>::apply( Scaler<VoxelRGB, double>,
                              const VolumeRef<VoxelRGB> & );
  extern template void
  VolumeUtil<VoxelRGB>::selfApply( Scaler<VoxelRGB, double>,
                                  VolumeRef<VoxelRGB> & );
  extern template VolumeRef<VoxelRGB>
  VolumeUtil<VoxelRGB>::apply( std::plus<VoxelRGB>, const VolumeRef<VoxelRGB> &,
                              const VolumeRef<VoxelRGB> & );
  extern template void
  VolumeUtil<VoxelRGB>::selfApply( std::plus<VoxelRGB>, VolumeRef<VoxelRGB> &,
                                  const VolumeRef<VoxelRGB> & );
  extern template VolumeRef<VoxelRGB>
  VolumeUtil<VoxelRGB>::apply( UnaryFromConstantBinaryFunctor<VoxelRGB,
                              std::plus<VoxelRGB> >,
                              const VolumeRef<VoxelRGB> & );
  extern template void
  VolumeUtil<VoxelRGB>::selfApply( UnaryFromConstantBinaryFunctor<VoxelRGB,
                                  std::plus<VoxelRGB> >, VolumeRef<VoxelRGB> & );
  extern template VolumeRef<VoxelRGB>
  VolumeUtil<VoxelRGB>::apply( UnaryFromConstantBinaryFunctor2<VoxelRGB,
                              std::plus<VoxelRGB> >,
                              const VolumeRef<VoxelRGB> & );
  extern template VolumeRef<VoxelRGB>
  VolumeUtil<VoxelRGB>::apply( std::minus<VoxelRGB>, const VolumeRef<VoxelRGB> &,
                              const VolumeRef<VoxelRGB> & );
  extern template void
  VolumeUtil<VoxelRGB>::selfApply( std::minus<VoxelRGB>, VolumeRef<VoxelRGB> &,
                                  const VolumeRef<VoxelRGB> & );
  extern template VolumeRef<VoxelRGB>
  VolumeUtil<VoxelRGB>::apply( UnaryFromConstantBinaryFunctor<VoxelRGB,
                              std::minus<VoxelRGB> >,
                              const VolumeRef<VoxelRGB> & );
  extern template void
  VolumeUtil<VoxelRGB>::selfApply( UnaryFromConstantBinaryFunctor<VoxelRGB,
                                  std::minus<VoxelRGB> >,
                                  VolumeRef<VoxelRGB> & );
  extern template VolumeRef<VoxelRGB>
  VolumeUtil<VoxelRGB>::apply( UnaryFromConstantBinaryFunctor2<VoxelRGB,
                              std::minus<VoxelRGB> >,
                              const VolumeRef<VoxelRGB> & );

  extern template VolumeRef<VoxelRGBA>
  VolumeUtil<VoxelRGBA>::apply( Scaler<VoxelRGBA, double>,
                               const VolumeRef<VoxelRGBA> & );
  extern template void
  VolumeUtil<VoxelRGBA>::selfApply( Scaler<VoxelRGBA, double>,
                                   VolumeRef<VoxelRGBA> & );
  extern template VolumeRef<VoxelRGBA>
  VolumeUtil<VoxelRGBA>::apply( std::plus<VoxelRGBA>,
                               const VolumeRef<VoxelRGBA> &,
                               const VolumeRef<VoxelRGBA> & );
  extern template void
  VolumeUtil<VoxelRGBA>::selfApply( std::plus<VoxelRGBA>, VolumeRef<VoxelRGBA> &,
                                   const VolumeRef<VoxelRGBA> & );
  extern template VolumeRef<VoxelRGBA>
  VolumeUtil<VoxelRGBA>::apply( UnaryFromConstantBinaryFunctor<VoxelRGBA,
                               std::plus<VoxelRGBA> >,
                               const VolumeRef<VoxelRGBA> & );
  extern template void
  VolumeUtil<VoxelRGBA>::selfApply( UnaryFromConstantBinaryFunctor<VoxelRGBA,
                                   std::plus<VoxelRGBA> >,
                                   VolumeRef<VoxelRGBA> & );
  extern template VolumeRef<VoxelRGBA>
  VolumeUtil<VoxelRGBA>::apply( UnaryFromConstantBinaryFunctor2<VoxelRGBA,
                               std::plus<VoxelRGBA> >,
                               const VolumeRef<VoxelRGBA> & );
  extern template VolumeRef<VoxelRGBA>
  VolumeUtil<VoxelRGBA>::apply( std::minus<VoxelRGBA>,
                               const VolumeRef<VoxelRGBA> &,
                               const VolumeRef<VoxelRGBA> & );
  extern template void
  VolumeUtil<VoxelRGBA>::selfApply( std::minus<VoxelRGBA>,
                                   VolumeRef<VoxelRGBA> &,
                                   const VolumeRef<VoxelRGBA> & );
  extern template VolumeRef<VoxelRGBA>
  VolumeUtil<VoxelRGBA>::apply( UnaryFromConstantBinaryFunctor<VoxelRGBA,
                               std::minus<VoxelRGBA> >,
                               const VolumeRef<VoxelRGBA> & );
  extern template void
  VolumeUtil<VoxelRGBA>::selfApply( UnaryFromConstantBinaryFunctor<VoxelRGBA,
                                   std::minus<VoxelRGBA> >,
                                   VolumeRef<VoxelRGBA> & );
  extern template VolumeRef<VoxelRGBA>
  VolumeUtil<VoxelRGBA>::apply( UnaryFromConstantBinaryFunctor2<VoxelRGBA,
                               std::minus<VoxelRGBA> >,
                               const VolumeRef<VoxelRGBA> & );
#endif

}

#endif

