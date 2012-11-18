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
 *  Bucket class
 */
#ifndef AIMS_BUCKET_BUCKET_H
#define AIMS_BUCKET_BUCKET_H

#include <aims/config/aimsdata_config.h>
#include <aims/bucket/item.h>
#include <aims/data/pheader.h>
#include <iostream>
#include <map>
#include <list>


template <class T> class AimsBucket;


template <class T>
AIMSDATA_API std::ostream& operator << (std::ostream& out, 
					const AimsBucket<T>& thing);

namespace aims
{
  template<class T> class BucketMap;
}


/** The bucket base class to manage packages of points associated to their
    value during time.
    Time can be omitted if not needed.
    Buckets are based upon STL maps of lists of basic bucket items. One can
    get some more details in STL documentation.
*/
template <class T>
class AimsBucket 
  : public std::map< int , std::list< AimsBucketItem<T> > >
{
public:
  typedef typename std::map<int, std::list<AimsBucketItem<T> > >::iterator 
  iterator;
  typedef typename
      std::map<int, std::list<AimsBucketItem<T> > >::const_iterator
      const_iterator;
  typedef typename std::map<int, std::list<AimsBucketItem<T> > >::key_type
      key_type;

  AimsBucket() : std::map< int , std::list< AimsBucketItem<T> > >()
  { }
  AimsBucket( const AimsBucket<T> & other )
    : std::map<int,std::list< AimsBucketItem<T> > >( other ),
      _header( other._header ) {}
  AimsBucket( const aims::BucketMap<T> & other );
  virtual ~AimsBucket() { }

  AimsBucket<T> & operator = ( const aims::BucketMap<T> & );

  /**@name Item manipulation and methods*/
  //@{
  /** Function redefined to omit time. Push an item into the front of the
      list of index 0 of the map. It is equivalent to write:\\
      AimsBucket<T> bucket; \\
      AimsBucketItem<T> item; \\
      bucket.push_front(item); \\ \\
      than to write : \\
      bucket[0].push_front(item)
  */
  inline void push_front(const AimsBucketItem<T>& item);
  inline void push_back(const AimsBucketItem<T>& item);

  inline void pop_front();
  inline void pop_back();

  // because map[] does not have const operator [ ] : warning at compil.
  inline const std::list< AimsBucketItem<T> >& 
  operator [] ( const key_type& n ) const
  { return  find( n )->second; }
  inline std::list< AimsBucketItem<T> >& operator [] ( const key_type& n )
  { return  
      (*((this->insert
        (typename std::map< int, std::list< AimsBucketItem<T> > >::value_type
        (n, std::list< AimsBucketItem<T> >()))).first)).second; }

  /// returns the X resolution in mm
  inline float  sizeX()     const;
  /// returns the Y resolution in mm
  inline float  sizeY()     const;
  /// returns the Z resolution in mm
  inline float  sizeZ()     const;
  /// returns the T resolution in mm
  inline float  sizeT()     const;

  /// sets the X resolution of the data in mm
  inline void setSizeX(float sizex);
  /// sets the Y resolution of the data in mm
  inline void setSizeY(float sizey);
  /// sets the Z resolution of the data in mm
  inline void setSizeZ(float sizez);
  /// sets the T resolution of the data in mm
  inline void setSizeT(float sizet);
  /// sets X,Y,Z and T resolutions of the data in mm
  inline void setSizeXYZT(float sizex,float sizey,float sizez,float sizet);

  /// Empty the whole map
  inline void erase();
  //@}

  inline const aims::PythonHeader &header() const { return _header; }
  inline aims::PythonHeader &header() { return _header; }
  void setHeader( const aims::PythonHeader &hdr ) { _header = hdr; }

  friend
  std::ostream& operator << <> (std::ostream& out, 
				const AimsBucket<T>& thing);

protected:
  aims::PythonHeader _header;
};


#ifndef DOXYGEN_HIDE_INTERNAL_CLASSES

namespace carto
{

  template<typename T> class DataTypeCode<AimsBucket<T> >
  {
  public:
    static std::string objectType()
    { return "Bucket"; }
    static std::string dataType()
    { return DataTypeCode<T>::dataType(); }
    static std::string name() 
    { 
      return std::string("bucket of ") + DataTypeCode< T >::name(); 
    }
  };

}

