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
#include <aims/data/data.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/getopt/getopt2.h>
#include <aims/utility/utility_g.h>
#include <aims/mesh/surface.h>
#include <aims/mesh/mesher.h>

using namespace aims;
using namespace carto;
using namespace std;

#define NLABEL 32767

int main( int argc, const char* argv[] )
{
  string fileIn, fileOut;
  float deciMaxClearance = 3.0;
  float	deciMaxError = 10.0;
  int minFacetNumber = 50;
  bool asciiFlag = false;

  AimsApplication app( argc, argv,
    "Computes the brain surface from a segmented image as a mesh" );
  app.addOption( fileIn, "-i",
    "input short volume label (level 0 is assumed to be the background)" );
  app.alias( "--input", "-i" );
  app.addOption( fileOut, "-o", "output mesh filename "
    "[default=input + mesh extension]", true );
  app.alias( "--output", "-o" );
  app.addOption( deciMaxClearance, "--deciMaxClearance",
    "maximum clearance expected in the resulting mesh in the decimation "
    "process, in mm [default= 5 mm]", true );
  app.addOption( deciMaxError, "--deciMaxError",
    "maximum expected error distance from the original data in the decimation "
    "process, in mm [default = 3 mm]", true );
  app.addOption( minFacetNumber, "--minFacetNumber",
    "minimum number of facets to allow decimation [default=50]", true );
  app.addOption( asciiFlag, "--ascii",
    "write mesh file in ASCII mode if supported by the format "
    "[default=binary]", true );

  try
  {
    app.initialize();

    ASSERT( deciMaxClearance >= 0.0 );
    ASSERT( deciMaxError >= 0.0 );

    string	fout;
    if( fileOut.empty() )
    {
      fout = fileIn;
      string::size_type	pos = fout.rfind( '.' );
      string::size_type	pos2 = fout.rfind( '/' );
      if( pos != string::npos && ( pos2 == string::npos || pos2 < pos ) )
        fout.erase( pos, fout.length() - pos );
    }
    else
      fout = fileOut;

    //
    // load label volume
    //
    cout << "reading image      : " << flush;
    AimsData<short> data;
    Reader<AimsData<short> > dataR( fileIn );
    dataR.read( data, 1 );
    cout << "done" << endl;

    cout << "resolutions        : (";
    cout << data.sizeX() << " mm,";
    cout << data.sizeY() << " mm,";
    cout << data.sizeZ() << " mm)" << endl;
    data.fillBorder(-1);

    Mesher mesher;
    mesher.setDecimation( 100.0, deciMaxClearance, deciMaxError, 180.0 );
    mesher.setMinFacetNumber( minFacetNumber );
    AimsSurfaceTriangle surface;
    mesher.getWhite( data, surface );

    // compute the surface mesh
    cout << "surface            : " << flush
        << mesher.surfaceOfInterface( surface ) << " mm2" << endl;

    // save the triangulation
    Writer<AimsSurfaceTriangle> surfaceW( fout );
    cout << "saving mesh " << fout << "...\n";
    surfaceW.write( surface, asciiFlag );
    cout << "done\n";
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
