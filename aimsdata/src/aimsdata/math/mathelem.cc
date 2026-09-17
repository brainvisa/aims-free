// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

/*
 *  Elementary math functions.
 */
#include <aims/math/mathelem.h>

AIMSDATA_API int AimsNextPowerOfTwo(int number)
{
  int next=2,nb;

  nb = number;
  while (nb>=2)
  {
    next *= 2;
    nb   /= 2;
  }
  return next;
}


AIMSDATA_API double AimsSigmoid(double x)
{
  if (x<-80.0)
    return 0.0;
  else
    return 1.0 / (1.0 + std::exp(-x));
}
