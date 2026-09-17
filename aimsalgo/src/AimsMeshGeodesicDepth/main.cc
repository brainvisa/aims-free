
#include <aims/distancemap/meshdistance.h>
#include <aims/data/data_g.h>
#include <aims/io/io_g.h>
#include <iomanip>
#include <aims/getopt/getopt2.h>
#include <aims/mesh/texture.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>

using namespace aims;
using namespace aims::meshdistance;
using namespace carto;
using namespace std;


int main( int argc, const char** argv )
{
  string meshfile, volumefile, outtexfile;
  float close = 10, erode = 2;

  //
  // Parser of options
  //
  AimsApplication app( argc, argv,
                       "Compute geodesic depth on a triangulation" );
  app.addOption( meshfile, "-i", "input mesh file" );
  app.alias( "--input", "-i" );
  app.addOption( outtexfile, "-o", "output depth texture file" );
  app.alias( "--output", "-o" );
  app.addOption( volumefile, "-v",
                 "brain hull definition (binary image relative to the "
                 "triangualtion)" );
  app.alias( "--volume", "-v" );
  app.addOption( close, "-c", "radius of the closing [default = 10]", true );
  app.alias( "--close", "-c" );
  app.addOption( erode, "-e", "radius of the erosion [default = 5]", true );
  app.alias( "--erosion", "-e" );

  try
  {
    app.initialize();

    //
    // read triangulation
    //
    cout << "reading triangulation   : " << flush;
    AimsSurfaceTriangle surface;
    Reader<AimsSurfaceTriangle> triR( meshfile );
    triR >> surface;
    cout << "done" << endl;

    //
    // read image of brain
    //
    cout << "reading brain  : " << flush;
    AimsData < short > data;
    Reader< AimsData <short> > dataR( volumefile );
    //dataR >> data ;
    dataR.read(data,1);
    data.fillBorder(0);
    cout << "done" << endl;


    cout << "mesh vertices : " << surface[0].vertex().size() << endl;
    cout << "mesh polygons : " << surface[0].polygon().size() << endl;


    TimeTexture<float>	outTex;

    outTex[0] = GeodesicDepth(surface[0],data,close,erode);

    // Writting the textures
    cout << "writing texture : " << flush;
    Writer<TimeTexture<float> >	texW( outtexfile );
    texW <<  outTex ;

    cout << "done" << endl;
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



