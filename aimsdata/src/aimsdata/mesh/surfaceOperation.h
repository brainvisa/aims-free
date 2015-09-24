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

/*
 *  Surface class
 */
#ifndef AIMS_SURFACE_SURFACEOPERATION_H
#define AIMS_SURFACE_SURFACEOPERATION_H

#include <aims/mesh/surface.h>
#include <cartobase/smart/rcptr.h>
#include <set>
#include <vector>
#include <map>
#include <list>

namespace aims
{
  class AffineTransformation3d;
}
typedef aims::AffineTransformation3d Motion;
template<typename T> class TimeTexture;
template<typename T> class Texture;

namespace aims
{

  class SurfaceManip
  {
  public:
    template<int D, class T> static std::vector<std::set<uint> >
      surfaceNeighbours( const AimsSurface<D,T> & surf );

    template<int D, class T> static std::vector<std::set<uint> >
      surfaceNeighbours( const AimsTimeSurface<D,T> & surf );

    template<int D, class T> static std::vector<std::set<uint> >
      surfaceNeighbours2ndOrder( const AimsTimeSurface<D,T> & surf );

    template<int D, class T> static std::vector<std::map<uint, float> >
      surfaceNeighbourDistance( const AimsTimeSurface<D,T> & surf );

    template<int D, class T> 
      static void invertSurfacePolygons( AimsTimeSurface<D,T> & surface );

    static void cutMesh( const AimsSurfaceTriangle & insurf,
                         const Point4df & plane, AimsSurfaceTriangle & cut,
                         AimsTimeSurface<2, Void> & borderline );
    static void
      cutMesh( const std::vector<const AimsSurfaceTriangle *> & insurf,
               const Point4df & plane,
               std::vector<carto::rc_ptr<AimsSurfaceTriangle> > & cut,
               AimsTimeSurface<2, Void> & borderline );
    static void cutMesh( const AimsSurfaceTriangle & insurf,
                         const Point4df & plane, AimsSurfaceTriangle & cut,
                         AimsSurfaceTriangle & planemesh,
                         bool checkplane = true );
    static void
      cutMesh( const std::vector<const AimsSurfaceTriangle *> & insurf,
               const Point4df & plane,
               std::vector<carto::rc_ptr<AimsSurfaceTriangle> > & cut,
               AimsSurfaceTriangle & planemesh,
               bool checkplane = true );
    static void cutMesh( const AimsSurfaceTriangle & insurf,
                         const Point4df & plane, AimsSurfaceTriangle & cut,
                         AimsSurfaceTriangle & planemesh,
                         AimsTimeSurface<2, Void> & borderline,
                         bool checkplane = true );
    static void
      cutMesh( const std::vector<const AimsSurfaceTriangle *> & insurf,
               const Point4df & plane,
               std::vector<carto::rc_ptr<AimsSurfaceTriangle> > & cut,
               AimsSurfaceTriangle & planemesh,
               AimsTimeSurface<2, Void> & borderline,
               bool checkplane = true );
    static void cutMesh( const AimsSurfaceTriangle & insurf,
                         const Point4df & plane, AimsSurfaceTriangle & cut,
                         AimsSurfaceTriangle & planemesh,
                         AimsTimeSurface<2, Void> & borderline,
                         bool meshplane, bool checkplane );
    static void
      cutMesh( const std::vector<const AimsSurfaceTriangle *> & insurf,
               const Point4df & plane,
               std::vector<carto::rc_ptr<AimsSurfaceTriangle> > & cut,
               AimsSurfaceTriangle & planemesh,
               AimsTimeSurface<2, Void> & borderline,
               bool meshplane, bool checkplane );
    /// This variant only computes the border line
    static void cutMesh( const AimsSurfaceTriangle & insurf,
                         const Point4df & plane,
                         AimsTimeSurface<2, Void> & borderline,
                         int timestep = 0 );
    /// This variant only computes the border line
    static void cutMesh( const AimsSurface<3, Void> & insurf,
                         const Point4df & plane,
                         AimsTimeSurface<2, Void> & borderline );
    static void meshPlanarPolygon( const Point4df & plane,
                                   const AimsTimeSurface<2, Void> & border,
                                   AimsSurfaceTriangle & outmesh );
    /** Check mesh for intersecting triangles. 
	If \c outsurf is not null and intersecting triangles do exist, 
	\c outsurf is used to store a corrected mesh */
    static bool checkMesh( const AimsSurfaceTriangle & insurf, 
			   AimsSurfaceTriangle *outsurf = 0 );
    static float meshArea( const AimsSurfaceTriangle & surf );
    static float meshVolume( const AimsSurfaceTriangle & surf );
    /// concatenates 2 meshes into one (adds the second to the first one)
    template<int D, class T>
    static void meshMerge( AimsTimeSurface<D,T> & dst, 
			   const AimsTimeSurface<D,T> & add );
    /// concatenates several meshes (src) into a single one (dst)
    template<int D, class T>
    static void meshMerge( AimsTimeSurface<D,T> & dst, 
			   const std::list<AimsTimeSurface<D,T> > & src );
    /// Applies a transformation to a mesh
    template<int D, typename T>
    static void meshTransform( AimsTimeSurface<D,T> & mesh, 
                               const Motion & trans );
    template <int D, typename T>
    static AimsTimeSurface<D,T> *
        meshExtract( const AimsTimeSurface<D,T> & mesh,
                     const TimeTexture<int16_t> & tex, int16_t value,
                     std::vector<size_t> ** overtIndex = 0 );
    /** Extracts the boundary of region of value <region> of the input texture,
        on the mesh. If region is negative, take boundaries of all regions.
        The input texture is a label texture.
    */
    template <typename T>
    static AimsSurface<2,Void> *
        meshTextureBoundary( const AimsSurface<3,Void> & mesh,
          const Texture<T> & tex, T region );
    /** Extracts the boundary of region of value <region> of the input texture,
        on the mesh. If region is negative, take boundaries of all regions.
        The input texture is a label texture.
    */
    template <typename T>
    static AimsTimeSurface<2,Void> *
        meshTextureBoundary( const AimsSurfaceTriangle & mesh,
          const TimeTexture<T> & tex, T region );
    /** computes the position of the point in the mesh nearest to the given 
        position, averaging across the nneighbours nearest vertices.
        The optional index parameter will contain, when given, the index of 
        the nearest mesh vertex
    */
    static Point3df nearestPointToMesh( const Point3df & pos, 
                                        const AimsSurfaceTriangle & mesh, 
                                        unsigned nneighbours = 1, 
                                        unsigned* index = 0 );

