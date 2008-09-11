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


#include <aims/getopt/getopt2.h>
#include <aims/data/data.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/io/process.h>
#include <aims/io/finder.h>
#include <aims/texdata/texdata.h>
#include <aims/signalfilter/diffusionSmoother.h>
#include <aims/primalsketch/dericheSmoother_d.h>
#include <aims/primalsketch/peronaMalikSmoother_d.h>
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
      float dt=0.1, tmax;
      float K=0.98;

      AimsApplication     app( argc, argv, "Image smoothing by diffusion or " 
                               "recursive filtering");

      app.addOption( fileIn, "-i", "input Image");
      app.alias( "--input", "-i" );
      app.addOption( fileout, "-o", "output Image");
      app.alias( "--output", "-o" );
      app.addOption( tmax, "-t", "scale");
      app.alias( "--scale", "-t" );
      app.addOption( smoothOperator, "-s", "smoother (0 : Gaussian, " 
                     "1 : isotropic diffusion, 2 : Perona-Malik)");
      app.alias( "--smoother", "-s" );
      app.addOption( dt, "-dt", "time step (diffusion only; default=0.1)", 
                     true );
      app.alias( "--deltat", "-dt" );
      app.addOption( K, "-k", "gradient bound (0-1), Perona-Malik only, " 
                     "default=0.98", true );
      app.alias( "--gradient", "-k" );

      app.initialize();

      AimsData<float> imageIn, imageOut;
      Reader<AimsData<float> > readerS( fileIn );
      cout << "Reading input image " << fileIn << endl;

      if( !readerS.read( imageIn))
        return EXIT_FAILURE;
      cout << "Dimensions : " << imageIn.dimX() << "x" << imageIn.dimY() 
           << "x" << imageIn.dimZ() << endl;

      cout << "Creating appropriate smoother : ";


      // --------------------- temp ---------------------
      /*int x,y,z;
        AimsData<float> test;
        test=AimsData<float>(60,60,60);

        for (z=0; z<60; z++)
	for (y=0; y<60; y++)
        for (x=0; x<60; x++)
        {
        if (x<30)
        test(x,y,z)=0.0;
        else
        test(x,y,z)=100.0;
        }

        Writer<AimsData<float> > writerTest( "testIma.ima" );

        writerTest.write(test);
      */
      //------------------------------------------------------



      DiffusionSmoother<float> *diffSmooth;
      diffSmooth=new DiffusionSmoother<float>(dt);

      DericheSmoother<float> *gaussSmooth;
      gaussSmooth=new DericheSmoother<float>();

      PeronaMalikSmoother<float> *peronaSmooth;
      peronaSmooth=new PeronaMalikSmoother<float>(dt, K, 1.0, 2);
  
      Smoother<AimsData<float>, AimsData<float> > *smooth;
  
      if (smoothOperator==0)
        {
          smooth=gaussSmooth;
          cout << "Gaussian recursive filtering " << endl;
        }
      else if (smoothOperator==1)
        {
          smooth=diffSmooth;
          cout << "diffusion equation" << endl;
        }
      else if (smoothOperator==2)
        {
          smooth=peronaSmooth;
          cout << "anisotropic Perona-Malik diffusion equation" << endl;
        }
      else
        {
          cerr << "Wrong value for -s option" << endl;
	  return EXIT_FAILURE;
        }

      cout << "Smoothing" << endl;
 
      imageOut=smooth->doSmoothing(imageIn, tmax / dt);

      cout << "Writing smoothed image " << fileout << endl;

      Writer<AimsData<float> > writerS( fileout );

      writerS.write( imageOut);
    }
  catch( user_interruption & )
    {
      return EXIT_FAILURE;
    }
  catch( exception & e )
    {
      cerr << e.what() << endl;
      return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
