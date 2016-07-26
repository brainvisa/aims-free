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

#ifndef CARTODATA_VOLUME_VOLUMEBASE_D_H
#define CARTODATA_VOLUME_VOLUMEBASE_D_H

//--- cartodata --------------------------------------------------------------
#include <cartodata/volume/volumebase.h>
#include <cartodata/volume/volumebase_d_operators.h>
#include <cartodata/volume/volumebase_d_inline.h>
#include <cartodata/volume/volumebase_d_instantiate.h>
#include <cartodata/volume/volumeref.h>
#include <cartodata/volume/volumeproxy_d.h>
//--- cartobase --------------------------------------------------------------
#include <cartobase/type/limits.h>
#include <cartobase/type/string_conversion.h>
#include <cartobase/object/object.h>
#include <cartobase/object/property.h>
#include <cartobase/object/propertyfilter.h>
//--- soma-io ----------------------------------------------------------------
#include <soma-io/allocator/allocator.h>
//--- std --------------------------------------------------------------------
#include <cstdlib>
#include <algorithm>
#include <stdexcept>
#include <string>
#include <stdio.h>
//----------------------------------------------------------------------------

namespace carto
{
//============================================================================
//   CONSTRUCTORS
//============================================================================

  /***************************************************************************
   * Default Constructor
   **************************************************************************/
  template < typename T >
  Volume< T >::Volume( int sizeX, int sizeY, int sizeZ, int sizeT,
                       const AllocatorContext& allocatorContext,
                       bool allocated )
    : VolumeProxy< T >( sizeX, sizeY, sizeZ, sizeT ),
      _items( 0U, allocatorContext ),
#ifndef CARTO_USE_BLITZ
      _lineoffset( 0 ),
      _sliceoffset( 0 ),
      _volumeoffset( 0 ),
#endif
      _pos( 4, 0 )
  {
    allocate( -1, -1, -1, -1, allocated, allocatorContext );
  }

  template < typename T >
  Volume< T >::Volume( const Position4Di & size,
                       const AllocatorContext& allocatorContext,
                       bool allocated ):
    VolumeProxy< T >( size[0] > 0 ? size[0] : 1,
                      size[1] > 0 ? size[1] : 1,
                      size[2] > 0 ? size[2] : 1,
                      size[3] > 0 ? size[3] : 1 ),
    _items( 0U, allocatorContext ),
#ifndef CARTO_USE_BLITZ
    _lineoffset( 0 ),
    _sliceoffset( 0 ),
    _volumeoffset( 0 ),
#endif
    _pos( 4, 0 )
  {
    allocate( -1, -1, -1, -1, allocated, allocatorContext );
  }

  /***************************************************************************
   * Constructor with border
   **************************************************************************/

  template < typename T >
  void Volume< T >::constructBorders( const Position & bordersize,
                                      const AllocatorContext& allocatorContext,
                                      bool allocated )
  {
    if( !bordersize.empty()
      && bordersize != Position4Di().toVector() )
    {
      int i, n = VolumeProxy<T>::_size.size();
      size_t bsize = sizeof(T);
      std::vector<int> large_size( n );
      for( i=0; i<n; ++i )
      {
        large_size[i] = VolumeProxy<T>::_size[i];
        if( i < bordersize.size() )
          large_size[i] += bordersize[i] * 2;
        bsize *= VolumeProxy<T>::_size[i];
      }
      _refvol.reset( new Volume<T>( large_size,
                                    allocatorContext, allocated ) );

      allocate( -1, -1, -1, -1, true,
        _refvol->allocatorContext().isAllocated()
          ? AllocatorContext( AllocatorStrategy::NotOwner,
                              rc_ptr<DataSource>( new BufferDataSource
                                ( (char *) &(*_refvol)( bordersize ),
                                  bsize ) ) )
          : allocatorContext );
      if( _refvol->allocatorContext().isAllocated() )
      {
        // fix offsets
#ifdef CARTO_USE_BLITZ
        blitz::TinyVector<int, Volume<T>::DIM_MAX> dims;
        int i, n=VolumeProxy<T>::_size.size();
        for( int i=0; i<n; ++i )
          dims[i] = VolumeProxy<T>::_size[i];
        for( ; i<Volume<T>::DIM_MAX; ++i )
          dims[i] = 1;
        _blitz.reference
          ( blitz::Array<T,8>
            ( &_items[0],
              dims,
              _refvol->_blitz.stride(),
              blitz::GeneralArrayStorage<8>
              ( blitz::shape( 0, 1, 2, 3, 4, 5, 6, 7 ), true ) ) );
#else
        _lineoffset = &(*_refvol)( 0, 1, 0 ) - &(*_refvol)( 0, 0, 0 );
        _sliceoffset = &(*_refvol)( 0, 0, 1 ) - &(*_refvol)( 0, 0, 0 );
        _volumeoffset = &(*_refvol)( 0, 0, 0, 1 ) - &(*_refvol)( 0, 0, 0 );
#endif
      }
    }
    else // no border
    {
      allocate( -1, -1, -1, -1, allocated, allocatorContext );
    }
  }

  template < typename T >
  Volume< T >::Volume( int sizeX, int sizeY, int sizeZ, int sizeT,
                       const Position4Di & bordersize,
                       const AllocatorContext& allocatorContext,
                       bool allocated )
    : VolumeProxy< T >( sizeX, sizeY, sizeZ, sizeT ),
      _items( 0U, AllocatorContext( AllocatorStrategy::NotOwner,
        DataSource::none() ) ),
#ifndef CARTO_USE_BLITZ
      _lineoffset( 0 ),
      _sliceoffset( 0 ),
      _volumeoffset( 0 ),
#endif
      _pos( bordersize.toVector() )
  {
    constructBorders( bordersize.toVector(), allocatorContext, allocated );
  }

