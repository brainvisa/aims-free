
#include <aims/getopt/getopt2.h>
#include <aims/io/reader.h>
#include <aims/graph/graphmanip.h>
#include <graph/graph/graph.h>

using namespace aims;
using namespace carto;
using namespace std;

int main( int argc, const char** argv )
{
  Reader<Graph> reader;
  AimsApplication	app( argc, argv, "Reads a graph, finds AIMS objects "
                             "in it" );

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

      rc_ptr<GraphElementTable> get;
      graph.getProperty( "aims_objects_table", get );
      if( !get )
        {
          cout << "no AIMS objects in this graph" << endl;
          return EXIT_SUCCESS;
        }

      Graph::iterator				iv, ev = graph.end();
      Vertex::iterator				ie, ee;
      Vertex					*v;
      GraphElementTable::iterator		iget, eget = get->end();
      map<string,GraphElementCode>::iterator	igec, egec;

      for( iv=graph.begin(); iv!=ev; ++iv )
        {
          v = *iv;
          iget = get->find( v->getSyntax() );
          if( iget != eget )
            {
              map<string, GraphElementCode> & mgec = iget->second;
              for( igec=mgec.begin(), egec=mgec.end(); igec!=egec; ++igec )
                {
                  GraphElementCode	& gec = igec->second;
                  if( ( gec.storageType != GraphElementCode::GlobalPacked 
                        && v->hasAttribute( gec.attribute ) )
                      || ( gec.storageType == GraphElementCode::GlobalPacked 
                           && v->hasAttribute( gec.global_index_attribute ) ) )
                    {
                      cout << "vertex " << v << " (" << v->getSyntax() 
                           << ") has in " << gec.attribute 
                           << " an object of type " << gec.objectType 
                           << " / " << gec.dataType;
                      switch( gec.storageType )
                        {
                        case GraphElementCode::Local:
                          cout << ", local storage, filename in prop " 
                               << gec.local_file_attribute;
                          break;
                        case GraphElementCode::Global:
                          cout << ", global storage, filename: " 
                               << gec.global_filename 
                               << ", referenced in graph prop " 
                               << gec.global_attribute 
                               << ", index prop in vertex: " 
                               << gec.global_index_attribute;
                          break;
                        case GraphElementCode::GlobalPacked:
                          cout << ", globalPacked storage, filename: " 
                               << gec.global_filename << ", stored in graph "
                               << "prop " << gec.global_attribute 
                               << ", index prop in vertex: " 
                               << gec.global_index_attribute;
                          break;
                        }
                      cout << endl;
                    }
                  }
            }
        }
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


