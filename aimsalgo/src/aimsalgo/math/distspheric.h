
#ifndef AIMS_MATH_DISTSPHERIC_H
#define AIMS_MATH_DISTSPHERIC_H

#include <aims/config/aimsalgo_config.h>
#include <vector>

template <class T,int D> class AimsVector;

/**@name Spheric distribution of points.*/
//@{
/**@name Continous distribution
         Returns a unit vector closed to another one in a solid angle.
         @param vec axis of the cone where to get another vector
         @param theta_max aperture of the cone
*/
//@{
///
AimsVector<float,3>  AimsPointInSolidAngle(const AimsVector<float,3>  &vec,
                                           float theta_max);
//@}

/**@name Discrete distribution.
         Returns a vector of uniformly distributed points on a unit sphere.
         The construction of that distribution starts from an icosahedron
         (20 facets, 12 points) and iterates a subdivision of each triangle
         into 4 sub-triangles.
         @param minpoint minimum number of points in the distribution
                         (12 at least) 
*/
//@{
///
std::vector<AimsVector<float,3>* >
AimsIcosahedronDiscreteSphericDistribution(int minpoint);
//@}

//@}

#endif
