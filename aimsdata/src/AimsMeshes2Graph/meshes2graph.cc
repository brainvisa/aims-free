#include <cstdlib>
#include <aims/getopt/getopt2.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/def/path.h>
#include <aims/graph/graphmanip.h>
#include <aims/mesh/surface.h>
#include <graph/graph/graph.h>
#include <math.h>

using namespace carto;
using namespace aims;
using namespace std;


int main( int argc, const char **argv )
{
  vector<string> filein;
  string fileout;
  string name;
  AimsApplication	app( argc, argv, "Makes a graph from a set of meshes" 
			     );

  app.addOption( fileout, "-o", "output data graph" );
  app.alias( "-output", "-o" );
  app.addOption( name, "-n", "name attribute given to the graph's nodes "
                 "(default=name of corresponding mesh file)", true );
  app.alias( "--name", "-n" );
  app.addOptionSeries( filein, "-i", "input meshes filenames" );
  app.alias( "-input", "-i" );

  try
    {
      app.initialize();

      Graph		g( "RoiArg" );

        // global graph attributes
      vector<float>	vs(3);
      vector<int>	bbmin(3), bbmax(3);
      bool		firstmesh = true;
      vs[0] = 1;  // voxel size
      vs[1] = 1;
      vs[2] = 1;
      g.setProperty( "voxel_size", vs );
      g.setProperty( "filename_base", string( "*" ) );

      // make nodes
      vector<string>::const_iterator	im, em = filein.end();
      for ( im=filein.begin(); im!=em; ++im )
        {
          rc_ptr<AimsSurfaceTriangle>	mesh( new AimsSurfaceTriangle );
          Reader<AimsSurfaceTriangle>	mr( *im );
          mr.read( *mesh );
          // create the node
          Vertex *v = g.addVertex( "roi" );
          // set its name attribute
          if ( name.empty() ) {
            v->setProperty( "name", *im );
          } else {
            v->setProperty( "name", name );
          }
          //  store the mesh in the vertex
          GraphManip::storeAims( g, v, "aims_Tmtktri", mesh );

          // scan extrama
          mesh->setMini();
          mesh->setMaxi();
          Point3df	m = mesh->minimum();
          if ( firstmesh || m[0] < bbmin[0] )
#if defined(__GNUC__) && defined (__sun__)
            bbmin[0] = (int) ::floor( m[0] );
#else
            bbmin[0] = (int) floor( m[0] );
#endif
          if ( firstmesh || m[1] < bbmin[1] )
#if defined(__GNUC__) && defined (__sun__)
            bbmin[1] = (int) ::floor( m[1] );
#else
            bbmin[1] = (int) floor( m[1] );
#endif
          if ( firstmesh || m[2] < bbmin[2] )
#if defined(__GNUC__) && defined (__sun__)
            bbmin[2] = (int) ::floor( m[2] );
#else
            bbmin[2] = (int) floor( m[2] );
#endif
          m = mesh->maximum();
          if ( firstmesh || m[0] > bbmax[0] )
#if defined(__GNUC__) && defined (__sun__)
            bbmax[0] = (int) ::ceil( m[0] );
#else
            bbmax[0] = (int) ceil( m[0] );
#endif
          if ( firstmesh || m[1] > bbmax[1] )
#if defined(__GNUC__) && defined (__sun__)
            bbmax[1] = (int) ::ceil( m[1] );
#else
            bbmax[1] = (int) ceil( m[1] );
#endif
          if ( firstmesh || m[2] > bbmax[2] )
#if defined(__GNUC__) && defined (__sun__)
            bbmax[2] = (int) ::ceil( m[2] );
#else
            bbmax[2] = (int) ceil( m[2] );
#endif
          firstmesh = false;
         }
      g.setProperty( "boundingbox_min", bbmin );
      g.setProperty( "boundingbox_max", bbmax );

      // write
      Writer<Graph>	gw( fileout );
      gw.write( g );

      return EXIT_SUCCESS;
    }
  catch( user_interruption &e )
    {
    }
  catch( exception & e )
    {
      cerr << e.what() << endl;
      return 1;
    }

  return EXIT_FAILURE;
}
