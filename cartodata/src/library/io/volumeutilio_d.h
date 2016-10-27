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
#ifndef CARTODATA_IO_VOLUMEUTILIO_D_H
#define CARTODATA_IO_VOLUMEUTILIO_D_H

//--- cartodata --------------------------------------------------------------
#include <cartodata/io/volumeutilio.h>                     // class definition
#include <cartodata/volume/volume.h>
//--- soma-io ----------------------------------------------------------------
#include <soma-io/config/soma_config.h>                      // (#define soma)
#include <soma-io/io/reader.h>
#include <soma-io/datasourceinfo/datasourceinfoloader.h>
#include <soma-io/datasourceinfo/datasourceinfo.h>
//--- cartobase --------------------------------------------------------------
#include <cartobase/object/object.h>                        // header, options
#include <cartobase/object/property.h>                      // header, options
//--- system -----------------------------------------------------------------
#include <vector>                                                // 4D vectors
#include <set>                                              // properties list
//--- debug ------------------------------------------------------------------
#include <cartobase/config/verbose.h>
#define localMsg( message ) cartoCondMsg( 3, message, "VOLUMEUTILIO" )
// localMsg must be undef at end of file
//----------------------------------------------------------------------------

namespace soma {

  template <typename T> std::set<std::string> 
  VolumeUtilIO<T>::listReadProperties()
  {
    std::set<std::string> properties;
    properties.insert( "ox" );
    properties.insert( "oy" );
    properties.insert( "oz" );
    properties.insert( "ot" );
    properties.insert( "sx" );
    properties.insert( "sy" );
    properties.insert( "sz" );
    properties.insert( "st" );
    properties.insert( "bx" );
    properties.insert( "by" );
    properties.insert( "bz" );
    properties.insert( "border" );
    properties.insert( "ox1" );
    properties.insert( "ox1" );
    properties.insert( "ox2" );
    properties.insert( "ox3" );
    properties.insert( "ox4" );
    properties.insert( "ox5" );
    properties.insert( "ox6" );
    properties.insert( "ox7" );
    properties.insert( "ox8" );
    properties.insert( "sx1" );
    properties.insert( "sx2" );
    properties.insert( "sx3" );
    properties.insert( "sx4" );
    properties.insert( "sx5" );
    properties.insert( "sx6" );
    properties.insert( "sx7" );
    properties.insert( "sx8" );
    return properties;
  }

  //==========================================================================
  //   R E A D
  //==========================================================================

  template <typename T> carto::Volume<T>*
  VolumeUtilIO<T>::read( carto::Volume<T> *             obj,
                         carto::rc_ptr<DataSourceInfo>  dsi,
                         carto::Object                  options )
  {
    std::vector<int> position( 4, 0 );
    std::vector<int> frame( 4, 0 );
    std::vector<int> borders( 4, 0 );
    try {
      position[0] = (int) rint(options->getProperty( "ox" )->getScalar() );
      options->removeProperty( "ox" );
    } catch( ... ) {}
    try {
      position[1] = (int) rint( options->getProperty( "oy" )->getScalar() );
      options->removeProperty( "oy" );
    } catch( ... ) {}
    try {
      position[2] = (int) rint( options->getProperty( "oz" )->getScalar() );
      options->removeProperty( "oz" );
    } catch( ... ) {}
    try {
      position[3] = (int) rint( options->getProperty( "ot" )->getScalar() );
      options->removeProperty( "ot" );
    } catch( ... ) {}
    try {
      frame[0] = (int) rint( options->getProperty( "sx" )->getScalar() );
      options->removeProperty( "sx" );
    } catch( ... ) {}
    try {
      frame[1] = (int) rint( options->getProperty( "sy" )->getScalar() );
      options->removeProperty( "sy" );
    } catch( ... ) {}
    try {
      frame[2] = (int) rint( options->getProperty( "sz" )->getScalar() );
      options->removeProperty( "sz" );
    } catch( ... ) {}
    try {
      frame[3] = (int) rint( options->getProperty( "st" )->getScalar() );
      options->removeProperty( "st" );
    } catch( ... ) {}
    try {
      borders[0] = (int) rint( options->getProperty( "border" )->getScalar() );
      borders[1] = (int) rint( options->getProperty( "border" )->getScalar() );
      borders[2] = (int) rint( options->getProperty( "border" )->getScalar() );
      options->removeProperty( "border" );
    } catch( ... ) {}
    try {
      borders[0] = (int) rint( options->getProperty( "bx" )->getScalar() );
      options->removeProperty( "bx" );
    } catch( ... ) {}
    try {
      borders[1] = (int) rint( options->getProperty( "by" )->getScalar() );
      options->removeProperty( "by" );
    } catch( ... ) {}
    try {
      borders[2] = (int) rint( options->getProperty( "bz" )->getScalar() );
      options->removeProperty( "bz" );
    } catch( ... ) {}

    int dim, value;
    for( dim=0; dim<carto::Volume<T>::DIM_MAX; ++dim )
    {
      try
      {
        std::stringstream skey;
        skey << "ox" << dim+1;
        std::string key = skey.str();
        value = (int) rint( options->getProperty( key )->getScalar() );
        options->removeProperty( key );
        if( position.size() <= dim )
          position.resize( dim + 1, 0 );
        position[ dim ] = value;
      }
      catch( ... ) {}
      try
      {
        std::stringstream skey;
        skey << "sx" << dim+1;
        std::string key = skey.str();
        value = (int) rint( options->getProperty( key )->getScalar() );
        options->removeProperty( key );
        if( frame.size() <= dim )
          frame.resize( dim + 1, 0 );
        frame[ dim ] = value;
      }
      catch( ... ) {}
    }

    bool partial = false;
    for( dim=0; !partial && dim<position.size(); ++dim )
      if( position[dim] != 0 )
        partial = true;
    for( dim=0; !partial && dim<frame.size(); ++dim )
      if( frame[dim] != 0 )
        partial = true;

    if( partial )
      return readPartial( obj, dsi, position, frame, borders, options );
    else
      return readFull( obj, dsi, borders, options );
  }

