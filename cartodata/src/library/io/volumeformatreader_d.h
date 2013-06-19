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

#ifndef VOLUMEFORMATREADER_D_H
#define VOLUMEFORMATREADER_D_H
//--- cartodata ----------------------------------------------------------------
#include <cartodata/io/volumeformatreader.h>
#include <cartodata/volume/volumeview.h>
#include <cartodata/io/volumeutilio.h>
//--- soma io ------------------------------------------------------------------
#include <soma-io/config/soma_config.h>
#include <soma-io/datasourceinfo/datasourceinfo.h>
#include <soma-io/datasource/datasource.h> 
#include <soma-io/datasource/filedatasource.h> 
#include <soma-io/image/imagereader.h>
#include <soma-io/reader/formatreader.h>
#include <soma-io/allocator/allocator.h>
//--- cartobase ----------------------------------------------------------------
#include <cartobase/object/object.h>
#include <cartobase/smart/rcptr.h>
#include <cartobase/exception/ioexcept.h>
#include <cartobase/config/verbose.h>                         // verbosity level
#include <cartobase/stream/fileutil.h>
//--- system -------------------------------------------------------------------
#include <vector>
#include <iostream>
//------------------------------------------------------------------------------

namespace soma
{
////////////////////////////////////////////////////////////////////////////////
////                V O L U M E F O R M A T R E A D E R                     ////
////////////////////////////////////////////////////////////////////////////////

  //============================================================================
  //   C O N S T R U C T O R S
  //============================================================================
  template <typename T>
  VolumeFormatReader<T>::~VolumeFormatReader()
  {
  }

  //============================================================================
  //   U R I   O P T I O N S
  //============================================================================

//   template <typename T> void 
//   VolumeFormatReader<T>::setupAndRead( Volume<T> & obj, 
//                                        carto::rc_ptr<DataSourceInfo> dsi,
//                                        const AllocatorContext & context,
//                                        carto::Object options )
//   {
//   }

  template <typename T> Volume<T>* 
  VolumeFormatReader<T>::createAndRead( carto::rc_ptr<DataSourceInfo> dsi,
                                        const AllocatorContext & context,
                                        carto::Object options )
  {
    //// Reading URI ///////////////////////////////////////////////////////////
    std::string uri = dsi->list().dataSource( "default", 0 )->url();
    std::string url = FileUtil::uriFilename( uri );
    carto::Object urioptions = FileUtil::uriOptions( uri );
    if( urioptions.get() ) {
      if( !options.get() )
        options = carto::Object::value( carto::PropertySet() );
      options->copyProperties( urioptions );
      dsi->list().dataSource( "default", 0 ).reset( new FileDataSource(  url ) );
    }
    
    if( !options.get() )
      return FormatReader<Volume<T> >::createAndRead( dsi, context, options );
    
    std::set<std::string> prop = VolumeUtilIO<T>::listReadProperties();
    typename std::set<std::string>::iterator p;
    typename std::set<std::string>::iterator plast = prop.end();
    for( p = prop.begin(); p != prop.end(); ++p )
    {
      if( options->hasProperty( *p ) )
        return VolumeUtilIO<T>::read( dsi, options );
    }
    
    return FormatReader<Volume<T> >::createAndRead( dsi, context, options );
  }

  //============================================================================
  //   N E W   M E T H O D S
  //============================================================================
  
