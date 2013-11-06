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

#ifndef AIMS_IO_GISR_H
#define AIMS_IO_GISR_H

#include <aims/config/aimsdata_config.h>
#include <aims/io/defaultItemR.h>
#include <aims/io/gisheader.h>
#include <aims/io/datatypecode.h>
#include <aims/data/data.h>
#include <cartobase/exception/file.h>
#include <cartobase/exception/format.h>
#include <soma-io/datasource/filedatasource.h>
#include <soma-io/allocator/mappingcopy.h>
#include <soma-io/allocator/mappingro.h>
#include <soma-io/allocator/mappingrw.h>
// protection against MacOS macros
#ifdef MAP_COPY
#undef MAP_COPY
#endif

namespace aims
{

  template<typename T>
  class GisReader
  {
  public:
    GisReader( const std::string& name, ItemReader<T>* ir = 0 ) 
      : _name( name ), _itemr( ir )
    {}
    ~GisReader() { delete _itemr; }

    void read( AimsData<T>& thing, const carto::AllocatorContext & context, 
               carto::Object options );
    void setItemReader( ItemReader<T>* ir )
      { delete _itemr; _itemr = ir; }

  private:
    std::string   _name;
    ItemReader<T> *_itemr;
  };


  template <class T>
  inline AIMSDATA_API GisReader<T> & 
  operator >> ( GisReader<T> & reader, AimsData<T> & thing )
  {
    reader.read( thing );
    return reader;
  }


  template <class T>
  inline
  void GisReader<T>::read( AimsData<T>& thing, 
                           const carto::AllocatorContext & context, 
                           carto::Object options )
  {
    GisHeader	*hdr = new GisHeader( _name );
    try
    {
      hdr->read();
    }
    catch( std::exception & e )
    {
      delete hdr;
      throw;
    }

    carto::DataTypeCode<T>	dtc;
    if( hdr->dataType() != dtc.dataType() )
      throw carto::datatype_format_error
        ( std::string( "Wrong data type / reader type : file is " )
          + hdr->dataType() + ", expecting " + dtc.dataType(), _name );

    int	frame = -1, border = 0;
    options->getProperty( "frame", frame );
    options->getProperty( "border", border );

    int	dimt, tmin;
    if( frame >= 0 )
    {
      if( frame >= hdr->dimT() )
      {
        delete hdr;
        throw carto::invalid_format_error( "frame higher than file dimT",
                                            _name );
      }
      dimt = 1;
      tmin = frame;
    }
    else
    {
      dimt = hdr->dimT();
      tmin = 0;
    }

    std::string name = hdr->removeExtension( _name ) + ".ima";

    carto::AllocatorContext 
      cont2( context.accessMode(), 
             carto::rc_ptr<carto::DataSource>
             ( new carto::FileDataSource
               ( name, 0, 
                 context.accessMode() == carto::AllocatorStrategy::ReadWrite ? 
                 carto::DataSource::ReadWrite : carto::DataSource::Read ) ), 
             carto::AllocatorStrategy::isMMapCompatible
             ( hdr->openMode() != "binar", hdr->byteOrder(), border ), 
             context.useFactor() );

    AimsData<T> data( hdr->dimX(), hdr->dimY(), hdr->dimZ(), dimt, 
                      border, cont2 );

    data.setSizeX( hdr->sizeX() );
    data.setSizeY( hdr->sizeY() );
    data.setSizeZ( hdr->sizeZ() );
    data.setSizeT( hdr->sizeT() );

    carto::AllocatorStrategy::MappingMode 
      mode = data.allocator().allocatorType();
    // if there is a border, the allocated volume is the "parent" volume.
    if( mode == carto::AllocatorStrategy::Unallocated
        && !data.volume()->refVolume().isNull() )
      mode = data.volume()->refVolume()->allocatorContext().allocatorType();

    if( mode == carto::AllocatorStrategy::Memory 
        || mode == carto::AllocatorStrategy::CopyMap 
        || mode == carto::AllocatorStrategy::MAP )
    {
      std::ios::openmode omd = std::ios::in;
      if( hdr->openMode() == "binar" )
        omd |= std::ios::binary;
      std::ifstream	is( name.c_str(), omd );
      if( !is )
        carto::io_error::launchErrnoExcept( name );
      if( hdr->openMode() == "binar" )
        is.unsetf( std::ios::skipws );

      if( !_itemr )
        _itemr = new DefaultItemReader<T>;
      ItemReader<T>	*ir
        = _itemr->reader( hdr->openMode(),
                          hdr->byteOrder() != AIMS_MAGIC_NUMBER );

      //	skip volumes to the first we want to read
      is.seekg( ((off_t) tmin) * data.dimZ() * data.dimY() * data.dimX()
                * sizeof( T ), std::ios::cur);
      if( !is || is.eof() )
        carto::io_error::launchErrnoExcept( name );

      for( int t=0; t<dimt; ++t )
        for( int z=0; z<data.dimZ(); ++z )
          for( int y=0; y<data.dimY(); ++y )
          {
            ir->read( is, &data(0,y,z,t), data.dimX() );
            if( !is || is.eof() )
              carto::io_error::launchErrnoExcept( name );
          }

      delete ir;
    }

    thing = data;
    thing.setHeader( hdr );
  }


}

#endif
