/*
 *  Data reader class
 */
#ifndef AIMS_IO_BASEFORMATS_HIERARCHY_H
#define AIMS_IO_BASEFORMATS_HIERARCHY_H

#include <aims/config/aimsdata_config.h>
#include <aims/io/fileFormat.h>


namespace aims
{
  class Hierarchy;

  class HieFormat : public FileFormat<Hierarchy>
  {
    virtual bool read( const std::string & filename, Hierarchy & obj, 
                       const carto::AllocatorContext & context, 
                       carto::Object options );
    virtual bool write( const std::string & filename,
                        const Hierarchy & vol,
                        carto::Object options = carto::none() );
  };

}


#endif
