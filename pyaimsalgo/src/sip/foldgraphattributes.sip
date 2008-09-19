
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

};


class FoldArgOverSegment
{
%TypeHeaderCode
#include <aims/foldgraph/foldoversegmentation.h>
%End

public:
  FoldArgOverSegment( Graph* g );
  ~FoldArgOverSegment();
  Vertex * splitVertex( Vertex* v, const Point3d & pos );
  rc_ptr_Volume_S16 argBackToSkeleton( const rc_ptr_Volume_S16 oldskel );
  void printSplitInSkeleton( rc_ptr_Volume_S16 skel,
                             const Vertex* v1, const Vertex* v2 );
};

};
