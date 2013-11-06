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

#ifndef AIMS_IO_QTFORMATSR_H
#define AIMS_IO_QTFORMATSR_H

#include <aims/data/data.h>
#include <aims/io/qtformatsheader.h>
#include <aims/io/datatypecode.h>
#include <aims/rgb/rgb.h>
#include <cartobase/stream/fileutil.h>
#include <cartobase/exception/file.h>
#include <cartobase/exception/format.h>
#include <soma-io/datasource/filedatasource.h>
#include <cartobase/thread/mutex.h>
#include <qcolor.h>
#include <QImage>
#include <QImageReader>
#include <QImageWriter>

namespace aims
{

  template<typename T>
  class QtFormatsReader
  {
  public:
    QtFormatsReader( const std::string& name ) : _name( name ) {}
    ~QtFormatsReader() {}

    void read( AimsData<T>& thing, const carto::AllocatorContext & context, 
               carto::Object options );
    /**	called by read(), but you can call it for single frame reading 
	(axial slice) */
    void readFrame( AimsData<T> & thing, QtFormatsHeader* hdr,
                    const std::string & filename, unsigned zfame,
                    unsigned tframe );

  private:
    std::string		_name;
    static T convertColor( const QRgb & );
  };


  template <typename T>
  inline QtFormatsReader<T> & 
  operator >> ( QtFormatsReader<T> & reader, AimsData<T> & thing )
  {
    reader.read( thing );
    return reader;
  }


  template <typename T>
  inline
  void QtFormatsReader<T>::read( AimsData<T>& thing, 
                                 const carto::AllocatorContext & context, 
                                 carto::Object options )
  {
    QtFormatsHeader *hdr = new QtFormatsHeader( _name );
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

    std::vector<std::string> files = hdr->inputFilenames();

    unsigned	tmin = 0, tmax = hdr->dimT() - 1;
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

    carto::AllocatorContext	al 
      ( context.accessMode(), 
        carto::rc_ptr<carto::DataSource>
        ( new carto::FileDataSource( *files.begin(), 0, 
                                     carto::DataSource::Read ) ), 
        false, context.useFactor() );

    AimsData<T> data( hdr->dimX(), hdr->dimY(), hdr->dimZ(), 
		      tmax - tmin + 1, border, al );

    data.setSizeX( hdr->sizeX() );
    data.setSizeY( hdr->sizeY() );
    data.setSizeZ( hdr->sizeZ() );
    data.setSizeT( hdr->sizeT() );

    std::vector<int>	dims(4);
    dims[0] = hdr->dimX();
    dims[1] = hdr->dimY();
    dims[2] = hdr->dimZ();
    dims[3] = tmax - tmin + 1;
    hdr->setProperty( "volume_dimension", dims );

    if( dims.size() < 1 )
      dims.push_back( hdr->dimX() );
    if( dims.size() < 2 )
      dims.push_back( hdr->dimY() );
    if( dims.size() < 3 )
      dims.push_back( hdr->dimZ() );
    if( dims.size() < 4 )
      dims.push_back( hdr->dimT() );

    //	force data type into header
    carto::DataTypeCode<T>	dtc;
    hdr->setType( dtc.dataType() );
    std::string		dir = carto::FileUtil::dirname( _name );
    if( !dir.empty() )
      dir += carto::FileUtil::separator();

    unsigned	i = 0, s, t, ns = (unsigned) data.dimZ(), nt = tmax - tmin + 1;
    for( t=0; t<nt; ++t )
      for( s=0; s<ns; ++s, ++i )
        readFrame( data, hdr, dir + files[i], s, t );

    thing = data;
    if( hdr->hasProperty( "filenames" ) )
      hdr->removeProperty( "filenames" );
    thing.setHeader( hdr );
  }

  template<typename T>
  inline
  void QtFormatsReader<T>::readFrame( AimsData<T> & data,
                                      QtFormatsHeader * hdr,
                                      const std::string & name, unsigned z, 
                                      unsigned t )
  {
    // std::cout << "readFrame: " << name << ", z: " << z << ", t: " << t << "\n";
    const QImage	*imp = 0;
    QImage		ima;
    QImageReader	qio;

    if( hdr->filename() == name && hdr->hasRead() )
      imp = &hdr->qimage();
    else
      {
        std::string		format;
        hdr->getProperty( "file_type", format );
        qio.setFileName( name.c_str() );
        qio.setFormat( format.c_str() );
        bool lock = false;
        if( format == "JP2" )
        {
          lock = true;
          QtFormatsHeader::qformatsMutex().lock();
        }
        ima = qio.read();
        if( lock )
          QtFormatsHeader::qformatsMutex().unlock();
        if( ima.isNull() )
          throw carto::format_mismatch_error( name );
        imp = &ima;
      }

    const QImage	& im = *imp;
    int			y, dx = data.dimX(), dy = data.dimY();

    if( im.depth() == sizeof(T) && im.numColors() == 0 )
      for( y=0; y<dy; ++y )
        memcpy( &data( 0, y, z, t ), im.scanLine( y ), dx * sizeof( T ) );
    else
      for( y=0; y<dy; ++y )
        for( int x=0; x<dx; ++x )
          data( x, y, z, t ) = convertColor( im.pixel( x, y ) );
  }


  template<typename T> inline 
  T QtFormatsReader<T>::convertColor( const QRgb & x )
  {
    return (T) sqrt( ( ( (double) qRed( x ) ) * (double) qRed( x ) 
                       + qGreen( x ) * (double) qGreen( x ) 
                       + qBlue( x ) * (double) qBlue( x ) ) / 3 );
  }


  template<> inline 
  AimsRGB QtFormatsReader<AimsRGB>::convertColor( const QRgb & x )
  {
    return AimsRGB( (byte) qRed( x ), (byte) qGreen( x ), (byte) qBlue( x ) );
  }


  template<> inline 
  AimsRGBA QtFormatsReader<AimsRGBA>::convertColor( const QRgb & x )
  {
    return AimsRGBA( (byte) qRed( x ), (byte) qGreen( x ), (byte) qBlue( x ), 
                     (byte) qAlpha( x ) );
  }

}

#endif

