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

// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

//#include <cartodata/roi/siteIterator.h>
#include <aims/io/reader.h>
#include <aims/roi/siteIterator.h>
#include <iostream>

using namespace aims;
using namespace std; 

namespace carto
{



/*****************   Site **********************************/

ISite::ISite(const Point3d &p3d):
  _coord()
{
  _coord.reserve(3);
  _coord.push_back(p3d[0]);
  _coord.push_back(p3d[1]);
  _coord.push_back(p3d[2]);  
}

ISite::ISite( int x, int y, int z):
  _coord()
{
  _coord.reserve(3);
  _coord.push_back(x);
  _coord.push_back(y);
  _coord.push_back(z);
}

int 
ISite::size() const{ return _coord.size(); }

int  
ISite::intAt( int index ) const {  return _coord.at(index);}

double  
ISite::at( int index) const { return (double)_coord.at(index);}

void 
ISite::set( int index, double value ){ _coord[index] = (int) value;}

void 
ISite::set( int index, int value){  _coord[index] = value;}


DSite::DSite( const Point3df &p3d)
{
  _coord.reserve(3);
  _coord.push_back( (double) p3d[0]);
  _coord.push_back( (double) p3d[1]);
  _coord.push_back( (double) p3d[2]);
  
  }

DSite::DSite( float x, float y, float z)
{
  _coord.reserve(3);
  _coord.push_back((double) x);
  _coord.push_back((double) y);
  _coord.push_back((double) z);
  
}

int 
DSite::size() const { return _coord.size(); }

double  
DSite::at( int index) const {  return _coord.at(index);}

void 
DSite::set( int index, double value ){  _coord[index] = value;}




/*************************  Bucket *************************************/
SiteIteratorOf<  BucketMap<Void> >::
SiteIteratorOf(  BucketMap<Void> &bckMap ) :
  _bucketMap( &bckMap), _useLabel(false)
{
  restart();
}

SiteIteratorOf<  BucketMap<Void> >::
SiteIteratorOf( BucketMap<Void> &bckMap, int label ) :
  _bucketMap( &bckMap),_useLabel(true), _label(label)
{
  restart();
}
  
SiteIteratorOf< BucketMap<Void> >::
SiteIteratorOf( string filename ):
  _useLabel(false)
{
  aims::Reader< AimsBucket<Void> > r(filename);
  _bucketMap = rc_ptr< BucketMap<Void> >( new  BucketMap<Void>(* r.read()));
  restart();
}

SiteIteratorOf< BucketMap<Void> >::
SiteIteratorOf( string filename, int label ):
  _useLabel(true), _label(label)
{
  aims::Reader< AimsBucket<Void> > r(filename);
  _bucketMap = rc_ptr< BucketMap<Void> >( new  BucketMap<Void>(* r.read()));
  restart();
}


void SiteIteratorOf< BucketMap<Void> >::restart()
{
  if (! _bucketMap->empty()) {
    _itBck = _useLabel ?  _bucketMap->find(_label) : _bucketMap->begin();
    
    if (_itBck != _bucketMap->end() )
      _bucket = & _itBck->second;
  }
  
  if ( !_bucket->empty() )
    _itPoints =_bucket->begin();
  else 
    cout << "!!! Bucket is empty !";
}

void SiteIteratorOf< BucketMap<Void> >::next()
{
  ++_itPoints;
  
  if (_itPoints == _bucket->end()){ // end of current bucket ?
    
    if (! _useLabel){  // we scan all points lists
      ++_itBck;                      // next bucket
      
      if (! (_itBck == _bucketMap->end()) ) {
	_bucket = & _itBck->second;
	if ( !_bucket->empty() )
	  _itPoints =_bucket->begin();
      }
    } else             // scan only one labelled list
      _itBck = _bucketMap->end();
  }
}

bool SiteIteratorOf< BucketMap<Void> >::isValid() const
{
  return  _itBck != _bucketMap->end() ;
}


rc_ptr<Site> SiteIteratorOf< BucketMap<Void> >::getSite() 
{ 
  return rc_ptr<Site>(new ISite( _itPoints->first[0],  _itPoints->first[1],  _itPoints->first[2] ));
}

void SiteIteratorOf< BucketMap<Void> >::writeSite()
{
  Point3d p =  _itPoints->first  ;
  cout << p[0] <<" "<< p[1]<< " "<< p[2] <<endl; 
}



/********************************* Mesh ***************************************************/

SiteIteratorOf< AimsSurfaceTriangle >::
SiteIteratorOf( AimsSurfaceTriangle &mesh):
  _mesh(&mesh)
{
  restart();
}

SiteIteratorOf< AimsSurfaceTriangle >::
SiteIteratorOf( std::string filename )
{
  aims::Reader< AimsSurfaceTriangle > r(filename);
  //  _mesh = rc_ptr< AimsSurfaceTriangle >(new AimsSurfaceTriangle(* r.read()));
 _mesh = rc_ptr< AimsSurfaceTriangle >( r.read());
  restart();
}
 
void SiteIteratorOf< AimsSurfaceTriangle >::next()
{
  ++_itVertex;
}


void SiteIteratorOf< AimsSurfaceTriangle >::restart()
{
  _itVertex = _mesh->vertex().begin();
}

bool SiteIteratorOf< AimsSurfaceTriangle >::isValid() const
{
  return  _itVertex != _mesh->vertex().end();
}

rc_ptr<Site> 
SiteIteratorOf< AimsSurfaceTriangle >::getSite()
{
  return rc_ptr<Site>( new DSite((* _itVertex)[0], (* _itVertex)[1], (* _itVertex)[2]));
}

void SiteIteratorOf< AimsSurfaceTriangle >::writeSite()
{
  cout << (* _itVertex)[0]<<" "<<(* _itVertex)[1]<<" "<<(* _itVertex)[2]<<endl;
}



/*************************************  Mesh + Texture   ********************************************/
SiteIteratorOf< Texture1d >::
SiteIteratorOf( std::string meshFilename, std::string texFilename ):
  SiteIteratorOf<AimsSurfaceTriangle>(meshFilename), _useLabel(false)
{  

  aims::Reader< Texture1d> rTex(texFilename);
  _tex = *( rTex.read());
  
  if ( _mesh->vertex().size() != _tex.nItem())
    throw runtime_error( "Mesh and texture don't match" );
  restart();
  
}



SiteIteratorOf< Texture1d >::
SiteIteratorOf( std::string meshFilename, std::string texFilename, float label):
  SiteIteratorOf<AimsSurfaceTriangle>(meshFilename),_useLabel(true), _label(label)
{
  aims::Reader< Texture1d> rTex(texFilename);
  _tex = *( rTex.read());
  
  restart();

}


void
SiteIteratorOf< Texture1d >::restart()
{
  _itVertex = _mesh->vertex().begin();
  _currentVertexNum = 0;

  if ( isValid() && ( _useLabel ? _tex.item(_currentVertexNum) !=  _label : !( _tex.item(_currentVertexNum)) ) )
    next();
}

void 
SiteIteratorOf< Texture1d >::next()
{
  do {
    ++ _itVertex;
    ++ _currentVertexNum;
  } while ( isValid() && ( _useLabel ? _tex.item(_currentVertexNum) !=  _label : !(_tex.item(_currentVertexNum)) ) );
}

} //namespace carto
