%ModuleHeaderCode
#include <aims/information/information.h>
%End

float AimsEntropy(const AimsData_FLOAT& p);

float AimsMutualInformation(const AimsData_FLOAT& p1,
                            const AimsData_FLOAT& p2,
                            const AimsData_FLOAT& p12);

float AimsCorrelationRatio( const AimsData_FLOAT& p1,
                            const AimsData_FLOAT& p2,
                            const AimsData_FLOAT& p12 );
                            