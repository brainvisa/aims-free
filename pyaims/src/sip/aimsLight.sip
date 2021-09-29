%Feature CARTODATA

%Module aimssip
%ModuleCode
#include <pyaims/init.h>
%End

%PostInitialisationCode
carto::pyaims_init();
%End

%Include mutex.sip
%Include system_VOID.sip
%Include std.sip
%Include cartobasetypes.sip
%Include exception.sip
%Include allocator.sip

%Include aimsvector_FLOAT_2.sip
%Include aimsvector_FLOAT_3.sip
%Include aimsvector_FLOAT_4.sip
%Include aimsvector_S16_3.sip

%Include point.sip
%Include vector_S32.sip
%Include vector_FLOAT.sip

%Include set_S32.sip
%Include set_FLOAT.sip
%Include set_STRING.sip
%Include list_S32.sip
%Include list_FLOAT.sip
%Include map_STRING_STRING.sip
%Include map_S32_STRING.sip

%Include rcobject.sip
%Include objectLight.sip
%Include rcptr_GenericObject.sip

%Include aimsrgb.sip
%Include getopt.sip
%Include finder.sip

%Include quaternion.sip

%Include aimsdata_S16.sip
%Include aimsdata_FLOAT.sip

%Include motion.sip
%Include rcptr_Motion.sip
%Include reader_Motion.sip

%If ( CARTODATA )

%Include volume_S16.sip
%Include volume_FLOAT.sip

%Include rcptr_Volume_S16.sip
%Include rcptr_Volume_FLOAT.sip


%End

%Include headered.sip


