
#include <cstdlib>
#include <aims/getopt/getopt2.h>
#include <aims/getopt/getoptProcess.h>
#include <aims/io/reader.h>
#include <aims/mesh/surfaceOperation.h>

using namespace aims;
using namespace carto;
using namespace std;

int main( int argc, const char **argv )
{
  Reader<AimsSurfaceTriangle>	filein;
  Writer<AimsSurfaceTriangle>	cutout, planeout;
  Writer<AimsTimeSurface<2, Void> >	borderout;
  float				a, b, c, d;

  AimsApplication	app( argc, argv, "Cuts a mesh by a plane" );
  app.addOption( filein, "-i", "input mesh" );
  app.alias( "--input", "-i" );
  app.addOption( cutout, "-o", "output cutout mesh" );
  app.alias( "--output", "-o" );
  app.addOption( a, "-a", "plane equation coef: a" );
  app.addOption( b, "-b", "plane equation coef: b" );
  app.addOption( c, "-c", "plane equation coef: c" );
  app.addOption( d, "-d", "plane equation coef: d" );
  app.addOption( planeout, "-p", "output plane intersection mesh", true );
  app.alias( "--plane", "-p" );
  app.addOption( borderout, "--border", "output border polygon", true );

  try
    {
      app.initialize();

      AimsSurfaceTriangle	inm, cutm, plm;
      AimsTimeSurface<2, Void>	border;
      filein.read( inm );

      Point4df	plane( a, b, c, d );

      SurfaceManip::cutMesh( inm, plane, cutm, plm, border );

      cutout.write( cutm );
      if( !planeout.fileName().empty() )
	planeout.write( plm );
      if( !borderout.fileName().empty() )
	borderout.write( border );
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


