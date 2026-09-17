/*
 *  ROI reader class
 */
#ifndef AIMS_IO_ROIR_H
#define AIMS_IO_ROIR_H

#include <aims/roi/roi.h>
#include <aims/io/argR.h>


namespace aims
{

  class LowLevelRoiReader : public LowLevelStandardArgReader
  {
  public:
    LowLevelRoiReader();
    virtual ~LowLevelRoiReader();
    virtual Graph* read( const std::string & filename, 
			 int subobjectsfilter = -1 );
  };

}


#endif
