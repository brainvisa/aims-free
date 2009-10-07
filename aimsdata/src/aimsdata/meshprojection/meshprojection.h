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
