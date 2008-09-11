/* Copyright (c) 1995-2005 CEA
 *
 *  This software and supporting documentation were developed by
 *      CEA/DSV/SHFJ
 *      4 place du General Leclerc
 *      91401 Orsay cedex
 *      France
 *
 * This software is governed by the CeCILL license version 2 under 
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the 
 * terms of the CeCILL license version 2 as circulated by CEA, CNRS
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
 * knowledge of the CeCILL license version 2 and that you accept its terms.
 */

#ifndef AIMS_IO_JPEGW_H
#define AIMS_IO_JPEGW_H

#include <aims/io/jpegheader.h>
#include <aims/io/datatypecode.h>
#include <aims/data/pheader.h>
#include <aims/data/data.h>
#include <cartobase/stream/fileutil.h>
#include <cartobase/stream/sstream.h>
#include <cartobase/exception/file.h>
#include <iomanip>
#include <stdio.h>
extern "C"
{
#include <jpeglib.h>
}


namespace aims
{

  template<class T>
  class JpegWriter
  {
  public:
    JpegWriter( const std::string& name ) : _name( name ) {}
    ~JpegWriter() {}

    void write( const AimsData<T> & thing );
    /**	called by write(), but you can call it for single frame writing 
	(axial slice) */
    void writeFrame( const AimsData<T> & thing, const std::string & filename, 
		     unsigned zfame, unsigned tframe );
    /// Return a name without .jpg extension
    std::string removeExtension( const std::string& name ) const;

  private:
    std::string		_name;
  };

  template <class T>
  inline JpegWriter<T> & 
  operator << ( JpegWriter<T> & writer, const AimsData<T> & thing )
  {
    writer.write( thing );
    return writer;
  }


  template <class T>
  inline
  void JpegWriter<T>::write( const AimsData<T>& thing )
  {
    unsigned	t, z, dt = thing.dimT(), dz = thing.dimZ();
    JpegHeader	hdr( _name, carto::DataTypeCode<T>().dataType(), thing.dimX(), 
		     thing.dimY(), thing.dimZ(), thing.dimT(), thing.sizeX(), 
		     thing.sizeY(), thing.sizeZ(), thing.sizeT() );
    const PythonHeader 
      *ph = dynamic_cast<const PythonHeader *>( thing.header() );
    if( ph )
      hdr.copy( *ph );

    std::string			dir = carto::FileUtil::dirname( _name );
    std::vector<std::string>	files = hdr.outputFilenames();

    hdr.setProperty( "file_type", std::string( "JPEG" ) );
    hdr.setProperty( "object_type", carto::DataTypeCode<T>().objectType() );
    hdr.setProperty( "data_type", carto::DataTypeCode<T>().dataType() );
    hdr.setProperty( "filenames", files );
    if( !dir.empty() )
      dir += carto::FileUtil::separator();

    unsigned	i = 0;
    for( t=0; t<dt; ++t )
      for( z=0; z<dz; ++z, ++i )
        writeFrame( thing, dir + files[i], z, t );

    hdr.writeMinf( dir + carto::FileUtil::removeExtension( files[0] ) 
                   + hdr.extension() + ".minf" );
  }

  template<class T>
  inline
  void JpegWriter<T>::writeFrame( const AimsData<T> & thing, 
				  const std::string & filename, unsigned z, 
				  unsigned t )
  {
    struct jpeg_compress_struct	cinfo;
    struct jpeg_error_mgr	jerr;
    FILE			*fp;
    unsigned			i;
    JSAMPROW			row_pointer[1];

    cinfo.err = jpeg_std_error( &jerr );
    jpeg_create_compress( &cinfo );

    cinfo.image_width = thing.dimX();
    cinfo.image_height = thing.dimY();
    cinfo.in_color_space = ( sizeof( T ) == 3 ? JCS_RGB : JCS_GRAYSCALE );
    cinfo.input_components = ( sizeof( T ) == 3 ? 3 : 1 );
    cinfo.input_gamma = 1;
    jpeg_set_defaults( &cinfo );
    if( sizeof( T ) != 3 )
      cinfo.data_precision = 8 * sizeof( T );
    cinfo.smoothing_factor = 0;
    jpeg_set_quality( &cinfo, 100, true );
    cinfo.density_unit = 1;
    cinfo.X_density = (UINT16) ( 25.4 / thing.sizeX() );
    cinfo.Y_density = (UINT16) ( 25.4 / thing.sizeY() );

    fp = fopen( filename.c_str(), "wb" );
    if( !fp )
      throw carto::file_error( "JPEG writer : can't open file ", filename );

    jpeg_stdio_dest( &cinfo, fp );
    jpeg_start_compress( &cinfo, true );

    for( i=0; i<cinfo.image_height; ++i )
      {
	row_pointer[0] = (JSAMPROW) &thing( 0, i, z, t );
	jpeg_write_scanlines( &cinfo, row_pointer, 1 );
      }

    jpeg_finish_compress( &cinfo );
    fclose( fp );
    jpeg_destroy_compress( &cinfo );
  }

}

#endif

