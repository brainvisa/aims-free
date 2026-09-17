/*
 *  Data reader class
 */
#ifndef AIMS_IO_BASEFORMATS_HIERARCHY_H
#define AIMS_IO_BASEFORMATS_HIERARCHY_H

#include <aims/config/aimsdata_config.h>
#include <aims/io/fileFormat.h>
#include <cartobase/object/syntax.h>

namespace aims
{

  class StxFormat : public FileFormat<carto::SyntaxSet>
  {
    virtual bool read( const std::string & filename, carto::SyntaxSet & obj,
                       const carto::AllocatorContext & context,
                       carto::Object options );
    virtual bool write( const std::string & filename,
                        const carto::SyntaxSet & vol,
                        carto::Object options = carto::none() );
  };

}


#endif
