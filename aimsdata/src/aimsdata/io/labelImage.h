/*
 *  Finder class
 */
#ifndef AIMS_IO_LABELIMAGE_H
#define AIMS_IO_LABELIMAGE_H

#include <aims/config/aimsdata_config.h>
#include <aims/io/reader.h>
#include <cartodata/volume/volume.h>

namespace aims
{

  class LabelReader : public Reader<carto::VolumeRef<int16_t> >
  {
  public:
    LabelReader( const std::string& filename );
    virtual ~LabelReader() {}

    using Reader<carto::VolumeRef<int16_t> >::read;
    virtual bool read( carto::VolumeRef<int16_t> & vol, int border=0,
                       const std::string* format = 0 );
  };

}


#endif
