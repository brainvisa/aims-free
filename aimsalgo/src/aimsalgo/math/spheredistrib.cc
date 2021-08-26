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


// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/math/distspheric.h>
#include <aims/vector/vector.h>
#include <cartodata/volume/volume.h>
#include <aims/math/mathelem.h>
#include <aims/math/random.h>
#include <math.h>

using namespace aims;
using namespace carto;

AimsVector<float,3> AimsPointInSolidAngle(const AimsVector<float,3> &vec,
                                          float theta_max)
{ AimsVector<float,3> nvec(vec);
  float cos_theta1,sin_theta1,cos_theta2,sin_theta2,phi,theta;

  nvec /= norm(nvec);

  if ( (square(nvec.item(0))+square(nvec.item(1))) != 0 )
  { cos_theta1 = nvec.item(1) / sqrt(square(nvec.item(0))+square(nvec.item(1)));
    sin_theta1 = nvec.item(0) / sqrt(square(nvec.item(0))+square(nvec.item(1)));

    cos_theta2 = nvec.item(2) / sqrt(square(nvec.item(0)) +
                                     square(nvec.item(1)) +
                                     square(nvec.item(2)) );
    sin_theta2 = sqrt(square(nvec.item(0)) + square(nvec.item(1))) / 
                 sqrt(square(nvec.item(0)) +
                      square(nvec.item(1)) +
                      square(nvec.item(2)) );
    VolumeRef<float> kernel( 3, 3, 1, 1, AllocatorContext::fast() );
    kernel(0,0) = cos_theta1;
    kernel(0,1) = cos_theta2 * sin_theta1;
    kernel(0,2) = sin_theta2 * sin_theta1;
    kernel(1,0) = - sin_theta1;
    kernel(1,1) = cos_theta2 * cos_theta1;
    kernel(1,2) = sin_theta2 * cos_theta1;
    kernel(2,0) = 0;
    kernel(2,1) = - sin_theta2;
    kernel(2,2) = cos_theta2;

#if defined(__GNUC__) && defined (__sun__)
    theta = ::asin(sin(theta_max) * (2 * UniformRandom() - 1));
#else
    theta = asin(sin(theta_max) * (2 * UniformRandom() - 1));
#endif
    phi   = UniformRandom() * 2 * M_PI;

    VolumeRef<float> ran( 3, 1, 1, 1, AllocatorContext::fast() );
    ran(0) = cos(phi) * sin(theta);
    ran(1) = sin(phi) * sin(theta);
    ran(2) = cos(theta);
    VolumeRef<float> res = matrix_product( kernel, ran );
    return(AimsVector<float,3>(res(0),res(1),res(2))*norm(nvec));
  }
  else
  {
#if defined(__GNUC__) && defined (__sun__)
    theta = ::asin(sin(theta_max) * (2 * UniformRandom() - 1));
#else
    theta = asin(sin(theta_max) * (2 * UniformRandom() - 1));
#endif
    phi   = UniformRandom() * 2 * M_PI;
    AimsVector<float,3> ran;
    ran.item(0) = cos(phi) * sin(theta);
    ran.item(1) = sin(phi) * sin(theta);
    ran.item(2) = cos(theta);
    return(ran*norm(nvec));
  }
  return(AimsVector<float,3>(0,0,0));
}
