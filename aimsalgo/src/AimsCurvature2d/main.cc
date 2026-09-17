
#include <cstdlib>
#include <cartodata/volume/volume.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/getopt/getopt2.h>
#include <aims/math/math_g.h>

using namespace aims;
using namespace carto;
using namespace std;


int main( int argc, const char **argv )
{
  Reader<Volume<short> >	reader;
  Writer<Volume<float> >	writer;

  AimsApplication	app( argc, argv, "2D curvature of an intensity image " 
                             "f(x,y) = I" );
  app.addOption( reader, "-i", "source matrix" );
  app.addOption( writer, "-o", "destination float matrix" );
  app.alias( "--input", "-i" );
  app.alias( "--output", "-o" );

  try
    {
      app.initialize();

      VolumeRef<short> mat;
      mat = reader.read();

      VolumeRef<float> curvature;
      curvature = AimsIsoIntensityCurvature2D( mat );

      writer.write( *curvature );

      return EXIT_SUCCESS;
    }
  catch( user_interruption & )
    {
    }
  catch( exception & e )
    {
      cerr << e.what() << endl;
    }
  return EXIT_FAILURE;
}
