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


#ifndef AIMS_FOLDGRAPH_FOLDGRAPHATTRIBUTES
#define AIMS_FOLDGRAPH_FOLDGRAPHATTRIBUTES

#include <aims/data/data.h>
class Graph;
class Motion;

namespace aims
{

  /// Creation of cortical folds graph attributes
  class FoldGraphAttributes
  {
  public:
    /** skel is the skeleton image of the hemisphere, It must be allocated 
        with a border of at least 1 voxel width. */
    FoldGraphAttributes( const AimsData<int16_t> & skel, Graph & graph, 
                         const Motion *talairachMotion = 0, 
                         int16_t inside = 0, int16_t outside = 11, 
                         bool withmeshes = true,
                         const std::vector<int> & graphversion
                             = std::vector<int>() );
    /// frees the depth map
    void cleanup();
    /// creates all attributes (global + all nodes/relations types)
    void doAll();

    void makeMeshes();
    void makeGlobalAttributes();
    void makeSimpleSurfaceAttributes();
    void makeJunctionAttributes();
    void makePliDePassageAttributes();
    void makeCorticalRelationAttributes();
    void makeSummaryGlobalAttributes();

    AimsData<int16_t> &getDepth();
    const AimsData<int16_t> &getDepth() const;
    AimsData<int16_t> &getNDepth();
    const AimsData<int16_t> &getNDepth() const;
    AimsData<float> &getBrainDepth();
    AimsData<float> &getBrainDepthGradX();
    AimsData<float> &getBrainDepthGradY();
    AimsData<float> &getBrainDepthGradZ();
    const AimsData<float> &getBrainDepth() const;
    const AimsData<float> &getBrainDepthGradX() const;
    const AimsData<float> &getBrainDepthGradY() const;
    const AimsData<float> &getBrainDepthGradZ() const;
    AimsData<float> &getDilatedDepth();
    AimsData<float> &getDilatedDepthGradX();
    AimsData<float> &getDilatedDepthGradY();
    AimsData<float> &getDilatedDepthGradZ();
    const AimsData<float> &getDilatedDepth() const;
    const AimsData<float> &getDilatedDepthGradX() const;
    const AimsData<float> &getDilatedDepthGradY() const;
    const AimsData<float> &getDilatedDepthGradZ() const;
    float getDepthfactor() const;
    void prepareDepthMap();
    void prepareBrainDepthMap();
    /// outputs the voronoi in brain hull from hull junctions
    AimsData<int16_t> rebuildCorticalRelations();
    /** returns a vector value of 2 or more numbers from the target graph
        version, or the current cartograph library version
    */
    std::vector<int> graphVersion() const;

  private:
    AimsData<int16_t>	_skel;
    Graph		& _graph;
    int16_t		_inside;
    int16_t		_outside;
    const Motion	*_motion;
    ///depth map from brain hull constrainted to sulci
    AimsData<int16_t>	_depth;
    AimsData<int16_t>	_ndepth;
    ///depth map from brain hull unconstrainted (whole brain hull)
    AimsData<float>	_braindepthmap;
    ///braindepthmap X gradient
    AimsData<float>	_braindepthmap_gradX;
    ///braindepthmap Y gradient
    AimsData<float>	_braindepthmap_gradY;
    ///braindepthmap Z gradient
    AimsData<float>	_braindepthmap_gradZ;
    ///depth map from brain hull constrainted to dilated sulci
    AimsData<float>	_dilated_depthmap;
    ///braindepthmap X gradient
    AimsData<float>	_dilated_depthmap_gradX;
    ///braindepthmap Y gradient
    AimsData<float>	_dilated_depthmap_gradY;
    ///braindepthmap Z gradient
    AimsData<float>	_dilated_depthmap_gradZ;
    bool		_domeshes;
    std::vector<int>    _graphversion;
  };

}

#endif


