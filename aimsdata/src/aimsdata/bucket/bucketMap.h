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
#ifndef AIMS_BUCKET_BUCKETMAP_H
#define AIMS_BUCKET_BUCKETMAP_H

#include <cartobase/object/object.h>
#include <aims/config/aimsdata_config.h>
#include <aims/bucket/bucket.h>
#include <aims/math/dtitensor.h>
#include <cartobase/type/types.h>
#include <iostream>
#include <map>


namespace aims
{
  template <class T> class BucketMap;
  //typedef BucketMap<short> BucketMapShort ;


  /// Helper class for ordering Coordinates by their z, y, x coordinates
  template <class T, int D>
  struct CoordinatesLess
  {};

  // Specialization
  template <class T>
  struct CoordinatesLess<T, 3>: public std::binary_function<T, T, bool>
  {
    bool operator () ( const T & p1, const T & p2 ) const
    {
      return p1[2] < p2[2] 
        || ( p1[2] == p2[2] 
        && ( p1[1] < p2[1] 
      || ( p1[1] == p2[1] && ( p1[0] < p2[0] ) ) ) );
    }
  };

  typedef CoordinatesLess<Point3d, 3> BucketMapLess;
//  ///	Helper class for ordering Point3d by their z, y, x coordinates
//   struct BucketMapLess : public std::binary_function<Point3d, Point3d, bool>
//   {
//     bool operator () ( const Point3d & p1, const Point3d & p2 ) const
//     {
//       return p1[2] < p2[2] 
// 	      || ( p1[2] == p2[2] 
// 		   && ( p1[1] < p2[1] 
// 			|| ( p1[1] == p2[1] && ( p1[0] < p2[0] ) ) ) );
//     }
//   };


  /** An alternate, ordered, representation for buckets (voxels lists).

  Compared to AimsBucket, this version stores points in an ordered map, 
  sorted by (z,y,x) key.

  Conversion operator makes it easy to transform an AimsBucket to a 
  BucketMap and vice versa
  */
  template <class T>
  class AIMSDATA_API BucketMap 
    : public virtual carto::RCObject,
      public std::map< int , std::map< Point3d, T, BucketMapLess > >
  {
  public:
    typedef std::map< int , std::map< Point3d, T, BucketMapLess > > BaseMap;
    typedef std::map< Point3d, T, BucketMapLess > Bucket;
    typedef typename std::map<int, Bucket >::iterator iterator;
    typedef typename std::map<int, Bucket >::const_iterator const_iterator;

    BucketMap() : std::map< int , Bucket >()
    { }
    BucketMap( const BucketMap<T> & other )
      : RCObject(), std::map< int , Bucket >( other ), _header( other._header )
    { }
    /// build from AimsBucket
    BucketMap( const AimsBucket<T> & );
    virtual ~BucketMap() { }

    BucketMap<T> & operator = ( const AimsBucket<T> & );
    /// Function redefined to omit time.
    inline void insert( const Point3d & pos, const T & item );
    /// Function redefined to omit time, looks more like STL maps
    inline void insert( const std::pair<const Point3d,T> & item );
    using BaseMap::insert; // make overloads from std::map visible

    /// Function redefined to omit time.
    inline void erase( const Point3d & pos );
    using BaseMap::erase; // make overloads from std::map visible

    inline void merge( const BucketMap<T> & );

    /// returns the X resolution in mm
    inline float  sizeX()     const;
    /// returns the Y resolution in mm
    inline float  sizeY()     const;
    /// returns the Z resolution in mm
    inline float  sizeZ()     const;
    /// returns the T resolution in s
    inline float  sizeT()     const;

    /// sets the X resolution of the data in mm
    inline void setSizeX(float sizex);
    /// sets the Y resolution of the data in mm
    inline void setSizeY(float sizey);
    /// sets the Z resolution of the data in mm
    inline void setSizeZ(float sizez);
    /// sets the T resolution of the data in s
    inline void setSizeT(float sizet);
    /// sets X,Y,Z and T resolutions of the data
    inline void setSizeXYZT(float sizex,float sizey,float sizez,float sizet);

    inline const aims::PythonHeader &header() const { return _header; }
    inline aims::PythonHeader &header() { return _header; }
    void setHeader( const aims::PythonHeader &hdr ) { _header = hdr; }

  protected:
    aims::PythonHeader _header;
  };

}



#ifndef DOXYGEN_HIDE_INTERNAL_CLASSES

namespace carto
{

  template<class T> class DataTypeCode<aims::BucketMap<T> >
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


namespace aims
{

  template <class T> inline
  void BucketMap<T>::insert( const Point3d & pos, const T & item )
  {
    (*this)[0][pos] = item;
  }


  template <class T> inline
  void BucketMap<T>::insert( const std::pair<const Point3d, T> & item )
  {
    (*this)[0].insert(item);
  }


  template <class T> inline
  void BucketMap<T>::erase( const Point3d & pos )
  {
    (*this)[0].erase( pos );
  }


  template <class T> inline
  void BucketMap<T>::merge( const BucketMap<T> & bck )
  {
    typename BucketMap<T>::const_iterator  i, e = bck.end();
    for( i=bck.begin(); i!=e; ++i )
      (*this)[ i->first ].insert( i->second.begin(), i->second.end() );
  }


  template <class T> inline float BucketMap<T>::sizeX() const
  {
    std::vector<float> vs;
    _header.getProperty( "voxel_size", vs );
    if( vs.empty() )
      return 1.;
    return vs[0];
  }


