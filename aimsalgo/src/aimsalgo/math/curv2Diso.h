
#ifndef AIMS_MATH_CURV2DISO_H
#define AIMS_MATH_CURV2DISO_H

#include <cartodata/volume/volume.h>

/** 2D Curvature functions on an intensity image f(x,y) = I.
*/
template <class T>
carto::VolumeRef<float> AimsIsoIntensityCurvature2D(
  const carto::rc_ptr<carto::Volume<T> > &mat );


template <class T> inline
carto::VolumeRef<float>
AimsIsoIntensityCurvature2D( const carto::rc_ptr<carto::Volume<T> > & mat )
{
  ASSERT(mat->getSizeZ()==1 && mat->getSizeT()==1);
  carto::VolumeRef<float> curv(
    mat->getSizeX(), mat->getSizeY(), 1, 1, mat->getBorders()[0] );
  curv.fillBorder(0);
  curv = 0.0;
  curv.setVoxelSize( mat->getVoxelSize() );
  std::vector<int> dim = mat->getSize();
  float fx,fy,fxx,fyy,fxy;
  
  for (int y=2;y<dim[1]-2;y++)
    for (int x=2;x<dim[0]-2;x++)
    {
      if (mat->at(x-1,y)!=mat->at(x+1,y) || mat->at(x,y-1)!=mat->at(x,y+1))
      { fx  = ((float)mat->at(x+1,y) - (float)mat->at(x-1,y) ) / 2;
        fy  = ((float)mat->at(x,y+1) - (float)mat->at(x,y-1) ) / 2;
        fxx = ((float)mat->at(x+2,y) - 2 * (float)mat->at(x,y) +
               (float)mat->at(x-2,y) ) / 4;
        fyy = ((float)mat->at(x,y+2) - 2 * (float)mat->at(x,y) +
               (float)mat->at(x,y-2) ) / 4;
        fxy = ((float)mat->at(x+1,y+1) -
               (float)mat->at(x-1,y+1) -
               (float)mat->at(x+1,y-1) +
               (float)mat->at(x-1,y-1) ) / 4;
        curv(x,y) = (- fxx*fy*fy + 2*fx*fy*fxy - fyy*fx*fx) / 
                     cube( sqrt(fx*fx + fy*fy) );
      }
      else curv(x,y) = 0;
    }
  return curv;
}

#endif
