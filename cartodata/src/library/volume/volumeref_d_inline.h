#ifndef CARTODATA_VOLUME_VOLUMEREF_D_INLINE_H
#define CARTODATA_VOLUME_VOLUMEREF_D_INLINE_H

//--- cartodata --------------------------------------------------------------
#include <cartodata/volume/volumeref.h>
//----------------------------------------------------------------------------

namespace carto {

  //==========================================================================
  //   RCPTR CONSTRUCTORS
  //==========================================================================
  template <typename T>
  VolumeRef<T>::VolumeRef():
    rc_ptr<Volume<T> >( new Volume<T> )
  {}

  template <typename T>
  VolumeRef<T>::VolumeRef( Volume<T>* x ):
    rc_ptr<Volume<T> >(x)
  {}

  template <typename T>
  VolumeRef<T>::VolumeRef( const rc_ptr<Volume<T> > & x ):
    rc_ptr<Volume<T> >(x)
  {}

  template <typename T>
  VolumeRef<T>::~VolumeRef()
  {}

#ifdef CARTO_VOLUME_AUTO_DEREFERENCE

  //==========================================================================
  //   CONSTRUCTORS
  //==========================================================================
  template <typename T>
  VolumeRef<T>::VolumeRef( int sizeX, int sizeY, int sizeZ, int sizeT,
                           const AllocatorContext& allocatorContext,
                           bool allocated ):
    rc_ptr<Volume<T> >( new Volume<T>( sizeX, sizeY, sizeZ, sizeT,
                                       allocatorContext, allocated ) )
  {}

  template <typename T>
  VolumeRef<T>::VolumeRef( const Position4Di & size,
                           const AllocatorContext& allocatorContext,
                           bool allocated ):
    rc_ptr<Volume<T> >( new Volume<T>( size, allocatorContext, allocated ) )
  {}

  template <typename T>
  VolumeRef<T>::VolumeRef( int sizeX, int sizeY, int sizeZ, int sizeT,
                           int border,
                           const AllocatorContext& allocatorContext,
                           bool allocated ):
    rc_ptr<Volume<T> >( new Volume<T>( sizeX, sizeY, sizeZ, sizeT, border,
                                       allocatorContext, allocated ) )
  {}

  template <typename T>
  VolumeRef<T>::VolumeRef( const Position4Di & size, int border,
                           const AllocatorContext& allocatorContext,
                           bool allocated ):
    rc_ptr<Volume<T> >( new Volume<T>( size, border, allocatorContext, allocated ) )
  {}

  template <typename T>
  VolumeRef<T>::VolumeRef( int sizeX, int sizeY, int sizeZ, int sizeT,
                           const Position4Di & border,
                           const AllocatorContext& allocatorContext,
                           bool allocated ):
    rc_ptr<Volume<T> >( new Volume<T>( sizeX, sizeY, sizeZ, sizeT, border,
                                       allocatorContext, allocated ) )
  {}

  template <typename T>
  VolumeRef<T>::VolumeRef( const Position4Di & size,
                           const Position4Di & border,
                           const AllocatorContext& allocatorContext,
                           bool allocated ):
    rc_ptr<Volume<T> >( new Volume<T>( size, border, allocatorContext, allocated ) )
  {}

  template <typename T>
  VolumeRef<T>::VolumeRef( rc_ptr<Volume<T> > other,
                           const Position4Di & pos,
                           const Position4Di & size,
                           const AllocatorContext & allocContext,
                           bool transToParent ):
    rc_ptr<Volume<T> >( new Volume<T>( other, pos, size, allocContext,
                                       transToParent ) )
  {}

  template <typename T>
  VolumeRef<T>::VolumeRef( rc_ptr<Volume<T> > other,
                           const Position & pos,
                           const Position & size,
                           const AllocatorContext & allocContext,
                           bool transToParent ):
    rc_ptr<Volume<T> >( new Volume<T>( other, pos, size, allocContext,
                                       transToParent ) )
  {}

  //==========================================================================
  //   VOLUME PROXY
  //==========================================================================
  template <typename T>
  inline
  std::vector<int> VolumeRef<T>::getSize() const
  {
    return (*this)->getSize();
  }

  template <typename T>
  inline
  int VolumeRef<T>::getSizeX() const
  {
    return (*this)->getSizeX();
  }

  template <typename T>
  inline
  int VolumeRef<T>::getSizeY() const
  {
    return (*this)->getSizeY();
  }

