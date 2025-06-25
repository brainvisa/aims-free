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



#ifndef AIMS_MESH_GEOMETRIC_H
#define AIMS_MESH_GEOMETRIC_H

#include <aims/mesh/texture.h>
#include <aims/mesh/surface.h>
#include <aims/mesh/mesh_graph.h>
#include <cartobase/smart/rcptr.h>
#include <list>

template <class T>
inline float sign(T x) 
{
  if (x>(T)0) return(1);
  else
    if (x<(T)0) return(-1);
    else
      return(0);
}

 

// Cross product between two 3D points
inline Point3df cross( const Point3df & a, const Point3df & b )
{
  
  Point3df n;
  n[0] = a[1]*b[2] - a[2]*b[1];
  n[1] = a[2]*b[0] - a[0]*b[2];
  n[2] = a[0]*b[1] - a[1]*b[0];
  return n;
}

// Cross product between two 2D points
inline float cross( const Point2df & a, const Point2df & b )
{
  return (a[0] * b[1]) - (a[1] * b[0]);
}

namespace aims
{

  class GeometricProperties 
  {
  public:
    typedef std::list<unsigned> Neighborhood;
    typedef std::vector< Neighborhood > NeighborList;
    typedef std::vector< std::list<float> > WeightNeighborList;
    typedef std::vector<float> WeightList;
    typedef typename
      meshgraph::MeshVertexNode<uint>::VertexIndexCollection::value_type
      VertexPointer;

    /// constructor working on a const mesh
    GeometricProperties( const AimsSurfaceTriangle & mesh );
    /// constructor working on a non-const mesh
    GeometricProperties( carto::rc_ptr<AimsSurfaceTriangle> mesh );
    virtual ~GeometricProperties();

    const WeightNeighborList & getPhi() const ;
    const WeightNeighborList & getTheta() const ;
    const WeightNeighborList & getDot() const ;
    const WeightNeighborList & getSurface() const ;
    const WeightList & getAlpha() const ;
    const WeightList & getSimpleAlpha() const ;
    const WeightList & getBeta() const ;
    const AimsSurfaceTriangle & getMesh() const;
    carto::rc_ptr<AimsSurfaceTriangle> getRcMesh();
    const NeighborList & getNeighbor() const;
    NeighborList & getNeighbor();
    const NeighborList & getTriangleNeighbor() const;
    NeighborList & getTriangleNeighbor();
    meshgraph::MeshGraphVertices & getVertices()
    { return _graphvertices; }
    const meshgraph::MeshGraphVertices & getVertices() const
    { return _graphvertices; }
    meshgraph::MeshGraphFaces & getFaces() { return _graphfaces; }
    const meshgraph::MeshGraphFaces & getFaces() const
    { return _graphfaces; }

//   protected:
    void doPhi();
    void doTheta();
    void doAlpha();
    void doSimpleAlpha();
    void doBeta();
    void doDot();
    void doSurface();
    void doNeighbor();
    void sortPolygons( Neighborhood &npoly );
    void buildSortVerticesNeighborhood( size_t i );
    void doGraph();
    void graphToMesh();

  private:
    const AimsSurfaceTriangle & _mesh;
    carto::rc_ptr<AimsSurfaceTriangle> _pmesh;
    NeighborList  _neighbourso ;
    NeighborList  _triangleNeighbourso ;
    WeightNeighborList  _phi ;
    WeightNeighborList _theta;
    WeightList		_alpha;       // this represents sum(l*l*cotan(alpha)) 
    WeightList		_simplealpha; // this is just sum(alpha)
    WeightList		_beta;
    WeightNeighborList		_dot;
    WeightNeighborList		_surface; 
    NeighborList doTriangleNeighbor() ;
    // using meshgraph
    meshgraph::MeshGraphVertices _graphvertices;
    meshgraph::MeshGraphFaces _graphfaces;

    friend class VertexRemover;
    
  };
  
  class Curvature : public GeometricProperties
  {
  public:
    Curvature( const AimsSurfaceTriangle & mesh );
    Curvature( carto::rc_ptr<AimsSurfaceTriangle> mesh );
    virtual ~Curvature();
    virtual Texture<float> doIt() = 0; //car defini dnas les classes derivees -> classe abstraite non instantiable
    static void regularize(Texture<float> & tex, float ratio); // pas lie a une instance de la classe
    static void getTextureProperties(const Texture<float> & tex); // pas lie a une instance de la classe
  };

