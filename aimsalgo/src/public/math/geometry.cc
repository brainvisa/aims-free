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


#include <aims/math/geometry.h>
#include <aims/vector/vector.h>
#include <aims/data/fastAllocationData.h>
#include <aims/math/mathelem.h>
#include <aims/math/random.h>
#include <aims/math/gausslu.h>
#include <aims/math/trieder.h>
#include <iostream>
#include <math.h>

using namespace std;
using namespace aims;

float Aims2LinesAngle( const Point3df& v1, const Point3df& v2 )
{
  float dp = v1.dot( v2 ) / ( norm( v1 ) * norm( v2 ) );
  dp = max ( 0.0f, 1 - dp * dp );
#if defined(__GNUC__) && defined (__sun__)
  return fabs( ::asin( sqrt( dp ) ) );
#else
  return fabs( asin( sqrt( dp ) ) );
#endif
}


float Aims2VectorsAngle( const Point3df& v1, const Point3df& v2 )
{
#if defined(__GNUC__) && defined (__sun__)
  return ::acos( v1.dot( v2 ) / ( norm( v1 ) * norm( v2 ) ) );
#else
  return acos( v1.dot( v2 ) / ( norm( v1 ) * norm( v2 ) ) );
#endif
}


Trieder AimsOrthonormalTrieder(const AimsVector<float,3>& v1)
{ Trieder trieder;

  trieder.dirX() = v1 / norm(v1);

  if (v1.item(0)!=0 && v1.item(2)!=1)
  { trieder.dirY().item(2) = 
       UniformRandom(-(float)sqrt(1.0 - square(trieder.dirX().item(2))),
                     +(float)sqrt(1.0 - square(trieder.dirX().item(2))));
    trieder.dirY().item(1) = (-2 * trieder.dirX().item(1) * 
                                   trieder.dirX().item(2) * 
                                   trieder.dirY().item(2) + 
                               sqrt(4.0 * square(trieder.dirX().item(0)) * 
                                   (-square(trieder.dirY().item(2)) + 
                               (1.0 - square(trieder.dirX().item(2))) )) ) /
                               (2.0 * (1.0 - square(trieder.dirX().item(2))));
    trieder.dirY().item(0) = -(trieder.dirX().item(1) * trieder.dirY().item(1) +
                               trieder.dirX().item(2) * trieder.dirY().item(2))/
                               trieder.dirX().item(0);
  }
  else if (trieder.dirX().item(0)==0 && trieder.dirX().item(1)!=0)
  { trieder.dirY().item(2) = UniformRandom(-trieder.dirX().item(1),
                                            trieder.dirX().item(1));
    trieder.dirY().item(1) = - trieder.dirX().item(2) * trieder.dirY().item(2) /
                               trieder.dirX().item(1);
    trieder.dirY().item(0) = + sqrt(1.0 - square(trieder.dirY().item(2)) /
                               square(trieder.dirX().item(1)));
  }
  else if ((trieder.dirX().item(0)==0 && trieder.dirX().item(1)==0) ||
            trieder.dirX().item(2)==1)
  { float theta = UniformRandom(0.0,2.0 * (float)M_PI);
    trieder.dirY().item(2) = 0.0;
    trieder.dirY().item(1) = cos(theta);
    trieder.dirY().item(0) = sin(theta);
  }
  else
  { cout << "v1 = " << trieder.dirX() << endl;
    AimsError("Trieder \
               AimsOrthonormalTrieder(const AimsVector<float,3>&) : bad case");
  }

  trieder.dirZ() = crossed(trieder.dirX(),trieder.dirY());
  
  return(trieder);
}



bool AimsIsInCylinder(const AimsVector<float,3>& p,
                      const AimsVector<float,3>& p0,
                      const AimsVector<float,3>& dir,
                      float radius,float length)
{ bool answer = false;
  AimsVector<float,3> u = dir / norm(dir);

  float lambda = (p.item(0) - p0.item(0)) * u.item(0) +
                 (p.item(1) - p0.item(1)) * u.item(1) +
                 (p.item(2) - p0.item(2)) * u.item(2);

  if (lambda<0.0 || lambda>length) answer = false;
  else
  { AimsVector<float,3> h = p0 + u * lambda;
    float hm = norm(p - h);
    if (hm <= radius) answer = true;
    else              answer = false;
  }
  return(answer);
}


