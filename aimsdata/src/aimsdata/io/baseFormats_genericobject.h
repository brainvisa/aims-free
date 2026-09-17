#ifndef AIMS_IO_BASEFORMATS_GENERICOBJECT_H
#define AIMS_IO_BASEFORMATS_GENERICOBJECT_H

#include <aims/config/aimsdata_config.h>
#include <aims/io/fileFormat.h>
#include <cartobase/object/object.h>


namespace aims
{

  template<> inline
  carto::GenericObject *FileFormat<carto::GenericObject>::read(
      const std::string &, const carto::AllocatorContext &,
      carto::Object )
  {
    // just to avoid default implementation which instantiates a T
    return 0;
  }


  template <typename T>
  class MinfFormat : public FileFormat<T>
  {
  public:
    virtual ~MinfFormat();

    virtual bool read( const std::string & filename, T & obj,
                       const carto::AllocatorContext & context, 
                       carto::Object options );
    virtual T* read( const std::string & filename,
                     const carto::AllocatorContext & context,
                     carto::Object options );
    virtual bool write( const std::string &, const T &,
                        carto::Object options = carto::none() );
  };


  /*
  template <typename T>
  class MinfXMLFormat : public MinfFormat<T>
  {
  public:
    virtual ~MinfXMLFormat();

    virtual bool write( const std::string &, const T &, bool = false );
  };
  */

}

#endif


