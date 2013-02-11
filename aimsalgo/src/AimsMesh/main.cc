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
#include <aims/io/io_g.h>
#include <aims/getopt/getopt2.h>
#include <aims/utility/utility_g.h>
#include <aims/mesh/mesh_g.h>
#include <cartobase/stream/fileutil.h>

using namespace aims;
using namespace carto;
using namespace std;


int main(int argc, const char *argv[])
{
  try
    {
      string fileIn, fout;
      int labelInf = 1, labelSup = 32767;
      float minSurf = 0.0;
      bool smoothFlag = false;
      int smoothIt = 10;
      float smoothRate = 0.2;
      float smoothAngle = 180.0;
      int deciFlag = 0;
      float	deciReductionRate = 99.0;
      float	deciMaxClearance = 10.0;
      float	deciMaxError = 10.0;
      float	deciAngle = 180.0;
      bool splitFlag = false;
      bool ascii = false;
      int minFacetNumber = 50;

      AimsApplication	app( argc, argv, "Extracts triangulation in a label " 
                             "short volume" );
      app.addOption( fileIn, "-i", "input short volume label (level 0 is " 
                     "assumed to be the background)" );
      app.alias( "--input", "-i" );
      app.alias( "-input", "-i" );
      app.addOption( fout, "-o", "output mesh prefix name (extension will " 
                     "determine the format) [default=filein]", true );
      app.alias( "--output", "-o" );
      app.alias( "-output", "-o" );
      app.addOption( labelInf, "-l", "labelInf: extracts only label in " 
                     "[<labelInf>;<labelSup>]", true );
      app.alias( "--label", "-l" );
      app.alias( "-label", "-l" );
      app.addOption( labelSup, "-L", "labelSup: extracts only label in " 
                     "[<labelInf>;<labelSup>]", true );
      app.alias( "--Label", "-L" );
      app.alias( "-Label", "-L" );
      app.addOption( minSurf, "-S", " minimum surface of the interfaces to " 
                     "extract [default=0.0 mm^2]", true );
      app.alias( "--Surface", "-S" );
      app.alias( "-Surface", "-S" );
      app.addOption( smoothFlag, "--smooth", "smoothes the mesh [default=no " 
                     "smoothing]", true );
      app.addOption( smoothIt, "--smoothIt", "smoothing number of iterations " 
                     "[default=10]", true );
      app.addOption( smoothRate, "--smoothRate", "smoothing moving factor at " 
                     "each iteration [default=0.2]", true );
      app.addOption( smoothAngle, "--smoothAngle", "smoothing feature angle " 
                     "in degrees between 0 and 180 degrees, [default=180]", 
                     true );
      app.addOption( deciFlag, "--decimation", "decimate the mesh " 
                     "[default=no decimation]", true );
      app.addOption( deciReductionRate, "--deciReductionRate", 
                     "expected % decimation reduction rate [default=99%]", 
                     true );
      app.addOption( deciMaxClearance, "--deciMaxClearance", 
                     "maximum clearance of the deci", true );
      app.addOption( deciMaxError, "--deciMaxError", 
                     "maximum error distance from the original mesh (mm) " 
                     " [default=10.0]", true );
      app.addOption( deciAngle, "--deciAngle", "feature angle (degrees), " 
                     "between 0 and 180 [default=180]", true );
      app.addOption( splitFlag, "--split", "split the surface corresponding " 
                     "to one connected component into sub-surfaces, each " 
                     "corresponding to an interface between 2 different " 
                     "components [default=no splitting]", true );
      app.addOption( minFacetNumber, "--minFacetNumber", 
                     "minimum number of facets to allow decimation " 
                     "[default=50]", true );

      app.initialize();

      ASSERT( labelInf >= 0 && labelInf <= labelSup && labelSup <= 32767);
      ASSERT( minSurf >= 0.0 );
      ASSERT( smoothIt > 0 );
      ASSERT( smoothAngle >= 0.0 && smoothAngle <= 180.0 );
      ASSERT( smoothRate >= 0.0 && smoothRate <= 1.0 );
      ASSERT( deciReductionRate >= 0.0 && deciReductionRate <= 100.0 );
      ASSERT( deciMaxClearance >= 0.0 );
      ASSERT( deciMaxError >= 0.0 );
      ASSERT( deciAngle >= 0.0 && deciAngle <= 180.0 );

      if ( fout.empty() )
        {
          fout = fileIn;
          string	bn = FileUtil::basename( fout );
          string::size_type	pos = bn.rfind( '.' );
          if( pos != string::npos )
            fout.erase( fout.length() - bn.length() + pos, bn.length() - pos );
        }

      //
      // load label volume
      //
      cout << endl << "reading image      : " << flush;

      AimsData<short> data;
      Reader<AimsData<short> > dataR( fileIn );
      dataR.read( data, 1 );

      cout << "done" << endl;
      cout << "resolutions        : ( ";
      cout << data.sizeX() << " mm, ";
      cout << data.sizeY() << " mm, ";
      cout << data.sizeZ() << " mm )" << endl;
      data.fillBorder( -1 );

      Mesher mesher;

      mesher.setLabelInf( labelInf );
      mesher.setLabelSup( labelSup );
      cout << "label inferior     : " << labelInf << endl;
      cout << "label superior     : " << labelSup << endl;

      if ( smoothFlag )
        mesher.setSmoothing( smoothAngle, smoothIt, 
                             smoothRate, smoothRate, smoothRate );
      if ( deciFlag )
        mesher.setDecimation( deciReductionRate, deciMaxClearance,
                              deciMaxError, deciAngle );
      if ( splitFlag )
        mesher.setSplitting();

      mesher.setMinFacetNumber( minFacetNumber );
      mesher.setMinSurface( minSurf );

      if ( ascii )
        mesher.doit( data, fout, "ascii" );
      else
        mesher.doit( data, fout, "binar" );
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


