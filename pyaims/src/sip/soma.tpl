
%ModuleCode
#include <cartobase/config/cartobase_config.h>
%End

namespace soma
{

%#if defined(__osf__)%
    typedef unsigned long offset_t;
%#else%
    typedef unsigned long long offset_t;
%#endif%

};
