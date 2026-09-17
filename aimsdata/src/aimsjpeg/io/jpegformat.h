#ifndef AIMS_IO_JPEGFORMAT_H
#define AIMS_IO_JPEGFORMAT_H

#include <aims/io/fileFormat.h>
#include <aims/data/data.h>

namespace aims
{

  template<class T>
  class JpegFormat : public FileFormat<AimsData<T> >
  {
    virtual bool read( const std::string & filename, AimsData<T> & vol, 
                       const carto::AllocatorContext & context, 
                       carto::Object options );
    virtual bool write( const std::string & filename, const AimsData<T> & vol, 
                        carto::Object options = carto::none() );
  };

}

#endif
