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
//--- soma-io ------------------------------------------------------------------
#include <soma-io/config/soma_config.h>
#include <soma-io/io/reader.h>
#include <soma-io/io/writer.h>
#include <soma-io/writer/pythonwriter.h>
//--- cartodata ----------------------------------------------------------------
#include <cartodata/volume/volume.h>
//--- cartobase ----------------------------------------------------------------
#include <cartobase/object/object.h>
#include <cartobase/getopt/getopt.h>
#include <cartobase/config/verbose.h>
#include <cartobase/type/string_conversion.h>
#include <cartobase/type/voxelrgba.h>
#include <cartobase/type/voxelrgb.h>
#include <aims/resampling/motion.h>
//--- system -------------------------------------------------------------------
#include <iostream>
//------------------------------------------------------------------------------

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
  cartoMsg( 0, "-----", "" );
  for( t=0; t<vol->getSizeT(); ++t ) {
    cartoMsg( 0, "=== T = "+toString( t ), "" );
    for( z=0; z<vol->getSizeZ(); ++z ) {
      cartoMsg( 0, "-----", "" );
      for( y=0; y<vol->getSizeY(); ++y ) {
        for( x=0; x<vol->getSizeX(); ++x ) {
          cout << vol( x, y, z, t ) << "\t";
        }
        cout << endl;
      }
    }
  }
  cartoMsg( 0, "-----", "" );
}

bool testVolumeBorders()
{
  cartoMsg( 0, "=== VOLUME BORDERS", "testVolumeBorders" );
  int x, y, val = 0;
  cartoMsg( 0, "creating allocatedVolume...", "testVolumeBorders" );
  cartoMsg( 0, "-> size = ( 10, 10, 1, 1 )", "testVolumeBorders" );
  VolumeRef<int16_t>  allocatedVolume( new Volume<int16_t>( 10, 10, 1, 1 ) );
  cartoMsg( 0, "filling allocated volume...", "testVolumeBorders" );
  for( x=0; x<10; ++x ) {
    for( y=0; y<10; ++y ) {
      allocatedVolume( x, y, 0, 0 ) = val++;
    }
  }
  cartoMsg( 0, "printing allocatedVolume...", "testVolumeBorders" );
  printVolume( allocatedVolume );
  
  Volume<int16_t>::Position4Di pos( 2, 2, 0, 0 );
  Volume<int16_t>::Position4Di size( 6, 6, 1, 1 );
  cartoMsg( 0, "creating viewVolume...", "testVolumeBorders" );
  cartoMsg( 0, "-> father = allocatedVolume", "testVolumeBorders" );
  cartoMsg( 0, "-> pos = ( 2, 2, 0, 0 )", "testVolumeBorders" );
  cartoMsg( 0, "-> size = ( 6, 6, 1, 1 )", "testVolumeBorders" );
  VolumeRef<int16_t> viewVolume( new Volume<int16_t>( allocatedVolume, pos, size ) );
  cartoMsg( 0, "printing viewVolume...", "testVolumeBorders" );
  printVolume( viewVolume );
  cartoMsg(0, "===", "testVolumeBorders");
  return true;
}

bool testCreateVolume( const string & fname, 
                       int sx = 1, int sy = 1, 
                       int sz = 1, int st = 1 )
{
  cartoMsg( 0, "=== CREATE NUMBERED VOLUME", "testCreateVolume" );
  cartoMsg( 0, "creating volume...", "testCreateVolume" );
  VolumeRef<VoxelRGB> vol( new Volume<VoxelRGB>( sx, sy, sz, st ) );
  cartoMsg( 0, "filling volume...", "testCreateVolume" );
  int x, y, z, t, val = 0;
  for( t=0; t<st; ++t )
    for( z=0; z<sz; ++z )
      for( y=0; y<sy; ++y ) 
        for( x=0; x<sx; ++x ) {
          vol( x, y, z, t ) = VoxelRGB(val,val,val);
          val++;
        }
  cartoMsg( 0, "writing volume...", "testCreateVolume" );
  Writer<VolumeRef<VoxelRGB> > wVol( fname );
  wVol.write( vol );
  cartoMsg( 0, "===", "testCreateVolume");
  
  return true;
}

bool testReadVolumeRef( const string & fname )
{
  Reader<VolumeRef<VoxelRGBA> > reader( fname );
  VolumeRef<VoxelRGBA> volref;
  reader >> volref;
  
  return true;
}
  

int main( int argc, const char** argv )
{
  try
  {
    //=== APPLICATION ==========================================================
    bool volumeBorders = false;
    bool createVolume = false;
    bool readVolumeRef = false;
    string ofname = "/tmp/volume.ima";
    string ifname = "";
    int sx = 1, sy = 1, sz = 1, st = 1;
    CartoApplication  app( argc, argv, "Various tests" );
    app.addOption( volumeBorders, "volumeBorders", "Views to an allocated"
                   "Volume and indexes.\n", true );
    app.addOption( createVolume, "createVolume", "Creates a volume. Depends on"
                   "-o -sx -sy -sz -st options.\n", true );
    app.addOption( readVolumeRef, "readVolumeRef", "Apply Reader directly to "
                   "a reference.\n", true );
    app.addOption( ifname, "-i", "In filename.\n", true );
    app.addOption( ofname, "-o", "Out filename.\n", true );
    app.addOption( sx, "-sx", "Size (comp x).\n", true );
    app.addOption( sy, "-sy", "Size (comp y).\n", true );
    app.addOption( sz, "-sz", "Size (comp z).\n", true );
    app.addOption( st, "-st", "Size (comp t).\n", true );
    app.alias( "-v", "--verbose" );
    app.alias( "-d", "--debugLevel" );
    app.initialize();
    
    //=== RUN ALGORITHM ========================================================
    if( volumeBorders )
      testVolumeBorders();
    if( createVolume )
      testCreateVolume( ofname, sx, sy, sz, st );
    if( readVolumeRef )
      testReadVolumeRef( ifname );
    
  aims::DecomposedAffineTransformation3d m;
  m.affine()(0, 0) = 2.;
  m.setRotation( .5, 1.5, 2. );
  cout << "DecomposedAffineTransformation3d:\n";
  cout << m << endl;

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

