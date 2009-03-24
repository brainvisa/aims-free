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


#include <aims/distancemap/chamfer.h>
#include <aims/data/data.h>
#include <iostream>

using namespace std;

static void AimsVolumeChamferPreparation(AimsData<int16_t> &vol);


static void AimsVolumeChamferPreparation(AimsData<int16_t> &vol)
{
  AimsData<int16_t>::iterator it;

  it = vol.begin() + vol.oFirstPoint();

  for (int z=vol.dimZ();z--;)
  {
    for (int y=vol.dimY();y--;)
	 {
	   for (int x=vol.dimX();x--;)
	   {
	     if (*it) *it++ = 0;
	     else     *it++ = AIMS_CHAMFER_DOMAIN;
	   }
	   it += vol.oPointBetweenLine();
	 }
    it += vol.oLineBetweenSlice();
  }
}


AimsData<int16_t>
AimsConnectivityChamferDistanceMap(const AimsData<int16_t> &vol,
				   Connectivity::Type connectivity)
{
  ASSERT(vol.dimT()==1);

  AimsData<int16_t> volbin = vol.clone();
  AimsDistmapMask f,b;


  ASSERT(volbin.borderWidth()>0);
  volbin.fillBorder(AIMS_CHAMFER_OUTSIDE_DOMAIN);

  AimsVolumeChamferPreparation(volbin);

  AimsCreateAndSplitConnectivityMask(volbin, connectivity, f, b);
				         
  //cout << "Forward" << endl;
  AimsForwardSweepingWithBorder (volbin,f,AIMS_CHAMFER_OUTSIDE_DOMAIN);
  //cout << "Backward" << endl;
  AimsBackwardSweepingWithBorder(volbin,b,AIMS_CHAMFER_OUTSIDE_DOMAIN);

  return(volbin);
}

AimsData<int16_t>
AimsChamferDistanceMap(const AimsData<int16_t> &vol,
                                       int xmask,int ymask,int zmask,
                                       float mult_factor)
{
  ASSERT(vol.dimT()==1);

  AimsData<int16_t> volbin = vol.clone();
  AimsDistmapMask f,b;

  ASSERT(mult_factor>0);

  ASSERT(volbin.borderWidth()>0);
  volbin.fillBorder(AIMS_CHAMFER_OUTSIDE_DOMAIN);

  AimsVolumeChamferPreparation(volbin);

  AimsCreateAndSplitCubicMask(volbin,xmask,ymask,zmask,f,b,mult_factor);
     
  //cout << "Forward" << endl;
  AimsForwardSweepingWithBorder (volbin,f,AIMS_CHAMFER_OUTSIDE_DOMAIN);
  //cout << "Backward" << endl;
  AimsBackwardSweepingWithBorder(volbin,b,AIMS_CHAMFER_OUTSIDE_DOMAIN);

  return(volbin);
}


AimsData<float>
AimsFloatChamferDistanceMap(const AimsData<int16_t> &vol,int side,
                            int xsize,int ysize,int zsize,float mult_factor)
{
  ASSERT(vol.dimT()==1);

  AimsData<int16_t> distmap = vol.clone();

  ASSERT(vol.borderWidth()>0);

  if (side==AIMS_CHAMFER_INSIDE)
    for (int z=0;z<distmap.dimZ();z++)
      for (int y=0;y<distmap.dimY();y++)
        for (int x=0;x<distmap.dimX();x++)
          distmap(x,y,z) = (distmap(x,y,z)==0 ? 32767 : 0);  

  distmap = AimsChamferDistanceMap(distmap,
                                   xsize,ysize,zsize,mult_factor);

  AimsData<float> distfloat(distmap.dimX(),distmap.dimY(),
                            distmap.dimZ(),1,distmap.borderWidth());

  distfloat.setSizeX(vol.sizeX());
  distfloat.setSizeY(vol.sizeY());
  distfloat.setSizeZ(vol.sizeZ());

  for (int z=0;z<distmap.dimZ();z++)
    for (int y=0;y<distmap.dimY();y++)
      for (int x=0;x<distmap.dimX();x++)
        distfloat(x,y,z) = (float)distmap(x,y,z) / mult_factor;  
 
  return(distfloat);
}


AimsData<float>
AimsFloatSignedChamferDistanceMap(const AimsData<int16_t> &vol,
                                  int xsize,int ysize,int zsize,
                                  float mult_factor)
{
  ASSERT(vol.dimT()==1);
  ASSERT(vol.borderWidth()>0);
  AimsData<float> 
  inside = AimsFloatChamferDistanceMap(vol,AIMS_CHAMFER_INSIDE ,
                                       xsize,ysize,zsize,mult_factor);
  AimsData<float>
  outside = AimsFloatChamferDistanceMap(vol,AIMS_CHAMFER_OUTSIDE,
                                        xsize,ysize,zsize,mult_factor);
  AimsData<float> res(vol.dimX(),vol.dimY(),vol.dimZ(),1,vol.borderWidth());
  res.setSizeXYZT(vol.sizeX(),vol.sizeY(),vol.sizeZ(),vol.sizeT());

  for (int z=0;z<res.dimZ();z++)
    for (int y=0;y<res.dimY();y++)
      for (int x=0;x<res.dimX();x++)
        res(x,y,z) = inside(x,y,z) - outside(x,y,z);  

  return(res);
}
