
#include <cstdlib>
#include <aims/getopt/getopt2.h>
#include <aims/mesh/surfacegen.h>
#include <soma-io/reader/pythonreader.h>

using namespace aims;
using namespace carto;
using namespace std;

int main( int argc, const char **argv )
{
  string			filein;
  Writer<AimsSurfaceTriangle>	meshout;

  AimsApplication	app( argc, argv, "Generates a common-shaped mesh" );
  ostringstream	sg;
  sg << "input parameter file (.minf-like)\nPossible types and parameters:\n";
  SurfaceGenerator::printDescription( sg );
  SurfaceGenerator::printDescription_wireframe( sg );
  app.addOption( filein, "-i", sg.str() );
  app.alias( "--input", "-i" );
  app.addOption( meshout, "-o", "output mesh" );
  app.alias( "--output", "-o" );

  try
    {
      app.initialize();

      PythonReader	pr( filein );
      AttributedObject	params( "AimsMeshGenerate" );
      pr.read( params );

      try
      {
        AimsSurfaceTriangle	*mesh = SurfaceGenerator::generate( params );

        meshout.write( *mesh );
      }
      catch( runtime_error & )
      {
        // try wireframe objects
        AimsTimeSurface<2, Void> *mesh = SurfaceGenerator::generate_wireframe(
          params );
        Writer<AimsTimeSurface<2, Void> > w( meshout.fileName() );
        w.write( *mesh );
      }
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


