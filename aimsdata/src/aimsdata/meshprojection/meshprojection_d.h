/*
 *  Mesh projection class
 */

#ifndef AIMS_MESHPROJECTION_MESHPROJECTION_D_H
#define AIMS_MESHPROJECTION_MESHPROJECTION_D_H

#include <aims/meshprojection/meshprojection.h>

namespace aims
{

  template<typename T> MeshProjection<T>::~MeshProjection()
  {

  }

  template<typename T> 
  MeshProjection<T>::MeshProjection(const AimsSurfaceTriangle *mesh, 
                                    const carto::Volume<T> * vol):
                                    _mesh(mesh),_volume(vol)
  {

  }

  template<typename T> 
  const AimsSurfaceTriangle * MeshProjection<T>::mesh() const
  {
    return _mesh;
  }

  template<typename T> 
  const carto::Volume<T> * MeshProjection<T>::volume() const
  {
    return _volume;
  }

  template<typename T> MeshProjection<T>*
  MeshProjectionFactory<T>::create(const AimsSurfaceTriangle *mesh, 
                                   const carto::Volume<T> *vol,
                                   const std::string &method,
                                   carto::Object params )
  {
    std::map<std::string,CreationMethod>& projectMethod = mapMethod();
    typename std::map<std::string,CreationMethod>::const_iterator i;
    i = projectMethod.find(method);
    MeshProjection<T>* p=0;
    if ( i!= projectMethod.end() )
      p= i->second(mesh,vol,params);

    for(i=projectMethod.begin(); !p && i!=projectMethod.end();++i )
      p= i->second(mesh,vol,params);

    return p;
  }

  template<typename T>
  std::map<std::string,typename MeshProjectionFactory<T>::CreationMethod>&
  MeshProjectionFactory<T>::mapMethod()
  {
    static std::map<std::string,CreationMethod> projMethod;
    return projMethod;
  }

  template<typename T>
  void MeshProjectionFactory<T>::registerMethod(const std::string &method,
                                                CreationMethod pf)
  {
    mapMethod()[method]=pf;
  }

  template<typename T>
  void MeshProjectionFactory<T>::unregisterMethod(const std::string &method)
  {
    mapMethod().erase(method);
  }

  template<typename T>
  std::set<std::string> MeshProjectionFactory<T>::listMethod()
  {
    std::set<std::string> list;
    std::map<std::string,CreationMethod>& projectMethod = mapMethod();
    typename std::map<std::string,CreationMethod>::const_iterator i;
    for (i=projectMethod.begin(); i!=projectMethod.end(); ++i)
      list.insert(i->first);
    return list;
  }
}

#endif
