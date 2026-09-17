
#include <aims/io/io_g.h>
#include <aims/math/math_g.h>
#include <iomanip>
#include <aims/getopt/getopt2.h>
#include <aims/mesh/texture.h>
#include <aims/mesh/geometric.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>

using namespace aims;
using namespace carto;
using namespace std;

   
   

int main( int argc, const char** argv )
{
  try
    {
      string  outtexfile, curvtexfile;
      float ratio= 0.995;

      AimsApplication	app( argc, argv, "Regularize the texture" );
      app.addOption( curvtexfile, "-t", "texture file" );
      app.alias( "--texture", "-t" );
      app.addOption( outtexfile, "-o", "output texture" );
      app.alias( "--output", "-o" );
      app.addOption( ratio, "-r", "proportion (%) of point to be thresholded ", true );
      app.alias( "--ratio", "-r" );
      app.initialize();

     
      //
      // read input curv texture
      //
      cout << "reading texture   : " << flush;
      TimeTexture<float>	curvTex;
      Reader<TimeTexture<float> > ctexR( curvtexfile );
      ctexR >> curvTex;
      cout << "done" << endl;

      cout << "texture dim   : " << curvTex[0].nItem() << endl;

      TimeTexture<float>	outTex;
      Curvature::regularize(curvTex[0],ratio);
      Curvature::getTextureProperties(curvTex[0]);
        
      cout << "writing texture : " << flush;
      Writer<TimeTexture<float> >	texW( outtexfile);
      texW << outTex;
      cout << "done" << endl;

      return( 0 );
    }
  catch( user_interruption & )
    {
    }
  catch( exception & e )
    {
      cerr << e.what() << endl;
    }
  return 1;
}


