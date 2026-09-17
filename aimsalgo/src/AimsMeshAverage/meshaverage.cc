
#include <aims/io/io_g.h>
#include <aims/getopt/getopt2.h>
#include <aims/mesh/texture.h>
#include <aims/distancemap/meshvoronoi.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>

using namespace aims;
using namespace carto;
using namespace std;


int main( int argc, const char** argv )
{
  string outtexfile, meshfile, outmesh;

  //
  // Parser of options
  //
  AimsApplication app( argc, argv, "Average a time series mesh.\n"
    "WARNING: Be sure that the nodes at different time serie correspond to "
    "the same point..." );
  app.addOption( meshfile, "-m", "input time series mesh" );
  app.alias( "--mesh", "-m" );
  app.addOption( outmesh, "-o", "output mean mesh" );
  app.alias( "--output", "-o" );
  app.addOption( outtexfile, "-v", "variability output texture", true );
  app.alias( "--variability", "-v" );

  try
  {
    app.initialize();

    //
    // read triangulation
    //
    cout << "reading triangulation   : " << flush;
    AimsSurfaceTriangle surface,meansurf;
    Reader<AimsSurfaceTriangle> triR( meshfile );
    triR >> surface;
    cout << "done" << endl;

    unsigned            j, i, nnodes = surface.vertex().size();
    unsigned            ntimes = surface.size();
    TimeTexture<float>  outTex(1,nnodes);
    float m;

    Point3df       			vert ;

    cout << "Node average..." << flush;
    for (j=0; j<nnodes;++j)
      {
        vert[0] = 0;
        vert[1] = 0;
        vert[2] = 0;
        for (i = 0; i < ntimes; ++i)
          {
            vert[0] += surface[i].vertex()[j][0];
            vert[1] += surface[i].vertex()[j][1];
            vert[2] += surface[i].vertex()[j][2];
          }

        vert[0] /= ntimes;
        vert[1] /= ntimes;
        vert[2] /= ntimes;
        m = 0;
        for (i = 0; i < ntimes; ++i)
          m += (surface[i].vertex()[j][0] - vert[0])*(surface[i].vertex()[j][0] - vert[0])
            + (surface[i].vertex()[j][1] - vert[1])*(surface[i].vertex()[j][1] - vert[1])
            + (surface[i].vertex()[j][2] - vert[2])*(surface[i].vertex()[j][2] - vert[2]);

        m = sqrt(m) / ntimes;
        meansurf[0].vertex().push_back(vert);
        outTex[0].item(j) = m;

      }
    cout << "done\n";


    cout << "Build connectivity" << endl;
    meansurf[0].polygon() = surface[0].polygon();

    if( !outtexfile.empty() )
    {
      cout << "writing texture : " << flush;
      Writer<Texture1d>	texW( outtexfile );
      texW.write( outTex );
      cout << "done " << endl;
    }

    cout << "Write the mesh..." << flush;
    Writer<AimsSurfaceTriangle >	meshW( outmesh);
    meshW << meansurf;
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


