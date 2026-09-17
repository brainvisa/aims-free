
#include <aims/distancemap/meshvoronoi.h>
#include <aims/io/io_g.h>
#include <aims/getopt/getopt2.h>
#include <aims/mesh/texture.h>
#include <float.h>
#include <assert.h>



using namespace aims;
using namespace aims::meshdistance;
using namespace carto;
using namespace std;
 
typedef float float3[3];


int main( int argc, const char** argv )
{
  float dist = FLT_MAX;
  bool connexity = false;
  short background = 0 ;
  short forbidden = -1;
  bool verbose = false;
  int T = 0;
  Reader<AimsSurfaceTriangle> triR;
  Writer<TimeTexture<short> > texW;
  Reader<TimeTexture<short> > texR;

  AimsApplication app( argc, argv,
                       "Compute the Voronoi diagram of the input (short int) "
                       "seed texture" );
  app.addOption( triR, "-m", "input_mesh" );
  app.addOption( texW, "-o", "output voronoi diagram texture" );
  app.addOption( texR, "-t", "seeds texture definition (short int)" );
  app.addOption( T, "-T", "time step <default = 0>", true );
  app.addOption( background, "-b", "background value <default = 0>", true );
  app.addOption( forbidden, "-f", "value out of the domain <default = -1>",
                 true );
  app.addOption( dist, "-s", "step size <default = infinity>", true );
  app.addOption( connexity, "-c",
                 "connectivity or geodesic euclidean distance "
                 "<default: euclidean>", true );
  app.addOption( verbose, "--verbose", "<default = 0>", true );
  app.alias( "--mesh", "-m" );
  app.alias( "--output", "-o" );
  app.alias( "--texture", "-t" );
  app.alias( "--Time", "-T" );
  app.alias( "--background", "-b" );
  app.alias( "--forbidden", "-f" );
  app.alias( "--step", "-s" );
  app.alias( "--connexity", "-c" );
  app.alias( "--connectivity", "-c" );

  try
  {
    app.initialize();

    //
    // read triangulation
    //
    if ( verbose )
    {
    cout << "reading white triangulation   : " << flush;
    }
    AimsSurfaceTriangle surface;
    triR >> surface;
    if ( verbose )
    {
    cout << "done" << endl;
    }
    //
    // read input texture
    //
    if ( verbose )
    {
    cout << "reading texture   : " << flush;
    }
    TimeTexture<short>    seed, voronoi;
    texR >> seed;
    if ( verbose )
    {
    cout << "done" << endl;

    cout << "mesh vertices : " << surface[0].vertex().size() << endl;
    cout << "mesh polygons : " << surface[0].polygon().size() << endl;
    cout << "Object texture dim   : " << seed[0].nItem() << endl;
    cout << "Background label: " << background << " Forbidden label : "
      << forbidden << endl;
    }
    if ( ! (  seed[0].nItem() ==  surface[0].vertex().size() ) )
      {
        cerr << "The triangulation and the textures must correspond to the same object \n";
        assert( 0 );
      }

    if ( verbose )
    {
    cout << "Compute the voronoi diagram\n";
    }
    voronoi[0] = MeshVoronoiT<short>( surface[0], seed[(unsigned) T],
                                      background ,forbidden, dist,
                                      connexity, true );
    if ( verbose )
    {
    cout << "writing texture : " << flush;
    }
    texW <<  voronoi ;
    if ( verbose )
    {
    cout << "done" << endl;
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

