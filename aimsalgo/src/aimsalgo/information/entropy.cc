// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/information/information.h>
#include <aims/math/mathelem.h>

using namespace carto;

float AimsEntropy(const rc_ptr<Volume<float> >& p)
{
    int levels = p->getSizeX();

    ASSERT( p->getSizeY() == 1 && p->getSizeZ() == 1);

    double entropy = 0.0;
    int x;
    for ( x = 0; x < levels; x++ )
        entropy -= double(p->at(x))
          * (p->at(x) > 0. ? std::log(double(p->at(x))) : 0.);

    return float(entropy);
}
