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
#include <aims/io/giftiutil.h>
#include <aims/io/process.h>
#include <aims/io/finder.h>
extern "C"
{
#include <gifti_io.h>
}

// DEBUG
#include <cartobase/object/pythonwriter.h>

namespace aims
{

  namespace
  {
    template <int D, typename T>
    void giftiReadTexture( AimsTimeSurface<D, T> & vol, int texnum,
                           giiDataArray *da, GiftiHeader & hdr )
    {
      int vnum = da->dims[0];
      int j;
      std::vector<T> & tex = vol[texnum].texture();
      tex.clear();
      tex.reserve( vnum );
      for( j=0; j<vnum; ++j )
      {
        tex.push_back( convertedNiftiValue<T>( da->data, j,
                                              da->datatype ) );
      }
    }


    class GiftiReadExternalTexture : public Process
    {
    public:
      GiftiReadExternalTexture( carto::Object textures, giiDataArray* da )
      : Process(), textures( textures ), da( da )
      {}

      carto::Object textures;
      giiDataArray *da;
    };


    template <typename T>
    bool giftiReadExternalTexture( Process & p, const std::string &, Finder & )
    {
      GiftiReadExternalTexture & gp
        = static_cast<GiftiReadExternalTexture &>( p );
      giiDataArray *da = gp.da;
      int j, vnum = da->dims[0];
      carto::Object o = carto::Object::value( TimeTexture<T>() );
      TimeTexture<T> & ttex
        = o->carto::GenericObject::value< TimeTexture<T> >();
      std::vector<T> & tex = ttex[0].data();
      tex.reserve( vnum );
      for( j=0; j<vnum; ++j )
      {
        tex.push_back( convertedNiftiValue<T>( da->data, j, da->datatype ) );
      }
      carto::Object textures = gp.textures;
      textures->insertArrayItem( -1, o );

      return true;
    }


    template <int D>
    void giftiReadTexture( AimsTimeSurface<D, Void> & /*vol*/, int /*texnum*/,
                           giiDataArray *da, GiftiHeader & hdr )
    {
      carto::Object textures;
      try
      {
        textures = hdr.getProperty( "textures" );
      }
      catch( ... )
      {
        textures = carto::Object::value( carto::ObjectVector() );
      }
      // get data type
      int ndim = da->num_dim;
      std::string dtype = giftiTextureDataType( da->datatype, ndim, da->dims );
      // std::cout << "reading texture of: " << dtype << std::endl;

      GiftiReadExternalTexture p( textures, da );
      p.registerProcessType( "Texture", "FLOAT",
                             &giftiReadExternalTexture<float> );
      p.registerProcessType( "Texture", "POINT2DF",
                             &giftiReadExternalTexture<Point2df> );
      p.registerProcessType( "Texture", "S16",
                             &giftiReadExternalTexture<int16_t> );
      p.registerProcessType( "Texture", "S32",
                             &giftiReadExternalTexture<int32_t> );
      p.registerProcessType( "Texture", "U32",
                             &giftiReadExternalTexture<uint32_t> );
      p.registerProcessType( "Texture", "VECTOR_OF_2_S16",
                             &giftiReadExternalTexture<Point2d> );
      // TODO: etc for other types...
      Finder f;
      f.setObjectType( "Texture" );
      f.setDataType( dtype );
      if( p.execute( f, "" ) )
      {
        hdr.setProperty( "textures", textures );
      }
    }

  }


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
            // texture
            giftiReadTexture( vol, ttex, da, hdr );
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


  namespace
  {

    template <typename T>
    void giftiFillTextureBuffer( gifti_image *gim, giiDataArray * da,
                                 const std::vector<T> & tex )
    {
      unsigned i, n = tex.size();
      da->dims[1] = 1;
      da->nbyper = sizeof( T );
      da->datatype = niftiIntDataType( carto::DataTypeCode<T>::name() );
      da->nvals = n;
      int nda = gim->numDA - 1;
      gifti_alloc_DA_data( gim, &nda, 1 );
      T* buf = reinterpret_cast<T *>( da->data );
      for( i=0; i<n; ++i )
        *buf++ = tex[i];
    }


