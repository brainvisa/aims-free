
#ifndef AIMS_FOLDGRAPH_FOLDGRAPHATTRIBUTES
#define AIMS_FOLDGRAPH_FOLDGRAPHATTRIBUTES

#include <cartodata/volume/volume.h>
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
    FoldGraphAttributes( const carto::rc_ptr<carto::Volume<int16_t> > & skel,
                         Graph & graph,
                         const AffineTransformation3d *
                           talairachAffineTransformation3d = 0,
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

    carto::VolumeRef<int16_t> &getDepth();
    const carto::VolumeRef<int16_t> &getDepth() const;
    carto::VolumeRef<int16_t> &getNDepth();
    const carto::VolumeRef<int16_t> &getNDepth() const;
    carto::VolumeRef<float> &getBrainDepth();
    carto::VolumeRef<float> &getBrainDepthGradX();
    carto::VolumeRef<float> &getBrainDepthGradY();
    carto::VolumeRef<float> &getBrainDepthGradZ();
    const carto::VolumeRef<float> &getBrainDepth() const;
    const carto::VolumeRef<float> &getBrainDepthGradX() const;
    const carto::VolumeRef<float> &getBrainDepthGradY() const;
    const carto::VolumeRef<float> &getBrainDepthGradZ() const;
    carto::VolumeRef<float> &getDilatedDepth();
    carto::VolumeRef<float> &getDilatedDepthGradX();
    carto::VolumeRef<float> &getDilatedDepthGradY();
    carto::VolumeRef<float> &getDilatedDepthGradZ();
    const carto::VolumeRef<float> &getDilatedDepth() const;
    const carto::VolumeRef<float> &getDilatedDepthGradX() const;
    const carto::VolumeRef<float> &getDilatedDepthGradY() const;
    const carto::VolumeRef<float> &getDilatedDepthGradZ() const;
    float getDepthfactor() const;
    void prepareDepthMap();
    void prepareBrainDepthMap();
    /// outputs the voronoi in brain hull from hull junctions
    carto::VolumeRef<int16_t> rebuildCorticalRelations();
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
    void prepareNormalizedDepthMap(
      const carto::rc_ptr<carto::Volume<int16_t> > & th );
    void prepareDilatedDepthMap(
      const carto::rc_ptr<carto::Volume<int16_t> > & th );
    void prepareGradientX();
    void prepareGradientY();
    void prepareGradientZ();
    void prepareBrainGradientX();
    void prepareBrainGradientY();
    void prepareBrainGradientZ();

    carto::VolumeRef<int16_t>	_skel;
    Graph		& _graph;
    int16_t		_inside;
    int16_t		_outside;
    const AffineTransformation3d	*_motion;
    ///depth map from brain hull constrainted to sulci
    carto::VolumeRef<int16_t>	_depth;
    carto::VolumeRef<int16_t>	_ndepth;
    ///depth map from brain hull unconstrainted (whole brain hull)
    carto::VolumeRef<float>	_braindepthmap;
    ///braindepthmap X gradient
    carto::VolumeRef<float>	_braindepthmap_gradX;
    ///braindepthmap Y gradient
    carto::VolumeRef<float>	_braindepthmap_gradY;
    ///braindepthmap Z gradient
    carto::VolumeRef<float>	_braindepthmap_gradZ;
    ///depth map from brain hull constrainted to dilated sulci
    carto::VolumeRef<float>	_dilated_depthmap;
    ///braindepthmap X gradient
    carto::VolumeRef<float>	_dilated_depthmap_gradX;
    ///braindepthmap Y gradient
    carto::VolumeRef<float>	_dilated_depthmap_gradY;
    ///braindepthmap Z gradient
    carto::VolumeRef<float>	_dilated_depthmap_gradZ;
    bool		_domeshes;
    std::vector<int>    _graphversion;
    int                 _maxthreads;
    Mesher              _mesher;
  };

}

#endif


