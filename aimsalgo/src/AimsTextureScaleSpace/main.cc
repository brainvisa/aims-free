/* Copyright (c) 1995-2007 CEA
 *
 *  This software and supporting documentation were developed by
 *      CEA/DSV/SHFJ
 *      4 place du General Leclerc
 *      91401 Orsay cedex
 *      France
 *
 * This software is governed by the CeCILL license version 2 under 
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the 
 * terms of the CeCILL license version 2 as circulated by CEA, CNRS
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
 * knowledge of the CeCILL license version 2 and that you accept its terms.
 */


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
