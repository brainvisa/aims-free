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


#ifndef AIMS_FOLDGRAPH_FOLDGRAPHATTRIBUTES
#define AIMS_FOLDGRAPH_FOLDGRAPHATTRIBUTES

#include <aims/data/data.h>
#include <aims/mesh/mesher.h>

class Graph;

namespace aims
{
  class AffineTransformation3d;
  template <typename T> class BucketMap;

  /// Creation of cortical folds graph attributes
  class FoldGraphAttributes
  {
  public:
    /** skel is the skeleton image of the hemisphere, It must be allocated 
        with a border of at least 1 voxel width. */
    FoldGraphAttributes( const AimsData<int16_t> & skel, Graph & graph, 
                         const AffineTransformation3d *talairachAffineTransformation3d = 0,
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
    void thickness( const BucketMap<float> & midinterfaceDistances,
                    const carto::VolumeRef<int16_t> voronoi );
    void greyAndCSFVolumes( const carto::VolumeRef<int16_t> gw,
                            const carto::VolumeRef<int16_t> voronoi );
    /** Sets the maxumum number of threads used in multithreaded-enabled
        parts. 1 means mono-threaded, 0 means une thread per CPU.
        A negative value means one thread per CPU, but never use more CPUs
        than the absolute value of the given number.
    */
    void setMaxThreads( int mt );
    int maxThreads() const { return _maxthreads; }
    Mesher & mesher() { return _mesher; }

  private:
    class DistanceMapThreadContext;
    friend class DistanceMapThreadContext;

    void prepareNativeDepthMap();
    void prepareNormalizedDepthMap( const AimsData<int16_t> & th );
    void prepareDilatedDepthMap( const AimsData<int16_t> & th );
    void prepareGradientX();
    void prepareGradientY();
    void prepareGradientZ();
    void prepareBrainGradientX();
    void prepareBrainGradientY();
    void prepareBrainGradientZ();

    AimsData<int16_t>	_skel;
    Graph		& _graph;
    int16_t		_inside;
    int16_t		_outside;
    const AffineTransformation3d	*_motion;
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
    int                 _maxthreads;
    Mesher              _mesher;
  };

}

#endif


