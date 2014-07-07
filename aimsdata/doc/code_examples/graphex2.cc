
#include <aims/getopt/getopt2.h>
#include <aims/io/reader.h>
#include <aims/graph/graphmanip.h>
#include <aims/roi/roiIterator.h>
#include <graph/graph/graph.h>

using namespace aims;
using namespace carto;
using namespace std;

int main( int argc, const char** argv )
{
  Reader<Graph> reader;
  AimsApplication	app( argc, argv, "Reads a graph, iterates over ROIs " 
                             "and voxels in ROIs" );

  app.addOption( reader, "-i", "input graph" );

  try
    {
      app.initialize();

      Graph	graph;
      reader.read( graph );
      cout << reader.fileName() << " read\n";

      cout << "graph vertices: " << graph.order() << endl;
      cout << "graph edges   : " << graph.edgesSize() << endl;
      cout << "graph syntax  : " << graph.getSyntax() << endl;

      RoiIteratorOf<Graph>	roiit( graph );
      // iterate on regions
      for( ; roiit.isValid(); roiit.next() )
        {
          cout << "ROI: " << roiit.regionName() << endl;
          rc_ptr<MaskIterator>	maskit = roiit.maskIterator();
          // iterate on voxels
          for( ; maskit->isValid(); maskit->next() )
            {
              cout <<  maskit->value() << "  ";
            }
          cout << endl;
        }
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


