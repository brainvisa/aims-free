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


#include <aims/distancemap/chamfer.h>
#include <aims/connectivity/connectivity.h>
#include <aims/data/data.h>
#include <iomanip>
#include <iostream>

using namespace std;

void AimsForwardSweepingWithBorder(AimsData<int16_t>& vol,
                                   const AimsDistmapMask& mask,
                                   int borderlevel)
{
  ASSERT(vol.dimT()==1);

  bool correctBorderWidth = false;

  AimsData<int16_t>::iterator it;
  AimsDistmapMaskPoint        *mask_first_point,*maskptr;
  int                         mask_length;
  int                         xcubesize, ycubesize, zcubesize;
  int                         xcourant,ycourant,zcourant;
  int                         xmaxtest,ymaxtest,zmaxtest;
  int                         maxval,newval;
  int                         neighbor;

  it = vol.begin() + vol.oFirstPoint();

  mask_first_point = mask.FirstPoint;
  mask_length      = mask.Length;
  xcubesize = mask.xCubeSize;
  ycubesize = mask.yCubeSize;
  zcubesize = mask.zCubeSize;
  xmaxtest = vol.dimX() - xcubesize;
  ymaxtest = vol.dimY() - ycubesize;
  zmaxtest = vol.dimZ() - zcubesize;
  xcourant = 1;
  ycourant = 1;
  zcourant = 1;

  if ((vol.borderWidth() >= mask.xCubeSize)  &&
      (vol.borderWidth() >= mask.yCubeSize)  &&
      (vol.borderWidth() >= mask.zCubeSize)    )
    correctBorderWidth = true;

  for (zcourant=1;zcourant<=vol.dimZ();zcourant++)
  {
    for (ycourant=1;ycourant<=vol.dimY();ycourant++)
    {
      if (correctBorderWidth==false)
        for (xcourant=1;xcourant<=vol.dimX();xcourant++)
        {
	  maxval =  *it;	
          if (maxval!=AIMS_CHAMFER_OUTSIDE_DOMAIN) 
          {
	    if ((xcourant<=xcubesize) || (xcourant>=xmaxtest) ||
                (ycourant<=ycubesize) || (ycourant>=ymaxtest) ||
                (zcourant<=zcubesize) || (zcourant>=zmaxtest)  )
            {
	      maskptr = mask_first_point;
              for (int l=mask_length;l--;)
              {
	        if (((xcourant-1) >= -maskptr->x)         &&
                    ((vol.dimX()-xcourant) >= maskptr->x) &&
                    ((ycourant-1) >= -maskptr->y)         &&
                    ((vol.dimY()-ycourant) >= maskptr->y) &&
                    ((zcourant-1) >= -maskptr->z )        &&
                    ((vol.dimZ()-zcourant) >= maskptr->z)   )
                {
		  newval = *(it + maskptr->Offset) + (maskptr->Dist);
                  if (newval<maxval) maxval = newval;
                }
                maskptr++;
              }
            }
            else
            {
	      maskptr = mask_first_point;
              for (int l=mask_length;l--;)
              {
	        newval = *(it + maskptr->Offset) + (maskptr->Dist);
                if (newval<maxval) maxval = newval;
                maskptr++;
              }
            }
            if (*it != (int16_t)maxval) *it = (int16_t)maxval;
          }
          it++;
        }
      else
        for (xcourant=1;xcourant<=vol.dimX();xcourant++)
        {
	  maxval =  *it;	
          if (maxval!=AIMS_CHAMFER_OUTSIDE_DOMAIN) 
          {
	    maskptr = mask_first_point;
            for (int l=mask_length;l--;)
            {
	      neighbor = *(it + maskptr->Offset);
              if (neighbor!=borderlevel)
              {
	        newval = neighbor + (maskptr->Dist);
                if (newval<maxval) maxval = newval;
              }
              maskptr++;
            }
            if (*it != (int16_t)maxval) *it = (int16_t)maxval;
          }
          it++;
        }
      it += vol.oPointBetweenLine();
    }
    it += vol.oLineBetweenSlice(); 
  }
}


