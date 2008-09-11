/*
 *  Copyright (C) 1997-2005 CEA
 *
 *  This software and supporting documentation were developed by
 *  	CEA/DSV/SHFJ
 *  	4 place du General Leclerc
 *  	91401 Orsay cedex
 *  	France
 */

#include <aims/resampling/motion.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/io/motionR.h>
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
      Motion	motion;
      if( !fileTransf.empty() )
	{
	  MotionReader	mr( fileTransf );
	  mr.read( motion );
	}
      AimsData<float>	& r = motion.rotation();
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

