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

#ifndef CARTODATA_IO_GISFORMATWRITER_D_H
#define CARTODATA_IO_GISFORMATWRITER_D_H

#include <cartodata/wip/io/gisformatwriter.h>
#include <cartodata/io/volumedatasource.h>
#include <cartodata/volume/volumeview.h>
#include <cartobase/stream/fileutil.h>
#include <cartobase/exception/ioexcept.h>
#include <cartobase/allocator/allocator.h>
#include <cartobase/datasource/filedatasource.h>

namespace carto
{

  template <typename T>
  GisFormatWriter<T>::~GisFormatWriter()
  {
  }


  template <typename T>
  bool GisFormatWriter<T>::write( rc_ptr<DataSource> ds, 
                                  const Volume<T> & obj, Object options )
  {
    if( !obj.allocatorContext().isAllocated() 
        || obj.allocatorContext().allocatorType() 
        == AllocatorStrategy::ReadWriteMap )
      return true;

    Object	header = Object::value( PropertySet() );
    header->setProperty( "sizeX", obj.getSizeX() );
    header->setProperty( "sizeY", obj.getSizeY() );
    header->setProperty( "sizeZ", obj.getSizeZ() );
    header->setProperty( "sizeT", obj.getSizeT() );
    const PropertySet	& ps = obj.header();
    header->setProperty( "voxel_size",
                         ps.getProperty( "voxel_size" ) );
    Object	x;
    if( options )
      try
        {
          x = options->getProperty( "ascii" );
          if( x )
            {
              bool ascii = (bool) x->getScalar();
              header->setProperty( "ascii", ascii );
            }
        }
      catch( ... )
        {
        }

    rc_ptr<DataSource>	pvds;
    VolumeDataSource<T>	*vds = 0;
    if( !dynamic_cast<VolumeDataSource<T> *>( ds.get() ) )
      {
        std::string		fname = ds->url();

        if( !fname.empty() )
          {
            // TODO: URL transformation and URL->DataSource factory

            std::string	ext = FileUtil::extension( fname );
            if( ext == "dim" )
              {
                fname = FileUtil::removeExtension( fname ) + ".ima";
              }
            else if( ext != "ima" )
              {
                fname = FileUtil::removeExtension( fname ) + ".ima";
              }
            if( fname != ds->url() )
              {
                if( FileUtil::fileStat( fname ).find( '+' )
                    != std::string::npos )
                  {
                    vds = new VolumeDataSource<T>
                      ( rc_ptr<DataSource>( new FileDataSource( fname ) ), 
                        header, fname );
                    pvds.reset( vds );
                  }
                else
                  fname = ds->url();
              }
          }
        if( !vds )
          {
            vds = new VolumeDataSource<T>( ds, header, fname );
            pvds.reset( vds );
          }
      }
    else
      pvds = ds;

    vds = static_cast<VolumeDataSource<T> *>( pvds.get() );
    int		sx = vds->getSizeX();
    int		sy = vds->getSizeY();
    int		sz = vds->getSizeZ();
    int		st = vds->getSizeT();
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
    const VolumeView<T>	*vv = dynamic_cast<const VolumeView<T> *>( &obj );
    if( vv )
      {
        const typename VolumeView<T>::Position4Di 
          & p = vv->posInRefVolume();
        ox = p[0];
        oy = p[1];
        oz = p[2];
        ot = p[3];
      }
    if( !vds->open( DataSource::Write ) )
      throw open_error( "cannot open for writing", vds->url() );
    for( t=0; t<dt; ++t )
      for( z=0; z<dz; ++z )
        for( y=0; y<dy; ++y )
          {
            vds->at( ( sz * sy * sx * ( t + ot ) + sy * sx * ( z + oz )
                      + sx * ( y + oy ) + ox ) * sizeof(T) );
//             vds->at( ( sx * ( sy * ( sz * ( t + ot ) + z + oz ) 
//                               + y + oy ) + ox ) * sizeof(T) );
            if( vds->writeBlock( (char *) &obj( 0, y, z, t ), len ) 
                != (long) len )
              io_error::launchErrnoExcept( vds->url() );
          }

    // now write the .dim header
    // ... TODO ...

    return true;
  }

}

#endif

