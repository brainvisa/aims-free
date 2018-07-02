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


#ifndef AIMS_RESAMPLING_NEARESTNEIGHBORRESAMPLER_D_H
#define AIMS_RESAMPLING_NEARESTNEIGHBORRESAMPLER_D_H

#include <aims/resampling/nearestneighborresampler.h>

#include <cmath>

using namespace std;

namespace aims
{

template <class T>
void
NearestNeighborResampler<T>::doResample( const AimsData< T > &inVolume,
                                         const aims::Transformation3d &invTransform3d,
                                         const T &outBackground,
                                         const Point3df &outLocation,
                                         T &outValue, int t ) const
{

  Point3df inLocation = invTransform3d.transform( outLocation );

  doResample(inVolume, inLocation, outBackground, outValue, t);
}

template <class T>
void
NearestNeighborResampler<T>::doResample( const AimsData< T > &inVolume,
                                         const Point3df &inLocation,
                                         const T &outBackground,
                                         T &outValue, int t ) const
{
  float xf = std::round(inLocation[0]);
  float yf = std::round(inLocation[1]);
  float zf = std::round(inLocation[2]);

  // The test is done using floating-point so that NaN values are excluded (the
  // background value is returned if the transformation yields NaN)
  if ( ( xf >= 0 ) && ( xf < inVolume.dimX() ) &&
       ( yf >= 0 ) && ( yf < inVolume.dimY() ) &&
       ( zf >= 0 ) && ( zf < inVolume.dimZ() ) )
  {
    int x = static_cast<int>(xf);
    int y = static_cast<int>(yf);
    int z = static_cast<int>(zf);
    outValue = inVolume( x, y, z, t );
  }
  else
  {
    outValue = outBackground;
  }

}

} // namespace aims

#endif
