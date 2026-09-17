#include <cstdlib>
#include <aims/mesh/surfaceOperation.h>
#include <aims/getopt/getopt2.h>
#include <aims/io/reader.h>

using namespace aims;
using namespace carto;
using namespace std;

int main( int argc, const char** argv )
{
  Reader<AimsSurfaceTriangle>	filein;

  AimsApplication	app( argc, argv, "Prints a mesh surface area" );
  app.addOption( filein, "-i", "input mesh" );
  app.alias( "--input", "-i" );

  try
    {
      app.initialize();

      AimsSurfaceTriangle	mesh;

      filein.read( mesh );

      cout << "Area: " << SurfaceManip::meshArea( mesh ) << " mm2" << endl;
      cout << "Volume: " << SurfaceManip::meshVolume( mesh ) << " mm3" << endl;
    }
  catch( user_interruption & )
    {
    }
  catch( exception & e )
    {
      cerr << e.what() << endl;
      return( EXIT_FAILURE );
    }

  return EXIT_SUCCESS;
}