  template < typename T >
  Volume< T >::Volume( const Position4Di & size,
                       const Position4Di & bordersize,
                       const AllocatorContext& allocatorContext,
                       bool allocated ):
    VolumeProxy< T >( size[0] > 0 ? size[0] : 1,
                      size[1] > 0 ? size[1] : 1,
                      size[2] > 0 ? size[2] : 1,
                      size[3] > 0 ? size[3] : 1 ),
    _items( 0U, AllocatorContext( AllocatorStrategy::NotOwner,
            DataSource::none() ) ),
#ifndef CARTO_USE_BLITZ
    _lineoffset( 0 ),
    _sliceoffset( 0 ),
    _volumeoffset( 0 ),
#endif
    _pos( bordersize.toVector() )
  {
    constructBorders( bordersize.toVector(), allocatorContext, allocated );
  }

  template < typename T >
  Volume< T >::Volume( int sizeX, int sizeY, int sizeZ,
                       int sizeT, int bordersize,
                       const AllocatorContext& allocatorContext,
                       bool allocated )
    : VolumeProxy< T >( sizeX, sizeY, sizeZ, sizeT ),
      _items( 0U, AllocatorContext( AllocatorStrategy::NotOwner,
        DataSource::none() ) ),
#ifndef CARTO_USE_BLITZ
      _lineoffset( 0 ),
      _sliceoffset( 0 ),
      _volumeoffset( 0 ),
#endif
      _pos( 4, 0 )
  {
    _pos[0] = bordersize;
    _pos[1] = bordersize;
    _pos[2] = bordersize;

    constructBorders(
      Position4Di( bordersize, bordersize, bordersize, 0 ).toVector(),
      allocatorContext, allocated );
  }

  template < typename T >
  Volume< T >::Volume( const Position4Di & size,
                       int bordersize,
                       const AllocatorContext& allocatorContext,
                       bool allocated ):
    VolumeProxy< T >( size[0] > 0 ? size[0] : 1,
                      size[1] > 0 ? size[1] : 1,
                      size[2] > 0 ? size[2] : 1,
                      size[3] > 0 ? size[3] : 1 ),
    _items( 0U, AllocatorContext( AllocatorStrategy::NotOwner,
            DataSource::none() ) ),
#ifndef CARTO_USE_BLITZ
    _lineoffset( 0 ),
    _sliceoffset( 0 ),
    _volumeoffset( 0 ),
#endif
    _pos( 4, 0 )
  {
    _pos[0] = bordersize;
    _pos[1] = bordersize;
    _pos[2] = bordersize;
    constructBorders(
      Position4Di( bordersize, bordersize, bordersize, 0 ).toVector(),
      allocatorContext, allocated );
  }


  template < typename T >
  Volume< T >::Volume( const std::vector<int> & size,
                       const AllocatorContext& allocatorContext,
                       bool allocated ):
    VolumeProxy< T >( size ),
    _items( 0U, allocatorContext ),
#ifndef CARTO_USE_BLITZ
    _lineoffset( 0 ),
    _sliceoffset( 0 ),
    _volumeoffset( 0 ),
#endif
    _pos( 4, 0 )
  {
    allocate( std::vector<int>( 1, -1 ), allocated, allocatorContext );
  }


  template < typename T >
  Volume< T >::Volume( const std::vector<int> & size,
                       const std::vector<int> & bordersize,
                       const AllocatorContext& allocatorContext,
                       bool allocated ):
    VolumeProxy< T >( size ),
    _items( 0U, AllocatorContext( AllocatorStrategy::NotOwner,
            DataSource::none() ) ),
#ifndef CARTO_USE_BLITZ
    _lineoffset( 0 ),
    _sliceoffset( 0 ),
    _volumeoffset( 0 ),
#endif
    _pos( bordersize )
  {
    while( _pos.size() < 4 )
      _pos.push_back( 0 );
    constructBorders( bordersize, allocatorContext, allocated );
  }

  /***************************************************************************
   * Buffer Constructor
   **************************************************************************/
  template < typename T >
  Volume< T >::Volume( int sizeX, int sizeY, int sizeZ, int sizeT, T* buffer )
    : VolumeProxy< T >( sizeX, sizeY, sizeZ, sizeT ),
      _items( sizeX * sizeY * sizeZ * sizeT, buffer ),
#ifndef CARTO_USE_BLITZ
      _lineoffset( 0 ),
      _sliceoffset( 0 ),
      _volumeoffset( 0 ),
#endif
      _pos( 4, 0 )
  {
    allocate( -1, -1, -1, -1, true, allocatorContext() );
  }


  template < typename T >
  Volume< T >::Volume( const Position4Di & size, T* buffer ):
    VolumeProxy< T >( size[0] > 0 ? size[0] : 1,
                      size[1] > 0 ? size[1] : 1,
                      size[2] > 0 ? size[2] : 1,
                      size[3] > 0 ? size[3] : 1 ),
    _items( (long)(size[0] > 0 ? size[0] : 1) *
            (long)(size[1] > 0 ? size[1] : 1) *
            (long)(size[2] > 0 ? size[2] : 1) *
            (long)(size[3] > 0 ? size[3] : 1), buffer ),
#ifndef CARTO_USE_BLITZ
    _lineoffset( 0 ),
    _sliceoffset( 0 ),
    _volumeoffset( 0 ),
#endif
    _pos( 4, 0 )
  {
    allocate( -1, -1, -1, -1, true, allocatorContext() );
  }


