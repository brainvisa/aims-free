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


#ifndef AIMS_DISTANCEMAP_FRONT_H
#define AIMS_DISTANCEMAP_FRONT_H

#include <aims/config/aimsalgo_config.h>

template <typename T> class AimsData;
namespace aims
{
  template <typename T> class BucketMap;
}


template <typename T> void 
AimsDistanceFrontPropagation( AimsData<T>& vol, T val_domain, T val_outside,
                              int xm, int ym, int zm, float mult_factor,
                              bool divide );

/** Chamfer front propagation distance map on a bucket
\param vol both input and output
\param val_domain value where the distance can propagate in the input bucket
\param val_outside voxels of this value are ignored, the front cannot 
  propagate on this value. In buckets it is generally unused, so provide an 
  unused value (negative for instance). Input voxels of other values are 
  considered fixed at their given distance. Use at least one voxel with a 
  given distance (0 typically) to start the propagation.
\param xm size of the kernel on x (in voxels)
\param ym size of the kernel on y (in voxels)
\param zm size of the kernel on z (in voxels)
\param mult_factor all distances are internally multiplied by this factor to 
  overcome precision issues (typically when using int values)
\param divide tells whether the multiplied distances should be re-divided to 
  millimeters values at the end of the operation
*/
template <typename T> void 
AimsDistanceFrontPropagation( aims::BucketMap<T>& vol, T val_domain, 
                              T val_outside,
                              int xm, int ym, int zm, float mult_factor,
                              bool divide );

#endif