  template <typename T>
  inline
  int VolumeRef<T>::getSizeZ() const
  {
    return (*this)->getSizeZ();
  }

  template <typename T>
  inline
  int VolumeRef<T>::getSizeT() const
  {
    return (*this)->getSizeT();
  }

  template <typename T>
  inline
  const PropertySet& VolumeRef<T>::header() const
  {
    return (*this)->header();
  }

  template <typename T>
  inline
        PropertySet& VolumeRef<T>::header()
  {
    return (*this)->header();
  }

  template <typename T>
  inline
  const PropertySet& VolumeRef<T>::getPropertySet() const
  {
    return (*this)->header();
  }

  template <typename T>
  inline
  PropertySet& VolumeRef<T>::getPropertySet()
  {
    return (*this)->header();
  }

  template <typename T>
  inline
  std::vector<float> VolumeRef<T>::getVoxelSize() const
  {
    return (*this)->getVoxelSize();
  }

  template <typename T>
  inline
  void VolumeRef<T>::setVoxelSize( const std::vector<float> & vs )
  {
    (*this)->setVoxelSize( vs );
  }

  template <typename T>
  inline
  void VolumeRef<T>::setVoxelSize( float vx, float vy, float vz, float vt )
  {
    (*this)->setVoxelSize( vx, vy, vz, vt );
  }

  //==========================================================================
  //   ITERATORS
  //==========================================================================
  template <typename T>
  inline
  typename VolumeRef<T>::iterator VolumeRef<T>::begin()
  {
    return (*this)->begin();
  }

  template <typename T>
  inline
  typename VolumeRef<T>::iterator VolumeRef<T>::end()
  {
    return (*this)->end();
  }

  template <typename T>
  inline
  typename VolumeRef<T>::const_iterator VolumeRef<T>::begin() const
  {
    return (*this)->begin();
  }

  template <typename T>
  inline
  typename VolumeRef<T>::const_iterator VolumeRef<T>::end() const
  {
    return (*this)->end();
  }

  //==========================================================================
  //   ACCESSORS
  //==========================================================================
  template <typename T>
  inline
  const T& VolumeRef<T>::operator()( long x, long y, long z, long t ) const
  {
    return (**this)( x, y, z, t );
  }

  template <typename T>
  inline
  T& VolumeRef<T>::operator() ( long x, long y, long z, long t )
  {
    return (**this)( x, y, z, t );
  }

  template <typename T>
  inline
  const T& VolumeRef<T>::at( long x, long y, long z, long t ) const
  {
    return (*this)->at( x, y, z, t );
  }

  template <typename T>
  inline
  T& VolumeRef<T>::at( long x, long y, long z, long t )
  {
    return (*this)->at( x, y, z, t );
  }

  template <typename T>
  inline
  const T& VolumeRef<T>::operator() ( const Position4Di & position ) const
  {
    return (**this)(position);
  }

  template <typename T>
  inline
  T& VolumeRef<T>::operator() ( const Position4Di & position )
  {
    return (**this)(position);
  }

  template <typename T>
  inline
  const T& VolumeRef<T>::at( const Position4Di & position ) const
  {
    return (*this)->at(position);
  }

  template <typename T>
  inline
  T& VolumeRef<T>::at( const Position4Di & position )
  {
    return (*this)->at(position);
  }

  template <typename T>
  inline
  const T& VolumeRef<T>::operator() ( const Position & position ) const
  {
    return (**this)(position);
  }

  template <typename T>
  inline
  T& VolumeRef<T>::operator() ( const Position & position )
  {
    return (**this)(position);
  }

  template <typename T>
  inline
  const T& VolumeRef<T>::at( const Position & position ) const
  {
    return (*this)->at(position);
  }

  template <typename T>
  inline
  T& VolumeRef<T>::at( const Position & position )
  {
    return (*this)->at(position);
  }

  //==========================================================================
  //   INIT / ALLOCATION
  //==========================================================================
  template <typename T>
  inline
  void VolumeRef<T>::initialize()
  {
    (*this)->initialize();
  }

  template <typename T>
  inline
  const AllocatorContext & VolumeRef<T>::allocatorContext() const
  {
    return (*this)->allocatorContext();
  }

  template <typename T>
  inline
  void VolumeRef<T>::allocate()
  {
    (*this)->allocate();
  }

