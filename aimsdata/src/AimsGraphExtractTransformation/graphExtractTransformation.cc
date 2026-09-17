#include <cstdlib>
#include <aims/getopt/getopt2.h>
#include <cartobase/config/paths.h>
#include <aims/transformation/affinetransformation3d.h>
#include <aims/graph/graphmanip.h>
#include <graph/graph/graph.h>
#include <graph/graph/greader.h>
#include <cartobase/object/sreader.h>
#include <cartobase/stream/fileutil.h>

using namespace aims;
using namespace carto;
using namespace std;


int main( int argc, const char **argv )
{
  string	filein, fileout;

  AimsApplication	app( argc, argv, 
                             "Extracts the Talairach transformation in a graph"
                             " file and write it as an Aims/Anatomist "
                             "transformation file" );

  app.addOption( filein, "-i", "input data graph" );
  app.alias( "-input", "-i" );
  app.addOption( fileout, "-o", "output transformation (default: "
                 "<input>TOtalairach.trm)", true );
  app.alias( "-output", "-o" );

  Graph		g( "ClusterArg" );
  SyntaxSet	syntax;

  try
    {
      app.initialize();

      if( fileout.empty() )
      {
        fileout = filein;
        string::size_type p1 = fileout.rfind( '.' );
        if( p1 != string::npos )
        {
          string::size_type p2 = fileout.rfind( '/' );
          if( p2 == string::npos || p2 < p1 )
            fileout.erase( p1, fileout.length() - p1 );
        }
        fileout += "TOtalairach.trm";
      }

      // syntax
      SyntaxReader	sr( Paths::findResourceFile(
                            "nomenclature/syntax/graph.stx", "aims" ) );
      sr >> syntax;

      // read
      GraphReader	grd( filein, syntax );
      grd >> g;

      // read transformation
      AffineTransformation3d m = GraphManip::talairach( g );

      Writer<AffineTransformation3d> w( fileout );
      w.write( m );
    }
  catch( user_interruption &e )
    {
    }
  catch( exception & e )
    {
      cerr << e.what() << endl;
      return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
