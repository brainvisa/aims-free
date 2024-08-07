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
