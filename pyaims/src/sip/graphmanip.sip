
namespace aims
{

class GraphManip
{
%TypeHeaderCode
#include <aims/graph/graphmanip.h>
#include <aims/mesh/surface.h>
#ifndef PYAIMS_RCPTR_AimsTimeSurface_3_VOID_DEFINED
#define PYAIMS_RCPTR_AimsTimeSurface_3_VOID_DEFINED
typedef carto::rc_ptr<AimsTimeSurface<3,Void> > rc_ptr_AimsTimeSurface_3_VOID;
#endif
#include <aims/bucket/bucket.h>
#ifndef PYAIMS_RCPTR_BucketMap_VOID_DEFINED
#define PYAIMS_RCPTR_BucketMap_VOID_DEFINED
typedef carto::rc_ptr<aims::BucketMap<Void> > rc_ptr_BucketMap_VOID;
#endif
%End

public:
  static AffineTransformation3d talairach( const Graph & g );
  static void storeTalairach( Graph & g, const AffineTransformation3d & m );
  static AffineTransformation3d getICBMTransform( const Graph & g );
%Docstring
  transform = aims.GraphManip.getICBMTransform(graph)

  Extract the transformation to the MNI ICBM152 space. This space has
  a fixed transformation with our Aims Talairach.
%End

  static AffineTransformation3d
    getICBM2009cTemplateTransform( const Graph & g );
%Docstring
  transform = aims.GraphManip.getICBM2009cTemplateTransform(graph)

  Extract the transformation to the MNI ICBM152 space, shifted to the
  "most standard" field of view of the template image, the one from the
  ICBM2009c_nlin_asym template from the MNI. Actually we find various fields
  of view for the templates. The one we use here is (193, 229, 193).
  The transform to the "real" ICBM space (0 roughly at AC) is provided
  in the output transformation header.
  This space has a fixed transformation with our Aims Talairach, along
  with the template volume size and voxel size.
  This space has a fixed transformation with our Aims Talairach.
  Includes shifts and axes inversions.
%End

  static void buckets2Volume( Graph & g );
  static void volume2Buckets( Graph & g );
  static void storeAims( Graph & graph, GraphObject* vertex,
                         const std::string & attribute,
                         rc_ptr_AimsTimeSurface_3_VOID obj );
  static void storeAims( Graph & graph, GraphObject* vertex,
                         const std::string & attribute,
                         rc_ptr_AimsTimeSurface_2_VOID obj );

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

  static list_EdgePtr getPath( const Vertex *from, const Vertex *to );

};

};

