
#include <aims/getopt/getopt2.h>
#include <aims/io/reader.h>
#include <aims/graph/graphmanip.h>
#include <graph/graph/graph.h>

using namespace aims;
using namespace carto;
using namespace std;

int main( int argc, const char** argv )
{
  Reader<Graph>			reader;
  Writer<Graph>			writer;
  Reader<AimsSurfaceTriangle>	meshreader;
  string			labeltoset;
  AimsApplication	app( argc, argv, "Reads a graph, finds some selected " 
                             "AIMS objects in it, fills other ones" );

  app.addOption( reader, "-i", "input graph" );
  app.addOption( writer, "-o", "output graph", true );
  app.addOption( meshreader, "-m", "mesh to store in selected node", true );
  app.addOption( labeltoset, "-n", "name (label) of the selected node to put " 
                 "mesh in (if any)", true );

  try
    {
      app.initialize();

      // read graph
      Graph	graph;
      reader.read( graph );
      cout << reader.fileName() << " read\n";

      // read mesh if any, in a rc_ptr
      rc_ptr<AimsSurfaceTriangle>	mesh;
      if( !meshreader.fileName().empty() )
        mesh.reset( meshreader.read() );

      cout << "graph vertices: " << graph.order() << endl;
      cout << "graph edges   : " << graph.edgesSize() << endl;
      cout << "graph syntax  : " << graph.getSyntax() << endl;

      if( mesh )
        {
          cout << "mesh vertices : " << mesh->vertex().size() << endl;
          cout << "mesh polygons : " << mesh->polygon().size() << endl;
        }

      Graph::iterator				iv, ev = graph.end();
      Vertex					*v;
      string					label;

      for( iv=graph.begin(); iv!=ev; ++iv )
        {
          v = *iv;
          rc_ptr<BucketMap<Void> >	bck;
          label.clear();
          v->getProperty( "name", label );
          if( v->getProperty( "aims_roi", bck ) )
            {
              cout << "vertex " << v << " (" << v->getSyntax() << ")";
              if( !label.empty() )
                cout << ", named " << label;
              cout << " has a bucket of " << (*bck)[0].size() << " voxels in " 
                   << "property aims_roi" << endl;
            }
          if( label == labeltoset && mesh )
            {
              GraphManip::storeAims( graph, v, "aims_Tmtktri", mesh );
              cout << "mesh stored in vertex " << v << " (" << v->getSyntax() 
                   << "), named " << label << " in property aims_Tmtktri" 
                   << endl;
            }
        }
      if( !writer.fileName().empty() )
        writer.write( graph );
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


