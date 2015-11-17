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

#ifndef AIMS_IO_JPEGR_H
#define AIMS_IO_JPEGR_H

#include <aims/data/data.h>
#include <aims/io/jpegheader.h>
#include <aims/io/datatypecode.h>
#include <cartobase/exception/file.h>
#include <cartobase/exception/format.h>
#include <cartobase/stream/fileutil.h>
#include <soma-io/datasource/filedatasource.h>
#include <stdio.h>
extern "C"
{
#include <jpeglib.h>
}


namespace aims
{

  template<class T>
  class JpegReader
  {
  public:
    JpegReader( const std::string& name ) : _name( name ) {}
    ~JpegReader() {}

    void read( AimsData<T>& thing, const carto::AllocatorContext & context, 
               carto::Object options );
    /**	called by read(), but you can call it for single frame reading 
	(axial slice) */
    void readFrame( AimsData<T> & thing, const std::string & filename, 
                    unsigned zfame, unsigned tframe );

  private:
    std::string		_name;
  };


  template <class T>
  inline JpegReader<T> & 
  operator >> ( JpegReader<T> & reader, AimsData<T> & thing )
  {
    reader.read( thing );
    return reader;
  }


  template <class T>
  inline
  void JpegReader<T>::read( AimsData<T>& thing, 
                            const carto::AllocatorContext & context, 
                            carto::Object options )
  {
    JpegHeader *hdr = new JpegHeader( _name );
    try
      {
	hdr->read();
      }
    catch( std::exception & e )
      {
	delete hdr;
	throw;
      }

    int	frame = -1, border = 0;
    options->getProperty( "frame", frame );
    options->getProperty( "border", border );

    std::vector<std::string>	files = hdr->inputFilenames();
    std::vector<int>		dims;
    hdr->getProperty( "volume_dimensions", dims );

    if( dims.size() < 1 )
      dims.push_back( hdr->dimX() );
    if( dims.size() < 2 )
      dims.push_back( hdr->dimY() );
    if( dims.size() < 3 )
      dims.push_back( hdr->dimZ() );
    if( dims.size() < 4 )
      dims.push_back( hdr->dimT() );

    unsigned	tmin = 0, tmax = dims[3] - 1;
    if( frame >= 0 )
      {
        if( tmax < (unsigned) frame )
          {
            delete hdr;
            throw std::domain_error( "frame higher than file dimT" );
          }
        if( (unsigned) frame < tmax )
          files.erase( files.begin() + ( frame + 1 ) * hdr->dimZ(), 
                       files.end() );
        if( frame > 0 )
          files.erase( files.begin(), files.begin() + frame * hdr->dimZ() );
        tmin = frame;
        tmax = frame;
      }

    carto::AllocatorContext 
      cont2( context.accessMode(), 
             carto::rc_ptr<carto::DataSource>
             ( new carto::FileDataSource
               ( *files.begin(), 0, carto::DataSource::Read ) ), 
             false, context.useFactor() );

    AimsData<T> data( hdr->dimX(), hdr->dimY(), hdr->dimZ(), 
                      tmax - tmin + 1, border, cont2 );
    data.setSizeX( hdr->sizeX() );
    data.setSizeY( hdr->sizeY() );
    data.setSizeZ( hdr->sizeZ() );
    data.setSizeT( hdr->sizeT() );

    dims[3] = tmax - tmin + 1;
    hdr->setProperty( "volume_dimension", dims );

    //	force data type into header
    carto::DataTypeCode<T>	dtc;
    hdr->setType( dtc.dataType() );
    std::string		dir = carto::FileUtil::dirname( _name );
    if( !dir.empty() )
      dir += carto::FileUtil::separator();

    unsigned	i = 0, s, t, ns = (unsigned) data.dimZ(), nt = tmax - tmin + 1;
    for( t=0; t<nt; ++t )
      for( s=0; s<ns; ++s, ++i )
        readFrame( data, dir + files[i], s, t );

    thing = data;
    if( hdr->hasProperty( "filenames" ) )
      hdr->removeProperty( "filenames" );
    thing.setHeader( hdr );
  }

  template<class T>
  inline
  void JpegReader<T>::readFrame( AimsData<T> & data, 
				 const std::string & name, unsigned z, 
				 unsigned t )
  {
    struct jpeg_decompress_struct	cinfo;
    struct jpeg_error_mgr		jerr;
    FILE				*fp;
    unsigned				i;
    JSAMPROW				row_pointer[1];

    cinfo.err = jpeg_std_error( &jerr );
    jpeg_create_decompress( &cinfo );

    fp = fopen( name.c_str(), "rb" );
    if( !fp )
      throw carto::file_error( name );

    jpeg_stdio_src( &cinfo, fp );
    if( jpeg_read_header( &cinfo, true ) != 1 )
      throw carto::format_error( name );
    else
      {
	carto::DataTypeCode<T>	dtc;
	if( dtc.dataType() == "RGB" )
	  {
	    cinfo.out_color_space = JCS_RGB;
	    cinfo.out_color_components = 3;
	  }
	else
	  {
	    cinfo.out_color_space = JCS_GRAYSCALE;
	    cinfo.out_color_components = 1;
	  }
	jpeg_start_decompress( &cinfo );
	for( i=0; i<cinfo.image_height; ++i )
	  {
	    row_pointer[0] = (JSAMPROW) &data( 0, i, z, t );
	    jpeg_read_scanlines( &cinfo, row_pointer, 1 );
	  }
      }

    jpeg_finish_decompress( &cinfo );
    fclose( fp );
    jpeg_destroy_decompress( &cinfo );
  }

}

#endif
