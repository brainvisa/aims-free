#ifndef CARTODATA_VOLUME_VOLUMEPROXY_D_H
#define CARTODATA_VOLUME_VOLUMEPROXY_D_H


#include <cartodata/volume/volumeproxy.h>


namespace carto
{


  template < typename T >
  VolumeProxy< T >::VolumeProxy( int sizeX, int sizeY, int sizeZ, int sizeT )
    : Headered(),
      _size( 4 )
  {
    _size[0] = sizeX;
    _size[1] = sizeY;
    _size[2] = sizeZ;
    _size[3] = sizeT;

    header().addBuiltinProperty( "volume_dimension", _size );
    header().addBuiltinProperty( "sizeX", _size[0] );
    header().addBuiltinProperty( "sizeY", _size[1] );
    header().addBuiltinProperty( "sizeZ", _size[2] );
    header().addBuiltinProperty( "sizeT", _size[3] );

  }


  template < typename T >
  VolumeProxy< T >::VolumeProxy( const std::vector<int> & size )
    : Headered(),
      _size( size )
  {
    if( _size.size() < 4 )
    {
      _size.reserve( 4 );
      _size.insert( _size.end(), 4 - _size.size(), 1 );
    }
    header().addBuiltinProperty( "volume_dimension", _size );
    header().addBuiltinProperty( "sizeX", _size[0] );
    header().addBuiltinProperty( "sizeY", _size[1] );
    header().addBuiltinProperty( "sizeZ", _size[2] );
    header().addBuiltinProperty( "sizeT", _size[3] );
  }


  template < typename T >
  VolumeProxy< T >::VolumeProxy( const VolumeProxy< T >& other )
    : RCObject(),
      Headered( other ),
      _size( other._size )
  {

    if( header().hasProperty( "volume_dimension" ) )
      header().removeProperty( "volume_dimension" );
    if( header().hasProperty( "sizeX" ) )
      header().removeProperty( "sizeX" );
    if( header().hasProperty( "sizeY" ) )
      header().removeProperty( "sizeY" );
    if( header().hasProperty( "sizeZ" ) )
      header().removeProperty( "sizeZ" );
    if( header().hasProperty( "sizeT" ) )
      header().removeProperty( "sizeT" );

    header().addBuiltinProperty( "volume_dimension", _size );
    header().addBuiltinProperty( "sizeX", _size[0] );
    header().addBuiltinProperty( "sizeY", _size[1] );
    header().addBuiltinProperty( "sizeZ", _size[2] );
    header().addBuiltinProperty( "sizeT", _size[3] );

  }


  template < typename T >
  VolumeProxy< T >::~VolumeProxy()
  {
  }


  template < typename T >
  VolumeProxy< T >&
  VolumeProxy< T >::operator=( const VolumeProxy< T >& other )
  {

    if( this == &other )
      return *this;

    this->Headered::operator=( other );
    _size = other._size;

    if( header().hasProperty( "sizeX" ) )
      header().changeBuiltinProperty( "sizeX", _size[0] );
    if( header().hasProperty( "sizeY" ) )
      header().changeBuiltinProperty( "sizeY", _size[1] );
    if( header().hasProperty( "sizeZ" ) )
      header().changeBuiltinProperty( "sizeZ", _size[2] );
    if( header().hasProperty( "sizeT" ) )
      header().changeBuiltinProperty( "sizeT", _size[3] );

    return *this;

  }


  template < typename T >
  std::vector<float> VolumeProxy< T >::getVoxelSize() const
  {

    size_t i, n = _size.size();
    std::vector<float> voxelsize( n, 1. );
    carto::Object vso;
    try
    {
      vso = header().getProperty( "voxel_size" );
      if( vso->size() < n )
        n = vso->size();
      for( i=0; i<n; ++i )
        try
        {
          voxelsize[i] = float( vso->getArrayItem(i)->getScalar() );
        }
        catch( std::exception & )
        {
        }
    }
    catch( std::exception & )
    {
    }

    return voxelsize;

  }


  template < typename T>
  void VolumeProxy<T>::setVoxelSize( float vx, float vy, float vz, float vt )
  {
    std::vector<float> vs( 4 );
    vs[0] = vx;
    vs[1] = vy;
    vs[2] = vz;
    vs[3] = vt;
    setVoxelSize( vs );
  }


  template < typename T>
  void VolumeProxy<T>::setVoxelSize( const std::vector<float> & vs )
  {
    header().setProperty( "voxel_size", vs );
  }


  template < typename T >
  void VolumeProxy< T >::copyHeaderFrom( const Object & other,
                                         bool stopOnError )
  {
    if( other.isNull() )
      return;

    std::set<std::string> forbidden;
    forbidden.insert( "sizeX" );
    forbidden.insert( "sizeY" );
    forbidden.insert( "sizeZ" );
    forbidden.insert( "sizeT" );
    forbidden.insert( "volume_dimension" );

    PropertySet & hdr = header();
    std::string err_msg;

    Object it = other->objectIterator();
    while( it->isValid() )
    {
      if( forbidden.find( it->key() ) == forbidden.end() )
        try
        {
          hdr.setProperty( it->key(), it->currentValue() );
        }
        catch( std::exception & e )
        {
          std::cerr << "header property " << it->key() << " could not be copied: probably the destination already exists as a builtin with a different type.\n";
          std::cerr << "error message: " << e.what() << std::endl;
          if( stopOnError )
            throw;
          err_msg = e.what();
        }
      it->next();
    }
    if( !err_msg.empty() )
      throw std::runtime_error( err_msg );
  }

}

#endif
