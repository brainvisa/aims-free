
#ifndef AIMS_IO_ROIGTMR_H
#define AIMS_IO_ROIGTMR_H

#include <aims/config/aimsalgo_config.h>
#include <aims/roi/roi.h>
#include <aims/roi/roigtm.h>


namespace aims
{
  class RoiGtmReader;

  RoiGtmReader& operator >> ( RoiGtmReader&, RoiGtm& );


  class AIMSALGO_API RoiGtmReader
  {
  public:

    enum ReadMode
      {
	read_gtmarg_only,
	read_gtmarg_and_roi
      };



    RoiGtmReader( const std::string& name, ReadMode r=read_gtmarg_only );

    virtual ~RoiGtmReader();

    void read( RoiGtm& thing );
    std::string removeExtension(const std::string& name);
    ReadMode   ReadRoi(){return _readRoi;}


    friend RoiGtmReader& operator >> ( RoiGtmReader&, RoiGtm& );

  private:

    std::string   _name;
    ReadMode _readRoi;
  };


  inline RoiGtmReader& operator >> ( RoiGtmReader& reader, RoiGtm& thing )
  {
    reader.read( thing );
    return reader;
  }

}


#endif
