/*
 *  Data reader class
 */
#ifndef AIMS_IO_BASEFORMATS_MESH_H
#define AIMS_IO_BASEFORMATS_MESH_H

#include <aims/config/aimsdata_config.h>
#include <aims/io/fileFormat.h>
#include <aims/mesh/surface.h>


namespace aims
{

  template<int D, class T>
  class MeshFormat : public FileFormat<AimsTimeSurface<D,T> >
  {
  public:
    virtual ~MeshFormat();

    virtual bool read( const std::string & filename,
                       AimsTimeSurface<D,T> & obj,
                       const carto::AllocatorContext & context,
                       carto::Object options );
    virtual bool write( const std::string & filename,
                        const AimsTimeSurface<D,T> & vol,
                        carto::Object options = carto::none() );
  };

  class TriFormat : public FileFormat<AimsSurfaceTriangle>
  {
  public:
    virtual ~TriFormat();

    virtual bool read( const std::string & filename,
                       AimsSurfaceTriangle & obj,
                       const carto::AllocatorContext & context,
                       carto::Object options );
    virtual bool write( const std::string & filename,
                        const AimsSurfaceTriangle & vol,
                        carto::Object options = carto::none() );
  };


  template<int D>
  class PovFormat : public FileFormat<AimsTimeSurface<D,Void> >
  {
  public:
    virtual ~PovFormat();

    virtual bool read( const std::string & filename,
                       AimsTimeSurface<D,Void> & obj,
                       const carto::AllocatorContext & context,
                       carto::Object options );
    virtual bool write( const std::string & filename,
                        const AimsTimeSurface<D,Void> & vol,
                        carto::Object options = carto::none() );
  };


  class Vrml1Format : public FileFormat<AimsTimeSurface<3, Void> >
  {
  public:
    virtual ~Vrml1Format();

    virtual bool read( const std::string & filename,
                       AimsTimeSurface<3,Void>&obj,
                       const carto::AllocatorContext & context,
                       carto::Object options );

    virtual bool write( const std::string & filename,
                        const AimsTimeSurface<3, Void> &vol,
                        carto::Object options = carto::none());
  };


  class MniObjFormat : public FileFormat<AimsTimeSurface<3, Void> >
  {
  public:
    virtual ~MniObjFormat();

    virtual bool read( const std::string & filename,
                       AimsTimeSurface<3,Void>&obj,
                       const carto::AllocatorContext & context,
                       carto::Object options );

    virtual bool write(const std::string & filename,
      const AimsTimeSurface<3, Void> &vol,
      carto::Object options = carto::none() );
  };


  template<int D, typename T>
  class WavefrontMeshFormat : public FileFormat<AimsTimeSurface<D, T> >
  {
  public:
    virtual ~WavefrontMeshFormat();

    virtual bool read( const std::string & filename,
                       AimsTimeSurface<D, T>&obj,
                       const carto::AllocatorContext & context,
                       carto::Object options );

    virtual bool write(const std::string & filename,
      const AimsTimeSurface<D, T> &vol,
      carto::Object options = carto::none() );
  };


  class STLMeshFormat : public FileFormat<AimsTimeSurface<3, Void> >
  {
  public:
    virtual ~STLMeshFormat();

    virtual bool read( const std::string & filename,
                       AimsTimeSurface<3, Void>&obj,
                       const carto::AllocatorContext & context,
                       carto::Object options );

    virtual bool write(const std::string & filename,
      const AimsTimeSurface<3, Void> &vol,
      carto::Object options = carto::none() );

  protected:
    bool readAscii( const std::string & filename,
                    AimsTimeSurface<3, Void>&obj,
                    const carto::AllocatorContext & context,
                    carto::Object options, PythonHeader & hdr );
    bool readBinary( const std::string & filename,
                     AimsTimeSurface<3, Void>&obj,
                     const carto::AllocatorContext & context,
                     carto::Object options, PythonHeader & hdr );
    bool writeAscii( const std::string & filename,
                     const AimsTimeSurface<3, Void>&obj,
                     carto::Object options );
    bool writeBinary( const std::string & filename,
                      const AimsTimeSurface<3, Void>&obj,
                      carto::Object options );
  };

}


#endif