    template <typename T, int D>
    void giftiFillTextureBuffer( gifti_image *gim, giiDataArray * da,
                                 const std::vector<AimsVector<T, D> > & tex )
    {
      unsigned i, n = tex.size();
      int j;
      da->dims[1] = D;
      da->nbyper = sizeof( T );
      da->datatype = niftiIntDataType( carto::DataTypeCode<T>::name() );
      da->nvals = n * D;
      int nda = gim->numDA - 1;
      gifti_alloc_DA_data( gim, &nda, 1 );
      T* buf = reinterpret_cast<T *>( da->data );
      for( i=0; i<n; ++i )
        for( j=0; j<D; ++j )
          *buf++ = tex[i][j];
    }


    template <typename T>
    void giftiAddTexture( gifti_image* gim, const std::vector<T> & tex,
                          const PythonHeader & /*hdr*/ )
    {
      if( !tex.empty() )
      {
        int nda = gim->numDA;
        gifti_add_empty_darray( gim, 1 );
        giiDataArray * da = gim->darray[nda];
        gifti_set_DA_defaults( da );
        da->intent = NIFTI_INTENT_SHAPE;
        da->num_dim = 2;
        da->dims[0] = tex.size();
        da->dims[2] = 0;
        da->dims[3] = 0;
        da->dims[4] = 0;
        da->dims[5] = 0;
        giftiFillTextureBuffer( gim, da, tex );
      }
    }


    template <typename T>
    void giftiAddTextureObject( gifti_image* gim, carto::Object texture,
                                const PythonHeader & hdr )
    {
      TimeTexture<T> & tex = texture->value<TimeTexture<T> >();
      unsigned i, n = tex.size();
      for( i=0; i<n; ++i )
        giftiAddTexture( gim, tex[i].data(), hdr );
    }


    carto::Object giftiFindHdrDA( int & nda, carto::Object dainfo,
                                  const std::string & intent )
    {
      if( dainfo.isNone() )
        return carto::none();
      carto::Object inf;
      carto::Object it = dainfo->objectIterator();
      int i;
      std::string intentit;
      for( i=0; i<nda && it->isValid(); ++i, it->next() ) {}
      for( ; it->isValid(); ++i, it->next() )
      {
        carto::Object el = it->currentValue();
        if( el->getProperty( "intent", intentit ) )
        {
          if( intent.empty() )
          {
            if( intentit == "NIFTI_INTENT_POINTSET"
              || intentit == "NIFTI_INTENT_VECTOR"
              || intentit == "NIFTI_INTENT_TRIANGLE" )
              continue;
          }
          else
            if( intentit != intent )
              continue;
          inf = el;
          nda = i;
          break;
        }
      }

      return inf;
    }


    void giftiCopyMetaToGii( carto::Object dainf, giiDataArray *da )
    {
      std::cout << "giftiCopyMetaToGii\n";
      std::cout << dainf->size() << std::endl;
      carto::PythonWriter pw;
      pw.attach( std::cout );
      pw.write( dainf );
      if( dainf->hasProperty( "GIFTI_coordinates_systems" ) )
      {
        std::cout << "GIFTI_coordinates_systems found\n";
        carto::Object cs
          = dainf->getProperty( "GIFTI_coordinates_systems" );
        std::cout << "cs: " << cs << std::endl;
        carto::Object it = cs->objectIterator();
        std::cout << "it: " << it << std::endl;
        for( ; it->isValid(); it->next() )
        {
          carto::Object lcs = it->currentValue();
          std::vector<float> tr;
          if( lcs->getProperty( "transformations", tr ) )
          {
            std::cout << "transfo\n";
          }
        }
      }
      if( dainf->hasProperty( "GIFTI_metadata" ) )
      {
        carto::Object md = dainf->getProperty( "GIFTI_metadata" );
        carto::Object it = md->objectIterator();
        for( ; it->isValid(); it->next() )
        {
          carto::Object val = it->currentValue();
          if( val.isNull() )
            gifti_add_to_meta( &da->meta, it->key().c_str(), 0, 1 );
          else
            try
            {
              gifti_add_to_meta( &da->meta, it->key().c_str(),
                                 val->getString().c_str(), 1 );
            }
            catch( ... )
            {
            }
        }
      }
    }

}


