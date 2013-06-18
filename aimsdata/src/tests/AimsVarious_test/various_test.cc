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

//--- aims ---------------------------------------------------------------------
#include <aims/data/partialio_managevolumes.h>
//--- soma-io ------------------------------------------------------------------
#include <soma-io/config/soma_config.h>
#include <soma-io/io/writer.h>
#include <soma-io/writer/pythonwriter.h>
#include <soma-io/image/voxelrgba_d.h>
//--- cartodata ----------------------------------------------------------------
#include <cartodata/volume/volume.h>
#include <cartodata/volume/volumeview.h>
//--- cartobase ----------------------------------------------------------------
#include <cartobase/object/object.h>
#include <cartobase/getopt/getopt.h>
#include <cartobase/config/verbose.h>
//--- system -------------------------------------------------------------------
#include <iostream>
#include <cstdlib>
#include <limits>
//------------------------------------------------------------------------------

using namespace partialio;
using namespace soma;
using namespace carto;
using namespace std;

void printheader( Object hdr )
{
  PythonWriter  pw;
  pw.attach( cout );
  pw.write( hdr );
}

template <typename T>
void printVolume( const VolumeRef<T> & vol )
{
  int x, y, z, t;
  printEnd( 0, '-' );
  for( t=0; t<vol->getSizeT(); ++t ) {
    printTitle( "T = "+itos( t ), 0, ' ' );
    for( z=0; z<vol->getSizeZ(); ++z ) {
      printEnd( 0, '-' );
      for( y=0; y<vol->getSizeY(); ++y ) {
        for( x=0; x<vol->getSizeX(); ++x ) {
          cout << vol( x, y, z, t ) << "\t";
        }
        cout << endl;
      }
    }
  }
  printEnd( 0, '-' );
}

bool testVolumeBorders()
{
  printTitle( "VOLUME BORDERS" );
  int x, y, val = 0;
  printMessage( "creating allocatedVolume..." );
  printMessage( "-> size = ( 10, 10, 1, 1 )" );
  VolumeRef<int16_t>  allocatedVolume( new Volume<int16_t>( 10, 10, 1, 1 ) );
  printMessage( "filling allocated volume..." );
  for( x=0; x<10; ++x ) {
    for( y=0; y<10; ++y ) {
      allocatedVolume( x, y, 0, 0 ) = val++;
    }
  }
  printMessage( "printing allocatedVolume..." );
  printVolume( allocatedVolume );
  
  VolumeView<int16_t>::Position4Di pos( 2, 2, 0, 0 );
  VolumeView<int16_t>::Position4Di size( 6, 6, 1, 1 );
  printMessage( "creating viewVolume..." );
  printMessage( "-> father = allocatedVolume" );
  printMessage( "-> pos = ( 2, 2, 0, 0 )" );
  printMessage( "-> size = ( 6, 6, 1, 1 )" );
  VolumeRef<int16_t> viewVolume( new VolumeView<int16_t>( allocatedVolume, pos, size ) );
  printMessage( "printing viewVolume..." );
  printVolume( viewVolume );
  printEnd();
  return true;
}

bool testCreateVolume( string fname, int sx = 1, int sy = 1, 
                       int sz = 1, int st = 1 )
{
  printTitle( "CREATE NUMBERED VOLUME" );
  printMessage( "creating volume..." );
  VolumeRef<VoxelRGBA> vol( new Volume<VoxelRGBA>( sx, sy, sz, st ) );
  printMessage( "filling volume..." );
  int x, y, z, t, val = 0;
  for( t=0; t<st; ++t )
    for( z=0; z<sz; ++z )
      for( y=0; y<sy; ++y ) 
        for( x=0; x<sx; ++x ) {
          vol( x, y, z, t ) = VoxelRGBA(val,val,val,255);
          val++;
        }
  printMessage( "writing volume..." );
  Writer<VolumeRef<VoxelRGBA> > wVol( fname );
  wVol.write( vol );
  printEnd();
  
  return true;
}

int main( int argc, const char** argv )
{
  try
  {
    //=== APPLICATION ==========================================================
    bool volumeBorders = false;
    bool createVolume = false;
    string ofname = "/tmp/volume.ima";
    int sx = 1, sy = 1, sz = 1, st = 1;
    CartoApplication  app( argc, argv, "Various tests" );
    app.addOption( volumeBorders, "volumeBorders", "Views to an allocated"
                   "Volume and indexes\n", true );
    app.addOption( createVolume, "createVolume", "Creates a volume. Depends on"
                   "-o -sx -sy -sz -st options\n", true );
    app.addOption( ofname, "-o", "Out filename\n", true );
    app.addOption( sx, "-sx", "Size (comp x)\n", true );
    app.addOption( sy, "-sy", "Size (comp y)\n", true );
    app.addOption( sz, "-sz", "Size (comp z)\n", true );
    app.addOption( st, "-st", "Size (comp t)\n", true );
    app.alias( "-v", "--verbose" );
    app.initialize();
    
    //=== RUN ALGORITHM ========================================================
    if( volumeBorders )
      testVolumeBorders();
    if( createVolume )
      testCreateVolume( ofname, sx, sy, sz, st );
    
  }
  catch( user_interruption & )
  {
  }
  catch( exception & e )
  {
    cerr << e.what() << endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

