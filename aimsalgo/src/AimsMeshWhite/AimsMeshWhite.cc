
#include <cstdlib>
#include <aims/data/data.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/getopt/getopt2.h>
#include <aims/utility/utility_g.h>
#include <aims/mesh/surface.h>
#include <aims/mesh/mesher.h>

using namespace aims;
using namespace carto;
using namespace std;

#define NLABEL 32767

int main( int argc, const char* argv[] )
{
  string fileIn, fileOut;
  float deciMaxClearance = 3.0;
  float	deciMaxError = 10.0;
  int minFacetNumber = 50;
  bool asciiFlag = false;

  AimsApplication app( argc, argv,
    "Computes the brain surface from a segmented image as a mesh" );
  app.addOption( fileIn, "-i",
    "input short volume label (level 0 is assumed to be the background)" );
  app.alias( "--input", "-i" );
  app.addOption( fileOut, "-o", "output mesh filename "
    "[default=input + mesh extension]", true );
  app.alias( "--output", "-o" );
  app.addOption( deciMaxClearance, "--deciMaxClearance",
    "maximum clearance expected in the resulting mesh in the decimation "
    "process, in mm [default= 5 mm]", true );
  app.addOption( deciMaxError, "--deciMaxError",
    "maximum expected error distance from the original data in the decimation "
    "process, in mm [default = 3 mm]", true );
  app.addOption( minFacetNumber, "--minFacetNumber",
    "minimum number of facets to allow decimation [default=50]", true );
  app.addOption( asciiFlag, "--ascii",
    "write mesh file in ASCII mode if supported by the format "
    "[default=binary]", true );

  try
  {
    app.initialize();

    ASSERT( deciMaxClearance >= 0.0 );
    ASSERT( deciMaxError >= 0.0 );

    string	fout;
    if( fileOut.empty() )
    {
      fout = fileIn;
      string::size_type	pos = fout.rfind( '.' );
      string::size_type	pos2 = fout.rfind( '/' );
      if( pos != string::npos && ( pos2 == string::npos || pos2 < pos ) )
        fout.erase( pos, fout.length() - pos );
    }
    else
      fout = fileOut;

    //
    // load label volume
    //
    cout << "reading image      : " << flush;
    AimsData<short> data;
    Reader<AimsData<short> > dataR( fileIn );
    dataR.read( data, 1 );
    cout << "done" << endl;

    cout << "resolutions        : (";
    cout << data.sizeX() << " mm,";
    cout << data.sizeY() << " mm,";
    cout << data.sizeZ() << " mm)" << endl;
    data.fillBorder(-1);

    Mesher mesher;
    mesher.setDecimation( 100.0, deciMaxClearance, deciMaxError, 180.0 );
    mesher.setMinFacetNumber( minFacetNumber );
    AimsSurfaceTriangle surface;
    mesher.getWhite( data, surface );

    // compute the surface mesh
    cout << "surface            : " << flush
        << mesher.surfaceOfInterface( surface ) << " mm2" << endl;

    // save the triangulation
    Writer<AimsSurfaceTriangle> surfaceW( fout );
    cout << "saving mesh " << fout << "...\n";
    surfaceW.write( surface, asciiFlag );
    cout << "done\n";
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
