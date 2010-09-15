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

#ifndef AIMS_TEXDATA_TEXDATA_H
#define AIMS_TEXDATA_TEXDATA_H

#include <aims/mesh/surface.h>
#include <aims/mesh/surfaceOperation.h>
#include <aims/mesh/texture.h>
#include <aims/data/data.h>
#include <aims/utility/converter_volume.h>
#include <aims/utility/converter_texture.h>
#include <aims/io/writer.h>
#include <aims/io/reader.h>
#include <vector>
#include <set>
#include <utility>

//=================== types ======================================

template<typename T> struct SiteType {
    typedef T type;
};

template<typename T> struct TexType {
    typedef T type;
};

// specialisations de SiteType

template<typename T> class SiteType<AimsData<T> >  { // Images
    public:
        typedef Point3d type;
};

template<int D> class SiteType<AimsSurface<D, Void> > { // surface
    public:
        typedef std::pair<Point3df,uint> type;
};

template<typename T> class TexType<AimsData<T> >  { // texture image
    public:
        typedef T type;
};

template<typename T> class TexType<Texture<T> >  {// texture surface
    public:
        typedef T type;
};

//========== iterateur g��ique ==================================

template<typename S> class SiteIterator {
    public:
        typedef  typename SiteType<S>::type Site;
        Site & operator *();
        //const Site & operator * () const;
        SiteIterator<S> & operator ++ ();
        SiteIterator<S> & operator --(); // optionnel
        bool operator == ( const SiteIterator<S> & other ) const;
        bool operator != ( const SiteIterator<S> & other ) const;
        // etc
};

// iterateur semi-specialise pour images

template<typename T> class SiteIterator<AimsData<T> > {
    public:
        typedef typename SiteType<AimsData<T> >::type Site;
        Site operator *() { return _pos; }
        //const Site & operator *() const { return (*_data)( _pos ); }
        SiteIterator<AimsData<T> > & operator ++ ();
        SiteIterator<AimsData<T> > & operator --(); // optionnel
        SiteIterator( const AimsData<T> *data, const Site & pos );
        bool operator == ( const SiteIterator<AimsData<T> > & other ) const;
        bool operator != ( const SiteIterator<AimsData<T> > & other ) const;
    private:
        const AimsData<T>     *_data;
        Site                  _pos;
};

// iterateur semi-specialise pour surfaces

template<int D> class SiteIterator<AimsSurface<D,Void> > {
    public:
        typedef typename SiteType<AimsSurface<D,Void> >::type Site;
    
        const Site operator *() { return std::pair<Point3df,uint>((*_coordinates)[ _index], _index); }
        //const Site & operator *() const { return _data->vertex()[ _index ]; }
        SiteIterator<AimsSurface<D,Void> > & operator ++ ();
        SiteIterator<AimsSurface<D,Void> > & operator --(); // optionnel
        SiteIterator( const AimsSurface<D,Void> *data, uint index );
        SiteIterator( const AimsSurface<D,Void> * data, std::vector<Point3df> *coords, uint index );
        bool operator == ( const SiteIterator<AimsSurface<D,Void> > & other ) const;
        bool operator != ( const SiteIterator<AimsSurface<D,Void> > & other ) const;
    private:
        const AimsSurface<D,Void>  *_data;
        uint                      _index;
        std::vector<Point3df> *_coordinates;
};



//======== Classe g��ale TexturedData ============================

// La classe generique qui nous interesse

template<typename Geom, typename Text> class TexturedData {
       // Geom est la geometrie (AimsSurface ou AimsData)
       // Tex est la texture (Texture ou AimsData)

    public:
        typedef typename SiteType<Geom>::type Site; // j'espere que cette ruse marche
        typedef typename TexType<Text>::type Tex;
        SiteIterator<Geom> siteBegin();       // pointe sur le premier site
        SiteIterator<Geom> siteEnd();         // pointe juste apres le dernier site
        std::vector<Site> neighbours( const Site & pos ); // acces aux voisins de <pos>
        Tex & intensity( const Site & pos );
        int NbSites();     // nb of sites (comes handy sometime)
};



//=============================================================================
// Specialisation de TexturedData pour images
//=============================================================================

