/*
 *  Clock class
 */

#ifndef AIMS_UTILITY_CLOCK_H
#define AIMS_UTILITY_CLOCK_H


#include <aims/config/aimsdata_config.h>
#include <time.h>
#include <iostream>


namespace aims
{

  /** The basic time tool.
      This object can be used as a chronometer to evaluate times in a program.
  */
  class AIMSDATA_API Clock
  {
  public:
    /// Constructor does a reset of _clock
    Clock();

    /// Reset the _clock variable to actual time
    void reset();
    /// Return initial time
    clock_t initialTime() const;
    /// Return time elapsed from last reset
    clock_t laps() const;
    /// Return time elapsed from last reset and reset _clock variable
    clock_t lapsAndReset();

    /// Output stream
    friend std::ostream& operator << (std::ostream& os, const Clock& thing);

  private:
    /// Storage of time at the reset
    clock_t _clock;
  };


}


#endif
