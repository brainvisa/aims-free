
#include <aims/getopt/getopt2.h>
#include <aims/io/reader.h>
#include <graph/graph/graph.h>

using namespace aims;
using namespace carto;
using namespace std;

int main( int argc, const char** argv )
{
  Reader<Graph> reader;
  Writer<Graph> writer;
  AimsApplication	app( argc, argv, "Reads a graph, iterates over its "
      "nodes and relations" );

  app.addOption( reader, "-i", "input graph" );
  app.addOption( writer, "-o", "output graph", true );

  try
    {
      app.initialize();

      Graph	graph;
      reader.read( graph );
      cout << reader.fileName() << " read\n";

      cout << "graph vertices: " << graph.order() << endl;
      cout << "graph edges   : " << graph.edgesSize() << endl;
      cout << "graph syntax  : " << graph.getSyntax() << endl;

      Graph::iterator  iv, ev = graph.end();
      Vertex::iterator ie, ee;
      Vertex           *v;

      for( iv=graph.begin(); iv!=ev; ++iv )
      {
        v = *iv;
        cout << "vertex " << v << ", syntax: " << v->getSyntax()
            /* WARNING: for now Vertex::size() is its number of relations
              (synonym to edgesSize()). It will change in the future but
              at the moment, to get its properties size, use the following: */
            << ", properties: " << v->TypedObject<Dictionary>::size()
            << ", edges: " << v->edgesSize() << " ( ";
        for( ie=v->begin(), ee=v->end(); ie!=ee; ++ie )
          cout << *ie << " ";
        cout << ")" << endl;
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
      return 1;
    }
}


