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


#include <aims/morphology/operatormorpho.h>
#include <aims/data/data.h>
#include <aims/distancemap/chamfer.h>
#include <aims/utility/threshold.h>
#include <aims/utility/bininverse.h>
#include <aims/math/mathelem.h>


AimsData<short> AimsMorphoConnectivityChamferDilation(AimsData<short> &vol,
						     float size,
						     Connectivity::Type type)
{
  ASSERT(vol.dimT()==1);
  ASSERT( size>0 && size<(float)square(vol.dimX()) && 
                    size<(float)square(vol.dimY()) );

  AimsData<short> dilated;
  AimsBinaryInverse inversion;
  dilated = AimsConnectivityChamferDistanceMap(vol, type);

  AimsThreshold<short,short> thresh(AIMS_LOWER_OR_EQUAL_TO,(short)(size+0.01));


  return thresh.bin(dilated);
}


AimsData<short> AimsMorphoDilation(AimsData<short> &vol,
                                     float size, AimsMorphoMode mode)
{
  ASSERT(vol.dimT()==1);
  AimsData<short> dilate;

  switch (mode)
  {
    case AIMS_CHAMFER_BALL_3D :
      dilate = AimsMorphoChamferDilation(vol,size,3,3,3,50);
      break;
    case AIMS_CHAMFER_BALL_2D :
      dilate = AimsMorphoChamferDilation(vol,size,3,3,1,50);
      break;
  }
  return dilate;
}


AimsData<short> AimsMorphoChamferDilation(AimsData<short> &vol,
                                           float size,
                                           int xmask,int ymask,int zmask,
                                           float mult_fact)
{
  ASSERT(vol.dimT()==1);
  ASSERT( size>0 && size<(float)square(vol.dimX()) && 
                    size<(float)square(vol.dimY()) );

  AimsData<short> dilated;
  dilated = AimsChamferDistanceMap(vol,xmask,ymask,zmask,mult_fact);

  AimsThreshold<short,short> thresh(AIMS_LOWER_OR_EQUAL_TO,
                                    (short)(size*mult_fact+0.5));
  return thresh.bin(dilated);
}