  template < typename T >
  Volume< T >::Volume( const std::vector<int> & size, T* buffer ):
    VolumeProxy< T >( size ),
    _items( (long) Position4Di::size_num_elements( size ), buffer ),
#ifndef CARTO_USE_BLITZ
    _lineoffset( 0 ),
    _sliceoffset( 0 ),
    _volumeoffset( 0 ),
#endif
    _pos( 4, 0 )
  {
    allocate( std::vector<int>( 1, -1 ), true, allocatorContext() );
  }


  /***************************************************************************
   * View Constructor
   **************************************************************************/
  template<typename T> inline
  Volume<T>::Volume( rc_ptr<Volume<T> > other,
                     const Position4Di & pos, const Position4Di & size,
                     const AllocatorContext & allocContext )
    : VolumeProxy<T>( size[0] >= 0 ? size[0] :
          other->allocatorContext().isAllocated() ? other->getSizeX() : 1,
        size[1] >= 0 ? size[1] :
          other->allocatorContext().isAllocated() ? other->getSizeY() : 1,
        size[2] >= 0 ? size[2] :
          other->allocatorContext().isAllocated() ? other->getSizeZ() : 1,
        size[3] >= 0 ? size[3] :
          other->allocatorContext().isAllocated() ? other->getSizeT() : 1 ),
      _items( 0U, allocContext ),
      _refvol( other ),
#ifndef CARTO_USE_BLITZ
      _lineoffset( 0 ),
      _sliceoffset( 0 ),
      _volumeoffset( 0 ),
#endif
      _pos( pos.toVector() )
  {
    if( other->allocatorContext().isAllocated() )
    {
      size_t bsize = sizeof(T);
      int i, n = size.size();
      for( i=0; i<n; ++i )
        bsize *= size[i];

      allocate( -1, -1, -1, -1, true,
                AllocatorContext( AllocatorStrategy::NotOwner,
                                  rc_ptr<DataSource>( new BufferDataSource
                                    ( (char *) &(*other)( pos.toVector() ),
                                      bsize ) ) ) );

      // fix offsets
#ifdef CARTO_USE_BLITZ
      blitz::TinyVector<int, Volume<T>::DIM_MAX> dims;
      n = VolumeProxy<T>::_size.size();
      for( int i=0; i<n; ++i )
        dims[i] = VolumeProxy<T>::_size[i];
      for( ; i<Volume<T>::DIM_MAX; ++i )
        dims[i] = 1;
      _blitz.reference
        ( blitz::Array<T,Volume<T>::DIM_MAX>
          ( &_items[0],
            dims,
            other->_blitz.stride(),
            blitz::GeneralArrayStorage<Volume<T>::DIM_MAX>
            ( blitz::shape( 0, 1, 2, 3, 4, 5, 6, 7 ), true ) ) );
#else
      _lineoffset = &(*other)( 0, 1, 0 ) - &(*other)( 0, 0, 0 );
      _sliceoffset = &(*other)( 0, 0, 1 ) - &(*other)( 0, 0, 0 );
      _volumeoffset = &(*other)( 0, 0, 0, 1 ) - &(*other)( 0, 0, 0 );
#endif
    }
    else
      allocate( -1, -1, -1, -1, true, allocContext );

    /* copy voxel_size from underlying volume, if any.
       This should probably be more general, but cannot be applied to all
       header properties (size, transformations...).
       WARNING: Moreover here we do not guarantee to keep both voxel_size
       unique: we point to the same vector of values for now, but it can be
       replaced (thus, duplicated) by a setProperty().
       We could use a addBuiltinProperty(), but then the voxel size has to be
       stored in a fixed location somewhere.
    */
    try
    {
      carto::Object vs = other->header().getProperty( "voxel_size" );
      this->header().setProperty( "voxel_size", vs );
    }
    catch( ... )
    {
      // never mind.
    }
  }


