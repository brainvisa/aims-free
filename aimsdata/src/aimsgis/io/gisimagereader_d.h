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

#ifndef GISIMAGEREADER_D_H
#define GISIMAGEREADER_D_H
//--- aims ---------------------------------------------------------------------
#ifdef USE_SOMA_IO
  #include <aims/io/gisimagereader.h>                       // class declaration
#else
  #include <soma-io/plugingis/gisimagereader.h>
#endif
//--- soma-io ------------------------------------------------------------------
#include <soma-io/config/soma_config.h>
#include <soma-io/image/imagereader.h>                               // heritage
#include <soma-io/datasourceinfo/datasourceinfo.h>        // function's argument
#include <soma-io/datasource/filedatasource.h>                // used by clone()
#include <soma-io/datasource/datasource.h>
#include <soma-io/datasource/chaindatasource.h>                      // heritage
#include <soma-io/reader/itemreader.h>                        // read + byteswap
//#include <soma-io/writer/itemwriter.h>                     // write + byteswap
//--- cartobase ----------------------------------------------------------------
#include <cartobase/object/object.h>                          // header, options
#include <cartobase/type/string_conversion.h>                 // conversion
//--- system -------------------------------------------------------------------
#include <memory>
#include <vector>
#include <iostream>
//------------------------------------------------------------------------------
//#define SOMAIO_GIR_DEBUG

namespace soma {
  
  //============================================================================
  //   U S E F U L
  //============================================================================
  template <typename T> 
  void GisImageReader<T>::updateParams( DataSourceInfo & dsi )
  {
    try {
      ImageReader<T>::_binary = !(bool) dsi.header()->getProperty( "ascii" )
                                                    ->getScalar();
    } catch( ... ) {
      ImageReader<T>::_binary = true;
    }
    
    try {
      ImageReader<T>::_byteswap = (bool) dsi.header()
                                            ->getProperty( "byte_swapping" )
                                            ->getScalar();
    } catch( ... ) {
      ImageReader<T>::_byteswap = false;
    }
    
    DefaultItemReader<T>  dir;
    _itemr.reset( dir.reader( ImageReader<T>::_binary, 
                              ImageReader<T>::_byteswap ) );
    //DefaultItemWriter<T>  diw;
    //_itemw.reset( diw.writer( _binary, _byteswap ) );
    
    ImageReader<T>::_sizes = std::vector< std::vector<int> >( 1, std::vector<int>(4) );
    dsi.header()->getProperty( "sizeX", ImageReader<T>::_sizes[ 0 ][ 0 ] );
    dsi.header()->getProperty( "sizeY", ImageReader<T>::_sizes[ 0 ][ 1 ] );
    dsi.header()->getProperty( "sizeZ", ImageReader<T>::_sizes[ 0 ][ 2 ] );
    dsi.header()->getProperty( "sizeT", ImageReader<T>::_sizes[ 0 ][ 3 ] );
    
    ChainDataSource::setSource( dsi.list().dataSource( "ima", 0 ), 
                                dsi.list().dataSource( "ima", 0)->url() );
  }
  
  //============================================================================
  //   C O N S T R U C T O R S
  //============================================================================
  /* base constructors usage :
   * - FileDataSource( rc_ptr<DataSource> ds, const string & url = string() );
   * - /!\ no copy constructor
   * - ImageReader( DataSourceInfo & dsi = 0, bool threadsafe = false );
   * - ImageReader( const ImageReader<T> & );
   */
  
  template <typename T>
  GisImageReader<T>::GisImageReader() :
    ImageReader<T>(),
    ChainDataSource( DataSource::none() ),
    _itemr()
  {
  }
  
  template <typename T>
  GisImageReader<T>::~GisImageReader()
  {
  }
  
  //============================================================================
  //   C H A I N D A T A S O U R C E   M E T H O D S
  //============================================================================
  template <typename T> 
  DataSource* GisImageReader<T>::clone() const
  {
    return new FileDataSource( ChainDataSource::url() );
  }
  
  template <typename T> 
  int GisImageReader<T>::iterateMode() const
  {
    int m = source()->iterateMode();
    // direct access is not possible on ascii streams
    if( ImageReader<T>::_binary )
      m &= SequentialAccess;
    return m;
  }
  
  template <typename T> 
  offset_t GisImageReader<T>::size() const
  {
    return source() ? source()->size() : 0;
  }
  