  template<int D, typename T>
  bool GiftiMeshFormat<D, T>::write( const std::string & filename,
                                     const AimsTimeSurface<D, T> & thing,
                                     bool )
  {
    try
      {
        // std::cout << "GiftiMeshFormat<D, T>::write\n";
        std::string fname = filename;
        if( fname.length() < 4
          || fname.substr( fname.length() - 4, 4 ) != ".gii" )
          fname += ".gii";
        gifti_image * gim = gifti_create_image( -1, -1, -1, -1, 0, 0 );

        int nda = 0, t = 0;
        const PythonHeader & thdr = thing.header();
        GiftiHeader hdr( fname );
        hdr.copy( thdr );

        // metadata
        if( thdr.hasProperty( "GIFTI_metadata" ) )
        {
          carto::Object md = thdr.getProperty( "GIFTI_metadata" );
          carto::Object it = md->objectIterator();
          for( ; it->isValid(); it->next() )
          {
            carto::Object val = it->currentValue();
            if( val.isNull() )
              gifti_add_to_meta( &gim->meta, it->key().c_str(), 0, 0 );
            else
              try
              {
                gifti_add_to_meta( &gim->meta, it->key().c_str(),
                                   val->getString().c_str(), 0 );
              }
              catch( ... )
              {
              }
          }
        }
        // TODO: should we put all .minf properties in Gifti metadata ?

        int hdrmeshda = 0, hdrnormda = 0, hdrpolyda = 0, hdrtexa = 0;
        carto::Object da_info;
        try
        {
          da_info = thdr.getProperty( "GIFTI_dataarrays_info" );
        }
        catch( ... )
        {
        }

        typename AimsTimeSurface<D, T>::const_iterator is, es=thing.end();
        for( is=thing.begin(); is!=es; ++is, ++t )
        {
          const AimsSurface<D, T> & surf = is->second;
          // write vertices
          {
            const std::vector<Point3df> & vert = surf.vertex();
            nda = gim->numDA;
            gifti_add_empty_darray( gim, 1 );
            giiDataArray * da = gim->darray[nda];
            gifti_set_DA_defaults( da );
            da->intent = NIFTI_INTENT_POINTSET;
            da->datatype = NIFTI_TYPE_FLOAT32;
            da->num_dim = 2;
            da->dims[0] = vert.size();
            da->dims[1] = 3;
            da->dims[2] = 0;
            da->dims[3] = 0;
            da->dims[4] = 0;
            da->dims[5] = 0;
            da->nvals = vert.size() * 3;
            da->nbyper = 4;
            gifti_alloc_DA_data( gim, &nda, 1 );
            unsigned i, n = vert.size();
            float *buf = reinterpret_cast<float *>( da->data );
            for( i=0; i<n; ++i )
            {
              buf[i*3] = vert[i][0];
              buf[i*3+1] = vert[i][1];
              buf[i*3+2] = vert[i][2];
            }
            if( t != is->first )
            {
              // store timestep
              std::ostringstream ts;
              ts << is->first;
              gifti_add_to_meta( &da->meta, "Timestep", ts.str().c_str(), 1 );
            }
            // metadata
            carto::Object dainf = giftiFindHdrDA( hdrmeshda, da_info,
                                                  "NIFTI_INTENT_POINTSET" );
            if( !dainf.isNone() )
            {
              std::cout << "mesh DA meta found: " << hdrmeshda << std::endl;
              ++hdrmeshda;
              giftiCopyMetaToGii( dainf, da );
            }

          }
          // write normals
          const std::vector<Point3df> & norm = surf.normal();
          if( !norm.empty() )
          {
            nda = gim->numDA;
            gifti_add_empty_darray( gim, 1 );
            giiDataArray * da = gim->darray[nda];
            gifti_set_DA_defaults( da );
            da->intent = NIFTI_INTENT_VECTOR;
            da->datatype = NIFTI_TYPE_FLOAT32;
            da->num_dim = 2;
            da->dims[0] = norm.size();
            da->dims[1] = 3;
            da->dims[2] = 0;
            da->dims[3] = 0;
            da->dims[4] = 0;
            da->dims[5] = 0;
            da->nvals = norm.size() * 3;
            da->nbyper = 4;
            gifti_alloc_DA_data( gim, &nda, 1 );
            unsigned i, n = norm.size();
            float *buf = reinterpret_cast<float *>( da->data );
            for( i=0; i<n; ++i )
            {
              buf[i*3] = norm[i][0];
              buf[i*3+1] = norm[i][1];
              buf[i*3+2] = norm[i][2];
            }
          }
          // write polygons
          {
            const std::vector<AimsVector<unsigned, D> > & poly
              = surf.polygon();
            nda = gim->numDA;
            gifti_add_empty_darray( gim, 1 );
            giiDataArray * da = gim->darray[nda];
            gifti_set_DA_defaults( da );
            da->intent = NIFTI_INTENT_TRIANGLE;
            da->datatype = NIFTI_TYPE_UINT32;
            da->num_dim = 2;
            da->dims[0] = poly.size();
            da->dims[1] = D;
            da->dims[2] = 0;
            da->dims[3] = 0;
            da->dims[4] = 0;
            da->dims[5] = 0;
            da->nvals = poly.size() * D;
            da->nbyper = 4;
            gifti_alloc_DA_data( gim, &nda, 1 );
            unsigned i, j, n = poly.size();
            unsigned *buf = reinterpret_cast<unsigned *>( da->data );
            for( i=0; i<n; ++i )
              for( j=0; j<D; ++j )
                buf[i*D+j] = poly[i][j];
          }
          // write texture
          const std::vector<T> & tex = surf.texture();
          giftiAddTexture( gim, tex, thdr );
        }

        // add external textures
        try
        {
          carto::Object tex = thdr.getProperty( "textures" );
          carto::Object it = tex->objectIterator();
          for( ; it->isValid(); it->next() )
          {
            carto::Object texture = it->currentValue();
            std::string ttype = texture->type();
            if( ttype == carto::DataTypeCode<TimeTexture<float> >::name() )
              giftiAddTextureObject<float>( gim, texture, thdr );
            else if( ttype
              == carto::DataTypeCode<TimeTexture<Point2df> >::name() )
              giftiAddTextureObject<Point2df>( gim, texture, thdr );
            else if( ttype
              == carto::DataTypeCode<TimeTexture<int16_t> >::name() )
              giftiAddTextureObject<int16_t>( gim, texture, thdr );
            else if( ttype
              == carto::DataTypeCode<TimeTexture<int32_t> >::name() )
              giftiAddTextureObject<int32_t>( gim, texture, thdr );
            else if( ttype
              == carto::DataTypeCode<TimeTexture<uint32_t> >::name() )
              giftiAddTextureObject<uint32_t>( gim, texture, thdr );
            else if( ttype
              == carto::DataTypeCode<TimeTexture<Point2d> >::name() )
              giftiAddTextureObject<Point2d>( gim, texture, thdr );
            else
              std::cerr << "GIFTI writer warning: cannot save texture of type "
              << ttype << std::endl;
          }
        }
        catch( ... )
        {
        }

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
        hdr.writeMinf( fname + ".minf" );
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
