/* Copyright (c) 1995-2005 CEA
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
#include <aims/getopt/getoptProcess.h>
#include <aims/io/reader.h>
#include <aims/mesh/surfaceOperation.h>

using namespace aims;
using namespace carto;
using namespace std;

int main( int argc, const char **argv )
{
  Reader<AimsSurfaceTriangle>	filein;
  Writer<AimsSurfaceTriangle>	cutout, planeout;
  Writer<AimsTimeSurface<2, Void> >	borderout;
  float				a, b, c, d;

  AimsApplication	app( argc, argv, "Cuts a mesh by a plane" );
  app.addOption( filein, "-i", "input mesh" );
  app.alias( "--input", "-i" );
  app.addOption( cutout, "-o", "output cutout mesh" );
  app.alias( "--output", "-o" );
  app.addOption( a, "-a", "plane equation coef: a" );
  app.addOption( b, "-b", "plane equation coef: b" );
  app.addOption( c, "-c", "plane equation coef: c" );
  app.addOption( d, "-d", "plane equation coef: d" );
  app.addOption( planeout, "-p", "output plane intersection mesh", true );
  app.alias( "--plane", "-p" );
  app.addOption( borderout, "--border", "output border polygon", true );

  try
    {
      app.initialize();

      AimsSurfaceTriangle	inm, cutm, plm;
      AimsTimeSurface<2, Void>	border;
      filein.read( inm );

      Point4df	plane( a, b, c, d );

      SurfaceManip::cutMesh( inm, plane, cutm, plm, border );

      cutout.write( cutm );
      if( !planeout.fileName().empty() )
	planeout.write( plm );
      if( !borderout.fileName().empty() )
	borderout.write( border );
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


