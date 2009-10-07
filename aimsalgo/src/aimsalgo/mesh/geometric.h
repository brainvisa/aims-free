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
inline Point3df cross(Point3df a, Point3df b)
{
  
  Point3df n;
  n[0] = a[1]*b[2] - a[2]*b[1];
  n[1] = a[2]*b[0] - a[0]*b[2];
  n[2] = a[0]*b[1] - a[1]*b[0];
  return (n);
}

namespace aims
{
  class GeometricProperties 
  {
  public:
    typedef std::vector< std::list<unsigned> > NeighborList;
    typedef std::vector< std::list<float> > WeightNeighborList;
    typedef std::vector<float> WeightList;

  protected:
    GeometricProperties(const AimsSurfaceTriangle & mesh);
    virtual ~GeometricProperties(); //car class derivee

    void doPhi();
    void doTheta();
    void doAlpha();
    void doSimpleAlpha();
    void doBeta();
    void doDot();
    void doSurface();
    void doNeighbor();
    
    const WeightNeighborList & getPhi() const ;
    const WeightNeighborList & getTheta() const ;
    const WeightNeighborList & getDot() const ;
    const WeightNeighborList & getSurface() const ;
    const WeightList & getAlpha() const ;
    const WeightList & getSimpleAlpha() const ;
    const WeightList & getBeta() const ;
    const NeighborList & getNeighbor() const;
    const AimsSurfaceTriangle & getMesh() const;

  private:
    const AimsSurfaceTriangle & _mesh;
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
    
  };
  
  class Curvature : public GeometricProperties
  {
  public:
    Curvature(const AimsSurfaceTriangle & mesh);
    virtual ~Curvature();
    virtual Texture<float> doIt() = 0; //car defini dnas les classes derivees -> classe abstraite non instantiable
    static void regularize(Texture<float> & tex, float ratio); // pas lie a une instance de la classe
    static void getTextureProperties(const Texture<float> & tex); // pas lie a une instance de la classe
  };

  class CurvatureFactory
  {
  public:
    Curvature * createCurvature(const AimsSurfaceTriangle & mesh, const std::string & method );
      
  };

  class FiniteElementCurvature : public Curvature
  {
  public:
    FiniteElementCurvature(const AimsSurfaceTriangle & mesh);
    virtual ~FiniteElementCurvature();
    virtual Texture<float> doIt();  
  };

  
  // All these are Mean Curvature estimates
  
  class BoixCurvature : public Curvature
  {
  public:
    BoixCurvature(const AimsSurfaceTriangle & mesh);
    virtual ~BoixCurvature();
    virtual Texture<float> doIt();  
  };

  class BarycenterCurvature : public Curvature
  {
  public:
    BarycenterCurvature(const AimsSurfaceTriangle & mesh);
    virtual ~BarycenterCurvature();
    virtual Texture<float> doIt();  
  };
  
  // This is a Gaussian Curvature estimate
  
  class BoixGaussianCurvature : public Curvature
  {
  public:
    BoixGaussianCurvature(const AimsSurfaceTriangle & mesh);
    virtual ~BoixGaussianCurvature();
    virtual Texture<float> doIt();  
  };

  

}



#endif
