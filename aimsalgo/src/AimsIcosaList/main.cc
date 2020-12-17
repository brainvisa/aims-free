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
