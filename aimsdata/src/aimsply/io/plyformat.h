#ifndef AIMS_IO_PLYFORMAT_H
#define AIMS_IO_PLYFORMAT_H

#include <aims/io/fileFormat.h>
#include <aims/data/data.h>

namespace aims
{

  template<long D, typename T>
  class PlyFormat : public FileFormat<AimsTimeSurface<D,T> >
  {
    virtual bool read( const std::string & filename, 
                       AimsTimeSurface<D,T> & vol, 
                       const carto::AllocatorContext & context, 
                       carto::Object options );
    virtual bool write( const std::string & filename, 
                        const AimsTimeSurface<D,T> & vol, 
                        carto::Object options = carto::none() );
  };

}

#endif

