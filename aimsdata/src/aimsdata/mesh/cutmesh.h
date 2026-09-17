#ifndef AIMS_MESH_CUTMESH_H
#define AIMS_MESH_CUTMESH_H

#include <aims/mesh/surface.h>
#include <aims/mesh/texture.h>
#include <vector>

namespace aims
{

  /** Cut meshes by a plane. The output of the operation is one cut mesh per
      input mesh, and possibly a border polygon mesh, and a plane intersection
      mesh.

      The plane mesh is not always correct, it is now preferred to use GLU's
      tesselation algorithm.

      To use it:

      * initialize using a constructor and/or set input parameters (see also
        the CutTexturedMesh subclasses to handle textured meshes)
      * call cut() or cutBorder(), these are the methods actually doing things
      * get outputs using cutMeshes(), borderLine(), planeMesh() and
        CutTexturedMesh::cutTextures()
  */
  class CutMesh
  {
  public:
    CutMesh( const AimsSurfaceTriangle & insurf, const Point4df & plane );
    CutMesh( const std::vector<const AimsSurfaceTriangle *> & insurf,
             const Point4df & plane );
    virtual ~CutMesh();

    /// Perform calculations
    void cut( bool buildborderline = true, bool meshplane = false,
              bool checkplane = true );
    /// build border line mesh only
    void cutBorder( int timestep = 0 );

    /// get output
    std::vector<carto::rc_ptr<AimsSurfaceTriangle> > cutMeshes() const
    { return _cut; }
    /// get output
    carto::rc_ptr<AimsTimeSurface<2, Void> > borderLine() const
    { return _borderline; }
    /// get output
    carto::rc_ptr<AimsSurfaceTriangle> planeMesh() const
    { return _planemesh; }

  protected:
    virtual void initializeOutputTextures() {}
    /** this callback is called to handle textures: add tex corresponding to a
        vertex in the given mesh */
    virtual void addTexturePoint( int /* mesh */, uint /* v */ ) {}
    /** this callback is called to handle textures: add tex corresponding to an
        interpolation between two vertices in the given mesh */
    virtual void addTextureInterpolPoint( int /* mesh */, uint /* v */, float /* w1 */,
                                          uint /* w */, float /* w2 */ )
    {}

    std::vector<const AimsSurfaceTriangle *> _insurf;
    std::vector<carto::rc_ptr<AimsSurfaceTriangle> > _insurfrc;
    Point4df _plane;

    std::vector<carto::rc_ptr<AimsSurfaceTriangle> > _cut;
    carto::rc_ptr<AimsTimeSurface<2, Void> > _borderline;
    carto::rc_ptr<AimsSurfaceTriangle> _planemesh;
  };


  /** Textured variant of CutMesh. This specialization also cuts a set of
      textures mapped on each of the meshes to be cut.
  */
  template <typename T>
  class CutTexturedMesh : public CutMesh
  {
  public:
    CutTexturedMesh( const std::vector<const AimsSurfaceTriangle *> & insurf,
                     const std::vector<std::vector<const TimeTexture<T> *> >
                     & intex,
                     const Point4df & plane );
    CutTexturedMesh( const std::vector<const AimsSurfaceTriangle *> & insurf,
                     const std::vector<std::vector<
                      carto::rc_ptr<TimeTexture<T> > > > & intex,
                     const Point4df & plane );
    virtual ~CutTexturedMesh();

    void setTextures(
      const std::vector<std::vector<carto::rc_ptr<TimeTexture<T> > > >
      & intex );
    void setTextures(
      const std::vector<std::vector<const TimeTexture<T> *> > & intex );

    /// get output
    std::vector<std::vector<carto::rc_ptr<TimeTexture<T> > > >
      cutTextures() const
    { return _cuttex; }

  protected:
    virtual void initializeOutputTextures();
    virtual void addTexturePoint( int mesh, uint v );
    virtual void addTextureInterpolPoint( int mesh, uint v, float w1,
                                          uint w, float w2 );

  private:
    std::vector<std::vector<const TimeTexture<T> *> > _intex;
    std::vector<std::vector<carto::rc_ptr<TimeTexture<T> > > > _intexrc;

    std::vector<std::vector<carto::rc_ptr<TimeTexture<T> > > > _cuttex;
  };

}

#endif

