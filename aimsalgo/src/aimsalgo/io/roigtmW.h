
#ifndef AIMS_IO_ROIGTMW_H
#define AIMS_IO_ROIGTMW_H

#include <aims/config/aimsalgo_config.h>
#include <aims/roi/roi.h>
#include <aims/roi/roigtm.h>


namespace aims
{

  /** Class for AimsRoiGtm writing operation.
   */
  class AIMSALGO_API RoiGtmWriter
  { 
  public:
    RoiGtmWriter( const std::string& name );
    virtual ~RoiGtmWriter();

    std::string removeExtension( const std::string& name );
    void write( RoiGtm& thing );

  private:
    std::string   _name;
  };

}


inline aims::RoiGtmWriter& operator << ( aims::RoiGtmWriter& writer,
					 aims::RoiGtm& thing )
{
  writer.write( thing );
  return writer;
}


#endif
