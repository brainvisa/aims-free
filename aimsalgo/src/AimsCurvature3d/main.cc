
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
  bool				gaussian = false;

  AimsApplication	app( argc, argv, "3D curvature of an intensity image "
                             "f(x,y) = I" );
  app.addOption( reader, "-i", "source matrix" );
  app.addOption( writer, "-o", "destination float matrix" );
  app.addOption( gaussian, "-g", "use gaussian curvature [default=mean]", 
                 true );
  app.alias( "--input", "-i" );
  app.alias( "--output", "-o" );
  app.alias( "--gaussian", "-g" );

  try
    {
      app.initialize();

      VolumeRef<short> vol;
      vol = reader.read();

      VolumeRef<float> curvature;
      curvature 
        = AimsIsoIntensityCurvature3D( vol,
                                       ( gaussian ? AIMS_GAUSSIAN_CURVATURE 
                                         : AIMS_MEAN_CURVATURE ) );

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


