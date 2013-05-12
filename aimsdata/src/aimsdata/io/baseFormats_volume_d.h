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

#ifndef AIMS_IO_BASEFORMATS_VOLUME_D_H
#define AIMS_IO_BASEFORMATS_VOLUME_D_H

#include <aims/config/aimsdata_config.h>
#include <aims/io/baseFormats_volume.h>
#include <aims/io/gisR.h>
#include <aims/io/gisW.h>
#include <aims/io/spmR.h>
#include <aims/io/spmW.h>
#include <aims/io/genesisR.h>
#include <aims/io/imasparseheader.h>


namespace aims
{

  //	GIS

  template<typename T>
  GisFormat<T>::~GisFormat()
  {
  }


  template<class T>
  bool GisFormat<T>::read( const std::string & filename, AimsData<T> & vol, 
                           const carto::AllocatorContext & context, 
                           carto::Object options )
  {
    GisReader<T>	r( filename );
    r.read( vol, context, options );
    return( true );
  }


  template<class T>
  bool GisFormat<T>::write( const std::string & filename, 
			    const AimsData<T> & vol, carto::Object options )
  {
    bool ascii = false;
    try
    {
      if( !options.isNull() )
      {
        carto::Object aso = options->getProperty( "ascii" );
        if( !aso.isNull() )
          ascii = (bool) aso->getScalar();
      }
    }
    catch( ... )
    {
    }
    GisWriter<T>	r( filename, ascii );
    r.write( vol );
    return( true );
  }

  //	SPM

  template<typename T>
  SpmFormat<T>::~SpmFormat()
  {
  }


  template<class T>
  bool SpmFormat<T>::read( const std::string & filename, AimsData<T> & vol, 
                           const carto::AllocatorContext & context, 
                           carto::Object options )
  {
    SpmReader<T>	r( filename );
    r.read( vol, context, options );
    return( true );
  }


  template<class T>
  bool SpmFormat<T>::write( const std::string & filename, 
			    const AimsData<T> & vol, carto::Object )
  {
    SpmWriter<T>	r( filename );
    r.write( vol );
    return( true );
  }

  //	GENESIS

  template<typename T>
  GenesisFormat<T>::~GenesisFormat()
  {
  }


  template<class T>
  bool GenesisFormat<T>::read( const std::string & filename, 
                               AimsData<T> & vol, 
                               const carto::AllocatorContext & context, 
                               carto::Object options )
  {
    GenesisReader<T>	r( filename );
    r.read( vol,context, options );
    return( true );
  }


  // IMAS (sparse)

  template <typename T>
  bool ImasVolFormat<T>::read( const std::string & filename, 
                               AimsData<T> & obj, 
                               const carto::AllocatorContext & context, 
                               carto::Object options )
  {
    ImasHeader *hdr = new ImasHeader( filename );
    if( !hdr->read() )
    {
      delete hdr;
      return false;
    }

    int ascii = false, bswap = false;
    hdr->getProperty( "ascii", ascii );
    hdr->getProperty( "byte_swapping", bswap );

    std::ifstream is( filename.c_str() );

    ItemReader<uint32_t> *itemR1 = DefaultItemReader<uint32_t>().reader(
      ascii ? "ascii" : "binar", bswap );
    ItemReader<double> *itemR2 = DefaultItemReader<double>().reader(
      ascii ? "ascii" : "binar", bswap );

    uint32_t size1 = 0U;
    uint32_t size2 = 0U;
    uint32_t nonZeroElementCount = 0U;
    uint32_t k, s1, s2;
    double value;

    itemR1->read( is, size1 );
    itemR1->read( is, size2 );
    itemR1->read( is, nonZeroElementCount );

    std::streampos p = is.tellg(), e;
    is.seekg( 0, std::ios_base::end );
    e = is.tellg();
    is.seekg( p, std::ios_base::beg );
    if( nonZeroElementCount > size1 * size2
        || nonZeroElementCount * 16 != e - p )
      throw carto::wrong_format_error( 
        "Wrong format or corrupted .imas format", filename );

    carto::Volume<T> *vol = new carto::Volume<T>( size2, size1 );
    vol->fill( 0 );

    for ( k = 0; k < nonZeroElementCount; k++ )
    {
      itemR1->read( is, s1 );
      itemR1->read( is, s2 );
      itemR2->read( is, value );
      vol->at( s2, s1 ) = value;
    }

    obj = carto::rc_ptr<carto::Volume<T> >( vol );

    is.close();

    delete itemR1;
    delete itemR2;

    obj.setHeader( hdr );
    return true;
  }


  template<class T>
  bool ImasVolFormat<T>::write( const std::string & filename, 
                                const AimsData<T> & obj, 
                                carto::Object options )
  {
    bool ascii = false;
    try
    {
      if( !options.isNull() )
      {
        carto::Object aso = options->getProperty( "ascii" );
        if( !aso.isNull() )
          ascii = (bool) aso->getScalar();
      }
    }
    catch( ... )
    {
    }

    std::ofstream os( filename.c_str() );

    ItemWriter<uint32_t> *itemW1 = DefaultItemWriter<uint32_t>().writer(
      ascii ? "ascii" : "binar", false );
    ItemWriter<double> *itemW2 = DefaultItemWriter<double>().writer(
      ascii ? "ascii" : "binar", false );

    unsigned x, y, nx = obj.dimX(), ny = obj.dimY();
    unsigned /*long*/ count = 0;

    for( y=0; y<ny; ++y )
    {
      const T* buf = &obj( 0, y );
      long inc = &obj( 1, y ) - buf;

      for( x=0; x<nx; ++x, buf+=inc )
        if( *buf != 0 )
          ++count;
    }

    itemW1->write( os, obj.dimY() );
    itemW1->write( os, obj.dimX() );
    itemW1->write( os, count ); // WARNING should be 64 bits !

    for( y=0; y<ny; ++y )
    {
      const T* buf = &obj( 0, y );
      long inc = &obj( 1, y ) - buf;

      for( x=0; x<nx; ++x, buf+=inc )
        if( *buf != 0 )
        {
          itemW1->write( os, y );
          itemW1->write( os, x );
          itemW2->write( os, *buf );
        }

    }

    os.close();
    delete itemW1;
    delete itemW2;

    obj.header()->writeMinf( filename + ".minf" );
    return true;
  }

}


#endif


