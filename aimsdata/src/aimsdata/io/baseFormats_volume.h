#ifndef AIMS_IO_BASEFORMATS_VOLUME_H
#define AIMS_IO_BASEFORMATS_VOLUME_H

#include <aims/config/aimsdata_config.h>
#include <aims/io/fileFormat.h>
#include <aims/data/data.h>


namespace aims
{

  template<class T>
  class SpmFormat : public FileFormat<AimsData<T> >
  {
  public:
    virtual ~SpmFormat();

    virtual bool read( const std::string & filename, AimsData<T> & vol, 
                       const carto::AllocatorContext & context, 
                       carto::Object options );
    virtual bool write( const std::string & filename, const AimsData<T> & vol, 
                        carto::Object options = carto::none() );
  };

  template<class T>
  class GenesisFormat : public FileFormat<AimsData<T> >
  {
  public:
    virtual ~GenesisFormat();

    virtual bool read( const std::string & filename, AimsData<T> & vol, 
                       const carto::AllocatorContext & context, 
                       carto::Object options );
  };


  template <typename T>
  class ImasVolFormat : public FileFormat<AimsData<T> >
  {
  public:
    virtual bool read( const std::string & filename, AimsData<T> & obj,
                       const carto::AllocatorContext & context,
                       carto::Object options );
    virtual bool write( const std::string & filename,
                        const AimsData<T> & obj,
                        carto::Object options = carto::none() );
  };


  template <typename T>
  class SomaIOAimsDataFormat : public FileFormat<AimsData<T> >
  {
  public:
    virtual ~SomaIOAimsDataFormat();
    virtual bool read( const std::string & filename, AimsData<T> & obj,
                       const carto::AllocatorContext & context,
                       carto::Object options );
    virtual AimsData<T>* read( const std::string & filename, 
                     const carto::AllocatorContext & context, 
                     carto::Object options );
    virtual bool write( const std::string & filename,
                        const AimsData<T> & obj,
                        carto::Object options = carto::none() );
  };

}


#endif
