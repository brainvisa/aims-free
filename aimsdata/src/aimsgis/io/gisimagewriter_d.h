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

#ifndef PLUGINGIS_GISIMAGEWRITER_D_H
#define PLUGINGIS_GISIMAGEWRITER_D_H
//--- aims ---------------------------------------------------------------------
#ifdef USE_SOMA_IO
  #include <aims/io/gisimagewriter.h>                       // class declaration
#else
  #include <soma-io/plugingis/gisimagewriter.h>
#endif
//--- soma-io ------------------------------------------------------------------
#include <soma-io/config/soma_config.h>
#include <soma-io/image/imagewriter.h>                               // heritage
#include <soma-io/io/writer.h>                                  // to write minf
#include <soma-io/datasourceinfo/datasourceinfo.h>        // function's argument
#include <soma-io/datasourceinfo/datasourceinfoloader.h>  // for partial writing
#include <soma-io/datasource/filedatasource.h>                // used by clone()
#include <soma-io/datasource/streamdatasource.h>        // used by writeHeader()
#include <soma-io/datasource/datasource.h>
#include <soma-io/datasource/chaindatasource.h>                      // heritage
//#include <soma-io/reader/itemreader.h>                        // read + byteswap
#include <soma-io/writer/itemwriter.h>                       // write + byteswap
#include <soma-io/writer/pythonwriter.h>
//--- cartobase ----------------------------------------------------------------
#include <cartobase/object/object.h>                          // header, options
#include <cartobase/object/property.h>                        // header, options
#include <cartobase/type/string_conversion.h>                      // conversion
#include <cartobase/type/types.h>                             // to write header
#include <cartobase/stream/fileutil.h>                            // utilitaires
//--- system -------------------------------------------------------------------
#include <memory>
#include <vector>
#include <iostream>
#include <fstream>
//------------------------------------------------------------------------------
#define SOMAIO_BYTE_ORDER 0x41424344  //"ABCD" in ascii
//#define SOMAIO_GIW_DEBUG

namespace soma {
  
