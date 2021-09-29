%ModuleHeaderCode
#include <aims/information/information.h>
%End

float AimsEntropy(const rc_ptr_Volume_FLOAT& p);

float AimsMutualInformation(const rc_ptr_Volume_FLOAT& p1,
                            const rc_ptr_Volume_FLOAT& p2,
                            const rc_ptr_Volume_FLOAT& p12);

float AimsCorrelationRatio( const rc_ptr_Volume_FLOAT& p1,
                            const rc_ptr_Volume_FLOAT& p2,
                            const rc_ptr_Volume_FLOAT& p12 );