  //--- Reading to a Volume<T> -------------------------------------------------
  /* This method depends deeply on the data structure (Volume<T>). It is 
   * declared in FormatReader but only defined here.
   */
  template <typename T>
  void VolumeFormatReader<T>::read( carto::Volume<T> & obj, 
                                    carto::rc_ptr<DataSourceInfo> dsi, 
                                    const AllocatorContext & context, 
                                    carto::Object options )
  {
    if( carto::debugMessageLevel > 3 )
      std::cout << "VOLUMEFORMATREADER:: Reading object ( "
                << dsi->list().dataSource( "default", 0 )->url() 
                << " )" << std::endl;
    
    //=== test for memory mapping ==============================================
    if( carto::debugMessageLevel > 3 )
      std::cout << "VOLUMEFORMATREADER:: checking for memory mapping..." 
                << std::endl;
    if( obj.allocatorContext().allocatorType() 
        == AllocatorStrategy::ReadOnlyMap )
    {
      if( carto::debugMessageLevel > 3 )
      std::cout << "VOLUMEFORMATREADER:: Nothing to read -> Memory Mapping" 
                << std::endl;
      return;
    }
    
    //=== volume is a view ? ===================================================
    if( carto::debugMessageLevel > 3 )
      std::cout << "VOLUMEFORMATREADER:: checking if object is a view..." 
                << std::endl;
    carto::VolumeView<T> *vv = dynamic_cast<carto::VolumeView<T> *>( &obj );
    carto::VolumeView<T> *p1vv = 0;
    carto::Volume<T> *parent1 = 0;
    carto::Volume<T> *parent2 = 0;
    if( vv ) {
      parent1 = vv->refVolume().get();
      p1vv = dynamic_cast<carto::VolumeView<T> *>( parent1 );
      if( p1vv )
        parent2 = p1vv->refVolume().get();
    } else if( !vv && !obj.allocatorContext().isAllocated() ) {
      if( carto::debugMessageLevel > 3 )
      std::cout << "VOLUMEFORMATREADER:: Nothing to read -> Unallocated volume" 
                << std::endl;
      return;
    }
    if( carto::debugMessageLevel > 3 ) {
      std::cout << "VOLUMEFORMATREADER:: -> object "
                << ( vv ? "is" : "isn't" ) << " a view and "
                << ( obj.allocatorContext().isAllocated() ? "is" : "isn't" )
                << " allocated." << std::endl;
      if( parent1 )
        std::cout << "VOLUMEFORMATREADER:: -> parent exists and " 
                  << ( parent1->allocatorContext().isAllocated() ? "is" : "isn't" )
                  << " allocated." << std::endl;
      if( parent2 )
        std::cout << "VOLUMEFORMATREADER:: -> grandparent exists and " 
                  << ( parent2->allocatorContext().isAllocated() ? "is" : "isn't" )
                  << " allocated." << std::endl;
    }
    
    //=== view size ============================================================
    if( carto::debugMessageLevel > 3 )
      std::cout << "VOLUMEFORMATREADER:: reading view size..." 
                << std::endl;
    std::vector<int>  viewsize( 4, 0 );
    viewsize[ 0 ] = obj.getSizeX();
    viewsize[ 1 ] = obj.getSizeY();
    viewsize[ 2 ] = obj.getSizeZ();
    viewsize[ 3 ] = obj.getSizeT();
    if( carto::debugMessageLevel > 3 )
      std::cout << "VOLUMEFORMATREADER:: -> view size ( "
                << viewsize[ 0 ] << ", "
                << viewsize[ 1 ] << ", "
                << viewsize[ 2 ] << ", "
                << viewsize[ 3 ] << " )"
                << std::endl;
    
    //=== multiresolution level ================================================
    if( carto::debugMessageLevel > 3 )
      std::cout << "VOLUMEFORMATREADER:: reading multiresolution level..." 
                << std::endl;
    int level = 0;
    if( options->hasProperty( "resolution_level" ) )
        options->getProperty( "resolution_level", level );
    if( carto::debugMessageLevel > 3 )
      std::cout << "VOLUMEFORMATREADER:: -> level to read : " 
                << level << std::endl;
    
    //=== full volume size =====================================================
    if( carto::debugMessageLevel > 3 )
      std::cout << "VOLUMEFORMATREADER:: reading full volume size..." 
                << std::endl;
    std::vector<int>  imagesize( 4, 0 );
    try {
      // first we look for "resolutions_dimension" property
      imagesize[ 0 ] = dsi->header()->getProperty( "resolutions_dimension" )
                          ->getArrayItem( level )->getArrayItem( 0 )
                          ->getScalar();
      imagesize[ 1 ] = dsi->header()->getProperty( "resolutions_dimension" )
                          ->getArrayItem( level )->getArrayItem( 1 )
                          ->getScalar();
      imagesize[ 2 ] = dsi->header()->getProperty( "resolutions_dimension" )
                          ->getArrayItem( level )->getArrayItem( 2 )
                          ->getScalar();
      imagesize[ 3 ] = dsi->header()->getProperty( "resolutions_dimension" )
                          ->getArrayItem( level )->getArrayItem( 3 )
                          ->getScalar();
      if( carto::debugMessageLevel > 3 )
        std::cout << "VOLUMEFORMATREADER:: -> found \"resolutions_dimension\"." 
                  << std::endl;
    } catch( ... ) {
      try {
        // if it doesn't work, we look for "volume_dimension"
        imagesize[ 0 ] = dsi->header()->getProperty( "volume_dimension" )
                            ->getArrayItem( 0 )->getScalar();
        imagesize[ 1 ] = dsi->header()->getProperty( "volume_dimension" )
                            ->getArrayItem( 1 )->getScalar();
        imagesize[ 2 ] = dsi->header()->getProperty( "volume_dimension" )
                            ->getArrayItem( 2 )->getScalar();
        imagesize[ 3 ] = dsi->header()->getProperty( "volume_dimension" )
                            ->getArrayItem( 3 )->getScalar();
        if( carto::debugMessageLevel > 3 )
          std::cout << "VOLUMEFORMATREADER:: -> found \"volume_dimension\"." 
                    << std::endl;
      } catch( ... ) {
        // if still nothing, we look for parent volumes
        if( parent1 && !parent1->allocatorContext().isAllocated() ) {
          imagesize[ 0 ] = parent1->getSizeX();
          imagesize[ 1 ] = parent1->getSizeY();
          imagesize[ 2 ] = parent1->getSizeZ();
          imagesize[ 3 ] = parent1->getSizeT();
          if( carto::debugMessageLevel > 3 )
            std::cout << "VOLUMEFORMATREADER:: -> found unallocated parent." 
                      << std::endl;
        } else if( parent2 ) {
          imagesize[ 0 ] = parent2->getSizeX();
          imagesize[ 1 ] = parent2->getSizeY();
          imagesize[ 2 ] = parent2->getSizeZ();
          imagesize[ 3 ] = parent2->getSizeT();
          if( carto::debugMessageLevel > 3 )
            std::cout << "VOLUMEFORMATREADER:: -> found grandparent." 
                      << std::endl;
        } else {
          imagesize = viewsize;
          if( carto::debugMessageLevel > 3 )
            std::cout << "VOLUMEFORMATREADER:: -> full volume is self." 
                      << std::endl;
        }
      }
    }
    if( carto::debugMessageLevel > 3 )
      std::cout << "VOLUMEFORMATREADER:: -> full volume size ( "
                << imagesize[ 0 ] << ", "
                << imagesize[ 1 ] << ", "
                << imagesize[ 2 ] << ", "
                << imagesize[ 3 ] << " )"
                << std::endl;
    
    //--- allocated volume size ------------------------------------------------
    if( carto::debugMessageLevel > 3 )
      std::cout << "VOLUMEFORMATREADER:: reading allocated size..." 
                << std::endl;
    std::vector<int> allocsize( 4, 0 );
    if( !vv ) {
      allocsize = viewsize;
      if( carto::debugMessageLevel > 3 )
        std::cout << "VOLUMEFORMATREADER:: "
                  << "-> allocated volume is self (full volume)." 
                  << std::endl;
    } else if( !parent1->allocatorContext().isAllocated() ) {
      allocsize = viewsize;
      if( carto::debugMessageLevel > 3 )
        std::cout << "VOLUMEFORMATREADER:: "
                  << "-> allocated volume is self (partial volume)." 
                  << std::endl;
    } else if( parent1 && parent1->allocatorContext().isAllocated() ) {
      allocsize[0] = parent1->getSizeX();
      allocsize[1] = parent1->getSizeY();
      allocsize[2] = parent1->getSizeZ();
      allocsize[3] = parent1->getSizeT();
      if( carto::debugMessageLevel > 3 )
        std::cout << "VOLUMEFORMATREADER:: "
                  << "-> allocated volume is parent "
                  << "(borders or partially loading in full volume)." 
                  << std::endl;
    }
    if( carto::debugMessageLevel > 3 )
      std::cout << "VOLUMEFORMATREADER:: -> allocated volume size ( "
                << allocsize[ 0 ] << ", "
                << allocsize[ 1 ] << ", "
                << allocsize[ 2 ] << ", "
                << allocsize[ 3 ] << " )"
                << std::endl;

    //--- strides --------------------------------------------------------------
    // TODO - for now we don't use them
    std::vector<int> strides;

    //--- region's origine -----------------------------------------------------
    if( carto::debugMessageLevel > 3 )
      std::cout << "VOLUMEFORMATREADER:: reading view position in reference to full volume..." 
                << std::endl;
    std::vector<int>  pos ( 4 , 0 );
    if( parent1 && !parent1->allocatorContext().isAllocated() ) {
      pos[ 0 ] = vv->posInRefVolume()[ 0 ];
      pos[ 1 ] = vv->posInRefVolume()[ 1 ];
      pos[ 2 ] = vv->posInRefVolume()[ 2 ];
      pos[ 3 ] = vv->posInRefVolume()[ 3 ];
    } else if( parent2 ) {
      pos[ 0 ] = vv->posInRefVolume()[ 0 ] + p1vv->posInRefVolume()[ 0 ];
      pos[ 1 ] = vv->posInRefVolume()[ 1 ] + p1vv->posInRefVolume()[ 1 ];
      pos[ 2 ] = vv->posInRefVolume()[ 2 ] + p1vv->posInRefVolume()[ 2 ];
      pos[ 3 ] = vv->posInRefVolume()[ 3 ] + p1vv->posInRefVolume()[ 3 ];
    }
    
    if( carto::debugMessageLevel > 3 )
      std::cout << "VOLUMEFORMATREADER:: -> view position ( "
                << pos[ 0 ] << ", "
                << pos[ 1 ] << ", "
                << pos[ 2 ] << ", "
                << pos[ 3 ] << " )"
                << std::endl;

    //--- possibilities : with borders, partial reading ------------------------
    bool withborders = allocsize[0] > viewsize[0] ||
                       allocsize[1] > viewsize[1] ||
                       allocsize[2] > viewsize[2] ||
                       allocsize[3] > viewsize[3];
    bool partialreading = imagesize[0] > viewsize[0] ||
                          imagesize[1] > viewsize[1] ||
                          imagesize[2] > viewsize[2] ||
                          imagesize[3] > viewsize[3];
    if( carto::debugMessageLevel > 3 ) {
      std::cout << "VOLUMEFORMATREADER:: With Borders : "
                << ( withborders ? "yes" : "no" ) << std::endl;
      std::cout << "VOLUMEFORMATREADER:: Partial Reading : "
                << ( partialreading ? "yes" : "no" ) << std::endl;
    }

    //--- reading volume -------------------------------------------------------
    if( carto::debugMessageLevel > 3 )
      std::cout << "VOLUMEFORMATREADER:: reading volume..." 
                << std::endl;
    int y, z, t;
    if ( !withborders ) {
      // we can read the volume/region into a contiguous buffer
      _imr->read( ( T * ) &obj(0,0,0,0), *dsi, pos, 
                  viewsize, strides, options );
    } else {
      // we are in a "border" context. The volume/region must be read
      // line by line
      std::vector<int> posline ( pos );
      std::vector<int> sizeline ( 4, 1 );
      sizeline[ 0 ] = viewsize[ 0 ];
      for ( t=0; t<viewsize[ 3 ]; ++t )
        for ( z=0; z<viewsize[ 2 ]; ++z )
          for ( y=0; y<viewsize[ 1 ]; ++y ) {
            posline[ 1 ] = pos[ 1 ] + y;
            posline[ 2 ] = pos[ 2 ] + z;
            posline[ 3 ] = pos[ 3 ] + t;
            _imr->read( ( T * ) &obj(0,y,z,t), *dsi, posline, 
                        sizeline, strides, options );
          }
    }
    
    // we reset at 0 the ImageReader's members (sizes, binary, ...) so that 
    // they are recomputed at the next reading.
    _imr->resetParams();
  }
  