  template<typename T> inline
  Volume<T>::Volume( rc_ptr<Volume<T> > other,
                     const Position & pos, const Position & size,
                     const AllocatorContext & allocContext )
    : VolumeProxy<T>( Position4Di::fixed_size( size ) ),
      _items( 0U, allocContext ),
      _refvol( other ),
#ifndef CARTO_USE_BLITZ
      _lineoffset( 0 ),
      _sliceoffset( 0 ),
      _volumeoffset( 0 ),
#endif
      _pos( Position4Di::fixed_position( pos ) )
  {
    if( other->allocatorContext().isAllocated() )
    {
      size_t bsize = sizeof(T);
      int i, n = size.size();
      for( i=0; i<n; ++i )
        bsize *= size[i];

      allocate( -1, -1, -1, -1, true,
                AllocatorContext( AllocatorStrategy::NotOwner,
                                  rc_ptr<DataSource>( new BufferDataSource
                                    ( (char *) &(*other)( pos ),
                                      bsize ) ) ) );

      // fix offsets
#ifdef CARTO_USE_BLITZ
      blitz::TinyVector<int, Volume<T>::DIM_MAX> dims;
      n = VolumeProxy<T>::_size.size();
      for( int i=0; i<n; ++i )
        dims[i] = VolumeProxy<T>::_size[i];
      for( ; i<Volume<T>::DIM_MAX; ++i )
        dims[i] = 1;
      _blitz.reference
        ( blitz::Array<T,Volume<T>::DIM_MAX>
          ( &_items[0],
            dims,
            other->_blitz.stride(),
            blitz::GeneralArrayStorage<Volume<T>::DIM_MAX>
            ( blitz::shape( 0, 1, 2, 3, 4, 5, 6, 7 ), true ) ) );
#else
      _lineoffset = &(*other)( 0, 1, 0 ) - &(*other)( 0, 0, 0 );
      _sliceoffset = &(*other)( 0, 0, 1 ) - &(*other)( 0, 0, 0 );
      _volumeoffset = &(*other)( 0, 0, 0, 1 ) - &(*other)( 0, 0, 0 );
#endif
    }
    else
      allocate( -1, -1, -1, -1, true, allocContext );

    /* copy voxel_size from underlying volume, if any.
       This should probably be more general, but cannot be applied to all
       header properties (size, transformations...).
       WARNING: Moreover here we do not guarantee to keep both voxel_size
       unique: we point to the same vector of values for now, but it can be
       replaced (thus, duplicated) by a setProperty().
       We could use a addBuiltinProperty(), but then the voxel size has to be
       stored in a fixed location somewhere.
    */
    try
    {
      carto::Object vs = other->header().getProperty( "voxel_size" );
      this->header().setProperty( "voxel_size", vs );
    }
    catch( ... )
    {
      // never mind.
    }
  }


  /***************************************************************************
   * Copy Constructor
   **************************************************************************/
  template < typename T >
  Volume< T >::Volume( const Volume< T >& other )
    : RCObject(),
      VolumeProxy< T >( other ),
      _items( other._items ),
#ifdef CARTO_USE_BLITZ
      // TODO: test blitz ownership / strides
      // _blitz = other.blitz;
      _blitz( &_items[0],
              other._blitz.shape(),
              blitz::GeneralArrayStorage<8>
              ( blitz::shape( 0, 1, 2, 3, 4, 5, 6, 7 ), true ) ),
#else
      _lineoffset( other._lineoffset ),
      _sliceoffset( other._sliceoffset ),
      _volumeoffset( other._volumeoffset ),
#endif
      _refvol( other.refVolume().get() ?
        new Volume<T>( *other.refVolume() ) : 0 ),
      _pos( other.posInRefVolume() )
  {
    if( _refvol.get() ) // view case: the underlying volume is copied.
    {
      Position4Di pos = other.posInRefVolume();
      std::vector<int> oldSize(1, -1);
      allocate( oldSize, true, _refvol->allocatorContext().isAllocated()
                ? AllocatorContext( AllocatorStrategy::NotOwner,
                    rc_ptr<DataSource>( new BufferDataSource
                      ( (char *) &(*_refvol)( pos[0], pos[1], pos[2], pos[3] ),
                      VolumeProxy<T>::getSizeX() * VolumeProxy<T>::getSizeY()
                        * VolumeProxy<T>::getSizeZ()
                        * VolumeProxy<T>::getSizeT()
                        * sizeof(T) ) ) )
                : AllocatorContext( other.allocatorContext() ) );
      if( _refvol->allocatorContext().isAllocated() )
      {
        // fix offsets
#ifdef CARTO_USE_BLITZ
        blitz::TinyVector<int, Volume<T>::DIM_MAX> dims;
        int i, n=VolumeProxy<T>::_size.size();
        for( int i=0; i<n; ++i )
          dims[i] = VolumeProxy<T>::_size[i];
        for( ; i<Volume<T>::DIM_MAX; ++i )
          dims[i] = 1;
        _blitz.reference
          ( blitz::Array<T,Volume<T>::DIM_MAX>
            ( &_items[0],
              dims,
              other._blitz.stride(),
              blitz::GeneralArrayStorage<Volume<T>::DIM_MAX>
              ( blitz::shape( 0, 1, 2, 3, 5, 6, 7, 8 ), true ) ) );
#else
        _lineoffset = &other( 0, 1, 0 ) - &other( 0, 0, 0 );
        _sliceoffset = &other( 0, 0, 1 ) - &other( 0, 0, 0 );
        _volumeoffset = &other( 0, 0, 0, 1 ) - &other( 0, 0, 0 );
#endif
      }
    }
  }

  template < typename T >
  Volume< T >::~Volume()
  {
  }

//============================================================================
//   M E T H O D S
//============================================================================

  template < typename T > inline
  const AllocatorContext& Volume<T>::allocatorContext() const
  {
    return _items.allocatorContext();
  }

  template <typename T> inline
  rc_ptr<Volume<T> > Volume<T>::refVolume() const
  {
    return _refvol;
  }

  template <typename T> inline
  const typename Volume<T>::Position & Volume<T>::posInRefVolume() const
  {
    return _pos;
  }

