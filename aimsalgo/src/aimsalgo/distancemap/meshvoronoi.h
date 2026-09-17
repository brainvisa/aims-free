

#ifndef AIMS_DISTANCEMAP_MESHVORONOI_H
#define AIMS_DISTANCEMAP_MESHVORONOI_H

#include <aims/mesh/texture.h>
#include <aims/mesh/surface.h>
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
