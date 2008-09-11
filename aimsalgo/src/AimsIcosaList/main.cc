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


#include <aims/data/data_g.h>
#include <aims/vector/vector.h>
#include <aims/math/math_g.h>
#include <aims/getopt/getopt.h>
#include <iostream>
#include <list>
#include <stdlib.h>

using namespace std;

BEGIN_USAGE(usage)
  "-------------------------------------------------------------------------",
  "AimsIcosaList  -d[irection] <dir>                                        ",
  "               -o[utput] <output>                                        ",
  "               [-e[psilon] <eps>]                                        ",
  "               [-a[ll]]                                                  ",
  "               [-h[elp]]                                                 ",
  "-------------------------------------------------------------------------",
  " Output a file with coordinates of discrete spheric distribution         ",
  "-------------------------------------------------------------------------",
  "     dir           : number of discrete directions                       ",
  "     output        : output ASCII file                                   ",
  "     all           : all directions (+d and -d)                          ",
  "                     (useful for DTI where +d and -d are equivalent      ",
  "     eps           : error on dot product (+d).(-d) [default=1e-6]       ",
  "-------------------------------------------------------------------------",
END_USAGE



void Usage( void )
{
  AimsUsage( usage );
}



int main(int argc,char **argv)
{ char* fileOut = NULL;
  int direction = 0;
  int all = 0;
  float epsilon = 1e-6;

  //
  // Parser of options
  //
  AimsOption opt[] = {
  { 'h',"help"         ,AIMS_OPT_FLAG  ,( void* )Usage            ,AIMS_OPT_CALLFUNC,0},
  { 'd',"direction"    ,AIMS_OPT_INT   ,&direction       ,0                ,1},
  { 'o',"output"       ,AIMS_OPT_STRING,&fileOut         ,0                ,1},
  { 'e',"epsilon"      ,AIMS_OPT_FLOAT ,&epsilon         ,0                ,0},
  { 'a',"all"          ,AIMS_OPT_FLAG  ,&all             ,0                ,0},
  { 0  ,0              ,AIMS_OPT_END   ,0                ,0                ,0}};

  AimsParseOptions( &argc, argv, opt, usage );

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
    ofstream os( fileOut );
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

    ofstream os( fileOut );

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
