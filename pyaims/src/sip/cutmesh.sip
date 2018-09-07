namespace aims
{

  class CutMesh
  {
%Docstring
Cut meshes by a plane. The output of the operation is one cut mesh per
input mesh, and possibly a border polygon mesh, and a plane intersection
mesh.

The plane mesh is not always correct, it is now preferred to use GLU's
tesselation algorithm.

To use it:

* initialize using a constructor and/or set input parameters (see also
  the CutTexturedMesh subclasses to handle textured meshes)
* call cut() or cutBorder(), these are the methods actually doing things
* get outputs using cutMeshes(), borderLine(), planeMesh() and
  CutTexturedMesh.cutTextures()
%End

%TypeHeaderCode
#include <aims/mesh/cutmesh.h>
%End

  public:
    CutMesh( const AimsSurfaceTriangle & insurf, const Point4df & plane );
/*    CutMesh( const std::vector<const AimsSurfaceTriangle *> & insurf,
             const Point4df & plane );
*/
    virtual ~CutMesh();

    void cut( bool buildborderline = true, bool meshplane = false,
              bool checkplane = true );
    void cutBorder( int timestep = 0 );

    vector_rc_ptr_AimsTimeSurface_3_VOID cutMeshes() const;
    rc_ptr_AimsTimeSurface_2_VOID borderLine() const;
    rc_ptr_AimsTimeSurface_3_VOID planeMesh() const;
  };


/* TODO, missing types
  class CutTexturedMesh_FLOAT : aims::CutMesh
  {
%Docstring
Textured variant of CutMesh. This specialization also cuts a set of
textures mapped on each of the meshes to be cut.
%End

%TypeHeaderCode
%End

  public:
    CutTexturedMesh_FLOAT( const vector_AimsSurfaceTriangleP & insurf,
                     const vector_vector_TimeTexture_FLOAT_P & intex,
                     const Point4df & plane );
    virtual ~CutTexturedMesh();

    void setTextures(
      const std::vector<std::vector<carto::rc_ptr<TimeTexture<T> > > >
      & intex );

    /// get output
    std::vector<std::vector<carto::rc_ptr<TimeTexture<T> > > >
      cutTextures() const;

  };
*/

};