template<typename T> class TexturedData<AimsData<T>, AimsData<T> >
{
public:
  // interface standard, commune
  typedef typename SiteType<AimsData<T> >::type Site;
  typedef T Tex;
  SiteIterator<AimsData<T> > siteBegin();           // pointe sur le premier site
  SiteIterator<AimsData<T> > siteEnd();             // pointe juste apres le dernier site
  std::vector<Site> neighbours( const Site & pos ); // acces aux voisins de <pos>
  Tex & intensity( const Site & pos );
  const Tex & intensity( const Site & pos ) const;

  TexturedData() {}
  TexturedData( AimsData<T> *data ){_data=data;} // pass an image
  TexturedData( const TexturedData<AimsData<T>, AimsData<T> > &other);
                                     // copy constructor
  TexturedData(int dimx, int dimy, int dimz, int dimt = 1,
	       int borderw = 0);

  // Writer

  void write(char *name) {aims::Writer<AimsData<T> > dataW(name); dataW.write(*_data);}

  // Getting the image

  AimsData<T>   *GetImage() {return _data;}

  // nb of sites (comes handy sometime)

  int NbSites() {return (_data->dimX()*_data->dimY()*_data->dimZ());}

	TexturedData<AimsData<T>, AimsData<T> > & operator = ( const TexturedData<AimsData<T>, AimsData<T> > &other );


private:
  AimsData<T>   *_data; // ou compteur de reference rc_ptr (cf libcartobase)
};


//=============================================================================
// Specialisation de TexturedData pour surfaces
//=============================================================================

template<int D, class T> class TexturedData<AimsSurface<D,Void>, Texture<T> > {
    public:
        // interface standard, commune
        typedef typename SiteType<AimsSurface<D, Void> >::type Site;
        typedef T Tex;
        SiteIterator<AimsSurface<D,Void> > siteBegin();   // pointe sur le premier site
        SiteIterator<AimsSurface<D,Void> > siteEnd();     // pointe juste apres le dernier site
        std::vector<Site> neighbours( const Site & pos );    // acces aux voisins de <pos>
        Tex & intensity( const Site & pos ); 
        const Tex & intensity( const Site & pos ) const;
        TexturedData () { }
        TexturedData ( AimsSurface<D, Void> *mesh, Texture<T> *tex ) {
            _mesh = mesh;
            _tex = tex;
            _coordinates = NULL;
        }
                                                             // pass mesh and texture
        TexturedData ( AimsSurface<D, Void> *mesh, Texture<T> *tex, std::vector<Point3df> *coords ) : 
            _mesh(mesh), _tex(tex), _coordinates(coords) { }

        TexturedData ( const TexturedData<AimsSurface<D,Void>, Texture<T> > &other );
                                                             //copy constructor
        // Writer
        void write (char *name) {
            TimeTexture<T> timetext; 
            timetext[0] = *_tex; 
            aims::Writer<TimeTexture<T> > writerT(name); 
            writerT.write(timetext);
        }

        // Getting the texture and geometry    
        AimsSurface<D,Void> *GetMesh() { return _mesh; }
        Texture<T> *GetTexture() { return _tex; }
    
        TexturedData<AimsSurface<D,Void>, Texture<T> > & operator = ( const TexturedData<AimsSurface<D,Void>, Texture<T> >& other );
    
        // nb of sites (comes handy sometime)
    
        int NbSites() { return _mesh->vertex().size(); }

    private:
        AimsSurface<D,Void>   *_mesh;
        Texture<T>            *_tex;
        std::vector<std::set<uint> > _allNeighbours;
        std::vector<Point3df> *_coordinates;
  
};


//=================================================================================
//    Definitions
//=================================================================================

template<typename T>  bool
SiteIterator<AimsData<T> >::operator == ( const SiteIterator<AimsData<T> > & other ) const
{
  if ((_pos[0]==other._pos[0]) && (_pos[1]==other._pos[1]) && (_pos[2]==other._pos[2])) return true;
  else return false;
}

template<typename T>  bool
SiteIterator<AimsData<T> >::operator != ( const SiteIterator<AimsData<T> > & other ) const
{
  if ((this->_pos[0] != other._pos[0]) || (this->_pos[1] != other._pos[1]) || (this->_pos[2] != other._pos[2])) return true;
  else return false;
}

