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
#include <aims/mesh/mesh_g.h>
#include <aims/io/io_g.h> 
#include <aims/utility/utility_g.h>
#include <aims/getopt/getopt2.h>

using namespace aims;
using namespace carto;
using namespace std;


int main( int argc, const char** argv )
{ 
  // char *fileIn = NULL, *fileOut = NULL;
  int smoothFlag = 0;
  int smoothIt = 10;
  float smoothRate = 0.2;
  float smoothAngle = 180.0;
  float deciReductionRate = 99.0;
  float deciMaxClearance = 10.0;
  float deciMaxError = 10.0 ;
  float deciAngle = 180.0;
  int triFlag = 0;
  uint minFacetNumber = 50;
  int asciiFlag = 0;
  Reader<AimsSurfaceTriangle> triR;
  Writer<AimsSurfaceTriangle> triW;
  Reader<TimeTexture<float> > texR;
  TimeTexture<float> precisionmap;
  vector<float> precthresholds;

  //
  // parse options
  //
  AimsApplication app( argc, argv, "Decimates a triangulation" );
  app.addOption( triR, "-i", "input triangulation" );
  app.alias( "--input", "-i" );
  app.addOption( triW, "-o", "output triangulation", true );
  app.alias( "--output", "-o" );
  app.addOption( smoothFlag, "--smooth",
                 "smoothes the mesh [default is no smoothing]", true );
  app.addOption( smoothIt, "--smoothIt",
                 "smoothing number of iterations [default=10]", true );
  app.addOption( smoothRate, "--smoothRate",
                 "smoothing moving factor at each iteration [default=0.2]",
                 true );
  app.addOption( smoothAngle, "--smoothAngle",
                 "smoothing feature angle in degrees, between 0 and 180 "
                 "degree, [default=180]", true );
  app.addOption( deciReductionRate, "--deciReductionRate",
                 "decimation reduction rate expected in % [default=99%]",
                 true );
  app.addOption( deciMaxClearance, "--deciMaxClearance",
                 "maximum clearance of the decimated mesh expected in mm "
                 "[default=10]", true );
  app.addOption( deciMaxError, "--deciMaxError",
                 "maximum error distance from the original mesh in mm "
                 "[default=10]", true );
  app.addOption( deciAngle, "--deciAngle",
                 "feature angle in degrees, between 0 and 180: angle formed "
                 "by local mesh edges above which edges are preferably not "
                 "smoothed [default=180: no filtering]", true );
  app.addOption( asciiFlag, "--ascii",
                 "write *.mesh in ASCII [default=binar]", true );
  app.addOption( texR, "--precisionmap",
                 "precision map texture: if provided, use this texture to "
                 "determine if vertices can be removed easily or not during "
                 "decimation, instead of the default edges angles (see "
                 "--deciAngle parameter)", true );
  app.addOptionSeries( precthresholds, "--precthreshold",
                       "precision map thresholds (see --precisionmap "
                       "parameter): vertices with precision above the "
                       "threshold are not removed at a given iteration step. "
                       "Several values may be used for the successive "
                       "iteration steps.\nIf no threshold is provided, the "
                       "proportion from the decimation reduction rate is used "
                       "at step 0, then increases exponentially to "
                       "asymtotically reach the max precision within "
                       "about 20 steps",
                       0 );

  try
  {
    app.initialize();

    //
    // check options
    //
    ASSERT( smoothIt > 0 );
    ASSERT( smoothAngle >= 0.0 && smoothAngle <= 180.0 );
    ASSERT( smoothRate >= 0.0 && smoothRate <= 1.0 );
    ASSERT( deciReductionRate >= 0.0 && deciReductionRate <= 100.0 );
    ASSERT( deciMaxClearance >= 0.0 );
    ASSERT( deciMaxError >= 0.0 );
    ASSERT( deciAngle >= 0.0 && deciAngle <= 180.0 );


    if ( triW.fileName().empty() )
      triW.setFileName( triR.fileName() );

    cout << endl;

    //
    // read triangulation
    //
    cout << "reading triangulation        : " << flush;
    AimsSurfaceTriangle surface;
    triR >> surface;
    cout << "done" << endl;

    if( !texR.fileName().empty() )
    {
      cout << "reading precision texture : " << flush;
      texR.read( precisionmap );
      cout << "done" << endl;
    }


    //
    // decimating mesh
    //
    cout << "decimating mesh             : " << flush;
    Mesher mesher;
    if ( smoothFlag )
      mesher.setSmoothing( smoothAngle, smoothIt,
                          smoothRate, smoothRate, smoothRate );
    mesher.setDecimation( deciReductionRate, deciMaxClearance,
                          deciMaxError, deciAngle );
    mesher.setMinFacetNumber( minFacetNumber );
    float rate;
    if( !precisionmap.empty() )
      rate = mesher.decimate( surface, precthresholds, precisionmap );
    else
      rate = mesher.decimate( surface );
    cout << "done ( " << rate << "% ) " << endl;


    //
    // save triangulation
    //
    triW.write( surface, asciiFlag );
    cout << "done" << endl;


  }
  catch( user_interruption & )
  {
  }
  catch( exception & e )
  {
    cerr << e.what() << endl;
    return( EXIT_FAILURE );
  }

  return EXIT_SUCCESS;
}

