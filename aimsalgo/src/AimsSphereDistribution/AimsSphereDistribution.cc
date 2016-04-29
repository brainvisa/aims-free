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
  const double minimal_step=1.e-10;

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