template<int D> bool
SiteIterator<AimsSurface<D,Void> >::operator == ( const SiteIterator<AimsSurface<D,Void> > & other ) const
{
  if (this->_index == other._index) return true;
  else return false;
}

template<int D> bool
SiteIterator<AimsSurface<D,Void> >::operator != ( const SiteIterator<AimsSurface<D,Void> > & other ) const
{
  if (this->_index != other._index) return true;
  else return false;
}


template<typename T> 
SiteIterator<AimsData<T> >::SiteIterator( const AimsData<T> *data, const Site & pos )
{
  _data=data;
  _pos=pos;
}

template<typename T> SiteIterator<AimsData<T> > &
SiteIterator<AimsData<T> >::operator ++() 
{
  if ( _pos[0] < (_data->dimX() -1) )
    _pos[0]++;
  else if ( _pos[1] < (_data->dimY() -1) )
    {
      _pos[0]=0;
      _pos[1]++;
    }
  else
    {
      _pos[0]=0;
      _pos[1]=0;
      _pos[2]++;
    }
  return(*this);
}

template<typename T> SiteIterator<AimsData<T> > &
SiteIterator<AimsData<T> >::operator --()
{
  if ( _pos[0] > 0 )
    _pos[0]--;
  else if ( _pos[1] > 0 )
    {
      _pos[0]=(_data->dimX() -1);
      _pos[1]--;
    }
  else
    {
      _pos[0]=(_data->dimX() -1);
      _pos[1]=(_data->dimY() -1);
      _pos[2]--;
    }
  return(*this);
}

//----------------------------

template<int D> 
SiteIterator<AimsSurface<D,Void> >::SiteIterator( const AimsSurface<D,Void> * data, uint index )
{
  _data=data;
  _index=index;
  _coordinates = (std::vector<Point3df> *)(&(data->vertex())); 
}

template<int D> 
    SiteIterator<AimsSurface<D,Void> >::SiteIterator( const AimsSurface<D,Void> * data,  std::vector<Point3df> *coords, uint index )
{
  _data=data;
  _index=index;
  _coordinates = coords;

}

template<int D> SiteIterator<AimsSurface<D,Void> > &
SiteIterator<AimsSurface<D,Void> >::operator++()
{
  _index++;
  return(*this);
}

template<int D> SiteIterator<AimsSurface<D,Void> > &
SiteIterator<AimsSurface<D,Void> >::operator--()
{
  _index--;
  return(*this);
}

//----------------------------

template<typename T>
TexturedData<AimsData<T>, AimsData<T> >::TexturedData( const TexturedData<AimsData<T>, AimsData<T> > &other)
{
	(*this)._data=new AimsData<T>;
	*((*this)._data)=(*(other._data)).clone();
}

template<typename T>
TexturedData<AimsData<T>, AimsData<T> >::TexturedData(int dimx, int dimy, int dimz, int dimt, int borderw)
{
  _data=new AimsData<T>(dimx, dimy, dimz, dimt, borderw);
}

template<typename T> SiteIterator<AimsData<T> >
TexturedData<AimsData<T>, AimsData<T> >::siteBegin()
{
  Point3d point(0,0,0);
  SiteIterator<AimsData<T> > iter((*this)._data, point);

  return iter;
}

template<typename T> SiteIterator<AimsData<T> >
TexturedData<AimsData<T>, AimsData<T> >::siteEnd()
{
  Point3d point(0, 0, _data->dimZ());
  SiteIterator<AimsData<T> > iter((*this)._data, point);

  return iter;
}

template<typename T> std::vector<typename SiteType<AimsData<T> >::type>
TexturedData<AimsData<T>, AimsData<T> >::neighbours( const Site & pos )
{
  // 26-connectivity so far ...
  std::vector<Point3d> neigh;
  int x=pos[0], y=pos[1], z=pos[2];
  Point3d point;
  int i, j, k;

  for (i=-1; i<=1; i=i+1)
    for (j=-1; j<=1; j=j+1)
      for (k=-1; k<=1; k=k+1)
	{
	  if ((i!=0) || (j!=0) || (k!=0))
	    {
	      if ( ((x+i)>=0) && ((x+i)<_data->dimX()) && ((y+j)>=0) && ((y+j)<_data->dimY()) && ((z+k)>=0) && ((z+k)<_data->dimZ()) )
		{
		  point[0]=x+i;
		  point[1]=y+j;
		  point[2]=z+k;
		  neigh.push_back(point);
		}
	    }
	}
  return neigh;
}

