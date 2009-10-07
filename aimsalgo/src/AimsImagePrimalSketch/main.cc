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
#include <aims/primalsketch/primalSketch.h>
#include <aims/primalsketch/primalSketch2graph.h>
#include <aims/primalsketch/primalSketchUtil.h>
#include <iostream>
using namespace aims;
using namespace carto;
using namespace std;

int main(int argc, const char **argv)
{
  try
    {
      std::string fileIn, fileoutScales, fileoutBlobs, fileMask, subject;
      int smoothOperator=0;
      float dt=0.1, tmax, tmin=1.0;
      string statFile, graphFile;

      subject="none";

      AimsApplication     app( argc, argv, "ScaleSpace d'une image au format FLOAT");

      app.addOption( fileIn, "-i", "inputImage");
      app.alias( "--input", "-i" );
      app.addOption( fileMask, "-m", "mask");
      app.alias( "--mask", "-m" );
      app.addOption( fileoutScales, "-os", "output scalespace filename");
      app.alias( "--output-scales", "-os" );
      app.addOption( fileoutBlobs, "-ob", "output blobs filename");
      app.alias( "--output-blobs", "-ob" );
      app.addOption( tmin, "-t1", "minimum scale (default=1.0)", 1.0);
      app.alias( "--scalemin", "-t1" );
      app.addOption( tmax, "-t2", "maximum scale");
      app.alias( "--scalemax", "-t2" );
      app.addOption( smoothOperator, "-s", "smoother (0 : Gaussian, 1 : diffusion; default=0)", 0);
      app.alias( "--smoother", "-s" );
      app.addOption( dt, "-dt", "time step (for diffusion only; default=0.1)", 0.1);
      app.alias( "--deltat", "-dt" );
      app.addOption( statFile, "-f", "name of file containing statistics for blob measurements normalisation", "" );
      app.alias ("--stats", "-f");
      app.addOption( subject, "-sj", "subject name (default : inputImage)", "none");
      app.alias ("--subject", "-sj");
      app.addOption( graphFile, "-og", "primal sketch graph", "");
      app.alias ("--graph", "-og");
      app.initialize();

      AimsData<float> imageS;
      Reader<AimsData<float> > readerS( fileIn );
      AimsData<float> mask;
      Reader<AimsData<float> > readerM( fileMask );
      if (subject=="none")
        subject=fileIn;

      std::cout << "Reading input image " << fileIn << " for subject " << subject << endl;
      if( !readerS.read( imageS))
        return EXIT_FAILURE;
      // Checking for NaNs. spmT maps have NaNs outside the brain.
      // (for some reason !)
      // This screws up the smoothing. If NaNs are detected, they
      // are replaced with 0 and a new volume is written.
      int x, y, z, sx, sy, sz, nbN=0;
      sx=imageS.dimX(); sy=imageS.dimY(); sz=imageS.dimZ();
      for (z=0; z<sz; z++)
        for (y=0; y<sy; y++)
          for (x=0; x< sx; x++)
            {
              if (imageS(x,y,z) != imageS(x,y,z))  // test for a NaN
                {
                  imageS(x,y,z)=0.0;
                  nbN++;
                }
            }
      if (nbN > 0)
        {
          std::cout << "Found some NaN value in the volume. This must be a spmT ;-)" << endl;
          std::cout << "Rewriting a clean volume" << endl;
          aims::Writer<AimsData<float> > writeNoNaN(fileIn);
          writeNoNaN.write(imageS);
        }

      cout << "Reading mask " << fileMask << endl;
      if( !readerM.read( mask ))
        return EXIT_FAILURE;

      TexturedData<AimsData<float>, AimsData<float> > maskData(&mask);

      cout << "Creating scale-space with geometric sampling of the scales (t=2^n) up to " << tmax << endl;

      cout << "Creating smoother" << endl;

      DiffusionSmoother<float> *diffSmooth;
      diffSmooth=new DiffusionSmoother<float>(dt);

      DericheSmoother<float> *gaussSmooth;
      gaussSmooth=new DericheSmoother<float>();

      Smoother<AimsData<float>, AimsData<float> > *smooth;

      if (smoothOperator==0)
     smooth=gaussSmooth;
      else if (smoothOperator==1)
     smooth=diffSmooth;
      else
        {
          cerr << "Wrong value for -s option" << endl;
          return EXIT_FAILURE;
        }

      cout << "Scale-space computation" << endl;

      ScaleSpace<AimsData<float>, AimsData<float> > scale_space(&imageS, smooth);

      scale_space.GenerateDefaultScaleSpace(tmax);

      cout << "Ecriture du scale-space dans le fichier " << fileoutScales << endl;

      scale_space.Write(fileoutScales);

      cout << "Computing primal sketch" << endl;

      PrimalSketch<AimsData<float>, AimsData<float> > sketch(subject, &scale_space, &maskData, IMAGE);

      sketch.ComputePrimalSketch(tmin, tmax, statFile);

      cout << "Converting primal sketch into graph" << endl;

      Primalsketch2graph<AimsData<float>, AimsData<float> > translate(&sketch);

      translate.DoIt();

      Graph *graphSketch=translate.GetGraph();

      if (graphFile=="")
           graphFile = fileIn + "_graphSketch.arg";

      aims::Writer<Graph > dataW(graphFile);
      cerr << "Writing file " << graphFile << endl;
      dataW.write(*graphSketch);

      cout << "Getting scale-space blob image" << endl;
      AimsData<short> *blobImage;
      blobImage=GetSSBlobImage(&sketch);

      cout << "Writing it in file " << fileoutBlobs << endl;
      aims::Writer<AimsData<short> > blobW(fileoutBlobs);
      blobW.write(*blobImage);

      cout << "Getting scale-space blob buckets" << endl;
      AimsBucket<Void> *blobBck;
      blobBck=GetSSBlobBucket(&sketch);

      cout << "Writing it in file " << fileoutBlobs << endl;
      aims::Writer<AimsBucket<Void> > bckW(fileoutBlobs);
      bckW.write(*blobBck);

      cout << "Finished" << endl;
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
