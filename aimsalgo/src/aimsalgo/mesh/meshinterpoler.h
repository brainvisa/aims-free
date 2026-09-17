
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

    /** Discontinuities may occur in some meshes, on their vertices coordinates
        (especially when built from coordinates fields). In this case triangles
        which present coordinates differences on their edges larger than the
        given thresholds are discarded from the projection correspondance map,
        so that no vertex is projected directly on them.
        Thresholds of 0 means that no thresholding is applied for the
        corresponding coordinates.
    */
    void setDiscontinuityThresholds( float xthresh, float ythresh,
                                     float zthresh );
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

