
/* Copyright (c) 1995-2007 CEA
 *
 *  This software and supporting documentation were developed by
 *      CEA/DSV/SHFJ
 *      4 place du General Leclerc
 *      91401 Orsay cedex
 *      France
 *
 * This software is governed by the CeCILL license version 2 under 
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the 
 * terms of the CeCILL license version 2 as circulated by CEA, CNRS
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
 * knowledge of the CeCILL license version 2 and that you accept its terms.
 */


#ifndef AIMS_SCALESPACE_MESHBLOB_H
#define AIMS_SCALESPACE_MESHBLOB_H

#include <aims/config/aimsalgo_config.h>
#include <aims/mesh/texture.h>
#include <aims/mesh/surface.h>
#include <list>
#include <vector>
#include <aims/mesh/curv.h>
#include <aims/scalespace/meshDiffuse.h>
#include <graph/graph/graph.h>
#include <graph/graph/graph.h>
#include <cartobase/smart/rcptr.h>


class Graph;


namespace aims
{

  class AIMSALGO_API ScaleSpace
  {
  public:
    enum MeshShape
    {
      Surface, 
      Tore
    };

    enum BlobSaddleType
    {
      Junction, // blob to blob
      Bottom
    };

    enum MeshGrowingMode
    {
      Translate, 
      Scale, 
      PushNormal
    };

    struct BlobDescriptor
    {
      unsigned		minimum;
      unsigned		maximum;
      BlobSaddleType	saddle;
      unsigned		neighbour;
    };

    enum MergeCode
    {
      OK = 0, 
      PbUp = 1, 
      PbDown = 2, 
      PbBoth = 3
    };

    Texture<int> meshBlobExtract( const AimsSurface<3,Void> & mesh, 
				  const Texture<float> & inittex, 
				  std::map<int, BlobDescriptor> & limits );

    int blobToGraph( Graph & g, const Texture<int> & blobs , 
		     const AimsSurfaceTriangle & mesh, const AimsSurfaceTriangle & infl_mesh, 
		     const Texture<float> & values, float scale, 
		     std::map<int, BlobDescriptor> & limits, int startindex );

    void cleanGraphScale( Graph & g, float scale);

    MergeCode mergeGraphs( Graph & g, Graph & h );

    void createSubMeshes( Graph & g, const AimsSurface<3,Void> & mesh, 
			  float depl = 0, float depl0 = 0, 
			  MeshShape ms = Surface, MeshGrowingMode gm = Scale, 
			  bool compressedMeshes = true);

    void grayLevel2PrimalSketch( Graph & glevel, Graph & psketch );


    std::multimap<float, unsigned> sortGLB(std::string key,
				      const std::map<unsigned, Vertex *> & nodes,
				      const std::map<unsigned, std::set<Vertex *> > & scblobs);


    void GLBVertexToSSBVertex(Graph & psketch,
			      const std::multimap<float, unsigned> & ordering,
			      const std::map<unsigned, Vertex *> & nodes,
			      const std::map<unsigned, std::set<Vertex *> > & scblobs);
    
    void  GLBEdgeToSSBEdge(Graph & glevel, Graph & psketch, 
			   const std::map<Vertex *, unsigned> & labels,
			   const std::map<unsigned, Vertex *> & nodes);

    void  setGraphLabelAndName(  const std::multimap<float, unsigned> &  ordering,
			       const std::map<unsigned, Vertex *> & nodes,
			       const std::map<unsigned, std::set<Vertex *> > & scblobs);

    float getRepresentativeScale(const std::set<Vertex *> & SSB);


    carto::rc_ptr<AimsSurfaceTriangle> getRepresentativeBlobMesh(float scale0, const std::set<Vertex *> & SSB);

    int adaptiveScaleSpace( Graph & psketch, 
			    const AimsSurfaceTriangle & mesh, 
                            const AimsSurfaceTriangle & infl_mesh,
			    const Texture1d & inittex,
			    std::map<float,Texture<int> > & tex_blobs,
			    std::map<float,Texture<float> > & tex_curv,
			    int index, float tbegin, float tend, 
			    float dt,float dts, float dtmin,
			    const float HMAX,bool logFlag,
			    const std::map<unsigned, 
			    std::set< std::pair<unsigned,float> > > 
			    & weightLapl);

    AimsSurfaceTriangle surface2Tore( const AimsSurfaceTriangle & );
  };

}

#endif


