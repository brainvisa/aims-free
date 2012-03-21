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

#ifndef AIMS_IO_GIFTITEXTUREFORMAT_D_H
#define AIMS_IO_GIFTITEXTUREFORMAT_D_H

#include <aims/io/giftiformat.h>
#include <aims/io/giftiheader.h>
#include <aims/io/giftiutil.h>
extern "C"
{
#include <gifti_io.h>
}


namespace aims
{

  template<typename T>
  bool GiftiTextureFormat<T>::read( const std::string & filename,
                                    TimeTexture<T> & vol,
                                    const carto::AllocatorContext
                                        & /*context*/,
                                    carto::Object options )
  {

    // std::cout << "gifti texture read\n";

    GiftiHeader hdr( filename );

    setOptions(options);

    if( !hdr.read() )
      carto::io_error::launchErrnoExcept( hdr.name() );

    gifti_image *gim = gifti_read_image( hdr.name().c_str(), 1 );
    if( !gim )
    {
      throw carto::format_error( "could not re-read GIFTI file", hdr.name() );
    }

    int nda = gim->numDA, i;
    int ttex = 0;
    for( i=0; i<nda; ++i )
    {
      giiDataArray *da = gim->darray[i];
      switch( da->intent )
      {
        case NIFTI_INTENT_POINTSET:
          break;
        case NIFTI_INTENT_VECTOR:
          break;
        case NIFTI_INTENT_TRIANGLE:
          break;

        default:
          {
        	//std::cout << "type : " << gifti_intent_to_string(da->intent) << "\n";

            int vnum = da->dims[0];
            int j;
            std::vector<T> & tex = vol[ttex].data();
            tex.clear();
            tex.reserve( vnum );
            for( j=0; j<vnum; ++j )
            {
              tex.push_back( convertedNiftiValue<T>( da->data, j,
                                                     da->datatype ) );
            }
            ++ttex;
          }
      }
    }

    vol.setHeader( hdr );
    gifti_free_image( gim );

    // std::cout << "OK\n";
    return true;
  }

  template<typename T>
  bool GiftiTextureFormat<T>::write( const std::string & filename,
                                     const TimeTexture<T> & thing,
                                     carto::Object )
  {

    // std::cout << "gifti texture write\n";

    try
    {
      const PythonHeader & thdr = thing.header();
      GiftiHeader hdr( filename );
      hdr.copy( thdr );

      if( hdr.hasProperty( "nb_t_pos" ) )
          hdr.removeProperty( "nb_t_pos" );

      gifti_image *gim = hdr.giftiImageBase();
      std::string fname = hdr.name();

      int hdrtexda = 0;
      carto::Object da_info;
      try
      {
        da_info = thdr.getProperty( "GIFTI_dataarrays_info" );
      }
      catch( ... )
      {
        // std::cout << "error GIFTI_dataarrays_info\n";
      }

      hdr.setOptions(options());

      //std::cout << "add texture\n";

      hdr.giftiAddTexture( gim, thing);
      // add external textures
      hdr.giftiAddExternalTextures( gim, hdrtexda, da_info );

      carto::Object da_label;
      try
      {
        da_label = thdr.getProperty( "GIFTI_labels_table" );
        // std::cout << "ecriture label OK\n";
      }
      catch( ... )
      {
      }
      // labels table
      hdr.giftiAddLabelTable( gim );

      // write all
      gifti_write_image( gim, fname.c_str(), 1 );
      gifti_free_image( gim );
      // .minf header
      if( hdr.hasProperty( "GIFTI_metadata") )
        hdr.removeProperty( "GIFTI_metadata" );
      if( hdr.hasProperty( "GIFTI_version" ) )
        hdr.removeProperty( "GIFTI_version" );
      if( hdr.hasProperty( "GIFTI_dataarrays_info" ) )
        hdr.removeProperty( "GIFTI_dataarrays_info" );
      if( hdr.hasProperty( "file_type" ) )
        hdr.removeProperty( "file_type" );
      if( hdr.hasProperty( "GIFTI_labels_table") )
        hdr.removeProperty( "GIFTI_labels_table" );
      hdr.writeMinf( fname + ".minf" );

      // std::cout << "OK\n";
      return true;
    }
    catch( std::exception & e )
    {
      return false;
    }
    return true;
  }
}

#endif
