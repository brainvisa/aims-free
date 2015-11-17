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
#include <cartobase/config/paths.h>
#include <aims/resampling/motion.h>
#include <aims/io/motionW.h>
#include <aims/graph/graphmanip.h>
#include <graph/graph/graph.h>
#include <graph/graph/greader.h>
#include <cartobase/object/sreader.h>
#include <cartobase/stream/fileutil.h>

using namespace aims;
using namespace carto;
using namespace std;


int main( int argc, const char **argv )
{
  string	filein, fileout;

  AimsApplication	app( argc, argv, 
			     "Extracts the Talairach transformation in a graph"
			     " file and write it as an Aims/Anatomist "
			     "transformation file" );

  app.addOption( filein, "-i", "input data graph" );
  app.alias( "-input", "-i" );
  app.addOption( fileout, "-o", "output transformation (default: "
		 "<input>TOtalairach.trm)", true );
  app.alias( "-output", "-o" );

  Graph		g( "ClusterArg" );
  SyntaxSet	syntax;

  try
    {
      app.initialize();

      if( fileout.empty() )
	{
	  fileout = filein;
	  string::size_type p1 = fileout.rfind( '.' );
	  if( p1 != string::npos )
	    {
	      string::size_type p2 = fileout.rfind( '/' );
	      if( p2 == string::npos || p2 < p1 )
		fileout.erase( p1, fileout.length() - p1 );
	    }
	  fileout += "TOtalairach.trm";
	}

      // syntax
      SyntaxReader	sr( Paths::findResourceFile(
                            "nomenclature/syntax/graph.stx", "aims" ) );
      sr >> syntax;

      // read
      GraphReader	grd( filein, syntax );
      grd >> g;

      // read transformation
      Motion	m = GraphManip::talairach( g );

      MotionWriter	w( fileout );
      w.write( m );
    }
  catch( user_interruption &e )
    {
    }
  catch( exception & e )
    {
      cerr << e.what() << endl;
      return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
