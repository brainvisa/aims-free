
#include <aims/connectivity/meshcc.h>
#include <aims/data/data_g.h>
#include <aims/io/io_g.h>
#include <aims/math/math_g.h>
#include <aims/getopt/getopt2.h>
#include <aims/vector/vector.h>
#include <aims/mesh/texture.h>
#include <iomanip>

using namespace aims;
using namespace carto;
using namespace std;


int main( int argc, const char** argv )
{
  string meshfile, intexfile, outtexfile;
  int modeFlag = 0;
  float threshold = 0;

  //
  // Parser of options
  //
  AimsApplication app( argc, argv, "Connected Components in a label texture" );
  app.addOption( meshfile, "-i", "input mesh file" );
  app.alias( "--input", "-i" );
  app.addOption( intexfile, "-t", "object definition texture" );
  app.alias( "--texture", "-t" );
  app.addOption( outtexfile, "-o",
                 "output connected components texture file" );
  app.alias( "--output", "-o" );
  app.addOption( threshold, "-T",
                 "threshold applied to binarize the input texture [0]", true );
  app.alias( "--Threshold", "-T" );
  app.addOption( modeFlag, "-m",
                 "mode: 1: lesser than threshold, 0: greater [default=0]",
                 true );

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
    TimeTexture<float>	inpTex;
    Reader<Texture1d> texR( intexfile );
    texR >> inpTex;
    cout << "done" << endl;

    cout << "mesh vertices : " << surface[0].vertex().size() << endl;
    cout << "mesh polygons : " << surface[0].polygon().size() << endl;
    cout << "texture dim   : " << inpTex[0].nItem() << endl;

    TimeTexture<float>	outTex;
    cout << "Thresholding: " << threshold << endl;

    outTex[0] = AimsMeshLabelConnectedComponent( surface[0], inpTex[0],
                                                 threshold, modeFlag);

    cout << "writing texture : " << flush;
    Writer<Texture1d>	texW( outtexfile );
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