  template <typename T> inline
  void Volume<T>::updateItemsBuffer()
  {

    if ( !allocatorContext().isAllocated()
         || (allocatorContext().accessMode() == AllocatorStrategy::NotOwner) )
    {

      // Free old buffer
      _items.free();

      if (_refvol.get())
      {
        // Recreate items buffer that reference volume
        // using correct sizes and position
        if( _refvol->allocatorContext().isAllocated() )
        {
          size_t size = sizeof(T);
          int i, n = VolumeProxy<T>::_size.size();
          for( i=0; i<n; ++i )
            size *= VolumeProxy<T>::_size[i];
          _items = AllocatedVector<T>(
            0U,
            AllocatorContext( AllocatorStrategy::NotOwner,
                              rc_ptr<DataSource>( new BufferDataSource
                                ( (char *) &(*(_refvol))( _pos ),
                                  size ) ) ) );
        }
        else
          _items = AllocatedVector<T>( 0U, allocatorContext() );

        if ( _refvol->allocatorContext().isAllocated() )
        {
          // fix offsets
#ifdef CARTO_USE_BLITZ
          blitz::TinyVector<int, Volume<T>::DIM_MAX> dims;
          int i, n=VolumeProxy<T>::_size.size();
          for( int i=0; i<n; ++i )
            dims[i] = VolumeProxy<T>::_size[i];
          for( ; i<Volume<T>::DIM_MAX; ++i )
            dims[i] = 1;
          _blitz.reference
            ( blitz::Array<T,Volume<T>::DIM_MAX>
              ( &_items[0],
                dims,
                _refvol->_blitz.stride(),
                blitz::GeneralArrayStorage<Volume<T>::DIM_MAX>
                ( blitz::shape( 0, 1, 2, 3, 4, 5, 6, 7 ), true ) ) );
#else
          _lineoffset = &(*_refvol)( 0, 1, 0 ) - &(*_refvol)( 0, 0, 0 );
          _sliceoffset = &(*_refvol)( 0, 0, 1 ) - &(*_refvol)( 0, 0, 0 );
          _volumeoffset = &(*_refvol)( 0, 0, 0, 1 ) - &(*_refvol)( 0, 0, 0 );
#endif
        }

        /* copy voxel_size from underlying volume, if any.
          This should probably be more general, but cannot be applied to all
          header properties (size, transformations...).
          WARNING: Moreover here we do not guarantee to keep both voxel_size
          unique: we point to the same vector of values for now, but it can be
          replaced (thus, duplicated) by a setProperty().
          We could use a addBuiltinProperty(), but then the voxel size has to be
          stored in a fixed location somewhere.
        */
        try
        {
          carto::Object vs = _refvol->header().getProperty( "voxel_size" );
          this->header().setProperty( "voxel_size", vs );
        }
        catch( ... )
        {
          // never mind.
        }
      }
    }
  }

  template <typename T> inline
  void Volume<T>::setPosInRefVolume( const Position4Di & pos )
  {
    if ( pos != _pos )
    {
      _pos = pos.toVector();
      updateItemsBuffer();
    }
  }


  template <typename T> inline
  void Volume<T>::setPosInRefVolume( const Position & pos ) {
    if (pos != _pos)
    {
      _pos = pos;
      while( _pos.size() < 4 )
        _pos.push_back( 0 );
      updateItemsBuffer();
    }
  }

  template <typename T> inline
  void Volume<T>::setRefVolume( const rc_ptr<Volume<T> > & refvol) {
    if (refvol.get() != _refvol.get()) {
      _refvol = refvol;
      updateItemsBuffer();
    }
  }

  template <typename T> inline
  std::vector<int> Volume<T>::getBorders() const
  {
    std::vector<int> borders( VolumeProxy<T>::_size.size() * 2, 0 );
    if( _refvol.get() && _refvol->allocatorContext().isAllocated() )
    {
      int i, n = VolumeProxy<T>::_size.size();
      for( i=0; i<n; ++i )
        borders[i*2 + 1] = _refvol->_size[i] - VolumeProxy<T>::_size[i];
      for( i=0, n=_pos.size(); i<n; ++i )
      {
        borders[i*2] = _pos[i];
        borders[i*2+1] -= _pos[i];
      }
    }

    return borders;
  }

  template <typename T> inline
  std::vector<size_t> Volume<T>::getStrides() const
  {

#ifdef CARTO_USE_BLITZ
    const blitz::TinyVector<int, Volume<T>::DIM_MAX>& bstrides = _blitz.stride();
    int d, n = VolumeProxy<T>::_size.size();
    std::vector<size_t> strides( n );
    for (int d = 0; d < n; ++d)
        strides[d] = bstrides[d];
#else
    std::vector<size_t> strides(4);

    strides[0] = 1;
    strides[1] = _lineoffset;
    strides[2] = _sliceoffset;
    strides[3] = _volumeoffset;
#endif

    return strides;
  }

  template < typename T >
  Volume< T >& Volume< T >::operator=( const Volume< T >& other )
  {
    if( &other == this )
      return *this;

    bool        b = Headered::signalsBlocked();
    if( !b )
      Headered::blockSignals( true );
    this->VolumeProxy< T >::operator=( other );
    _items = other._items;

#ifdef CARTO_USE_BLITZ
    // TODO: test blitz ownership / strides
    // _blitz.reference( other.blitz );
    blitz::TinyVector<long, Volume<T>::DIM_MAX> dims;
    int i, n = VolumeProxy<T>::_size.size();
    for( i=0; i<n; ++i )
      dims[i] = VolumeProxy<T>::_size[i];
    for( ; i<Volume<T>::DIM_MAX; ++i )
      dims[i] = 1;
    _blitz.reference
        ( blitz::Array<T,Volume<T>::DIM_MAX>
        ( &_items[0],
            dims,
            other._blitz.stride(),
            blitz::GeneralArrayStorage<Volume<T>::DIM_MAX>
            ( blitz::shape( 0, 1, 2, 3, 4, 5, 6, 7 ), true ) ) );
#else
    _lineoffset = other._lineoffset;
    _sliceoffset = other._sliceoffset;
    _volumeoffset = other._volumeoffset;
#endif
    _refvol = other._refvol;
    _pos = other._pos;

    initialize();

    if( !b )
      Headered::blockSignals( false );

    return *this;

  }

