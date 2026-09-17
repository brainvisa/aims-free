
#include <cstdlib>
#include <aims/getopt/getopt2.h>
#include <aims/points_distribution/points_distribution.h>
#include <time.h>


using namespace std;
using namespace aims;
using namespace carto;


inline double frand(void){return ((rand()-(RAND_MAX/2))/(RAND_MAX/2.));}


int main( int argc, const char **argv )
{
  string fileOut;
  int direction = 0;
  int Nstep = 1000;
  double step=0.01, radius = 1;

  AimsApplication app( argc, argv,
                       "Output a file with coordinates of discrete spheric "
                       "distribution with any number of directions." );

  app.addOption( direction, "-d", "number of directions" );
  app.alias( "--direction", "-d" );
  app.addOption( fileOut, "-o", "output file name (default: standard output)",
                 true );
  app.alias( "--output", "-o" );
  app.addOption( Nstep, "-s",
                 "maximum number of minimization steps (default 1000)",
                 true );
  app.alias( "--steps", "-s" );
  app.addOption( radius, "-r", "radius of the sphere (default : 1)", true );
  app.alias( "--radius", "-r" );

  try
  {
    app.initialize();

    vector< Point3df > p0( direction );

    srand( time( NULL ) );

    for( int i = 0; i < direction; ++i ) {
      p0[ i ][ 0 ] = 2 * frand();
      p0[ i ][ 1 ] = 2 * frand();
      p0[ i ][ 2 ] = 2 * frand();
      double l = p0[ i ].dnorm();
      if( l != 0.0 ) {
        p0[ i ][ 0 ] /= l;
        p0[ i ][ 1 ] /= l;
        p0[ i ][ 2 ] /= l;
      } else {
        --i;
      }
    }

    vector<Point3df> *p1 = PointsDistribution().distribute( p0, Nstep, step );

    ostream *out;
    if ( !fileOut.empty() ) {
      out = new ofstream( fileOut.c_str() );
    } else {
      out = &cout;
    }
    *out << direction << endl;
    for( int i = 0; i < direction; ++i ) {
      *out << radius * (*p1)[i][0] << " " << radius * (*p1)[i][1] << " "
        << radius * (*p1)[i][2] << endl;
    }
    if ( !fileOut.empty() ) {
      delete out;
    }
    delete p1;
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