  class CurvatureFactory
  {
  public:
    Curvature * createCurvature( const AimsSurfaceTriangle & mesh,
                                 const std::string & method );
    Curvature * createCurvature( carto::rc_ptr<AimsSurfaceTriangle> mesh,
                                 const std::string & method );

  };

  class FiniteElementCurvature : public Curvature
  {
  public:
    FiniteElementCurvature( const AimsSurfaceTriangle & mesh );
    FiniteElementCurvature( carto::rc_ptr<AimsSurfaceTriangle> mesh );
    virtual ~FiniteElementCurvature();
    virtual Texture<float> doIt();  
  };

  
  // All these are Mean Curvature estimates
  
  class BoixCurvature : public Curvature
  {
  public:
    BoixCurvature( const AimsSurfaceTriangle & mesh );
    BoixCurvature( carto::rc_ptr<AimsSurfaceTriangle> mesh );
    virtual ~BoixCurvature();
    virtual Texture<float> doIt();  
  };

  class BarycenterCurvature : public Curvature
  {
  public:
    BarycenterCurvature( const AimsSurfaceTriangle & mesh );
    BarycenterCurvature( carto::rc_ptr<AimsSurfaceTriangle> mesh );
    virtual ~BarycenterCurvature();
    virtual Texture<float> doIt();  
  };
  
  // This is a Gaussian Curvature estimate
  
  class BoixGaussianCurvature : public Curvature
  {
  public:
    BoixGaussianCurvature( const AimsSurfaceTriangle & mesh );
    BoixGaussianCurvature( carto::rc_ptr<AimsSurfaceTriangle> mesh );
    virtual ~BoixGaussianCurvature();
    virtual Texture<float> doIt();  
  };

  // This is another Gaussian Curvature estimate (from aims-til/Cathier)

  class GaussianCurvature : public Curvature
  {
  public:
    GaussianCurvature( const AimsSurfaceTriangle & mesh );
    GaussianCurvature( carto::rc_ptr<AimsSurfaceTriangle> mesh );
    virtual ~GaussianCurvature();
    virtual Texture<float> doIt();
    void localProcess( size_t i_vert, float & gaussianCurvature,
                       float & meanCurvature,
                       std::pair<float, float> & principalCurvatures,
                       float & orientedMeanCurvature,
                       float & orientedGaussianCurvature,
                       Point3df & normal, float & voronoiArea );
    void localProcess( const VertexPointer & i,
                       float & gaussianCurvature,
                       float & meanCurvature,
                       std::pair<float, float> & principalCurvatures,
                       float & orientedMeanCurvature,
                       float & orientedGaussianCurvature,
                       Point3df & normal, float & voronoiArea );
  };


  /** A class to remove a vertex from a mesh, and remeshing the hole.

      NB: remeshing is not always possible, therefore removing a given point in
      a mesh is not always possible.
      Therefore, it may be important to check the return value of operator() to
      check if the point has actually been removed.
  */
  class VertexRemover
  {
  public: // typedefs

    typedef typename meshgraph::MeshVertexNode<uint>::VertexIndexCollection::value_type
      VertexPointer;

    // constructors

    VertexRemover( carto::rc_ptr<GeometricProperties> geom );
    VertexRemover( carto::rc_ptr<AimsSurfaceTriangle> mesh );

    // set & get

    const GeometricProperties & geometricProperties() const
    { return *_geom; }
    GeometricProperties & geometricProperties() { return *_geom; }

    // functions

//     bool operator()( size_t i );
    bool operator()( VertexPointer & i );

    /// clean a whole mesh
    static void cleanMesh( AimsSurfaceTriangle & mesh, float maxCurv = 0.5,
                           bool verbose=false );

  private: // functions

  // NB: neighbors should be a std::vector so far
  //   template < typename Neighborhood >
  std::vector<Point2df>
  simple_neighborhood_flattening(
    const Point3df & point,
    const GeometricProperties::Neighborhood & neighbors );

  std::vector<Point2df>
  simple_neighborhood_flattening(
    const Point3df & point,
    const std::vector<VertexPointer> & neighbors );

    // data, input
    carto::rc_ptr<GeometricProperties> _geom;

  };


  /** SimpleDelaunayTriangulation

      Get a Delaunay triangulation for a polygon. Poitns in the polygon should
      be ordered in a clockwise or counterclockwise order.

      Note that it is different from a classical Delaunay algorithm wich meshes
      a cloud point (and its convex hull), not a polygon as here.
  */
  std::list<AimsVector<uint, 3> >
  simple_delaunay_triangulation( const std::vector<Point2df> & points );

}



#endif
