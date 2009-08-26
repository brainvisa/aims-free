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

#ifndef AIMS_IO_GIFTIMESHFORMAT_D_H
#define AIMS_IO_GIFTIMESHFORMAT_D_H

#include <aims/io/giftiformat.h>
#include <aims/io/giftiheader.h>


namespace aims
{

  template<int D, typename T>
  bool GiftiMeshFormat<D, T>::read( const std::string & filename,
                                    AimsTimeSurface<D, T> & vol,
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
    int tmesh = 0, ttex = 0, tnorm = 0, tpoly = 0;
    for( i=0; i<nda; ++i )
    {
      giiDataArray *da = gim->darray[i];
      switch( da->intent )
      {
        case NIFTI_INTENT_POINTSET:
          {
            int vnum = da->dims[0];
            int j;
            std::vector<Point3df> & vert = vol[tmesh].vertex();
            vert.clear();
            vert.reserve( vnum );
            for( j=0; j<vnum; ++j )
              vert.push_back( Point3df(
                convertedNiftiValue<float>( da->data, j*3, da->datatype ),
                convertedNiftiValue<float>( da->data, j*3+1, da->datatype ),
                convertedNiftiValue<float>( da->data, j*3+2, da->datatype ) ) );
            ++tmesh;
          }
          break;
        case NIFTI_INTENT_VECTOR:
          {
            int vnum = da->dims[0];
            int j;
            std::vector<Point3df> & norm = vol[tnorm].normal();
            norm.clear();
            norm.reserve( vnum );
            for( j=0; j<vnum; ++j )
              norm.push_back( Point3df(
                convertedNiftiValue<float>( da->data, j*3, da->datatype ),
                convertedNiftiValue<float>( da->data, j*3+1, da->datatype ),
                convertedNiftiValue<float>( da->data, j*3+2, da->datatype ) ) );
            ++tnorm;
            break;
          }
        case NIFTI_INTENT_TRIANGLE:
          {
            int vnum = da->dims[0];
            int j, k;
            std::vector<AimsVector<unsigned,D> > & poly = vol[tpoly].polygon();
            poly.clear();
            poly.reserve( vnum );
            for( j=0; j<vnum; ++j )
            {
              poly.push_back( AimsVector<unsigned,D>() );
              AimsVector<unsigned,D> &p = poly[j];
              for( k=0; k<D; ++k )
                p[k] = convertedNiftiValue<unsigned>( da->data, j*D+k,
                                                      da->datatype );
            }
            ++tpoly;
            break;
          }
        default:
          {
            int vnum = da->dims[0];
            int j;
            std::vector<T> & tex = vol[ttex].texture();
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

    if( tmesh > tpoly )
      tmesh = tpoly;
    if( vol.size() > (unsigned) tmesh )
    {
      std::cout << "warning: some incomplete meshes; truncating "
        << vol.size() << " instead of " << tmesh << "elements" << std::endl;
      while( vol.size() > (unsigned) tmesh )
        static_cast<std::map<int,AimsSurface<D,T> > &>(vol).erase(
          (int) vol.size() - 1 );
    }
    // verify polygons are all in the vertices range
    bool        broken = false;
    for( i=0; i<tmesh; ++i )
    {
      AimsSurface<D,T>        & surf = vol[i];
      std::vector<Point3df>   & vert = surf.vertex();
      std::vector<AimsVector<uint,D> >  & poly = surf.polygon();
      typename std::vector<AimsVector<uint,D> >::iterator ip;
      unsigned nvertex = vert.size();
      for ( ip=poly.begin(); ip!=poly.end(); ++ip )
      {
        AimsVector<uint,D>  & pol = *ip;
        for ( int j=0; j<D; ++j )
          if ( pol[j] >= nvertex )
          {
            if ( !broken )
            {
              broken = true;
              std::cerr << "Broken mesh: polygon pointing to a "
              << "vertex out of range" << std::endl;
              // std::cout << pol[j] << " / " << nvertex << std::endl;
            }
            poly.erase( ip );
            --ip;
            break;
          }
      }

      if( surf.normal().size() != vert.size() )
        surf.updateNormals();
    }

    vol.setHeader( hdr );
    gifti_free_image( gim );

    std::cout << "gifti mesh read OK\n";
    return true;
  }

  template<int D, typename T>
  bool GiftiMeshFormat<D, T>::write( const std::string & /*filename*/,
                                     const AimsTimeSurface<D, T> & /*vol*/,
                                     bool )
  {
    try
      {
        /*
        NiftiWriter<T>  r( filename );
        r.write( vol );
        */
      }
    catch( std::exception & e )
      {
        return false;
      }

    return true;
  }

}

#endif
