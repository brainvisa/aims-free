/* This software and supporting documentation are distributed by
 *     Institut Federatif de Recherche 49
 *     CEA/NeuroSpin, Batiment 145,
 *     91191 Gif-sur-Yvette cedex
 *     France
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

#ifndef AIMS_IO_QTFORMATSW_H
#define AIMS_IO_QTFORMATSW_H

#include <aims/io/qtformatsheader.h>
#include <aims/io/datatypecode.h>
#include <aims/data/pheader.h>
#include <cartodata/volume/volume.h>
#include <aims/rgb/rgb.h>
#include <cartobase/exception/ioexcept.h>
#include <cartobase/stream/fileutil.h>
#include <cartobase/type/string_conversion.h>
#include <qimage.h>
#include <qapplication.h>


namespace aims
{

  template<typename T>
  class QtFormatsWriter
  {
  public:
    QtFormatsWriter( const std::string& name ) : _name( name ) {}
    ~QtFormatsWriter() {}

    void write( const carto::Volume<T> & thing );
    /**	called by write(), but you can call it for single frame writing 
	(axial slice) */
    void writeFrame( const carto::Volume<T> & thing,
                     const std::string & filename,
		     unsigned zfame, unsigned tframe );
    std::string format() const;

  private:
    std::string		_name;

    static QRgb convertColor( const T & );
  };


  template <typename T>
  inline QtFormatsWriter<T> & 
  operator << ( QtFormatsWriter<T> & writer, const carto::Volume<T> & thing )
  {
    writer.write( thing );
    return writer;
  }


  template <typename T>
  inline
  void QtFormatsWriter<T>::write( const carto::Volume<T>& thing )
  {
    std::string fmt = format();
    // std::cout << "QtFormatsWriter< " << carto::DataTypeCode<T>::name() << " >::write, format: " << fmt << std::endl;
    if( !qApp && ( fmt == "EPS" || fmt == "EPSF" || fmt == "EPSI" ) )
      throw carto::format_error( fmt + " format needs a QApplication",
                                 _name );
    unsigned	t, z, dt = thing.getSizeT(), dz = thing.getSizeZ();
    std::vector<float> vs = thing.getVoxelSize();
    QtFormatsHeader	hdr( _name, carto::DataTypeCode<T>().dataType(), 
                             thing.getSizeX(), thing.getSizeY(),
                             thing.getSizeZ(), thing.getSizeT(),
                             vs[0], vs[1], vs[2], vs[3] );
    const carto::PropertySet *ph = &thing.header();
    hdr.copyProperties( carto::Object::reference( *ph ) );

    std::string			dir = carto::FileUtil::dirname( _name );
    std::vector<std::string>	files = hdr.outputFilenames();

    hdr.setProperty( "file_type", format() );
    hdr.setProperty( "filenames", files );
    hdr.setProperty( "data_type", carto::DataTypeCode<T>::name() );
    if( !dir.empty() )
      dir += carto::FileUtil::separator();

    unsigned	i = 0;
    for( t=0; t<dt; ++t )
      for( z=0; z<dz; ++z, ++i )
        writeFrame( thing, dir + files[i], z, t );

    hdr.writeMinf( dir + carto::FileUtil::removeExtension( files[0] ) + '.' 
                   + hdr.extension() + ".minf" );
  }


  template<typename T>
  inline
  std::string QtFormatsWriter<T>::format() const
  {
    std::string f = carto::stringUpper( carto::FileUtil::extension( _name ) );
    if( f == "JPG" )
      return "JPEG";
    else
      return f;
  }


  template<typename T>
  inline
  void QtFormatsWriter<T>::writeFrame( const carto::Volume<T> & thing,
                                       const std::string & filename, 
                                       unsigned z, unsigned t )
  {
    int		x, y, dx = thing.getSizeX(), dy = thing.getSizeY();
    QImage	im( dx, dy, QImage::Format_RGB32 );
    if( carto::DataTypeCode<T>::name()
        == carto::DataTypeCode<AimsRGBA>::name() )
      im = im.convertToFormat( QImage::Format_ARGB32 );
    std::vector<float> vs = thing.getVoxelSize();
    im.setDotsPerMeterX( (int) rint( 1000 / vs[0] ) );
    im.setDotsPerMeterY( (int) rint( 1000 / vs[1] ) );
    for( y=0; y<dy; ++y )
      for( x=0; x<dx; ++x )
        im.setPixel( x, y, convertColor( thing.at( x, y, z, t ) ) );
    if( !im.save( filename.c_str(), 
                  format().c_str(), 
                  100 ) )
      carto::io_error::launchErrnoExcept( filename );
  }


  template<>
  inline
  void QtFormatsWriter<uint8_t>::writeFrame(
    const carto::Volume<uint8_t> & thing,
    const std::string & filename,
    unsigned z, unsigned t )
  {
    int		x, y, dx = thing.getSizeX(), dy = thing.getSizeY();
    QImage	im( dx, dy, QImage::Format_Indexed8 );
    std::vector<float> vs = thing.getVoxelSize();
    im.setDotsPerMeterX( (int) rint( 1000 / vs[0] ) );
    im.setDotsPerMeterY( (int) rint( 1000 / vs[1] ) );
    for( x=0; x<256; ++x )
      im.setColor( x, qRgb( x, x, x ) );
    for( y=0; y<dy; ++y )
      for( x=0; x<dx; ++x )
        im.setPixel( x, y, thing.at( x, y, z, t ) );
    if( !im.save( filename.c_str(), 
                  carto::stringUpper( carto::FileUtil::extension( filename 
                                                                  ) ).c_str(), 
                  100 ) )
      carto::io_error::launchErrnoExcept( filename );
  }


  template<typename T> inline 
  QRgb QtFormatsWriter<T>::convertColor( const T & x )
  {
    return qRgb( x, x, x );
  }


  template<> inline 
  QRgb QtFormatsWriter<AimsRGB>::convertColor( const AimsRGB & x )
  {
    return qRgb( x.red(), x.green(), x.blue() );
  }


  template<> inline 
  QRgb QtFormatsWriter<AimsRGBA>::convertColor( const AimsRGBA & x )
  {
    return qRgba( x.red(), x.green(), x.blue(), x.alpha() );
  }

}

#endif

