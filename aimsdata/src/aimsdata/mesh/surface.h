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
 *  Surface class
 */
#ifndef AIMS_SURFACE_SURFACE_H
#define AIMS_SURFACE_SURFACE_H


#include <cartobase/smart/rcptr.h>
#include <aims/config/aimsdata_config.h>
#include <aims/vector/vector.h>
#include <aims/def/general.h>
#include <aims/data/pheader.h>
#include <iostream>
#include <vector>
#include <map>
#include <set>

//
// Basic surface class
//


template <int D,class T = Void> class AimsSurface;

template <int D,class T> AIMSDATA_API
std::ostream& operator << ( std::ostream& out, const AimsSurface<D,T>& thing);


/** The template class to manage a mesh.
    The first template argument gives the dimension of the polygons of the
    mesh. The second template argument provides the type of the texture
    information.
*/
template <int D,class T>
class AIMSDATA_API AimsSurface
{
  protected:
    /// Vector of vertices
    std::vector< Point3df > _vertex;
    /// Vector of normals
    std::vector< Point3df > _normal;
    /// Vector of textures
    std::vector< T > _texture;
    /// Vector of polygons
    typedef std::vector< AimsVector< uint, D > > Polygons;
    Polygons _polygon;

    /// Inferior point of a box containing the whole mesh in the 3D space
    Point3df _mini;
    /// Superior point of a box containing the whole mesh in the 3D space
    Point3df _maxi;

  public:
    /// Constructor does nothing special
    AimsSurface() { }
    /// Destructor does nothing
    virtual ~AimsSurface() { }

    /// Get a const reference to the vector of vertices
    const std::vector<Point3df>& vertex() const { return _vertex; }
    /// Get a non const reference to the vector of verteces
    std::vector<Point3df>& vertex() { return _vertex; }

    /// Get a const reference to the vector of normals
    const std::vector<Point3df>& normal() const { return _normal; }
    /// Get a non const reference to the vector of normals
    std::vector<Point3df>& normal() { return _normal; }

    /// Get a const reference to the vector of textures
    const std::vector<T>& texture() const { return _texture; }
    /// Get a non const reference to the vector of textures
    std::vector<T>& texture() { return _texture; }

    /// Get a const reference to the vector of polygons
    const std::vector< AimsVector<uint,D> >& polygon() const
    { return _polygon; }
    /// Get a non const reference to the vector of polygons
    std::vector< AimsVector<uint,D> >& polygon() { return _polygon; }

    /// Get the inferior point of the box containing the mesh
    Point3df minimum() const { return _mini; }
    /// Get the superior point of the box containing the mesh
    Point3df maximum() const { return _maxi; }

    /// Calculate the inferior point of the box containing the mesh
    inline void setMini();
    /// Calculate the superior point of the box containing the mesh
    inline void setMaxi();

    /// Clear the mesh
    inline void erase();

    /// Update/Compute the normals
    inline void updateNormals();

    /// Output stream operator
    friend
    std::ostream& operator << <>( std::ostream& out,
				  const AimsSurface<D,T>& thing );
};


template <int D,class T> inline
void AimsSurface<D,T>::setMini()
{
  float xmini=1e38,ymini=1e38,zmini=1e38;

  std::vector<Point3df>::const_iterator it;
  for (it=_vertex.begin();it!=_vertex.end();it++)
  { if (it->item(0) < xmini) xmini = it->item(0);
    if (it->item(1) < ymini) ymini = it->item(1);
    if (it->item(2) < zmini) zmini = it->item(2);
  }
  _mini = Point3df(xmini,ymini,zmini);
}


template <int D,class T> inline
void AimsSurface<D,T>::setMaxi()
{
  float xmaxi=-1e38,ymaxi=-1e38,zmaxi=-1e38;

  std::vector<Point3df>::const_iterator it;
  for (it=_vertex.begin();it!=_vertex.end();it++)
  { if (it->item(0) > xmaxi) xmaxi = it->item(0);
    if (it->item(1) > ymaxi) ymaxi = it->item(1);
    if (it->item(2) > zmaxi) zmaxi = it->item(2);
  }
  _maxi = Point3df(xmaxi,ymaxi,zmaxi);
}


template <int D,class T> inline
void AimsSurface<D,T>::erase()
{
  _vertex.erase( _vertex.begin() , _vertex.end() );
  _normal.erase( _normal.begin() , _normal.end() );
  _texture.erase( _texture.begin() , _texture.end() );
  _polygon.erase( _polygon.begin() , _polygon.end() );
}


template <int D,class T> inline
void AimsSurface<D,T>::updateNormals()
{
  // No general method to calculate normals
  // see specialization for D=3 and T=Void
}


