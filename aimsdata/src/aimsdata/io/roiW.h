/*
 *  ROI writer class
 */
#ifndef AIMS_IO_ROIW_H
#define AIMS_IO_ROIW_H

#include <aims/config/aimsdata_config.h>
#include <aims/roi/roi.h>
#include <aims/io/argW.h>


namespace aims
{

  class LowLevelRoiWriter : public LowLevelStandardArgWriter
  {
  public:
    LowLevelRoiWriter();
    virtual ~LowLevelRoiWriter();
    virtual void write( const std::string & filename, Graph & graph );
  };

}

#endif
