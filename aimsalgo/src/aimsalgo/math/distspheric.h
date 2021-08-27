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