template <> inline
void AimsSurface<3,Void>::updateNormals()
{
  unsigned int	i;
  std::vector< Point3df >::size_type	nVert=_vertex.size();
  Polygons::size_type			nPoly=_polygon.size();
  std::vector<std::set<uint> >		polyVert(_vertex.size());
  Point3df				norm;

  if (_normal.size() != nVert)
    _normal.resize(nVert);

  // build vector of set(poly id)
  for (i=0; i<nPoly; ++i)
    for (Polygons::size_type j=0; j<3; j++)
      polyVert[_polygon[i][j]].insert(i);

  // fill normals 
  for (i=0; i<nVert; ++i)
  {
    norm=Point3df(0.0);  // for each point, run through polygons it belongs
    std::set<uint>::const_iterator it;
    for (it= polyVert[i].begin(); it != polyVert[i].end(); it++)
    {
      /* For each polygon which the point belongs to,
         calculate area.Normal=0.5.(ABxAC) */
      norm += crossed(
        (_vertex[_polygon[(*it)][1]]-_vertex[_polygon[(*it)][0]]),
        (_vertex[_polygon[(*it)][2]]-_vertex[_polygon[(*it)][0]]) ) * 0.5F;
    }
    norm.normalize();
    _normal[i]=norm;
  }
}


template <> inline
void AimsSurface<4,Void>::updateNormals()
{
  unsigned int	i;
  std::vector< Point3df >::size_type	nVert=_vertex.size();
  Polygons::size_type			nPoly=_polygon.size();
  std::vector<std::pair<std::set<uint>, std::set<uint> > >
    polyVert(_vertex.size());
  Point3df				norm;

  if( _normal.size() != nVert )
    _normal.resize( nVert );

  // build vector of set(poly id), each quad divided in 2 distinct triangles
  for( i=0; i<nPoly; ++i )
  {
    polyVert[_polygon[i][0]].first.insert(i);
    polyVert[_polygon[i][1]].first.insert(i);
    polyVert[_polygon[i][2]].first.insert(i);
    polyVert[_polygon[i][0]].second.insert(i);
    polyVert[_polygon[i][2]].second.insert(i);
    polyVert[_polygon[i][3]].second.insert(i);
  }

  // fill normals
  for( i=0; i<nVert; ++i )
  {
    norm=Point3df(0.0);
    // for each point, run through triangles it belongs (both sets)
    std::set<uint>::const_iterator it;
    const std::pair<std::set<uint>, std::set<uint> > & polySet = polyVert[i];
    for( it=polySet.first.begin(); it != polySet.first.end(); ++it )
    {
      /* For each polygon which the point belongs to,
         calculate area.Normal=0.5.(ABxAC) */
      norm += crossed(
        (_vertex[_polygon[(*it)][1]]-_vertex[_polygon[(*it)][0]]),
        (_vertex[_polygon[(*it)][2]]-_vertex[_polygon[(*it)][0]]) ) * 0.5F;
    }
    for( it=polySet.second.begin(); it != polySet.second.end(); ++it )
    {
      /* For each polygon which the point belongs to,
         calculate area.Normal=0.5.(ABxAC) */
      norm += crossed(
        (_vertex[_polygon[(*it)][2]]-_vertex[_polygon[(*it)][0]]),
        (_vertex[_polygon[(*it)][3]]-_vertex[_polygon[(*it)][0]]) ) * 0.5F;
    }
    norm.normalize();
    _normal[i] = norm;
  }
}


template <int D,class T> inline
std::ostream& operator << (std::ostream& out,const AimsSurface<D,T>& thing)
{
  out << "{D=" << D << ","
      << "vertex=" << thing.vertex().size() << ","
      << "normal=" << thing.normal().size() << ","
      << "texture=" << thing.texture().size() << ","
      << "polygon=" << thing.polygon().size() << "}";

  return out;
}


//
// Mixing surface and time
//

template <int D,class T = Void> class AimsTimeSurface;

template <int D,class T> AIMSDATA_API
std::ostream& operator << ( std::ostream& out, 
			    const AimsTimeSurface<D,T>& thing );

