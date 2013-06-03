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

#ifndef AIMS_IO_TIFFR_H
#define AIMS_IO_TIFFR_H

#include <aims/data/data.h>
#include <aims/io/tiffheader.h>
#include <aims/io/datatypecode.h>
#include <cartobase/exception/file.h>
#include <cartobase/exception/format.h>
#include <cartobase/stream/fileutil.h>
#ifdef USE_SOMA_IO
  #include <soma-io/datasource/filedatasource.h>
#else
  #include <cartobase/datasource/filedatasource.h>
#endif
#include <stdio.h>
extern "C"
{
#include <tiffio.h>
}

using namespace std;
using namespace carto;


namespace aims
{

  template<class T>
  class TiffReader
  {
  public:
    TiffReader( const std::string& name ) : _name( name ) {}
    ~TiffReader() {}

    void read( AimsData<T>& thing, const carto::AllocatorContext & context, 
               carto::Object options );
    /**	called by read(), but you can call it for single frame reading 
	(axial slice) */
    void readFrame( AimsData<T> & thing, const std::string & filename, 
		    int zframe, unsigned tframe );

  private:
    std::string		_name;
  };


  template <class T>
  inline TiffReader<T> & 
  operator >> ( TiffReader<T> & reader, AimsData<T> & thing )
  {
    reader.read( thing, thing.allocator(), 
                 carto::Object::value( carto::PropertySet() ) );
    return reader;
  }


  template <class T>
  inline
  void TiffReader<T>::read( AimsData<T>& thing, 
                            const carto::AllocatorContext & context, 
                            carto::Object options )
  {
    TiffHeader *hdr = new TiffHeader( _name );
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
    hdr->getProperty( "volume_dimension", dims );

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

    dims[3] = tmax - tmin + 1;
    hdr->setProperty( "volume_dimension", dims );

    //	force data type into header
    carto::DataTypeCode<T>	dtc;
    hdr->setType( dtc.dataType() );
    std::string		dir = carto::FileUtil::dirname( _name );
    if( !dir.empty() )
      dir += carto::FileUtil::separator();

    unsigned	i = 0, s, t, ns = (unsigned) data.dimZ(), nt = tmax - tmin + 1;

    if ( files.size() == ns * nt )
    {
        // Each file contain 1 slice and 1 frame
        for( t=0; t<nt; ++t ) {
            for( s=0; s<ns; ++s, ++i ) {
                if ( ! carto::FileUtil::fileStat( dir + files[i] ).empty() ) {
                    readFrame( data, dir + files[i], s, t );
                }
            }
        }
    } else if ( files.size() == nt )
    {
        // Each file contain z slices and 1 frame
        for( t=0; t<nt; ++t ) {
            if ( ! carto::FileUtil::fileStat( dir + files[t] ).empty() ) {
                readFrame( data, dir + files[t], -1, t );
            }
        }
    }

    thing = data;
    if( hdr->hasProperty( "filenames" ) )
      hdr->removeProperty( "filenames" );
    thing.setHeader( hdr );
  }

  template<class T>
  inline
  void TiffReader<T>::readFrame( AimsData<T> & data, 
				 const std::string & name, int zframe, 
				 unsigned tframe )
  {
    int tiled, stripSize, rowsPerStrip, i, s;
    uint zmin, zmax;
    ushort photometric;

    TIFFSetWarningHandler( 0 );
    TIFF* tif = TIFFOpen(name.c_str(), "r");
    if (tif) {
      tiled = TIFFIsTiled(tif);
      if (tiled) {
        throw carto::invalid_format_error( "Not a tiled TIFF image : " 
                          "can't read" );
      }
      else {
        if ( zframe == -1 ) {
            // Process all Tiff directories
            zmin = 0;
            zmax = data.dimZ();
        } else {
            // Process only the matching z slice
            zmin = (uint)zframe;
            zmax = (uint)zframe + 1;
        }

        for ( uint z = zmin; z < zmax; z++ ) {
            TIFFSetDirectory(tif,  z );
            stripSize = TIFFStripSize(tif);
    
            // Read tif frame properties
            TIFFGetFieldDefaulted(tif, TIFFTAG_ROWSPERSTRIP, &rowsPerStrip);
            TIFFGetFieldDefaulted(tif, TIFFTAG_PHOTOMETRIC, &photometric);
            if ( photometric != PHOTOMETRIC_PALETTE ) {
                for(s=0, i=0; s < data.dimY(); s += rowsPerStrip, ++i) {
                     TIFFReadEncodedStrip(tif, i, &data(0, s, z, tframe), stripSize);
                }

                if(photometric == PHOTOMETRIC_MINISWHITE){
                    // Flip bits
                    byte* buffer;

                    for(int y = 0; y < data.dimY(); ++y) {
                        buffer = (byte*)&data(0, y, z, tframe);
                        for (unsigned index = 0; index < (data.dimX() * sizeof(T)) ; index++) {
                            buffer[index] =~ buffer[index];
                        }
                    }
                }
            }
            else if ( DataTypeCode<T>().dataType() == DataTypeCode<AimsRGBA>().dataType() ) {
                // Indexed images can only be read as RGBA data
                uint32 * rgba_data = (uint32 *)&data(0, 0, z, tframe);
                TIFFReadRGBAImageOriented(tif, data.dimX(), data.dimY(), rgba_data, ORIENTATION_TOPLEFT);
            }
        }
      }
      TIFFClose(tif);
    }
  }
}

#endif