    /** Refine a mesh by subdivising every triangle into 4 smaller ones.
        If selectedPolygons is specified (non-empty), it should contain polygons
        indices which must specifically be refined, the remaining ones will stay
        unchanged.
    */
    static AimsSurfaceTriangle* refineMeshTri4(
      const AimsSurfaceTriangle & mesh,
      const std::vector<uint> & selectedPolygons = std::vector<uint>() );
    /** Calculate a mesh density: inverse of the average edges distance
        If asDistance is true, then the average distance is returned instead of
        the density
    */
    template <int D, typename T>
    static TimeTexture<float>* meshDensity( const AimsTimeSurface<D,T> & mesh,
      bool asDistance = false );
    /** Calculate an edge length ratio in edges of two meshes with the same
        topology. The max length ratios of edges is kept for each vertex
    */
    template <int D, typename T>
    static TimeTexture<float>* meshEdgeLengthRatioTexture(
      const AimsTimeSurface<D,T> & nummesh,
      const AimsTimeSurface<D,T> & denommesh );

    //! Checks if two meshes intersect
    static bool checkMeshIntersect( const AimsSurfaceTriangle &,
                                    const AimsSurfaceTriangle & );

    /// calculate directions of a line mesh, fora each vertex
    static std::vector<Point3df>* lineDirections(
      const AimsTimeSurface<2, Void> & );

    /** Sort polygons along a given direction.
        Polygons centers will be used for sorting. Only one timestep is
        performed (to be fast).
    */
    template <int D, typename T>
    static void sortPolygonsAlongDirection(
      AimsTimeSurface<D,T> & mesh, int timestep, const Point3df & direction );

  };

}


#endif
