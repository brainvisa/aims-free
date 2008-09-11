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

#include <aims/data/data_g.h>
#include <aims/bucket/bucket.h>
#include <aims/io/io_g.h>
#include <aims/getopt/getopt2.h>
#include <aims/math/math_g.h>
#include "index.h"
#include <iomanip>

using namespace aims;
using namespace carto;
using namespace std;


int main( int argc, const char **argv )
{
  Reader<AimsBucket<DtiTensor> >	filein;
  string fileout;
  bool	trace=false, l1=false, l2=false, l3=false, fa=false, vr=false;
  bool	dir=false, ra=false, ci=false, li=false, m=false, all=false;
  int	dimX, dimY, dimZ;

  AimsApplication	app( argc, argv, 
                             "Extract trace,frationnal and relative " 
                             "anisotropy, volume ratio, directions, coherence "
                             "index, lattice index, linear, planar and " 
                             "spherical measures from dtitensor bucket" );

  app.addOption( filein, "-i", "source dtitensor bucket" );
  app.addOption( fileout, "-o", "destination base file name", true );
  app.addOption( dimX, "-x", "dimensions of output images" );
  app.addOption( dimY, "-y", "dimensions of output images" );
  app.addOption( dimZ, "-z", "dimensions of output images" );
  app.addOption( trace, "--trace", "calculate trace <fileout_trace.ima>", 
                 true );
  app.addOption( l1, "--l1", "calculate largest eigenvalue " 
                 "<fileout_lambda1.ima>", true );
  app.addOption( l2, "--l2", "calculate second eigenvalue " 
                 "<fileout_lambda2.ima>", true );
  app.addOption( l3, "--l3", "calculate third eigenvalue " 
                 "<fileout_lambda3.ima>", true );
  app.addOption( fa, "--fa", "calculate fractionnal anisotropy " 
                 "<fileout_fa.ima>", true );
  app.addOption( vr, "--vr", "calculate volume ratio <fileout_vr.ima>", true );
  app.addOption( dir, "--dir", "calculate the main direction " 
                 "<fileout_dir.ima>", true );
  app.addOption( ra, "--ra", "calculate relative anisotropy <fileout_ra.ima>", 
                 true);
  app.addOption( ci, "--ci", "calculate coherence index  <fileout_ci.ima>. "
                 "NB: This index is computed in 2D which seems meaningless", 
                 true );
  app.addOption( li, "--li", "calculate lattice index <fileout_li.ima>", 
                 true );
  app.addOption( m, "--m", "calculate linear, planar and spherical measures " 
                 "<fileout_linear.ima>, <fileout_planar.ima>, " 
                 "<fileout_spherical.ima>", true );
  app.addOption( all, "--all", "calculate all maps" );
  app.alias( "--input", "-i" );
  app.alias( "--output", "-o" );

  try
    {
      app.initialize();

      if( fileout.empty() )
        fileout = filein.fileName();
      if( all )
        {
          trace = true;
          l1    = true;
          l2    = true;
          l3    = true;
          fa    = true;
          vr    = true;
          dir   = true;
          ra    = true;
          ci    = true;
          li    = true;
          m     = true;
        }
  
      // Lecture du fichier d'entree *.bck
      cout << "reading file..." << endl;
      AimsBucket< DtiTensor > data;
      filein >> data;
      Index test( data, dimX, dimY,dimZ );
 
      // Sauvegarde de la trace
      if(trace !=0)
        {
          cout << "saving trace..." << endl;
          AimsData <float> tr ( dimX, dimY,dimZ );
          test.calcul_Trace();
          tr = test.Trace();
          Writer< AimsData<float> > dataWtr( string(fileout) + "_trace" );
          dataWtr << tr;  
        }

      // Sauvegarde de lambda 1
      if(l1 !=0)
        {
          cout << "saving lambda 1..." << endl;
          AimsData <float> tr ( dimX, dimY,dimZ );
          test.calcul_Lambda1();
          tr = test.Lambda1();
          Writer< AimsData<float> > dataWtr( string(fileout) + "_lambda1" );
          dataWtr << tr;  
        }

      // Sauvegarde de lambda 2
      if(l2 !=0)
        {
          cout << "saving lambda 2..." << endl;
          AimsData <float> tr ( dimX, dimY,dimZ );
          test.calcul_Lambda2();
          tr = test.Lambda2();
          Writer< AimsData<float> > dataWtr( string(fileout) + "_lambda2" );
          dataWtr << tr;  
        }

      // Sauvegarde de lambda 3
      if(l3 !=0)
        {
          cout << "saving lambda 3..." << endl;
          AimsData <float> tr ( dimX, dimY,dimZ );
          test.calcul_Lambda3();
          tr = test.Lambda3();
          Writer< AimsData<float> > dataWtr( string(fileout) + "_lambda3" );
          dataWtr << tr;  
        }
  
  
      // Sauvegarde de Fractionnal Anisotropy
      if(fa !=0)
        {
          cout << "saving fractionnal anisotropy..." << endl;
          AimsData <float> fracAni ( dimX, dimY,dimZ);
          test.calcul_FA();
          fracAni = test.FracAniso();
          Writer< AimsData<float> > dataWfa( string(fileout) + "_fa" );
          dataWfa << fracAni;  
        }
  
  
      // Sauvegarde de Volume Ratio
      if(vr !=0)
        {
          cout << "saving volume ratio..." << endl;
          AimsData <float> volRatio ( dimX, dimY,dimZ );
          test.calcul_VR();
          volRatio = test.VolRatio();
          Writer< AimsData<float> > dataWvr( string(fileout) + "_vr" );
          dataWvr << volRatio;  
        }
  
      // Sauvegarde de la direction principale
      if(dir !=0)
        {
          cout << "saving direction..." << endl;
          AimsData <Point3df> direction ( dimX, dimY,dimZ );
          test.calcul_Direction();
          direction = test.Direction();
          Writer<AimsData<Point3df> > dataWdir( string(fileout) + "_dir" );
          dataWdir << direction;  
        }
      
      // Sauvegarde de Relative Anisotropy
      if(ra !=0)
        {
          cout << "saving relative anisotropy..." << endl;
          AimsData <float> relAni ( dimX, dimY,dimZ );
          test.calcul_RA();
          relAni = test.RelativAniso();
          Writer< AimsData<float> > dataWra( string(fileout) + "_ra" );
          dataWra << relAni;  
        }
    
      // Sauvegarde de mean Tensor
      // DtiTensor mean;
      // test.calcul_meanTensor();   
      // mean = test.MeanTensor();
      // ofstream os("mean");
      // int i;
      // for(i=0; i < 6; i++)
      //   os << mean.base().coef().item(i) <<"\n";
 
      // Sauvegarde de coherence index
      if(ci !=0)
        {
          cout << "saving coherence index..." << endl;
          AimsData <float> CI (dimX, dimY,dimZ);
          test.calcul_Direction();
          test.calcul_coherenceIndex();
          CI=test.CoherenceIndex ();
          Writer< AimsData<float> > dataWci( string(fileout) + "_ci" );
          dataWci << CI;  
        }
  
  
      //AimsData <float> LI ( dimX, dimY,dimZ );
      if(li != 0)
        {
          cout << "saving lattice index..." << endl;
          AimsData <float> LI (dimX, dimY,dimZ);
          test.calcul_latticeIndex();
          LI = test.LatticeIndex();
          Writer< AimsData<float> > dataWli(string(fileout) + "_li" );
          dataWli << LI;  
        }
  
      if(m !=0)
        {

          cout << "saving linear, planar and spherical measures..." << endl;
          AimsData <float> LM( dimX, dimY,dimZ );
          AimsData <float> PM( dimX, dimY,dimZ );
          AimsData <float> SM( dimX, dimY,dimZ );
          AimsData <float> AniM( dimX, dimY,dimZ );
          test.calcul_Measure();
  
          LM = test.LinearMeasure();
          Writer <AimsData<float> > dataWlm(string(fileout) + "_linear" );
          dataWlm << LM; 
    
          PM = test.PlanarMeasure();
          Writer <AimsData<float> > dataWpm(string(fileout) + "_planar" );
          dataWpm << PM; 
    
          SM = test.SphericalMeasure();  
          Writer <AimsData<float> > dataWsm(string(fileout) + "_spherical" );
          dataWsm << SM; 
    
          int x, y, z;
          ForEach3d(AniM, x, y, z)
            AniM(x,y,z) = LM(x,y,z) + PM(x,y,z);
  
          Writer <AimsData<float> > dataWam(string(fileout) +"_anisotropy");
          dataWam << AniM;
        }

      return EXIT_SUCCESS;
    }
  catch( user_interruption & )
    {
    }
  catch( exception & e )
    {
      cerr << e.what() << endl;
    }
  return EXIT_FAILURE;
}
 
