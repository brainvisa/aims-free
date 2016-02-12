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
                           const AllocatorContext & allocContext ):
    rc_ptr<Volume<T> >( new Volume<T>( other, pos, size, allocContext ) )
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
    return (*this)->getPropertySet();
  }

  template <typename T>
  inline
  PropertySet& VolumeRef<T>::getPropertySet()
  {
    return (*this)->getPropertySet();
  }

  template <typename T>
  inline
  std::vector<float> VolumeRef<T>::getVoxelSize() const
  {
    return (*this)->getVoxelSize();
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
    (*this)->allocatorContext();
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
  std::vector<int> VolumeRef<T>::getBorders() const
  {
    return (*this)->getBorders();
  }

  template <typename T>
  inline
  std::vector<size_t> VolumeRef<T>::getStrides() const
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

} // namespace carto


//============================================================================
//   STREAMS
//============================================================================

template <typename T>
std::ostream & operator<< ( std::ostream & out,
                            const carto::rc_ptr<carto::Volume<T> > & volume )
{
  carto::VolumeOStream volumeout( out );
  return volumeout << volume;
}

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


#endif // CARTODATA_VOLUME_VOLUMEREF_D_INLINE_H
