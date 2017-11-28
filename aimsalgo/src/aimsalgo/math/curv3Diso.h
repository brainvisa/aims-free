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


#ifndef AIMS_MATH_CURV3DISO_H
#define AIMS_MATH_CURV3DISO_H

#include <aims/math/mathelem.h>
#include <aims/data/fastAllocationData.h>

/** The different 3D curvature types
*/
enum AimsCurvatureType
{
  AIMS_GAUSSIAN_CURVATURE,
  AIMS_MEAN_CURVATURE
};


/** 3D curvature functions on an intensity image f(x,y,z) = I
*/
template <class T> inline
AimsData<float> 
AimsIsoIntensityCurvature3D(const AimsData<T> &vol,
                            AimsCurvatureType type = AIMS_MEAN_CURVATURE);


template <class T>
AimsData<float> 
AimsIsoIntensityCurvature3D(const AimsData<T> &vol,AimsCurvatureType type)
{ ASSERT(vol.dimT()==1);
  aims::AimsFastAllocationData<float> curv(vol.dimX(),vol.dimY(),vol.dimZ(),1,vol.borderWidth());
  curv.fillBorder(0);
  curv = 0.0;
  curv.setSizeX(vol.sizeX());
  curv.setSizeY(vol.sizeY());
  curv.setSizeZ(vol.sizeZ());
  float fx,fy,fz,fxx,fyy,fzz,fxy,fxz,fyz;

  switch (type)
  {
    case AIMS_GAUSSIAN_CURVATURE:
         for (int z=2;z<vol.dimZ()-2;z++)
           for (int y=2;y<vol.dimY()-2;y++)
             for (int x=2;x<vol.dimX()-2;x++)
             { if (vol(x-1,y,z)!=vol(x+1,y,z) || 
                   vol(x,y-1,z)!=vol(x,y+1,z) || 
                   vol(x,y,z-1)!=vol(x,y,z+1)   )
               { fx  = ((float)vol(x+1,y,z) - (float)vol(x-1,y,z) ) / 2;
                 fy  = ((float)vol(x,y+1,z) - (float)vol(x,y-1,z) ) / 2;
                 fz  = ((float)vol(x,y,z+1) - (float)vol(x,y,z-1) ) / 2;
                 fxx = ((float)vol(x+2,y,z) - 2 * (float)vol(x,y,z) + 
                        (float)vol(x-2,y,z) ) / 4;
                 fyy = ((float)vol(x,y+2,z) - 2 * (float)vol(x,y,z) + 
                        (float)vol(x,y-2,z) ) / 4;
                 fzz = ((float)vol(x,y,z+2) - 2 * (float)vol(x,y,z) + 
                        (float)vol(x,y,z-2) ) / 4;
                 fxy = ((float)vol(x+1,y+1,z) - 
                        (float)vol(x-1,y+1,z) - 
                        (float)vol(x+1,y-1,z) + 
                        (float)vol(x-1,y-1,z) ) / 4;
                 fxz = ((float)vol(x+1,y,z+1) - 
                        (float)vol(x-1,y,z+1) - 
                        (float)vol(x+1,y,z-1) + 
                        (float)vol(x-1,y,z-1) ) / 4;
                 fyz = ((float)vol(x,y+1,z+1) - 
                        (float)vol(x,y-1,z+1) - 
                        (float)vol(x,y+1,z-1) + 
                        (float)vol(x,y-1,z-1) ) / 4;
                 curv(x,y,z)   = ( fx*fx*(fyy*fzz-fyz*fyz) +
                                   2*fy*fz*(fxz*fxy-fxx*fyz)  + 
                                   fy*fy*(fxx*fzz-fxz*fxz) +
                                   2*fx*fz*(fyz*fxy-fyy*fxz)  +
                                   fz*fz*(fxx*fyy-fxy*fxy) +
                                   2*fx*fy*(fxz*fyz-fzz*fxy)  ) /
                                   ((fx*fx+fy*fy+fz*fz)*(fx*fx+fy*fy+fz*fz));
               }
               else curv(x,y,z) = 0;
             }
         break;
    case AIMS_MEAN_CURVATURE:
         for (int z=2;z<vol.dimZ()-2;z++)
           for (int y=2;y<vol.dimY()-2;y++)
             for (int x=2;x<vol.dimX()-2;x++)
             { if (vol(x-1,y,z)!=vol(x+1,y,z) || 
                   vol(x,y-1,z)!=vol(x,y+1,z) || 
                   vol(x,y,z-1)!=vol(x,y,z+1)   )
               { fx  = ((float)vol(x+1,y,z) - (float)vol(x-1,y,z) ) / 2;
                 fy  = ((float)vol(x,y+1,z) - (float)vol(x,y-1,z) ) / 2;
                 fz  = ((float)vol(x,y,z+1) - (float)vol(x,y,z-1) ) / 2;
                 fxx = ((float)vol(x+2,y,z) - 2 * (float)vol(x,y,z) + 
                        (float)vol(x-2,y,z) ) / 4;
                 fyy = ((float)vol(x,y+2,z) - 2 * (float)vol(x,y,z) + 
                        (float)vol(x,y-2,z) ) / 4;
                 fzz = ((float)vol(x,y,z+2) - 2 * (float)vol(x,y,z) + 
                        (float)vol(x,y,z-2) ) / 4;
                 fxy = ((float)vol(x+1,y+1,z) - 
                        (float)vol(x-1,y+1,z) - 
                        (float)vol(x+1,y-1,z) + 
                        (float)vol(x-1,y-1,z) ) / 4;
                 fxz = ((float)vol(x+1,y,z+1) - 
                        (float)vol(x-1,y,z+1) - 
                        (float)vol(x+1,y,z-1) + 
                        (float)vol(x-1,y,z-1) ) / 4;
                 fyz = ((float)vol(x,y+1,z+1) - 
                        (float)vol(x,y-1,z+1) - 
                        (float)vol(x,y+1,z-1) + 
                        (float)vol(x,y-1,z-1) ) / 4;
                 curv(x,y,z) = ( fx*fx*(fyy+fzz) - 2*fy*fz*fyz +
                                 fy*fy*(fxx+fzz) - 2*fx*fz*fxz +
                                 fz*fz*(fxx+fyy) - 2*fx*fy*fxy ) /
                               ( 2 * cube( sqrt(fx*fx + fy*fy + fz*fz) ) );
               }
               else curv(x,y,z) = 0;
             }
         break;
  }
  return curv;
}


#endif
