#include <cstdlib>
#include <iostream>
#include <aims/getopt/getopt2.h>
#include <aims/data/data.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/io/process.h>
#include <aims/io/finder.h>
#include <aims/texdata/texdata.h>
#include <aims/primalsketch/greyLevelBlob.h>
#include <aims/mesh/surface.h>
#include <aims/mesh/texture.h>
#include <aims/primalsketch/scalespace.h>
#include <aims/primalsketch/finiteElementSmoother_d.h>

using namespace aims;
using namespace carto;
using namespace std;

int main(int argc, const char **argv)
{
  try
    {
      std::string fileInT, fileInM, fileout;
      float dt, tmax;

      AimsApplication     app( argc, argv, "ScaleSpace d'une surface/texture");

      app.addOption( fileInT, "-i1", "inputTexture");
      app.alias( "--input1", "-i1" );
      app.addOption( fileInM, "-i2", "inputMesh");
      app.alias( "--input2", "-i2" );
      app.addOption( fileout, "-o", "output filename pattern");
      app.alias( "--output", "-o" );
      app.addOption( tmax, "-tm", "maximum scale");
      app.alias( "--scalemax", "-tm" );
      app.addOption( dt, "-dt", "time step");
      app.alias( "--deltat", "-dt" );

      app.initialize();

      cout << "Reading input surface and texture: " << fileInM << " and " << fileInT << endl;
      cout << "Creating up to scale " << tmax << "with geometric sampling of the scales (t=2^n)" << endl;

      Reader<AimsSurfaceTriangle>	r1(fileInM);
      Reader<Texture1d>             r2(fileInT);
      AimsSurfaceTriangle           laMesh;
      Texture1d                     laTexture;
      r1.read(laMesh);
      r2.read(laTexture);

      cout << "Size=" << laMesh[0].vertex().size() << endl;

      cout << "Création du smoother" << endl;

      FiniteElementSmoother<3, float> *smooth;
      smooth=new FiniteElementSmoother<3, float>(dt, &(laMesh[0]));

      cout << "Création du scale-space" << endl;

      ScaleSpace<AimsSurface<3, Void>, Texture<float> > scale_space(&(laMesh[0]), &(laTexture[0]), smooth);

      scale_space.GenerateDefaultScaleSpace(tmax);
 
      scale_space.Write(fileout);

      /*  cout << "Creation du textured data" << endl;

      TexturedData<AimsSurface<3,Void>, Texture<float> > textured(&(laMesh[0]), &(laTexture[0]));

      cout << "cr�ation de l'extracteur de blobs" << endl;

      ExtractGreyLevelBlobs<AimsSurface<3, Void>, Texture<float> > extractBlobs(&textured);

      cout << "Lancement extraction" << endl;

      extractBlobs.Run();

      cout << "found " << extractBlobs.nbBlobs() << " blobs" << endl;
      */
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
