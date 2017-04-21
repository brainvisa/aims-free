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



#ifndef AIMS_MESH_MESHMERGE_H
#define AIMS_MESH_MESHMERGE_H


#include <aims/config/aimsalgo_config.h>
#include <aims/mesh/surfaceOperation.h>
#include <list>


namespace aims
{

  /// \deprecated{OBSOLETE, use SurfaceManip::meshMerge() instead}
  template<int D, class T>
  void meshMerge( AimsTimeSurface<D,T> & dst, 
		  const AimsTimeSurface<D,T> & add )
    __attribute__((__deprecated__("OBSOLETE, use SurfaceManip::meshMerge() instead")));

  template<int D, class T>
  void meshMerge( AimsTimeSurface<D,T> & dst, 
		  const AimsTimeSurface<D,T> & add )
  {
    SurfaceManip::meshMerge( dst, add );
  }
  /// \deprecated{OBSOLETE, use SurfaceManip::meshMerge() instead}
  template<int D, class T>
  void meshMerge( AimsTimeSurface<D,T> & dst, 
		  const std::list<AimsTimeSurface<D,T> > & src )
    __attribute__((__deprecated__("OBSOLETE, use SurfaceManip::meshMerge() instead")));

  template<int D, class T>
  void meshMerge( AimsTimeSurface<D,T> & dst, 
		  const std::list<AimsTimeSurface<D,T> > & src )
  {
    SurfaceManip::meshMerge( dst, src );
  }

  AIMSALGO_API AimsSurfaceTriangle createCylinder( const Point3df & p1, 
						   const Point3df & p2, 
						   float radius, 
						   unsigned nfacets, 
						   bool closed = false );

  AIMSALGO_API AimsSurfaceTriangle createCube( const Point3df & center, 
					       float size );

  AIMSALGO_API AimsSurfaceTriangle createIcosahedron( const Point3df & center, 
						      float radius );

}


#endif