
#ifndef AIMS_MATH_CURV3DISO_H
#define AIMS_MATH_CURV3DISO_H

#include <aims/math/mathelem.h>
#include <cartodata/volume/volume.h>

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
carto::VolumeRef<float>
AimsIsoIntensityCurvature3D( const carto::rc_ptr<carto::Volume<T> > &vol,
                             AimsCurvatureType type = AIMS_MEAN_CURVATURE );


template <class T>
carto::VolumeRef<float>
AimsIsoIntensityCurvature3D( const carto::rc_ptr<carto::Volume<T> > &rvol,
                             AimsCurvatureType type )
{
  carto::VolumeRef<T> vol( rvol ); // convenience API
  ASSERT(vol->getSizeT()==1);
  carto::VolumeRef<float> curv( vol->getSizeX(), vol->getSizeY(),
                                vol->getSizeZ(), 1, vol->getBorders() );
  curv.fillBorder(0);
  curv = 0.0;
  curv.setVoxelSize( vol->getVoxelSize() );
  float fx,fy,fz,fxx,fyy,fzz,fxy,fxz,fyz;
  std::vector<int> dim = vol->getSize();

  switch (type)
  {
    case AIMS_GAUSSIAN_CURVATURE:
         for (int z=2;z<dim[2]-2;z++)
           for (int y=2;y<dim[1]-2;y++)
             for (int x=2;x<dim[0]-2;x++)
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
         for (int z=2;z<dim[2]-2;z++)
           for (int y=2;y<dim[1]-2;y++)
             for (int x=2;x<dim[0]-2;x++)
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
