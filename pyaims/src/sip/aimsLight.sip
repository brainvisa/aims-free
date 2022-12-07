%ExportedHeaderCode
// Such warnings are caused by the Python API and cannot be avoided
#ifdef __clang__
#pragma clang diagnostic ignored "-Wc++11-compat-deprecated-writable-strings"
#endif

%End

%Module soma.aims.aimssip

%Feature CARTODATA

%ModuleCode
#include <pyaims/init.h>
%End

%PostInitialisationCode
carto::pyaims_init();
%End

%Include system_VOID.sip
%Include std.sip
%Include cartobasetypes.sip
%Include exception.sip
%Include mutex.sip
%Include soma_VOID.sip
%Include set_STRING.sip
%Include vector_S32.sip

%Include datasource.sip
%Include rcptr_DataSource.sip
%Include rcptr_DataSourceInfo.sip

%Include allocator.sip

%Include aimsvector_FLOAT_2.sip
%Include aimsvector_FLOAT_3.sip
%Include aimsvector_FLOAT_4.sip
%Include aimsvector_DOUBLE_3.sip
%Include aimsvector_S16_3.sip
%Include aimsvector_S32_3.sip
%Include aimsvector_S64_3.sip
%Include aimsvector_S64_4.sip

%Include point.sip
%Include vector_FLOAT.sip
%Include set_S32.sip
%Include set_FLOAT.sip

%Include list_S32.sip
%Include list_FLOAT.sip
/*
%Include map_STRING_STRING.sip
%Include map_S32_STRING.sip
*/
%Include rcobject.sip
%Include genericobjectLight.sip
// %Include object.sip

/*
%Include vector_ULONG.sip

%Include system2_VOID.sip
*/

%Include rcptr_GenericObject.sip

%Include objectLight.sip

/*
%Include aimsrgb.sip
%Include getopt.sip
%Include finder.sip

%Include quaternion.sip

%Include motion.sip
%Include rcptr_AffineTransformation3d.sip
%Include reader_AffineTransformation3d.sip

%If ( CARTODATA )

%Include volume_S16.sip
%Include volume_FLOAT.sip

%Include rcptr_Volume_S16.sip
%Include rcptr_Volume_FLOAT.sip

%End

%Include headered.sip
*/


