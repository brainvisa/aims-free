#ifndef AIMS_IO_SOMA_FSCURVFORMATREADER_D_H
#define AIMS_IO_SOMA_FSCURVFORMATREADER_D_H

//-------------------------------------------------------------------
#include <aims/io_soma/fscurvformatreader.h>
#include <aims/mesh/texture.h>
#include <soma-io/datasource/datasource.h>
#include <soma-io/datasourceinfo/datasourceinfoloader.h>
#include <soma-io/reader/itemreader.h>
//--- debug ------------------------------------------------------------------
#include <cartobase/config/verbose.h>
#define localMsg( message ) cartoCondMsg( 4, message, "FSCURVFORMATREADER" )
// localMsg must be undef at end of file
//----------------------------------------------------------------------------


// from http://www.grahamwideman.com/gw/brain/fs/surfacefileformats.htm

namespace soma
{

  template <typename T>
  TimeTexture<T>*
  FsCurvFormatReader<T>::createAndRead( rc_ptr<DataSourceInfo> dsi,
                                        const AllocatorContext & context,
                                        Object options )
  {
    carto::rc_ptr<soma::DataSource> ds = dsi->list().dataSource();
    localMsg( "createAndRead " + ds->url() );
    TimeTexture<T> *mesh = new TimeTexture<T>;
    try
    {
      read( *mesh, dsi, context, options );
    }
    catch( ... )
    {
      delete mesh;
      throw;
    }
    mesh->header().copyProperties( dsi->header() );
    return mesh;
  }


  template <typename T>
  void FsCurvFormatReader<T>::read( TimeTexture<T> & obj,
                                    rc_ptr<DataSourceInfo> dsi,
                                    const AllocatorContext & /* context */,
                                    Object options )
  {
    // std::cout << "FsCurvFormatReader::read " << dsi->list().dataSource()->url() << std::endl;
    carto::rc_ptr<DataSource> ds = dsi->list().dataSource();

    if( !ds->isOpen() )
    {
      std::cout << "WARNING: reopening freesurfer curv file at beginning\n";
      ds->open( DataSource::Read );  // FIXME recover offset
    }
    if( !ds->isOpen() )
      throw file_not_found_error( ds->url() );

    carto::Object hdr = dsi->header();

    int asciii = 0;
    int bswapi = 0;
    hdr->getProperty( "ascii", asciii );
    bool ascii = bool( asciii );
    hdr->getProperty( "byte_swapping", bswapi );
    bool bswap = bool( bswapi );
    int nv = 0, np = 0, ps;
    hdr->getProperty( "vertex_number", nv );

    if( !readTexture( ds, obj, nv, ascii, bswap ) )
      throw carto::invalid_format_error( ds->url() );


    hdr = carto::Object::value( carto::PropertySet() );
    rc_ptr<DataSource> mds( 0 );
    try
    {
      mds = dsi->list().dataSource( "minf" );
      if( mds.get() )
        DataSourceInfoLoader::readMinf( *mds, hdr, options );
    }
    catch( ... )
    {
    }

    obj.header().copyProperties( hdr );
    if( dsi->header().get() )
      obj.header().copyProperties( dsi->header() );

    // std::cout << "read FSCURV OK\n";
  }


  template <typename T>
  FormatReader<TimeTexture<T> >*
    FsCurvFormatReader<T>::clone() const
  {
    return new FsCurvFormatReader;
  }


  template <typename T>
  bool FsCurvFormatReader<T>::readTexture( carto::rc_ptr<DataSource> ds,
                                           TimeTexture<T> & obj,
                                           int nv, bool ascii,
                                           bool bswap ) const
  {
    // std::cout << "readVTexture: " << nv << ", " << ascii << ", " << bswap << std::endl;

    DefaultItemReader<T> dir;
    std::unique_ptr<ItemReader<T> > itemr;
    itemr.reset( dir.reader( !ascii, bswap ) );

    std::vector<T> & tex = obj[0].data();
    tex.resize( nv );

    long n = itemr->read( *ds, &tex[0], nv );
    if( n != nv )
      return false;

    return true;
  }


}

#undef localMsg

#endif
