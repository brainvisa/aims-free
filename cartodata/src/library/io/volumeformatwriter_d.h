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

#ifndef CARTODATA_IO_VOLUMEFORMATWRITER_D_H
#define CARTODATA_IO_VOLUMEFORMATWRITER_D_H
//--- cartodata --------------------------------------------------------------
#include <cartodata/io/volumeformatwriter.h>              // class declaration
#include <cartodata/volume/volume.h>                       // manipulate sizes
//--- soma-io ----------------------------------------------------------------
#include <soma-io/config/soma_config.h>
#include <soma-io/datasourceinfo/datasourceinfo.h>      // function's argument
#include <soma-io/image/imagewriter.h>              // use of member functions
#include <soma-io/utilities/minfutil.h>          // used by filterProperties()
//--- cartobase --------------------------------------------------------------
#include <cartobase/smart/rcptr.h>
#include <cartobase/object/object.h>                       // header & options
#include <cartobase/object/property.h>                     // header & options
#include <cartobase/exception/ioexcept.h>                        // exceptions
//--- system -----------------------------------------------------------------
#include <vector>
#include <iostream>
#include <string>
//--- debug ------------------------------------------------------------------
#include <cartobase/config/verbose.h>
#define localMsg( message ) cartoCondMsg( 4, message, "VOLUMEFORMATWRITER" )
// localMsg must be undef at end of file
//----------------------------------------------------------------------------

namespace soma
{
//////////////////////////////////////////////////////////////////////////////
////               V O L U M E F O R M A T W R I T E R                    ////
//////////////////////////////////////////////////////////////////////////////

  //==========================================================================
  //   C O N S T R U C T O R S
  //==========================================================================
  template <typename T>
  VolumeFormatWriter<T>::~VolumeFormatWriter()
  {
  }

  //==========================================================================
  //   F I L T E R   M E T H O D S
  //==========================================================================
  template <typename T>
  bool VolumeFormatWriter<T>::filterProperties(carto::Object header, 
                                               carto::Object options)
  {
      // Filter minf to remove irrelevant properties
      soma::MinfUtil::filter(header, options);
      
      return true;
  }
  