  template <class T> inline float BucketMap<T>::sizeY() const
  {
    std::vector<float> vs;
    _header.getProperty( "voxel_size", vs );
    if( vs.size() < 2 )
      return 1.;
    return vs[1];
  }


  template <class T> inline float BucketMap<T>::sizeZ() const
  {
    std::vector<float> vs;
    _header.getProperty( "voxel_size", vs );
    if( vs.size() < 3 )
      return 1.;
    return vs[2];
  }


  template <class T> inline float BucketMap<T>::sizeT() const
  {
    std::vector<float> vs;
    _header.getProperty( "voxel_size", vs );
    if( vs.size() < 4 )
      return 1.;
    return vs[3];
  }


  template <class T> inline
  void BucketMap<T>::setSizeX(float sizex)
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
  void BucketMap<T>::setSizeY(float sizey)
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
  void BucketMap<T>::setSizeZ(float sizez)
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
  void BucketMap<T>::setSizeT(float sizet)
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
  void BucketMap<T>::setSizeXYZT( float sizex, float sizey, float sizez,
				  float sizet)
  {
    std::vector<float> vs(4);
    vs[0] = sizex;
    vs[1] = sizey;
    vs[2] = sizez;
    vs[3] = sizet;
    _header.setProperty( "voxel_size", vs );
  }


  template<class T> inline 
  BucketMap<T>::BucketMap( const AimsBucket<T> & other )
  : _header( other.header() )
  {
    *this = other;
  }


  template<class T> inline BucketMap<T> & 
  BucketMap<T>::operator = ( const AimsBucket<T> & other )
  {
    this->clear();
    typename AimsBucket<T>::const_iterator		ib, eb = other.end();
    typename std::list<AimsBucketItem<T> >::const_iterator	ibi, ebi;

    for ( ib=other.begin(); ib!=eb; ++ib )
      {
	Bucket	& bk = (*this)[ ib->first ];
	for ( ibi=ib->second.begin(), ebi=ib->second.end(); ibi!=ebi; ++ibi )
	  bk[ ibi->location() ] = ibi->value();
      }
    _header = other.header();
    return *this;
  }


}

template <class T> inline
std::ostream& operator << (std::ostream& out, const aims::BucketMap<T> & thing)
{
  out << "{"; 

  typename aims::BucketMap<T>::const_iterator it1, e1 = thing.end();

  for ( it1=thing.begin(); it1!=e1; ++it1 )
  {
    typename aims::BucketMap<T>::Bucket::const_iterator it2 , e2 = it1->second.end();
    out << "{";
    out << "t=" << (*it1).first << ",";
    for ( it2=it1->second.begin(); it2!=e2; ++it2 )
      out << *it2 << ",";
    out << "NULL},";
  }

  return out << "NULL}" << std::flush;
}

// Implementation of some AimsBucket conversion functions

template<class T> inline 
AimsBucket<T>::AimsBucket( const aims::BucketMap<T> & b2 )
{
  *this = b2;
}


template<class T> inline AimsBucket<T> & 
AimsBucket<T>::operator = ( const aims::BucketMap<T> & b2 )
{
  this->clear();
  typename aims::BucketMap<T>::const_iterator		ib, eb = b2.end();
  typename aims::BucketMap<T>::Bucket::const_iterator	ibi, ebi;
  AimsBucketItem<T>				bitem;

  for ( ib=b2.begin(); ib!=eb; ++ib )
    {
      std::list<AimsBucketItem<T> >	& bk = (*this)[ ib->first ];
      for ( ibi=ib->second.begin(), ebi=ib->second.end(); ibi!=ebi; ++ibi )
	{
	  bitem.location() = ibi->first;
	  bitem.value() = ibi->second;
	  bk.push_back( bitem );
	}
    }
  _header = b2.header();
  return *this;
}

namespace carto {

DECLARE_GENERIC_OBJECT_TYPE( aims::BucketMap<Void> )
DECLARE_GENERIC_OBJECT_TYPE( aims::BucketMap<int16_t> )
DECLARE_GENERIC_OBJECT_TYPE( aims::BucketMap<uint16_t> )
DECLARE_GENERIC_OBJECT_TYPE( aims::BucketMap<int32_t> )
DECLARE_GENERIC_OBJECT_TYPE( aims::BucketMap<uint32_t> )
DECLARE_GENERIC_OBJECT_TYPE( aims::BucketMap<float> )
DECLARE_GENERIC_OBJECT_TYPE( aims::BucketMap<double> )
DECLARE_GENERIC_OBJECT_TYPE( aims::BucketMap<DtiTensor> )

DECLARE_GENERIC_OBJECT_TYPE( rc_ptr< aims::BucketMap<Void> > )
DECLARE_GENERIC_OBJECT_TYPE( rc_ptr< aims::BucketMap<int16_t> > )
DECLARE_GENERIC_OBJECT_TYPE( rc_ptr< aims::BucketMap<uint16_t> > )
DECLARE_GENERIC_OBJECT_TYPE( rc_ptr< aims::BucketMap<int32_t> > )
DECLARE_GENERIC_OBJECT_TYPE( rc_ptr< aims::BucketMap<uint32_t> > )
DECLARE_GENERIC_OBJECT_TYPE( rc_ptr< aims::BucketMap<float> > )
DECLARE_GENERIC_OBJECT_TYPE( rc_ptr< aims::BucketMap<double> > )
DECLARE_GENERIC_OBJECT_TYPE( rc_ptr< aims::BucketMap<DtiTensor> > )

} // namespace carto

#endif
