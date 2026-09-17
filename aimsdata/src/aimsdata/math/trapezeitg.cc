// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

/*
 *  Trapeze numerical integration.
 */
#include <aims/math/trapezeitg.h>

namespace aims {

template
class TrapezeIntegratorOf<float>;
template
class TrapezeIntegratorOf<double>;

}