  template < typename T >
  typename Volume< T >::iterator Volume< T >::begin()
  {

#ifdef CARTO_USE_BLITZ
    return _blitz.begin();
#else
    return _items.begin();
#endif

  }


  template < typename T >
  typename Volume< T >::iterator Volume< T >::end()
  {

#ifdef CARTO_USE_BLITZ
    return _blitz.end();
#else
    return _items.end();
#endif

  }


  template < typename T >
  typename Volume< T >::const_iterator Volume< T >::begin() const
  {

#ifdef CARTO_USE_BLITZ
    return _blitz.begin();
#else
    return _items.begin();
#endif

  }


  template < typename T >
  typename Volume< T >::const_iterator Volume< T >::end() const
  {

#ifdef CARTO_USE_BLITZ
    return _blitz.end();
#else
    return _items.end();
#endif

  }


  template < typename T >
  void Volume< T >::initialize()
  {

    // initializing headered stuff
    this->Headered::initialize();

    // creating size filter
    std::set< std::string > sizePropertyNames;
    sizePropertyNames.insert( "sizeX" );
    sizePropertyNames.insert( "sizeY" );
    sizePropertyNames.insert( "sizeZ" );
    sizePropertyNames.insert( "sizeT" );
    rc_ptr< PropertyFilter >
      sizeRcPropertyFilter( new PropertyFilter( "size", sizePropertyNames ) );

    // adding size filter to headered and connecting signal to slot
    Headered::addPropertyFilter( sizeRcPropertyFilter );
    Headered::connect( sizeRcPropertyFilter->getName(),
                       ::sigc::mem_fun( *this, &Volume< T >::slotSizeChanged ) );

  }


  template < typename T >
  void Volume< T >::allocate( int oldSizeX,
                              int oldSizeY,
                              int oldSizeZ,
                              int oldSizeT,
                              bool allocate,
                              const AllocatorContext& ac )
  {
    std::vector<int> oldSize(4);
    oldSize[0] = oldSizeX;
    oldSize[1] = oldSizeY;
    oldSize[2] = oldSizeZ;
    oldSize[3] = oldSizeT;
    Volume< T >::allocate( oldSize, allocate, ac );
  }


  template < typename T >
  void Volume< T >::allocate( const std::vector<int> & oldSize,
                              bool allocate,
                              const AllocatorContext& ac )
  {

    std::vector<unsigned long long int> strides(Volume<T>::DIM_MAX, 0);
    int i, n = oldSize.size(), nn = VolumeProxy<T>::_size.size();

    for( i=0; i<nn; ++i )
    {
      strides[i] = ( i == 0 ? VolumeProxy<T>::_size[0]
                    : VolumeProxy<T>::_size[i] * strides[i-1] );
    }
    for( ; i<Volume<T>::DIM_MAX; ++i )
      strides[i] = strides[i-1];

    unsigned long long int total_len = strides[nn-1];

    if ( total_len * sizeof(T) >
         (unsigned long long int) std::numeric_limits< size_t >::max() )
      {

        throw std::runtime_error
          ( std::string( "attempt to allocate a volume which size is greater "
                         "than allowed by the system (" )
            + toString( std::numeric_limits< size_t >::max() ) + " bytes)" );

      }

    bool no_old = true;
    for( i=0; i<n; ++i )
      if( oldSize[i] != -1 )
      {
        no_old = false;
        break;
      }

    if ( !allocate // why !allocate ?
          || !_items.allocatorContext().isAllocated()
          || no_old )
    {
      // allocating memory space
      _items.free();
      if( allocate )
        _items.allocate( ( size_t ) total_len, ac );
    }
    else if ( oldSize != VolumeProxy<T>::_size
              || &ac != &_items.allocatorContext() )
      {

        // allocating a new memory space
        AllocatedVector<T> newItems( ( size_t ) total_len, ac );

        std::vector<int> minSize = VolumeProxy<T>::_size;
        for( i=0; i<std::min(n, nn); ++i )
          if( oldSize[i] < minSize[i] )
            minSize[i] = oldSize[i];

        // preserving data
        int x, y, z, t;
        if( newItems.allocatorContext().allocatorType()
            != AllocatorStrategy::ReadOnlyMap )
          for ( t = 0; t < minSize[3]; t++ )
            {

              for ( z = 0; z < minSize[2]; z++ )
                {

                  for ( y = 0; y < minSize[1]; y++ )
                    {

                      for ( x = 0; x < minSize[0]; x++ )
                        {
                          newItems[ x +
                                    y * strides[0] +
                                    z * ( size_t ) strides[1] +
                                    t * ( size_t ) strides[2] ] =
                            _items[ x +
                                    y * oldSize[0] +
                                    z * ( size_t ) oldSize[0]
                                    * ( size_t ) oldSize[1] +
                                    t * ( size_t ) oldSize[0] *
                                    ( size_t ) oldSize[1]
                                    * ( size_t ) oldSize[2] ];

                        }

                    }

                }

          }

        // copying new data to old one
        _items = newItems;

      }

    if( allocate )
      {
#ifdef CARTO_USE_BLITZ
        // TODO: test blitz ownership / strides
        /*
        std::cout << "alloc blitz: " << VolumeProxy<T>::_size[0] << ", "
                  << VolumeProxy<T>::_size[1] << ", "
                  << VolumeProxy<T>::_size[2] << ", "
                  << VolumeProxy<T>::_size[3] << std::endl;
        */
        blitz::TinyVector<int, Volume<T>::DIM_MAX> dims;
        for( i=0; i<nn; ++i )
          dims[i] = VolumeProxy<T>::_size[i];
        for( ; i<Volume<T>::DIM_MAX; ++i )
          dims[i] = 1;
        _blitz.reference( blitz::Array<T,Volume<T>::DIM_MAX>
                          ( &_items[0],
                            dims,
                            blitz::GeneralArrayStorage<Volume<T>::DIM_MAX>
                            ( blitz::shape( 0, 1, 2, 3, 4, 5, 6, 7 ),
                              true ) ) );
        /*
        std::cout << &_items[0] << " / " << &_blitz( 0 ) << std::endl;
        std::cout << blitz::shape( VolumeProxy<T>::_size[0],
                                   VolumeProxy<T>::_size[1],
                                   VolumeProxy<T>::_size[2],
                                   VolumeProxy<T>::_size[3}] ) << std::endl;
        std::cout << "blitz data: " << _blitz.data() << std::endl;
        std::cout << "blitz ordering: " << _blitz.ordering() << std::endl;
        std::cout << "blitz numEl: " << _blitz.numElements() << std::endl;
        std::cout << "blitz strides: " << _blitz.stride() << std::endl;
        */
#else
        _lineoffset = VolumeProxy<T>::_size[0];
        _sliceoffset = sizeXY;
        _volumeoffset = sizeXYZ;
#endif
      }
    else
      {
#ifdef CARTO_USE_BLITZ
        blitz::TinyVector<int, Volume<T>::DIM_MAX> dims;
        for( i=0; i<nn; ++i )
          dims[i] = VolumeProxy<T>::_size[i];
        for( ; i<Volume<T>::DIM_MAX; ++i )
          dims[i] = 1;
        // TODO: test blitz ownership / strides
        _blitz.reference( blitz::Array<T,Volume<T>::DIM_MAX>
                      ( 0,
                        dims,
                        blitz::GeneralArrayStorage<Volume<T>::DIM_MAX>
                        ( blitz::shape( 0, 1, 2, 3, 4, 5, 6, 7 ), true ) ) );
#else
        _lineoffset = 0;
        _sliceoffset = 0;
        _volumeoffset = 0;
#endif
      }

  }


