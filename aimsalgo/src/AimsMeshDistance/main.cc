
#include <aims/distancemap/meshdistance.h>
#include <aims/data/data_g.h>
#include <aims/io/io_g.h>
#include <aims/math/math_g.h>
#include <aims/getopt/getopt2.h>
#include <aims/vector/vector.h>
#include <aims/mesh/texture.h>
#include <iomanip>

using namespace aims;
using namespace carto;
using namespace aims::meshdistance;
using namespace std;


int main( int argc, const char** argv )
{
  string meshfile, intexfile, outtexfile;

  //
  // Parser of options
  //
  AimsApplication app( argc, argv, "Compute geodesic distance to object" );
  app.addOption( meshfile, "-i", "input mesh file" );
  app.alias( "--input", "-i" );
  app.addOption( outtexfile, "-o", "output distance texture file" );
  app.alias( "--output", "-o" );
  app.addOption( intexfile, "-t", "object definition texture" );
  app.alias( "--texture", "-t" );

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
    // read input texture
    //
    cout << "reading texture   : " << flush;
    TimeTexture<short>	inpTex;
    Reader<TimeTexture<short> > texR( intexfile );
    texR >> inpTex;
    cout << "done" << endl;

    cout << "mesh vertices : " << surface[0].vertex().size() << endl;
    cout << "mesh polygons : " << surface[0].polygon().size() << endl;
    cout << "texture dim   : " << inpTex[0].nItem() << endl;

    TimeTexture<float>	outTex;
    outTex[0] = MeshDistance( surface[0], inpTex[0], false );

    cout << "writing texture : " << flush;
    Writer<TimeTexture<float> >	texW( outtexfile );
    texW.write( outTex );
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


