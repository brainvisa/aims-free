
#include <cstdlib>
#include <aims/data/data_g.h>
#include <aims/vector/vector.h>
#include <aims/math/math_g.h>
#include <aims/getopt/getopt2.h>
#include <iostream>
#include <list>
#include <stdlib.h>

using namespace aims;
using namespace carto;
using namespace std;

int main( int argc, const char **argv )
{ string fileOut;
  int direction = 0;
  bool all = false;
  float epsilon = 1e-6;

  AimsApplication app( argc, argv, "Output a file with coordinates of discrete spheric distribution" );
  app.addOption( direction, "-d", "number of discrete directions" );
  app.addOption( fileOut, "-o", "output ASCII file" );
  app.addOption( epsilon, "-e",
                 "error on dot product (+d).(-d) [default=1e-6]", true );
  app.addOption( all, "-a",
                 "all directions (+d and -d) (useful for DTI where +d and -d "
                 "are equivalent", true );
  app.alias( "--direction", "-d" );
  app.alias( "--output", "-o" );
  app.alias( "--epsilon", "-e" );
  app.alias( "--all", "-a" );

  try
  {
    app.initialize();

    vector< Point3df* > distrib;

    //
    // Generate full distribution
    //
    cout << "required directions : " << direction << endl;
    distrib = AimsIcosahedronDiscreteSphericDistribution( direction );
    cout << "real number of directions : " << distrib.size() << endl;
    cout << "all directions : " << ( all ? "yes" : "no" ) << endl;



    if ( all )
    {
      ofstream os( fileOut.c_str() );
      os << distrib.size() << endl;
      for (vector< Point3df* >::const_iterator it=distrib.begin();
          it!=distrib.end();it++)
        os << (*it)->item(0) << " "
          << (*it)->item(1) << " "
          << (*it)->item(2) << endl;
      os.close();
    }
    else
    {
      vector< Point3df* >::const_iterator it1,it2;

      it1=distrib.begin();
      bool oppositeExist=false;
      list< Point3df* > listPt;

      listPt.push_back( *it1 );

      for (it1++;it1!=distrib.end();it1++)
      { oppositeExist = false;
        for (it2=distrib.begin();it2!=it1;it2++)
          if ( fabs( fabs( (*it2)->dot( **it1 ) ) - 1 ) < epsilon )
          { oppositeExist = true;
            break;
          }
        if ( oppositeExist == false ) listPt.push_back( *it1 );
      }
      cout << "number of non-oriented directions : " << listPt.size() << endl;

      ofstream os( fileOut.c_str() );

      os << listPt.size() << endl;
      for (list< Point3df* >::const_iterator it=listPt.begin();
          it!=listPt.end();it++)
        os << (*it)->item(0) << " "
          << (*it)->item(1) << " "
          << (*it)->item(2) << endl;

      os.close();
    }

    for ( vector< Point3df* >::const_iterator it=distrib.begin();
          it!=distrib.end();it++ )
      delete *it;

    return EXIT_SUCCESS;

  }
  catch( user_interruption & )
  {
  }
  catch( exception & e )
  {
    cerr << e.what() << endl;
  }

  return EXIT_FAILURE;
}
