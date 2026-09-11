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
    VolumeRef<T>	res( new Volume<T>( o->getSize() ) );
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


  // matrix product
  template <typename T>
  VolumeRef<T> matrix_product( const Volume<T> & v1, const Volume<T> & v2 )
  {
    std::vector<int> size1 = v1.getSize();
    std::vector<int> size2 = v2.getSize();
    if( size1[1] != size2[0] )
      throw std::runtime_error( "matrix dimensions do not match" );

    VolumeRef<T> prod( size1[0], size2[1], 1, 1,
                       std::max( v1.getBorders()[0], v2.getBorders()[0] ) );
    for( long y = 0; y < size2[1]; y++ )
      for( long x = 0; x < size1[0]; x++ )
      {
        prod->at( x, y ) = T( 0 );
        for( long k = 0; k < size1[1]; k++ )
          prod->at( x, y ) += v1.at( x, k ) * v2.at( k, y );
      }

    return prod;
  }


  // matrix product
  template <typename T>
  VolumeRef<T> matrix_product( const rc_ptr<Volume<T> > & v1,
                               const rc_ptr<Volume<T> > & v2 )
  {
    return matrix_product( *v1, *v2 );
  }


  // transpose
  template <typename T>
  VolumeRef<T> transpose( const Volume<T> & v )
  {
    std::vector<int> size1 = v.getSize();
    int s0 = size1[0];
    size1[0] = size1[1];
    size1[1] = s0;

    VolumeRef<T> trans( size1,
                        std::max( v.getBorders()[1], v.getBorders()[0] ) );

    const_line_NDIterator<T> it( &v.at( 0 ), v.getSize(), v.getStrides() );
    const T *op, *pp;
    T *rp;
    long inc = &trans->at(0, 1) - &trans->at(0);

    for( ; !it.ended(); ++it )
    {
      op = &*it;
      std::vector<int> pos = it.position();
      int p0 = pos[0];
      pos[0] = pos[1];
      pos[1] = p0;
      rp = &trans->at( pos );

      for( pp=op + it.line_length(); op!=pp;
           it.inc_line_ptr( op ), rp += inc )
        *rp = *op;
    }

    return trans;
  }


  // transpose
  template <typename T>
  VolumeRef<T> transpose( const rc_ptr<Volume<T> > & v, bool copy )
  {
    if( copy )
      return transpose( *v );
    else
    {
      std::vector<int> size1 = v->getSize();
      int s0 = size1[0];
      size1[0] = size1[1];
      size1[1] = s0;

      std::vector<long> strides = v->getStrides();
      size_t st0 = strides[0];
      strides[0] = strides[1];
      strides[1] = st0;

      VolumeRef<T> trans( new Volume<T>( v,
                                         std::vector<int>( size1.size(), 0 ),
                                         size1, &v->at(0),
                                         strides ) );
      return trans;
    }
  }
}

#endif

