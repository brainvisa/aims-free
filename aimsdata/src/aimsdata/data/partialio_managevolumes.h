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
#ifndef PARTIALIO_MANAGE_VOLUMES
#define PARTIALIO_MANAGE_VOLUMES

//--- cartodata ----------------------------------------------------------------
#include <cartodata/volume/volume.h>
#include <cartodata/volume/volumeview.h>
//--- soma-io ------------------------------------------------------------------
#include <soma-io/io/reader.h>
#include <soma-io/datasourceinfo/datasourceinfoloader.h>
#include <soma-io/datasourceinfo/datasourceinfo.h>
#include <soma-io/datasource/filedatasource.h>
#include <soma-io/writer/pythonwriter.h>
//--- cartobase ----------------------------------------------------------------
#include <cartobase/config/verbose.h>
#include <cartobase/object/object.h>
#include <cartobase/object/property.h>
//--- system -------------------------------------------------------------------
#include <iostream>
#include <vector>
#include <sstream>
//------------------------------------------------------------------------------

namespace partialio {
  
  void printheader( carto::Object hdr )
  {
    soma::PythonWriter  pw;
    pw.attach( std::cout );
    pw.write( hdr );
  }
  
  void printTitle( const std::string & title, int verbosity = 0, char symb = '=' )
  {
    if( carto::debugMessageLevel >= verbosity ) {
      int title_size = title.size();
      int col_size = 80;
      int symb_1_count = ( col_size - title_size - 2 ) / 2;
      int symb_2_count = col_size - symb_1_count - title_size - 2;
      std::string out( symb_1_count, symb );
      out += ( " " + title + " " );
      if( symb_2_count > 0 )
        out += std::string( symb_2_count, symb );
      std::cout << std::endl << out << std::endl;
    }
  }
  
  void printMessage( const std::string & message, int verbosity = 0 )
  {
    if( carto::debugMessageLevel >= verbosity ) {
      std::cout << message << std::endl;
    }
  }
  
  void printEnd( int verbosity = 0, char symb = '=' )
  {
    if( carto::debugMessageLevel >= verbosity ) {
      int col_size = 80;
      std::cout << std::string( col_size, symb ) << std::endl;
    }
  }
  
  std::string itos( int number )
  {
    std::ostringstream oss;
    oss << number;
    return oss.str();
  }
  
  //============================================================================
  //   D E C L A R A T I O N
  //============================================================================
  template <typename T> carto::VolumeRef<T> 
  read( const std::string & fname, 
        std::vector<int> inframe, std::vector<int> inpos,
        std::vector<int> border, int resolution );
  
  template <typename T> carto::VolumeRef<T> 
  readPartial( const std::string & fname, 
               std::vector<int> inframe, std::vector<int> inpos,
               std::vector<int> border, int resolution );
  
  template <typename T> carto::VolumeRef<T> 
  readFull( const std::string & fname, std::vector<int> border, int resolution );
  
  //============================================================================
  //   D E F I N I T I O N
  //============================================================================
  
  template <typename T> carto::VolumeRef<T> 
  read( const std::string & fname, 
        std::vector<int> inframe = std::vector<int>(4,0),
        std::vector<int> inpos = std::vector<int>(4,0),
        std::vector<int> border = std::vector<int>(3,0),
        int resolution = 0 )
  {
    int verbose = carto::debugMessageLevel;
    bool partial = !( inframe[0] == 0 && inframe[1] == 0 &&
                     inframe[2] == 0 && inframe[3] == 0 );

    printTitle( "PARTIALIO READING AND ALLOCATING :: PARAMETERS", 1 );
    if( verbose > 0 ) {
      std::cout << "Filename : " << fname << std::endl;
      std::cout << "Borders : ( " 
                << border[0] << ", "
                << border[1] << ", "
                << border[2] << " )"
                << std::endl;
      std::cout << "Resolution level : " << resolution << std::endl;
      std::cout << "Partial reading : " << ( partial ? "yes" : "no" ) << std::endl;
      if( partial ) {
        std::cout << "Frame size : ( " 
                  << inframe[0] << ", "
                  << inframe[1] << ", "
                  << inframe[2] << ", "
                  << inframe[3] << " )"
                  << std::endl;
        std::cout << "Frame position : ( "
                  << inpos[0] << ", "
                  << inpos[1] << ", "
                  << inpos[2] << ", "
                  << inpos[3] << " )"
                  << std::endl;
      }
    }
    printEnd( 1 );

    if( partial )
      return readPartial<T>( fname, inframe, inpos, border, resolution );
    else
      return readFull<T>( fname, border, resolution );
  }