  //--- Attaching a ImageReader ------------------------------------------------
  template <typename T>
  void VolumeFormatReader<T>::attach( carto::rc_ptr<ImageReader<T> > imr )
  {
    _imr = imr;
  }
  
////////////////////////////////////////////////////////////////////////////////
////             V O L U M E R E F F O R M A T R E A D E R                  ////
////////////////////////////////////////////////////////////////////////////////

  //============================================================================
  //   C O N S T R U C T O R S
  //============================================================================
  template <typename T>
  VolumeRefFormatReader<T>::~VolumeRefFormatReader()
  {
  }
  
  //============================================================================
  //   N E W   M E T H O D S
  //============================================================================
  
  //--- Reading to a Volume<T> -------------------------------------------------
  /* This method depends deeply on the data structure (Volume<T>). It is 
   * declared in FormatReader but only defined here.
   */
  template <typename T>
  void VolumeRefFormatReader<T>::read( carto::VolumeRef<T> & obj, 
                                       carto::rc_ptr<DataSourceInfo> dsi, 
                                       const AllocatorContext & context, 
                                       carto::Object options )
  {
    VolumeFormatReader<T> vrf;
    vrf.attach( _imr );
    vrf.read( *obj, dsi, context, options );
  }
  
  //--- Attaching a ImageReader ------------------------------------------------
  template <typename T>
  void VolumeRefFormatReader<T>::attach( carto::rc_ptr<ImageReader<T> > imr )
  {
    _imr = imr;
  }

}

#endif