  //============================================================================
  //   U S E F U L
  //============================================================================
  template <typename T> 
  void GisImageWriter<T>::updateParams( DataSourceInfo & dsi )
  {
    try {
      ImageWriter<T>::_binary = !(bool) dsi.header()->getProperty( "ascii" )
                                                    ->getScalar();
    } catch( ... ) {
      ImageWriter<T>::_binary = true;
    }
    
    try {
      ImageWriter<T>::_byteswap = (bool) dsi.header()
                                            ->getProperty( "byte_swapping" )
                                            ->getScalar();
    } catch( ... ) {
      ImageWriter<T>::_byteswap = false;
    }
    
    DefaultItemWriter<T>  diw;
    _itemw.reset( diw.writer( ImageWriter<T>::_binary, 
                              ImageWriter<T>::_byteswap ) );
    
    ImageWriter<T>::_sizes = std::vector< std::vector<int> >( 1, std::vector<int>(4) );
    dsi.header()->getProperty( "sizeX", ImageWriter<T>::_sizes[ 0 ][ 0 ] );
    dsi.header()->getProperty( "sizeY", ImageWriter<T>::_sizes[ 0 ][ 1 ] );
    dsi.header()->getProperty( "sizeZ", ImageWriter<T>::_sizes[ 0 ][ 2 ] );
    dsi.header()->getProperty( "sizeT", ImageWriter<T>::_sizes[ 0 ][ 3 ] );
    
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
  GisImageWriter<T>::GisImageWriter() :
    ImageWriter<T>(),
    ChainDataSource( DataSource::none() ),
    _itemw()
  {
  }
  
  template <typename T>
  GisImageWriter<T>::~GisImageWriter()
  {
  }
  
  //============================================================================
  //   C H A I N D A T A S O U R C E   M E T H O D S
  //============================================================================
  template <typename T> 
  DataSource* GisImageWriter<T>::clone() const
  {
    return new FileDataSource( ChainDataSource::url() );
  }
  
  template <typename T> 
  int GisImageWriter<T>::iterateMode() const
  {
    int m = source()->iterateMode();
    // direct access is not possible on ascii streams
    if( ImageWriter<T>::_binary )
      m &= SequentialAccess;
    return m;
  }
  
  template <typename T> 
  offset_t GisImageWriter<T>::size() const
  {
    return source() ? source()->size() : 0;
  }
  
  template <typename T> 
  offset_t GisImageWriter<T>::at() const
  {
    return source()->at();
  }
  
  template <typename T> 
  bool GisImageWriter<T>::at( offset_t pos )
  {
    return source()->at( pos );
  }
  
  template <typename T> 
  long GisImageWriter<T>::readBlock( char * data, unsigned long maxlen )
  {
    /*
    unsigned long nitems = maxlen / sizeof( T );
    return _itemr->read( *source(), (T *) data, nitems ) * sizeof( T );
    */
    return 0;
  }
  
  
  template <typename T> 
  long GisImageWriter<T>::writeBlock( const char * data, unsigned long len )
  {
    if( !_itemw.get() )
    {
      DefaultItemWriter<T>      diw;
      _itemw.reset( diw.writer( ImageWriter<T>::_binary, 
                                ImageWriter<T>::_byteswap ) );
    }
    unsigned long nitems = len / sizeof( T );
    return _itemw->write( *source(), (const T *) data, nitems ) * sizeof( T );
  }
  
  
  template <typename T> 
  int GisImageWriter<T>::getch()
  {
    return source()->getch();
  }
  
  template <typename T> 
  int GisImageWriter<T>::putch( int ch )
  {
    return source()->putch( ch );
  }
  
  template <typename T> 
  bool GisImageWriter<T>::ungetch( int ch )
  {
    return source()->ungetch( ch );
  }
  
  template <typename T> 
  bool GisImageWriter<T>::allowsMemoryMapping() const
  {
    return ImageWriter<T>::_binary && !ImageWriter<T>::_byteswap;
    
  }
  
  template <typename T> 
  bool GisImageWriter<T>::setpos( int x, int y, int z, int t )
  {
    offset_t  lin = (offset_t) ImageWriter<T>::_sizes[ 0 ][ 0 ];
    offset_t  sli = lin * ImageWriter<T>::_sizes[ 0 ][ 1 ];
    offset_t  vol = sli * ImageWriter<T>::_sizes[ 0 ][ 2 ];
    return source()->at( ( (offset_t) sizeof(T) ) * 
                         ( x + y * lin + z * sli + t * vol ) );
  }
  
  //============================================================================
  //   I M A G E R E A D E R   M E T H O D S
  //============================================================================
  template <typename T>
  void GisImageWriter<T>::write( T * source, DataSourceInfo & dsi,
                                 std::vector<int> & pos,  /* size 4 : x,y,z,t */
                                 std::vector<int> & size, /* size 4 : x,y,z,t */
                                 int /* level */, 
                                 carto::Object options )
  {
    if( ImageWriter<T>::_sizes.empty() )
      updateParams( dsi );
    
    // total volume size
    int  sx = ImageWriter<T>::_sizes[ 0 ][ 0 ];
    int  sy = ImageWriter<T>::_sizes[ 0 ][ 1 ];
    int  sz = ImageWriter<T>::_sizes[ 0 ][ 2 ];
    int  st = ImageWriter<T>::_sizes[ 0 ][ 3 ];
    #ifdef SOMAIO_GIW_DEBUG
      std::cout << "GIW::s : " << sx << " " << sy << " " 
                << sz << " " << st << std::endl;
    #endif
    // region size
    int  vx = size[ 0 ];
    int  vy = size[ 1 ];
    int  vz = size[ 2 ];
    int  vt = size[ 3 ];
    #ifdef SOMAIO_GIW_DEBUG
      std::cout << "GIW::v : " << vx << " " << vy << " " 
                << vz << " " << vt << std::endl;
    #endif
    // region position
    int  ox = pos[ 0 ];
    int  oy = pos[ 1 ];
    int  oz = pos[ 2 ];
    int  ot = pos[ 3 ];
    #ifdef SOMAIO_GIW_DEBUG
      std::cout << "GIW::o : " << ox << " " << oy << " " 
                << oz << " " << ot << std::endl;
    #endif
    int  y, z, t;
    // region line size
    offset_t  len = vx * sizeof( T );
    
    if( options->hasProperty( "partial_writing" ) && !open( DataSource::ReadWrite ) )
      throw carto::open_error( "data source not available", url() );
    else if( !open( DataSource::Write ) )
      throw carto::open_error( "data source not available", url() );
    
    for( t=0; t<vt; ++t ) {
      for( z=0; z<vz; ++z ) {
        for( y=0; y<vy; ++y ) {
          // we move in the file
          at( ( sx * ( sy * ( sz * ( t + ot ) + z + oz ) 
                            + y + oy ) + ox ) * sizeof(T) );
          // we move in the buffer
          char * target = (char *) source + len * ( vy * ( vz * t + z ) + y );
          if( writeBlock( target, len ) != (long) len )
            throw carto::eof_error( url() );
        }
      }
    }
  }
  
  template <typename T>
  DataSourceInfo GisImageWriter<T>::writeHeader( DataSourceInfo dsi, 
                                                 carto::Object options )
  {
    //--- build datasourcelist -------------------------------------------------
    #ifdef SOMAIO_GIW_DEBUG
      std::cout << "GIW:: building DataSourceList..." << std::endl;
    #endif
    bool dolist = dsi.list().nbTypes() == 1 ;
    if( dolist )
      buildDSList( dsi.list(), options );
    #ifdef SOMAIO_GIW_DEBUG
      std::cout << "GIW:: done: building DataSourceList" << std::endl;
    #endif
    //--- set header -----------------------------------------------------------
    #ifdef SOMAIO_GIW_DEBUG
      std::cout << "GIW:: setting Header..." << std::endl;
    #endif
    if( options->hasProperty( "partial_writing" ) ) {
      DataSourceInfoLoader  dsil;
      DataSourceInfo dimdsi( dsi.list().dataSource( "dim", 0 ) );
      dimdsi = dsil.check( dimdsi );
      dsi.header()->setProperty( "byte_swapping", dimdsi.header()->getProperty( "byte_swapping" ) );
      dsi.header()->setProperty( "ascii", dimdsi.header()->getProperty( "ascii" ) );
      return dsi;
    } else {
      if( options->hasProperty( "byte_swapping" ) )
        dsi.header()->setProperty( "byte_swapping", options->getProperty( "byte_swapping" ) );
      else
        dsi.header()->setProperty( "byte_swapping", false );
      if( options->hasProperty( "ascii" ) )
        dsi.header()->setProperty( "ascii", options->getProperty( "ascii" ) );
      else
        dsi.header()->setProperty( "ascii", false );
    }
    #ifdef SOMAIO_GIW_DEBUG
      std::cout << "GIW:: done:setting Header" << std::endl;
    #endif
    //--- write header ---------------------------------------------------------
    #ifdef SOMAIO_GIW_DEBUG
      std::cout << "GIW:: writing Header..." << std::endl;
    #endif
    ChainDataSource::setSource( dsi.list().dataSource( "dim", 0 ), 
                                dsi.list().dataSource( "dim", 0)->url() );
    DataSource* ds;
    ds = dsi.list().dataSource( "dim", 0 ).get();
    if( !open( DataSource::Write ) )
      throw carto::open_error( "data source not available", url() );
    // reading volume size
    std::vector<int> dim( 4, 0 );
    dsi.header()->getProperty( "sizeX", dim[0] );
    dsi.header()->getProperty( "sizeY", dim[1] );
    dsi.header()->getProperty( "sizeZ", dim[2] );
    dsi.header()->getProperty( "sizeT", dim[3] );
    std::vector<float> vs( 4, 0 );
    // reading voxel size
    vs[0] = dsi.header()->getProperty( "voxel_size")
                        ->getArrayItem(0)->getScalar();
    vs[1] = dsi.header()->getProperty( "voxel_size")
                        ->getArrayItem(1)->getScalar();
    vs[2] = dsi.header()->getProperty( "voxel_size")
                        ->getArrayItem(2)->getScalar();
    vs[3] = dsi.header()->getProperty( "voxel_size")
                        ->getArrayItem(3)->getScalar();
    // header :: volume dimensions
    *ds << dim[0] << " " << dim[1] << " " 
        << dim[2] << " " << dim[3] << "\n";
    // header :: data type
    *ds << "-type " << carto::DataTypeCode<T>::dataType() << "\n";
    // header :: voxel size
    *ds << "-dx " << vs[0] << " -dy " << vs[1] 
        << " -dz " << vs[2] << " -dt " << vs[3] << "\n";
    // header :: byte ordering
    *ds << "-bo ";
    uint magicNumber = SOMAIO_BYTE_ORDER;
    try {
      if( options->getProperty( "byte_swapping" )->getScalar() ) {
        uint hhbyte = ( magicNumber & 0xff000000 ) >> 24;
        uint hbyte = ( magicNumber & 0x00ff0000 ) >> 16;
        uint lbyte = ( magicNumber & 0x0000ff00 ) >> 8;
        uint llbyte = ( magicNumber & 0x000000ff);
        magicNumber = llbyte << 24 | lbyte << 16 | hbyte << 8 | hhbyte;
      }
    } catch ( ... ) {
    }
    ds->writeBlock( (char *) &magicNumber , sizeof(uint) );
    *ds << "\n";
    //header :: opening mode
    *ds << "-om " 
        << ( dsi.header()->getProperty( "ascii" )->getScalar() ? "ascii" : "binar" )
        << "\n";
    close();
    #ifdef SOMAIO_GIW_DEBUG
      std::cout << "GIW:: done: writing Header" << std::endl;
    #endif
    //--- write minf -----------------------------------------------------------
    #ifdef SOMAIO_GIW_DEBUG
      std::cout << "GIW:: writing Minf..." << std::endl;
    #endif
    carto::Object minf = carto::Object::value( carto::PropertySet() );
    minf->setProperty( "file_type", std::string( "GIS" ) );
    minf->setProperty( "data_type", carto::DataTypeCode<T>::dataType() );
    minf->setProperty( "object_type", std::string( "Volume" ) );
    std::vector<int> dims( 4, 0 );
    dsi.header()->getProperty( "sizeX", dims[ 0 ] );
    dsi.header()->getProperty( "sizeY", dims[ 1 ] );
    dsi.header()->getProperty( "sizeZ", dims[ 2 ] );
    dsi.header()->getProperty( "sizeT", dims[ 3 ] );
    minf->setProperty( "volume_dimension", dims );
    minf->setProperty( "voxel_size", 
                       dsi.header()->getProperty( "voxel_size" ) );

    Writer<carto::GenericObject> minfw( dsi.list().dataSource( "minf", 0 ) );
    minfw.write( *minf );
    #ifdef SOMAIO_GIW_DEBUG
      std::cout << "GIW:: done: writing Minf" << std::endl;
    #endif
    //--- partial-io case ------------------------------------------------------
    if( options->hasProperty( "unallocated" ) ) {
      #ifdef SOMAIO_GIW_DEBUG
        std::cout << "GIW:: building file for partial writing..." << std::endl;
      #endif
      if( ImageWriter<T>::_sizes.empty() )
        updateParams( dsi );
      ChainDataSource::setSource( dsi.list().dataSource( "ima", 0 ), 
                                  dsi.list().dataSource( "ima", 0)->url() );
      if( !open( DataSource::Write ) )
        throw carto::open_error( "data source not available", url() );
      T value[1] = {0};
      at( (dim[0]*dim[1]*dim[2]*dim[3]-1)*sizeof(T) );
      if( writeBlock( (char * ) value, sizeof(T) ) != (long) sizeof(T) )
            throw carto::eof_error( url() );
      #ifdef SOMAIO_GIW_DEBUG
        std::cout << "GIW:: done:building file for partial writing" << std::endl;
      #endif
    }
      
    //--------------------------------------------------------------------------
    return dsi;
  }
  
  //============================================================================
  //   U T I L I T I E S
  //============================================================================
  template <typename T>
  void GisImageWriter<T>::buildDSList( DataSourceList & dsl, 
                                       carto::Object /*options*/ ) const
  {
    DataSource* pds = dsl.dataSource( "default", 0 ).get();
    std::string dimname, imaname, minfname;
    
    dimname = imaname = minfname = pds->url();
    if( dimname.empty() ) {
      throw carto::file_not_found_error( "needs a filename", dimname );
    } else {
      std::string ext = carto::FileUtil::extension( dimname );
      std::string basename = carto::FileUtil::removeExtension( dimname );
      
      if( ext == "ima" ) {
        dimname = basename + ".dim";
      } else if( ext == "dim" ) {
        imaname = basename + ".ima";
      } else {
        ext.clear();
      }
      
      if( ext.empty() ) {
        basename = dimname;
        dimname += ".dim";
        imaname += ".ima";
      }
      minfname = imaname + ".minf";
      
      if( dimname == pds->url() ) {
        // if dimname is original url
        dsl.addDataSource( "dim", carto::rc_ptr<DataSource>( pds ) );
      } else {
        dsl.addDataSource( "dim", carto::rc_ptr<DataSource>
                                  ( new FileDataSource( dimname ) ) );
      }
      if( imaname == pds->url() ) {
        // if imaname is original url
        dsl.addDataSource( "ima", carto::rc_ptr<DataSource>( pds ) );
      } else {
        dsl.addDataSource( "ima", carto::rc_ptr<DataSource>
                                  ( new FileDataSource( imaname ) ) );
      }
    }
    
    dsl.addDataSource( "minf", carto::rc_ptr<DataSource>
                               ( new FileDataSource( minfname ) ) );
    
  }
}

#endif