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
#include <cartodata/io/volumeutilio.h>
#include <cartodata/volume/volume.h>
#include <cartodata/volume/volumeview.h>
//--- soma-io ------------------------------------------------------------------
#include <soma-io/config/soma_config.h>
#include <soma-io/io/reader.h>
#include <soma-io/datasourceinfo/datasourceinfoloader.h>
#include <soma-io/datasourceinfo/datasourceinfo.h>
#include <soma-io/datasource/filedatasource.h>
#include <soma-io/writer/pythonwriter.h>
//--- cartobase ----------------------------------------------------------------
#include <cartobase/config/verbose.h>
#include <cartobase/object/object.h>
#include <cartobase/object/property.h>
#include <cartobase/stream/fileutil.h>
//--- system -------------------------------------------------------------------
#include <iostream>
#include <vector>
//------------------------------------------------------------------------------
#define VOLUMEUTILIO_VERBOSITY 3

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
    return properties;
  }

  //============================================================================
  //   R E A D
  //============================================================================

  template <typename T> carto::Volume<T>*
  VolumeUtilIO<T>::read( carto::rc_ptr<DataSourceInfo> dsi, carto::Object options )
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
  VolumeUtilIO<T>::readFull( carto::rc_ptr<DataSourceInfo> dsi, 
                          std::vector<int> borders, carto::Object options )
  {
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

      printTitle( "READING FULL VOLUME SIZE", VOLUMEUTILIO_VERBOSITY );
      newoptions = carto::Object::value( carto::PropertySet() );
      newoptions->copyProperties( options );
      printMessage( "checking full volume...", VOLUMEUTILIO_VERBOSITY );
      *dsi = dsil.check( *dsi, options );
      printMessage( "reading size...", VOLUMEUTILIO_VERBOSITY );
      fullsize[0] = dsi->header()->getProperty( "sizeX" )->getScalar();
      fullsize[1] = dsi->header()->getProperty( "sizeY" )->getScalar();
      fullsize[2] = dsi->header()->getProperty( "sizeZ" )->getScalar();
      fullsize[3] = dsi->header()->getProperty( "sizeT" )->getScalar();
      printMessage( "-> size =  ( "
                    + carto::FileUtil::itos( fullsize[0] ) + ", "
                    + carto::FileUtil::itos( fullsize[1] ) + ", "
                    + carto::FileUtil::itos( fullsize[2] ) + ", "
                    + carto::FileUtil::itos( fullsize[3] ) + " )"
                    , VOLUMEUTILIO_VERBOSITY );
      printEnd( VOLUMEUTILIO_VERBOSITY );

      printTitle( "ALLOCATED BORDERS VOLUME", VOLUMEUTILIO_VERBOSITY );
      printMessage( "computing sizes...", VOLUMEUTILIO_VERBOSITY );
      bordersize[0] = fullsize[0] + 2*borders[0];
      bordersize[1] = fullsize[1] + 2*borders[1];
      bordersize[2] = fullsize[2] + 2*borders[2];
      bordersize[3] = fullsize[3];
      printMessage( "creating allocated volume...", VOLUMEUTILIO_VERBOSITY );
      printMessage( "-> with size ( "
                    + carto::FileUtil::itos( bordersize[0] ) + ", "
                    + carto::FileUtil::itos( bordersize[1] ) + ", "
                    + carto::FileUtil::itos( bordersize[2] ) + ", "
                    + carto::FileUtil::itos( bordersize[3] ) + " )"
                    , VOLUMEUTILIO_VERBOSITY );
      bordersVolume = carto::VolumeRef<T>( new carto::Volume<T>( bordersize[0], bordersize[1], bordersize[2], bordersize[3] ) );
      printEnd( VOLUMEUTILIO_VERBOSITY );
      
      printTitle( "HEADER", VOLUMEUTILIO_VERBOSITY+1 );
      if( verbose >= 2 ) {
        std::cout << "Volume size : ( " 
                  << bordersVolume->header().getProperty( "sizeX" )->getScalar() << ", " 
                  << bordersVolume->header().getProperty( "sizeY" )->getScalar() << ", "
                  << bordersVolume->header().getProperty( "sizeZ" )->getScalar() << ", "
                  << bordersVolume->header().getProperty( "sizeT" )->getScalar() << " )"
                  << std::endl;
      }
      printEnd( VOLUMEUTILIO_VERBOSITY+1 );

      printTitle( "UNALLOCATED FULL VIEW", VOLUMEUTILIO_VERBOSITY );
      rVol = soma::Reader< carto::Volume<T> >( dsi );
      newoptions = carto::Object::value( carto::PropertySet() );
      newoptions->copyProperties( options );
      rVol.setOptions( newoptions );
      printMessage( "creating volume view...", VOLUMEUTILIO_VERBOSITY );
      printMessage( "-> with pos ( "
                    + carto::FileUtil::itos( volumepos[0] ) + ", "
                    + carto::FileUtil::itos( volumepos[1] ) + ", "
                    + carto::FileUtil::itos( volumepos[2] ) + ", "
                    + carto::FileUtil::itos( volumepos[3] ) + " )"
                    , VOLUMEUTILIO_VERBOSITY );
      printMessage( "-> with size ( "
                    + carto::FileUtil::itos( fullsize[0] ) + ", "
                    + carto::FileUtil::itos( fullsize[1] ) + ", "
                    + carto::FileUtil::itos( fullsize[2] ) + ", "
                    + carto::FileUtil::itos( fullsize[3] ) + " )"
                    , VOLUMEUTILIO_VERBOSITY );
      fullVolume = new carto::VolumeView<T>( bordersVolume, volumepos, fullsize );
      printMessage( "reading unallocated volume...", VOLUMEUTILIO_VERBOSITY );
      rVol.read( *fullVolume );
      printEnd( VOLUMEUTILIO_VERBOSITY );

      printTitle( "HEADER", VOLUMEUTILIO_VERBOSITY+1 );
      if( verbose >= VOLUMEUTILIO_VERBOSITY+1 ) {
        printMessage( "Volume's :", VOLUMEUTILIO_VERBOSITY+1 );
        printheader( carto::Object::value( fullVolume->header() ) );
        std::cout << "Volume size : ( " 
                  << fullVolume->header().getProperty( "sizeX" )->getScalar() << ", " 
                  << fullVolume->header().getProperty( "sizeY" )->getScalar() << ", "
                  << fullVolume->header().getProperty( "sizeZ" )->getScalar() << ", "
                  << fullVolume->header().getProperty( "sizeT" )->getScalar() << " )"
                  << std::endl;
        printMessage( "Reader's :", VOLUMEUTILIO_VERBOSITY+1 );
        printheader( rVol.dataSourceInfo()->header() );
      }
      printEnd( VOLUMEUTILIO_VERBOSITY+1 );
    }
    else
    {
      printTitle( "ALLOCATED FULL VIEW", VOLUMEUTILIO_VERBOSITY );
      rVol = soma::Reader< carto::Volume<T> >( dsi );
      newoptions = carto::Object::value( carto::PropertySet() );
      newoptions->copyProperties( options );
      rVol.setOptions( newoptions );
      printMessage( "reading volume...", VOLUMEUTILIO_VERBOSITY );
      fullVolume = rVol.read();
      printEnd( VOLUMEUTILIO_VERBOSITY );

      printTitle( "HEADER", VOLUMEUTILIO_VERBOSITY+1 );
      if( verbose >= VOLUMEUTILIO_VERBOSITY+1 ) {
        printMessage( "Volume's :", VOLUMEUTILIO_VERBOSITY+1 );
        printheader( carto::Object::value( fullVolume->header() ) );
        std::cout << "Volume size : ( " 
                  << fullVolume->header().getProperty( "sizeX" )->getScalar() << ", " 
                  << fullVolume->header().getProperty( "sizeY" )->getScalar() << ", "
                  << fullVolume->header().getProperty( "sizeZ" )->getScalar() << ", "
                  << fullVolume->header().getProperty( "sizeT" )->getScalar() << " )"
                  << std::endl;
        printMessage( "Reader's :", VOLUMEUTILIO_VERBOSITY+1 );
        printheader( rVol.dataSourceInfo()->header() );
      }
      printEnd( VOLUMEUTILIO_VERBOSITY+1 );
    }

    return fullVolume;
  }

  template <typename T> carto::Volume<T>* 
  VolumeUtilIO<T>::readPartial( carto::rc_ptr<DataSourceInfo> dsi, 
                               std::vector<int> position, std::vector<int> frame,
                               std::vector<int> borders, carto::Object options )
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

    printTitle( "UNALLOCATED FULL VOLUME", VOLUMEUTILIO_VERBOSITY );
    rVol = soma::Reader<carto::Volume<T> >( dsi );
    newoptions = carto::Object::value( carto::PropertySet() );
    newoptions->setProperty( "unallocated", true );
    newoptions->copyProperties( options );
    rVol.setOptions( newoptions );
    printMessage( "reading unallocated volume...", VOLUMEUTILIO_VERBOSITY );
    fullVolume = carto::VolumeRef<T>( rVol.read() );
    printMessage( "reading size from volume...", VOLUMEUTILIO_VERBOSITY );
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
    printEnd( 1 );
    
    printTitle( "HEADER", VOLUMEUTILIO_VERBOSITY+1 );
    if( verbose >= VOLUMEUTILIO_VERBOSITY+1 ) {
      printMessage( "Volume's :", VOLUMEUTILIO_VERBOSITY+1 );
      printheader( carto::Object::value( fullVolume->header() ) );
      std::cout << "Volume size : ( " 
                << fullVolume->header().getProperty( "sizeX" )->getScalar() << ", " 
                << fullVolume->header().getProperty( "sizeY" )->getScalar() << ", "
                << fullVolume->header().getProperty( "sizeZ" )->getScalar() << ", "
                << fullVolume->header().getProperty( "sizeT" )->getScalar() << " )"
                << std::endl;
      printMessage( "Reader's :", VOLUMEUTILIO_VERBOSITY+1 );
      printheader( rVol.dataSourceInfo()->header() );
    }
    printEnd( VOLUMEUTILIO_VERBOSITY+1 );

    if( borders[0] != 0 || borders[1] != 0 || borders[2] != 0 ) {
      //=== ALLOCATED BORDERS VOLUME ===========================================
      
      printTitle( "ALLOCATED BORDERS VOLUME", VOLUMEUTILIO_VERBOSITY );
      printMessage( "computing borders...", VOLUMEUTILIO_VERBOSITY );
      // setting "bordersVolume" position / "fullVolume"
      borderpos = viewpos;
      borderpos[0] -= borders[0];
      borderpos[1] -= borders[1];
      borderpos[2] -= borders[2];
      borderframe = viewframe;
      borderframe[0] += 2*borders[0];
      borderframe[1] += 2*borders[1];
      borderframe[2] += 2*borders[2];
      printMessage( "creating volume...", VOLUMEUTILIO_VERBOSITY );
      printMessage( "with frame size ( "
                    + carto::FileUtil::itos( borderframe[0] ) + ", "
                    + carto::FileUtil::itos( borderframe[1] ) + ", "
                    + carto::FileUtil::itos( borderframe[2] ) + ", "
                    + carto::FileUtil::itos( borderframe[3] ) + " )"
                    , VOLUMEUTILIO_VERBOSITY );
      printMessage( "with frame pos ( "
                    + carto::FileUtil::itos( borderpos[0] ) + ", "
                    + carto::FileUtil::itos( borderpos[1] ) + ", "
                    + carto::FileUtil::itos( borderpos[2] ) + ", "
                    + carto::FileUtil::itos( borderpos[3] ) + " )"
                    , VOLUMEUTILIO_VERBOSITY );
      bordersVolume = carto::VolumeRef<T>( new carto::VolumeView<T>( fullVolume, borderpos, borderframe ) );
      printEnd( VOLUMEUTILIO_VERBOSITY );
      
      printTitle( "HEADER", VOLUMEUTILIO_VERBOSITY+1 );
      if( verbose >= VOLUMEUTILIO_VERBOSITY+1 ) {
        std::cout << "Volume size : ( " 
                  << bordersVolume->header().getProperty( "sizeX" )->getScalar() << ", " 
                  << bordersVolume->header().getProperty( "sizeY" )->getScalar() << ", "
                  << bordersVolume->header().getProperty( "sizeZ" )->getScalar() << ", "
                  << bordersVolume->header().getProperty( "sizeT" )->getScalar() << " )"
                  << std::endl;
      }
      printEnd( VOLUMEUTILIO_VERBOSITY+1 );

      //=== UNALLOCATED READ VIEW ==============================================

      printTitle( "UNALLOCATED READ VIEW", VOLUMEUTILIO_VERBOSITY );
      printMessage( "computing read frame...", VOLUMEUTILIO_VERBOSITY );
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
      printMessage( "creating volume...", VOLUMEUTILIO_VERBOSITY );
      printMessage( "with frame size ( "
                    + carto::FileUtil::itos( readframe[0] ) + ", "
                    + carto::FileUtil::itos( readframe[1] ) + ", "
                    + carto::FileUtil::itos( readframe[2] ) + ", "
                    + carto::FileUtil::itos( readframe[3] ) + " )"
                    , VOLUMEUTILIO_VERBOSITY );
      printMessage( "with frame pos ( "
                    + carto::FileUtil::itos( readpos[0] ) + ", "
                    + carto::FileUtil::itos( readpos[1] ) + ", "
                    + carto::FileUtil::itos( readpos[2] ) + ", "
                    + carto::FileUtil::itos( readpos[3] ) + " )"
                    , VOLUMEUTILIO_VERBOSITY );
      readVolume = carto::VolumeRef<T>( new carto::VolumeView<T>( bordersVolume, readpos, readframe ) );
      printMessage( "reading frame...", VOLUMEUTILIO_VERBOSITY );
      rView.read( *readVolume );
      printEnd( VOLUMEUTILIO_VERBOSITY );
      
      printTitle( "HEADER", VOLUMEUTILIO_VERBOSITY+1 );
      if( verbose >= VOLUMEUTILIO_VERBOSITY+1 ) {
        printMessage( "Volume's :", VOLUMEUTILIO_VERBOSITY+1 );
        printheader( carto::Object::value( readVolume->header() ) );
        std::cout << "Volume size : ( " 
                  << readVolume->header().getProperty( "sizeX" )->getScalar() << ", " 
                  << readVolume->header().getProperty( "sizeY" )->getScalar() << ", "
                  << readVolume->header().getProperty( "sizeZ" )->getScalar() << ", "
                  << readVolume->header().getProperty( "sizeT" )->getScalar() << " )"
                  << std::endl;
        printMessage( "Reader's :", VOLUMEUTILIO_VERBOSITY+1 );
        printheader( rView.dataSourceInfo()->header() );
      }
      printEnd( VOLUMEUTILIO_VERBOSITY+1 );
      
      //=== UNALLOCATED PROCCESSED VOLUME ======================================
      
      printTitle( "UNALLOCATED PROCCESSED VOLUME", VOLUMEUTILIO_VERBOSITY );
      printMessage( "computing view frame...", VOLUMEUTILIO_VERBOSITY );
      // setting "viewVolume" position / "bordersVolume"
      viewpos[0] = borders[0];
      viewpos[1] = borders[1];
      viewpos[2] = borders[2];
      viewpos[3] = 0;
      printMessage( "creating volume...", VOLUMEUTILIO_VERBOSITY );
      printMessage( "with frame size ( "
                    + carto::FileUtil::itos( viewframe[0] ) + ", "
                    + carto::FileUtil::itos( viewframe[1] ) + ", "
                    + carto::FileUtil::itos( viewframe[2] ) + ", "
                    + carto::FileUtil::itos( viewframe[3] ) + " )"
                    , VOLUMEUTILIO_VERBOSITY );
      printMessage( "with frame pos ( "
                    + carto::FileUtil::itos( viewpos[0] ) + ", "
                    + carto::FileUtil::itos( viewpos[1] ) + ", "
                    + carto::FileUtil::itos( viewpos[2] ) + ", "
                    + carto::FileUtil::itos( viewpos[3] ) + " )"
                    , VOLUMEUTILIO_VERBOSITY );
      viewVolume = new carto::VolumeView<T>( bordersVolume, viewpos, viewframe );
      printMessage( "copying header...", VOLUMEUTILIO_VERBOSITY );
      viewVolume->blockSignals( true );
      viewVolume->header().copyProperties( carto::Object::value( readVolume->header() ) );
      carto::PropertySet & ps = viewVolume->header();
      ps.setProperty( "sizeX", viewframe[0] );
      ps.setProperty( "sizeY", viewframe[1] );
      ps.setProperty( "sizeZ", viewframe[2] );
      ps.setProperty( "sizeT", viewframe[3] );
      viewVolume->blockSignals( false );
      printEnd( VOLUMEUTILIO_VERBOSITY );
      
      printTitle( "HEADER", VOLUMEUTILIO_VERBOSITY+1 );
      if( verbose >= VOLUMEUTILIO_VERBOSITY+1 ) {
        printMessage( "Volume's :", VOLUMEUTILIO_VERBOSITY+1 );
        printheader( carto::Object::value( viewVolume->header() ) );
        std::cout << "Volume size : ( " 
                  << viewVolume->header().getProperty( "sizeX" )->getScalar() << ", " 
                  << viewVolume->header().getProperty( "sizeY" )->getScalar() << ", "
                  << viewVolume->header().getProperty( "sizeZ" )->getScalar() << ", "
                  << viewVolume->header().getProperty( "sizeT" )->getScalar() << " )"
                  << std::endl;
      }
      printEnd( VOLUMEUTILIO_VERBOSITY+1 );
    }
    else 
    {
      //=== ALLOCATED PROCESSED VOLUME =========================================
      
      printTitle( "ALLOCATED PROCESSED VOLUME", VOLUMEUTILIO_VERBOSITY );
      rView = soma::Reader<carto::Volume<T> >( rVol.dataSourceInfo() );
      newoptions = carto::Object::value( carto::PropertySet() );
      newoptions->setProperty( "partial_reading", true );
      newoptions->copyProperties( options );
      rView.setOptions( newoptions );
      printMessage( "creating volume...", VOLUMEUTILIO_VERBOSITY );
      viewVolume = new carto::VolumeView<T>( fullVolume, viewpos, viewframe );
      printMessage( "reading partial volume...", VOLUMEUTILIO_VERBOSITY );
      rView.read( *viewVolume );
      printEnd( VOLUMEUTILIO_VERBOSITY );
      
      printTitle( "HEADER", VOLUMEUTILIO_VERBOSITY+1 );
      if( verbose >= VOLUMEUTILIO_VERBOSITY+1 ) {
        printMessage( "Volume's :", VOLUMEUTILIO_VERBOSITY+1 );;
        printheader( carto::Object::value( viewVolume->header() ) );
        std::cout << "Volume size : ( " 
                  << viewVolume->header().getProperty( "sizeX" )->getScalar() << ", " 
                  << viewVolume->header().getProperty( "sizeY" )->getScalar() << ", "
                  << viewVolume->header().getProperty( "sizeZ" )->getScalar() << ", "
                  << viewVolume->header().getProperty( "sizeT" )->getScalar() << " )"
                  << std::endl;
        printMessage( "Reader's :", VOLUMEUTILIO_VERBOSITY+1 );
        printheader( rView.dataSourceInfo()->header() );
      }
      printEnd( VOLUMEUTILIO_VERBOSITY+1 );
    }
    
    return viewVolume;
  }

}

#undef VERBOSITY

#endif