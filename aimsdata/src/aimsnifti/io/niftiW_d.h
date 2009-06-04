/* Copyright (c) 1995-2005 CEA
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

#ifndef AIMS_IO_NIFTIW_D_H
#define AIMS_IO_NIFTIW_D_H

#include <aims/io/niftiW.h>
#include <aims/io/datatypecode.h>
#include <aims/def/settings.h>
#include <aims/io/niftiheader.h>
#include <aims/io/defaultItemW.h>
#include <cartobase/stream/fileutil.h>
#include <cartobase/exception/ioexcept.h>
#include <aims/resampling/motion.h>
#include <aims/io/scaledcoding.h>
#include <cartobase/stream/fdinhibitor.h>
#include <string>
#include <stdio.h>


namespace
{
  template <typename T>
  void dataTOnim( nifti_image *nim, aims::NiftiHeader& hdr,
                  const AimsData<T>& thing, int tt = 0 );
}


namespace aims
{

  template <class T>
  inline 
  std::string NiftiWriter<T>::removeExtension( const std::string& name )
  {
    return std::string( nifti_makebasename( name.c_str() ) );
  }

  template <class T>
  void NiftiWriter<T>::write( const AimsData<T>& thing )
  {
    // If .hdr/.img extension then give the hand to SPM/Analyze writer
    char *cext = nifti_find_file_extension( _name.c_str() );
    std::string ext, name = _name;
    if( cext )
      ext = cext;
    else
    {
      ext = ".nii";
      name += ext;
    }
    if((ext == ".hdr") || (ext == ".img"))
    {
      throw carto::wrong_format_error( _name );
    }

    // std::cout << "This is NIFTI Writer: use at your own risk." << std::endl;
    std::cout << "name: " << name << std::endl;

    NiftiHeader hdr( thing.dimX(), thing.dimY(), thing.dimZ(), thing.dimT(),
                    thing.sizeX(), thing.sizeY(), thing.sizeZ(),
                    thing.sizeT(), name );

    const PythonHeader 
      *ph = dynamic_cast<const PythonHeader *>( thing.header() );
    std::string	code = carto::DataTypeCode<T>().dataType();
    if( ph )
    {
      hdr.copy( *ph );
      std::vector<int> dims(4);
      dims[0] = thing.dimX();
      dims[1] = thing.dimY();
      dims[2] = thing.dimZ();
      dims[3] = thing.dimT();
      hdr.setProperty( "volume_dimension", dims );
      std::vector<float> vs(4);
      vs[0] = thing.sizeX();
      vs[1] = thing.sizeY();
      vs[2] = thing.sizeZ();
      vs[3] = thing.sizeT();
      hdr.setProperty( "voxel_size", vs );
      // remove specific attribute that may be incompatible
      if( hdr.hasProperty( "disk_data_type" ) )
        hdr.removeProperty( "disk_data_type" );
      if( hdr.hasProperty( "possible_data_types" ) )
        hdr.removeProperty( "possible_data_types" );
      hdr.setProperty( "data_type", code );
    }

    if( hdr.hasProperty( "scale_factor" ) )
      hdr.removeProperty( "scale_factor" );
    if( hdr.hasProperty( "scale_offset" ) )
      hdr.removeProperty( "scale_offset" );
    int x, y, z, f;
    T vmin = thing( 0 ), vmax = thing( 0 );

    // Can float data be saved as integer without loss?
    if( code == "FLOAT" || code == "DOUBLE" )
    {
      // double maxm = 0;
      float scale = 1, offset = 0;
      bool shen = canEncodeAsScaledS16( *thing.volume(), scale, offset, true,
                                         0 /* &maxm */ );
      if( shen )
      {
        /* std::cout << "16 bit coding possible: scale: " << scale
            << ", offset: " << offset << ", max error: " << maxm << std::endl;
        */
        hdr.setProperty( "disk_data_type",
                         carto::DataTypeCode<int16_t>().dataType() );
        hdr.setProperty( "scale_factor_applied", true );
        hdr.setProperty( "scale_factor", scale );
        hdr.setProperty( "scale_offset", offset );
      }
      /* else
      std::cout << "matching interval not found. Not 16 bit codable\n"; */
    }
    ForEach4d( thing, x, y, z, f )
    {
      if( thing( x, y, z, f ) < vmin )
            vmin = thing( x, y, z, f );
      if( thing( x, y, z, f ) > vmax )
            vmax = thing( x, y, z, f );
    }
    //hdr.setProperty( "bits_allocated", (int) ( sizeof( T ) * 8 ) );
    //hdr.setProperty( "minimum", (int) vmin ); // cal_min ?
    //hdr.setProperty( "maximum", (int) vmax ); // cal_max ?

    const Settings sett = Settings::settings();
    bool  write4d = true;
    try
    {
      write4d = (bool) 
        sett.getProperty( "nifti_output_4d_volumes" )->getScalar();
    }
    catch( std::exception & )
    {
    }

    bool ok = true;
    carto::fdinhibitor fdi( 2 );
    fdi.close(); // disable output on stderr
    if( write4d || thing.dimT() == 1 )
    {
      // fill in nifti_image struct from properties
      hdr.fillNim();

      // get nifti_image as filled in by NiftiHeader
      nifti_image *nim = hdr.niftiNim();

      nim->data = (void *)calloc(1, nifti_get_volsize(nim));

      dataTOnim(nim, hdr, thing);

      // write Nifti header and data
      znzFile zfp = nifti_image_write_hdr_img( nim, 3, "wb" );
      // don't close file because we have no other way to know it it failed
      if( znz_isnull( zfp ) )
        ok = false;

      if( ok )
      {
        znzclose(zfp);
        hdr.writeMinf(std::string(nim->iname) + ".minf");
      }

      // unload data in the nifti_image struct
      nifti_image_unload( nim );
    }
    else
    {
      // std::cout << "Converting from 4D to 3D." << std::endl;
      char sequence[16];
      int  nt = thing.dimT();
      std::vector<std::string> fnames;
      std::string dname, bname;
      dname = carto::FileUtil::dirname( name )
          + carto::FileUtil::separator();
      bname = carto::FileUtil::basename( removeExtension( name ) );
      fnames.reserve( nt );
      for( f=0; f<nt; ++f )
      {
        sprintf( sequence, "%04d", f );
        fnames.push_back( bname + std::string( sequence ) + ext );
      }
      hdr.setProperty( "series_filenames", fnames );

      for( f=0; f < nt; ++f )
      {
        hdr.setName( dname + fnames[f] );
        hdr.fillNim( false ); // allow4D set to false
        nifti_image *nim = hdr.niftiNim();

        nim->data = (void *)calloc(1, nifti_get_volsize(nim));

        dataTOnim(nim, hdr, thing, f);

        znzFile zfp = nifti_image_write_hdr_img( nim, 3, "wb" );
        if( znz_isnull( zfp ) )
          ok = false;
        else
          znzclose( zfp );
        if( f == 0 && ok )
          hdr.writeMinf(std::string(nim->iname) + ".minf");
        nifti_image_unload( nim );
        if( !ok )
          break;
      }
    }
    fdi.open(); // enable stderr
    if( !ok )
      carto::io_error::launchErrnoExcept();
  }

} // namespace aims