  template <typename T>
  inline
  void VolumeRef<T>::reallocate( int sizeX, int sizeY, int sizeZ, int sizeT,
                                 bool keepcontents,
                                 const AllocatorContext& allocatorContext,
                                 bool allocate )
  {
    (*this)->reallocate( sizeX, sizeY, sizeZ, sizeT,
                         keepcontents, allocatorContext, allocate );
  }

#if 0
  template <typename T>
  inline
  void VolumeRef<T>::reallocate( const Position4Di & size, bool keepcontents,
                                 const AllocatorContext& allocatorContext,
                                 bool allocate )
  {
    (*this)->reallocate( size, keepcontents, allocatorContext, allocate );
  }
#endif


  //==========================================================================
  //   COPY / VIEW
  //==========================================================================

  template <typename T>
  inline
  rc_ptr<Volume<T> > VolumeRef<T>::refVolume() const
  {
    return (*this)->refVolume();
  }

  template <typename T>
  inline
  void VolumeRef<T>::setRefVolume(const rc_ptr<Volume<T> > & refvol)
  {
    return (*this)->setRefVolume( refvol );
  }

  template <typename T>
  inline
  const typename VolumeRef<T>::Position4Di VolumeRef<T>::posInRefVolume() const
  {
    return (*this)->posInRefVolume();
  }

  template <typename T>
  inline
  void VolumeRef<T>::setPosInRefVolume(const Position4Di & pos)
  {
    return (*this)->setPosInRefVolume( pos );
  }
  

  template <typename T>
  inline
  int VolumeRef<T>::getLevelsCount() const {
    return (*this)->getLevelsCount();
  }

  template <typename T>
  inline
  int VolumeRef<T>::refLevel(const int level) const {
    return (*this)->refLevel(level);
  }

  template <typename T>
  inline
  rc_ptr<Volume<T> > VolumeRef<T>::refVolumeAtLevel(const int level) const {
    return (*this)->refVolumeAtLevel(level);
  }

  template <typename T>
  inline
  typename Volume<T>::Position VolumeRef<T>::posInRefVolumeAtLevel(
      const int level) const {
    return (*this)->posInRefVolumeAtLevel(level);
  }

  template <typename T>
  inline
  std::vector<int> VolumeRef<T>::getBorders() const
  {
    return (*this)->getBorders();
  }

  template <typename T>
  inline
  std::vector<long> VolumeRef<T>::getStrides() const
  {
      return (*this)->getStrides();
  }

  template < typename T >
  inline
  void VolumeRef< T >::copyHeaderFrom( const PropertySet & other )
  {
    (*this)->copyHeaderFrom( other );
  }

#endif // CARTO_VOLUME_AUTO_DEREFERENCE

  //==========================================================================
  //   MAKE VIEW
  //==========================================================================

  template <typename T>
  inline
  VolumeRef<T> VolumeRef<T>::view( const Position4Di & pos, const Position4Di & size )
  {
    return VolumeRef<T>( *this, pos, size );
  }

  template <typename T>
  inline
  const VolumeRef<T> VolumeRef<T>::view( const Position4Di & pos, const Position4Di & size ) const
  {
    return VolumeRef<T>( *this, pos, size );
  }

  template <typename T>
  carto::Object getObjectHeader( VolumeRef<T> & obj )
  {
    return Object::reference( obj->header() );
  }

  //==========================================================================
  //   STREAMS
  //==========================================================================

  template <typename T>
  std::ostream & operator<< ( const carto::VolumeOStream & out,
                              const carto::rc_ptr<carto::Volume<T> > & volume )
  {
    out.ostream() << "VolumeRef" << std::flush;
    if( !volume.get() ) {
      out.ostream() << " of " << carto::DataTypeCode<T>::dataType()
                    << ": empty" << std::endl;
      return out.ostream();
    }
    else
    {
      out.ostream() << ": " << std::flush;
      return out << *(volume.get());
    }
  }

  template <typename T>
  std::ostream & operator<< ( std::ostream & out,
                              const carto::rc_ptr<carto::Volume<T> > & volume )
  {
    carto::VolumeOStream volumeout( out );
    return volumeout << volume;
  }
  
  template <typename T>
  inline
  void displayRefVolumes(const carto::VolumeRef<T> & vol) {
    carto::displayRefVolumes(*(vol.get()));
  }

} // namespace carto




#endif // CARTODATA_VOLUME_VOLUMEREF_D_INLINE_H
