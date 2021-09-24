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
#include <aims/getopt/getopt2.h>   // Utilitaire pour le parsing des options
#include <aims/data/data_g.h>      // Format mémoire données Aims
#include <aims/io/io_g.h>	   // IO standard des formats disque
#include <aims/texdata/texdata.h>
#include <aims/signalfilter/diffusionSmoother.h>
#include <aims/primalsketch/greyLevelBlob.h>
#include <aims/primalsketch/scalespace.h>
#include <aims/primalsketch/dericheSmoother_d.h>
#include <aims/primalsketch/primalSketch.h>
#include <aims/primalsketch/primalSketch2graph.h>
#include <aims/primalsketch/primalSketchUtil.h>
#include <stdlib.h>

using namespace aims;
using namespace carto;
using namespace std;


int main(int argc, const char **argv)
{
  try
    {
      std::string fileIn, fileMask, statPattern; 
      float dt=0.1, tmax, tmin=1.0;
      char statFile[100];
      int smoothOperator=0;

      std::cout << "Here we go !" << std::endl;
    
      AimsApplication application( argc, argv, 
                                   "Compute every scale and corresponding " 
                                   "grey-level blob measurements from an " 
                                   "image" );
      application.addOption( fileIn, "-i", "input data" ); 
      application.addOption( fileMask, "-m", "mask" ); 
      application.addOption( tmin, "-t1",  "scale min (default=1.0)", 1.0 );
      application.addOption( tmax, "-t2",  "scale max");
      application.addOption( statPattern, "-p", "stats file name pattern" ); 
      application.addOption( smoothOperator, "-s", "smoother (0 : Gaussian, " 
                             "1 : diffusion)");
      application.initialize();

      std::cout << "initialisation done" << std::endl;
    
      AimsData<float> imaIn;
      Reader<AimsData<float> > readerIn( fileIn );
      std::cout << "Reading input image " << fileIn << std::endl;
    
      if( !readerIn.read( imaIn))
    	return EXIT_FAILURE;
    
      AimsData<float> imaMask;
      Reader<AimsData<float> > readerMask( fileMask );
      std::cout << "Reading input Mask " << std::endl;

      if( !readerMask.read( imaMask ))
    	return EXIT_FAILURE;
        
      TexturedData<AimsData<float>, AimsData<float> > mask(&imaMask);
            
      int sx=imaIn.dimX(),
        sy=imaIn.dimY(),
        sz=imaIn.dimZ();

      cout << "Dimensions : " << sx << "x" << sy << "x" << sz << endl;
    
      cout << "checking for NaNs" << endl;
      int x,y,z, nbN=0;
      // test for NaNs and replace them with 0; might be tmp
      // spmT sometime (always) have NaN values outside the brain...
      for (z=0; z<sz; z++)
        for (y=0; y<sy; y++)
          for (x=0; x< sx; x++)
            {
              if (imaIn(x,y,z) != imaIn(x,y,z))  // test for a NaN
                {
                  imaIn(x,y,z)=0.0;
                  nbN++;
                }
            }
    
      std::cout << "Found " << nbN << "NaNs." << std::endl;
      if (nbN > 0)
        {
          std::cout << "Rewriting a clean volume" << std::endl;
          aims::Writer<AimsData<float> > scaleN(fileIn);
          scaleN.write(imaIn);
        }
    
      std::cout << "Creating smoother" << std::endl;
    
      DiffusionSmoother<float> *diffSmooth = new DiffusionSmoother<float>(dt);

      DericheSmoother<float> *gaussSmooth = new DericheSmoother<float>();
  
      Smoother<VolumeRef<float>, VolumeRef<float> > *smooth;
  
      if (smoothOperator==0)
        smooth=gaussSmooth;
      else if (smoothOperator==1)
        smooth=diffSmooth;
      else
        {
          std::cerr << "Wrong value for -s option" << std::endl;
          return EXIT_FAILURE;
        }
    
      ScaleSpace<AimsData<float>, AimsData<float> > sspace(&imaIn, smooth);
    
      std::cout << "Computing scale levels and blobs" << std::endl;

      float t, del, tlow=0.0;
      for (t=tmin; t<=tmax ; t=t+dt)
        {
          std::cout << "Scale " << t << std::endl;
          sspace.AddScale(t);
          sprintf(statFile, "%s_%.1f_.dat", statPattern.c_str(), t);
          std::cout << "Computing blobs and writing it in " << statFile 
                    << std::endl;
          sspace.Scale(t)->DetectBlobs(&mask, statFile);
          tlow=t;
          del=tlow;
          sspace.RemoveScale(del);
        }
    
      std::cout << "Finished" << std::endl;

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

    exit(EXIT_SUCCESS);
}
