%ExportedHeaderCode
// Such warnings are caused by the Python API and cannot be avoided
#ifdef __clang__
#pragma clang diagnostic ignored "-Wc++11-compat-deprecated-writable-strings"
#endif

%End

%Module soma.aims.aimssip

%Feature CARTODATA
%Platforms { SIPTHROW_BUG }

%DefaultDocstringFormat "deindented"
%DefaultDocstringSignature "prepended"

%ModuleCode
#include <pyaims/init.h>
%End

%PostInitialisationCode
carto::pyaims_init();
// If an exception occurs while running pyaims_init, it must be propagated by
// returning NULL from the module initialization function. Otherwise, the
// original exception is discarded and Python shows a cryptic error:
// "SystemError: initialization of aimssip raised unreported exception".
if(PyErr_Occurred()) {
#if PY_MAJOR_VERSION >= 3
  Py_DECREF(sipModule);
  return NULL;
#else
  return;
#endif
}
%End

typedef unsigned short uint16_t;
typedef short int16_t;
typedef unsigned int uint32_t;
typedef int int32_t;

%Include libxml2.sip
%Include carto.sip
%Include paths.sip
%Include info.sip

%Include mutex.sip
%Include system_VOID.sip
%Include std.sip
%Include complex.sip
%Include cartobasetypes.sip
%Include exception.sip
%Include allocator.sip

%Include aimsvector_FLOAT_2.sip
%Include aimsvector_FLOAT_3.sip
%Include aimsvector_FLOAT_4.sip
%Include aimsvector_DOUBLE_2.sip
%Include aimsvector_DOUBLE_3.sip
%Include aimsvector_DOUBLE_4.sip
%Include aimsvector_S16_2.sip
%Include aimsvector_S16_3.sip
%Include aimsvector_S16_4.sip
%Include aimsvector_S32_2.sip
%Include aimsvector_S32_3.sip
%Include aimsvector_S32_4.sip
%Include aimsvector_U32_2.sip
%Include aimsvector_U32_3.sip
%Include aimsvector_U32_4.sip
%Include aimsvector_S64_2.sip
%Include aimsvector_S64_3.sip
%Include aimsvector_S64_4.sip

%Include point.sip
%Include vector_U8.sip
%Include vector_S16.sip
%Include vector_U16.sip
%Include vector_S32.sip
%Include vector_U32.sip
%Include vector_ULONG.sip
%Include vector_ULONGLONG.sip
%Include vector_FLOAT.sip
%Include vector_DOUBLE.sip
%Include vector_POINT2DF.sip
%Include vector_POINT3DF.sip
%Include vector_POINT3DD.sip
%Include vector_POINT3D.sip
%Include vector_STRING.sip
%Include vector_AimsVector_U32_2.sip
%Include vector_AimsVector_U32_3.sip
%Include vector_AimsVector_U32_4.sip
%Include vector_set_U32.sip
%Include vector_VOID.sip
%Include vector_vector_S32.sip
%Include vector_vector_FLOAT.sip
%Include vector_vector_STRING.sip
%Include vector_vector_POINT3DF.sip
%Include vector_rc_ptr_AimsTimeSurface_3_VOID.sip
%Include vector_list_U32.sip
//%Include vector_InfoPtr.sip
%Include set_S16.sip
%Include set_S32.sip
%Include set_U32.sip
%Include set_FLOAT.sip
%Include set_DOUBLE.sip
%Include set_STRING.sip
%Include set_VertexPtr.sip
%Include set_EdgePtr.sip
%Include list_S32.sip
%Include list_U32.sip
%Include list_FLOAT.sip
%Include list_DOUBLE.sip
%Include list_STRING.sip
%Include list_PluginFile.sip
%Include list_GraphPtr.sip
%Include list_rc_ptr_Graph.sip
%Include list_POINT3D.sip
%Include list_POINT3DF.sip
%Include list_vector_STRING.sip
%Include list_vector_POINT3DF.sip
%Include list_AimsTimeSurface_3_VOID.sip
%Include list_EdgePtr.sip
%Include map_STRING_STRING.sip
%Include map_S32_STRING.sip
%Include map_STRING_S32.sip
%Include map_STRING_U8.sip
%Include map_STRING_S16.sip
%Include map_STRING_U16.sip
%Include map_STRING_U32.sip
%Include map_STRING_FLOAT.sip
%Include map_STRING_DOUBLE.sip
%Include map_U8_ULONG.sip
%Include map_U8_ULONGLONG.sip
%Include map_U8_U32.sip
%Include map_S16_ULONG.sip
%Include map_S16_ULONGLONG.sip
%Include map_S16_U32.sip
%Include map_U16_ULONG.sip
%Include map_U16_ULONGLONG.sip
%Include map_U16_U32.sip
%Include map_S32_ULONG.sip
%Include map_S32_ULONGLONG.sip
%Include map_S32_U32.sip
%Include map_S32_FLOAT.sip
%Include map_U32_ULONG.sip
%Include map_U32_ULONGLONG.sip
%Include map_U32_U32.sip
// %Include map_STRING_RGB.sip
// %Include map_STRING_RGBA.sip
// %Include map_STRING_HSV.sip
%Include map_U32_set_U32.sip
%Include map_SIZE_T_list_AimsTimeSurface_3_VOID.sip

