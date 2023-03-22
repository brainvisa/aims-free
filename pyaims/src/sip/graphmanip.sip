
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

  static Graph* graphFromVolume( const rc_ptr_Volume_S16 & vol,
                                 int16_t background = 0,
                                 std::map<int16_t, std::string> *trans = 0 );
%Docstring
  graph = graphFromVolume(vol, background=0, trans=None)
  graphFromVolume(vol, graph, background=0, trans=None,
                  automaticBackgroundSearch=True)

  builds a ROI graph from a volume of labels
%End

  static Graph* graphFromVolume( const rc_ptr_Volume_S32 & vol,
                                 int32_t background = 0,
                                 std::map<int32_t, std::string> *trans = 0 );
  static void graphFromVolume( const rc_ptr_Volume_S16 & vol , Graph & g,
                               int16_t background = 0,
                               std::map<int16_t, std::string> *trans = 0,
                               bool automaticBackgroundSearch = true );
  static void graphFromVolume( const rc_ptr_Volume_S32 & vol , Graph & g,
                               int32_t background = 0,
                               std::map<int32_t, std::string> *trans = 0,
                               bool automaticBackgroundSearch = true );

  static void setAttributeColor( Graph & graph, const std::string & att,
                                 const AimsRGB & );
  static void setAttributeColor( Graph & graph, const std::string & att,
                                 const AimsRGBA & );
  static void setAttributeColor( Graph & graph, const std::string & att,
                                 const vector_S32 & );
  vector_S32 attributeColor( const Graph & graph, const std::string & att );
  static void completeGraph( Graph & );

  static list_EdgePtr getPath( const Vertex *from, const Vertex *to );

  static void printGraphElementTable( const Graph & );

  static SIP_PYOBJECT get_element_table( const Graph & graph ) /Factory/;
%Docstring
  Get the GraphElementTable as a dict. The dict has the following shape::

      {
          syntax_name: {
              id_name: GraphElementCode_object
          }
      }

  GraphElementCode objects are given as dicts (str, str)::

    {
        'id': <same_as_key: filename (globals) or attribute for filename (locals)>,
        'attribute': <attribute_name_in_graph_element>,
        'objectType': <type of object>,
        'dataType': <data type of object>,
        'storageType': <'Global', 'Local', or 'GlobalPacked'>,
        'local_file_attribute': <attribute giving the filename for local storage in each graph elememt>,
        'global_index_attribute': <attribute giving the index for global storage in each graph elememt>,
        'global_filename': <filename for global storage>,
        'global_attribute': <attribute>,
        'syntax': <syntax_name>
    }
%End
%MethodCode
  carto::rc_ptr<aims::GraphElementTable> get( new aims::GraphElementTable );
  a0->getProperty( "aims_objects_table", get );
  PyObject *map = PyDict_New();
  sipRes = map;
  char errors[] = "strict";
  aims::GraphElementTable::const_iterator it, et = get->end();
  for( it=get->begin(); it!=et; ++it )
  {
    const std::string & key = it->first;
    PyObject *item = PyDict_New();

    std::map<std::string, aims::GraphElementCode>::const_iterator
      ic, ec = it->second.end();
    for( ic=it->second.begin(); ic!=ec; ++ic )
    {
      const std::string & ckey = ic->first;
      PyObject *citem = PyDict_New();

      PyDict_SetItemString( citem, "id",
                              PyUnicode_DecodeUTF8(
                                ic->second.id.c_str(), ic->second.id.length(),
                                errors ) );
      PyDict_SetItemString( citem, "attribute",
                              PyUnicode_DecodeUTF8(
                                ic->second.attribute.c_str(),
                                ic->second.attribute.length(), errors ) );
      PyDict_SetItemString( citem, "objectType",
                              PyUnicode_DecodeUTF8(
                                ic->second.objectType.c_str(),
                                ic->second.objectType.length(), errors ) );
      PyDict_SetItemString( citem, "dataType",
                              PyUnicode_DecodeUTF8(
                                ic->second.dataType.c_str(),
                                ic->second.dataType.length(), errors ) );

      std::string storage;
      switch( ic->second.storageType )
      {
      case aims::GraphElementCode::Local:
        storage = "Local";
        break;
      case aims::GraphElementCode::GlobalPacked:
        storage = "GlobalPacked";
        break;
      default:
        storage = "Global";
      }
      PyDict_SetItemString( citem, "storageType",
                              PyUnicode_DecodeUTF8(
                                storage.c_str(), storage.length(), errors ) );

      PyDict_SetItemString( citem, "local_file_attribute",
                              PyUnicode_DecodeUTF8(
                                ic->second.local_file_attribute.c_str(),
                                ic->second.local_file_attribute.length(),
                                errors ) );
      PyDict_SetItemString( citem, "global_index_attribute",
                              PyUnicode_DecodeUTF8(
                                ic->second.global_index_attribute.c_str(),
                                ic->second.global_index_attribute.length(),
                                errors ) );
      PyDict_SetItemString( citem, "global_filename",
                              PyUnicode_DecodeUTF8(
                                ic->second.global_filename.c_str(),
                                ic->second.global_filename.length(),
                                errors ) );
      PyDict_SetItemString( citem, "global_attribute",
                              PyUnicode_DecodeUTF8(
                                ic->second.global_attribute.c_str(),
                                ic->second.global_attribute.length(),
                                errors ) );
      PyDict_SetItemString( citem, "syntax",
                              PyUnicode_DecodeUTF8(
                                ic->second.syntax.c_str(),
                                ic->second.syntax.length(), errors ) );

      PyDict_SetItemString( item, ckey.c_str(), citem );
    }

    PyDict_SetItemString( map, key.c_str(), item );
  }
%End

};

};

