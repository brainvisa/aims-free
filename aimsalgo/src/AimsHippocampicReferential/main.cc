
#include <cstdlib>
#include <aims/io/writer.h>
#include <aims/getopt/getopt2.h>
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
    AffineTransformation3d toRef = talRef.computeTransformation( talPoints ) ;
    
    Point3df rotCenter( (acMm[0]+pcMm[0])/2., (acMm[1]+pcMm[1])/2., (acMm[2]+pcMm[2])/2. ) ;
    
    AffineTransformation3d rot ;
    rot.setToIdentity() ;
    
    rot.setRotationAffine( axis == "x" ? angle * M_PI / 180. : 0., 
                           axis == "y" ? angle * M_PI / 180. : 0.,
                           axis == "z" ? angle * M_PI / 180. : 0.,
                           rotCenter ) ;

    Writer<AffineTransformation3d> wri( outfile ) ;
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

