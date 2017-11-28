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


#ifndef AIMS_MATH_CURV2DISO_H
#define AIMS_MATH_CURV2DISO_H

#include <aims/data/fastAllocationData.h>

/** 2D Curvature functions on an intensity image f(x,y) = I.
*/
template <class T>
AimsData<float> AimsIsoIntensityCurvature2D(const AimsData<T> &mat);


template <class T> inline
AimsData<float> 
AimsIsoIntensityCurvature2D(const AimsData<T> &mat)
{ ASSERT(mat.dimZ()==1 && mat.dimT()==1);
  aims::AimsFastAllocationData<float> curv(mat.dimX(),mat.dimY(),1,1,mat.borderWidth());
  curv.fillBorder(0);
  curv = 0.0;
  curv.setSizeX(mat.sizeX());
  curv.setSizeY(mat.sizeY());
  float fx,fy,fxx,fyy,fxy;
  
  for (int y=2;y<mat.dimY()-2;y++)
    for (int x=2;x<mat.dimX()-2;x++)
    { if (mat(x-1,y)!=mat(x+1,y) || mat(x,y-1)!=mat(x,y+1))
      { fx  = ((float)mat(x+1,y) - (float)mat(x-1,y) ) / 2;
        fy  = ((float)mat(x,y+1) - (float)mat(x,y-1) ) / 2;
        fxx = ((float)mat(x+2,y) - 2 * (float)mat(x,y) + 
               (float)mat(x-2,y) ) / 4;
        fyy = ((float)mat(x,y+2) - 2 * (float)mat(x,y) + 
               (float)mat(x,y-2) ) / 4;
        fxy = ((float)mat(x+1,y+1) - 
               (float)mat(x-1,y+1) - 
               (float)mat(x+1,y-1) + 
               (float)mat(x-1,y-1) ) / 4;
        curv(x,y) = (- fxx*fy*fy + 2*fx*fy*fxy - fyy*fx*fx) / 
                     cube( sqrt(fx*fx + fy*fy) );
      }
      else curv(x,y) = 0;
    }
  return(curv);
}

#endif