%Include rcobject.sip
%Include sharedobject.sip
%Include genericobject.sip

%Include system2_VOID.sip

%Include rcptr_GenericObject.sip
%Include object.sip
%Include property.sip

%Include aimsrgb.sip
%Include getopt.sip
%Include finder.sip
%Include plugin.sip
%Include fdinhibitor.sip

%Include syntax.sip
/* %Include map_STRING_Semantic.sip
%Include map_STRING_Syntax.sip */
%Include graph.sip
%Include rcptr_Tree.sip
%Include rcptr_Graph.sip
%Include rcptr_Vertex.sip
%Include rcptr_Edge.sip
%Include graphmanip.sip
%Include tree.sip
%Include hierarchy.sip
%Include quaternion.sip

%Include soma_VOID.sip
%Include datasource.sip

%Include rcptr_DataSource.sip
%Include rcptr_DataSourceInfo.sip

%Include aimstimesurface_2_VOID.sip
%Include aimstimesurface_3_VOID.sip
%Include aimstimesurface_4_VOID.sip
%Include aimstimesurface_2_FLOAT.sip
%Include aimstimesurface_3_FLOAT.sip
%Include aimstimesurface_4_FLOAT.sip
%Include aimstimesurface_2_POINT2DF.sip
%Include aimstimesurface_3_POINT2DF.sip
%Include aimstimesurface_4_POINT2DF.sip

// %Include timetexture_S8.sip
%Include timetexture_U8.sip
%Include timetexture_S16.sip
%Include timetexture_U16.sip
%Include timetexture_S32.sip
%Include timetexture_U32.sip
// %Include timetexture_S64.sip
// %Include timetexture_U64.sip
%Include timetexture_FLOAT.sip
%Include timetexture_DOUBLE.sip
%Include timetexture_POINT2DF.sip

%Include rcptr_AimsTimeSurface_2_VOID.sip
%Include rcptr_AimsTimeSurface_3_VOID.sip
%Include rcptr_AimsTimeSurface_4_VOID.sip
%Include rcptr_AimsTimeSurface_2_FLOAT.sip
%Include rcptr_AimsTimeSurface_3_FLOAT.sip
%Include rcptr_AimsTimeSurface_4_FLOAT.sip
%Include rcptr_AimsTimeSurface_2_POINT2DF.sip
%Include rcptr_AimsTimeSurface_3_POINT2DF.sip
%Include rcptr_AimsTimeSurface_4_POINT2DF.sip
// %Include rcptr_TimeTexture_S8.sip
%Include rcptr_TimeTexture_U8.sip
%Include rcptr_TimeTexture_S16.sip
%Include rcptr_TimeTexture_U16.sip
%Include rcptr_TimeTexture_S32.sip
%Include rcptr_TimeTexture_U32.sip
// %Include rcptr_TimeTexture_S64.sip
// %Include rcptr_TimeTexture_U64.sip
%Include rcptr_TimeTexture_FLOAT.sip
%Include rcptr_TimeTexture_DOUBLE.sip
%Include rcptr_TimeTexture_POINT2DF.sip

