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


#include <cstdlib>
#include <iostream>
#include <aims/mesh/inflate.h>
#include <aims/getopt/getopt2.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/io/process.h>
#include <aims/io/finder.h>
#include <aims/io/meshW.h>
#include <aims/mesh/surface.h>
#include <aims/mesh/texture.h>
#include <aims/mesh/geometric.h>

using namespace aims;
using namespace carto;
using namespace std;


int main(int argc, const char **argv)
{
  try
    {
      string filein,fileout,filecurv;

      int t = 500;
      float Knorm=0.01, Kspring=0.01, Ksmooth=0.5, bound=5000.0 ;
      bool seq=false;

      AimsApplication     app( argc, argv,
                               "Gonflement d'une surface (tri ou mesh)" );

      app.addOption( filein, "-i", "input" );
      app.alias( "--input", "-i" );
      app.addOption( t, "-t", "evolution time", 500);
      app.alias( "--time", "-t" );
      app.addOption( fileout, "-o", "output filename" );
      app.alias( "--output", "-o" );
      app.addOption( Knorm, "-Kn", "normal force weight (default=0.01)", 
                     true );
      app.alias( "--Knormal", "-Kn" );
      app.addOption( Kspring, "-Ksp", "spring force weight (default=0.01)", 
                     true );
      app.alias( "--Kspring", "-Ksp" );
      app.addOption( Ksmooth, "-Ksm", "smoothing force weight (default=0.5)", 
                     true );
      app.alias( "--Ksmooth", "-Ksm" );
      app.addOption( bound, "-B", "bound for the force computation " 
                     "(default=5000.0)", true );
      app.alias( "--Bound", "-B" );
      app.addOption( seq, "-S", "to save the inflating sequence", true );
      app.alias( "--Sequence", "-S" );
      app.addOption( filecurv, "-c", "mean curvature file name" );
      app.alias( "--curvature", "-c" );
      app.initialize();

      AimsSurfaceTriangle		    *surf = new AimsSurfaceTriangle;
      Reader<AimsSurfaceTriangle> reader( filein );
      if( !reader.read( *surf ) )
        {
          cerr << "could not read " << filein << " mesh file" << endl;
          exit( 1 );
        }

      cout << "Parameters: " << endl;
      cout << "----------- " << endl;

      cout << endl;
      cout << "Input     : " << filein << endl;
      cout << "Output    : " << fileout << endl;
      cout << "Iterations: " << t << endl;
      cout << "Knormal   : " << Knorm << endl;
      cout << "Kspring   : " << Kspring << endl;
      cout << "Ksmooth   : " << Ksmooth << endl;
      cout << "Bound     : " << bound << endl;
      cout << "Sequence  : " << seq << endl;
      cout << endl;

      set<int> timeSet;
      timeSet.insert(t-1);
      if (seq)
        {
          timeSet.insert(1);
          cout << "Adding time t=1";
          int k=0;
          while ((10*::pow(2,k))<t)
            {
              timeSet.insert(int(floor(10*::pow(2,k))));
              cout << ", " << int(floor(10*::pow(2,k)));
              k++;
            }
          cout << " to the sequence time list" << endl;
        }

      cout << "The surface mesh has " << surf->vertex().size() << " nodes" 
           << endl;

      AimsSurfaceTriangle *inflated;

      BarycenterCurvature BC((*surf));
      TimeTexture<float> curvText;
      curvText[0]=BC.doIt();

 
      Writer<TimeTexture<float> > writerM(filecurv);
      if ( !writerM.write( curvText, true ) )
        {
          cerr << "could not write mean curvature texture" << endl;
          exit( 1 );
        }

      inflated 
        = AimsInflateMesh(t, *surf, Knorm, Kspring, Ksmooth, bound, timeSet);

      MeshWriter<3,Void> writer( fileout );
      cout << endl;
      cout << "Writing inflated surface/sequence " << fileout << endl;
      //   if ( !writer.write( *inflated, true ) )
      //   {
      //     cerr << "could not write " << fileout << " mesh file" << endl;
      //     exit( 1 );
      //   }

      writer.write( *inflated);
      cout << "Finished" << endl;

      delete surf;
      delete inflated;
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
