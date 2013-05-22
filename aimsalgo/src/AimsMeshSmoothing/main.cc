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
#include <aims/utility/utility_g.h>
#include <aims/mesh/mesh_g.h>
#include <aims/io/io_g.h>

using namespace aims;
using namespace carto;
using namespace std;


int main( int argc, const char** argv )
{
  
  Mesher::SmoothingType algoType = Mesher::LOWPASS;
  string algoTypeStr = "lowpass";
  int nIteration = 10;
  float rate = 0.2;
  float featureAngle = 180.0;
  float springForce = 0.2;
  int asciiFlag = 0;
  Reader<AimsSurfaceTriangle> triR;
  Writer<AimsSurfaceTriangle> triW;

  //
  // parse options
  //
  AimsApplication app( argc, argv, "Smoothes a triangulation" );
  app.addOption( triR, "-i", "input triangulation" );
  app.alias( "--input", "-i" );
  app.addOption( triW, "-o", "output triangulation", true );
  app.alias( "--output", "-o" );
  app.addOption( algoTypeStr, "--algoType", "alorithm's type : "
                 "laplacian, simplespring, polygonspring or lowpass "
                 "[default=lowpass]", true );
  app.addOption( nIteration, "--nIteration",
                 "number of iterations [default=10]", true );
  app.addOption( rate, "--rate",
                 "moving factor at each iteration [default=0.2]",
                 true );
  app.addOption( featureAngle, "--featureAngle",
                 "feature angle (in degrees) below which the vertex is not "
                 "moved, only for the Laplacian algorithm, between 0 and 180 "
                 "degree [default=0]", true );
  app.addOption( springForce, "--springForce", "restoring force for the Simple "
                 "Spring and Polygon Spring algorithm, between 0 and 1 "
                 "[default=0.2]", true );
  app.addOption( asciiFlag, "--ascii",
                 "write *.mesh in ASCII [default=binar]", true );
  
  try
  {
    app.initialize();
  
    //
    // chek options
    //
    ASSERT( nIteration > 0 );
    ASSERT( featureAngle >= 0.0 && featureAngle <= 180.0 );
    ASSERT( springForce >= 0.0 && springForce <= 1.0 );
    ASSERT( rate >= 0.0 && rate <= 1.0 );
    
    if( algoTypeStr == "laplacian" )
      algoType = Mesher::LAPLACIAN;
    else if ( algoTypeStr == "lowpass" )
      algoType = Mesher::LOWPASS;
    else if ( algoTypeStr == "simplespring" )
      algoType = Mesher::SIMPLESPRING;
    else if ( algoTypeStr == "polygonspring" )
      algoType = Mesher::POLYGONSPRING;
    
    if ( triW.fileName().empty() )
      triW.setFileName( triR.fileName() );

    cout << endl;

    //
    // read triangulation
    //
    cout << "reading triangulation   : " << flush;
    AimsSurfaceTriangle surface;
    triR >> surface;
    cout << "done" << endl;

    //
    // smooth mesh
    //
    cout << "smoothing mesh          : " << flush;
    Mesher mesher;
    
    mesher.setSmoothing( algoType, nIteration, rate );
    if ( algoType == Mesher::LAPLACIAN )
      mesher.setSmoothingLaplacian( featureAngle );
    if ( algoType == Mesher::SIMPLESPRING or algoType == Mesher::POLYGONSPRING )
      mesher.setSmoothingSpring( springForce );
    
    mesher.smooth( surface );
    cout << "done" << endl;
    
    
    //
    // save triangulation
    //
//     cout << "saving triangulation    : " << flush;
    triW.write( surface, asciiFlag );
    cout << "done" << endl;
    
    
  }
  catch( user_interruption & )
  {
    return EXIT_FAILURE;
  }
  catch( exception & e )
  {
    cerr << e.what() << endl;
    return( EXIT_FAILURE );
  }

  return EXIT_SUCCESS;
}

