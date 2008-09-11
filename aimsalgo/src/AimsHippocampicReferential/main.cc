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
#include <aims/io/io_g.h>
#include <aims/io/motionW.h>
#include <aims/getopt/getopt2.h>
#include <aims/math/math_g.h>
#include <aims/talairach/talairach.h>
#include <cmath>
#include <math.h>
#include <aims/math/mathelem.h>

using namespace aims;   
using namespace std;
using namespace carto;

int main( int argc, const char **argv )

{
  int result = EXIT_SUCCESS;
  try {
    
    //
    // Default values
    //
    vector<float> acMm ;
    vector<float> pcMm ;
    vector<float> ihpMm ;
    float angle = 30 ;
    string outfile, axis = "x" ;
     //
    // Collecte des arguments.
    //
    AimsApplication application( argc, argv, "Transformation to hippocampic referential" );
    application.addOption( outfile, "-o", "Output motion file" );
    application.addOptionSeries( acMm, "--ac", "Anterior commissure (mm)", 3, 3);
    application.addOptionSeries( pcMm, "--pc", "Posterior commissure (mm)", 3, 3);
    application.addOptionSeries( ihpMm, "--ih", "Inter hemispheric point (mm)", 3, 3);
    application.addOption( angle, "--angle", "rotation angle in degrees (default 30°)", 1 );
    application.addOption( axis, "--axis", "rotation axis 'x', 'y' or 'z' (default x)", 1 ) ;
    application.initialize();
        
    //
    //     Data
    //
    
    if( axis != "x" && axis != "y" && axis != "z" )
      throw runtime_error("Bad axis argument : should be x, y, or z") ;
    
    TalairachPoints talPoints( Point3d(0,0,0), Point3d(0,0,0), Point3d(0,0,0) ) ;
    talPoints.ACmm() = Point3df( acMm[0], acMm[1], acMm[2] ) ;
    talPoints.PCmm() = Point3df( pcMm[0], pcMm[1], pcMm[2] ) ;
    talPoints.Hemimm() = Point3df( ihpMm[0], ihpMm[1], ihpMm[2] ) ;
    
    TalairachReferential talRef ;
    Motion toRef = talRef.computeTransformation( talPoints ) ;
    
    Point3df rotCenter( (acMm[0]+pcMm[0])/2., (acMm[1]+pcMm[1])/2., (acMm[2]+pcMm[2])/2. ) ;
    
    Motion rot ;
    rot.setToIdentity() ;
    
    rot.setRotationAffine( axis == "x" ? angle * M_PI / 180. : 0., 
			   axis == "y" ? angle * M_PI / 180. : 0., 
			   axis == "z" ? angle * M_PI / 180. : 0., rotCenter ) ;

    MotionWriter wri( outfile ) ;
    wri.write( rot * toRef ) ;
    result = EXIT_SUCCESS ;
  }
  catch( user_interruption &e ) {
  }
  catch( std::exception &e ) {
    cerr << argv[ 0 ] << ": " << e.what() << endl;
    result = EXIT_FAILURE;
  }
  return result;  
}

