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

#ifndef CARTODATA_IO_VOLUMEDATASOURCE_D_H
#define CARTODATA_IO_VOLUMEDATASOURCE_D_H

#include <cartodata/io/volumedatasource.h>

namespace carto
{

  template <typename T> 
  VolumeDataSource<T>::VolumeDataSource( rc_ptr<DataSource> ds, Object header, 
                                         const std::string & url )
    : VolumeProxy<T>(), ChainDataSource( ds, url )
  {
    VolumeProxy<T>::header().copyProperties( header );
    updateParams();
  }


  template <typename T> 
  VolumeDataSource<T>::~VolumeDataSource()
  {
  }


  template <typename T> 
  void VolumeDataSource<T>::updateParams()
  {
    try
      {
        _binary = !(bool) 
          VolumeProxy<T>::header().getProperty( "ascii" )->getScalar();
      }
    catch( ... )
      {
        _binary = true;
      }
    try
      {
        _byteswap = (bool) 
          VolumeProxy<T>::header().getProperty( "byte_swapping" )
          ->getScalar();
      }
    catch( ... )
      {
        _byteswap = false;
      }
    DefaultItemReader<T>	dir;
    _itemr.reset( dir.reader( _binary, _byteswap ) );
    //DefaultItemWriter<T>	diw;
    //_itemw.reset( diw.writer( _binary, _byteswap ) );
  }


  template <typename T> 
  DataSource* VolumeDataSource<T>::clone() const
  {
    Object	o = Object::reference
      ( const_cast<PropertySet &>( VolumeProxy<T>::header() ) );
    return new VolumeDataSource<T>
      ( rc_ptr<DataSource>( source() ? source()->clone() : 0 ), 
        o, url() );
  }


  template <typename T> 
  offset_t  VolumeDataSource<T>::size() const
  {
    return source() ? source()->size() : 0;
  }


  template <typename T> 
  int VolumeDataSource<T>::iterateMode() const
  {
    int m = source()->iterateMode();
    // direct access is not possible on ascii streams
    if( !_binary )
      m &= SequentialAccess;
    return m;
  }


  template <typename T> 
  offset_t VolumeDataSource<T>::at() const
  {
    return source()->at();
  }


  template <typename T> 
  bool VolumeDataSource<T>::at( offset_t pos )
  {
    return source()->at( pos );
  }


  template <typename T> 
  long VolumeDataSource<T>::readBlock( char * data, unsigned long maxlen )
  {
    unsigned long	nitems = maxlen / sizeof( T );
    return _itemr->read( *source(), (T *) data, nitems ) * sizeof( T );
  }


  template <typename T> 
  long VolumeDataSource<T>::writeBlock( const char * data, unsigned long len )
  {
    if( !_itemw.get() )
    {
      DefaultItemWriter<T>      diw;
      _itemw.reset( diw.writer( _binary, _byteswap ) );
    }
    unsigned long	nitems = len / sizeof( T );
    return _itemw->write( *source(), (const T *) data, nitems ) 
      * sizeof( T );
  }


  template <typename T> 
  int VolumeDataSource<T>::getch()
  {
    return source()->getch();
  }


  template <typename T> 
  int VolumeDataSource<T>::putch( int ch )
  {
    return source()->putch( ch );
  }


  template <typename T> 
  bool VolumeDataSource<T>::ungetch( int ch )
  {
    return source()->ungetch( ch );
  }


  template <typename T> 
  bool VolumeDataSource<T>::allowsMemoryMapping() const
  {
    return _binary && !_byteswap;
  }


  template <typename T> 
  bool VolumeDataSource<T>::setpos( int x, int y, int z, int t )
  {
    offset_t	lin = (offset_t) VolumeProxy<T>::getSizeX();
    offset_t	sli = lin * VolumeProxy<T>::getSizeY();
    offset_t	vol = sli * VolumeProxy<T>::getSizeZ();
    return source()->at( ( (offset_t) sizeof(T) ) * 
                         ( x + y * lin + z * sli + t * vol ) );
  }

}

#endif

