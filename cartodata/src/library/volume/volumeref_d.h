#ifndef CARTODATA_VOLUME_VOLUMEREF_D_H
#define CARTODATA_VOLUME_VOLUMEREF_D_H

#include <cartodata/volume/volumeref.h>
#include <cartodata/volume/volumeref_d_inline.h>
#include <cartodata/volume/volumeref_d_operators.h>
#include <cartodata/volume/volumeref_d_instantiate.h>
#include <cartodata/volume/volumebase_d.h>
#include <soma-io/allocator/allocator.h>
#include <cartobase/object/object.h>

namespace carto {

  template <typename T>
  void Creator<VolumeRef<T> >::setup( VolumeRef<T> & obj, Object header,
                                   const AllocatorContext & context,
                                   Object options )
  {
    Creator<Volume<T> >::setup( *obj, header, context, options );
  }

  template <typename T>
  VolumeRef<T>* Creator<VolumeRef<T> >::create( Object /* header */,
                                   const AllocatorContext & /* context */,
                                   Object /* options */ )
  {
    return new VolumeRef<T>;
  }
  
    
  template < typename T >
  void VolumeRef<T>::copyHeaderFrom( const Object & other )
  {
    (*this)->copyHeaderFrom( other );
  }

} // namespace carto

#endif // CARTODATA_VOLUME_VOLUMEREF_D_H