  template <typename T> carto::VolumeRef<T> 
  readFull( const std::string & fname, std::vector<int> border, int resolution )
  {
    int verbose = carto::debugMessageLevel;
    carto::Object options;
    soma::Reader<carto::Volume<T> > rVol;
    soma::DataSourceInfoLoader dsil;
    carto::rc_ptr<soma::DataSourceInfo> dsi;
    carto::VolumeRef<T> bordersVolume;
    carto::VolumeRef<T> fullVolume;
    typename carto::VolumeView<T>::Position4Di fullsize;
    typename carto::VolumeView<T>::Position4Di bordersize;
    typename carto::VolumeView<T>::Position4Di volumepos( border[0], border [1], border[2], 0 );
    
    if( border[0] !=0 || border[1] !=0 || border[2] !=0 )
    {
      
      printTitle( "READING FULL VOLUME SIZE", 1 );
      options = carto::Object::value( carto::PropertySet() );
      options->setProperty( "resolution_level", resolution );
      dsi.reset( new soma::DataSourceInfo( fname ) );
      printMessage( "checking full volume...", 1 );
      *dsi = dsil.check( *dsi, options );
      printMessage( "reading size...", 1 );
      fullsize[0] = dsi->header()->getProperty( "sizeX" )->getScalar();
      fullsize[1] = dsi->header()->getProperty( "sizeY" )->getScalar();
      fullsize[2] = dsi->header()->getProperty( "sizeZ" )->getScalar();
      fullsize[3] = dsi->header()->getProperty( "sizeT" )->getScalar();
      printMessage( "-> size =  ( "
                    + itos( fullsize[0] ) + ", "
                    + itos( fullsize[1] ) + ", "
                    + itos( fullsize[2] ) + ", "
                    + itos( fullsize[3] ) + " )"
                    , 1 );
      printEnd( 1 );

      printTitle( "ALLOCATED BORDERS VOLUME", 1 );
      printMessage( "computing sizes...", 1 );
      bordersize[0] = fullsize[0] + 2*border[0];
      bordersize[1] = fullsize[1] + 2*border[1];
      bordersize[2] = fullsize[2] + 2*border[2];
      bordersize[3] = fullsize[3];
      printMessage( "creating allocated volume...", 1 );
      printMessage( "-> with size ( "
                    + itos( bordersize[0] ) + ", "
                    + itos( bordersize[1] ) + ", "
                    + itos( bordersize[2] ) + ", "
                    + itos( bordersize[3] ) + " )"
                    , 1 );
      bordersVolume = carto::VolumeRef<T>( new carto::Volume<T>( bordersize[0], bordersize[1], bordersize[2], bordersize[3] ) );
      printEnd( 1 );
      
      printTitle( "HEADER", 2 );
      if( verbose >= 2 ) {
        std::cout << "Volume size : ( " 
                  << bordersVolume->header().getProperty( "sizeX" )->getScalar() << ", " 
                  << bordersVolume->header().getProperty( "sizeY" )->getScalar() << ", "
                  << bordersVolume->header().getProperty( "sizeZ" )->getScalar() << ", "
                  << bordersVolume->header().getProperty( "sizeT" )->getScalar() << " )"
                  << std::endl;
      }
      printEnd( 2 );
      
      printTitle( "UNALLOCATED FULL VIEW", 1 );
      rVol = soma::Reader< carto::Volume<T> >( dsi );
      options = carto::Object::value( carto::PropertySet() );
      options->setProperty( "resolution_level", resolution );
      rVol.setOptions( options );
      printMessage( "creating volume view...", 1 );
      printMessage( "-> with pos ( "
                    + itos( volumepos[0] ) + ", "
                    + itos( volumepos[1] ) + ", "
                    + itos( volumepos[2] ) + ", "
                    + itos( volumepos[3] ) + " )"
                    , 1 );
      printMessage( "-> with size ( "
                    + itos( fullsize[0] ) + ", "
                    + itos( fullsize[1] ) + ", "
                    + itos( fullsize[2] ) + ", "
                    + itos( fullsize[3] ) + " )"
                    , 1 );
      fullVolume = carto::VolumeRef<T>( new carto::VolumeView<T>( bordersVolume, volumepos, fullsize ) );
      printMessage( "reading unallocated volume...", 1 );
      rVol.read( *fullVolume );
      printEnd( 1 );
      
      printTitle( "HEADER", 2 );
      if( verbose >= 2 ) {
        printMessage( "Volume's :", 2 );
        printheader( carto::Object::value( fullVolume->header() ) );
        std::cout << "Volume size : ( " 
                  << fullVolume->header().getProperty( "sizeX" )->getScalar() << ", " 
                  << fullVolume->header().getProperty( "sizeY" )->getScalar() << ", "
                  << fullVolume->header().getProperty( "sizeZ" )->getScalar() << ", "
                  << fullVolume->header().getProperty( "sizeT" )->getScalar() << " )"
                  << std::endl;
        printMessage( "Reader's :", 1 );
        printheader( rVol.dataSourceInfo()->header() );
      }
      printEnd( 2 );
    }
    else
    {
      printTitle( "ALLOCATED FULL VIEW", 1 );
      rVol = soma::Reader< carto::Volume<T> >( fname );
      options = carto::Object::value( carto::PropertySet() );
      options->setProperty( "resolution_level", resolution );
      rVol.setOptions( options );
      printMessage( "reading volume...", 1 );
      fullVolume = carto::VolumeRef<T>( rVol.read() );
      printEnd( 1 );
      
      printTitle( "HEADER", 2 );
      if( verbose >= 2 ) {
        printMessage( "Volume's :", 2 );
        printheader( carto::Object::value( fullVolume->header() ) );
        std::cout << "Volume size : ( " 
                  << fullVolume->header().getProperty( "sizeX" )->getScalar() << ", " 
                  << fullVolume->header().getProperty( "sizeY" )->getScalar() << ", "
                  << fullVolume->header().getProperty( "sizeZ" )->getScalar() << ", "
                  << fullVolume->header().getProperty( "sizeT" )->getScalar() << " )"
                  << std::endl;
        printMessage( "Reader's :", 1 );
        printheader( rVol.dataSourceInfo()->header() );
      }
      printEnd( 2 );
    }
    
    return fullVolume;

  }

