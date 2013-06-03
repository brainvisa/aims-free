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
 
#ifndef CARTODATA_ROI_SITEITERATOR_H
#define CARTODATA_ROI_SITEITERATOR_H

#include <cartobase/object/object.h>
#ifdef USE_SOMA_IO
  #include <soma-io/io/reader.h>
#else
  #include <cartobase/io/reader.h>
#endif
#include <cartodata/volume/volume.h>
#include <vector>
#include <iostream>

namespace carto {

class Site : virtual public RCObject
{
public:

  virtual ~Site();

  Site& operator = ( const Site &other );

  virtual int size() const = 0;
  virtual double at( int index ) const = 0;
  virtual void set( int index, double value ) = 0;
  
  bool operator == (const Site &s1);
  bool operator != (const Site &s1);
  void  write();
  
}; // Site

class SiteIterator : public virtual RCObject
{
public:
  virtual ~SiteIterator();
  virtual void next() = 0;
  virtual void restart() = 0;
  virtual bool isValid() const = 0;
  virtual rc_ptr<Site> getSite() = 0;
  virtual void writeSite() = 0;
};

template <class T>
class SiteIteratorOf : public virtual SiteIterator
{
};



template <class T>
class SiteIteratorOf< Volume<T> > :  public virtual SiteIterator 
{
  VolumeRef<T> _data;   // labeled volume
  int _current[3];      // current coord
  T _label;             // which label to use ?
  bool _useLabel;       // false: ROI = Volume(!=0)

public:
  SiteIteratorOf( VolumeRef<T> data);
  SiteIteratorOf( VolumeRef<T> data, const T &label); 
  SiteIteratorOf( const std::string &filename);
  SiteIteratorOf( const std::string &filename, const T  &label);
  
  virtual ~SiteIteratorOf();

  virtual void next();
  virtual void restart();
  virtual bool isValid() const;
  // virtual rc_ptr<Site> getSite();
  virtual void writeSite();
};


template <class T>
SiteIteratorOf< Volume<T> >::
SiteIteratorOf( VolumeRef<T> data) :
  _data(data), _useLabel(false)
{
  restart();
}


template <class T>
SiteIteratorOf< Volume<T> >::
SiteIteratorOf( VolumeRef<T> data, const T  &label ) :
  _data(data), _label(label), _useLabel(true)
{
  restart();
}

template <class T>  
SiteIteratorOf< Volume<T> >:: 
SiteIteratorOf( const std::string &filename ) : 
  _data( new Volume<T> ), _useLabel(false)
  
{  
  Reader< Volume<T> > reader(filename);    
  reader.read(_data);  
  restart();  
}  


template <class T>   
SiteIteratorOf< Volume<T> >::  
SiteIteratorOf( const std::string &filename, const T  &label ) :
  _data( new Volume<T> ), _label(label), _useLabel(true)
{  
  Reader< Volume<T> > reader(filename);  
  reader.read(_data);  
  restart();  
}   
  
template <class T>   
SiteIteratorOf< Volume<T> >::~SiteIteratorOf()
{
}


template <class T>   
void SiteIteratorOf< Volume<T> >::restart()
{
  _current[ 0 ] = _current[ 1 ] = _current[ 2 ] = 0;
  if ( isValid() &&  ( _useLabel ? 
			    (*_data)( _current[0], _current[1], _current[2]) != (T) _label :
			    ! (*_data)( _current[0], _current[1], _current[2] ) ) ) next();
}


template <class T>   
void SiteIteratorOf< Volume<T> >::next()
{
  do {
      ++_current[ 0 ];
      if ( _current[ 0 ] == _data->getSizeX() ) {
	_current[ 0 ] = 0;
	++_current[ 1 ];
	if ( _current[ 1 ] == _data->getSizeY() ) {
	  _current[ 1 ] = 0;
	  ++_current[ 2 ];
	}
      }
    } while( isValid() && ( _useLabel ? 
			    (*_data)( _current[0], _current[1], _current[2]) != (T) _label :
			    ! (*_data)( _current[0], _current[1], _current[2] ) ) );
}


template <class T>   
bool SiteIteratorOf< Volume<T> >::isValid() const
{
    return _current[ 2 ] < _data->getSizeZ();
}

/*
template <class T>   
rc_ptr<iSite> SiteIteratorOf< Volume<T> >::getSite() 
{
  
  return rc_ptr<iSite>(new iSite(_current[0], _current[1],_current[2] ));
}
*/

template <class T>   
void SiteIteratorOf< Volume<T> >::writeSite()
{
  std::cout <<""<< _current[0]<<" "<<_current[1]<<" "<<_current[2]<<std::endl;

}

} //namespace carto


#endif // ifndef CARTODATA_ROI_ROI_H
