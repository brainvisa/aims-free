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

// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <cstdlib>
#include <aims/transformation/affinetransformation3d.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/getopt/getopt2.h>
#include <aims/mesh/surface.h>
#include <aims/mesh/surfaceOperation.h>

using namespace aims;
using namespace carto;
using namespace std;


int main( int argc, const char **argv )
{
  try
    {
      Reader<AimsSurfaceTriangle>	rmesh;
      Writer<AimsSurfaceTriangle>	wmesh;
      string				fileTransf;
      bool				ascii = false;
      float				scaling[ 3 ] = { 1.0, 1.0, 1.0 };

      AimsApplication	app( argc, argv, 
			     "Applies a transformation to a mesh" );
      app.addOption( rmesh, "-i", "input mesh" );
      app.alias( "--input", "-i" );
      app.addOption( wmesh, "-o", 
		     "output (transformed) mesh [default: same as input]", 
                     true );
      app.alias( "--output", "-o" );
      app.addOption( fileTransf, "-t", "transformation file *.trm", true );
      app.alias( "--transform", "-t" );
      app.addOption( ascii, "-a", "write in ascii format", true );
      app.alias( "--ascii", "-a" );
      app.addOption( scaling[0], "--scaleX", "specify X scale factor", true );
      app.addOption( scaling[1], "--scaleY", "specify Y scale factor", true );
      app.addOption( scaling[2], "--scaleZ", "specify Z scale factor", true );

      app.initialize();

      if( wmesh.fileName().empty() )
        wmesh.setFileName( rmesh.fileName() );

      // read triangulation
      cout << "reading triangulation   : " << flush;
      AimsSurfaceTriangle surface;
      rmesh >> surface;

      // loading transformation
      AffineTransformation3d	motion;
      if( !fileTransf.empty() )
      {
        Reader<AffineTransformation3d>	mr( fileTransf );
        mr.read( motion );
      }
      VolumeRef<float> r = motion.affine();
      r(0,0) *= scaling[0];
      r(1,0) *= scaling[0];
      r(2,0) *= scaling[0];
      r(0,1) *= scaling[1];
      r(1,1) *= scaling[1];
      r(2,1) *= scaling[1];
      r(0,2) *= scaling[2];
      r(1,2) *= scaling[2];
      r(2,2) *= scaling[2];

      // processing transformation
      cout << "processing transformation : " << flush;
      SurfaceManip::meshTransform( surface, motion );
      cout << "done" << endl;

      //
      // save triangulation
      //
      cout << "saving mesh    : " << flush;
      wmesh.write( surface, ascii );
      cout << "done" << endl;

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

