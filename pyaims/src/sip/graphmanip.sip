
namespace aims
{

class GraphManip
{
%TypeHeaderCode
#include <aims/graph/graphmanip.h>
#include <aims/mesh/surface.h>
#ifndef PYAIMS_RCPTR_AimsTimeSurface_3_DEFINED
#define PYAIMS_RCPTR_AimsTimeSurface_3_DEFINED
typedef carto::rc_ptr<AimsTimeSurface<3,Void> > rc_ptr_AimsTimeSurface_3;
#endif
#include <aims/bucket/bucket.h>
#ifndef PYAIMS_RCPTR_BucketMap_VOID_DEFINED
#define PYAIMS_RCPTR_BucketMap_VOID_DEFINED
typedef carto::rc_ptr<aims::BucketMap<Void> > rc_ptr_BucketMap_VOID;
#endif
%End

public:
  static Motion talairach( const Graph & g );
  static void storeTalairach( Graph & g, const Motion & m );
  static void buckets2Volume( Graph & g );
  static void volume2Buckets( Graph & g );
  static void storeAims( Graph & graph, GraphObject* vertex,
                         const std::string & attribute,
                         rc_ptr_AimsTimeSurface_3 obj );
  static void storeAims( Graph & graph, GraphObject* vertex,
                         const std::string & attribute,
                         rc_ptr_AimsTimeSurface_2 obj );

  static void storeAims( Graph & graph, GraphObject* vertex,
                         const std::string & attribute,
                         rc_ptr_BucketMap_VOID obj );
  static void setAttributeColor( Graph & graph, const std::string & att,
                                 const AimsRGB & );
  static void setAttributeColor( Graph & graph, const std::string & att,
                                 const AimsRGBA & );
  static void setAttributeColor( Graph & graph, const std::string & att,
                                 const vector_S32 & );
  vector_S32 attributeColor( const Graph & graph, const std::string & att );
  static void completeGraph( Graph & );
};

};