  template < typename T >
  void Volume< T >::allocate()
  {
    if( !allocatorContext().isAllocated() )
      allocate( VolumeProxy<T>::getSize(), true, allocatorContext() );
  }


  template < typename T >
  void Volume< T >::slotSizeChanged( const PropertyFilter& propertyFilter )
  {

    std::cout << "Volume< " << DataTypeCode<T>::name()
              << " >::slotSizeChanged"
              << std::endl;

    std::vector<int> oldSize = VolumeProxy<T>::_size;

    if ( propertyFilter.hasOldValue( "sizeX" ) )
      {

        oldSize[0] =
          propertyFilter.getOldValue( "sizeX" )->GenericObject::value< int >();

      }
    if ( propertyFilter.hasOldValue( "sizeY" ) )
      {

        oldSize[1] =
          propertyFilter.getOldValue( "sizeY" )->GenericObject::value< int >();

      }
    if ( propertyFilter.hasOldValue( "sizeZ" ) )
      {

        oldSize[2] =
          propertyFilter.getOldValue( "sizeZ" )->GenericObject::value< int >();

      }
    if ( propertyFilter.hasOldValue( "sizeT" ) )
      {

        oldSize[3] =
          propertyFilter.getOldValue( "sizeT" )->GenericObject::value< int >();

      }
    std::cout << "old size: " << oldSize[0] << ", " << oldSize[1] << ", "
              << oldSize[2] << ", " << oldSize[3] << std::endl;
    std::cout << "new size: " << VolumeProxy<T>::_size[0] << ", "
              << VolumeProxy<T>::_size[1] << ", "
              << VolumeProxy<T>::_size[2] << ", " << VolumeProxy<T>::_size[3]
              << std::endl;
    allocate( oldSize,
              _items.allocatorContext().isAllocated(), allocatorContext() );

  }


  template < typename T >
  void Volume< T >::reallocate( int sizeX,
                                int sizeY,
                                int sizeZ,
                                int sizeT,
                                bool keepcontents,
                                const AllocatorContext & ac,
                                bool alloc )
  {
    int oldx = VolumeProxy<T>::_size[0];
    int oldy = VolumeProxy<T>::_size[1];
    int oldz = VolumeProxy<T>::_size[2];
    int oldt = VolumeProxy<T>::_size[3];
    VolumeProxy<T>::_size[0] = sizeX;
    VolumeProxy<T>::_size[1] = sizeY;
    VolumeProxy<T>::_size[2] = sizeZ;
    VolumeProxy<T>::_size[3] = sizeT;
    if( keepcontents || ( sizeX == oldx && sizeY == oldy && sizeZ == oldz
                          && sizeT ==  oldt ) )
      allocate( oldx, oldy, oldz, oldt, alloc, ac );
    else
      allocate( std::vector<int>(1, -1), alloc, ac );
    // emit a signal ?
  }