  template <typename T> carto::VolumeRef<T> 
  readPartial( const std::string & fname, 
               std::vector<int> inframe = std::vector<int>(4,0),
               std::vector<int> inpos = std::vector<int>(4,0),
               std::vector<int> border = std::vector<int>(3,0),
               int resolution = 0 )
  {
    //=== VARIABLES ============================================================
    int verbose = carto::debugMessageLevel;

    typename carto::VolumeView<T>::Position4Di viewframe( inframe[0], inframe[1], 
                                                   inframe[2], inframe[3] );
    typename carto::VolumeView<T>::Position4Di viewpos( inpos[0], inpos[1], 
                                                 inpos[2], inpos[3] );
    typename carto::VolumeView<T>::Position4Di fullsize;     // full size
    typename carto::VolumeView<T>::Position4Di borderframe;  // allocated volume size
    typename carto::VolumeView<T>::Position4Di borderpos;    // allocated volume size
    typename carto::VolumeView<T>::Position4Di readframe;    // read frame size
    typename carto::VolumeView<T>::Position4Di readpos;      // read frame origin
    carto::Object options;
    soma::Reader<carto::Volume<T> > rVol;
    soma::Reader<carto::Volume<T> > rView;
    carto::VolumeRef<T> fullVolume, bordersVolume, readVolume, viewVolume;

    //=== UNALLOCATED FULL VOLUME ==============================================

    printTitle( "UNALLOCATED FULL VOLUME", 1 );
    rVol = soma::Reader<carto::Volume<T> >( fname );
    options = carto::Object::value( carto::PropertySet() );
    options->setProperty( "unallocated", true );
    options->setProperty( "resolution_level", resolution );
    rVol.setOptions( options );
    printMessage( "reading unallocated volume...", 1 );
    fullVolume = carto::VolumeRef<T>( rVol.read() );
    printMessage( "reading size from volume...", 1 );
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
    
    printTitle( "HEADER", 2 );
    if( verbose >= 2 ) {
      printMessage( "Volume's :", 2 );
      printheader( carto::Object::value( fullVolume->header() ) );
      std::cout << "Volume size : ( " 
                << fullVolume->header().getProperty( "sizeX" )->getScalar() << ", " 
                << fullVolume->header().getProperty( "sizeY" )->getScalar() << ", "
                << fullVolume->header().getProperty( "sizeZ" )->getScalar() << ", "
                << fullVolume->header().getProperty( "sizeT" )->getScalar() << " )"
                << std::endl;
      printMessage( "Reader's :", 1 );
      printheader( rVol.dataSourceInfo()->header() );
    }
    printEnd( 2 );

    if( border[0] != 0 || border[1] != 0 || border[2] != 0 ) {
      //=== ALLOCATED BORDERS VOLUME ===========================================
      
      printTitle( "ALLOCATED BORDERS VOLUME", 1 );
      printMessage( "computing borders...", 1 );
      // setting "bordersVolume" position / "fullVolume"
      borderpos = viewpos;
      borderpos[0] -= border[0];
      borderpos[1] -= border[1];
      borderpos[2] -= border[2];
      borderframe = viewframe;
      borderframe[0] += 2*border[0];
      borderframe[1] += 2*border[1];
      borderframe[2] += 2*border[2];
      printMessage( "creating volume...", 1 );
      printMessage( "with frame size ( "
                    + itos( borderframe[0] ) + ", "
                    + itos( borderframe[1] ) + ", "
                    + itos( borderframe[2] ) + ", "
                    + itos( borderframe[3] ) + " )"
                    , 1 );
      printMessage( "with frame pos ( "
                    + itos( borderpos[0] ) + ", "
                    + itos( borderpos[1] ) + ", "
                    + itos( borderpos[2] ) + ", "
                    + itos( borderpos[3] ) + " )"
                    , 1 );
      bordersVolume = carto::VolumeRef<T>( new carto::VolumeView<T>( fullVolume, borderpos, borderframe ) );
      printEnd( 1 );
      
      printTitle( "HEADER", 2 );
      if( verbose >= 2 ) {
        std::cout << "Volume size : ( " 
                  << bordersVolume->header().getProperty( "sizeX" )->getScalar() << ", " 
                  << bordersVolume->header().getProperty( "sizeY" )->getScalar() << ", "
                  << bordersVolume->header().getProperty( "sizeZ" )->getScalar() << ", "
                  << bordersVolume->header().getProperty( "sizeT" )->getScalar() << " )"
                  << std::endl;
      }
      printEnd( 2 );

      //=== UNALLOCATED READ VIEW ==============================================

      printTitle( "UNALLOCATED READ VIEW", 1 );
      printMessage( "computing read frame...", 1 );
      // setting "readVolume" position / "bordersVolume"
      readpos[0] = ( borderpos[0] < 0 ? border[0] : 0 );
      readpos[1] = ( borderpos[1] < 0 ? border[1] : 0 );
      readpos[2] = ( borderpos[1] < 0 ? border[2] : 0 );
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
      options = carto::Object::value( carto::PropertySet() );
      options->setProperty( "partial_reading", true );
      options->setProperty( "resolution_level", resolution );
      rView.setOptions( options );
      printMessage( "creating volume...", 1 );
      printMessage( "with frame size ( "
                    + itos( readframe[0] ) + ", "
                    + itos( readframe[1] ) + ", "
                    + itos( readframe[2] ) + ", "
                    + itos( readframe[3] ) + " )"
                    , 1 );
      printMessage( "with frame pos ( "
                    + itos( readpos[0] ) + ", "
                    + itos( readpos[1] ) + ", "
                    + itos( readpos[2] ) + ", "
                    + itos( readpos[3] ) + " )"
                    , 1 );
      readVolume = carto::VolumeRef<T>( new carto::VolumeView<T>( bordersVolume, readpos, readframe ) );
      printMessage( "reading frame...", 1 );
      rView.read( *readVolume );
      printEnd( 1 );
      
      printTitle( "HEADER", 2 );
      if( verbose >= 2 ) {
        printMessage( "Volume's :", 2 );
        printheader( carto::Object::value( readVolume->header() ) );
        std::cout << "Volume size : ( " 
                  << readVolume->header().getProperty( "sizeX" )->getScalar() << ", " 
                  << readVolume->header().getProperty( "sizeY" )->getScalar() << ", "
                  << readVolume->header().getProperty( "sizeZ" )->getScalar() << ", "
                  << readVolume->header().getProperty( "sizeT" )->getScalar() << " )"
                  << std::endl;
        printMessage( "Reader's :", 2 );
        printheader( rView.dataSourceInfo()->header() );
      }
      printEnd( 2 );
      
      //=== UNALLOCATED PROCCESSED VOLUME ======================================
      
      printTitle( "UNALLOCATED PROCCESSED VOLUME", 1 );
      printMessage( "computing view frame...", 1 );
      // setting "viewVolume" position / "bordersVolume"
      viewpos[0] = border[0];
      viewpos[1] = border[1];
      viewpos[2] = border[2];
      viewpos[3] = 0;
      printMessage( "creating volume...", 1 );
      printMessage( "with frame size ( "
                    + itos( viewframe[0] ) + ", "
                    + itos( viewframe[1] ) + ", "
                    + itos( viewframe[2] ) + ", "
                    + itos( viewframe[3] ) + " )"
                    , 1 );
      printMessage( "with frame pos ( "
                    + itos( viewpos[0] ) + ", "
                    + itos( viewpos[1] ) + ", "
                    + itos( viewpos[2] ) + ", "
                    + itos( viewpos[3] ) + " )"
                    , 1 );
      viewVolume = carto::VolumeRef<T>( new carto::VolumeView<T>( bordersVolume, viewpos, viewframe ) );
      printMessage( "copying header...", 1 );
      viewVolume->blockSignals( true );
      viewVolume->header().copyProperties( carto::Object::value( readVolume->header() ) );
      carto::PropertySet & ps = viewVolume->header();
      ps.setProperty( "sizeX", viewframe[0] );
      ps.setProperty( "sizeY", viewframe[1] );
      ps.setProperty( "sizeZ", viewframe[2] );
      ps.setProperty( "sizeT", viewframe[3] );
      viewVolume->blockSignals( false );
      printEnd( 1 );
      
      printTitle( "HEADER", 2 );
      if( verbose >= 2 ) {
        printMessage( "Volume's :", 2 );
        printheader( carto::Object::value( viewVolume->header() ) );
        std::cout << "Volume size : ( " 
                  << viewVolume->header().getProperty( "sizeX" )->getScalar() << ", " 
                  << viewVolume->header().getProperty( "sizeY" )->getScalar() << ", "
                  << viewVolume->header().getProperty( "sizeZ" )->getScalar() << ", "
                  << viewVolume->header().getProperty( "sizeT" )->getScalar() << " )"
                  << std::endl;
      }
      printEnd( 2 );
    }
    else 
    {
      //=== ALLOCATED PROCESSED VOLUME =========================================
      
      printTitle( "ALLOCATED PROCESSED VOLUME", 1 );
      rView = soma::Reader<carto::Volume<T> >( rVol.dataSourceInfo() );
      options = carto::Object::value( carto::PropertySet() );
      options->setProperty( "partial_reading", true );
      options->setProperty( "resolution_level", resolution );
      rView.setOptions( options );
      printMessage( "creating volume...", 1 );
      viewVolume = carto::VolumeRef<T>( new carto::VolumeView<T>( fullVolume, viewpos, viewframe ) );
      printMessage( "reading partial volume...", 1 );
      rView.read( *viewVolume );
      printEnd( 1 );
      
      printTitle( "HEADER", 2 );
      if( verbose >= 2 ) {
        printMessage( "Volume's :", 2 );;
        printheader( carto::Object::value( viewVolume->header() ) );
        std::cout << "Volume size : ( " 
                  << viewVolume->header().getProperty( "sizeX" )->getScalar() << ", " 
                  << viewVolume->header().getProperty( "sizeY" )->getScalar() << ", "
                  << viewVolume->header().getProperty( "sizeZ" )->getScalar() << ", "
                  << viewVolume->header().getProperty( "sizeT" )->getScalar() << " )"
                  << std::endl;
        printMessage( "Reader's :", 2 );
        printheader( rView.dataSourceInfo()->header() );
      }
      printEnd( 2 );
    }
    
    return viewVolume;
  }
  
}

#endif