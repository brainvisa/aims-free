
#include <cstdlib>
#include <aims/distancemap/meshparcellation.h>
#include <aims/distancemap/meshdistance.h>
#include <aims/distancemap/projection.h>
#include <aims/data/data_g.h>
#include <aims/io/io_g.h>
#include <aims/math/math_g.h>
#include <iomanip>
#include <aims/vector/vector.h>
#include <aims/mesh/texture.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/connectivity/meshcc.h>
#include <fstream>
#include <float.h>
#include <aims/def/path.h>
#include <cartobase/object/sreader.h>
#include <cartobase/plugin/plugin.h>
#include <aims/getopt/getopt2.h>

using namespace aims;
using namespace aims::meshdistance;
using namespace carto;
using namespace std;


int main( int argc, char** argv )
{
  
  unsigned	time=0;
  short label_vol = 100;

  string fileout,volfile;
  Reader< TimeTexture<short> >	texR;
  Reader<AimsSurfaceTriangle >  triR;

  AimsApplication	app( argc, (const char**) argv, "Project a mesh parcellation "
			     "to a volume" );

  PluginLoader::load();

  //
  // Parser of options
  //
  app.addOption( texR, "-t", "input label (short) texture" );
  app.alias( "-texture", "-t" );
  app.addOption( triR, "-m", "input mesh" );
  app.alias( "-mesh", "-m" );
  app.addOption( fileout, "-o", "output parcellation volume" );
  app.alias( "-output", "-o" );
  app.addOption( volfile, "-v", "input volume file" );
  app.alias( "-volume", "-v" );
  app.addOption( label_vol, "-l", "label of the object in the volume (default = 100)", true );
  app.alias( "-label", "-l" );
  app.addOption( time, "-T", "make graph from time T of input texture", true );
  app.alias( "-Time", "-T" );
  app.alias( "-time", "-T" );

  try
    {
      app.initialize();
      //
      // read triangulation
      //
      cout << "reading white triangulation   : " << flush;
      AimsSurfaceTriangle surface;
      Reader<AimsSurfaceTriangle> triread( triR );
      triread >> surface;
      cout << "done" << endl;
 
      //
      // read input texture
      //
      cout << "reading texture   : " << flush;
      TimeTexture<short>	inpTex; 
      Reader<TimeTexture<short> > texread( texR );
      texread >> inpTex; 
      cout << "done" << endl;

      cout << "mesh vertices : " << surface[0].vertex().size() << endl;
      cout << "mesh polygons : " << surface[0].polygon().size() << endl;
      cout << "Object texture dim   : " << inpTex[0].nItem() << endl;
  
      if ( ! (  inpTex[0].nItem() ==  surface[0].vertex().size() ) )
        {
          cerr << "The triangulation and the textures must correspond to the same object \n";
          assert( 0 );
        }

      cout << "Reading object volume   : " << volfile << endl;
      AimsData<short> inpVol;
      Reader<AimsData<short> > triGV( volfile );
      triGV >> inpVol;


      cout << "Mesh parcellation to volume parcellation" << endl;
      AimsData<short>    parcellationVolume;

      parcellationVolume = MeshParcellation2Volume( inpVol,inpTex[time],surface[0],label_vol,0 );


      cout << "Write parcellation volume\n";
      Writer<AimsData<short> >	imaW( fileout );
      imaW <<  parcellationVolume ;   
  
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

 
