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

template <class T>
void 
NearestNeighborResampler<T>::doResample( const AimsData< T > &inVolume, 
                                         const Motion &invTransform3d, 
                                         const T &outBackground, 
                                         const Point3df &outLocation, 
                                         T &outValue, int t )
{

  Point3df inLocation = invTransform3d.transform( outLocation );

  doResample(inVolume, inLocation, outBackground, outValue, t);
}

template <class T>
void 
NearestNeighborResampler<T>::doResample( const AimsData< T > &inVolume, 
                                         const Point3df &inLocation, 
                                         const T &outBackground, 
                                         T &outValue, int t )
{
  Point3df normalizedInLocation = inLocation;
  normalizedInLocation[0] += 0.5;
  normalizedInLocation[1] += 0.5;
  normalizedInLocation[2] += 0.5;

  int x = ( int )normalizedInLocation[0];
  int y = ( int )normalizedInLocation[1];
  int z = ( int )normalizedInLocation[2];

  if ( ( normalizedInLocation[0] < 0.0 ) &&
       ( ( double )x != normalizedInLocation[0] ) )
  {

    -- x;

  }
  if ( ( normalizedInLocation[1] < 0.0 ) &&
       ( ( double )y != normalizedInLocation[1] ) )
  {

    -- y;

  }
  if ( ( normalizedInLocation[2] < 0.0 ) &&
       ( ( double )z != normalizedInLocation[2] ) )
  {

    -- z;

  }
    
  if ( ( x >= 0 ) && ( x < inVolume.dimX() ) &&
       ( y >= 0 ) && ( y < inVolume.dimY() ) &&
       ( z >= 0 ) && ( z < inVolume.dimZ() ) )
  {

    outValue = inVolume( x, y, z, t );

  }
  else
  {

    outValue = outBackground;

  }

}

#endif

