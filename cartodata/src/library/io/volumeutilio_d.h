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

//--- cartodata ----------------------------------------------------------------
#include <cartodata/io/volumeutilio.h>                       // class definition
#include <cartodata/volume/volume.h>
#include <cartodata/volume/volumeview.h>
//--- soma-io ------------------------------------------------------------------
#include <soma-io/config/soma_config.h>                        // (#define soma)
#include <soma-io/io/reader.h>
#include <soma-io/datasourceinfo/datasourceinfoloader.h>
#include <soma-io/datasourceinfo/datasourceinfo.h>
//--- cartobase ----------------------------------------------------------------
#include <cartobase/config/verbose.h>        // verbosity level and cartoCondMsg
#include <cartobase/type/string_conversion.h>
#include <cartobase/object/object.h>                          // header, options
#include <cartobase/object/property.h>                        // header, options
//--- system -------------------------------------------------------------------
#include <iostream>                                                     // debug
#include <vector>                                                  // 4D vectors
#include <set>                                                // properties list
//------------------------------------------------------------------------------
#define VIO_V 3                                               // verbosity level

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
    return properties;
  }

  //============================================================================
  //   R E A D
  //============================================================================

  template <typename T> carto::Volume<T>*
  VolumeUtilIO<T>::read( //carto::Volume<T> *             obj,
                         carto::rc_ptr<DataSourceInfo>  dsi,
                         carto::Object                  options )
  {
    std::vector<int> position( 4, 0 );
    std::vector<int> frame( 4, 0 );
    std::vector<int> borders( 4, 0 );
    try {
      position[0] = options->getProperty( "ox" )->getScalar();
      options->removeProperty( "ox" );
    } catch( ... ) {}
    try {
      position[1] = options->getProperty( "oy" )->getScalar();
      options->removeProperty( "oy" );
    } catch( ... ) {}
    try {
      position[2] = options->getProperty( "oz" )->getScalar();
      options->removeProperty( "oz" );
    } catch( ... ) {}
    try {
      position[3] = options->getProperty( "ot" )->getScalar();
      options->removeProperty( "ot" );
    } catch( ... ) {}
    try {
      frame[0] = options->getProperty( "sx" )->getScalar();
      options->removeProperty( "sx" );
    } catch( ... ) {}
    try {
      frame[1] = options->getProperty( "sy" )->getScalar();
      options->removeProperty( "sy" );
    } catch( ... ) {}
    try {
      frame[2] = options->getProperty( "sz" )->getScalar();
      options->removeProperty( "sz" );
    } catch( ... ) {}
    try {
      frame[3] = options->getProperty( "st" )->getScalar();
      options->removeProperty( "st" );
    } catch( ... ) {}
    try {
      borders[0] = options->getProperty( "border" )->getScalar();
      borders[1] = options->getProperty( "border" )->getScalar();
      borders[2] = options->getProperty( "border" )->getScalar();
      options->removeProperty( "border" );
    } catch( ... ) {}
    try {
      borders[0] = options->getProperty( "bx" )->getScalar();
      options->removeProperty( "bx" );
    } catch( ... ) {}
    try {
      borders[1] = options->getProperty( "by" )->getScalar();
      options->removeProperty( "by" );
    } catch( ... ) {}
    try {
      borders[2] = options->getProperty( "bz" )->getScalar();
      options->removeProperty( "bz" );
    } catch( ... ) {}

    bool partial = !( frame[0] == 0 && frame[1] == 0 &&
                      frame[2] == 0 && frame[3] == 0 );
    if( partial )
      return readPartial( dsi, position, frame, borders, options );
    else
      return readFull( dsi, borders, options );
  }

  template <typename T> carto::Volume<T>*
  VolumeUtilIO<T>::readFull( //carto::Volume<T> *             obj,
                             carto::rc_ptr<DataSourceInfo>  dsi,
                             std::vector<int>               borders, 
                             carto::Object                  options )
  {
    //=== VARIABLES ============================================================
    int verbose = carto::debugMessageLevel;
    carto::Object newoptions;
    soma::Reader<carto::Volume<T> > rVol;
    soma::DataSourceInfoLoader dsil;
    carto::VolumeRef<T> bordersVolume;
    carto::Volume<T>* fullVolume;
    typename carto::VolumeView<T>::Position4Di fullsize;
    typename carto::VolumeView<T>::Position4Di bordersize;
    typename carto::VolumeView<T>::Position4Di volumepos( borders[0], borders[1], borders[2], 0 );

    if( borders[0] !=0 || borders[1] !=0 || borders[2] !=0 )
    {
      //=== CHECK FULL VOLUME ==================================================
      cartoCondMsg( VIO_V, "=== READING FULL VOLUME SIZE", "VOLUMEUTILIO" );
      newoptions = carto::Object::value( carto::PropertySet() );
      newoptions->copyProperties( options );
      cartoCondMsg( VIO_V, "checking full volume...", "VOLUMEUTILIO" );
      *dsi = dsil.check( *dsi, options );
      cartoCondMsg( VIO_V, "reading size...", "VOLUMEUTILIO" );
      fullsize[0] = dsi->header()->getProperty( "sizeX" )->getScalar();
      fullsize[1] = dsi->header()->getProperty( "sizeY" )->getScalar();
      fullsize[2] = dsi->header()->getProperty( "sizeZ" )->getScalar();
      fullsize[3] = dsi->header()->getProperty( "sizeT" )->getScalar();
      cartoCondMsg( VIO_V, "-> size =  ( "
                + carto::toString( fullsize[0] ) + ", "
                + carto::toString( fullsize[1] ) + ", "
                + carto::toString( fullsize[2] ) + ", "
                + carto::toString( fullsize[3] ) + " )"
                , "VOLUMEUTILIO" );
      cartoCondMsg( VIO_V, "===", "VOLUMEUTILIO" );

      //=== BORDERS VOLUME =====================================================
      cartoCondMsg( VIO_V, "=== ALLOCATED BORDERS VOLUME", "VOLUMEUTILIO" );
      cartoCondMsg( VIO_V, "computing sizes...", "VOLUMEUTILIO" );
      bordersize[0] = fullsize[0] + 2*borders[0];
      bordersize[1] = fullsize[1] + 2*borders[1];
      bordersize[2] = fullsize[2] + 2*borders[2];
      bordersize[3] = fullsize[3];
      cartoCondMsg( VIO_V, "creating allocated volume...", "VOLUMEUTILIO" );
      cartoCondMsg( VIO_V, "-> with size ( "
                + carto::toString( bordersize[0] ) + ", "
                + carto::toString( bordersize[1] ) + ", "
                + carto::toString( bordersize[2] ) + ", "
                + carto::toString( bordersize[3] ) + " )"
                , "VOLUMEUTILIO" );
      bordersVolume = carto::VolumeRef<T>( new carto::Volume<T>( bordersize[0], bordersize[1], bordersize[2], bordersize[3] ) );
      cartoCondMsg( VIO_V, "===", "VOLUMEUTILIO" );

      //--- HEADER -------------------------------------------------------------
      cartoCondMsg( VIO_V+1, "=== HEADER", "VOLUMEUTILIO" );
      cartoCondMsg( VIO_V+1, "Volume size : ( "
                + bordersVolume->header().getProperty( "sizeX" )->getString() + ", " 
                + bordersVolume->header().getProperty( "sizeY" )->getString() + ", "
                + bordersVolume->header().getProperty( "sizeZ" )->getString() + ", "
                + bordersVolume->header().getProperty( "sizeT" )->getString() + " )"
                , "VOLUMEUTILIO" );
      cartoCondMsg( VIO_V+1, "===", "VOLUMEUTILIO" );
      //------------------------------------------------------------------------

      //=== READ FULL VOLUME ===================================================
      cartoCondMsg( VIO_V, "=== UNALLOCATED FULL VIEW", "VOLUMEUTILIO" );
      rVol = soma::Reader< carto::Volume<T> >( dsi );
      newoptions = carto::Object::value( carto::PropertySet() );
      newoptions->copyProperties( options );
      rVol.setOptions( newoptions );
      cartoCondMsg( VIO_V, "creating volume view...", "VOLUMEUTILIO" );
      cartoCondMsg( VIO_V, "-> with pos ( "
                + carto::toString( volumepos[0] ) + ", "
                + carto::toString( volumepos[1] ) + ", "
                + carto::toString( volumepos[2] ) + ", "
                + carto::toString( volumepos[3] ) + " )"
                , "VOLUMEUTILIO" );
      cartoCondMsg( VIO_V, "-> with size ( "
                + carto::toString( fullsize[0] ) + ", "
                + carto::toString( fullsize[1] ) + ", "
                + carto::toString( fullsize[2] ) + ", "
                + carto::toString( fullsize[3] ) + " )"
                , "VOLUMEUTILIO" );
      fullVolume = new carto::VolumeView<T>( bordersVolume, volumepos, fullsize );
      cartoCondMsg( VIO_V, "reading unallocated volume...", "VOLUMEUTILIO" );
      rVol.read( *fullVolume );
      cartoCondMsg( VIO_V, "===", "VOLUMEUTILIO" );

      //--- HEADER -------------------------------------------------------------
      cartoCondMsg( VIO_V+1, "=== HEADER", "VOLUMEUTILIO" );
      cartoCondMsg( VIO_V+1, "Volume's :", "VOLUMEUTILIO" );
      printheader( VIO_V+1, carto::Object::value( fullVolume->header() ) );
      cartoCondMsg( VIO_V+1, "Volume size : ( "
                + fullVolume->header().getProperty( "sizeX" )->getString() + ", " 
                + fullVolume->header().getProperty( "sizeY" )->getString() + ", "
                + fullVolume->header().getProperty( "sizeZ" )->getString() + ", "
                + fullVolume->header().getProperty( "sizeT" )->getString() + " )"
                , "VOLUMEUTILIO" );
      cartoCondMsg( VIO_V+1, "Reader's :", "VOLUMEUTILIO" );
      printheader( VIO_V+1, rVol.dataSourceInfo()->header() );
      cartoCondMsg( VIO_V+1, "===", "VOLUMEUTILIO" );
      //------------------------------------------------------------------------
    }
    else
    {
      //=== FULL VOLUME ========================================================
      cartoCondMsg( VIO_V, "=== ALLOCATED FULL VIEW", "VOLUMEUTILIO" );
      rVol = soma::Reader< carto::Volume<T> >( dsi );
      newoptions = carto::Object::value( carto::PropertySet() );
      newoptions->copyProperties( options );
      rVol.setOptions( newoptions );
      cartoCondMsg( VIO_V, "reading volume...", "VOLUMEUTILIO" );
      fullVolume = rVol.read();
      cartoCondMsg( VIO_V, "===", "VOLUMEUTILIO" );

      //--- HEADER -------------------------------------------------------------
      cartoCondMsg( VIO_V+1, "=== HEADER", "VOLUMEUTILIO" );
      cartoCondMsg( VIO_V+1, "Volume's :", "VOLUMEUTILIO" );
      printheader( VIO_V+1, carto::Object::value( fullVolume->header() ) );
      cartoCondMsg( VIO_V+1, "Volume size : ( "
                + fullVolume->header().getProperty( "sizeX" )->getString() + ", " 
                + fullVolume->header().getProperty( "sizeY" )->getString() + ", "
                + fullVolume->header().getProperty( "sizeZ" )->getString() + ", "
                + fullVolume->header().getProperty( "sizeT" )->getString() + " )"
                , "VOLUMEUTILIO" );
      cartoCondMsg( VIO_V+1, "Reader's :", "VOLUMEUTILIO" );
      printheader( VIO_V+1, rVol.dataSourceInfo()->header() );
      cartoCondMsg( VIO_V+1, "===", "VOLUMEUTILIO" );
      //------------------------------------------------------------------------
    }

    return fullVolume;
  }

  template <typename T> carto::Volume<T>* 
  VolumeUtilIO<T>::readPartial( //carto::Volume<T> *             obj,
                                carto::rc_ptr<DataSourceInfo>  dsi,
                                std::vector<int>               position, 
                                std::vector<int>               frame,
                                std::vector<int>               borders, 
                                carto::Object                  options )
  {
    //=== VARIABLES ============================================================
    int verbose = carto::debugMessageLevel;

    typename carto::VolumeView<T>::Position4Di
      viewframe( frame[0], frame[1], frame[2], frame[3] );
    typename carto::VolumeView<T>::Position4Di
      viewpos( position[0], position[1], position[2], position[3] );
    typename carto::VolumeView<T>::Position4Di fullsize;     // full size
    typename carto::VolumeView<T>::Position4Di borderframe;  // allocated volume size
    typename carto::VolumeView<T>::Position4Di borderpos;    // allocated volume size
    typename carto::VolumeView<T>::Position4Di readframe;    // read frame size
    typename carto::VolumeView<T>::Position4Di readpos;      // read frame origin
    carto::Object newoptions;
    soma::Reader<carto::Volume<T> > rVol;
    soma::Reader<carto::Volume<T> > rView;
    carto::VolumeRef<T> fullVolume, bordersVolume, readVolume;
    carto::Volume<T>* viewVolume;

    //=== UNALLOCATED FULL VOLUME ==============================================
    cartoCondMsg( VIO_V, "=== UNALLOCATED FULL VOLUME", "VOLUMEUTILIO" );
    rVol = soma::Reader<carto::Volume<T> >( dsi );
    newoptions = carto::Object::value( carto::PropertySet() );
    newoptions->setProperty( "unallocated", true );
    newoptions->copyProperties( options );
    rVol.setOptions( newoptions );
    cartoCondMsg( VIO_V, "reading unallocated volume...", "VOLUMEUTILIO" );
    fullVolume = carto::VolumeRef<T>( rVol.read() );
    cartoCondMsg( VIO_V, "reading size from volume...", "VOLUMEUTILIO" );
    fullsize[ 0 ] = fullVolume->getSizeX();
    fullsize[ 1 ] = fullVolume->getSizeY();
    fullsize[ 2 ] = fullVolume->getSizeZ();
    fullsize[ 3 ] = fullVolume->getSizeT();
    if( viewframe[ 0 ] == 0 )
      viewframe[ 0 ] = fullsize[ 0 ];
    if( viewframe[ 1 ] == 0 )
      viewframe[ 1 ] = fullsize[ 1 ];
    if( viewframe[ 2 ] == 0 )
      viewframe[ 2 ] = fullsize[ 2 ];
    if( viewframe[ 3 ] == 0 )
      viewframe[ 3 ] = fullsize[ 3 ];
    cartoCondMsg( VIO_V, "===", "VOLUMEUTILIO" );

    //--- HEADER ---------------------------------------------------------------
    cartoCondMsg( VIO_V+1, "=== HEADER", "VOLUMEUTILIO" );
    cartoCondMsg( VIO_V+1, "Volume's :", "VOLUMEUTILIO" );
    printheader( VIO_V+1, carto::Object::value( fullVolume->header() ) );
    cartoCondMsg( VIO_V+1, "Volume size : ( "
              + fullVolume->header().getProperty( "sizeX" )->getString() + ", " 
              + fullVolume->header().getProperty( "sizeY" )->getString() + ", "
              + fullVolume->header().getProperty( "sizeZ" )->getString() + ", "
              + fullVolume->header().getProperty( "sizeT" )->getString() + " )"
              , "VOLUMEUTILIO" );
    cartoCondMsg( VIO_V+1, "Reader's :", "VOLUMEUTILIO" );
    printheader( VIO_V+1, rVol.dataSourceInfo()->header() );
    cartoCondMsg( VIO_V+1, "===", "VOLUMEUTILIO" );
    //--------------------------------------------------------------------------

    if( borders[0] != 0 || borders[1] != 0 || borders[2] != 0 ) 
    {
      //=== ALLOCATED BORDERS VOLUME ===========================================
      cartoCondMsg( VIO_V, "=== ALLOCATED BORDERS VOLUME", "VOLUMEUTILIO" );
      cartoCondMsg( VIO_V, "computing borders...", "VOLUMEUTILIO" );
      // setting "bordersVolume" position / "fullVolume"
      borderpos = viewpos;
      borderpos[0] -= borders[0];
      borderpos[1] -= borders[1];
      borderpos[2] -= borders[2];
      borderframe = viewframe;
      borderframe[0] += 2*borders[0];
      borderframe[1] += 2*borders[1];
      borderframe[2] += 2*borders[2];
      cartoCondMsg( VIO_V, "creating volume...", "VOLUMEUTILIO" );
      cartoCondMsg( VIO_V, "-> with frame size ( "
                + carto::toString( borderframe[0] ) + ", "
                + carto::toString( borderframe[1] ) + ", "
                + carto::toString( borderframe[2] ) + ", "
                + carto::toString( borderframe[3] ) + " )"
                , "VOLUMEUTILIO" );
      cartoCondMsg( VIO_V, "-> with frame pos ( "
                + carto::toString( borderpos[0] ) + ", "
                + carto::toString( borderpos[1] ) + ", "
                + carto::toString( borderpos[2] ) + ", "
                + carto::toString( borderpos[3] ) + " )"
                , "VOLUMEUTILIO" );
      bordersVolume = carto::VolumeRef<T>( new carto::VolumeView<T>( fullVolume, borderpos, borderframe ) );
      cartoCondMsg( VIO_V, "===", "VOLUMEUTILIO" );

      //--- HEADER -------------------------------------------------------------
      cartoCondMsg( VIO_V+1, "HEADER", "VOLUMEUTILIO" );
      cartoCondMsg( VIO_V+1, "Volume size : ( "
                + bordersVolume->header().getProperty( "sizeX" )->getString() + ", " 
                + bordersVolume->header().getProperty( "sizeY" )->getString() + ", "
                + bordersVolume->header().getProperty( "sizeZ" )->getString() + ", "
                + bordersVolume->header().getProperty( "sizeT" )->getString() + " )"
                , "VOLUMEUTILIO" );
      cartoCondMsg( VIO_V+1, "===", "VOLUMEUTILIO" );
      //------------------------------------------------------------------------

      //=== UNALLOCATED READ VIEW ==============================================
      cartoCondMsg( VIO_V, "=== UNALLOCATED READ VIEW", "VOLUMEUTILIO" );
      cartoCondMsg( VIO_V, "computing read frame...", "VOLUMEUTILIO" );
      // setting "readVolume" position / "bordersVolume"
      readpos[0] = ( borderpos[0] < 0 ? borders[0] : 0 );
      readpos[1] = ( borderpos[1] < 0 ? borders[1] : 0 );
      readpos[2] = ( borderpos[1] < 0 ? borders[2] : 0 );
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
      readframe[2] = borderframe[3] - readpos[2] - borderdep[2];
      readframe[3] = borderframe[3];
      rView = soma::Reader<carto::Volume<T> >( rVol.dataSourceInfo() );
      newoptions = carto::Object::value( carto::PropertySet() );
      newoptions->setProperty( "partial_reading", true );
      newoptions->copyProperties( options );
      rView.setOptions( newoptions );
      cartoCondMsg( VIO_V, "creating volume...", "VOLUMEUTILIO" );
      cartoCondMsg( VIO_V, "-> with frame size ( "
                + carto::toString( readframe[0] ) + ", "
                + carto::toString( readframe[1] ) + ", "
                + carto::toString( readframe[2] ) + ", "
                + carto::toString( readframe[3] ) + " )"
                , "VOLUMEUTILIO" );
      cartoCondMsg( VIO_V, "-> with frame pos ( "
                + carto::toString( readpos[0] ) + ", "
                + carto::toString( readpos[1] ) + ", "
                + carto::toString( readpos[2] ) + ", "
                + carto::toString( readpos[3] ) + " )"
                , "VOLUMEUTILIO" );
      readVolume = carto::VolumeRef<T>( new carto::VolumeView<T>( bordersVolume, readpos, readframe ) );
      cartoCondMsg( VIO_V, "reading frame...", "VOLUMEUTILIO" );
      rView.read( *readVolume );
      cartoCondMsg( VIO_V, "===", "VOLUMEUTILIO" );

      //--- HEADER -------------------------------------------------------------
      cartoCondMsg( VIO_V+1, "=== HEADER", "VOLUMEUTILIO" );
      cartoCondMsg( VIO_V+1, "Volume's :", "VOLUMEUTILIO" );
      printheader( VIO_V+1, carto::Object::value( readVolume->header() ) );
      cartoCondMsg( VIO_V+1, "Volume size : ( "
                + readVolume->header().getProperty( "sizeX" )->getString() + ", " 
                + readVolume->header().getProperty( "sizeY" )->getString() + ", "
                + readVolume->header().getProperty( "sizeZ" )->getString() + ", "
                + readVolume->header().getProperty( "sizeT" )->getString() + " )"
                , "VOLUMEUTILIO" );
      cartoCondMsg( VIO_V+1, "Reader's :", "VOLUMEUTILIO" );
      printheader( VIO_V+1, rView.dataSourceInfo()->header() );
      cartoCondMsg( VIO_V+1, "===", "VOLUMEUTILIO" );
      //------------------------------------------------------------------------

      //=== UNALLOCATED PROCCESSED VOLUME ======================================
      cartoCondMsg( VIO_V, "=== UNALLOCATED PROCCESSED VOLUME", "VOLUMEUTILIO" );
      cartoCondMsg( VIO_V, "computing view frame...", "VOLUMEUTILIO" );
      // setting "viewVolume" position / "bordersVolume"
      viewpos[0] = borders[0];
      viewpos[1] = borders[1];
      viewpos[2] = borders[2];
      viewpos[3] = 0;
      cartoCondMsg( VIO_V, "creating volume...", "VOLUMEUTILIO" );
      cartoCondMsg( VIO_V, "-> with frame size ( "
                + carto::toString( viewframe[0] ) + ", "
                + carto::toString( viewframe[1] ) + ", "
                + carto::toString( viewframe[2] ) + ", "
                + carto::toString( viewframe[3] ) + " )"
                , "VOLUMEUTILIO" );
      cartoCondMsg( VIO_V, "-> with frame pos ( "
                + carto::toString( viewpos[0] ) + ", "
                + carto::toString( viewpos[1] ) + ", "
                + carto::toString( viewpos[2] ) + ", "
                + carto::toString( viewpos[3] ) + " )"
                , "VOLUMEUTILIO" );
      viewVolume = new carto::VolumeView<T>( bordersVolume, viewpos, viewframe );
      cartoCondMsg( VIO_V, "copying header...", "VOLUMEUTILIO" );
      viewVolume->blockSignals( true );
      viewVolume->header().copyProperties( carto::Object::value( readVolume->header() ) );
      carto::PropertySet & ps = viewVolume->header();
      ps.setProperty( "sizeX", viewframe[0] );
      ps.setProperty( "sizeY", viewframe[1] );
      ps.setProperty( "sizeZ", viewframe[2] );
      ps.setProperty( "sizeT", viewframe[3] );
      viewVolume->blockSignals( false );
      cartoCondMsg( VIO_V, "===", "VOLUMEUTILIO" );

      //--- HEADER -------------------------------------------------------------
      cartoCondMsg( VIO_V+1, "=== HEADER", "VOLUMEUTILIO" );
      cartoCondMsg( VIO_V+1, "Volume's :", "VOLUMEUTILIO" );
      printheader( VIO_V+1, carto::Object::value( viewVolume->header() ) );
      cartoCondMsg( VIO_V+1, "Volume size : ( "
                + viewVolume->header().getProperty( "sizeX" )->getString() + ", " 
                + viewVolume->header().getProperty( "sizeY" )->getString() + ", "
                + viewVolume->header().getProperty( "sizeZ" )->getString() + ", "
                + viewVolume->header().getProperty( "sizeT" )->getString() + " )"
                , "VOLUMEUTILIO" );
      cartoCondMsg( VIO_V+1, "===", "VOLUMEUTILIO" );
      //------------------------------------------------------------------------
    }
    else 
    {
      //=== ALLOCATED PROCESSED VOLUME =========================================
      cartoCondMsg( VIO_V, "=== ALLOCATED PROCESSED VOLUME", "VOLUMEUTILIO" );
      rView = soma::Reader<carto::Volume<T> >( rVol.dataSourceInfo() );
      newoptions = carto::Object::value( carto::PropertySet() );
      newoptions->setProperty( "partial_reading", true );
      newoptions->copyProperties( options );
      rView.setOptions( newoptions );
      cartoCondMsg( VIO_V, "creating volume...", "VOLUMEUTILIO" );
      viewVolume = new carto::VolumeView<T>( fullVolume, viewpos, viewframe );
      cartoCondMsg( VIO_V, "reading partial volume...", "VOLUMEUTILIO" );
      rView.read( *viewVolume );
      cartoCondMsg( VIO_V, "===", "VOLUMEUTILIO" );

      //--- HEADER -------------------------------------------------------------
      cartoCondMsg( VIO_V+1, "=== HEADER", "VOLUMEUTILIO" );
      cartoCondMsg( VIO_V+1, "Volume's :", "VOLUMEUTILIO" );;
      printheader( VIO_V+1, carto::Object::value( viewVolume->header() ) );
      cartoCondMsg( VIO_V+1, "Volume size : ( "
                + viewVolume->header().getProperty( "sizeX" )->getString() + ", " 
                + viewVolume->header().getProperty( "sizeY" )->getString() + ", "
                + viewVolume->header().getProperty( "sizeZ" )->getString() + ", "
                + viewVolume->header().getProperty( "sizeT" )->getString() + " )"
                , "VOLUMEUTILIO" );
      cartoCondMsg( VIO_V+1, "Reader's :", "VOLUMEUTILIO" );
      printheader( VIO_V+1, rView.dataSourceInfo()->header() );
      cartoCondMsg( VIO_V+1, "===", "VOLUMEUTILIO" );
      //------------------------------------------------------------------------
    }
    
    return viewVolume;
  }

}

#undef VIO_V

#endif