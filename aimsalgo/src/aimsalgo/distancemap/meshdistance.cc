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


#include <aims/distancemap/meshdistance_d.h>
#include <aims/distancemap/distancemap_g.h>
#include <aims/morphology/morphology_g.h>

using namespace aims;
using namespace aims::meshdistance;
using namespace std;




Texture<float> aims::meshdistance::GeodesicDepth ( const AimsSurface<3,Void> & mesh, 
						   const AimsData <short> & vol ,
						       float radius_close, float radius_erode)
{
  
  const vector<Point3df>		& vert = mesh.vertex();
  unsigned				i, n = vert.size();
  Texture<float> 			dist(n);
  Texture<short>			inittex(n);
  long    				x,y,z;
  Point3df				pos;
  float 				sizex,sizey,sizez;
  AimsData <short>			envelop = vol.clone();
      
  cout << "Closing of the mask (size: " << radius_close << ")" << endl;
  envelop = AimsMorphoClosing( envelop, radius_close );

  cout << "Erosion of the mask (size: " << radius_erode << ")" << endl;
  envelop = AimsMorphoErosion( envelop , radius_erode );

  sizex = envelop.sizeX();
  sizey = envelop.sizeY();
  sizez = envelop.sizeZ();
    
  for (i=0;i<n;++i)
    {
     
      pos = vert[i];
      x = (long) rint(pos[0] / sizex );
      y = (long) rint(pos[1] / sizey );
      z = (long) rint(pos[2] / sizez );
      
      if (envelop(x,y,z) == 0)
	inittex.item(i) = 1;
      else
	inittex.item(i) = 0;

    }
  cout << "Compute the geodesic distance\n";
  dist = MeshDistance(mesh,inittex,true);
  
  return(dist) ;
}

namespace aims 
{
  namespace meshdistance 
  {
    template
    Texture<float> MeshDistance( const AimsSurface<3,Void> & mesh, 
				 const Texture<short> & 
				 inittex, bool allowUnreached );
    template
    Texture<float> MeshDistance( const AimsSurface<3,Void> & mesh,
                                 const Texture<float> &
                                 inittex, bool allowUnreached );
  }

}

