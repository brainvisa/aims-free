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