bool AimsIsInTorus(const AimsVector<float,3>& p,
                   const AimsVector<float,3>& p0,
                   const AimsVector<float,3>& axe,
                   float Radius,float radius,
                   AimsVector<float,3>* dir)
{ bool answer = false;
  Trieder trieder;
  AimsVector<float,3> om,h,oh,q;
  float lambda,gamma;

  trieder = AimsOrthonormalTrieder(axe / norm(axe));
  om = p - p0;
  lambda = om.dot(trieder.dirY());
  gamma  = om.dot(trieder.dirZ());
  h      = p0 + trieder.dirY() * lambda + trieder.dirZ() * gamma;  
  oh     = h - p0;
  q      = p0 + oh * (Radius / norm(oh));
  if (norm(p-q) < radius)
  { answer = true;
    if (dir != NULL) *dir = crossed(trieder.dirX(),(q-p0) / norm(q-p0));
  }

  return(answer);
}


bool AimsIsInCube(const AimsVector<float,3>& p,
                  const AimsVector<float,3>& p0,
                  const AimsVector<float,3>& p1)
{ ASSERT(p0.item(0)<=p1.item(0) && 
         p0.item(1)<=p1.item(1) && 
         p0.item(2)<=p1.item(2)  );
  if (p.item(0)>=p0.item(0) && p.item(0)<=p1.item(0) &&
      p.item(1)>=p0.item(1) && p.item(1)<=p1.item(1) &&
      p.item(2)>=p0.item(2) && p.item(2)<=p1.item(2)  ) return(true);
  else return(false); 
}


bool AimsIsInParallelepipede(const AimsVector<float,3>& p,
                             const AimsVector<float,3>& p0,
                             const AimsVector<float,3>& v1,
                             const AimsVector<float,3>& v2,
                             float L1,float L2,float L3)
{ AimsVector<float,3> V1,V2,V3;
  AimsFastAllocationData<float> mat(3,3);
  AimsFastAllocationData<float> op(3),lambda;

  op(0) = p.item(0) - p0.item(0);
  op(1) = p.item(1) - p0.item(1);
  op(2) = p.item(2) - p0.item(2);

  V1 = v1 / norm(v1);
  V2 = v2 / norm(v2);
  V3 = crossed(V1,V2);
  V3 /= norm(V3);

  // plan (O,V1,V2)
  mat(0,0) = V1.item(0);  mat(0,1) = V2.item(0);  mat(0,2) = -V3.item(0);
  mat(1,0) = V1.item(1);  mat(1,1) = V2.item(1);  mat(1,2) = -V3.item(1);
  mat(2,0) = V1.item(2);  mat(2,1) = V2.item(2);  mat(2,2) = -V3.item(2);
  lambda = AimsLinearResolutionLU(mat,op);
  if (lambda(0) < 0 || lambda(0) > L1 ||
      lambda(1) < 0 || lambda(1) > L2  )  return(false);

  // plan (O,V1,V3)
  mat(0,0) = V1.item(0);  mat(0,1) = V3.item(0);  mat(0,2) = -V2.item(0);
  mat(1,0) = V1.item(1);  mat(1,1) = V3.item(1);  mat(1,2) = -V2.item(1);
  mat(2,0) = V1.item(2);  mat(2,1) = V3.item(2);  mat(2,2) = -V2.item(2);
  lambda = AimsLinearResolutionLU(mat,op);
  if (lambda(0) < 0 || lambda(0) > L1 ||
      lambda(1) < 0 || lambda(1) > L3  )  return(false);

  // plan (O,V1,V2)
  mat(0,0) = V2.item(0);  mat(0,1) = V3.item(0);  mat(0,2) = -V1.item(0);
  mat(1,0) = V2.item(1);  mat(1,1) = V3.item(1);  mat(1,2) = -V1.item(1);
  mat(2,0) = V2.item(2);  mat(2,1) = V3.item(2);  mat(2,2) = -V1.item(2);
  lambda = AimsLinearResolutionLU(mat,op);
  if (lambda(0) < 0 || lambda(0) > L2 ||
      lambda(1) < 0 || lambda(1) > L3  )  return(false);

  return(true);
}



