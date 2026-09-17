#ifndef AIMS_IO_FDFFORMAT_H
#define AIMS_IO_FDFFORMAT_H

#include <aims/io/fileFormat.h>
#include <aims/data/data.h>

namespace aims
{

  template<class T>
  class FdfFormat : public FileFormat<AimsData<T> >
  {
    virtual bool read( const std::string & filename, AimsData<T> & vol, 
                       const carto::AllocatorContext & context, 
                       carto::Object options );
  };

}

#endif