  //==========================================================================
  //   W R I T E   M E T H O D S
  //==========================================================================
  template <typename T>
  bool VolumeFormatWriter<T>::write( const carto::Volume<T> & obj,
                                     carto::rc_ptr<DataSourceInfo> dsi,
                                     carto::Object options )
  {
    localMsg( "writing: " + dsi->url() );
    //=== memory mapping =====================================================
    localMsg( "checking for memory mapping..." );
    if( obj.allocatorContext().allocatorType() == AllocatorStrategy::ReadWriteMap )
      return true;

    //=== multiresolution level ==============================================
    localMsg( "reading resolution level..." );
    int level = 0;
    if( options->hasProperty( "resolution_level" ) ) {
      options->getProperty( "resolution_level", level );
      if (level < 0) {
        try {
            // Try to solve negative level values
          level += dsi->header()->getProperty( "resolutions_dimension" )
                                ->size();
        }
        catch(...){}
      }
    }
    localMsg( " -> level to write : " + carto::toString( level ) );

    //=== partial reading ====================================================
    localMsg( "checking for partial writing..." );
    bool partial = false;
    if( options->hasProperty( "partial_writing" ) )
      partial = true;
    if( partial )
    {
      localMsg( " -> partial writing enabled." );
    }

    size_t dim, ndim = obj.getSize().size();
    std::vector<int> position( ndim, 0 );
    std::vector<int> view( ndim, 0 );
    std::vector<int> size( ndim, 1 );

    //=== checking if obj is a view ==========================================
    localMsg( "checking if object is a view..." );
    carto::Volume<T> *parent1 = 0;
    carto::Volume<T> *parent2 = 0;
    parent1 = obj.refVolume().get();
    if( parent1 )
      parent2 = parent1->refVolume().get();
    localMsg( std::string("object ") + ( parent1 ? "is" : "isn't" ) + " a view and "
              + ( obj.allocatorContext().isAllocated() ? "is" : "isn't" )
              + " allocated." );
    if( parent1 )
    {
      localMsg( std::string("parent1 exists and ")
                + ( parent1->allocatorContext().isAllocated() ? "is" : "isn't" )
                + " allocated." );
    }
    if( parent2 )
    {
      localMsg( std::string("parent2 exists and ")
                + ( parent2->allocatorContext().isAllocated() ? "is" : "isn't" )
                + " allocated." );
    }

    //=== view size ==========================================================
    localMsg( "reading view size..." );
    view = obj.getSize();

    //=== full volume size ===================================================
    localMsg( "reading full volume size and view position..." );
    if( parent1 && !parent1->allocatorContext().isAllocated() ) {
      localMsg( " -> from parent1" )
      size = parent1->getSize();
      position = obj.posInRefVolume();
    } else if( parent2 ) {
      localMsg( " -> from parent2" )
      size = parent2->getSize();
      position = obj.posInRefVolume();
      size_t i, rndim = parent1->posInRefVolume().size();
      if( position.size() < rndim )
        position.resize( rndim, 0 );
      for( i=0; i<rndim; ++i )
        position[ i ] += parent1->posInRefVolume()[ i ];
    } else {
      localMsg( " -> from self" )
      size = view;
    }

    if( !partial ) {
      // we treat the view as a pure Volume
      localMsg( " -> from self (no partial writing)" )
      size = view;
      position = std::vector<int>( ndim, 0 );
    }

    localMsg( " -> Full volume size : ( "
              + carto::toString( size[0] ) + ", "
              + carto::toString( size[1] ) + ", "
              + carto::toString( size[2] ) + ", "
              + carto::toString( size[3] ) + " )" );
    localMsg( " -> View size : ( "
              + carto::toString( view[0] ) + ", "
              + carto::toString( view[1] ) + ", "
              + carto::toString( view[2] ) + ", "
              + carto::toString( view[3] ) + " )" );
    localMsg( " -> View position : ( "
              + carto::toString( position[0] ) + ", "
              + carto::toString( position[1] ) + ", "
              + carto::toString( position[2] ) + ", "
              + carto::toString( position[3] ) + " )" );

    //=== checking for borders ===============================================
    localMsg( "checking for borders..." );
    bool withborders = false;
    if( parent1 && parent1->allocatorContext().isAllocated() )
      withborders = true;
    localMsg( std::string(" -> ") + ( withborders ? "with borders" : "without borders" ) );
    withborders = withborders; // compilation warning

    // handle internal orientation: go back to LPI
    const Referential & ref = obj.referential();
    std::vector<float> osz( obj.getSize().begin(), obj.getSize().end() );
    for( dim=0; dim<osz.size(); ++dim )
      --osz[dim];
    rc_ptr<Transformation3d> tolpir = ref.toOrientation( "LPI", osz );
    AffineTransformation3dBase & tolpi
      = static_cast<AffineTransformation3dBase &>( *tolpir );
    AffineTransformation3dBase itolpi = tolpi.inverse();

    std::vector<int> lpi_size = tolpi.transformVector( size );
    for( dim=0; dim<lpi_size.size(); ++dim )
      lpi_size[dim] = std::abs( lpi_size[dim] );

    //=== header info ========================================================
    localMsg( "setting header..." );
    if( !options )
      options = carto::Object::value( carto::PropertySet() );
    if( !parent1 && !obj.allocatorContext().isAllocated() )
      options->setProperty( "unallocated", true );
    if( !dsi->header() )
    {
      dsi->header() = carto::Object::value( carto::PropertySet() );
      Object lpi_header = obj.reorientedHeader( "LPI" );
      dsi->header()->copyProperties( lpi_header );
      std::vector<int> size;
      lpi_header->getProperty( "volume_dimension", size );
      dsi->header()->setProperty( "sizeX", size[0] );
      dsi->header()->setProperty( "sizeY", size[1] );
      dsi->header()->setProperty( "sizeZ", size[2] );
      dsi->header()->setProperty( "sizeT", size[3] );
    }


    //=== use optional parameters for partial writing ========================
    if( partial ) {
      try {
        position[0] = (int) rint( options->getProperty( "ox" )->getScalar() );
        localMsg( "override ox : " + carto::toString(position[0]) );
      } catch( ... ) {}
      try {
        position[1] = (int) rint( options->getProperty( "oy" )->getScalar() );
        localMsg( "override oy : " + carto::toString(position[1]) );
      } catch( ... ) {}
      try {
        position[2] = (int) rint( options->getProperty( "oz" )->getScalar() );
        localMsg( "override oz : " + carto::toString(position[2]) );
//         std::cout << "override oz : " + carto::toString(position[2]) << std::endl;
      } catch( ... ) {}
      try {
        position[3] = (int) rint( options->getProperty( "ot" )->getScalar() );
        localMsg( "override ot : " + carto::toString(position[3]) );
//         std::cout << "override ot : " + carto::toString(position[3]) << std::endl;
      } catch( ... ) {}
      size_t dim, value;
      for( dim=0; dim<carto::Volume<T>::DIM_MAX; ++dim )
      {
        try
        {
          std::stringstream skey;
          skey << "ox" << dim+1;
          std::string key = skey.str();
          value = (int) rint( options->getProperty( key )->getScalar() );
          options->removeProperty( key );
          while( position.size() <= dim )
            position.push_back( 0 );
          position[ dim ] = value;
//           std::cout << "override " << key << " : " + carto::toString(value) << std::endl;
        }
        catch( ... ) {}
      }
    }

    //=== writing header & creating files ====================================
    localMsg( "writing header..." );
    std::vector<long> strides( ndim );
    for( dim=0; dim<ndim; ++dim )
    {
      std::vector<int> pos( ndim, 0 );
      pos[dim] = 1;
      strides[dim] = &obj( pos ) - &obj(0,0,0,0);
    }
    
    this->filterProperties(dsi->header(), options);

    // handle internal orientation: go back to LPI
    std::vector<int> opos = position;
    std::vector<int> oview = view;
    const T* start = &obj(0);
    {
      opos = tolpi.transformVector( position );
      Point3di pi = itolpi.transform( 0, 0, 0 );
      start = &obj( pi );
      oview = tolpi.transformVector( view );
      for( dim=0; dim<oview.size(); ++dim )
        oview[dim] = std::abs( oview[dim] );
      pi = itolpi.transformVector( 1, 0, 0 );
      strides[0] = &obj.at( pi ) - &obj.at( 0 );
      pi = itolpi.transformVector( 0, 1, 0 );
      strides[1] = &obj.at( pi ) - &obj.at( 0 );
      pi = itolpi.transformVector( 0, 0, 1 );
      strides[2] = &obj.at( pi ) - &obj.at( 0 );
    }

    
    *dsi = _imw->writeHeader( *dsi, start, opos, oview,
                              strides, options );

    //=== sanity check =======================================================
    if( partial )
    {
      // file sizes may be different from what has been set before (since
      // the file already exists, its size has been re-read by writeHeader())
      std::vector<int> file_sz( ndim, 1 );
      if( !dsi->header()->getProperty( "volume_dimension", file_sz ) )
      {
        dsi->header()->getProperty( "sizeX", file_sz[0] );
        dsi->header()->getProperty( "sizeY", file_sz[1] );
        dsi->header()->getProperty( "sizeZ", file_sz[2] );
        dsi->header()->getProperty( "sizeT", file_sz[3] );
      }
      for( dim=0; dim<ndim; ++dim  )
      {
        if( file_sz.size() <= dim )
          file_sz.push_back( 1 );
          if( opos[dim] + oview[dim] > file_sz[dim] )
        {
          localMsg( "view is larger than the volume." );
          throw carto::format_error( "view is larger than the volume." );
        }
      }
    }

    //=== writing image ======================================================
    localMsg( "writing volume..." );
    const T* data = 0;
    if( parent1 || obj.allocatorContext().isAllocated() )
      data = start;
    // in unallocated case, data is null; this is OK.
    _imw->write( data, *dsi, opos, oview, strides, options );
//     {
// //       if( !withborders ) {
//         _imw->write( (T*) &obj(0,0,0,0), *dsi, position, view, strides,
//                      options );
// //       } else {
// //         int y, z, t;
// //         std::vector<int> posline ( position );
// //         std::vector<int> sizeline ( 4, 1 );
// //         sizeline[ 0 ] = view[ 0 ];
// //         for( t=0; t<view[3]; ++t )
// //           for( z=0; z<view[2]; ++z )
// //             for( y=0; y<view[1]; ++y ) {
// //               posline[ 1 ] = position[ 1 ] + y;
// //               posline[ 2 ] = position[ 2 ] + z;
// //               posline[ 3 ] = position[ 3 ] + t;
// //               _imw->write( (T*) &obj(0,y,z,t), *dsi, posline,
// //                           sizeline, options );
// //             }
// //       }
//     }
    // else we just needed to write the header and reserve file space
    // no image to write

    // we reset at 0 the ImageWriter's members (sizes, binary, ...) so that
    // they are recomputed at the next writing.
    _imw->resetParams();
    return true;
  }

