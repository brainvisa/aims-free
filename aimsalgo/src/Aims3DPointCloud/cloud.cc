

#include <cstdlib>
#include <aims/getopt/getopt2.h>
#include <aims/mesh/surfacegen.h>
#include <fstream>
#include <aims/mesh/surfaceOperation.h>

using namespace aims;
using namespace carto;
using namespace std;

AimsSurfaceTriangle 
icosahedronCloud( const std::vector<std::pair<Point3df,float> >  &cloud )
{
  
  std::vector<std::pair<Point3df,float> >::const_iterator ip,ep = cloud.end();
  Point3df   center;
  AimsSurfaceTriangle meshCloud;
  unsigned i = 0;
  float radius;

  for ( ip = cloud.begin(); ip != ep; ++ip , ++i ) 
    { 
      center = ip->first;
      radius = ip->second;
      AimsSurfaceTriangle	*mesh  = SurfaceGenerator::icosahedron(center,radius); 
      SurfaceManip::meshMerge(meshCloud , *mesh) ;
    }
  
  return (meshCloud);

}

int main( int argc, const char **argv )
{
  string			filein;
  Writer<AimsSurfaceTriangle>	meshout;
  AimsSurfaceTriangle mesh;
  std::vector<std::pair<Point3df,float> >  cloud ;
  float radius;
  float x,y,z;
  int i = 0;
      
  AimsApplication	app( argc, argv, "Generates cloud of 3D points" );
  app.addOption( filein, "-i", "input coordinates file " );
  app.alias( "--input", "-i" );
  app.addOption( meshout, "-o", "output mesh" );
  app.alias( "--output", "-o" );
  app.addOption( radius, "-r", "sphere radius" );
  app.alias( "--radius", "-r" );

  
  
  try
    {
      app.initialize();
  
      cout << "Reading coordinate file..." << flush;
      ifstream 	mf(filein.c_str());
      while ( mf && !mf.eof() )
	{
	  ++i;
	  mf >> x ;
	  mf >> y ;
	  mf >> z;
	  
	  cloud.push_back(std::pair<Point3df,float>(Point3df(x,y,z), radius ));
	}
      --i;
      cout << "done\n";
      cout << "Number of points : " << i << endl;
      cout << "Compute the clouds..." << flush ;
      mesh = icosahedronCloud(cloud);
      cout << "done\n";

      cout << "Writing mesh..." << flush ;
      meshout.write( mesh );
      cout << "done\n";
	    
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


