#ifndef AIMS_DATA_FASTALLOCATIONDATA_H
#define AIMS_DATA_FASTALLOCATIONDATA_H

#include <aims/data/data.h>
#include <soma-io/allocator/allocator.h>

namespace aims {

  template <typename T>
  class AimsFastAllocationData : public AimsData<T>
  {
  public:

    static const carto::AllocatorContext fastAllocatorContext;

    inline AimsFastAllocationData( int dimx = 1, int dimy = 1, int dimz = 1, 
                                   int dimt = 1, int borderw = 0 ) :
      AimsData<T>( dimx, dimy, dimz, dimt, borderw,
                AimsFastAllocationData::fastAllocatorContext ) {}

    inline AimsFastAllocationData( const AimsFastAllocationData<T> &other ) :
      AimsData<T>( other ) {}

    inline AimsFastAllocationData( const AimsData<T> &other ) :
      AimsData<T>( other ) {}

    inline AimsFastAllocationData(
      const carto::rc_ptr<carto::Volume<T> > &other ) :
      AimsData<T>( other ) {}

    inline AimsFastAllocationData<T> &operator =
      ( const AimsFastAllocationData<T> &other )
    {
      if ( &other != this ) {
        static_cast<AimsData<T> &>( *this ) = other;
      }
      return *this;
    }

    inline AimsFastAllocationData<T> &operator =
      ( const AimsData<T> &other ) 
    {
      if ( &other != this ) {
        static_cast<AimsData<T> &>( *this ) = other;
      }
      return *this;
    }

    inline AimsFastAllocationData<T> &operator =
      ( const carto::rc_ptr<carto::Volume<T> > &other )
    {
      this->AimsData<T>::operator = ( other );
      return *this;
    }

    AimsFastAllocationData<T> & operator = ( const T &v )
    {
      AimsData<T>::operator =( v );
      return *this;
    }

    inline virtual ~AimsFastAllocationData() {};

    /// cast to Volume
    operator carto::rc_ptr<carto::Volume<T> > & ()
    { return this->AimsData<T>::operator carto::rc_ptr<carto::Volume<T> > & (); }
    operator const carto::rc_ptr<carto::Volume<T> > & () const
    { return this->AimsData<T>::operator const carto::rc_ptr<carto::Volume<T> > & (); }
    operator carto::VolumeRef<T> & ()
    { return this->AimsData<T>::operator carto::VolumeRef<T> & (); }
    operator const carto::VolumeRef<T> & () const
    { return this->AimsData<T>::operator const carto::VolumeRef<T> & (); }
  };


  template <typename T>
  const carto::AllocatorContext 
  AimsFastAllocationData<T>::fastAllocatorContext( &carto::MemoryAllocator::singleton() );


  extern template class AimsFastAllocationData<uint8_t>;
  extern template class AimsFastAllocationData<int8_t>;
  extern template class AimsFastAllocationData<uint16_t>;
  extern template class AimsFastAllocationData<int16_t>;
  extern template class AimsFastAllocationData<uint32_t>;
  extern template class AimsFastAllocationData<int32_t>;
  extern template class AimsFastAllocationData<float>;
  extern template class AimsFastAllocationData<double>;

} // namespace aims

#endif // ifndef AIMS_DATA_FASTALLOCATIONDATA_H
