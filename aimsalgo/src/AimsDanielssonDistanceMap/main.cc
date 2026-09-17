#include <aims/data/data.h>
#include <aims/distancemap/danielsson.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/getopt/getopt2.h>

using namespace aims;
using namespace carto;
using namespace std;


int main( int argc, const char **argv )
{
  Reader<AimsData<short> > reader;
  Writer<AimsData<float> > writer;

  string c_side;

  AimsApplication app( argc, argv, "Danielsson's distance map on a volume" );
  app.addOption( reader, "-i", "source volume" );
  app.addOption( writer, "-o", "destination volume" );
  app.addOption( c_side, "-s", "OUTSIDE, INSIDE, SIGNED [default=SIGNED]",
                 true );
  app.alias( "--input", "-i" );
  app.alias( "--output", "-o" );
  app.alias( "--side", "-s" );

  try
  {
    app.initialize();

    string side = "SIGNED";
    if ( !c_side.empty() )
    {
      side = c_side;
      ASSERT( side == "INSIDE" || side == "OUTSIDE" || side == "SIGNED" );
    }

    AimsData<short> vol;

    if( !reader.read( vol, 1 ) )
      return EXIT_FAILURE;

    AimsData<float> map;

    if ( side == "INSIDE" )
      map = AimsDanielssonDistanceMap( vol, AIMS_DANIELSSON_INSIDE );
    else if ( side == "OUTSIDE" )
      map = AimsDanielssonDistanceMap( vol, AIMS_DANIELSSON_OUTSIDE );
    else
      map = AimsSignedDanielssonDistanceMap( vol );

    if( ! writer.write( map ) )
    {
      throw logic_error( "Internal error: write failed" );
    }

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