#endif // DOXYGEN_HIDE_INTERNAL_CLASSES


template <class T> inline
void AimsBucket<T>::push_front(const AimsBucketItem<T>& item)
{ (*this)[0].push_front(item);
}


template <class T> inline
void AimsBucket<T>::push_back(const AimsBucketItem<T>& item)
{ (*this)[0].push_back(item);
}


template <class T> inline
void AimsBucket<T>::pop_front()
{ (*this)[0].pop_front();
}


template <class T> inline
void AimsBucket<T>::pop_back()
{ (*this)[0].pop_back();
}


template <class T> inline float AimsBucket<T>::sizeX() const
{
  std::vector<float> vs;
  _header.getProperty( "voxel_size", vs );
  if( vs.empty() )
    return 1.;
  return vs[0];
}


template <class T> inline float AimsBucket<T>::sizeY() const
{
  std::vector<float> vs;
  _header.getProperty( "voxel_size", vs );
  if( vs.size() < 2 )
    return 1.;
  return vs[1];
}


template <class T> inline float AimsBucket<T>::sizeZ() const
{
  std::vector<float> vs;
  _header.getProperty( "voxel_size", vs );
  if( vs.size() < 3 )
    return 1.;
  return vs[2];
}


template <class T> inline float AimsBucket<T>::sizeT() const
{
  std::vector<float> vs;
  _header.getProperty( "voxel_size", vs );
  if( vs.size() < 4 )
    return 1.;
  return vs[3];
}


template <class T> inline
void AimsBucket<T>::setSizeX(float sizex)
{
  ASSERT(sizex>0);
  std::vector<float> vs;
  _header.getProperty( "voxel_size", vs );
  while( vs.size() < 4 )
    vs.push_back( 1. );
  vs[0] = sizex;
  _header.setProperty( "voxel_size", vs );
}


template <class T> inline
void AimsBucket<T>::setSizeY(float sizey)
{
  ASSERT(sizey>0);
  std::vector<float> vs;
  _header.getProperty( "voxel_size", vs );
  while( vs.size() < 4 )
    vs.push_back( 1. );
  vs[1] = sizey;
  _header.setProperty( "voxel_size", vs );
}


template <class T> inline
void AimsBucket<T>::setSizeZ(float sizez)
{
  ASSERT(sizez>0);
  std::vector<float> vs;
  _header.getProperty( "voxel_size", vs );
  while( vs.size() < 4 )
    vs.push_back( 1. );
  vs[2] = sizez;
  _header.setProperty( "voxel_size", vs );
}


template <class T> inline
void AimsBucket<T>::setSizeT(float sizet)
{
  ASSERT(sizet>0);
  std::vector<float> vs;
  _header.getProperty( "voxel_size", vs );
  while( vs.size() < 4 )
    vs.push_back( 1. );
  vs[3] = sizet;
  _header.setProperty( "voxel_size", vs );
}


template <class T> inline
void AimsBucket<T>::setSizeXYZT( float sizex, float sizey, float sizez,
                                 float sizet)
{
  std::vector<float> vs(4);
  vs[0] = sizex;
  vs[1] = sizey;
  vs[2] = sizez;
  vs[3] = sizet;
  _header.setProperty( "voxel_size", vs );
}


template <class T> inline
void AimsBucket<T>::erase()
{
  typename AimsBucket<T>::iterator it;
  for (it=this->begin();it!=this->end();it++)
    ((*it).second).erase( ((*it).second).begin() , ((*it).second).end() );
  std::map< int , std::list< AimsBucketItem<T> > >::erase( this->begin(), this->end() );
}


template <class T> inline
std::ostream& operator << (std::ostream& out, const AimsBucket<T>& thing)
{
  out << "{"; 

  typename AimsBucket<T>::const_iterator it1;
  typename std::list< AimsBucketItem<T> >::const_iterator it2;

  for (it1=thing.begin();it1!=thing.end();it1++)
  {
    out << "{";
    out << "t=" << (*it1).first << ",";
    for (it2=((*it1).second).begin();it2!=((*it1).second).end();it2++)
      out << *it2 << ",";
    out << "NULL},";
  }

  return out << "NULL}" << std::flush;
}


#include <aims/bucket/bucketMap.h>


#endif
