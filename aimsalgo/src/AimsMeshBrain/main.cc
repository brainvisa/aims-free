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
#include <aims/data/data_g.h>
#include <aims/io/io_g.h>
#include <aims/getopt/getopt2.h>
#include <aims/utility/utility_g.h>
#include <aims/mesh/mesh_g.h>

using namespace aims;
using namespace carto;
using namespace std;

#define NLABEL 32767


int main( int argc, const char** argv )
{
  try
    {
    string fileIn, fileOut;
    float deciMaxClearance = 5.0;
    float	deciMaxError = 3.0;
    uint minFacetNumber = 50;
    bool asciiFlag = false;
    bool intinterface = false;

    AimsApplication app( argc, argv, "Computes the brain surface from a "
      "segmented image and saves it in a triangles mesh format file" );
    app.addOption( fileIn, "-i", "input volume label (short int data), "
      "level 0 is assumed to be the back" );
    app.alias( "--input", "-i" );
    app.alias( "-input", "-i" );
    app.addOption( fileOut, "-o", "output file name for the mesh "
      "[default: same basename as input]", true );
    app.alias( "--output", "-o" );
    app.alias( "-output", "-o" );
    app.addOption( deciMaxClearance, "--deciMaxClearance",
      "maximum clearance expected in the resulting mesh, in mm [default= 5]",
      true );
    app.addOption( deciMaxError, "--deciMaxError",
      "maximum error distance from the original data, in mm [default = 3]",
      true );
    app.addOption( minFacetNumber, "--minFacetNumber",
      "minimum number of facets to allow decimation [default=50]", true );
    app.addOption( intinterface, "--internalinterface",
      "mesh the internal interface of the main object [default=false]", true );
    app.addOption( asciiFlag, "--ascii", "write file in ASCII mode if the "
      "format supports it [default=binary]", true );

    app.initialize();

    ASSERT( deciMaxClearance >= 0.0 );
    ASSERT( deciMaxError >= 0.0 );

    string	fout;
    if ( fileOut.empty() )
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
    mesher.getBrain( data, surface, intinterface );

    // compute the surface mesh
    cout << "surface            : " << flush
        << mesher.surfaceOfInterface( surface ) << " mm2" << endl;

    // save the triangulation
    Writer<AimsSurfaceTriangle> surfaceW( fout );
    surfaceW.write( surface, asciiFlag );
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
