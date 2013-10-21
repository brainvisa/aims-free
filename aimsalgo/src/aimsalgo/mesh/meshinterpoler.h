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


#ifndef AIMS_MESH_MESHINTERPOLER_H
#define AIMS_MESH_MESHINTERPOLER_H

#include <aims/mesh/surface.h>
#include <aims/mesh/texture.h>


namespace aims
{

  /** Triangular mesh and texture interpolation.
      A source mesh has to be projected onto a destination one. Both have to be
      registered earlier (we assume there are no long distances in the
      geometries of both).

      The main operation is project(), which calculates the correspondances
      between the source and destination mesh.

      Then a texture from the source mesh can be resampled for the destination
      one.

      If the meshes correspond to registrations, or deformations, of other
      meshes in a different space (typically, a sphere like for FreeSurfer),
      then a mesh resampling can be performed back into the native space, to
      get a destination mesh topology in the native space.
   */
  class MeshInterpoler
  {
  public:
    enum InterpolationType
    {
      Linear,
      NearestNeighbour,
    };

    MeshInterpoler( const AimsSurfaceTriangle & source, const
      AimsSurfaceTriangle & dest );
    /** This constructor is used when meshes are low-level raw arrays.
     */
    MeshInterpoler( const Point3df* sourceVert,
                    const AimsVector<uint,3>* sourcePoly,
                    uint nSourcePolygon, const Point3df* destVertex,
                    const AimsVector<uint,3>* destPolygon, uint nDestVertex,
                    uint nDestPolygon );
    virtual ~MeshInterpoler();

    /** Main projection function.
        This function has to be called before the various resample*() functions
        are used. It calculates the projectedTriangles and projectedTriCoord*
        that can be accessed via the corresponding functions.
     */
    void project();
    /** Resample the sourceshape mesh onto the topology of the interpoler
        destination mesh, but staying in the native space of sourceshape.
        sourceshape and the source mesh of the interpoler must have the same
        structure and topology (same vertices number and order, same polygons).
     */
    AimsSurfaceTriangle *resampleMesh( const AimsSurfaceTriangle & sourceshape
      ) const;
    template <typename T> TimeTexture<T> *
      resampleTexture( const TimeTexture<T> &,
                       InterpolationType = Linear ) const;
    template <typename T> void
      resampleTexture( const Texture<T> & source, Texture<T> & dest,
                       int timestep = 0, InterpolationType = Linear ) const;
    /// low level texture resampling, working on preallocated buffers
    template <typename T> void
      resampleTexture( const T *source, T *dest, int timestep = 0,
                       InterpolationType = Linear ) const;

    const TimeTexture<uint> & projectedTriangles() const;
    const TimeTexture<float> & projectedTriCoord1() const;
    const TimeTexture<float> & projectedTriCoord2() const;
    const TimeTexture<float> & projectedTriCoord3() const;
    /// set interpoler internal state (for instance to reload it after saving)
    void reloadProjectionParams( const TimeTexture<uint> & projTriangles,
      const TimeTexture<float> & projTriCoord1,
      const TimeTexture<float> & projTriCoord2,
      const TimeTexture<float> & projTriCoord3 );
    void setMeshes( const AimsSurfaceTriangle & source, const
      AimsSurfaceTriangle & dest );

  protected:
    MeshInterpoler();
    /// sub-step 1 of project(): calculates projectedTriangles
    void findNeighbours();
    /// sub-step 2 of project(): calculates projectedTriCoord*
    void processCoordinates();
    void findNeighbours_timestep( const Point3df *sourceVert,
      const AimsVector<uint,3>* sourcePoly, uint nSourcePolygon,
      const Point3df* destVertex, uint nDestVertex, int timestep );
    void processCoordinates_timestep( const Point3df *sourceVert,
      const AimsVector<uint,3> *sourcePoly, const Point3df *destVertex,
      uint nDestVertex, int timestep );
  private:
    struct Private;
    Private *d;
  };


  class CoordinatesFieldMeshInterpoler : public MeshInterpoler
  {
  public:
    CoordinatesFieldMeshInterpoler( const AimsSurfaceTriangle & source,
                                    const AimsSurfaceTriangle & dest,
                                    const TimeTexture<float> & srccoord1,
                                    const TimeTexture<float> & srccoord2,
                                    const TimeTexture<float> & dstcoord1,
                                    const TimeTexture<float> & dstcoord2 );
    virtual ~CoordinatesFieldMeshInterpoler();

  private:
    struct Private;
    Private *d2;
  };

}

#endif

