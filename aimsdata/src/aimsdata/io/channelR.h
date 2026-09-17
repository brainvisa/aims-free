/*
 *  Data reader class
 */
#ifndef AIMSDATA_IO_CHANNELR_H
#define AIMSDATA_IO_CHANNELR_H

#include <aims/io/process.h>
#include <aims/utility/converter_rgb.h>
#include <aims/utility/converter_hsv.h>
#include <aims/utility/converter_volume.h>
#include <aims/utility/channel.h>
#include <string>


namespace {

    template<class OUTP>
    class InternalConverter : public aims::Process
    {
    public:
      InternalConverter( uint8_t c, int b, int frm, OUTP & d )
        : Process(), channel(c), border( b ), frame( frm ), data( d ) {}
      virtual ~InternalConverter() {}

      uint8_t           channel;
      int               border;
      int               frame;
      OUTP            & data;
    };

}

namespace aims
{
  /// Reads a carto::VolumeRef, with conversion if needed
  template<class OUTP>
  class ChannelReader
  {
  public:
    ChannelReader( const std::string & f );
    virtual ~ChannelReader();

    virtual void read( OUTP & data, 
                       uint8_t channel = 4,
                       int border = 0, 
                       const std::string* format = 0, 
                       int frame = -1 );

    void setFileName( const std::string &fileName );
    void setAllowedInputDataTypes(
                       const std::map<std::string, std::set<std::string> > & );

  private:
    std::string _filename;
    std::map<std::string, std::set<std::string> > _allowedTypes;
  };

}

#endif
