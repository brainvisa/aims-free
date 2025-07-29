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

#ifndef AIMS_IO_TIFFW_H
#define AIMS_IO_TIFFW_H

#include <cstdlib>
#include <aims/io/tiffheader.h>
#include <aims/io/datatypecode.h>
#include <aims/data/pheader.h>
#include <aims/data/data.h>
#include <cartobase/exception/file.h>
#include <cartobase/exception/ioexcept.h>
#include <stdio.h>
extern "C"
{
#include <tiffio.h>
}

namespace aims
{

  template<class T>
  class TiffWriter
  {
  public:
    TiffWriter( const std::string& name ) : _name( name ) {}
    ~TiffWriter() {}

    void write( const AimsData<T> & thing );
    /**	called by write(), but you can call it for single frame writing 
	(axial slice) */
    void writeFrame( const AimsData<T> & thing, const std::string & filename, 
		     unsigned zfame, unsigned tframe );

  private:
    std::string		_name;
  };

  template <class T>
  inline TiffWriter<T> & 
  operator << ( TiffWriter<T> & writer, const AimsData<T> & thing )
  {
    writer.write( thing );
    return writer;
  }


  template <class T>
  inline
  void TiffWriter<T>::write( const AimsData<T>& thing )
  {
    unsigned	t, z, dt = thing.dimT(), dz = thing.dimZ();
    TiffHeader	hdr( _name, carto::DataTypeCode<T>().dataType(), thing.dimX(), 
		     thing.dimY(), thing.dimZ(), thing.dimT(), thing.sizeX(), 
		     thing.sizeY(), thing.sizeZ(), thing.sizeT() );
    const PythonHeader 
      *ph = dynamic_cast<const PythonHeader *>( thing.header() );
    if( ph )
      hdr.copy( *ph );

    std::string			dir = carto::FileUtil::dirname( _name );
    std::vector<std::string>	files = hdr.outputFilenames();

    hdr.setProperty( "file_type", std::string( "TIFF" ) );
    hdr.setProperty( "filenames", files );
    if( !dir.empty() )
      dir += carto::FileUtil::separator();

    unsigned	i = 0;
    for( t=0; t<dt; ++t )
      for( z=0; z<dz; ++z, ++i ) {
        writeFrame( thing, dir + files[i], z, t );
      }

    hdr.writeMinf( dir + carto::FileUtil::removeExtension( files[0] ) 
                   + hdr.extension() + ".minf" );
  }