  template <typename T>
  void VolumeFormatWriter<T>::attach( carto::rc_ptr<ImageWriter<T> > imw )
  {
    _imw = imw;
  }

//////////////////////////////////////////////////////////////////////////////
////            V O L U M E R E F F O R M A T W R I T E R                 ////
//////////////////////////////////////////////////////////////////////////////

  template <typename T>
  VolumeRefFormatWriter<T>::~VolumeRefFormatWriter()
  {
  }
  
  template <typename T>
  bool VolumeRefFormatWriter<T>::filterProperties(carto::Object header,
                                                  carto::Object options)
  {
      // Filter minf to remove irrelevant properties
      soma::MinfUtil::filter(header, options);
      
      return true;
  }
  

  template <typename T>
  bool VolumeRefFormatWriter<T>::write( const carto::VolumeRef<T> & obj,
                                        carto::rc_ptr<DataSourceInfo> dsi,
                                        carto::Object options )
  {
    VolumeFormatWriter<T> vfw;
    vfw.attach( _imw );
    return vfw.write( *obj, dsi, options );
  }

  template <typename T>
  void VolumeRefFormatWriter<T>::attach( carto::rc_ptr<ImageWriter<T> > imw )
  {
    _imw = imw;
  }

}

#undef localMsg
#endif
