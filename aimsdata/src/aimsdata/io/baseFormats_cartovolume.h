/*
 *  Data reader class
 */
#ifndef AIMS_IO_BASEFORMATS_CARTOVOLUME_H
#define AIMS_IO_BASEFORMATS_CARTOVOLUME_H

#include <aims/config/aimsdata_config.h>
#include <aims/data/header.h>
#include <aims/io/fileFormat.h>
#include <cartodata/volume/volume.h>


namespace aims
{

  template<typename T>
  class VolumeFormat : public FileFormat<carto::Volume<T> >
  {
  public:
    VolumeFormat( const std::string & prefformat = std::string() );
    virtual ~VolumeFormat();

    virtual bool read( const std::string & filename, carto::Volume<T> & vol, 
		       const carto::AllocatorContext & context, 
                       carto::Object options );
    virtual bool write( const std::string & filename, 
                        const carto::Volume<T> & vol,
                        carto::Object options = carto::none() );
    virtual carto::Volume<T>* 
    read( const std::string &filename, 
          const carto::AllocatorContext & context, carto::Object options );

  private:
    std::string	_preferredFormat;
  };


  template<typename T>
  class VolumeRefFormat : public FileFormat<carto::VolumeRef<T> >
  {
  public:
    VolumeRefFormat( const std::string & prefformat = std::string() );
    virtual ~VolumeRefFormat();

    virtual bool read( const std::string & filename, 
                       carto::VolumeRef<T> & vol, 
		       const carto::AllocatorContext & context, 
                       carto::Object options );
    virtual bool write( const std::string & filename, 
                        const carto::VolumeRef<T> & vol, 
                        carto::Object options = carto::none() );
    virtual carto::VolumeRef<T>* 
    read( const std::string &filename, 
          const carto::AllocatorContext & context, carto::Object options );

  private:
    VolumeFormat<T>	_volformat;
  };


  template<typename T>
  class VolumeRefAimsFormat : public FileFormat<carto::VolumeRef<T> >
  {
  public:
    VolumeRefAimsFormat( const std::string & prefformat = std::string() );
    virtual ~VolumeRefAimsFormat();

    virtual bool read( const std::string & filename,
                       carto::VolumeRef<T> & vol,
		       const carto::AllocatorContext & context,
                       carto::Object options );
    virtual bool write( const std::string & filename,
                        const carto::VolumeRef<T> & vol,
                        carto::Object options = carto::none() );
    virtual carto::VolumeRef<T>*
    read( const std::string &filename,
          const carto::AllocatorContext & context, carto::Object options );

  private:
    std::string	_preferredFormat;
  };

}


#endif