%Include reader_Graph.sip
%Include reader_Hierarchy.sip
%Include reader_AimsTimeSurface_2_VOID.sip
%Include reader_AimsTimeSurface_3_VOID.sip
%Include reader_AimsTimeSurface_4_VOID.sip
%Include reader_AimsTimeSurface_2_FLOAT.sip
%Include reader_AimsTimeSurface_3_FLOAT.sip
%Include reader_AimsTimeSurface_4_FLOAT.sip
%Include reader_AimsTimeSurface_2_POINT2DF.sip
%Include reader_AimsTimeSurface_3_POINT2DF.sip
%Include reader_AimsTimeSurface_4_POINT2DF.sip
// %Include reader_TimeTexture_S8.sip
%Include reader_TimeTexture_U8.sip
%Include reader_TimeTexture_S16.sip
%Include reader_TimeTexture_U16.sip
%Include reader_TimeTexture_S32.sip
%Include reader_TimeTexture_U32.sip
// %Include reader_TimeTexture_S64.sip
// %Include reader_TimeTexture_U64.sip
%Include reader_TimeTexture_FLOAT.sip
%Include reader_TimeTexture_DOUBLE.sip
%Include reader_TimeTexture_POINT2DF.sip

%Include bucketmap_VOID.sip
%Include bucketmap_S16.sip
%Include bucketmap_U16.sip
%Include bucketmap_S32.sip
%Include bucketmap_U32.sip
%Include bucketmap_FLOAT.sip
%Include bucketmap_DOUBLE.sip
%Include rcptr_BucketMap_VOID.sip
%Include rcptr_BucketMap_S16.sip
%Include rcptr_BucketMap_U16.sip
%Include rcptr_BucketMap_S32.sip
%Include rcptr_BucketMap_U32.sip
%Include rcptr_BucketMap_FLOAT.sip
%Include rcptr_BucketMap_DOUBLE.sip
%Include reader_BucketMap_VOID.sip
%Include reader_BucketMap_S16.sip
%Include reader_BucketMap_U16.sip
%Include reader_BucketMap_S32.sip
%Include reader_BucketMap_U32.sip
%Include reader_BucketMap_FLOAT.sip
%Include reader_BucketMap_DOUBLE.sip

%Include bucket_util.sip

%Include motion.sip
%Include transformation.sip
%Include rcptr_Transformation3d.sip
%Include rcptr_AffineTransformation3d.sip
%Include reader_AffineTransformation3d.sip
%Include reader_Transformation3d.sip
%Include reader_TransformationGraph3d.sip
%Include fileformat_Transformation3d.sip
%Include fileformat_TransformationGraph3d.sip

%Include headered.sip
%Include roi.sip
%Include rcptr_Site.sip
%Include rcptr_SiteIterator.sip
%Include interpolation.sip
%Include rcptr_Interpolator.sip
%Include sparsematrix.sip
%Include sparseordensematrix.sip
%Include ciftitools.sip

typedef AimsTimeSurface_3_VOID AimsSurfaceTriangle;
// compatibility with pyaims 4.4
typedef AimsTimeSurface_2_VOID aimstimesurface_2;
typedef AimsTimeSurface_3_VOID aimstimesurface_3;
typedef AimsTimeSurface_4_VOID aimstimesurface_4;
typedef rc_ptr_AimsTimeSurface_2_VOID rc_ptr_AimsTimeSurface_2;
typedef rc_ptr_AimsTimeSurface_3_VOID rc_ptr_AimsTimeSurface_3;
typedef rc_ptr_AimsTimeSurface_4_VOID rc_ptr_AimsTimeSurface_4;
%Include surfacegenerator.sip
%Include surfacemanip.sip
%Include cutmesh.sip
%Include threshold.sip
%Include threshold_S16_S16.sip
%Include threshold_FLOAT_S16.sip
%Include mask.sip
%Include standardreferentials.sip
%Include connectivity.sip
//%Include connected_component_U8.sip
%Include connected_component_S16.sip
%Include connected_component_U16.sip
%Include connected_component_S32.sip
%Include connected_component_U32.sip

%Include merge.sip
%Include merge_S16_S16.sip
%Include texturetools.sip

%Include bundles.sip
%Include bundlesSlicer.sip

%Include reader_Object.sip
%Include soma_reader_Object.sip
%Include fileformat_Object.sip

