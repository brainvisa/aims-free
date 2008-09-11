/* Copyright (c) 1995-2007 CEA
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


