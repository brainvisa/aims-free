#include <aims/resampling/samplable.h>

using namespace aimsalgo;

/** The virtual template class fo which samplable classes must inherit from.
    The first template argument provides the type of vector stored items and
    the second template argument provides the size of vector.
*/

/** Declare the existing templated instanciations */
template class aimsalgo::Samplable<float,3>;
