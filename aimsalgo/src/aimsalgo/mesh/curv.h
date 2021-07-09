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



#ifndef AIMS_MESH_MESHCURVATURE_H
#define AIMS_MESH_MESHCURVATURE_H

#include <aims/mesh/texture.h>
#include <aims/mesh/surface.h>
#include <aims/data/data.h>
#include <list>

inline float fsign(float x);
inline Point3df cross(Point3df a, Point3df b); 

/// obsolete, see Curvature clases in <aims/mesh/geometric.h>

TimeTexture<float> AimsMeshCurvature( const AimsSurface<3,Void> & mesh);

/// obsolete, see Curvature clases in <aims/mesh/geometric.h>
//Estimation of the time step dt for the diffusion equation.
float AimsMeshFiniteElementDt( const Texture<float> &tex,
			       const Texture<float> &lapl,
			       float tmax);

/// obsolete, see Curvature clases in <aims/mesh/geometric.h>
//Estimation of the curvature. Boix method
Texture<float> AimsMeshBoixCurvature( const AimsSurface<3,Void> & mesh,
				      const std::vector<float> & ALPHA,
				      const std::vector<float> & BETA,
				      const std::vector<std::list<float> > & SURFACE);

/// obsolete, see Curvature clases in <aims/mesh/geometric.h>
//Regularization of a texture map.
//Definition of the Outliers from the histogram
Texture<float> AimsRegularizeTexture( const Texture<float> &tex,
				      const AimsSurface<3,Void> & mesh, 
				      float alpha ); 

/// obsolete, see Curvature clases in <aims/mesh/geometric.h>
Texture<float> AimsRegularizeTexture(const Texture<float> & tex,
				     float ratio );
/// obsolete, see Curvature clases in <aims/mesh/geometric.h>
//Estimation of the curvature (finite element method)
Texture<float> AimsMeshFiniteElementCurvature( const AimsSurface<3,Void> & mesh,
						   const std::vector< std::list<unsigned> > & neighbourso,
						   const std::vector< std::list<float> > & PHI,
						   const std::vector< std::list<float> > & THETA,
						   const std::vector< std::list<float> > & SURFACE,
						   const std::vector< std::list<float> > & DOT);

/// obsolete, see Curvature clases in <aims/mesh/geometric.h>
//Eequired for the curvature and the laplacian estimation (finite element method).
std::vector< std::list<float> > AimsMeshFiniteElementPhi( const AimsSurface<3,Void> & mesh,
						const std::vector< std::list<unsigned> > & neighbourso,
						const std::vector< std::list<float> > & surf);

/// obsolete, see Curvature clases in <aims/mesh/geometric.h>
//Eequired for the curvature and the laplacian estimation (finite element method).
std::vector< std::list<float> > AimsMeshFiniteElementTheta( const AimsSurface<3,Void> & mesh,
						  const std::vector< std::list<unsigned> > & neighbourso,
						  const std::vector< std::list<float> > & surf);
/// obsolete, see Curvature clases in <aims/mesh/geometric.h>
//Eequired for the curvature and the laplacian estimation (finite element method).
std::vector<float> AimsMeshFiniteElementAlpha( const AimsSurface<3,Void> & mesh,
					  const std::vector< std::list<unsigned> > & neighbourso);

/// obsolete, see Curvature clases in <aims/mesh/geometric.h>
//Eequired for the curvature  estimation (boix method).
std::vector<float>  AimsMeshFiniteElementBeta( const AimsSurface<3,Void> & mesh,
					  const std::vector< std::list<unsigned> > & neighbourso);

/// obsolete, see Curvature clases in <aims/mesh/geometric.h>
//Required for the curvature and the laplacian estimation (finite element and boix method).
//The first list element for each coordinate i of the vector
//Correspond to the total surface of the patch surrounding the node i.
std::vector< std::list<float> > AimsMeshFiniteElementSurface( const AimsSurface<3,Void> & mesh,
						    const std::vector< std::list<unsigned> > & neighbourso);

/// obsolete, see Curvature clases in <aims/mesh/geometric.h>
//Eequired for the curvature and the laplacian estimation (finite element method).
std::vector< std::list<float> > AimsMeshFiniteElementDot( const AimsSurface<3,Void> & mesh,
						const std::vector< std::list<unsigned> > & neighbourso);


//Give for each node the clockwise ordered list of neighbours nodes
std::vector< std::list<unsigned> > AimsMeshOrderNode(const AimsSurface<3,Void> & mesh);

//Give for each node the clockwise ordered list of neighbours triangles.
std::vector< std::list<unsigned> > AimsMeshOrderTriangle(const AimsSurface<3,Void> & mesh);

//Estimation of the laplacian of a texture map; (finite element method).

Texture<float> AimsMeshLaplacian( const Texture<float> &inittex, 
				  const std::map<unsigned, 
				  std::set< std::pair<unsigned,
				  float> > > &lapl);

template <typename T>
void AimsMeshLaplacian( const std::vector<T> &inittex,
                        std::vector<T> & outtex,
                        const std::map<unsigned,
                          std::set< std::pair<unsigned,float> > > &lapl);


std::map<unsigned, std::set< std::pair<unsigned,float> > >  AimsMeshWeightFiniteElementLaplacian( const AimsSurface<3,Void> & mesh, 
					       const float Wmax);

namespace aims
{
  typedef std::map<unsigned, std::set< std::pair<unsigned,float> > >
    LaplacianWeights;

  void makeLaplacianMatrix( const LaplacianWeights & weights,
                            LaplacianWeights & lmat, float dt );

  LaplacianWeights* sparseMult( const LaplacianWeights & in1,
                                const LaplacianWeights & in2,
                                float sparseThresh=0 );

  template <typename T>
  void applyLaplacianMatrix( const std::vector<T> &inittex,
                             std::vector<T> & outtex,
                             const LaplacianWeights &lapl );

  void laplacianMatrixThreshold( LaplacianWeights & lmat, float threshold );

  /** Compute Laplacian smoothing coefficients matrix for niter smoothing
      iterations.
      Input weights should be the output of
      AimsMeshWeightFiniteElementLaplacian().
      sparseThresh can be used to threshold the weights of the sparse matrix
      at each smoothing iteration, to reduce the sparse matrix size.
   */
  LaplacianWeights* makeLaplacianSmoothingCoefficients(
    const LaplacianWeights & weights, unsigned niter, float dt,
    float sparseThresh=0 );

}

#endif