  template <typename T> carto::Volume<T>*
  VolumeUtilIO<T>::readFull( carto::Volume<T> *             obj,
                             carto::rc_ptr<DataSourceInfo>  dsi,
                             std::vector<int>               borders, 
                             carto::Object                  options )
  {
    //=== VARIABLES ==========================================================
    int verbose = carto::debugMessageLevel;
    carto::Object newoptions;
    soma::Reader<carto::Volume<T> > rVol;
    soma::DataSourceInfoLoader dsil;
    carto::VolumeRef<T> bordersVolume;
    std::vector<int> fullsize;
    std::vector<int> bordersize;
    std::vector<int> volumepos;

    if( borders[0] !=0 || borders[1] !=0 || borders[2] !=0 )
    {
      //=== CHECK FULL VOLUME ================================================
      localMsg( "=== READING FULL VOLUME SIZE" );
      newoptions = carto::Object::value( carto::PropertySet() );
      newoptions->copyProperties( options );
      localMsg( "checking full volume..." );
      *dsi = dsil.check( *dsi, options );
      localMsg( "reading size..." );
      if( !dsi->header()->getProperty( "volume_dimension", fullsize ) )
      {
        fullsize[0] = (int) rint(
          dsi->header()->getProperty( "sizeX" )->getScalar() );
        fullsize[1] = (int) rint(
          dsi->header()->getProperty( "sizeY" )->getScalar() );
        fullsize[2] = (int) rint(
            dsi->header()->getProperty( "sizeZ" )->getScalar() );
        fullsize[3] = (int) rint(
            dsi->header()->getProperty( "sizeT" )->getScalar() );
      }
      if( fullsize.size() < 4 )
        fullsize.resize( 4, 1 );
      volumepos.resize( fullsize.size(), 0 );
      volumepos[0] = borders[0];
      volumepos[1] = borders[1];
      volumepos[2] = borders[2];
      localMsg( "-> size =  ( "
                + carto::toString( fullsize[0] ) + ", "
                + carto::toString( fullsize[1] ) + ", "
                + carto::toString( fullsize[2] ) + ", "
                + carto::toString( fullsize[3] ) + " )"
                 );

      //=== BORDERS VOLUME ===================================================
      localMsg( "=== ALLOCATED BORDERS VOLUME" );
      localMsg( "computing sizes..." );
      bordersize = fullsize;
      bordersize[0] = fullsize[0] + 2*borders[0];
      bordersize[1] = fullsize[1] + 2*borders[1];
      bordersize[2] = fullsize[2] + 2*borders[2];
      localMsg( "creating allocated volume..." );
      localMsg( "-> with size ( "
                + carto::toString( bordersize[0] ) + ", "
                + carto::toString( bordersize[1] ) + ", "
                + carto::toString( bordersize[2] ) + ", "
                + carto::toString( bordersize[3] ) + " )"
                 );
      bordersVolume = carto::VolumeRef<T>(
        new carto::Volume<T>( bordersize ) );

      //=== READ FULL VOLUME =================================================
      localMsg( "=== UNALLOCATED FULL VIEW" );
      rVol = soma::Reader< carto::Volume<T> >( dsi );
      newoptions = carto::Object::value( carto::PropertySet() );
      newoptions->copyProperties( options );
      newoptions->setProperty( "keep_allocation", true );
      rVol.setOptions( newoptions );
      localMsg( "creating volume view..." );
      localMsg( "-> with pos ( "
                + carto::toString( volumepos[0] ) + ", "
                + carto::toString( volumepos[1] ) + ", "
                + carto::toString( volumepos[2] ) + ", "
                + carto::toString( volumepos[3] ) + " )"
                 );
      localMsg( "-> with size ( "
                + carto::toString( fullsize[0] ) + ", "
                + carto::toString( fullsize[1] ) + ", "
                + carto::toString( fullsize[2] ) + ", "
                + carto::toString( fullsize[3] ) + " )"
                 );
      if( obj )
        *obj = carto::Volume<T>( bordersVolume, volumepos, fullsize );
      else
        obj = new carto::Volume<T>( bordersVolume, volumepos, fullsize );
      localMsg( "reading unallocated volume..." );
      rVol.setAllocatorContext( obj->allocatorContext() );
      rVol.read( *obj );

      /* copy voxel_size to underlying volume, if any.
        This should probably be more general, but cannot be applied to all
        header properties (size, transformations...).
        WARNING: Moreover here we do not guarantee to keep both voxel_size
        unique: we point to the same vector of values for now, but it can be
        replaced (thus, duplicated) by a setProperty().
        We could use a addBuiltinProperty(), but then the voxel size has to be
        stored in a fixed location somewhere.
      */
      try
      {
        carto::Object vs = obj->header().getProperty( "voxel_size" );
        bordersVolume->header().setProperty( "voxel_size", vs );
      }
      catch( ... )
      { // never mind.
      }

    }
    else
    {
      //=== FULL VOLUME ======================================================
      localMsg( "=== ALLOCATED FULL VIEW" );
      rVol = soma::Reader< carto::Volume<T> >( dsi );
      newoptions = carto::Object::value( carto::PropertySet() );
      newoptions->copyProperties( options );
      rVol.setOptions( newoptions );
      localMsg( "reading volume..." );
      obj = rVol.read();
    }

    return obj;
  }