/** The template class to manage a mesh with time if needed.
    The first template argument gives the dimension of the polygons of the
    mesh. The second template argument provides the type of the texture
    information.
    AimsTimeSurface is based upon STL maps of AimsSurface<D,T> items. One can
    get some more details in STL documentation.
*/
template <int D,class T>
class AIMSDATA_API AimsTimeSurface 
  : public virtual carto::RCObject, public std::map< int, AimsSurface<D,T> >
{
  public:
    typedef typename std::map< int , AimsSurface<D,T> >::iterator iterator;
    typedef typename std::map< int , AimsSurface<D,T> >::const_iterator 
      const_iterator;
    /// Constructor does nothing special
    AimsTimeSurface() : std::map< int, AimsSurface<D,T> >() { }
    /// Destructor does nothing
    virtual ~AimsTimeSurface() { }

    /// Get the header
    inline const aims::PythonHeader &header() const { return _header; }
    inline aims::PythonHeader &header() { return _header; }

    /// Set the header
    void setHeader( const aims::PythonHeader &hdr ) { _header = hdr; }

    /**@name Item manipulation and methods*/
    //@{
    /** Get a const reference to the vector of verteces of the surface
        of index 0.
        The function was redefined to omit time. It is equivalent to write:\\
        AimsTimeSurface<D,T> surface; \\
        Point3df item;
        surface.vertex().push_back(item); \\ \\
        than to write : \\
        surface[0].vertex().push_back(item); \\ \\
        
    */
    const std::vector<Point3df>& vertex() const 
    { return (*(AimsTimeSurface<D,T>*)this)[0].vertex(); }
    /// Get a non const reference to the vector of verteces of the 0 surface
    std::vector<Point3df>& vertex() { return (*this)[0].vertex(); }

    /// Get a const reference to the vector of normals of the 0 surface
    const std::vector<Point3df>& normal() const 
    { return (*(AimsTimeSurface<D,T>*)this)[0].normal(); }
    /// Get a non const reference to the vector of normals of the 0 surface
    std::vector<Point3df>& normal() { return (*this)[0].normal(); }

    /// Get a const reference to the vector of textures of the 0 surface
    const std::vector<T>& texture() const 
    { return (*(AimsTimeSurface<D,T>*)this)[0].texture(); }
    /// Get a non const reference to the vector of textures of the 0 surface
    std::vector<T>& texture() { return (*this)[0].texture(); }

    /// Get a const reference to the vector of polygons of the 0 surface
    const std::vector< AimsVector<uint,D> >& polygon() const
    { return (*(AimsTimeSurface<D,T>*)this)[0].polygon(); }
    /// Get a non const reference to the vector of polygons of the 0 surface
    std::vector< AimsVector<uint,D> >& polygon()
    { return (*this)[0].polygon(); }

    /// Get the inferior point of the box containing all the meshes
    Point3df minimum() const { return _mini; }
    /// Get the superior point of the box containing all the meshes
    Point3df maximum() const { return _maxi; }

    /// Calculates the inferior point of the box containing all the meshes
    inline void setMini();
    /// Calculates the superior point of the box containing all the meshes
    inline void setMaxi();

    /// Clear all the meshes
    inline void erase();

    /// Update/Compute the normals
    inline void updateNormals();

    /// Output stream operator
    friend
    std::ostream& operator << <>( std::ostream& out,  
				  const AimsTimeSurface<D,T>& thing );
    //@}

  protected:
    /**@name Data*/
    //@{
    /// Header
    aims::PythonHeader _header;
    /// Inferior point of the box containing all the meshes
    Point3df _mini;
    /// Inferior point of the box containing all the meshes
    Point3df _maxi;
    //@}
};


#ifndef DOXYGEN_HIDE_INTERNAL_CLASSES

namespace carto
{

  template<int D, class T> class DataTypeCode<AimsTimeSurface<D,T> >
  {
  public:
    static std::string objectType()
    { return "Mesh"; }
    static std::string dataType()
    { return DataTypeCode<T>::dataType(); }
    static std::string name() 
    { 
      return objectType() + std::string(" of ") + DataTypeCode< T >::name(); 
    }
    // Nothing about D ?
  };


  template<typename T> class DataTypeCode<AimsTimeSurface<2,T> >
  {
  public:
    static std::string objectType()
    { return "Segments"; }
    static std::string dataType()
    { return DataTypeCode<T>::dataType(); }
    static std::string name()
    {
      return objectType() + std::string(" of ") + DataTypeCode< T >::name();
    }
  };


  template<typename T> class DataTypeCode<AimsTimeSurface<4,T> >
  {
  public:
    static std::string objectType()
    { return "Mesh4"; }
    static std::string dataType()
    { return DataTypeCode<T>::dataType(); }
    static std::string name()
    {
      return objectType() + std::string(" of ") + DataTypeCode< T >::name();
    }
  };

  /*
  template<> inline std::string
  DataTypeCode<AimsTimeSurface<3,Void> >::name()
  {
    return "mesh of VOID";
  }
  */

}

#endif // DOXYGEN_HIDE_INTERNAL_CLASSES


template <int D,class T> inline
void AimsTimeSurface<D,T>::setMini()
{
  typename AimsTimeSurface<D,T>::iterator it;
  float xmini=1e38,ymini=1e38,zmini=1e38;
  Point3df tmp;

  for (it=this->begin();it!=this->end();it++)
  { ((*it).second).setMini();
    tmp = ((*it).second).minimum();
    if (tmp.item(0) < xmini) xmini = tmp.item(0);
    if (tmp.item(1) < ymini) ymini = tmp.item(1);
    if (tmp.item(2) < zmini) zmini = tmp.item(2);
  }
  _mini = Point3df(xmini,ymini,zmini);
}