  template <typename T> 
  offset_t GisImageReader<T>::at() const
  {
    return source()->at();
  }
  
  template <typename T> 
  bool GisImageReader<T>::at( offset_t pos )
  {
    return source()->at( pos );
  }
  
  template <typename T> 
  long GisImageReader<T>::readBlock( char * data, unsigned long maxlen )
  {
    unsigned long nitems = maxlen / sizeof( T );
    return _itemr->read( *source(), (T *) data, nitems ) * sizeof( T );
  }
  
  
  template <typename T> 
  long GisImageReader<T>::writeBlock( const char * data, unsigned long len )
  {
    /*
    if( !_itemw.get() )
    {
      DefaultItemWriter<T>      diw;
      _itemw.reset( diw.writer( _binary, _byteswap ) );
    }
    unsigned long nitems = len / sizeof( T );
    return _itemw->write( *source(), (const T *) data, nitems ) 
      * sizeof( T );
    */
    return 0;
  }
  
  
  template <typename T> 
  int GisImageReader<T>::getch()
  {
    return source()->getch();
  }
  
  template <typename T> 
  int GisImageReader<T>::putch( int ch )
  {
    return source()->putch( ch );
  }
  
  template <typename T> 
  bool GisImageReader<T>::ungetch( int ch )
  {
    return source()->ungetch( ch );
  }
  
  template <typename T> 
  bool GisImageReader<T>::allowsMemoryMapping() const
  {
    return ImageReader<T>::_binary && !ImageReader<T>::_byteswap;
    
  }
  
  template <typename T> 
  bool GisImageReader<T>::setpos( int x, int y, int z, int t )
  {
    offset_t  lin = (offset_t) ImageReader<T>::_sizes[ 0 ][ 0 ];
    offset_t  sli = lin * ImageReader<T>::_sizes[ 0 ][ 1 ];
    offset_t  vol = sli * ImageReader<T>::_sizes[ 0 ][ 2 ];
    return source()->at( ( (offset_t) sizeof(T) ) * 
                         ( x + y * lin + z * sli + t * vol ) );
  }
  
  //============================================================================
  //   I M A G E R E A D E R   M E T H O D S
  //============================================================================
  template <typename T>
  void GisImageReader<T>::read( T * dest, DataSourceInfo & dsi,
                                std::vector<int> & pos,  /* size 4 : x,y,z,t */
                                std::vector<int> & size, /* size 4 : x,y,z,t */
                                std::vector<int> /* stride */,
                                int /* level */, carto::Object /* options */ )
  {
    if( ImageReader<T>::_sizes.empty() )
      updateParams( dsi );
    
    // dest is supposed to be allocated
    
    // total volume size
    int  sx = ImageReader<T>::_sizes[ 0 ][ 0 ];
    int  sy = ImageReader<T>::_sizes[ 0 ][ 1 ];
    int  sz = ImageReader<T>::_sizes[ 0 ][ 2 ];
    int  st = ImageReader<T>::_sizes[ 0 ][ 3 ];
    // region size
    int  vx = size[ 0 ];
    int  vy = size[ 1 ];
    int  vz = size[ 2 ];
    int  vt = size[ 3 ];
    // region position
    int  ox = pos[ 0 ];
    int  oy = pos[ 1 ];
    int  oz = pos[ 2 ];
    int  ot = pos[ 3 ];
    int  y, z, t;
    // region line size
    offset_t  len = vx * sizeof( T );
    
    if( !open( DataSource::Read ) )
      throw carto::open_error( "data source not available", url() );
    
    for( t=0; t<vt; ++t ) {
      for( z=0; z<vz; ++z ) {
        for( y=0; y<vy; ++y ) {
          // we move in the file
          at( ( sx * ( sy * ( sz * ( t + ot ) + z + oz ) 
                            + y + oy ) + ox ) * sizeof(T) );
          // we move in the buffer
          char * target = (char *) dest + len * ( vy * ( vz * t + z ) + y );
          if( readBlock(target, len ) != (long) len )
            throw carto::eof_error( url() );
        }
      }
    }
    
    #ifdef SOMAIO_GIR_DEBUG
      int x;
      for( x=0; x<vt*vz*vy*vx; ++x ) {
        std::cout << *(dest + x) << " ";
        if( x%vx == vx-1 )
          std::cout << std:: endl;
      }
    #endif
  }
}

#endif