namespace
{
  void dataTOnim_checks2m( Motion & m, const Point3df & p,
                           const Point3df & tp, int ip, int il )
  {
    if( tp[il] != 0 )
    {
      if( tp[il] < 0 )
        m.translation()[il] = p[ip];
      else
        m.translation()[il] = 0;
    }
  }


  template <typename T>
  void dataTOnim( nifti_image *nim, aims::NiftiHeader& hdr,
                  const AimsData<T>& thing, int tt = 0 )
  {
    std::vector< float > storage_to_memory;
    Motion m;
    if( hdr.getProperty( "storage_to_memory", storage_to_memory ) )
    {
      m = storage_to_memory;
      /* adjust translations so that they fit (in case the vol size has
         changed) */
      Point3df p( nim->nx - 1, 0, 0 ), p0( 0, 0, 0 );
      Point3df tp = m.transform( p ) - m.transform( p0 );
      dataTOnim_checks2m( m, p, tp, 0, 0 );
      dataTOnim_checks2m( m, p, tp, 0, 1 );
      dataTOnim_checks2m( m, p, tp, 0, 2 );
      p = Point3df( 0, nim->ny - 1, 0 );
      tp = m.transform( p ) - m.transform( p0 );
      dataTOnim_checks2m( m, p, tp, 1, 0 );
      dataTOnim_checks2m( m, p, tp, 1, 1 );
      dataTOnim_checks2m( m, p, tp, 1, 2 );
      p = Point3df( 0, 0, nim->nz - 1 );
      tp = m.transform( p ) - m.transform( p0 );
      dataTOnim_checks2m( m, p, tp, 2, 0 );
      dataTOnim_checks2m( m, p, tp, 2, 1 );
      dataTOnim_checks2m( m, p, tp, 2, 2 );
      hdr.setProperty( "storage_to_memory", m.toVector() );
    }
    else
    {
      m.translation()[0] = nim->nx - 1;
      m.translation()[1] = nim->ny - 1;
      m.translation()[2] = nim->nz - 1;
      m.rotation()(0,0) = -1.0;
      m.rotation()(1,1) = -1.0;
      m.rotation()(2,2) = -1.0;
    }

    int tmin, tmax;
    if(tt == 0)
    {
      tmin = 0;
      tmax = nim->nt;
    }
    else
    {
      tmin = tt;
      tmax = tt+1;
    }

    Point3df incf = m.transform( Point3df( 1, 0, 0 ) )
        - m.transform( Point3df( 0, 0, 0 ) );
    Point4d inc = Point4d( int16_t( rint( incf[0] ) ),
                          int16_t( rint( incf[1] ) ),
                          int16_t( rint( incf[2] ) ), 0 );
    Point3df d0f;
    Point4d d0;

    bool scalef;
    std::vector<float> s(2);
    if( hdr.getProperty( "scale_factor_applied", scalef ) && scalef
        && hdr.getProperty( "scale_factor", s[0] )
        && hdr.getProperty( "scale_offset", s[1] ) )
    {
      int16_t *d = (int16_t*) nim->data;
      for( int t=tmin; t<tmax; ++t )
        for( int z=0; z<nim->nz; ++z )
          for( int y=0; y<nim->ny; ++y )
          {
            d0f = m.transform( Point3df( 0, y, z ) );
            d0 = Point4d( int16_t( rint( d0f[0] ) ), int16_t( rint( d0f[1] ) ),
                          int16_t( rint( d0f[2] ) ), t );
            for( int x=0; x<nim->nx; ++x, d0+=inc )
              *d++ = (int16_t) rint( (thing(d0) - s[1]) / s[0] );
          }
    }
    else
    {
      T *d = (T*)nim->data;

      for( int t=tmin; t<tmax; ++t )
        for( int z=0; z<nim->nz; ++z )
          for( int y=0; y<nim->ny; ++y )
          {
            d0f = m.transform( Point3df( 0, y, z ) );
            d0 = Point4d( int16_t( rint( d0f[0] ) ), int16_t( rint( d0f[1] ) ),
                          int16_t( rint( d0f[2] ) ), t );
            for( int x=0; x<nim->nx; ++x, d0+=inc )
              *d++ = thing(d0);
          }
    }
  }

} // namespace {}

#endif
