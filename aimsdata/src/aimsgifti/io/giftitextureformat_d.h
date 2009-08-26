/* Copyright (c) 2009 CEA
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

#ifndef AIMS_IO_GIFTITEXTUREFORMAT_D_H
#define AIMS_IO_GIFTITEXTUREFORMAT_D_H

#include <aims/io/giftiformat.h>
#include <aims/io/giftiheader.h>


namespace aims
{

  template<typename T>
  bool GiftiTextureFormat<T>::read( const std::string & filename,
                                    TimeTexture<T> & vol,
                                    const carto::AllocatorContext & /*context*/,
                                    carto::Object /*options*/ )
  {
    GiftiHeader hdr( filename );
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

    std::cout << "gifti texture read OK\n";
    return true;
  }

  template<typename T>
  bool GiftiTextureFormat<T>::write( const std::string & /*filename*/,
                                     const TimeTexture<T> & /*vol*/,
                                     bool )
  {
    try
      {
        return false;
      }
    catch( std::exception & e )
      {
        return false;
      }

    return true;
  }

}

#endif
