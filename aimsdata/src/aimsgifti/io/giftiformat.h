#ifndef AIMS_IO_GIFTIFORMAT_H
#define AIMS_IO_GIFTIFORMAT_H

#include <aims/io/fileFormat.h>
#include <aims/mesh/surface.h>
#include <aims/mesh/texture.h>

namespace aims
{

  template<int D, typename T>
  class GiftiMeshFormat : public FileFormat<AimsTimeSurface<D, T> >
  {
  public:
    virtual bool read( const std::string & filename,
                       AimsTimeSurface<D, T> & vol,
                       const carto::AllocatorContext & context,
                       carto::Object options );
    virtual bool write( const std::string & filename,
                        const AimsTimeSurface<D, T> & vol,
                        carto::Object options = carto::none() );

    const carto::Object options() const { return _options; }
    void setOptions( carto::Object opt ) { _options = opt; }

    //int getEncoding() { return _encoding; }
    //void setEncoding( int enc ) { _encoding = enc; }

  protected:
    std::string				_filename;
    carto::Object			_options;
    //int						_encoding;
  };


  template<typename T>
  class GiftiTextureFormat : public FileFormat<TimeTexture<T> >
  {
  public:
    virtual bool read( const std::string & filename,
                       TimeTexture<T> & vol,
                       const carto::AllocatorContext & context,
                       carto::Object options );
    virtual bool write( const std::string & filename,
                        const TimeTexture<T> & vol,
                        carto::Object options = carto::none() );

    const carto::Object options() const { return _options; }
    void setOptions( carto::Object opt ) { _options = opt; }

    int getEncoding() { return _encoding; }
    void setEncoding( int enc ) { _encoding = enc; }

  protected:
    std::string				_filename;
    carto::Object			_options;
    int						_encoding;
  };
}

#endif
