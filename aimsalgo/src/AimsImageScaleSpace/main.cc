/* This software and supporting documentation are distributed by
 *     Institut Federatif de Recherche 49
 *     CEA/NeuroSpin, Batiment 145,
 *     91191 Gif-sur-Yvette cedex
 *     France
 *
 * This software is governed by the CeCILL-B license under
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the
 * terms of the CeCILL-B license as circulated by CEA, CNRS
 * and INRIA at the following URL "http://www.cecill.info".
 *
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability.
 *
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or
 * data to be ensured and,  more generally, to use and operate it in the
 * same conditions as regards security.
 *
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL-B license and that you accept its terms.
 */


#include <cstdlib>
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
#include <aims/signalfilter/diffusionSmoother.h>
#include <aims/primalsketch/scalespace.h>
#include <aims/primalsketch/dericheSmoother_d.h>
#include <iostream>

using namespace aims;
using namespace carto;
using namespace std;

int main(int argc, const char **argv)
{
  try
    {
      std::string fileIn, fileout;
      int smoothOperator;
      float dt, tmax;

      AimsApplication     app( argc, argv, "ScaleSpace d'une image...");

      app.addOption( fileIn, "-i", "inputImage");
      app.alias( "--input", "-i" );
      app.addOption( fileout, "-o", "output filename pattern");
      app.alias( "--output", "-o" );
      app.addOption( tmax, "-tm", "maximum scale");
      app.alias( "--scalemax", "-tm" );
      app.addOption( smoothOperator, "-s", "smoother (0 : Gaussian, 1 : diffusion");
      app.alias( "--smoother", "-s" );
      app.addOption( dt, "-dt", "time step");
      app.alias( "--deltat", "-dt" );

      app.initialize();

      AimsData<float> imageS;
      Reader<AimsData<float> > readerS( fileIn );
      cout << "Reading input image " << fileIn << endl;

      if( !readerS.read( imageS))
        return EXIT_FAILURE;
      cout << "Dimensions : " << imageS.dimX() << "x" << imageS.dimY() << "x" << imageS.dimZ() << endl;

      cout << "Creating scale-space with geometric sampling of the scales (t=2^n) up to " << tmax << endl;

      cout << "Création du smoother" << endl;

      DiffusionSmoother<float> *diffSmooth;
      diffSmooth=new DiffusionSmoother<float>(dt);

      DericheSmoother<float> *gaussSmooth;
      gaussSmooth=new DericheSmoother<float>();
  
      Smoother<VolumeRef<float>, VolumeRef<float> > *smooth;
  
      if (smoothOperator==0)
        smooth=gaussSmooth;
      else if (smoothOperator==1)
        smooth=diffSmooth;
      else
        {
          cerr << "Wrong value for -s option" << endl;
          return EXIT_FAILURE;
        }

      cout << "Création du scale-space" << endl;

      ScaleSpace<AimsData<float>, AimsData<float> > scale_space(&imageS, smooth);

      scale_space.GenerateDefaultScaleSpace(tmax);

      scale_space.Write(fileout);

      /*  cout << "Creation du textured data" << endl;

      TexturedData<AimsSurface<3,Void>, Texture<float> > textured(&(laMesh[0]), &(laTexture[0]));

      cout << "création de l'extracteur de blobs" << endl;

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

