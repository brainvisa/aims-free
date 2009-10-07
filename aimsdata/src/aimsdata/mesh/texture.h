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
 *  Texture class
 */
#ifndef AIMS_MESH_TEXTURE_H
#define AIMS_MESH_TEXTURE_H


#include <cartobase/smart/rcptr.h>
#include <aims/config/aimsdata_config.h>
#include <aims/vector/vector.h>
#include <aims/def/general.h>
#include <aims/data/pheader.h>
#include <aims/math/dtitensor.h>
#include <vector>
#include <map>
#include <iostream>


template < class T = float > class Texture;

template < class T >
AIMSDATA_API std::ostream& operator << ( std::ostream& out, 
					 const Texture< T >& thing );

//
// Basic texture
//
template < class T >
class Texture
{
public:
  Texture() { }
  Texture(size_t n): _data(n) { }
  virtual ~Texture() { }

  size_t nItem() const { return _data.size(); }

  const T& item( int n ) const { return _data[ n ]; }
  const T & operator [] ( int n ) const { return _data[ n ]; }
  T & operator [] ( int n ) { return _data[ n ]; }
  T& item( int n ) { return _data[ n ]; }

  void reserve( size_t size ) { _data.reserve( size ); }
  void push_back( const T& item ) { _data.push_back( item ); }
  const std::vector<T> & data() const { return( _data ); }
  std::vector<T> & data() { return( _data ); }

  void erase() { _data.clear(); }

  friend
  std::ostream& operator << <>( std::ostream& out, 
                                const Texture< T >& thing );

protected:

  std::vector< T > _data;
};


template < class T > inline
std::ostream& operator << ( std::ostream& out, const Texture< T >& thing )
{
  out << "{nItem=" << thing.nItem() << ", data=(";
  for ( size_t n = 0; n < thing.nItem(); n++ )
    out << thing.item( n ) << ", ";
  out << "NULL)}";

  return out;
}


//
// Mixing surface and time
//

template < class T = float > class TimeTexture;

template < class T >
std::ostream& operator << ( std::ostream& out, const TimeTexture< T >& thing );

template < class T >
class TimeTexture : public virtual carto::RCObject, 
                    public std::map< int, Texture< T > >
{
  public:
    typedef typename std::map<int, Texture< T > >::iterator iterator;
    typedef typename std::map<int, Texture< T > >::const_iterator 
      const_iterator;

    TimeTexture() : std::map< int, Texture< T > >() { }
    TimeTexture(int ntimes, size_t nitems) : 
      std::map< int, Texture< T > >() 
    {
      for (int i=0; i<ntimes; ++i)
        (*this)[i]=Texture<T>(nitems);
    }
    virtual ~TimeTexture() {}

    /// Get the header
    inline const aims::PythonHeader &header() const { return _header; }
    inline aims::PythonHeader &header() { return _header; }

    /// Set the header
    void setHeader( const aims::PythonHeader &hdr ) { _header = hdr; }

    size_t nItem() const { return (*(TimeTexture< T >*)this)[0].nItem(); }
    
    const T& item( int n ) const 
      { return (*(TimeTexture< T >*)this)[0].item( n ); }
    T& item( int n ) { return (*this)[0].item( n ); }
    
    void reserve( size_t size ) { (*this)[0].reserve( size ); }
    void push_back( const T& item ) { (*this)[0].push_back( item ); }

    void erase();

    friend
    std::ostream& operator << <>( std::ostream& out, 
				  const TimeTexture< T >& thing );

protected:

    /// Header
    aims::PythonHeader _header;
};


#ifndef DOXYGEN_HIDE_INTERNAL_CLASSES

namespace carto
{

  template<class T> class DataTypeCode<TimeTexture<T> >
  {
  public:
    static std::string objectType()
    { return "Texture"; }
    static std::string dataType()
    { return DataTypeCode<T>::dataType(); }
    static std::string name() 
    { 
      return std::string("texture of ") + DataTypeCode< T >::name(); 
    }
  };

}

#endif // DOXYGEN_HIDE_INTERNAL_CLASSES


template < class T > inline
void TimeTexture< T >::erase()
{
  typename TimeTexture< T >::iterator it;
  for ( it = this->begin(); it != this->end(); it++ )
    (it->second).erase();
  std::map< int, Texture< T > >::erase( this->begin(), this->end() );
}


template < class T > inline
std::ostream& operator << ( std::ostream& out, const TimeTexture< T >& thing )
{
  out << "{"; 

  typename TimeTexture< T >::const_iterator it;

  for ( it = thing.begin(); it != thing.end(); it++ )
  {
    out << "{";
    out << "t=" << (*it).first << ",";
    out << (*it).second << "},";
  }

  return out << "NULL}" << std::flush;
}


//
// Useful typedefs
//

typedef AIMSDATA_API TimeTexture< float > Texture1d;
typedef AIMSDATA_API TimeTexture< Point2df > Texture2d;

namespace carto {

DECLARE_GENERIC_OBJECT_TYPE( TimeTexture< int16_t > )
DECLARE_GENERIC_OBJECT_TYPE( TimeTexture< uint32_t > )
DECLARE_GENERIC_OBJECT_TYPE( TimeTexture< int32_t > )
DECLARE_GENERIC_OBJECT_TYPE( TimeTexture< uint64_t > )
DECLARE_GENERIC_OBJECT_TYPE( TimeTexture< int64_t > )
DECLARE_GENERIC_OBJECT_TYPE( Texture1d )
DECLARE_GENERIC_OBJECT_TYPE( Texture2d )
#define _TMP_ TimeTexture< AimsVector< short, 2 > >
DECLARE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_

DECLARE_GENERIC_OBJECT_TYPE( rc_ptr< TimeTexture< int16_t > > )
DECLARE_GENERIC_OBJECT_TYPE( rc_ptr< TimeTexture< uint32_t > > )
DECLARE_GENERIC_OBJECT_TYPE( rc_ptr< TimeTexture< int32_t > > )
DECLARE_GENERIC_OBJECT_TYPE( rc_ptr< TimeTexture< uint64_t > > )
DECLARE_GENERIC_OBJECT_TYPE( rc_ptr< TimeTexture< int64_t > > )
DECLARE_GENERIC_OBJECT_TYPE( rc_ptr< Texture1d > )
DECLARE_GENERIC_OBJECT_TYPE( rc_ptr< Texture2d > )
#define _TMP_ rc_ptr< TimeTexture< AimsVector< short, 2 > > >
DECLARE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_

} // namespace carto

#endif