template<typename T> T &
TexturedData<AimsData<T>, AimsData<T> >::intensity(const Site & pos )
{
  return (*_data)(pos);
}

template<typename T> const T &
TexturedData<AimsData<T>, AimsData<T> >::intensity( const Site & pos ) const
{
  return (*_data)(pos);
}

//----------------------------

template<int D, class T>
TexturedData<AimsSurface<D,Void>, Texture<T> >::TexturedData( const TexturedData<AimsSurface<D,Void>, Texture<T> > &other)
{
    (*this)._tex=new Texture<T>(*(other._tex));
    (*this)._mesh=new AimsSurface<D,Void>(*(other._mesh));
    if (other._coordinates != NULL ){
      (*this)._coordinates = other._coordinates;
    }
    else {
      (*this)._coordinates=NULL;
    }
}


template<int D, class T> SiteIterator<AimsSurface<D,Void> > 
TexturedData<AimsSurface<D,Void>, Texture<T> >::siteBegin()
{          
  if (_coordinates == NULL) {
    SiteIterator<AimsSurface<D,Void> > iter(_mesh, 0);
    return iter;
  }
  else{ 
    SiteIterator<AimsSurface<D,Void> > iter(_mesh, _coordinates, 0);
    return iter;
  }
}

template<int D, class T> SiteIterator<AimsSurface<D,Void> > 
TexturedData<AimsSurface<D,Void>, Texture<T> >::siteEnd()
{
  if (_coordinates == NULL ) {
    SiteIterator<AimsSurface<D,Void> > iter(_mesh, _mesh->vertex().size());
    return iter;
  }
  else {
    SiteIterator<AimsSurface<D,Void> > iter(_mesh, _coordinates, _mesh->vertex().size());
    return iter;
  }
}

template<int D, class T>
std::vector<typename SiteType<AimsSurface<D,Void> >::type>
TexturedData<AimsSurface<D,Void>, Texture<T> >::neighbours(const Site & pos )
{
  int i,j,k,n;

  if (_allNeighbours.size()==0)
    {
      _allNeighbours=std::vector<std::set<uint> >( (*_mesh).vertex().size());
      n=(*_mesh).polygon().size();
	for( i=0; i<n; ++i )
	  for( j=0; j<D; ++j )
	    for( k=0; k<D; ++k )
	      if( j != k )
		_allNeighbours[(*_mesh).polygon()[i][j]].insert( (*_mesh).polygon()[i][k] );
    }
      
  std::vector<Site> out;
  std::set<uint>::iterator ptNeigh=_allNeighbours[pos.second].begin();

  for ( ; ptNeigh != _allNeighbours[pos.second].end() ; ++ptNeigh)
    out.push_back(std::pair<Point3df,uint>(_mesh->vertex()[*ptNeigh], *ptNeigh));

  return(out);
}


template<int D, class T> T & 
TexturedData<AimsSurface<D,Void>, Texture<T> >::intensity( const Site & pos )
{
  return((*_tex).item(pos.second));
}

template<int D, class T> const T &
TexturedData<AimsSurface<D,Void>, Texture<T> >::intensity( const Site & pos ) const
{
  return((*_tex).item(pos.second));
}

template <typename T> inline TexturedData<AimsData<T>, AimsData<T> > &
TexturedData<AimsData<T>, AimsData<T> >::operator = ( const TexturedData<AimsData<T>, AimsData<T> > & other )
{
	(*this)._data=new AimsData<T>;
	(*(*this)._data)=(*(other._data)).clone();
	return *this;
}

template < int D, class T> inline TexturedData<AimsSurface<D,Void>, Texture<T> > & 
TexturedData<AimsSurface<D,Void>, Texture<T> >::operator = ( const TexturedData<AimsSurface<D,Void>, Texture<T> > & other )
{
	(*this)._mesh=other._mesh;
	(*this)._tex=other._tex;
        if (other._coordinates != NULL){
          (*this)._coordinates=other._coordinates;          
        }
        else {
          (*this)._coordinates=NULL;
        }
	return *this;
}

#endif
