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

#ifndef AIMS_IO_SOMA_FSSURFFORMATREADER_D_H
#define AIMS_IO_SOMA_FSSURFFORMATREADER_D_H

//-------------------------------------------------------------------
#include <aims/io_soma/fssurfformatreader.h>
#include <aims/mesh/surface.h>
#include <soma-io/datasource/datasource.h>
#include <soma-io/datasourceinfo/datasourceinfoloader.h>
#include <soma-io/reader/itemreader.h>
//--- debug ------------------------------------------------------------------
#include <cartobase/config/verbose.h>
#define localMsg( message ) cartoCondMsg( 4, message, "FSSURFFORMATREADER" )
// localMsg must be undef at end of file
//----------------------------------------------------------------------------


// from http://www.grahamwideman.com/gw/brain/fs/surfacefileformats.htm

namespace soma
{

  template <int D>
  AimsTimeSurface<D, Void>*
  FsSurfFormatReader<D>::createAndRead( rc_ptr<DataSourceInfo> dsi,
                                        const AllocatorContext & context,
                                        Object options )
  {
    carto::rc_ptr<soma::DataSource> ds = dsi->list().dataSource();
    localMsg( "createAndRead " + ds->url() );
    AimsTimeSurface<D, Void> *mesh = new AimsTimeSurface<D, Void>;
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


  template <int D>
  void FsSurfFormatReader<D>::read( AimsTimeSurface<D, Void> & obj,
                                    rc_ptr<DataSourceInfo> dsi,
                                    const AllocatorContext & /* context */,
                                    Object options )
  {
    // std::cout << "FsSurfFormatReader::read " << dsi->list().dataSource()->url() << std::endl;
    carto::rc_ptr<DataSource> ds = dsi->list().dataSource();

    if( !ds->isOpen() )
    {
      std::cout << "WARNING: reopening freesurfer surf file at beginning\n";
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
    hdr->getProperty( "polygon_number", np );
    // hdr->getProperty( "polygon_dimension", ps );

    if( !readVertices( ds, obj, nv, ascii, bswap ) )
      throw carto::invalid_format_error( ds->url() );
    if( !readPolygons( ds, obj, np, ascii, bswap ) )
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

    // std::cout << "read FSSURF OK\n";
  }


  template <int D>
  FormatReader<AimsTimeSurface<D, Void> >*
    FsSurfFormatReader<D>::clone() const
  {
    return new FsSurfFormatReader;
  }


  template <int D>
  bool FsSurfFormatReader<D>::readVertices( carto::rc_ptr<DataSource> ds,
                                            AimsTimeSurface<D, Void> & obj,
                                            int nv, bool ascii,
                                            bool bswap ) const
  {
    // std::cout << "readVertices: " << nv << ", " << ascii << ", " << bswap << std::endl;

    DefaultItemReader<float> dir;
    std::unique_ptr<ItemReader<float> > itemr;
    itemr.reset( dir.reader( !ascii, bswap ) );

    std::vector<Point3df> & vert = obj.vertex();
    vert.resize( nv );

    long n = itemr->read( *ds, (float *) &vert[0], nv * 3 );
    if( n != nv * 3 )
      return false;

    return true;
  }


  template <int D>
  bool FsSurfFormatReader<D>::readPolygons( carto::rc_ptr<DataSource> ds,
                                            AimsTimeSurface<D, Void> & obj,
                                            int np, bool ascii,
                                            bool bswap ) const
  {
    // std::cout << "readPolygons: " << np << ", " << ascii << ", " << bswap << std::endl;

    DefaultItemReader<uint32_t> dir;
    std::unique_ptr<ItemReader<uint32_t> > itemr;
    itemr.reset( dir.reader( !ascii, bswap ) );

    std::vector<AimsVector<uint32_t, D> > & poly = obj.polygon();
    poly.resize( np );

    long n = itemr->read( *ds, (uint32_t *) &poly[0], np * D );
    if( n != np * D )
      return false;

    return true;
  }

}

#undef localMsg

#endif
