#include <cstdlib>
#include <aims/getopt/getopt2.h>
#include <aims/mesh/tex2graph.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/io/aimsGraphW.h>
#include <cartobase/config/paths.h>
#include <graph/graph/graph.h>
#include <graph/graph/gwriter.h>
#include <cartobase/object/sreader.h>
#include <cartobase/stream/fileutil.h>
#include <fstream>

using namespace aims;
using namespace carto;
using namespace std;


int main( int argc, char **argv )
{
  string			fileout;
  string			conversionfile;
  unsigned			time = 0;
  Reader< TimeTexture<short> >	texR;
  Reader<AimsSurfaceTriangle >  triR;
  bool reverseFlag = false;

  AimsApplication	app( argc, (const char**) argv, "Makes a roi graph from "
			     "a mesh and a texture of labels" );

  app.addOption( texR, "-t", "input label (short) texture" );
  app.alias( "-texture", "-t" );
  app.addOption( triR, "-m", "input mesh" );
  app.alias( "-mesh", "-m" );
  app.addOption( fileout, "-o", "output graph" );
  app.alias( "-output", "-o" );
  app.addOption( time, "-T", "make graph from time T of input texture", true );
  app.alias( "-Time", "-T" );
  app.alias( "-time", "-T" );
  app.addOption( conversionfile, "-c", "conversion table  (short label to  name)", true );
  app.alias( "-conversion", "-c" );
  app.addOption( reverseFlag, "--reverse", "flip conversion table (name to short label)", true );

  try
    {
      app.initialize();

      cout << "meshfilein  : " << triR.fileName() << endl;
      cout << "texturefilein  : " << texR.fileName() << endl;
      cout << "fileout : " << fileout << endl;
      cout << "Time : " << time << endl;

      //read input mesh
      cout << "reading mesh..." << flush;
      AimsSurfaceTriangle surface;
      triR >> surface;
      cout << "done" << endl;

      //read input texture
      cout << "reading texture..." << flush;
      TimeTexture<short>	inpTex;
      texR >> inpTex;
      cout << "done" << endl;


      // read translation file
      ifstream 	conversion(conversionfile.c_str());
      map<short,string>	label2name;
      
      if (!conversionfile.empty())
        {     
          cout << "reading conversion file..." << flush;
          short label;
          string name;
          while ( conversion && !conversion.eof() )
            {
              if ( reverseFlag )
                conversion >> name >> label;

              else
                conversion >> label >> name;
              if (!conversion.eof() && (!name.empty() || name != "unknown" ) )
                label2name[label] = name;;
            }
          cout << "done" << endl;

        }
      cout << "mesh vertices : " << surface[0].vertex().size() << endl;
      cout << "mesh polygons : " << surface[0].polygon().size() << endl;
      cout << "texture dim   : " << inpTex[0].nItem() << endl;

      Graph		g("RoiArg");
      Tex2Graph<short>	t2g;
 
      if (!conversionfile.empty())
        {
          cout << "Bulding graph and convert (short) label to (string) name."  << endl;
          t2g.makeGraph(g,surface,inpTex[time],label2name);
        }
      else
        {
          cout << "Bulding graph."  << endl;
          t2g.makeGraph(g,surface,inpTex[time]);
        }

      string	base = FileUtil::basename( fileout );
      string::size_type	pos = base.rfind( '.' );
      if( pos != string::npos )
	base.erase( pos, base.length() - pos );
      base += ".data";
      g.setProperty( "filename_base", base );

      SyntaxSet	syntax;
      SyntaxReader	sr( Paths::findResourceFile(
        "nomenclature/syntax/graph.stx", "aims" ) );
      sr >> syntax;

      AimsGraphWriter agw(fileout);
      agw.writeElements(g,AimsGraphWriter::Global);

      GraphWriter	gw(fileout, syntax);
      gw << g;

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