  template<class T>
  inline
  void TiffWriter<T>::writeFrame( const AimsData<T> & data,
                                  const std::string & filename,
                                  unsigned z,
                                  unsigned t )
  {
    int bps, spp, photometric;
    std::string name = carto::DataTypeCode<T>().name();
    uint16_t sampleformat = SAMPLEFORMAT_UINT;
    uint16_t extrasampletype = EXTRASAMPLE_UNASSALPHA;
    uint16_t extrasamplescount = 0;

    if ( name == "U8"  )
    {
      spp = 1;
      bps = 8;
      photometric = PHOTOMETRIC_MINISBLACK;
    }
    else if ( name == "S8"  )
    {
      spp = 1;
      bps = 8;
      photometric = PHOTOMETRIC_MINISBLACK;
      sampleformat = SAMPLEFORMAT_INT;
    }
    else if ( name == "U16"  )
    {
      spp = 1;
      bps = 16;
      photometric = PHOTOMETRIC_MINISBLACK;
    }
    else if ( name == "S16"  )
    {
      spp = 1;
      bps = 16;
      photometric = PHOTOMETRIC_MINISBLACK;
      sampleformat = SAMPLEFORMAT_INT;
    }
    else if ( name == "U32"  )
    {
      spp = 1;
      bps = 32;
      photometric = PHOTOMETRIC_MINISBLACK;
    }
    else if ( name == "S32"  )
    {
      spp = 1;
      bps = 32;
      photometric = PHOTOMETRIC_MINISBLACK;
      sampleformat = SAMPLEFORMAT_INT;
    }
    else if ( name == "FLOAT"  )
    {
      spp = 1;
      bps = 32;
      photometric = PHOTOMETRIC_MINISBLACK;
      sampleformat = SAMPLEFORMAT_IEEEFP;
    }
    else if ( name == "DOUBLE"  )
    {
      spp = 1;
      bps = 64;
      photometric = PHOTOMETRIC_MINISBLACK;
      sampleformat = SAMPLEFORMAT_IEEEFP;
    }
    else if ( name == "CFLOAT"  )
    {
      spp = 2;
      bps = 32;
      photometric = PHOTOMETRIC_MINISBLACK;
      sampleformat = SAMPLEFORMAT_COMPLEXIEEEFP;
    }
    else if ( name == "CDOUBLE"  )
    {
      spp = 2;
      bps = 64;
      photometric = PHOTOMETRIC_MINISBLACK;
      sampleformat = SAMPLEFORMAT_COMPLEXIEEEFP;
    }
    else if ( name == "RGBA"  )
    {
      spp = 4;
      bps = 8;
      photometric = PHOTOMETRIC_RGB;
      extrasamplescount = 1;
    }
    else
    {
      spp = 3;
      bps = 8;
      photometric = PHOTOMETRIC_RGB;
    }

    // std::cout << "-- TIFF Filename : " << filename << std::endl;
    // std::cout << "-- TIFF Type : " << name << ", Size : " << sizeof(T) << ", Spp : " << spp << ", Bps : " << bps << std::endl;
    // std::cout << "-- TIFF DimX : " << data.dimX() << ", DimY : " << data.dimY() << std::endl;
    // std::cout << "-- TIFF SizeX : " << data.sizeX() << ", SizeY : " << data.sizeY() << std::endl;
    // std::cout << "-- TIFF Photometric : " << photometric << std::endl;
    // std::cout << "-- TIFF data size: " << carto::toString((long)spp * data.dimX() * data.dimY()) << std::endl << std::flush;
    // std::cout << "-- TIFF data size limit: " << carto::toString(std::numeric_limits<uint32_t>::max()) << std::endl << std::flush;

    // Define an image
    TIFF *tif;

    if (((long)spp * data.dimX() * data.dimY()) <= std::numeric_limits<uint32_t>::max()) { 
      // Use standard tiff
      // std::cout << "-- TIFF support: standard" << std::endl;
      tif = TIFFOpen(filename.c_str(), "w");
    }
    else {
      // Use big tiff
      // std::cout << "-- TIFF support: big tiff" << std::endl;
      tif = TIFFOpen(filename.c_str(), "w8");
    }

    // Open the TIFF file
    if(tif == NULL){
      std::cout << "Could not open '" << filename << "' for writing." << std::endl;
      throw carto::file_not_found_error( _name );
    }

    // We need to set some values for basic tags before we can add any data
    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, data.dimX() );
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, data.dimY());
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, bps);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, spp );
    TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, 1 );

    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, photometric);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);

    TIFFSetField(tif, TIFFTAG_XRESOLUTION, 10. / data.sizeX());
    TIFFSetField(tif, TIFFTAG_YRESOLUTION, 10. / data.sizeY());
    TIFFSetField(tif, TIFFTAG_RESOLUTIONUNIT, RESUNIT_CENTIMETER);
    TIFFSetField(tif, TIFFTAG_SAMPLEFORMAT, sampleformat);

    if (extrasamplescount > 0) {
        TIFFSetField(tif, TIFFTAG_EXTRASAMPLES, extrasamplescount, &extrasampletype);
    }

    tsize_t res;
    int y, ny = data.dimY(), nx = data.dimX();

    long stride = &data( 1 ) - &data( 0 );
    std::vector<T> buffer;
    if( stride != 1 )
      // allocate buffer for un-strided data
      buffer.resize( nx );
    void *ptr;

    // Write the information to the file
    for( y=0; y<ny; ++y )
    {
      if( stride == 1 )
        ptr = (void *) &data(0, y, z, t);
      else
      {
        // stides along X axis: must copy things
        const T* p = &data( 0, y, z, t );
        for( long x=0; x<nx; ++x, p+=stride )
          buffer[x] = *p;
        ptr = (void *) &buffer[0];
      }

      res = TIFFWriteEncodedStrip(tif, y, ptr, (bps / 8) * spp * data.dimX() );

      if( res < 0 )
        throw carto::file_error( filename );
    }

    // Close the file
    TIFFClose(tif);
  }
}

#endif

