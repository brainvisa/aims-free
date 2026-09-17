/*
 *  Data reader class
 */
#ifndef AIMS_IO_BASEFORMATS_TEXTURE_H
#define AIMS_IO_BASEFORMATS_TEXTURE_H

#include <aims/config/aimsdata_config.h>
#include <aims/io/fileFormat.h>
#include <aims/mesh/texture.h>


namespace aims
{

  template<class T>
  class TexFormat : public FileFormat<TimeTexture<T> >
  {
    virtual bool read( const std::string & filename, TimeTexture<T> & obj, 
                       const carto::AllocatorContext & context, 
                       carto::Object options );
    virtual bool write( const std::string & filename, 
                        const TimeTexture<T> & obj,
                        carto::Object options = carto::none() );
  };

}


#endif
