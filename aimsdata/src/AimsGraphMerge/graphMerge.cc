#include <cstdlib>
#include <aims/getopt/getopt2.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/graph/graphmanip.h>
#include <graph/graph/graph.h>

using namespace aims;
using namespace carto;
using namespace std;

int main( int argc, const char** argv )
{
  try
    {
      AimsApplication	app( argc, argv, 
			     "Merge 2 graphs " 
			     "(cortical folds graphs generally)");
      Reader<Graph>		rg1,rg2;
      Writer<Graph>		wg;
      string key;
      app.addOption( rg1, "-i", "input graph1" );
      app.alias( "-input1", "-i" );
      app.addOption( key, "-k", "matching key" );
      app.alias( "-key", "-k" );
      app.addOption( rg2, "-j", "input graph2" );
      app.alias( "-input2", "-j" );
      app.addOption( wg, "-o", "output (merge) graph [default=input]", 
		     true );
      app.alias( "-output", "-o" );
      app.initialize();

      Graph	*g = rg1.read();
      Graph	*h = rg2.read();
      Graph *m = GraphManip::mergeGraph(key,*g,*h,true );
      
      string	base( wg.fileName() );
      string::size_type	pos = base.rfind( '/' );
      if( pos != string::npos )
	base.erase( 0, pos+1 );
      pos = base.rfind( '.' );
      if( pos != string::npos )
	base.erase( pos, base.length() - pos );
      base += ".data";
      m->setProperty( "filename_base", base );

      if( wg.fileName().empty() )
	wg.setFileName( rg1.fileName() );
      wg.write( *m );

      return EXIT_SUCCESS;
    }

  catch( user_interruption &e )
    {
    }
  catch( exception & e )
    {
      cerr << e.what() << endl;
    }
  return EXIT_FAILURE;
}
