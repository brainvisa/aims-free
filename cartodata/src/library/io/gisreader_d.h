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

#ifndef CARTODATA_IO_GISREADER_D_H
#define CARTODATA_IO_GISREADER_D_H

#include <cartodata/io/gisreader.h>

namespace carto
{

#if 0
  template <typename T> 
  rc_ptr<DataSource> GisReader<T>::getDataSource( Object header, 
                                                  rc_ptr<DataSource> source, 
                                                  Object /*options*/ )
  {
    std::string		fname = source->url();
    VolumeDataSource<T>	*vds = 0;
    rc_ptr<DataSource>	pvds;

    if( !fname.empty() )
      {
        std::string	ext = FileUtil::extension( fname );
        if( ext == "dim" )
          {
            fname = FileUtil::removeExtension( fname ) + ".ima";
          }
        else if( ext != "ima" )
          {
            fname = FileUtil::removeExtension( fname ) + ".ima";
          }
        if( fname != source->url() )
          {
            if( FileUtil::fileStat( fname ).find( '+' ) != std::string::npos )
              {
                vds = new VolumeDataSource<T>
                  ( rc_ptr<DataSource>( new FileDataSource( fname ) ), 
                    header, fname );
                pvds.reset( vds );
              }
            else
              fname = source->url();
          }
      }
    if( !vds )
      {
        vds = new VolumeDataSource<T>( source, header, fname );
        pvds.reset( vds );
      }

    return pvds;
  }


  template <typename T> 
  AllocatorContext 
  GisReader<T>::getAllocatorContext( Object /*header*/, 
                                     rc_ptr<DataSource> decoder, 
                                     const AllocatorContext & basecontext, 
                                     Object /*options*/ )
  {
    AllocatorContext	context = AllocatorContext( basecontext );
    context.setDataSource( decoder );
    context.setAllowsMemoryMapping( decoder->allowsMemoryMapping() );
    return context;
  }


  template <typename T> 
  void GisReader<T>::read( Volume<T> & obj, Object /*header*/, 
                           const AllocatorContext & /*alloc*/, 
                           Object /*options*/ )
  {
    rc_ptr<DataSource>	pvds = obj.allocatorContext().dataSource();
    VolumeDataSource<T>	*vds 
      = static_cast<VolumeDataSource<T> *>( pvds.get() );
    bool	allocd = obj.allocatorContext().isAllocated();
    int		sx = vds->getSizeX();
    int		sy = vds->getSizeY();
    int		sz = vds->getSizeZ();
    int		st = vds->getSizeT();
    if( allocd 
        && obj.allocatorContext().allocatorType() 
        != AllocatorStrategy::ReadOnlyMap )
      {
        int		vx = obj.getSizeX();
        int		vy = obj.getSizeY();
        int		vz = obj.getSizeZ();
        int		vt = obj.getSizeT();
        int		dx = std::min( vx, sx );
        int		dy = std::min( vy, sy );
        int		dz = std::min( vz, sz );
        int		dt = std::min( vt, st );
        int		y, z, t, ox = 0, oy = 0, oz = 0, ot = 0;
        offset_t	len = dx * sizeof( T );
        VolumeView<T>	*vv = dynamic_cast<VolumeView<T> *>( &obj );
        if( vv )
          {
            const typename VolumeView<T>::Position4Di 
              & p = vv->posInRefVolume();
            ox = p[0];
            oy = p[1];
            oz = p[2];
            ot = p[3];
          }
        if( !vds->open( DataSource::Read ) )
          throw open_error( "source not available", vds->url() );
        /*
        std::cout << "read vol " << dx << ", " << dy << ", " << dz << ", " 
                  << dt << std::endl;
        std::cout << "vol " << vx << ", " << vy << ", " << vz << ", " 
                  << vt << std::endl;
        std::cout << "len: " << len << std::endl;
        std::cout << "vol len: " << &obj( 0, 1 ) - &obj( 0 ) << std::endl;
        */
        for( t=0; t<dt; ++t )
          for( z=0; z<dz; ++z )
            for( y=0; y<dy; ++y )
              {
                vds->at( ( sx * ( sy * ( sz * ( t + ot ) + z + oz ) 
                                  + y + oy ) + ox ) * sizeof(T) );
                if( vds->readBlock( (char *) &obj( 0, y, z, t ), len ) 
                    != (long) len )
                  io_error::launchErrnoExcept( vds->url() );
              }
      }
  }

#endif

}

#endif