  template < typename T >
  void Volume< T >::reallocate( const Position4Di & size,
                                bool keepcontents,
                                const AllocatorContext & ac,
                                bool alloc )
  {
    return reallocate( size[0] > 0 ? size[0] : 1,
                       size[1] > 0 ? size[1] : 1,
                       size[2] > 0 ? size[2] : 1,
                       size[3] > 0 ? size[3] : 1,
                       keepcontents, ac, alloc );
  }

  template < typename T >
  void Volume< T >::reallocate( const std::vector<int> & size,
                                bool keepcontents,
                                const AllocatorContext & ac,
                                bool alloc )
  {
    std::vector<int> old = VolumeProxy<T>::_size;

    VolumeProxy<T>::header().removeProperty( "sizeX" );
    VolumeProxy<T>::header().removeProperty( "sizeY" );
    VolumeProxy<T>::header().removeProperty( "sizeZ" );
    VolumeProxy<T>::header().removeProperty( "sizeT" );

    VolumeProxy<T>::_size = size;
    while( VolumeProxy<T>::_size.size() < 4 )
      VolumeProxy<T>::_size.push_back( 1 );

    VolumeProxy<T>::header().addBuiltinProperty( "sizeX",
                                                 VolumeProxy<T>::_size[0] );
    VolumeProxy<T>::header().addBuiltinProperty( "sizeY",
                                                 VolumeProxy<T>::_size[1] );
    VolumeProxy<T>::header().addBuiltinProperty( "sizeZ",
                                                 VolumeProxy<T>::_size[2] );
    VolumeProxy<T>::header().addBuiltinProperty( "sizeT",
                                                 VolumeProxy<T>::_size[3] );

    if( keepcontents || size == old )
      allocate( old, alloc, ac );
    else
      allocate( std::vector<int>(1, -1), alloc, ac );
    // emit a signal ?
  }

//============================================================================
//   U T I L I T I E S
//============================================================================

  template <typename T>
  Volume<T>* Creator<Volume<T> >::create( Object header,
                                          const AllocatorContext & context,
                                          Object options )
  {
    int                sizex = 1, sizey = 1, sizez = 1, sizet = 1;
    bool        unalloc = false;
    header->getProperty( "sizeX", sizex );
    header->getProperty( "sizeY", sizey );
    header->getProperty( "sizeZ", sizez );
    header->getProperty( "sizeT", sizet );
    options->getProperty( "unallocated", unalloc );
    std::vector<int> borders( 3, 0 );
    try {
      borders[0] = (int) rint( options->getProperty( "border" )->getScalar() );
      borders[1] = (int) rint( options->getProperty( "border" )->getScalar() );
      borders[2] = (int) rint( options->getProperty( "border" )->getScalar() );
    } catch( ... ) {}
    try {
      borders[0] = (int) rint( options->getProperty( "bx" )->getScalar() );
    } catch( ... ) {}
    try {
      borders[1] = (int) rint( options->getProperty( "by" )->getScalar() );
    } catch( ... ) {}
    try {
      borders[2] = (int) rint( options->getProperty( "bz" )->getScalar() );
    } catch( ... ) {}
    Volume<T>        *obj;
    if( borders[0] != 0 || borders[1] != 0 || borders[2] != 0 )
    {
      obj = new Volume<T>( sizex + borders[0] * 2,
                           sizey + borders[1] * 2,
                           sizez + borders[2] * 2,
                           sizet, context, !unalloc );
      obj = new Volume<T>( rc_ptr<Volume<T> >( obj ),
                           typename Volume<T>::Position4Di( borders[0],
                              borders[1], borders[2], 0 ),
                           typename Volume<T>::Position4Di( sizex, sizey,
                              sizez, sizet ),
                           context );
    }
    else
      obj = new Volume<T>( sizex, sizey, sizez, sizet, context, !unalloc );
    obj->blockSignals( true );
    obj->header().copyProperties( header );
    // restore original sizes : temporary too...
    obj->blockSignals( false );
    return obj;
  }

  template <typename T>
  void Creator<Volume<T> >::setup( Volume<T> & obj, Object header,
                                   const AllocatorContext & context,
                                   Object options )
  {
    int                sizex = 1, sizey = 1, sizez = 1, sizet = 1;
    bool        unalloc = false, partial = false, keep_allocation = false;
    options->getProperty( "partial_reading", partial );
    if( !partial )
      {
        header->getProperty( "sizeX", sizex );
        header->getProperty( "sizeY", sizey );
        header->getProperty( "sizeZ", sizez );
        header->getProperty( "sizeT", sizet );
        options->getProperty( "unallocated", unalloc );
        options->getProperty( "keep_allocation", keep_allocation );
        if( !keep_allocation || !obj.allocatorContext().isAllocated() )
          obj.reallocate( sizex, sizey, sizez, sizet, false, context,
                          !unalloc );
      }
    else
      {
        const_cast<AllocatorContext &>( obj.allocatorContext() ).setDataSource
          ( context.dataSource() );
        // preserve dimensions
        sizex = obj.getSizeX();
        sizey = obj.getSizeY();
        sizez = obj.getSizeZ();
        sizet = obj.getSizeT();
      }
    obj.blockSignals( true );
    obj.header().copyProperties( header );
    if( partial )
      {
        // restore dimensions
        PropertySet        & ps = obj.header();
        ps.setProperty( "sizeX", sizex );
        ps.setProperty( "sizeY", sizey );
        ps.setProperty( "sizeZ", sizez );
        ps.setProperty( "sizeT", sizet );
      }
    obj.blockSignals( false );
  }

} // namespace carto

#endif // CARTODATA_VOLUME_VOLUMEBASE_D_H
