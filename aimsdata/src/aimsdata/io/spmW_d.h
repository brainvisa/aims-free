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

#ifndef AIMS_IO_SPMW_D_H
#define AIMS_IO_SPMW_D_H

#include <aims/io/spmW.h>
#include <aims/io/datatypecode.h>
#include <aims/def/settings.h>
#include <aims/io/spmheader.h>
#include <aims/io/defaultItemW.h>
#include <aims/io/scaledcoding.h>
#include <aims/resampling/motion.h>
#include <cartobase/stream/fileutil.h>
#include <stdio.h>

namespace aims
{

  template <class T>
  void SpmWriter<T>::write( const AimsData<T>& thing )
  {
    DefaultItemWriter<T> itemW;

    SpmHeader	hdr( thing.dimX(), thing.dimY(), thing.dimZ(), thing.dimT(), 
                     thing.sizeX(), thing.sizeY(), thing.sizeZ(),
                     thing.sizeT(), _name );

    // Flip if necessary
    const PythonHeader 
      *ph = dynamic_cast<const PythonHeader *>( thing.header() );
    if( ph )
      {
        hdr.copy( *ph );
        std::vector<int>	dims(4);
        dims[0] = thing.dimX();
        dims[1] = thing.dimY();
        dims[2] = thing.dimZ();
        dims[3] = thing.dimT();
        hdr.setProperty( "volume_dimension", dims );
        std::vector<float>	vs(4);
        vs[0] = thing.sizeX();
        vs[1] = thing.sizeY();
        vs[2] = thing.sizeZ();
        vs[3] = thing.sizeT();
        hdr.setProperty( "voxel_size", vs );
        // remove specific attribute that may be incompatible
        if( hdr.hasProperty( "SPM_data_type" ) )
          hdr.removeProperty( "SPM_data_type" );
        if( hdr.hasProperty( "disk_data_type" ) )
          hdr.removeProperty( "disk_data_type" );
        if( hdr.hasProperty( "possible_data_types" ) )
          hdr.removeProperty( "possible_data_types" );
      }

    std::string	code = carto::DataTypeCode<T>().dataType();
    hdr.setupWriteAttributes( code );

    int	y, z, x, dx, f;
    T		vmin = thing( 0 ), vmax = thing( 0 );
    bool	shen = false;
    float	offset = 0, scale = 1;

    if( code == "FLOAT" || code == "DOUBLE" )
    {
      double maxm = 0;
      shen = canEncodeAsScaledS16( *thing.volume(), scale, offset, false,
                                    &maxm );
      if( shen )
      {
        std::cout << "16 bit coding possible: scale: " << scale
                  << ", max error: " << maxm << std::endl;
        hdr.setProperty( "disk_data_type",
                          carto::DataTypeCode<int16_t>().dataType()
                          );
        hdr.setProperty( "scale_factor_applied", int(0) );
        hdr.setProperty( "scale_factor", scale );
      }
    else
      std::cout << "matching interval not found. Not 16 bit codable\n";
    }
    ForEach4d( thing, x, y, z, f )
    {
      if( thing( x, y, z, f ) < vmin )
        vmin = thing( x, y, z, f );
      if( thing( x, y, z, f ) > vmax )
        vmax = thing( x, y, z, f );
    }

    if( shen )
      hdr.setProperty( "bits_allocated", (int) 16 );
    else
      hdr.setProperty( "bits_allocated", (int) ( sizeof( T ) * 8 ) );
    hdr.setProperty( "minimum", (int) vmin ) ;
    hdr.setProperty( "maximum", (int) vmax ) ;

    const Settings	sett = Settings::settings();
    bool	write4d = true;
    try
    {
      write4d = (bool)
        sett.getProperty( "spm_output_4d_volumes" )->getScalar();
    }
    catch( std::exception & )
    {
    }

    // prepare header without writing it
    hdr.write( false, write4d );

    std::vector<float>  vstom;
    Motion  stom;
    if( hdr.getProperty( "storage_to_memory", vstom ) )
      stom = Motion( vstom );
    else
    {
      std::cout << "no storage_to_memory in SPM header\n";
      // no storage orientation: take a default one
      stom.rotation()(1,1) = -1;
      stom.rotation()(2,2) = -1;
      stom.translation()[1] = thing.dimY() - 1;
      stom.translation()[2] = thing.dimZ() - 1;
      bool        radio = true;
      try
      {
        radio = (bool) hdr.getProperty( "spm_radio_convention" )->getScalar();
      }
      catch( std::exception & )
      {
      }
      if( !radio )
      {
        stom.rotation()(0,0) = -1;
        stom.translation()[0] = thing.dimX() - 1;
      }
    }

    Motion  m2s = stom.inverse();
    Point3df df = m2s.transform( Point3df( thing.dimX(), thing.dimY(),
                                    thing.dimZ() ) )
        - m2s.transform( Point3df( 0, 0, 0 ) );
    Point4d tdims = Point4d( short( rint( fabs( df[0] ) ) ), 
                   short( rint( fabs( df[1] ) ) ),
                   short( rint( fabs( df[2] ) ) ), thing.dimT() );
    Point3df incf = stom.transform( Point3df( 1, 0, 0 ) )
        - stom.transform( Point3df( 0, 0, 0 ) );
    Point4d inc = Point4d( int16_t( rint( incf[0] ) ), 
                           int16_t( rint( incf[1] ) ),
                           int16_t( rint( incf[2] ) ), 0 );

    dx = tdims[0];
    std::vector<int16_t> datashort;
    DefaultItemWriter<int16_t> itemWs;
    if( shen )
      datashort.insert( datashort.end(), dx, 0 );
    std::vector<T> data( dx );

    Point3df d0f;
    Point4d d0;

    if( write4d || thing.dimT() == 1 )
    {
      std::string fname = removeExtension( _name );
      hdr.setName( fname );
      hdr.write();

      std::string name = removeExtension( fname ) + ".img";
      _os.open( name.c_str(), std::ios::out | std::ios::binary );
      _os.unsetf( std::ios::skipws );

      for ( f=0; f < tdims[3]; f++)
        for ( z=0;z<tdims[2];z++)
        {
          for ( y=0;y<tdims[1];y++)
          {
            d0f = stom.transform( Point3df( 0, y, z ) );
            d0 = Point4d( int16_t( rint( d0f[0] ) ), int16_t( rint( d0f[1] ) ),
                          int16_t( rint( d0f[2] ) ), f );
            if( shen )
            {
              for( x=0; x < dx; ++x, d0+=inc )
                datashort[ x ] = (int16_t) rint( thing( d0 ) / scale );
              itemWs.write( _os, &datashort[0] , dx );
            }
            else if( inc == Point4d( 1, 0, 0, 0 ) )
              itemW.write( _os, &thing( d0 ) , dx );
            else
            {
              for( x=0; x < dx; ++x, d0+=inc )
                data[ x ] = thing( d0 );
              itemW.write( _os, &data[0] , dx );
            }
          }
        }

      _os.close();
    }
    else
    {
      char			sequence[16];
      int			f, nt = thing.dimT();
      std::vector<std::string>	fnames;
      std::string		dname, bname;
      dname = carto::FileUtil::dirname( _name )
        + carto::FileUtil::separator();
      bname = carto::FileUtil::basename( removeExtension( _name ) );

      fnames.reserve( nt );
      for( f=0; f<nt; ++f )
      {
        sprintf( sequence, "%04d", f );
        fnames.push_back( bname + std::string( sequence ) + ".img" );
      }
      hdr.setProperty( "series_filenames", fnames );

      // Do the job for each frame
      for( f=0; f < nt; ++f )
      {
        std::string name = dname + fnames[f];
        hdr.setName( name );
        hdr.write( f == 0, false );

        _os.open( name.c_str(), std::ios::out | std::ios::binary );
        _os.unsetf( std::ios::skipws );

        for (int z=0;z<tdims[2];z++)
          for (int y=0;y<tdims[1];y++)
          {
            d0f = stom.transform( Point3df( 0, y, z ) );
            d0 = Point4d( int16_t( rint( d0f[0] ) ), int16_t( rint( d0f[1] ) ),
                          int16_t( rint( d0f[2] ) ), f );
            if( shen )
            {
              for( x=0; x < dx; ++x, d0+=inc )
                datashort[ x ] = (int16_t) rint( thing( d0 ) / scale );
              itemWs.write( _os, &datashort[0] , dx );
            }
            else if( inc == Point4d( 1, 0, 0, 0 ) )
              itemW.write( _os, &thing( d0 ) , dx );
            else
            {
              for( x=0; x < dx; ++x, d0+=inc )
                data[ x ] = thing( d0 );
              itemW.write( _os, &data[0] , dx );
            }
          }

        _os.close();
      }
    }
  }

}

#endif
