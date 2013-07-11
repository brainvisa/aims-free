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

#ifndef CARTODATA_VOLUME_VOLUME_D_H
#define CARTODATA_VOLUME_VOLUME_D_H

#include <cstdlib>
#include <cartodata/volume/volume.h>
#include <cartodata/volume/volumeproxy_d.h>
#include <cartobase/type/limits.h>
#include <cartobase/type/string_conversion.h>
#include <algorithm>
#include <stdexcept>
#include <string>
#include <stdio.h>
// DEBUG
#include <iostream>

namespace carto
{
//============================================================================
//   C O N S T R U C T O R S
//============================================================================

  /***************************************************************************
   * Default Constructor
   **************************************************************************/
  template < typename T >
  Volume< T >::Volume( int sizeX, int sizeY, int sizeZ, int sizeT,
                       const AllocatorContext& allocatorContext, 
                       bool allocated )
    : VolumeProxy< T >( sizeX, sizeY, sizeZ, sizeT ),
      _items( 0U, allocatorContext )
#ifndef CARTO_USE_BLITZ
    , 
      _lineoffset( 0 ), 
      _sliceoffset( 0 ), 
      _volumeoffset( 0 )
#endif
  {

    allocate( -1, -1, -1, -1, allocated, allocatorContext );

  }

