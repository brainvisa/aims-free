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

#ifndef AIMS_IO_SOMA_FSANNOTFORMATREADER_D_H
#define AIMS_IO_SOMA_FSANNOTFORMATREADER_D_H

//-------------------------------------------------------------------
#include <aims/io_soma/fsannotformatreader.h>
#include <aims/mesh/texture.h>
#include <soma-io/datasource/datasource.h>
#include <soma-io/datasourceinfo/datasourceinfoloader.h>
#include <soma-io/reader/itemreader.h>
//--- debug ------------------------------------------------------------------
#include <cartobase/config/verbose.h>
#define localMsg( message ) cartoCondMsg( 4, message, "FSANNOTFORMATREADER" )
// localMsg must be undef at end of file
//----------------------------------------------------------------------------


// from http://www.grahamwideman.com/gw/brain/fs/surfacefileformats.htm

namespace soma
{

  template <typename T>
  TimeTexture<T>*
  FsAnnotFormatReader<T>::createAndRead( rc_ptr<DataSourceInfo> dsi,
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
  void FsAnnotFormatReader<T>::read( TimeTexture<T> & obj,
                                     rc_ptr<DataSourceInfo> dsi,
                                     const AllocatorContext & /* context */,
                                     Object options )
  {
    // std::cout << "FsAnnotFormatReader::read " << dsi->list().dataSource()->url() << std::endl;
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

    // std::cout << "read FSANNOT OK\n";
  }


  template <typename T>
  FormatReader<TimeTexture<T> >*
    FsAnnotFormatReader<T>::clone() const
  {
    return new FsAnnotFormatReader;
  }


  template <typename T>
  bool FsAnnotFormatReader<T>::readTexture( carto::rc_ptr<DataSource> ds,
                                            TimeTexture<T> & obj,
                                            int nv, bool ascii,
                                            bool bswap ) const
  {
    // std::cout << "readTexture Annot: " << nv << ", " << ascii << ", " << bswap << std::endl;

    DefaultItemReader<uint32_t> dir;
    std::unique_ptr<ItemReader<uint32_t> > itemr;
    itemr.reset( dir.reader( !ascii, bswap ) );

    std::vector<T> & tex = obj[0].data();
    tex.resize( nv );

    std::vector<uint32_t> data( nv * 2 );
    long n = itemr->read( *ds, &data[0], nv * 2 );
    if( n != nv * 2 )
    {
      std::cout << "not read the right size: " << n << " instead of expected " << nv * 2 << std::endl;
      return false;
    }

    if( !ds->isOpen() || ds->eof() )
      return false;  // no color table ?

    std::vector<uint32_t> v( 4, 0 );
    n = itemr->read( *ds, &v[0], 4 );
    if( n < 4 )
    {
      std::cout << "EOF\n";
      return false;
    }
    if( v[0] != 1 || v[1] != 0xfffffffe )
    {
      std::cout << "unexpected values after data table: " << v[0] << ", " << v[1] << ", " << v[2] << ", " << v[3] << std::endl;
      return false;
    }
    if( !ds->isOpen() || ds->eof() )
      return false;  // no color table ?

    std::string line;
    bool r = StreamUtil::getline( *ds, line );
    if( !r )
    {
      std::cout << "EOF\n";
      return false;
    }
    obj.header().setProperty( "colormap_filename", line );
    if( line.length() + 1 != v[3] )
      std::cout << "colortable filename string length mismatch\n";
    n = itemr->read( *ds, &v[0], 1 );
    if( n < 1 )
    {
      std::cout << "EOF\n";
      return false;
    }
    // std::cout << "cmap values read: " << v[0] << std::endl;
    unsigned ncmap = v[0];

    carto::Object cmap = Object::value( std::map<int, carto::Object>() );
    obj.header().setProperty( "GIFTI_labels_table", cmap );
    std::map<uint32_t, T> rev_cols;

    int i = 0;
    T lindex = 0;
    while( ds->isOpen() && !ds->eof() )
    {
      n = itemr->read( *ds, &v[0], 2 );
      if( n != 2 )
      {
        std::cout << "EOF in cmap reading\n";
        return false;
      }
      r = StreamUtil::getline( *ds, line );
      if( !r )
      {
        std::cout << "EOF in cmap label reading\n";
        return false;
      }
      if( line.length() + 1 != v[1] )
        std::cout << "string length mismatch in cmap label: " << line
          << std::endl;
      carto::Object cmitem = Object::value( carto::Dictionary() );
      cmitem->setProperty( "Label", line );
      lindex = T( v[0] );
      cmap->setArrayItem( v[0], cmitem );
      n = itemr->read( *ds, &v[0], 4 );
      if( n < 4 )
      {
        std::cout << "EOF in cmap colors reading\n";
        return false;
      }
      std::vector<float> col( 4, 1. );
      col[0] = float( v[0] ) / 255.;
      col[1] = float( v[1] ) / 255.;
      col[2] = float( v[2] ) / 255.;
      col[3] = float( 255 - v[3] ) / 255.;
      cmitem->setProperty( "RGB", col );

      uint32_t rgb = v[2] * 0x10000 + v[1] * 0x100 + v[0];
      rev_cols[rgb] = lindex;
      ++i;
    }
    if( i != ncmap )
      std::cout << "wrong cmap entries found: " << i << " items, expected: "
        << ncmap << std::endl;

    uint32_t index, value;
    typename std::map<uint32_t, T>::const_iterator ic, ec = rev_cols.end();
    for( i=0; i<nv; ++i )
    {
      index = data[i * 2 ];
      if( index >= nv )
      {
        std::cout << "wrong index at: " << i << ": " << index << std::endl;
        return false;
      }
      value = data[ i * 2 + 1 ];

      ic = rev_cols.find( value );
      if( ic != ec )
        tex[i] = ic->second;
    }

    return true;
  }


}

#undef localMsg

#endif
