#ifndef CARTODATA_VOLUME_VOLUMEPROXY_H
#define CARTODATA_VOLUME_VOLUMEPROXY_H


#include <cartobase/object/headered.h>


namespace carto
{


  /** VolumeProxy is the base class for volumes. It handles the management of
      the volume size, but not its contents (it has no data block).
  */
  template < typename T >
  class VolumeProxy : public Headered
  {

  public:

    explicit VolumeProxy( int sizeX = 1, int sizeY = 1, int sizeZ = 1,
                          int sizeT = 1 );
    explicit VolumeProxy( const std::vector<int> & size );
    VolumeProxy( const VolumeProxy< T >& other );
    virtual ~VolumeProxy();

    int getSizeX() const;
    int getSizeY() const;
    int getSizeZ() const;
    int getSizeT() const;
    /// get the 4 dimensions in a vector
    std::vector<int> getSize() const;
    /** get the voxel size from the header,
       with 4 values defaulting to 1.mm if not present */
    std::vector<float> getVoxelSize() const;
    void setVoxelSize( float vx, float vy = 1., float vz = 1., float vt = 1. );
    void setVoxelSize( const std::vector<float> & vs );

    VolumeProxy< T >& operator=( const VolumeProxy< T >& other );

    /** copy properties from other to this, avoiding forbidden
        properties like size.
    */
    virtual void copyHeaderFrom( const PropertySet & other,
                                 bool stopOnError = true );
    virtual void copyHeaderFrom( const Object & other,
                                 bool stopOnError = true );

  protected:

    std::vector<int> _size;

  };


  template < typename T >
  inline
  int VolumeProxy< T >::getSizeX() const
  {

    return _size[0];

  }


  template < typename T >
  inline
  int VolumeProxy< T >::getSizeY() const
  {

    return _size[1];

  }


  template < typename T >
  inline
  int VolumeProxy< T >::getSizeZ() const
  {

    return _size[2];

  }


  template < typename T >
  inline
  int VolumeProxy< T >::getSizeT() const
  {

    return _size[3];

  }


  template < typename T >
  inline
  std::vector<int> VolumeProxy< T >::getSize() const
  {

    return _size;

  }


  template < typename T >
  inline
  void VolumeProxy< T >::copyHeaderFrom( const PropertySet & other,
                                         bool stopOnError )
  {
    copyHeaderFrom( Object::reference( other), stopOnError );
  }


 //  instanciations

  extern template class VolumeProxy<int8_t>;
  extern template class VolumeProxy<uint8_t>;
  // ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
  extern template class VolumeProxy<char>;
#endif
  extern template class VolumeProxy<int16_t>;
  extern template class VolumeProxy<uint16_t>;
  extern template class VolumeProxy<int32_t>;
  extern template class VolumeProxy<uint32_t>;
  extern template class VolumeProxy<long>;
  extern template class VolumeProxy<unsigned long>;
  extern template class VolumeProxy<float>;
  extern template class VolumeProxy<double>;
  extern template class VolumeProxy<cfloat>;
  extern template class VolumeProxy<cdouble>;
}


#endif