%Include rawconverter_bucket_S16.sip
%Include rcptr_SparseMatrix.sip
%Include rcptr_SparseOrDenseMatrix.sip
%Include reader_SparseMatrix.sip
%Include reader_SparseOrDenseMatrix.sip
%Include fileformat_SparseOrDenseMatrix.sip

%Include aimsgraphreader.sip
%Include iotypes.sip

%If ( CARTODATA )
 
%Include volume_BOOL.sip
%Include volume_U8.sip
%Include volume_S16.sip
%Include volume_U16.sip
%Include volume_S32.sip
%Include volume_U32.sip
%Include volume_FLOAT.sip
%Include volume_DOUBLE.sip
%Include volume_RGB.sip
%Include volume_RGBA.sip
%Include volume_HSV.sip
%Include volume_CFLOAT.sip
%Include volume_CDOUBLE.sip
%Include volume_POINT3DF.sip

%Include debuggingvol.sip

%Include rcptr_Volume_BOOL.sip
%Include rcptr_Volume_U8.sip
%Include rcptr_Volume_S16.sip
%Include rcptr_Volume_U16.sip
%Include rcptr_Volume_S32.sip
%Include rcptr_Volume_U32.sip
%Include rcptr_Volume_FLOAT.sip
%Include rcptr_Volume_DOUBLE.sip
%Include rcptr_Volume_RGB.sip
%Include rcptr_Volume_RGBA.sip
%Include rcptr_Volume_HSV.sip
%Include rcptr_Volume_CFLOAT.sip
%Include rcptr_Volume_CDOUBLE.sip
%Include rcptr_Volume_POINT3DF.sip

%Include volume_operators.sip

// %Include reader_Volume_BOOL.sip
%Include reader_Volume_U8.sip
%Include reader_Volume_S16.sip
%Include reader_Volume_U16.sip
%Include reader_Volume_S32.sip
%Include reader_Volume_U32.sip
%Include reader_Volume_FLOAT.sip
%Include reader_Volume_DOUBLE.sip
%Include reader_Volume_RGB.sip
%Include reader_Volume_RGBA.sip
%Include reader_Volume_HSV.sip
%Include reader_Volume_CFLOAT.sip
%Include reader_Volume_CDOUBLE.sip
%Include reader_Volume_POINT3DF.sip

// %Include soma_reader_Volume_BOOL.sip
%Include soma_reader_Volume_U8.sip
%Include soma_reader_Volume_S16.sip
%Include soma_reader_Volume_U16.sip
%Include soma_reader_Volume_S32.sip
%Include soma_reader_Volume_U32.sip
%Include soma_reader_Volume_FLOAT.sip
%Include soma_reader_Volume_DOUBLE.sip
%Include soma_reader_Volume_RGB.sip
%Include soma_reader_Volume_RGBA.sip
%Include soma_reader_Volume_HSV.sip
%Include soma_reader_Volume_CFLOAT.sip
%Include soma_reader_Volume_CDOUBLE.sip
%Include soma_reader_Volume_POINT3DF.sip

