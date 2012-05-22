
namespace aims
{
class FoldGraphAttributes
{
%TypeHeaderCode
  #include <aims/foldgraph/foldgraphattributes.h>
%End
  public:
    FoldGraphAttributes( const AimsData_S16 & skel, Graph & graph,
                         const Motion *talairachMotion = 0,
                         int inside = 0, int outside = 11,
                         bool withmeshes = true );

    void cleanup();
    void doAll();

    void makeMeshes();
    void makeGlobalAttributes();
    void makeSimpleSurfaceAttributes();
    void makeJunctionAttributes();
    void makePliDePassageAttributes();
    void makeCorticalRelationAttributes();
    void makeSummaryGlobalAttributes();

    AimsData_S16 &getNDepth(); 
    AimsData_S16 &getDepth(); 
    AimsData_FLOAT &getBrainDepth();
    AimsData_FLOAT &getBrainDepthGradX();
    AimsData_FLOAT &getBrainDepthGradY();
    AimsData_FLOAT &getBrainDepthGradZ();
    AimsData_FLOAT &getDilatedDepth();
    AimsData_FLOAT &getDilatedDepthGradX();
    AimsData_FLOAT &getDilatedDepthGradY();
    AimsData_FLOAT &getDilatedDepthGradZ();
    float getDepthfactor();
    void prepareDepthMap();
    void prepareBrainDepthMap();
    AimsData_S16 rebuildCorticalRelations();
    void thickness( const BucketMap_FLOAT & fm,
                    const rc_ptr_Volume_S16 voronoi );
    void greyAndCSFVolumes( const rc_ptr_Volume_S16 gw,
                            const rc_ptr_Volume_S16 voronoi );

};


class FoldArgOverSegment
{
%TypeHeaderCode
#include <aims/foldgraph/foldoversegmentation.h>
%End

public:
  FoldArgOverSegment( Graph* g );
  ~FoldArgOverSegment();
  Vertex * splitVertex( Vertex* v, const Point3d & pos, size_t minsize = 50 );
  Vertex * splitVertex( Vertex* v,
                        const list_POINT3D & splitline,
                        size_t minsize = 50 );
  Vertex * splitVertex( Vertex* v,
                        const list_POINT3DF & splitline,
                        size_t minsize = 50 );
  Vertex * splitVertex( Vertex* v,
                        rc_ptr_BucketMap_VOID splitline,
                        size_t minsize = 50 );
  void printSplitInSkeleton( rc_ptr_Volume_S16 skel,
                             const Vertex* v1, const Vertex* v2 );
  int subdivizeVertex( Vertex* v, float piecelength = 50,
                       size_t minsize = 50, set_VertexPtr* = 0 );
  int subdivizeGraph( float piecelength = 50, size_t minsize = 50,
                      set_VertexPtr* = 0 );
  static rc_ptr_BucketMap_VOID splitLineOnBucket(
    rc_ptr_BucketMap_VOID bucket, const list_POINT3D & pos );
  static SIP_PYTUPLE splitSimpleSurface(
      rc_ptr_BucketMap_VOID bucket,
      rc_ptr_BucketMap_VOID splitline );
%MethodCode
  carto::rc_ptr<aims::BucketMap<Void> > ss1, ss2;
  bool ok = aims::FoldArgOverSegment::splitSimpleSurface( *a0, *a1, ss1, ss2 );
  if( !ok )
    sipRes = Py_None;
  else
  {
    sipRes = PyTuple_New( 2 );
    PyTuple_SetItem( sipRes, 0, sipConvertFromType(
      new carto::rc_ptr<aims::BucketMap<Void> >( ss1 ),
      sipType_rc_ptr_BucketMap_VOID, Py_None ) );
    PyTuple_SetItem( sipRes, 1, sipConvertFromType(
      new carto::rc_ptr<aims::BucketMap<Void> >( ss2 ),
      sipType_rc_ptr_BucketMap_VOID, Py_None ) );
  }
%End
};

};
