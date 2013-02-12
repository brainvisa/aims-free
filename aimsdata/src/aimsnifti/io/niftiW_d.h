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

#ifndef AIMS_IO_NIFTIW_D_H
#define AIMS_IO_NIFTIW_D_H

#include <aims/io/niftiW.h>
#include <aims/io/datatypecode.h>
#include <aims/def/settings.h>
#include <aims/io/niftiheader.h>
#include <aims/io/defaultItemW.h>
#include <aims/io/fileFormat.h>
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
                  const AimsData<T>& thing, int tt, znzFile );

  template <typename T>
  bool expandNiftiScaleFactor( const aims::NiftiHeader & hdr,
                               nifti_image *nim, const Motion & m,
                               const AimsData<T> & thing, int tmin, int tmax,
                               znzFile zfp )
  {
    Point3df d0f;
    Point4d d0;
    Point3df incf = m.transform( Point3df( 1, 0, 0 ) )
        - m.transform( Point3df( 0, 0, 0 ) );
    Point4d inc = Point4d( int16_t( rint( incf[0] ) ),
                           int16_t( rint( incf[1] ) ),
                           int16_t( rint( incf[2] ) ), 0 );
    long pinc = inc[2] * ( &thing(0,1,0) - &thing(0) )
      + inc[1] * ( &thing(1) - &thing(0) ) + inc[0];
    const T *p0;
    bool scalef;
    std::vector<float> s(2);
    if( hdr.getProperty( "scale_factor_applied", scalef ) && scalef
        && hdr.getProperty( "scale_factor", s[0] )
        && hdr.getProperty( "scale_offset", s[1] ) )
    {
      size_t numbytes = nim->nx * sizeof( int16_t ), ss;
      std::vector<int16_t> buf( nim->nx );
      int16_t *d = 0;
      for( int t=tmin; t<tmax; ++t )
        for( int z=0; z<nim->nz; ++z )
          for( int y=0; y<nim->ny; ++y )
      {
        d0f = m.transform( Point3df( 0, y, z ) );
        d0 = Point4d( int16_t( rint( d0f[0] ) ), int16_t( rint( d0f[1] ) ),
                      int16_t( rint( d0f[2] ) ), t );
        d = &buf[0];
        p0 = &thing(d0);
        for( int x=0; x<nim->nx; ++x, p0 += pinc )
          *d++ = (int16_t) rint( (*p0 - s[1]) / s[0] );

        ss = znzwrite( (void*) &buf[0] , 1 , numbytes , zfp );
        if( ss != numbytes )
        {
          y = nim->ny;
          z = nim->nz;
          t = tmax;
          break;
        }
      }
      return true;
    }
    return false;
  }


  template <>
  bool expandNiftiScaleFactor( const aims::NiftiHeader &, nifti_image *,
                               const Motion &, const AimsData<AimsRGB> &,
                               int, int, znzFile )
  {
    return false;
  }


  template <>
  bool expandNiftiScaleFactor( const aims::NiftiHeader &, nifti_image *,
                               const Motion &, const AimsData<AimsRGBA> &,
                               int, int, znzFile )
  {
    return false;
  }

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
  void NiftiWriter<T>::write( const AimsData<T>& thing, carto::Object options )
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
      // if .img/.hdr, then say we use the Analyze format if it is available
      FileFormat<AimsData<T> > *fmt
        = FileFormatDictionary<AimsData<T> >::fileFormat( "SPM" );
      if( fmt )
        try
        {
          if( fmt->write( name, thing, options ) )
            return;
        }
        catch( ... )
        {
        }
    }

    // std::cout << "This is NIFTI Writer: use at your own risk." << std::endl;

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

    if( thing.dimX() >= 0x8000 || thing.dimY() >= 0x8000
      || thing.dimZ() >= 0x8000 )
      throw carto::invalid_format_error( "NIFTI-1 cannot handle volume "
        "dimensions exceeding 32737", name );
    int ntime = thing.dimT();
    if( write4d && thing.dimT() >= 0x8000 )
    {
      ntime = 1;
      write4d = false;
    }

    NiftiHeader hdr( thing.dimX(), thing.dimY(), thing.dimZ(), ntime,
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
      dims[3] = ntime;
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

    // Can float data be saved as integer without loss?
    if( code == "FLOAT" || code == "DOUBLE" )
    {
      bool forcedDT = false;
      try
      {
        if( !options.isNull() )
        {
          carto::Object aso = options->getProperty( "force_disk_data_type" );
          if( !aso.isNull() )
            forcedDT = (bool) aso->getScalar();
        }
      }
      catch( ... )
      {
      }
      if( !forcedDT )
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

      // write Nifti header and data
      znzFile zfp = nifti_image_write_hdr_img( nim, 2, "wb" );
      // don't close file because we have no other way to know it it failed
      if( znz_isnull( zfp ) )
        ok = false;

      if( ok )
      {

        dataTOnim(nim, hdr, thing, -1, zfp );

        if( znz_isnull( zfp ) )
          ok = false;
        else
        {
          znzclose(zfp);
          hdr.writeMinf(std::string(nim->iname) + ".minf");
        }
      }

      // unload data in the nifti_image struct
      nifti_image_unload( nim );
    }
    else
    {
      char sequence[16];
      int f;
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
      std::vector<int> vdim(4);
      vdim[0] = thing.dimX();
      vdim[1] = thing.dimY();
      vdim[2] = thing.dimZ();
      vdim[3] = thing.dimT();
      hdr.setProperty( "volume_dimension", vdim );

      for( f=0; f < nt; ++f )
      {
        hdr.setName( dname + fnames[f] );
        hdr.fillNim( false ); // allow4D set to false
        nifti_image *nim = hdr.niftiNim();

        znzFile zfp = nifti_image_write_hdr_img( nim, 2, "wb" );
        if( znz_isnull( zfp ) )
          ok = false;
        else
        {
          dataTOnim(nim, hdr, thing, f, zfp);
          if( znz_isnull( zfp ) )
            ok = false;
          else
            znzclose( zfp );
        }
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
                  const AimsData<T>& thing, int tt, znzFile zfp )
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
    if(tt < 0)
    {
      tmin = 0;
      tmax = nim->nt;
    }
    else
    {
      tmin = tt;
      tmax = tt+1;
    }

    if( !expandNiftiScaleFactor( hdr, nim, m, thing, tmin, tmax, zfp ) )
    {
      Point3df d0f;
      AimsVector<int,4> d0;
      Point3df incf = m.transform( Point3df( 1, 0, 0 ) )
          - m.transform( Point3df( 0, 0, 0 ) );
      AimsVector<int,4> inc( (int) rint( incf[0] ),
                             (int) rint( incf[1] ),
                             (int) rint( incf[2] ), 0 );

      long pinc = inc[2] * ( &thing(0,1,0) - &thing(0) )
        + inc[1] * ( &thing(1) - &thing(0) ) + inc[0];
      const T *p0;
      size_t numbytes = nim->nx * sizeof( T ), ss;
      std::vector<T> buf( nim->nx );
      T *d = 0;

      for( int t=tmin; t<tmax; ++t )
        for( int z=0; z<nim->nz; ++z )
          for( int y=0; y<nim->ny; ++y )
          {
            d0f = m.transform( Point3df( 0, y, z ) );
            d0 = AimsVector<int,4>( (int) rint( d0f[0] ),
                                    (int) rint( d0f[1] ),
                                    (int) rint( d0f[2] ), t );
            p0 = &thing(d0[0], d0[1], d0[2], d0[3]);
            d = &buf[0];
            for( int x=0; x<nim->nx; ++x, p0 += pinc )
              *d++ = *p0;
            ss = znzwrite( (void*) &buf[0] , 1 , numbytes , zfp );
            if( ss != numbytes )
            {
              // ok = false;
              y = nim->ny;
              z = nim->nz;
              t = tmax;
              break;
            }
          }
    }
  }

} // namespace {}

#endif
