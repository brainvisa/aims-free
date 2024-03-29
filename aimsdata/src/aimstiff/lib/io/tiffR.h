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
#include <soma-io/datasource/filedatasource.h>
#include <stdio.h>
extern "C"
{
#include <tiffio.h>
}


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

//     std::cout << "TiffReader allocate data " << carto::DataTypeCode<T>::dataType()
//               << "[ " << hdr->dimX() 
//               << ", " << hdr->dimY()
//               << ", " << hdr->dimZ()
//               << ", " << tmax - tmin + 1 << " ]"
//               << " border size " << carto::toString(border)
//               << std::endl << std::flush;
              
    AimsData<T> data( hdr->dimX(), 
                      hdr->dimY(), 
                      hdr->dimZ(),
                      tmax - tmin + 1, 
                      border, al );
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

  namespace
  {
    template <typename T, typename U> inline
    T tiff_scaled_value_single( const U* buffer )
    {
      T item;
      int shift = sizeof(U) <= sizeof(T) ?
        0 : ((sizeof(U) - sizeof(T)) * 8);
      item = *buffer >> shift;
      return item;
    }

    template <typename T, typename U> inline
    T tiff_scaled_value_items( const U* buffer, uint16_t nitems )
    {
      T item;
      int shift = sizeof(U) * nitems <= sizeof(T) ?
        0 : ((sizeof(U) - sizeof(T) / nitems) * 8);
      for( uint16_t i=0; i<nitems; ++i )
      {
        item[i] = *buffer++ >> shift;
      }
      return item;
    }

    template <typename T, typename U>
    class tiff_scaled_value_u
    {
    public:
      static inline T scale( const U* buffer )
      {
        return tiff_scaled_value_single<T, U>( buffer );
      }
    };

    template <typename U>
    class tiff_scaled_value_u<AimsRGB, U>
    {
    public:
      static inline AimsRGB scale( const U* buffer )
      {
        return tiff_scaled_value_items<AimsRGB, U>( buffer, 3 );
      }
    };

    template <typename U>
    class tiff_scaled_value_u<AimsRGBA, U>
    {
    public:
      static inline AimsRGBA scale( const U* buffer )
      {
        return tiff_scaled_value_items<AimsRGBA, U>( buffer, 4 );
      }
    };

    template <typename T> inline
    T tiff_scaled_value( const char *buffer, uint16_t bits )
    {
      if( bits == 8 )
        return tiff_scaled_value_u<T, uint8_t>::scale(
          (const uint8_t *) buffer );
      else if( bits == 16 )
        return tiff_scaled_value_u<T, uint16_t>::scale(
          (const uint16_t *) buffer );
      else // if( bits == 32 )
        return tiff_scaled_value_u<T, uint32_t>::scale(
          (const uint32_t *) buffer );
    }

  }

  template<class T>
  inline
  void TiffReader<T>::readFrame( AimsData<T> & data,
                                 const std::string & name, int zframe,
                                 unsigned tframe )
  {
    int tiled, stripSize, rowsPerStrip;
    uint i, s, zmin, zmax, dx = data.dimX(), dy = data.dimY();
    ushort photometric;

    TIFFSetWarningHandler( 0 );
    TIFF* tif = TIFFOpen(name.c_str(), "r");

    if( !tif )
      throw carto::wrong_format_error( _name );

    if (tif)
    {
      tiled = TIFFIsTiled(tif);
      if (tiled)
      {
        throw carto::invalid_format_error(
          "Not a tiled TIFF image : can't read" );
      }
      else
      {
        if ( zframe == -1 )
        {
            // Process all Tiff directories
            zmin = 0;
            zmax = data.dimZ();
        }
        else
        {
            // Process only the matching z slice
            zmin = (uint)zframe;
            zmax = (uint)zframe + 1;
        }

        for ( uint z = zmin; z < zmax; z++ )
        {
          TIFFSetDirectory(tif,  z );
          stripSize = TIFFStripSize(tif);
          uint16_t bits, samplesize/*, items*/;

          // Read tif frame properties
          TIFFGetFieldDefaulted(tif, TIFFTAG_ROWSPERSTRIP, &rowsPerStrip);
          TIFFGetFieldDefaulted(tif, TIFFTAG_PHOTOMETRIC, &photometric);
          TIFFGetFieldDefaulted(tif, TIFFTAG_BITSPERSAMPLE, &bits);
          samplesize = stripSize / dx / rowsPerStrip;
          //items = samplesize / (bits >> 3);
//           std::cout << "tiff rowsPerStrip: " << rowsPerStrip << ", photometric: " << photometric << ", t: " << tframe << ", z: " << z << ", stripsize: " << stripSize << ", bits: " << bits << ", samplesize: " << samplesize << ", items: " << items << std::endl;
          if( photometric != PHOTOMETRIC_PALETTE )
          {
            std::vector<char> buffer( stripSize, 0 );
            for(s=0, i=0; s < dy; s += rowsPerStrip, ++i)
            {
              TIFFReadEncodedStrip(tif, i, &buffer[0], stripSize);
              if( samplesize == sizeof(T) )
                for( uint y=s; y<std::min(s+rowsPerStrip, dy); ++y )
                  memcpy( &data(0, y, z, tframe),
                          &buffer[(y-s) * dx * samplesize],
                          dx * sizeof(T) );
              else
                for( uint y=s; y<std::min(s+rowsPerStrip, dy); ++y )
                {
                  T* ibuf = &data( 0, y, z, tframe );
                  for( uint x=0; x<dx; ++x, ++ibuf )
                    *ibuf = tiff_scaled_value<T>(
                      &buffer[((y-s) * dx + x) * samplesize], bits );
                }
            }

            if(photometric == PHOTOMETRIC_MINISWHITE)
            {
              // Flip bits
              byte* buffer;

              for(int y = 0; y < data.dimY(); ++y) {
                  buffer = (byte*)&data(0, y, z, tframe);
                  for( unsigned index=0; index<(dx * sizeof(T)); index++ )
                  {
                    buffer[index] =~ buffer[index];
                  }
              }
            }
          }
          else if( carto::DataTypeCode<T>().dataType()
            == carto::DataTypeCode<AimsRGBA>().dataType() )
          {
            // Indexed images can only be read as RGBA data
            uint32_t * rgba_data = (uint32_t *)&data(0, 0, z, tframe);
            TIFFReadRGBAImageOriented(tif, dx, data.dimY(),
                                      rgba_data, ORIENTATION_TOPLEFT);
          }
        }
      }
      TIFFClose(tif);
    }
  }
}

#endif
