// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

/*
 *  Clock class
 */
#include <aims/utility/clock.h>

using namespace std;

namespace aims 
{

static clock_t theReferenceTime = ::clock();


Clock::Clock()
{
  reset();
}


void Clock::reset()
{
  _clock = clock();
}
 

clock_t Clock::initialTime() const
{
  return _clock;
}


clock_t Clock::laps() const
{
  return ( clock() - theReferenceTime ) / CLOCKS_PER_SEC;
}


clock_t Clock::lapsAndReset()
{
  clock_t temp = laps();
  reset();
  return temp;
}


ostream& operator << ( ostream& os , const Clock& thing )
{
  os << "aims::Clock{clock=" << thing.initialTime() <<
        ", reference time=" << theReferenceTime << "}";
  return os;
}

} // namespace aims