  /***************************************************************************
   * Buffer Constructor
   **************************************************************************/
  template < typename T >
  Volume< T >::Volume( int sizeX, int sizeY, int sizeZ, int sizeT, T* buffer )
    : VolumeProxy< T >( sizeX, sizeY, sizeZ, sizeT ),
      _items( sizeX * sizeY * sizeZ * sizeT, buffer )
#ifndef CARTO_USE_BLITZ
    , 
      _lineoffset( 0 ), 
      _sliceoffset( 0 ), 
      _volumeoffset( 0 )
#endif
  {

    allocate( -1, -1, -1, -1, true, allocatorContext() );

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
      _items( 0U,
              other->allocatorContext().isAllocated()
                ? AllocatorContext( AllocatorStrategy::NotOwner,
                                    rc_ptr<DataSource>( new BufferDataSource
                                      ( (char *) &(*other)( pos[0], pos[1],
                                                            pos[2], pos[3] ),
                                        size[0] * size[1] * size[2] * size[3]
                                        * sizeof(T) ) ) )
                : allocContext ),
      _refvol( other ),
      _pos( pos )
#ifndef CARTO_USE_BLITZ
    ,
      _lineoffset( 0 ),
      _sliceoffset( 0 ),
      _volumeoffset( 0 )
#endif
  {
    allocate( -1, -1, -1, -1, true, other->allocatorContext().isAllocated()
                ? AllocatorContext( AllocatorStrategy::NotOwner,
                                    rc_ptr<DataSource>( new BufferDataSource
                                      ( (char *) &(*other)( pos[0], pos[1],
                                                            pos[2], pos[3] ),
                                        size[0] * size[1] * size[2] * size[3]
                                        * sizeof(T) ) ) )
                : allocContext );
    if( other->allocatorContext().isAllocated() )
      {
        // fix offsets
#ifdef CARTO_USE_BLITZ
        _blitz.reference
          ( blitz::Array<T,4>
            ( &_items[0],
              blitz::shape( VolumeProxy<T>::getSizeX(),
                            VolumeProxy<T>::getSizeY(),
                            VolumeProxy<T>::getSizeZ(),
                            VolumeProxy<T>::getSizeT() ),
              blitz::shape( 1, &(*other)( 0, 1, 0 ) - &(*other)( 0, 0, 0 ),
                            &(*other)( 0, 0, 1 ) - &(*other)( 0, 0, 0 ),
                            &(*other)( 0, 0, 0, 1 ) - &(*other)( 0, 0, 0 ) ),
              blitz::GeneralArrayStorage<4>
              ( blitz::shape( 0, 1, 2, 3 ), true ) ) );
#else
        _lineoffset = &(*other)( 0, 1, 0 ) - &(*other)( 0, 0, 0 );
        _sliceoffset = &(*other)( 0, 0, 1 ) - &(*other)( 0, 0, 0 );
        _volumeoffset = &(*other)( 0, 0, 0, 1 ) - &(*other)( 0, 0, 0 );
#endif
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
              blitz::shape( VolumeProxy<T>::_sizeX, 
                            VolumeProxy<T>::_sizeY, 
                            VolumeProxy<T>::_sizeZ, 
                            VolumeProxy<T>::_sizeT ), 
              blitz::GeneralArrayStorage<4>
              ( blitz::shape( 0, 1, 2, 3 ), true ) )
#else
      _lineoffset( other._lineoffset ), 
      _sliceoffset( other._sliceoffset ), 
      _volumeoffset( other._volumeoffset )
#endif
  {
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
  rc_ptr<Volume<T> > & Volume<T>::refVolume()
  {
    return _refvol;
  }

  template <typename T> inline
  typename Volume<T>::Position4Di Volume<T>::posInRefVolume() const
  {
    return _pos;
  }

  template <typename T> inline
  typename Volume<T>::Position4Di & Volume<T>::posInRefVolume()
  {
    return _pos;
  }

  template < typename T >
  Volume< T >& Volume< T >::operator=( const Volume< T >& other )
  {

    if( &other == this )
      return *this;

    bool	b = Headered::signalsBlocked();
    if( !b )
      Headered::blockSignals( true );
    this->VolumeProxy< T >::operator=( other );
    _items = other._items;
#ifdef CARTO_USE_BLITZ
    // TODO: test blitz ownership / strides
    // _blitz.reference( other.blitz );
    _blitz.reference( blitz::Array<T,4>
                      ( &_items[0], 
                        blitz::shape( VolumeProxy<T>::_sizeX, 
                                      VolumeProxy<T>::_sizeY, 
                                      VolumeProxy<T>::_sizeZ, 
                                      VolumeProxy<T>::_sizeT ), 
                        blitz::GeneralArrayStorage<4>
                        ( blitz::shape( 0, 1, 2, 3 ), true ) ) );
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
  void Volume< T >::fill( const T& value )
  {

    int	x, y, z, t, nx = VolumeProxy< T >::getSizeX(), 
      ny = VolumeProxy< T >::getSizeY(), nz = VolumeProxy< T >::getSizeZ(), 
      nt = VolumeProxy< T >::getSizeT();
    T	*p;
    for( t=0; t<nt; ++t )
      for( z=0; z<nz; ++z )
        for( y=0; y<ny; ++y )
          {
            p = &(*this)( 0, y, z, t );
            for( x=0; x<nx; ++x )
              *p++ = value;
          }

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

    unsigned long long int sizeXY =
        (unsigned long long int) VolumeProxy<T>::_sizeX
        * (unsigned long long int) VolumeProxy<T>::_sizeY;
    unsigned long long int sizeXYZ = sizeXY 
        * (unsigned long long int) VolumeProxy<T>::_sizeZ;
    unsigned long long int sizeXYZT = sizeXYZ 
        * (unsigned long long int) VolumeProxy<T>::_sizeT;

    if ( sizeXYZT * sizeof(T) > 
         (unsigned long long int) std::numeric_limits< size_t >::max() )
      {

        throw std::runtime_error
          ( std::string( "attempt to allocate a volume which size is greater " 
                         "than allowed by the system (" ) 
            + toString( std::numeric_limits< size_t >::max() ) + " bytes)" );

      }


    if ( !allocate 
	 || !_items.allocatorContext().isAllocated() 
	 || ( ( oldSizeX == -1 ) &&
	      ( oldSizeY == -1 ) &&
	      ( oldSizeZ == -1 ) &&
	      ( oldSizeT == -1 ) ) )
      {
  
        // allocating memory space
        _items.free();
	if( allocate )
	  _items.allocate( ( size_t )sizeXYZT, ac );
  
      }
    else if ( ( oldSizeX != VolumeProxy<T>::_sizeX ) ||
              ( oldSizeY != VolumeProxy<T>::_sizeY ) ||
              ( oldSizeZ != VolumeProxy<T>::_sizeZ ) ||
              ( oldSizeT != VolumeProxy<T>::_sizeT ) 
              || &ac != &_items.allocatorContext() )
      {
       
        // allocating a new memory space
        AllocatedVector<T> newItems( ( size_t )sizeXYZT, ac );

        int minSizeX = std::min( oldSizeX, VolumeProxy<T>::_sizeX );
        int minSizeY = std::min( oldSizeY, VolumeProxy<T>::_sizeY );
        int minSizeZ = std::min( oldSizeZ, VolumeProxy<T>::_sizeZ );
        int minSizeT = std::min( oldSizeT, VolumeProxy<T>::_sizeT );

        // preserving data
        int x, y, z, t;
        if( newItems.allocatorContext().allocatorType() 
            != AllocatorStrategy::ReadOnlyMap )
          for ( t = 0; t < minSizeT; t++ )
            {

              for ( z = 0; z < minSizeZ; z++ )
                {

                  for ( y = 0; y < minSizeY; y++ )
                    {

                      for ( x = 0; x < minSizeX; x++ )
                        {
                          newItems[ x +
                                    y * VolumeProxy<T>::_sizeX +
                                    z * ( size_t ) sizeXY +
                                    t * ( size_t ) sizeXYZ ] =
                            _items[ x +
                                    y * oldSizeX +
                                    z * ( size_t ) oldSizeX 
                                    * ( size_t ) oldSizeY +
                                    t * ( size_t ) oldSizeX *
                                    ( size_t ) oldSizeY 
                                    * ( size_t ) oldSizeZ ];

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
        std::cout << "alloc blitz: " << VolumeProxy<T>::_sizeX << ", " 
                  << VolumeProxy<T>::_sizeY << ", " 
                  << VolumeProxy<T>::_sizeZ << ", "
                  << VolumeProxy<T>::_sizeT << std::endl;
        */
        _blitz.reference( blitz::Array<T,4>
                          ( &_items[0], 
                            blitz::shape( VolumeProxy<T>::_sizeX, 
                                          VolumeProxy<T>::_sizeY, 
                                          VolumeProxy<T>::_sizeZ, 
                                          VolumeProxy<T>::_sizeT ), 
                            blitz::GeneralArrayStorage<4>
                            ( blitz::shape( 0, 1, 2, 3 ), true ) ) );
        /*
        std::cout << &_items[0] << " / " << &_blitz( 0 ) << std::endl;
        std::cout << blitz::shape( VolumeProxy<T>::_sizeX, 
                                   VolumeProxy<T>::_sizeY, 
                                   VolumeProxy<T>::_sizeZ, 
                                   VolumeProxy<T>::_sizeT ) << std::endl;
        std::cout << "blitz data: " << _blitz.data() << std::endl;
        std::cout << "blitz ordering: " << _blitz.ordering() << std::endl;
        std::cout << "blitz numEl: " << _blitz.numElements() << std::endl;
        std::cout << "blitz strides: " << _blitz.stride() << std::endl;
        */
#else
        _lineoffset = VolumeProxy<T>::_sizeX;
        _sliceoffset = sizeXY;
        _volumeoffset = sizeXYZ;
#endif
      }
    else
      {
#ifdef CARTO_USE_BLITZ
    // TODO: test blitz ownership / strides
    _blitz.reference( blitz::Array<T,4>
                      ( 0, 
                        blitz::shape( VolumeProxy<T>::_sizeX, 
                                      VolumeProxy<T>::_sizeY, 
                                      VolumeProxy<T>::_sizeZ, 
                                      VolumeProxy<T>::_sizeT ), 
                        blitz::GeneralArrayStorage<4>
                        ( blitz::shape( 0, 1, 2, 3 ), true ) ) );
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
      allocate( VolumeProxy<T>::getSizeX(), VolumeProxy<T>::getSizeY(), 
		VolumeProxy<T>::getSizeZ(), VolumeProxy<T>::getSizeT(), true, 
                allocatorContext() );
  }


  template < typename T >
  void Volume< T >::slotSizeChanged( const PropertyFilter& propertyFilter )
  {

    std::cout << "Volume< " << DataTypeCode<T>::name()
              << " >::slotSizeChanged"
              << std::endl;

    int oldSizeX = VolumeProxy<T>::_sizeX;
    int oldSizeY = VolumeProxy<T>::_sizeY;
    int oldSizeZ = VolumeProxy<T>::_sizeZ;
    int oldSizeT = VolumeProxy<T>::_sizeT;
  
    if ( propertyFilter.hasOldValue( "sizeX" ) )
      {
  
        oldSizeX =
          propertyFilter.getOldValue( "sizeX" )->GenericObject::value< int >();
        std::cout << "old sizex: " << oldSizeX << std::endl;
  
      }
    if ( propertyFilter.hasOldValue( "sizeY" ) )
      {
  
        oldSizeY =
          propertyFilter.getOldValue( "sizeY" )->GenericObject::value< int >();
        std::cout << "old sizey: " << oldSizeY << std::endl;
  
      }
    if ( propertyFilter.hasOldValue( "sizeZ" ) )
      {
  
        oldSizeZ =
          propertyFilter.getOldValue( "sizeZ" )->GenericObject::value< int >();
        std::cout << "old sizez: " << oldSizeZ << std::endl;
  
      }
    if ( propertyFilter.hasOldValue( "sizeT" ) )
      {
  
        oldSizeT =
          propertyFilter.getOldValue( "sizeT" )->GenericObject::value< int >();
        std::cout << "old sizet: " << oldSizeT << std::endl;
  
      }
    std::cout << "old size: " << oldSizeX << ", " << oldSizeY << ", " 
              << oldSizeZ << ", " << oldSizeT << std::endl;
    std::cout << "new size: " << VolumeProxy<T>::_sizeX << ", " 
              << VolumeProxy<T>::_sizeY << ", " 
              << VolumeProxy<T>::_sizeZ << ", " << VolumeProxy<T>::_sizeT 
              << std::endl;
    allocate( oldSizeX, oldSizeY, oldSizeZ, oldSizeT, 
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
    int oldx = VolumeProxy<T>::_sizeX;
    int	oldy = VolumeProxy<T>::_sizeY;
    int	oldz = VolumeProxy<T>::_sizeZ;
    int	oldt = VolumeProxy<T>::_sizeT;
    VolumeProxy<T>::_sizeX = sizeX;
    VolumeProxy<T>::_sizeY = sizeY;
    VolumeProxy<T>::_sizeZ = sizeZ;
    VolumeProxy<T>::_sizeT = sizeT;
    if( keepcontents || ( sizeX == oldx && sizeY == oldy && sizeZ == oldz 
                          && sizeT ==  oldt ) )
      allocate( oldx, oldy, oldz, oldt, alloc, ac );
    else
      allocate( -1, -1, -1, -1, alloc, ac );
    // emit a signal ?
  }


  // ---------------

  template <typename T>
  Volume<T>* Creator<Volume<T> >::create( Object header, 
                                          const AllocatorContext & context, 
                                          Object options )
  {
    int		sizex = 1, sizey = 1, sizez = 1, sizet = 1;
    bool	unalloc = false;
    header->getProperty( "sizeX", sizex );
    header->getProperty( "sizeY", sizey );
    header->getProperty( "sizeZ", sizez );
    header->getProperty( "sizeT", sizet );
    std::cout << "VOLUMECREATE:: ( " << sizex << ", " << sizey << ", "
              << sizez << ", " << sizet << " )" << std::endl;
    options->getProperty( "unallocated", unalloc );
    Volume<T>	*obj = new Volume<T>( sizex, sizey, sizez, sizet, context, 
                                      !unalloc );
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
    int		sizex = 1, sizey = 1, sizez = 1, sizet = 1;
    bool	unalloc = false, partial = false;
    options->getProperty( "partial_reading", partial );
    if( !partial )
      {
        header->getProperty( "sizeX", sizex );
        header->getProperty( "sizeY", sizey );
        header->getProperty( "sizeZ", sizez );
        header->getProperty( "sizeT", sizet );
        std::cout << "VOLUMESETUP:: ( " << sizex << ", " << sizey << ", "
                  << sizez << ", " << sizet << " )" << std::endl;
        options->getProperty( "unallocated", unalloc );
        obj.reallocate( sizex, sizey, sizez, sizet, false, context, !unalloc );
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
        PropertySet	& ps = obj.header();
        ps.setProperty( "sizeX", sizex );
        ps.setProperty( "sizeY", sizey );
        ps.setProperty( "sizeZ", sizez );
        ps.setProperty( "sizeT", sizet );
      }
    obj.blockSignals( false );
  }

  template <typename T>
  void Creator<VolumeRef<T> >::setup( VolumeRef<T> & obj, Object header,
                                   const AllocatorContext & context,
                                   Object options )
  {
    std::cout << "VOLUMEREFSETUP" << std::endl;
    Creator<Volume<T> >::setup( *obj, header, context, options );
  }

  template <typename T>
  VolumeRef<T>* Creator<VolumeRef<T> >::create( Object header,
                                   const AllocatorContext & context,
                                   Object options )
  {
    std::cout << "VOLUMEREFCREATE" << std::endl;
    return new VolumeRef<T>;
  }

}


#endif