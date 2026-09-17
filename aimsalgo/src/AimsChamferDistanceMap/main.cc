
#include <cstdlib>
#include <aims/data/data_g.h>
#include <aims/distancemap/distancemap_g.h>
#include <aims/io/io_g.h>
#include <aims/getopt/getopt2.h>

using namespace aims;
using namespace carto;
using namespace std;


int main( int argc, const char **argv )
{
  Reader<AimsData<short> > reader;
  Writer<AimsData<float> > writer;
  int xmask = 3,ymask = 3,zmask = 3;
  float factor = 50;
  string side = "SIGNED";

  AimsApplication	app( argc, argv, "Chamfer distance map on a volume" );

  app.addOption( reader, "-i", "source volume" );
  app.addOption( writer, "-o", "destination volume" );
  app.addOption( xmask, "-x", "X size of the distance mask [default=3]", 
                 true );
  app.addOption( ymask, "-y", "Y size of the distance mask [default=3]", 
                 true );
  app.addOption( zmask, "-z", "Z size of the distance mask [default=3]", 
                 true );
  app.addOption( factor, "-f", "chamfer multiplication factor [default=50]", 
                 true );
  app.addOption( side, "-s", "OUTSIDE, INSIDE, SIGNED [default=SIGNED]", 
                 true );

  app.alias( "--input", "-i" );
  app.alias( "--output", "-o" );
  app.alias( "--xmask", "-x" );
  app.alias( "--ymask", "-y" );
  app.alias( "--zmask", "-z" );
  app.alias( "--factor", "-f" );
  app.alias( "--side", "-s" );

  try
    {
      app.initialize();

      ASSERT( side == "INSIDE" || side == "OUTSIDE" || side == "SIGNED" );

      AimsData<short> vol;
 
      reader.read( vol, 1 );
 
      AimsData<float> map;

      if ( side == "INSIDE" )
        map = AimsFloatChamferDistanceMap( vol, AIMS_CHAMFER_INSIDE,
                                           xmask, ymask, zmask, factor );
      else if ( side == "OUTSIDE" )
        map = AimsFloatChamferDistanceMap( vol, AIMS_CHAMFER_OUTSIDE,
                                           xmask, ymask, zmask, factor );
      else
        map = AimsFloatSignedChamferDistanceMap( vol, xmask, ymask, zmask, 
                                                 factor );

      writer.write( map );

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