  template <typename T> carto::Volume<T>*
  VolumeUtilIO<T>::readPartial( carto::Volume<T> *             obj,
                                carto::rc_ptr<DataSourceInfo>  dsi,
                                std::vector<int>               position, 
                                std::vector<int>               frame,
                                std::vector<int>               borders, 
                                carto::Object                  options )
  {
    //=== VARIABLES ==========================================================
    std::vector<int> viewframe( frame );
    size_t dim, ndim = std::max( position.size(), frame.size() );
    if( viewframe.size() < ndim )
      viewframe.resize( ndim, 0 );
    std::vector<int> viewpos( position );
    if( viewpos.size() < ndim )
      viewpos.resize( ndim, 0 );
    std::vector<int> fullsize;     // full size
    std::vector<int> borderframe( ndim, 1 );  // allocated volume size
    std::vector<int> borderpos;    // allocated volume size
    std::vector<int> readframe;    // read frame size
    std::vector<int> readpos( ndim, 0 );      // read frame origin
    carto::Object newoptions;
    soma::Reader<carto::Volume<T> > rVol;
    soma::Reader<carto::Volume<T> > rView;
    carto::VolumeRef<T> fullVolume, bordersVolume, readVolume;
//     carto::Volume<T>* viewVolume;

    //=== UNALLOCATED FULL VOLUME ============================================
    localMsg( "=== UNALLOCATED FULL VOLUME" );
    rVol = soma::Reader<carto::Volume<T> >( dsi );
    newoptions = carto::Object::value( carto::PropertySet() );
    newoptions->setProperty( "unallocated", true );
    newoptions->copyProperties( options );
    rVol.setOptions( newoptions );
    localMsg( "reading unallocated volume..." );
    fullVolume = carto::VolumeRef<T>( rVol.read() );
    localMsg( "reading size from volume..." );
    fullsize = fullVolume->getSize();
//     localMsg( "-> full size ( "
//               + carto::toString( fullsize[0] ) + ", "
//               + carto::toString( fullsize[1] ) + ", "
//               + carto::toString( fullsize[2] ) + ", "
//               + carto::toString( fullsize[3] ) + " )" );
//     localMsg( "-> position ( "
//               + carto::toString( position[0] ) + ", "
//               + carto::toString( position[1] ) + ", "
//               + carto::toString( position[2] ) + ", "
//               + carto::toString( position[3] ) + " )" );
//     localMsg( "-> borders ( "
//               + carto::toString( borders[0] ) + ", "
//               + carto::toString( borders[1] ) + ", "
//               + carto::toString( borders[2] ) + ", "
//               + carto::toString( borders[3] ) + " )" );
    for( dim=0; dim<ndim; ++dim )
      if( viewframe[ dim ] == 0 )
        viewframe[ dim ] = (fullsize[ dim ] - position[ dim ]);
    localMsg( "===" );

    if( borders[0] != 0 || borders[1] != 0 || borders[2] != 0 ) 
    {
      //=== ALLOCATED BORDERS VOLUME =========================================
      localMsg( "=== ALLOCATED BORDERS VOLUME" );
      localMsg( "computing borders..." );
      // setting "bordersVolume" position / "fullVolume"
      borderpos = viewpos;
      borderpos[0] -= borders[0];
      borderpos[1] -= borders[1];
      borderpos[2] -= borders[2];
      borderframe = viewframe;
      borderframe[0] += 2*borders[0];
      borderframe[1] += 2*borders[1];
      borderframe[2] += 2*borders[2];
      localMsg( "creating volume..." );
      localMsg( "-> with frame size ( "
                + carto::toString( borderframe[0] ) + ", "
                + carto::toString( borderframe[1] ) + ", "
                + carto::toString( borderframe[2] ) + ", "
                + carto::toString( borderframe[3] ) + " )"
                 );
      localMsg( "-> with frame pos ( "
                + carto::toString( borderpos[0] ) + ", "
                + carto::toString( borderpos[1] ) + ", "
                + carto::toString( borderpos[2] ) + ", "
                + carto::toString( borderpos[3] ) + " )"
                 );
      bordersVolume = carto::VolumeRef<T>(
        new carto::Volume<T>( fullVolume, borderpos, borderframe ) );
      
//       localMsg( "-> effective frame size ( "
//                 + carto::toString( bordersVolume->getSizeX() ) + ", "
//                 + carto::toString( bordersVolume->getSizeY() ) + ", "
//                 + carto::toString( bordersVolume->getSizeZ() ) + ", "
//                 + carto::toString( bordersVolume->getSizeT() ) + " )"
//                  );

      //=== UNALLOCATED READ VIEW ============================================
      localMsg( "=== UNALLOCATED READ VIEW" );
      localMsg( "computing read frame..." );
      // setting "readVolume" position / "bordersVolume"
      readpos[0] = ( borderpos[0] < 0 ? borders[0] : 0 );
      readpos[1] = ( borderpos[1] < 0 ? borders[1] : 0 );
      readpos[2] = ( borderpos[2] < 0 ? borders[2] : 0 );
      readpos[3] = 0;
      std::vector<int> borderdep( 3, 0 );
      borderdep[0] = ( borderpos[0] + borderframe[0] - fullsize[0] > 0 
                       ? borderpos[0] + borderframe[0] - fullsize[0] : 0 );
      borderdep[1] = ( borderpos[1] + borderframe[1] - fullsize[1] > 0 
                       ? borderpos[1] + borderframe[1] - fullsize[1] : 0 );
      borderdep[2] = ( borderpos[2] + borderframe[2] - fullsize[2] > 0 
                       ? borderpos[2] + borderframe[2] - fullsize[2] : 0 );
      readframe[0] = borderframe[0] - readpos[0] - borderdep[0];
      readframe[1] = borderframe[1] - readpos[1] - borderdep[1];
      readframe[2] = borderframe[2] - readpos[2] - borderdep[2];
      readframe[3] = borderframe[3];
      rView = soma::Reader<carto::Volume<T> >( rVol.dataSourceInfo() );
      newoptions = carto::Object::value( carto::PropertySet() );
      newoptions->setProperty( "partial_reading", true );
      newoptions->copyProperties( options );
      rView.setOptions( newoptions );
      localMsg( "creating volume..." );
      localMsg( "-> with frame size ( "
                + carto::toString( readframe[0] ) + ", "
                + carto::toString( readframe[1] ) + ", "
                + carto::toString( readframe[2] ) + ", "
                + carto::toString( readframe[3] ) + " )"
                 );
      localMsg( "-> with frame pos ( "
                + carto::toString( readpos[0] ) + ", "
                + carto::toString( readpos[1] ) + ", "
                + carto::toString( readpos[2] ) + ", "
                + carto::toString( readpos[3] ) + " )"
                 );
      readVolume = carto::VolumeRef<T>(
        new carto::Volume<T>( bordersVolume, readpos, readframe ) );
      
//       localMsg( "-> effective read view size ( "
//                 + carto::toString( readVolume->getSizeX() ) + ", "
//                 + carto::toString( readVolume->getSizeY() ) + ", "
//                 + carto::toString( readVolume->getSizeZ() ) + ", "
//                 + carto::toString( readVolume->getSizeT() ) + " )"
//                  );
      
      localMsg( "reading frame..." );
      rView.read( *readVolume );

//       localMsg( "-> effective read view size after read ( "
//                 + carto::toString( readVolume->getSizeX() ) + ", "
//                 + carto::toString( readVolume->getSizeY() ) + ", "
//                 + carto::toString( readVolume->getSizeZ() ) + ", "
//                 + carto::toString( readVolume->getSizeT() ) + " )"
//                  );

      //=== UNALLOCATED PROCCESSED VOLUME ====================================
      localMsg( "=== UNALLOCATED PROCESSED VOLUME" );
      localMsg( "computing view frame..." );
      // setting "viewVolume" position / "bordersVolume"
      viewpos[0] = borders[0];
      viewpos[1] = borders[1];
      viewpos[2] = borders[2];
      viewpos[3] = 0;
      localMsg( "creating volume..." );
      localMsg( "-> with frame size ( "
                + carto::toString( viewframe[0] ) + ", "
                + carto::toString( viewframe[1] ) + ", "
                + carto::toString( viewframe[2] ) + ", "
                + carto::toString( viewframe[3] ) + " )"
                 );
      localMsg( "-> with frame pos ( "
                + carto::toString( viewpos[0] ) + ", "
                + carto::toString( viewpos[1] ) + ", "
                + carto::toString( viewpos[2] ) + ", "
                + carto::toString( viewpos[3] ) + " )"
                 );
      if(obj)
        *obj = carto::Volume<T>( bordersVolume, viewpos, viewframe );
      else
        obj = new carto::Volume<T>( bordersVolume, viewpos, viewframe );

      localMsg( "copying header..." );
      obj->blockSignals( true );
      obj->header().copyProperties( carto::Object::value( readVolume->header() ) );
      /* copy voxel_size to underlying volume, if any.
        This should probably be more general, but cannot be applied to all
        header properties (size, transformations...).
        WARNING: Moreover here we do not guarantee to keep both voxel_size
        unique: we point to the same vector of values for now, but it can be
        replaced (thus, duplicated) by a setProperty().
        We could use a addBuiltinProperty(), but then the voxel size has to be
        stored in a fixed location somewhere.
      */
      try
      {
        carto::Object vs = obj->header().getProperty( "voxel_size" );
        bordersVolume->header().setProperty( "voxel_size", vs );
      }
      catch( ... )
      { // never mind.
      }
      carto::PropertySet & ps = obj->header();
      ps.setProperty( "sizeX", viewframe[0] );
      ps.setProperty( "sizeY", viewframe[1] );
      ps.setProperty( "sizeZ", viewframe[2] );
      ps.setProperty( "sizeT", viewframe[3] );
      ps.setProperty( "volume_dimension", viewframe );
      obj->blockSignals( false );
    }
    else 
    {
      //=== ALLOCATED PROCESSED VOLUME =======================================
      localMsg( "=== ALLOCATED PROCESSED VOLUME" );
      rView = soma::Reader<carto::Volume<T> >( rVol.dataSourceInfo() );
      newoptions = carto::Object::value( carto::PropertySet() );
      newoptions->setProperty( "partial_reading", true );
      newoptions->copyProperties( options );
      rView.setOptions( newoptions );
      localMsg( "creating volume..." );
      if( obj )
        *obj = carto::Volume<T>( fullVolume, viewpos, viewframe );
      else
        obj = new carto::Volume<T>( fullVolume, viewpos, viewframe );

      localMsg( "reading partial volume..." );
      rView.read( *obj );
    }
    
    return obj;
  }

}

#undef localMsg
#endif
