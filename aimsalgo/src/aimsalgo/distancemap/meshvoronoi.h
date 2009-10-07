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



#ifndef AIMS_DISTANCEMAP_MESHVORONOI_H
#define AIMS_DISTANCEMAP_MESHVORONOI_H

#include <aims/mesh/texture.h>
#include <aims/mesh/surface.h>
#include <aims/data/data.h>
#include <set>
#include <list>
#include <map>
#include <aims/distancemap/stlsort.h>

class Graph;
namespace aims
{
  namespace meshdistance
  {

    
    TimeTexture<short>
    MeshVoronoiStepbyStep( const AimsSurface<3,Void> & mesh, 
			   const Texture<short> & inittex, 
			   float dist, bool connexity, 
			   bool object );
  
    Texture<short> MeshVoronoi( const AimsSurface<3,Void> & mesh, 
				const Texture<short> & inittex, 
				float dist, bool connexity,bool object );
  
    
    

    

    /** Compute a geodesic voronoi diagram (dist = MAX_FLOAT,object=true ) of 
	objects defined in inittex. 
	The background has the label Back and the objects have a positive 
	label.
	The distance can be euclidean geodesic (connexity=false) or just the 
	connexity of the triangulation(connexity=true)
	This function can as well be used for dilation(object=true) / 
	erosion(object=false) using the  parameter dist as the size of the 
	structuring element */
    template <class T>
    Texture<T> MeshVoronoiT ( const AimsSurface<3,Void> & mesh, 
			      const Texture<T> & inittex, 
			      const T & Back, const T & For, 
			      float dist, bool connexity,bool object);

    /// global variable...
    extern  const short MESHDISTANCE_FORBIDDEN;
    extern  const short MESHDISTANCE_UNREACHED;

  }
}


#endif