template <int D,class T> inline
void AimsTimeSurface<D,T>::setMaxi()
{
  typename AimsTimeSurface<D,T>::iterator it;
  float xmaxi=-1e38,ymaxi=-1e38,zmaxi=-1e38;
  Point3df tmp;

  for (it=this->begin();it!=this->end();it++)
  { ((*it).second).setMaxi();
    tmp = ((*it).second).maximum();
    if (tmp.item(0) > xmaxi) xmaxi = tmp.item(0);
    if (tmp.item(1) > ymaxi) ymaxi = tmp.item(1);
    if (tmp.item(2) > zmaxi) zmaxi = tmp.item(2);
  }
  _maxi = Point3df(xmaxi,ymaxi,zmaxi);
}


template <int D,class T> inline
void AimsTimeSurface<D,T>::erase()
{
  typename AimsTimeSurface<D,T>::iterator it;
  for (it=this->begin();it!=this->end();it++)
    ((*it).second).erase();
  std::map< int, AimsSurface<D,T> >::erase( this->begin(), this->end() );
}

template <int D,class T> inline
void AimsTimeSurface<D,T>::updateNormals()
{
  // No general method to calculate normals
  // see specialization for D=3 and T=Void
}


template <> inline
void AimsTimeSurface<3,Void>::updateNormals()
{
  AimsTimeSurface<3,Void>::iterator it;
  for (it=this->begin();it!=this->end();it++)
    ((*it).second).updateNormals();
}


template <> inline
void AimsTimeSurface<4,Void>::updateNormals()
{
  AimsTimeSurface<4,Void>::iterator it;
  for (it=this->begin();it!=this->end();it++)
    ((*it).second).updateNormals();
}


template <int D, class T> inline
std::ostream& operator << (std::ostream& out,
			   const AimsTimeSurface<D,T>& thing)
{
  out << "{"; 

  typename AimsTimeSurface<D,T>::const_iterator it;

  for (it=thing.begin();it!=thing.end();it++)
  { out << "{";
    out << "t=" << (*it).first << ",";
    out << (*it).second << "},";
  }

  return out << "NULL}" << std::flush;
}

//
// Useful typedefs
//

typedef AIMSDATA_API AimsTimeSurface<2,Void> AimsSegments;
typedef AIMSDATA_API AimsTimeSurface<3,Void> AimsSurfaceTriangle;
typedef AIMSDATA_API AimsTimeSurface<4,Void> AimsSurfaceFacet;

namespace carto {

DECLARE_GENERIC_OBJECT_TYPE( AimsSegments )
DECLARE_GENERIC_OBJECT_TYPE( AimsSurfaceTriangle )
DECLARE_GENERIC_OBJECT_TYPE( AimsSurfaceFacet )

DECLARE_GENERIC_OBJECT_TYPE( rc_ptr< AimsSegments > )
DECLARE_GENERIC_OBJECT_TYPE( rc_ptr< AimsSurfaceTriangle > )
DECLARE_GENERIC_OBJECT_TYPE( rc_ptr< AimsSurfaceFacet > )

#define _mesh_type AimsTimeSurface<2, float>
DECLARE_GENERIC_OBJECT_TYPE( _mesh_type )
DECLARE_GENERIC_OBJECT_TYPE( rc_ptr< _mesh_type > )
#undef _mesh_type
#define _mesh_type AimsTimeSurface<3, float>
DECLARE_GENERIC_OBJECT_TYPE( _mesh_type )
DECLARE_GENERIC_OBJECT_TYPE( rc_ptr< _mesh_type > )
#undef _mesh_type
#define _mesh_type AimsTimeSurface<4, float>
DECLARE_GENERIC_OBJECT_TYPE( _mesh_type )
DECLARE_GENERIC_OBJECT_TYPE( rc_ptr< _mesh_type > )
#undef _mesh_type
#define _mesh_type AimsTimeSurface<2, Point2df>
DECLARE_GENERIC_OBJECT_TYPE( _mesh_type )
DECLARE_GENERIC_OBJECT_TYPE( rc_ptr< _mesh_type > )
#undef _mesh_type
#define _mesh_type AimsTimeSurface<3, Point2df>
DECLARE_GENERIC_OBJECT_TYPE( _mesh_type )
DECLARE_GENERIC_OBJECT_TYPE( rc_ptr< _mesh_type > )
#undef _mesh_type
#define _mesh_type AimsTimeSurface<4, Point2df>
DECLARE_GENERIC_OBJECT_TYPE( _mesh_type )
DECLARE_GENERIC_OBJECT_TYPE( rc_ptr< _mesh_type > )
#undef _mesh_type

} // namespace carto

#endif