%Include converter_Volume_DOUBLE_DOUBLE.sip
%Include converter_Volume_DOUBLE_FLOAT.sip
%Include converter_Volume_DOUBLE_RGBA.sip
%Include converter_Volume_DOUBLE_RGB.sip
%Include converter_Volume_DOUBLE_HSV.sip
%Include converter_Volume_DOUBLE_S16.sip
%Include converter_Volume_DOUBLE_S32.sip
%Include converter_Volume_DOUBLE_U16.sip
%Include converter_Volume_DOUBLE_U32.sip
%Include converter_Volume_DOUBLE_U8.sip
%Include converter_Volume_FLOAT_DOUBLE.sip
%Include converter_Volume_FLOAT_FLOAT.sip
%Include converter_Volume_FLOAT_RGBA.sip
%Include converter_Volume_FLOAT_RGB.sip
%Include converter_Volume_FLOAT_HSV.sip
%Include converter_Volume_FLOAT_S16.sip
%Include converter_Volume_FLOAT_S32.sip
%Include converter_Volume_FLOAT_U16.sip
%Include converter_Volume_FLOAT_U32.sip
%Include converter_Volume_FLOAT_U8.sip
%Include converter_Volume_RGBA_DOUBLE.sip
%Include converter_Volume_RGBA_FLOAT.sip
%Include converter_Volume_RGBA_RGBA.sip
%Include converter_Volume_RGBA_RGB.sip
%Include converter_Volume_RGBA_HSV.sip
%Include converter_Volume_RGBA_S16.sip
%Include converter_Volume_RGBA_S32.sip
%Include converter_Volume_RGBA_U16.sip
%Include converter_Volume_RGBA_U32.sip
%Include converter_Volume_RGBA_U8.sip
%Include converter_Volume_RGB_DOUBLE.sip
%Include converter_Volume_RGB_FLOAT.sip
%Include converter_Volume_RGB_RGBA.sip
%Include converter_Volume_RGB_RGB.sip
%Include converter_Volume_RGB_HSV.sip
%Include converter_Volume_RGB_S16.sip
%Include converter_Volume_RGB_S32.sip
%Include converter_Volume_RGB_U16.sip
%Include converter_Volume_RGB_U32.sip
%Include converter_Volume_RGB_U8.sip
%Include converter_Volume_HSV_DOUBLE.sip
%Include converter_Volume_HSV_FLOAT.sip
%Include converter_Volume_HSV_RGBA.sip
%Include converter_Volume_HSV_RGB.sip
%Include converter_Volume_HSV_HSV.sip
%Include converter_Volume_HSV_S16.sip
%Include converter_Volume_HSV_S32.sip
%Include converter_Volume_HSV_U16.sip
%Include converter_Volume_HSV_U32.sip
%Include converter_Volume_HSV_U8.sip
%Include converter_Volume_S16_DOUBLE.sip
%Include converter_Volume_S16_FLOAT.sip
%Include converter_Volume_S16_RGBA.sip
%Include converter_Volume_S16_RGB.sip
%Include converter_Volume_S16_HSV.sip
%Include converter_Volume_S16_S16.sip
%Include converter_Volume_S16_S32.sip
%Include converter_Volume_S16_U16.sip
%Include converter_Volume_S16_U32.sip
%Include converter_Volume_S16_U8.sip
%Include converter_Volume_S32_DOUBLE.sip
%Include converter_Volume_S32_FLOAT.sip
%Include converter_Volume_S32_RGBA.sip
%Include converter_Volume_S32_RGB.sip
%Include converter_Volume_S32_HSV.sip
%Include converter_Volume_S32_S16.sip
%Include converter_Volume_S32_S32.sip
%Include converter_Volume_S32_U16.sip
%Include converter_Volume_S32_U32.sip
%Include converter_Volume_S32_U8.sip
%Include converter_Volume_U16_DOUBLE.sip
%Include converter_Volume_U16_FLOAT.sip
%Include converter_Volume_U16_RGBA.sip
%Include converter_Volume_U16_RGB.sip
%Include converter_Volume_U16_HSV.sip
%Include converter_Volume_U16_S16.sip
%Include converter_Volume_U16_S32.sip
%Include converter_Volume_U16_U16.sip
%Include converter_Volume_U16_U32.sip
%Include converter_Volume_U16_U8.sip
%Include converter_Volume_U32_DOUBLE.sip
%Include converter_Volume_U32_FLOAT.sip
%Include converter_Volume_U32_RGBA.sip
%Include converter_Volume_U32_RGB.sip
%Include converter_Volume_U32_HSV.sip
%Include converter_Volume_U32_S16.sip
%Include converter_Volume_U32_S32.sip
%Include converter_Volume_U32_U16.sip
%Include converter_Volume_U32_U32.sip
%Include converter_Volume_U32_U8.sip
%Include converter_Volume_U8_DOUBLE.sip
%Include converter_Volume_U8_FLOAT.sip
%Include converter_Volume_U8_RGBA.sip
%Include converter_Volume_U8_RGB.sip
%Include converter_Volume_U8_HSV.sip
%Include converter_Volume_U8_S16.sip
%Include converter_Volume_U8_S32.sip
%Include converter_Volume_U8_U16.sip
%Include converter_Volume_U8_U32.sip
%Include converter_Volume_U8_U8.sip
%Include converter_rc_ptr_Volume_U8_BucketMap_VOID.sip
%Include converter_rc_ptr_Volume_U8_BucketMap_S16.sip
%Include converter_rc_ptr_Volume_U8_BucketMap_U16.sip
%Include converter_rc_ptr_Volume_U8_BucketMap_S32.sip
%Include converter_rc_ptr_Volume_U8_BucketMap_U32.sip
%Include converter_rc_ptr_Volume_U8_BucketMap_FLOAT.sip
%Include converter_rc_ptr_Volume_U8_BucketMap_DOUBLE.sip
%Include converter_rc_ptr_Volume_S16_BucketMap_VOID.sip
%Include converter_rc_ptr_Volume_S16_BucketMap_S16.sip
%Include converter_rc_ptr_Volume_S16_BucketMap_U16.sip
%Include converter_rc_ptr_Volume_S16_BucketMap_S32.sip
%Include converter_rc_ptr_Volume_S16_BucketMap_U32.sip
%Include converter_rc_ptr_Volume_S16_BucketMap_FLOAT.sip
%Include converter_rc_ptr_Volume_S16_BucketMap_DOUBLE.sip
%Include converter_rc_ptr_Volume_U16_BucketMap_VOID.sip
%Include converter_rc_ptr_Volume_U16_BucketMap_S16.sip
%Include converter_rc_ptr_Volume_U16_BucketMap_U16.sip
%Include converter_rc_ptr_Volume_U16_BucketMap_S32.sip
%Include converter_rc_ptr_Volume_U16_BucketMap_U32.sip
%Include converter_rc_ptr_Volume_U16_BucketMap_FLOAT.sip
%Include converter_rc_ptr_Volume_U16_BucketMap_DOUBLE.sip
%Include converter_rc_ptr_Volume_U32_BucketMap_VOID.sip
%Include converter_rc_ptr_Volume_U32_BucketMap_S16.sip
%Include converter_rc_ptr_Volume_U32_BucketMap_U16.sip
%Include converter_rc_ptr_Volume_U32_BucketMap_S32.sip
%Include converter_rc_ptr_Volume_U32_BucketMap_U32.sip
%Include converter_rc_ptr_Volume_U32_BucketMap_FLOAT.sip
%Include converter_rc_ptr_Volume_U32_BucketMap_DOUBLE.sip
%Include converter_rc_ptr_Volume_S32_BucketMap_VOID.sip
%Include converter_rc_ptr_Volume_S32_BucketMap_S16.sip
%Include converter_rc_ptr_Volume_S32_BucketMap_U16.sip
%Include converter_rc_ptr_Volume_S32_BucketMap_S32.sip
%Include converter_rc_ptr_Volume_S32_BucketMap_U32.sip
%Include converter_rc_ptr_Volume_S32_BucketMap_FLOAT.sip
%Include converter_rc_ptr_Volume_S32_BucketMap_DOUBLE.sip
%Include converter_rc_ptr_Volume_FLOAT_BucketMap_VOID.sip
%Include converter_rc_ptr_Volume_FLOAT_BucketMap_S16.sip
%Include converter_rc_ptr_Volume_FLOAT_BucketMap_U16.sip
%Include converter_rc_ptr_Volume_FLOAT_BucketMap_S32.sip
%Include converter_rc_ptr_Volume_FLOAT_BucketMap_U32.sip
%Include converter_rc_ptr_Volume_FLOAT_BucketMap_FLOAT.sip
%Include converter_rc_ptr_Volume_FLOAT_BucketMap_DOUBLE.sip
%Include converter_rc_ptr_Volume_DOUBLE_BucketMap_VOID.sip
%Include converter_rc_ptr_Volume_DOUBLE_BucketMap_S16.sip
%Include converter_rc_ptr_Volume_DOUBLE_BucketMap_U16.sip
%Include converter_rc_ptr_Volume_DOUBLE_BucketMap_S32.sip
%Include converter_rc_ptr_Volume_DOUBLE_BucketMap_U32.sip
%Include converter_rc_ptr_Volume_DOUBLE_BucketMap_FLOAT.sip
%Include converter_rc_ptr_Volume_DOUBLE_BucketMap_DOUBLE.sip
%Include converter_BucketMap_VOID_rc_ptr_Volume_U8.sip
%Include converter_BucketMap_VOID_rc_ptr_Volume_U16.sip
%Include converter_BucketMap_VOID_rc_ptr_Volume_S16.sip
%Include converter_BucketMap_VOID_rc_ptr_Volume_U32.sip
%Include converter_BucketMap_VOID_rc_ptr_Volume_S32.sip
%Include converter_BucketMap_VOID_rc_ptr_Volume_FLOAT.sip
%Include converter_BucketMap_VOID_rc_ptr_Volume_DOUBLE.sip
%Include converter_BucketMap_S16_rc_ptr_Volume_U8.sip
%Include converter_BucketMap_S16_rc_ptr_Volume_U16.sip
%Include converter_BucketMap_S16_rc_ptr_Volume_S16.sip
%Include converter_BucketMap_S16_rc_ptr_Volume_U32.sip
%Include converter_BucketMap_S16_rc_ptr_Volume_S32.sip
%Include converter_BucketMap_S16_rc_ptr_Volume_FLOAT.sip
%Include converter_BucketMap_S16_rc_ptr_Volume_DOUBLE.sip
%Include converter_BucketMap_U16_rc_ptr_Volume_U8.sip
%Include converter_BucketMap_U16_rc_ptr_Volume_U16.sip
%Include converter_BucketMap_U16_rc_ptr_Volume_S16.sip
%Include converter_BucketMap_U16_rc_ptr_Volume_U32.sip
%Include converter_BucketMap_U16_rc_ptr_Volume_S32.sip
%Include converter_BucketMap_U16_rc_ptr_Volume_FLOAT.sip
%Include converter_BucketMap_U16_rc_ptr_Volume_DOUBLE.sip
%Include converter_BucketMap_S32_rc_ptr_Volume_U8.sip
%Include converter_BucketMap_S32_rc_ptr_Volume_U16.sip
%Include converter_BucketMap_S32_rc_ptr_Volume_S16.sip
%Include converter_BucketMap_S32_rc_ptr_Volume_U32.sip
%Include converter_BucketMap_S32_rc_ptr_Volume_S32.sip
%Include converter_BucketMap_S32_rc_ptr_Volume_FLOAT.sip
%Include converter_BucketMap_S32_rc_ptr_Volume_DOUBLE.sip
%Include converter_BucketMap_U32_rc_ptr_Volume_U8.sip
%Include converter_BucketMap_U32_rc_ptr_Volume_U16.sip
%Include converter_BucketMap_U32_rc_ptr_Volume_S16.sip
%Include converter_BucketMap_U32_rc_ptr_Volume_U32.sip
%Include converter_BucketMap_U32_rc_ptr_Volume_S32.sip
%Include converter_BucketMap_U32_rc_ptr_Volume_FLOAT.sip
%Include converter_BucketMap_U32_rc_ptr_Volume_DOUBLE.sip
%Include converter_BucketMap_FLOAT_rc_ptr_Volume_U8.sip
%Include converter_BucketMap_FLOAT_rc_ptr_Volume_U16.sip
%Include converter_BucketMap_FLOAT_rc_ptr_Volume_S16.sip
%Include converter_BucketMap_FLOAT_rc_ptr_Volume_U32.sip
%Include converter_BucketMap_FLOAT_rc_ptr_Volume_S32.sip
%Include converter_BucketMap_FLOAT_rc_ptr_Volume_FLOAT.sip
%Include converter_BucketMap_FLOAT_rc_ptr_Volume_DOUBLE.sip
%Include converter_BucketMap_DOUBLE_rc_ptr_Volume_U8.sip
%Include converter_BucketMap_DOUBLE_rc_ptr_Volume_U16.sip
%Include converter_BucketMap_DOUBLE_rc_ptr_Volume_S16.sip
%Include converter_BucketMap_DOUBLE_rc_ptr_Volume_U32.sip
%Include converter_BucketMap_DOUBLE_rc_ptr_Volume_S32.sip
%Include converter_BucketMap_DOUBLE_rc_ptr_Volume_FLOAT.sip
%Include converter_BucketMap_DOUBLE_rc_ptr_Volume_DOUBLE.sip

%Include borderfiller_BOOL.sip
%Include borderfiller_U8.sip
%Include borderfiller_S16.sip
%Include borderfiller_U16.sip
%Include borderfiller_S32.sip
%Include borderfiller_U32.sip
%Include borderfiller_FLOAT.sip
%Include borderfiller_DOUBLE.sip
%Include borderfiller_RGB.sip
%Include borderfiller_RGBA.sip

%Include fill.sip

%End
