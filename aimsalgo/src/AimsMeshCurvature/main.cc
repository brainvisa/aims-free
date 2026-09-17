
#include <aims/mesh/geometric.h>
#include <aims/data/data_g.h>
#include <aims/io/io_g.h>
#include <aims/math/math_g.h>
#include <aims/vector/vector.h>
#include <aims/mesh/texture.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/getopt/getopt2.h>
#include <iostream>
#include <iomanip>

using namespace aims;
using namespace carto;
using namespace std;

typedef float float3[3];


int main( int argc, const char** argv )
{
  
  try
    {
      string  meshfile, outtexfile;
      string  method = "fem";
      float ratio= 0.95;

      AimsApplication	app( argc, argv, "Estime the mean curvature of a mesh" );
      app.addOption( meshfile, "-i", "input file" );
      app.alias( "--input", "-i" );
      app.addOption( outtexfile, "-o", "output texture" );
      app.alias( "--output", "-o" );
      app.addOption( method, "-m", "method: \"fem\" -> finite elements, "
          "\"boix\" -> Boix, \"barycenter\" -> barycenter, "
          "\"boixgaussian\" -> Boix Gaussian, \"gaussian\" -> Gaussian" );
      app.alias( "--method", "-m" );
      app.addOption( ratio, "-r", "proportion (%) of point to be thresholded ", true );
      app.alias( "--ratio", "-r" );
      app.initialize();



      //
      // read triangulation
      //
      cout << "reading triangulation   : " << flush;
      AimsSurfaceTriangle surface;
      Reader<AimsSurfaceTriangle> triR( meshfile );
      triR >> surface;
      cout << "done" << endl;

      TimeTexture<float>	outTex;

  
      CurvatureFactory CF;
      Curvature * curv = CF.createCurvature(surface,method);
      cout << "processing..." << flush;
      outTex[0] = curv->doIt();
      cout << "done\n";
      curv->regularize(outTex[0],ratio);
      curv->getTextureProperties(outTex[0]);
      delete curv;
  
      cout << "writing texture : " << flush;
      Writer<Texture1d>	texW( outtexfile );
      texW.write( outTex );
      cout << "done " << endl;

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


