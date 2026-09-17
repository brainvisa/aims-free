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
    jpeg_set_quality( &cinfo, 100, TRUE );
    cinfo.density_unit = 1;
    cinfo.X_density = (UINT16) ( 25.4 / thing.sizeX() );
    cinfo.Y_density = (UINT16) ( 25.4 / thing.sizeY() );

    fp = fopen( filename.c_str(), "wb" );
    if( !fp )
      throw carto::file_error( filename );

    jpeg_stdio_dest( &cinfo, fp );
    jpeg_start_compress( &cinfo, TRUE );
    long stride = &thing( 1 ) - &thing( 0 );
    std::vector<T> buffer;
    if( stride != 1 )
      // allocate buffer for un-strided data
      buffer.resize( cinfo.image_width );

    for( i=0; i<cinfo.image_height; ++i )
    {

      if( stride == 1 )
        row_pointer[0] = (JSAMPROW) &thing( 0, i, z, t );
      else
      {
        // stides along X axis: must copy things
        const T* p = &thing( 0, i, z, t );
        for( long k=0; k<cinfo.image_width; ++k, p+=stride )
          buffer[k] = *p;
        row_pointer[0] = (JSAMPROW) &buffer[0];
      }
      jpeg_write_scanlines( &cinfo, row_pointer, 1 );
    }

    jpeg_finish_compress( &cinfo );
    fclose( fp );
    jpeg_destroy_compress( &cinfo );
  }

}

#endif

