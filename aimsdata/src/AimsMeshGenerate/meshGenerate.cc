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
#include <aims/mesh/surfacegen.h>
#include <soma-io/reader/pythonreader.h>

using namespace aims;
using namespace carto;
using namespace std;

int main( int argc, const char **argv )
{
  string			filein;
  Writer<AimsSurfaceTriangle>	meshout;

  AimsApplication	app( argc, argv, "Generates a common-shaped mesh" );
  ostringstream	sg;
  sg << "input parameter file (.minf-like)\nPossible types and parameters:\n";
  SurfaceGenerator::printDescription( sg );
  SurfaceGenerator::printDescription_wireframe( sg );
  app.addOption( filein, "-i", sg.str() );
  app.alias( "--input", "-i" );
  app.addOption( meshout, "-o", "output mesh" );
  app.alias( "--output", "-o" );

  try
    {
      app.initialize();

      PythonReader	pr( filein );
      AttributedObject	params( "AimsMeshGenerate" );
      pr.read( params );

      try
      {
        AimsSurfaceTriangle	*mesh = SurfaceGenerator::generate( params );

        meshout.write( *mesh );
      }
      catch( runtime_error & )
      {
        // try wireframe objects
        AimsTimeSurface<2, Void> *mesh = SurfaceGenerator::generate_wireframe(
          params );
        Writer<AimsTimeSurface<2, Void> > w( meshout.fileName() );
        w.write( *mesh );
      }
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