void AimsBackwardSweepingWithBorder(AimsData<int16_t>& vol,
                                    const AimsDistmapMask &mask,
                                    int borderlevel)
{
  ASSERT(vol.dimT()==1);

  bool correctBorderWidth = false;

  AimsData<int16_t>::iterator it;
  AimsDistmapMaskPoint        *mask_first_point,*maskptr;
  int                         mask_length;
  int                         xcubesize, ycubesize, zcubesize;
  int                         xcourant,ycourant,zcourant;
  int                         xmaxtest,ymaxtest,zmaxtest;
  int                         maxval,newval;
  int                         neighbor;

  it = vol.begin() + vol.oLastPoint();

  mask_first_point = mask.FirstPoint;
  mask_length      = mask.Length;
  xcubesize = mask.xCubeSize;
  ycubesize = mask.yCubeSize;
  zcubesize = mask.zCubeSize;
  xmaxtest = vol.dimX() - xcubesize;
  ymaxtest = vol.dimY() - ycubesize;
  zmaxtest = vol.dimZ() - zcubesize;
  xcourant = 1;
  ycourant = 1;
  zcourant = vol.dimZ();

  if ((vol.borderWidth() >= mask.xCubeSize)  &&
      (vol.borderWidth() >= mask.yCubeSize)  &&
      (vol.borderWidth() >= mask.zCubeSize)    )
    correctBorderWidth = true;

  for (zcourant=vol.dimZ();zcourant>0;zcourant--)
    {
    for (ycourant=vol.dimY();ycourant>0;ycourant--)
    {
      if (correctBorderWidth==false)
        for (xcourant=vol.dimX();xcourant--;)
        {
	  maxval =  *it;	
          if (maxval!=AIMS_CHAMFER_OUTSIDE_DOMAIN) 
          {
	    if ((xcourant<=xcubesize) || (xcourant>=xmaxtest) ||
                (ycourant<=ycubesize) || (ycourant>=ymaxtest) ||
                (zcourant<=zcubesize) || (zcourant>=zmaxtest)  )
            {
	      maskptr = mask_first_point;
              for (int l=mask_length;l--;)
              {
	        if (((xcourant-1) >= -maskptr->x)         &&
                    ((vol.dimX()-xcourant) >= maskptr->x) &&
                    ((ycourant-1) >= -maskptr->y)         &&
                    ((vol.dimY()-ycourant) >= maskptr->y) &&
                    ((zcourant-1) >= -maskptr->z )        &&
                    ((vol.dimZ()-zcourant) >= maskptr->z)   )
                {
		  newval = *(it + maskptr->Offset) + (maskptr->Dist);
                  if (newval<maxval) maxval = newval;
                }
                maskptr++;
              }
            }
            else
            {
	      maskptr = mask_first_point;
              for (int l=mask_length;l--;)
              {
	        newval = *(it + maskptr->Offset) + (maskptr->Dist);
                if (newval<maxval) maxval = newval;
                maskptr++;
              }
            }
            if (*it != (int16_t)maxval) *it = (int16_t)maxval;
          }
          it--;
        }
      else
        for (xcourant=vol.dimX();xcourant--;)
        {
	  maxval = *it;	
          if (maxval!=AIMS_CHAMFER_OUTSIDE_DOMAIN) 
          {
	    maskptr = mask_first_point;
            for (int l=mask_length;l--;)
            {
	      neighbor = *(it + maskptr->Offset);
              if (neighbor!=borderlevel)
              {
	        newval = neighbor + (maskptr->Dist);
                if (newval<maxval) maxval = newval;
              }
              maskptr++;
            }
            if (*it != (int16_t)maxval) *it = (int16_t)maxval;
          }
          it--;
        }
      it -= vol.oPointBetweenLine();
    }
    it -= vol.oLineBetweenSlice(); 
  }
}
