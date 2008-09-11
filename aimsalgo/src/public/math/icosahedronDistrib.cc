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


#include <aims/math/distspheric.h>
#include <aims/data/data.h>
#include <aims/vector/vector.h>
#include <map>
#include <math.h>

using namespace std;

struct my_comp : public binary_function<Point3df,Point3df,
                                        bool>
{ bool operator() (const Point3df& pt1,
                   const Point3df& pt2) const
  { return((pt1.item(0) + pt1.item(1)*10 + pt1.item(2)*100) < 
           (pt2.item(0) + pt2.item(1)*10 + pt2.item(2)*100));
  }
};


typedef map<Point3df,Point3df*,my_comp > MapOfPoint;


#define X .525731112119133606
#define Z .850650808352039932


static void subdivide(Point3df* v1,
                      Point3df* v2,
                      Point3df* v3,
                      int depth,
                      MapOfPoint& mp)
{ if (depth == 0) return;

  Point3df *v12,*v23,*v31;
  MapOfPoint::iterator it;

  v12 = new Point3df(*v1 + *v2);
  *v12 /= norm( *v12 );
  if ((it = mp.find(*v12)) != mp.end())
  { delete(v12);
    v12 = (Point3df*)(*it).second;
  }
  else mp[*v12] = v12;

  v23 = new Point3df(*v2 + *v3);
  *v23 /= norm( *v23 );
  if ((it = mp.find(*v23)) != mp.end())
  { delete(v23);
    v23 = (Point3df*)(*it).second;
  }
  else mp[*v23] = v23;

  v31 = new Point3df(*v3 + *v1);
  *v31 /= norm( *v31 );
  if ((it = mp.find(*v31)) != mp.end())
  { delete(v31);
    v31 = (Point3df*)(*it).second;
  }
  else mp[*v31] = v31;

  subdivide(v1 ,v12,v31,depth-1,mp);
  subdivide(v2 ,v23,v12,depth-1,mp);
  subdivide(v3 ,v31,v23,depth-1,mp);
  subdivide(v12,v23,v31,depth-1,mp);
}


AIMSALGOPUB_API vector<Point3df*> 
AimsIcosahedronDiscreteSphericDistribution(int minpoint)
{ MapOfPoint mp;

  // number of triangles at iteration i : 20.4^i
  // number of points at iteration i     : 2 + 10.4^i
  int depth = (int)(::log(((double)minpoint- 2 ) / 10) / ::log(4) + 1);

  //cout << "depth = " << depth << endl << flush;

  Point3df* vdata[12];
  vdata[0]  = new Point3df(-X,0.0,Z);
  vdata[1]  = new Point3df(X,0.0,Z);
  vdata[2]  = new Point3df(-X,0.0,-Z);
  vdata[3]  = new Point3df(X,0.0,-Z);
  vdata[4]  = new Point3df(0.0,Z,X);
  vdata[5]  = new Point3df(0.0,Z,-X);
  vdata[6]  = new Point3df(0.0,-Z,X);
  vdata[7]  = new Point3df(0.0,-Z,-X);
  vdata[8]  = new Point3df(Z,X,0.0);
  vdata[9]  = new Point3df(-Z,X,0.0);
  vdata[10] = new Point3df(Z,-X,0.0);
  vdata[11] = new Point3df(-Z,-X,0.0);

  int	i;
  for(i=0;i<12;i++)
    mp[*vdata[i]] = vdata[i];


  int  tindices[20][3] = {{0,4,1} ,{0,9,4} ,{9,5,4} ,{4,5,8} ,{4,8,1},
                           {8,10,1},{8,3,10},{5,3,8} ,{5,2,3} ,{2,7,3},
                           {7,10,3},{7,6,10},{7,11,6},{11,0,6},{0,1,6},
                           {6,1,10},{9,0,11},{9,11,2},{9,2,5} ,{7,2,11}};

   for (i=0;i<20;i++)
     subdivide(vdata[tindices[i][0]],
               vdata[tindices[i][1]],
               vdata[tindices[i][2]],depth,mp);

  vector< Point3df* > vec(mp.size());
  vector< Point3df* >::iterator it;
  for (it=vec.begin();it!=vec.end();it++)
  { *it = (Point3df*)(*mp.begin()).second;
    mp.erase(mp.begin());
  }
  return(vec);
}
