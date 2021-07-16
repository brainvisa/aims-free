
%ModuleHeaderCode
#include <aims/mesh/geometric.h>
%End

namespace aims
{
  class GeometricProperties
  {
  public:
    typedef list_U32 Neighborhood;
    typedef vector_list_U32 NeighborList;

    typedef vector_FLOAT WeightList;

    /*
    typedef std::vector< std::list<float> > WeightNeighborList;
    typedef std::vector<float> WeightList;
    typedef typename
      meshgraph::MeshVertexNode<uint>::VertexIndexCollection::value_type
      VertexPointer; */

    GeometricProperties( const AimsSurfaceTriangle & mesh );
    GeometricProperties( rc_ptr_AimsTimeSurface_3_VOID mesh );
    virtual ~GeometricProperties();

    /*
    const WeightNeighborList & getPhi() const ;
    const WeightNeighborList & getTheta() const ;
    const WeightNeighborList & getDot() const ;
    const WeightNeighborList & getSurface() const ;
    */
    const WeightList & getAlpha() const ;
    const WeightList & getSimpleAlpha() const ;
    const WeightList & getBeta() const ;
    const AimsSurfaceTriangle & getMesh() const;
    rc_ptr_AimsTimeSurface_3_VOID getRcMesh();

    GeometricProperties::NeighborList & getNeighbor();
    GeometricProperties::NeighborList & getTriangleNeighbor();

    void doPhi();
    void doTheta();
    void doAlpha();
    void doSimpleAlpha();
    void doBeta();
    void doDot();
    void doSurface();
    void doNeighbor();
    void sortPolygons( aims::GeometricProperties::Neighborhood &npoly );
    void buildSortVerticesNeighborhood( size_t i );
    void doGraph();
    void graphToMesh();
  };


  class Curvature : aims::GeometricProperties
  {
  public:
    Curvature( const AimsSurfaceTriangle & mesh );
    Curvature( rc_ptr_AimsTimeSurface_3_VOID mesh );
    virtual ~Curvature();
    virtual Texture_FLOAT doIt() = 0;
    static void regularize(Texture_FLOAT & tex, float ratio);
    static void getTextureProperties(const Texture_FLOAT & tex);
  };

  class CurvatureFactory
  {
  public:
    Curvature * createCurvature( const AimsSurfaceTriangle & mesh,
                                 const std::string & method );

    Curvature * createCurvature( rc_ptr_AimsTimeSurface_3_VOID mesh,
                                 const std::string & method );
  };

  class FiniteElementCurvature : public aims::Curvature
  {
  public:
    FiniteElementCurvature( const AimsSurfaceTriangle & mesh );
    FiniteElementCurvature( rc_ptr_AimsTimeSurface_3_VOID mesh );
    virtual ~FiniteElementCurvature();
    virtual Texture_FLOAT doIt();
  };


  // All these are Mean Curvature estimates

  class BoixCurvature : public aims::Curvature
  {
  public:
    BoixCurvature( const AimsSurfaceTriangle & mesh );
    BoixCurvature( rc_ptr_AimsTimeSurface_3_VOID mesh );
    virtual ~BoixCurvature();
    virtual Texture_FLOAT doIt();
  };

  class BarycenterCurvature : public aims::Curvature
  {
  public:
    BarycenterCurvature( const AimsSurfaceTriangle & mesh );
    BarycenterCurvature( rc_ptr_AimsTimeSurface_3_VOID mesh );
    virtual ~BarycenterCurvature();
    virtual Texture_FLOAT doIt();
  };

  // This is a Gaussian Curvature estimate

  class BoixGaussianCurvature : public aims::Curvature
  {
  public:
    BoixGaussianCurvature( const AimsSurfaceTriangle & mesh );
    BoixGaussianCurvature( rc_ptr_AimsTimeSurface_3_VOID mesh );
    virtual ~BoixGaussianCurvature();
    virtual Texture_FLOAT doIt();
  };

  // This is another Gaussian Curvature estimate (from aims-til/Cathier)

  class GaussianCurvature : public aims::Curvature
  {
  public:
    GaussianCurvature( const AimsSurfaceTriangle & mesh );
    GaussianCurvature( rc_ptr_AimsTimeSurface_3_VOID mesh );
    virtual ~GaussianCurvature();
    virtual Texture_FLOAT doIt();
    /*
    void localProcess( size_t i_vert, float & gaussianCurvature,
                       float & meanCurvature,
                       std::pair<float, float> & principalCurvatures,
                       float & orientedMeanCurvature,
                       float & orientedGaussianCurvature,
                       Point3df & normal, float & voronoiArea );
    */
  };


  class VertexRemover
  {
%Docstring
      A class to remove a vertex from a mesh, and remeshing the hole.

      NB: remeshing is not always possible, therefore removing a given point in
      a mesh is not always possible.
      Therefore, it may be important to check the return value of operator() to
      check if the point has actually been removed.
%End

  public:
    VertexRemover( rc_ptr_GeometricProperties geom );
    VertexRemover( rc_ptr_AimsTimeSurface_3_VOID & mesh );
    aims::GeometricProperties & geometricProperties();

  };

};


vector_AimsVector_U32_3*
simple_delaunay_triangulation( const vector_POINT2DF & points ) /Factory/;
%Docstring
    triangles = simple_delaunay_triangulation(points)

    Get a Delaunay triangulation for a polygon. Poins in the polygon should
    be ordered in a clockwise or counterclockwise order.

    Note that it is different from a classical Delaunay algorithm wich meshes
    a cloud point (and its convex hull), not a polygon as here.
%End

%MethodCode
  std::list<AimsVector<uint, 3> > res
    = aims::simple_delaunay_triangulation( *a0 );
  sipRes = new vector_AimsVector_U32_3( res.begin(), res.end() );
%End


