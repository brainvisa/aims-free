/* This software and supporting documentation are distributed by
 *     Institut Federatif de Recherche 49
 *     CEA/NeuroSpin, Batiment 145,
 *     91191 Gif-sur-Yvette cedex
 *     France
 *
 * This software is governed by the CeCILL-B license under
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the
 * terms of the CeCILL-B license as circulated by CEA, CNRS
 * and INRIA at the following URL "http://www.cecill.info".
 *
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability.
 *
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or
 * data to be ensured and,  more generally, to use and operate it in the
 * same conditions as regards security.
 *
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL-B license and that you accept its terms.
 */

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
