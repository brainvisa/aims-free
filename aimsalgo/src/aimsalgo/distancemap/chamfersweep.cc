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

#include <aims/distancemap/chamfer.h>
#include <aims/connectivity/connectivity.h>
#include <cartodata/volume/volume.h>
#include <iomanip>
#include <iostream>

using namespace carto;
using namespace std;

void AimsForwardSweepingWithBorder( rc_ptr<Volume<int16_t> >& vol,
                                    const AimsDistmapMask& mask,
                                    int borderlevel )
{
  ASSERT( vol->getSizeT() == 1 );

  bool correctBorderWidth = false;

  int16_t *it;
  AimsDistmapMaskPoint        *mask_first_point,*maskptr;
  int                         mask_length;
  int                         xcubesize, ycubesize, zcubesize;
  int                         xcourant,ycourant,zcourant;
  int                         xmaxtest,ymaxtest,zmaxtest;
  int                         maxval,newval;
  int                         neighbor;
  int dx = vol->getSizeX(), dy = vol->getSizeY(), dz = vol->getSizeZ();

  mask_first_point = mask.FirstPoint;
  mask_length      = mask.Length;
  xcubesize = mask.xCubeSize;
  ycubesize = mask.yCubeSize;
  zcubesize = mask.zCubeSize;
  xmaxtest = dx - xcubesize;
  ymaxtest = dy - ycubesize;
  zmaxtest = dz - zcubesize;
  xcourant = 1;
  ycourant = 1;
  zcourant = 1;

  if ((vol->getBorders()[0] >= mask.xCubeSize)  &&
      (vol->getBorders()[1] >= mask.yCubeSize)  &&
      (vol->getBorders()[2] >= mask.zCubeSize) )
    correctBorderWidth = true;

  for (zcourant=1;zcourant<=dz;zcourant++)
  {
    for (ycourant=1;ycourant<=dy;ycourant++)
    {
      it = &vol->at( 0, ycourant - 1, zcourant - 1 );

      if (correctBorderWidth==false)
        for (xcourant=1;xcourant<=dx;xcourant++)
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
                    ((dx-xcourant) >= maskptr->x) &&
                    ((ycourant-1) >= -maskptr->y)         &&
                    ((dy-ycourant) >= maskptr->y) &&
                    ((zcourant-1) >= -maskptr->z )        &&
                    ((dz-zcourant) >= maskptr->z)   )
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
        for (xcourant=1;xcourant<=dx;xcourant++)
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
    }
  }
}


void AimsBackwardSweepingWithBorder( rc_ptr<Volume<int16_t> >& vol,
                                     const AimsDistmapMask &mask,
                                     int borderlevel )
{
  ASSERT( vol->getSizeT() == 1 );

  bool correctBorderWidth = false;

  int16_t *it;
  AimsDistmapMaskPoint        *mask_first_point,*maskptr;
  int                         mask_length;
  int                         xcubesize, ycubesize, zcubesize;
  int                         xcourant,ycourant,zcourant;
  int                         xmaxtest,ymaxtest,zmaxtest;
  int                         maxval,newval;
  int                         neighbor;
  int dx = vol->getSizeX(), dy = vol->getSizeY(), dz = vol->getSizeZ();

  mask_first_point = mask.FirstPoint;
  mask_length      = mask.Length;
  xcubesize = mask.xCubeSize;
  ycubesize = mask.yCubeSize;
  zcubesize = mask.zCubeSize;
  xmaxtest = dx - xcubesize;
  ymaxtest = dy - ycubesize;
  zmaxtest = dz - zcubesize;
  xcourant = 1;
  ycourant = 1;
  zcourant = dz;

  if ((vol->getBorders()[0] >= mask.xCubeSize)  &&
      (vol->getBorders()[1] >= mask.yCubeSize)  &&
      (vol->getBorders()[2] >= mask.zCubeSize) )
    correctBorderWidth = true;

  for (zcourant=dz;zcourant>0;zcourant--)
  {
    for (ycourant=dy;ycourant>0;ycourant--)
    {
      it = &vol->at( dx - 1, ycourant - 1, zcourant - 1 );
      if (correctBorderWidth==false)
        for (xcourant=dx;xcourant--;)
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
                    ((dx-xcourant) >= maskptr->x) &&
                    ((ycourant-1) >= -maskptr->y)         &&
                    ((dy-ycourant) >= maskptr->y) &&
                    ((zcourant-1) >= -maskptr->z )        &&
                    ((dz-zcourant) >= maskptr->z)   )
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
      {
        for (xcourant=dx;xcourant--;)
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
      }
    }
  }
}
