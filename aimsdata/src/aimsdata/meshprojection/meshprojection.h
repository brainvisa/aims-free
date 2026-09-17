/*
 *  Mesh projection class
 */
#ifndef AIMS_MESHPROJECTION_MESHPROJECTION_H
#define AIMS_MESHPROJECTION_MESHPROJECTION_H


#include <aims/mesh/surface.h>
#include <string>

template <typename T> class TimeTexture;


namespace carto
{
  template <typename T> class Volume;
}

namespace aims
{
  template<typename T>
  class MeshProjection
  { 
    public:
      virtual ~MeshProjection();
      MeshProjection(const AimsSurfaceTriangle *,  const carto::Volume<T> *);

      const AimsSurfaceTriangle * mesh() const;
      const carto::Volume<T> * volume() const;
      virtual TimeTexture<T>* project( ) =0;    
      virtual std::string projectType() =0;

    private:
      const AimsSurfaceTriangle * _mesh;
      const carto::Volume<T> * _volume;

  };

  template<typename T>
  class MeshProjectionFactory
  {
    public:
      static MeshProjection<T>* create(const AimsSurfaceTriangle *, 
                                    const carto::Volume<T> *,
                                    const std::string & method,
                                    carto::Object params);
      
      typedef  MeshProjection<T>* (*CreationMethod)(const AimsSurfaceTriangle *, 
                                    const carto::Volume<T> *, carto::Object);

      static void registerMethod(const std::string &, CreationMethod);
      static void unregisterMethod(const std::string &);
      static std::set<std::string> listMethod();

    private:
      static std::map<std::string,CreationMethod>& mapMethod();
  };
}


#endif
