/* Copyright (c) 1995-2005 CEA
 *
 *  This software and supporting documentation were developed by
 *      CEA/DSV/SHFJ
 *      4 place du General Leclerc
 *      91401 Orsay cedex
 *      France
 *
 * This software is governed by the CeCILL license version 2 under 
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the 
 * terms of the CeCILL license version 2 as circulated by CEA, CNRS
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
 * knowledge of the CeCILL license version 2 and that you accept its terms.
 */

#ifndef AIMS_DATA_CARTODATAVOLUME_H
#define AIMS_DATA_CARTODATAVOLUME_H

#include <cartodata/volume/volume.h>
#include <cartodata/volume/volumeoperators.h>
#include <aims/vector/vector.h>
#include <aims/border/border.h>
#include <cartobase/smart/rcptr.h>
#include <cartobase/allocator/allocator.h>
#include <aims/data/pheader.h>


template<typename T>
class AimsData : public carto::RCObject, public aims::Border
{
 public:
  typedef T value_type;
  /// basic pointer
  typedef T* pointer;
  /// 4D-pointer
  typedef T**** pointer4d;
  /// basic iterator
  typedef T* iterator;
  /// basic constant iterator
  typedef const T* const_iterator;
  /// basic reference
  typedef T& reference;
  /// basic constant reference
  typedef const T& const_reference;
  /// size of the basic type
  typedef size_t size_type;
  /// difference type
  typedef ptrdiff_t difference_type;

  iterator begin();
  const_iterator begin() const;
  iterator end();
  const_iterator end() const;
  bool empty() const;

  AimsData( int dimx = 1, int dimy = 1, int dimz = 1, int dimt = 1,
	    int borderw = 0 );
  AimsData( int dimx, int dimy, int dimz, int dimt,
	    int borderw, const carto::AllocatorContext & al );
  AimsData( const AimsData<T>& other );
  AimsData( const AimsData<T>& other, int borderw);
  virtual ~AimsData();

  // conversions with carto::Volume
  AimsData( carto::rc_ptr<carto::Volume<T> > vol );
  AimsData<T> & operator = ( carto::rc_ptr<carto::Volume<T> > vol );

  AimsData<T> & operator = ( const AimsData<T> & );
  AimsData<T> & operator = ( const T & );

  carto::rc_ptr<carto::Volume<T> > volume();
  carto::rc_ptr<carto::Volume<T> > volume() const;
  /// cast to Volume
  operator carto::rc_ptr<carto::Volume<T> >();
  operator const carto::rc_ptr<carto::Volume<T> >() const;

  const carto::AllocatorContext & allocator() const;
  int dimX() const;
  int dimY() const;
  int dimZ() const;
  int dimT() const;
  float sizeX() const;
  float sizeY() const;
  float sizeZ() const;
  float sizeT() const;
  void setSizeX( float sizex );
  void setSizeY( float sizey );
  void setSizeZ( float sizez );
  void setSizeT( float sizet );
  void setSizeXYZT( float sizex = 1.0f, float sizey = 1.0f,
		    float sizez = 1.0f, float sizet = 1.0f );
  void setSizeXYZT( const AimsData<T>& other );
  const aims::Header* header() const;
  aims::Header* header();
  void setHeader( aims::Header* hdr );
  reference operator [] ( size_type n );
  const_reference operator [] ( size_type n ) const;
  reference operator () ( size_type x = 0, size_type y = 0,
			  size_type z = 0, size_type t = 0 );
  const_reference operator () ( size_type x = 0, size_type y = 0,
				size_type z = 0, size_type t = 0 ) const;
  reference operator () ( const Point4d& pt );
  const_reference operator () ( const Point4d& pt ) const;
  reference operator () ( const Point3d& pt );
  const_reference operator () ( const Point3d& pt ) const;
  reference operator () ( const Point2d& pt );
  const_reference operator () ( const Point2d& pt ) const;

  T minimum() const;
  T maximum() const;
  T minIndex( int* x, int* y, int* z, int* t ) const;
  T maxIndex( int* x, int* y, int* z, int* t ) const;
  void fillBorder( const T& val );
  AimsData<T> clone () const;
  AimsData<T> cross( const AimsData<T>& other );
  AimsData<T>& transpose();

 private:
  struct Private;

  carto::rc_ptr<carto::Volume<T> >  _volume;
  Private *d;
};


#ifndef DOXYGEN_HIDE_INTERNAL_CLASSES

namespace carto
{

  template<class T> class DataTypeCode<AimsData<T> >
  {
  public:
    static std::string objectType()
    { return "Volume"; }
    static std::string dataType()
    { return DataTypeCode<T>::dataType(); }
    static std::string name() 
    { 
      return std::string("volume of ") + DataTypeCode< T >::name(); 
    }
  };

  template <typename T>
  carto::Object getObjectHeader( AimsData<T> & obj )
  {
    return carto::Object::reference( obj.volume()->header() );
  }

}

#endif // DOXYGEN_HIDE_INTERNAL_CLASSES

// private struct

template<typename T>
struct AimsData<T>::Private
{
  Private();
  ~Private();

  static int borderWidth( carto::rc_ptr<carto::Volume<T> > vol );

  aims::Header *header;
};


// inline methods definition

// Private struct methods

template<typename T>
inline
AimsData<T>::Private::Private()
  : header( 0 )
{
}


template<typename T>
inline
AimsData<T>::Private::~Private()
{
  if( header )
    delete header;
}


template<typename T>
inline
int AimsData<T>::Private::borderWidth( carto::rc_ptr<carto::Volume<T> > vol )
{
  try
    {
      carto::Object 
        o = vol->header().getProperty( "_borderWidth" );
      if( o.get() )
        return (int) rint( o->getScalar() );
    }
  catch( ... )
    {
    }
  return 0;
}


// AimsData methods

template <typename T>
AimsData<T>::operator const carto::rc_ptr<carto::Volume<T> >() const
{
  return volume();
}


template <typename T>
AimsData<T>::operator carto::rc_ptr<carto::Volume<T> >()
{
  return volume();
}


template<typename T>
inline
typename AimsData<T>::iterator AimsData<T>::begin()
{
  return &*_volume->begin();
}


template<typename T>
inline
typename AimsData<T>::const_iterator AimsData<T>::begin() const
{
  return &*_volume->begin();
}


template<typename T>
inline
typename AimsData<T>::iterator AimsData<T>::end()
{
#ifdef CARTO_USE_BLITZ
  return &*_volume->begin() + _volume->getSizeX() * _volume->getSizeY()
      * _volume->getSizeZ() * _volume->getSizeT();
#else
  return &*_volume->end();
#endif
}


template<typename T>
inline
typename AimsData<T>::const_iterator AimsData<T>::end() const
{
#ifdef CARTO_USE_BLITZ
  return &*_volume->begin() + _volume->getSizeX() * _volume->getSizeY()
      * _volume->getSizeZ() * _volume->getSizeT();
#else
  return &*_volume->end();
#endif
}


template<typename T>
inline
bool AimsData<T>::empty() const
{
  return _volume->begin() == _volume->end();
}


template<typename T>
inline
AimsData<T>::AimsData( int dimx, int dimy, int dimz, int dimt, int borderw )
  : carto::RCObject(), aims::Border( dimx, dimy, dimz, borderw ), 
    _volume( new carto::Volume<T>( dimx + borderw * 2, dimy + borderw * 2, 
                                   dimz + borderw * 2, dimt ) ), 
    d( new Private )
{
  d->header = new aims::PythonHeader( *_volume );
  if( borderw != 0 )
    _volume->header().setProperty( "_borderWidth", borderw );
}


template < typename T >
inline
AimsData<T>::AimsData( int dimx, int dimy, int dimz, int dimt, 
		       int borderw, const carto::AllocatorContext & al )
  : carto::RCObject(), aims::Border( dimx, dimy, dimz, borderw ), 
    _volume( new carto::Volume<T>( dimx + borderw * 2, dimy + borderw * 2, 
                                   dimz + borderw * 2, dimt, al ) ), 
    d( new Private )
{
  d->header = new aims::PythonHeader( *_volume );
  if( borderw != 0 )
    _volume->header().setProperty( "_borderWidth", borderw );
}


template < typename T >
inline 
AimsData<T>::AimsData( const AimsData<T>& other )
  : carto::RCObject(), aims::Border( other.dimX(), other.dimY(), 
		  other.dimZ(), other.borderWidth() ), 
    _volume( other._volume ), 
    d( new Private )
{
  d->header = new aims::PythonHeader( *_volume );
  if( borderWidth() != 0 )
    _volume->header().setProperty( "_borderWidth", borderWidth() );
}


template < typename T >
inline 
AimsData<T>::AimsData( const AimsData<T>& other, int borderw)
  : carto::RCObject(), aims::Border( other.dimX(), other.dimY(), other.dimZ(),
  borderw ), _volume( new carto::Volume<T>( other.dimX() + borderw * 2,
  other.dimY() + borderw * 2, other.dimZ() + borderw * 2, other.dimT() ) ), 
    d( new Private )
{
  int x, xm = dimX(), y, ym = dimY(), z, zm = dimZ(), t, tm = dimT();
  for ( t = 0; t < tm; t++ )
    for ( z = 0; z < zm; z++ )
      for ( y = 0; y < ym; y++ )
        for ( x = 0; x < xm; x++ )
          (*this)( x, y, z, t ) = other( x, y, z, t );
  d->header = new aims::PythonHeader( *_volume );
  if( borderw != 0 )
    _volume->header().setProperty( "_borderWidth", borderw );
}


template < typename T >
inline 
AimsData<T>::~AimsData()
{
  delete d;
}


template < typename T >
inline 
AimsData<T>::AimsData( carto::rc_ptr<carto::Volume<T> > vol )
  : carto::RCObject(), 
    aims::Border( vol->getSizeX() - Private::borderWidth( vol ) * 2, 
                  vol->getSizeY() - Private::borderWidth( vol ) * 2, 
                  vol->getSizeZ() - Private::borderWidth( vol ) * 2, 
                  Private::borderWidth( vol ) ), 
    _volume( vol ), 
    d( new Private )
{
  d->header = new aims::PythonHeader( *_volume );
}


template < typename T >
inline 
AimsData<T> & AimsData<T>::operator = ( carto::rc_ptr<carto::Volume<T> > vol )
{
  if( _volume.get() == vol.get() )
    return *this;

  int	border = Private::borderWidth( vol );
  _setBorder( vol->getSizeX() - border * 2, vol->getSizeY() - border * 2, 
              vol->getSizeZ() - border * 2, border );
  delete d->header;
  _volume = vol;
  d->header = new aims::PythonHeader( *_volume );

  return *this;
}


template < typename T >
inline
AimsData<T>& AimsData<T>::operator = ( const AimsData<T>& other )
{
  if ( &other == this )
    return *this;

  _setBorder( other.dimX(), other.dimY(), other.dimZ(), other.borderWidth() );
  delete d->header;
  _volume = other._volume;
  *d = *other.d;
  d->header = new aims::PythonHeader( *_volume );
  return *this;
}


template < typename T >
inline
AimsData<T>& AimsData<T>::operator = ( const T& val )
{
  iterator it = begin() + oFirstPoint();
  int x, xm = dimX(), y, ym = dimY(), z, zm = dimZ(), t, tm = dimT();

  for ( t = 0; t < tm; t++ )
  {
    for ( z = 0; z < zm; z++ )
    {
      for ( y = 0; y < ym; y++ )
      {
        for ( x = 0; x < xm; x++ )
          *it++ = val;
        it += oPointBetweenLine();
      }
      it += oLineBetweenSlice();
    }
    it += oSliceBetweenVolume();
  }
  return *this;
}


template<typename T>
inline
carto::rc_ptr<carto::Volume<T> > AimsData<T>::volume()
{
  return _volume;
}


template<typename T>
inline
carto::rc_ptr<carto::Volume<T> > AimsData<T>::volume() const
{
  return _volume;
}


template<typename T>
inline
int AimsData<T>::dimX() const
{
  return _volume->getSizeX() - borderWidth() * 2;
}


template<typename T>
inline
int AimsData<T>::dimY() const
{
  return _volume->getSizeY() - borderWidth() * 2;
}


template<typename T>
inline
int AimsData<T>::dimZ() const
{
  return _volume->getSizeZ() - borderWidth() * 2;
}


template<typename T>
inline
int AimsData<T>::dimT() const
{
  return _volume->getSizeT();
}


template < typename T >
inline
float AimsData<T>::sizeX() const
{
  if( !d->header )
    return 1;
  aims::PythonHeader *ph = dynamic_cast<aims::PythonHeader *>( d->header );
  if( !ph )
    return 1;
  try
    {
      carto::Object	vs = ph->getProperty( "voxel_size" );
      if( !vs.isNull() && vs->size() >= 1 )
        return (float) vs->getArrayItem( 0 )->getScalar();
    }
  catch( ... )
    {
    }
  return 1;
}


template < typename T >
inline
float AimsData<T>::sizeY() const
{
  if( !d->header )
    return 1;
  aims::PythonHeader *ph = dynamic_cast<aims::PythonHeader *>( d->header );
  if( !ph )
    return 1;
  try
    {
      carto::Object	vs = ph->getProperty( "voxel_size" );
      if( !vs.isNull() && vs->size() >= 2 )
        return (float) vs->getArrayItem( 1 )->getScalar();
    }
  catch( ... )
    {
    }
  return 1;
}


template < typename T >
inline
float AimsData<T>::sizeZ() const
{
  if( !d->header )
    return 1;
  aims::PythonHeader *ph = dynamic_cast<aims::PythonHeader *>( d->header );
  if( !ph )
    return 1;
  try
    {
      carto::Object	vs = ph->getProperty( "voxel_size" );
      if( !vs.isNull() && vs->size() >= 3 )
        return (float) vs->getArrayItem( 2 )->getScalar();
    }
  catch( ... )
    {
    }
  return 1;
}


template < typename T >
inline
float AimsData<T>::sizeT() const
{
  if( !d->header )
    return 1;
  aims::PythonHeader *ph = dynamic_cast<aims::PythonHeader *>( d->header );
  if( !ph )
    return 1;
  try
    {
      carto::Object	vs = ph->getProperty( "voxel_size" );
      if( !vs.isNull() && vs->size() >= 4 )
        return (float) vs->getArrayItem( 3 )->getScalar();
    }
  catch( ... )
    {
    }
  return 1;
}


template < typename T >
inline
void AimsData<T>::setSizeX( float sizex )
{
  if( !d->header )
    setHeader( new aims::PythonHeader );
  aims::PythonHeader *ph = dynamic_cast<aims::PythonHeader *>( d->header );
  if( ph )
    {
      try
        {
          carto::Object	vs = ph->getProperty( "voxel_size" );
          if( !vs.isNull() )
            if( vs->size() >= 1 )
              vs->setArrayItem( 0, carto::Object::value( sizex ) );
        }
      catch( ... )
        {
          std::vector<float>  vs( 4, 1. );
          vs[0] = sizex;
          ph->setProperty( "voxel_size", vs );
        }
    }
}


template < typename T >
inline
void AimsData<T>::setSizeY( float sizey )
{
  if( !d->header )
    setHeader( new aims::PythonHeader );
  aims::PythonHeader *ph = dynamic_cast<aims::PythonHeader *>( d->header );
  if( ph )
  {
    std::vector<float>  vs( 4, 1. );
    try
    {
      carto::Object	vso = ph->getProperty( "voxel_size" );
      if( !vso.isNull() )
      {
        if( vso->size() >= 2 )
        {
          vso->setArrayItem( 1, carto::Object::value( sizey ) );
          return;
        }
        else
        {
          if( vso->size() >= 1 )
            vs[0] = vso->getArrayItem(0)->getScalar();
        }
      }
    }
  catch( ... )
    {
    }
  vs[1] = sizey;
    ph->setProperty( "voxel_size", vs );
  }
}


template < typename T >
inline
void AimsData<T>::setSizeZ( float sizez )
{
  if( !d->header )
    setHeader( new aims::PythonHeader );
  aims::PythonHeader *ph = dynamic_cast<aims::PythonHeader *>( d->header );
  if( ph )
    {
      std::vector<float>  vs( 4, 1. );
      try
        {
          carto::Object	vso = ph->getProperty( "voxel_size" );
          if( !vso.isNull() )
          {
            if( vso->size() >= 3 )
              {
                vso->setArrayItem( 2, carto::Object::value( sizez ) );
                return;
              }
            else
              {
                carto::Object	it;
                int	i;
                for( i=0, it=vso->objectIterator(); 
                     it->isValid() && i<3; ++i, it->next() )
                  vs[i] = it->currentValue()->getScalar();
              }
          }
        }
      catch( ... )
        {
        }
      vs[2] = sizez;
      ph->setProperty( "voxel_size", vs );
    }
}


template < typename T >
inline
void AimsData<T>::setSizeT( float sizet )
{
  if( !d->header )
    setHeader( new aims::PythonHeader );
  aims::PythonHeader *ph = dynamic_cast<aims::PythonHeader *>( d->header );
  if( ph )
    {
      std::vector<float>  vs( 4, 1. );
      try
        {
          carto::Object	vso = ph->getProperty( "voxel_size" );
          if( !vso.isNull() )
          {
            if( vso->size() >= 4 )
              {
                vso->setArrayItem( 3, carto::Object::value( sizet ) );
                return;
              }
            else
              {
                carto::Object	it;
                int	i;
                for( i=0, it=vso->objectIterator(); 
                     it->isValid() && i<4; ++i, it->next() )
                  vs[i] = it->currentValue()->getScalar();
              }
          }
        }
      catch( ... )
        {
        }
      vs[3] = sizet;
      ph->setProperty( "voxel_size", vs );
    }
}


template < typename T >
inline
void 
AimsData<T>::setSizeXYZT( float sizex, float sizey, float sizez, float sizet )
{
  if( !d->header )
    setHeader( new aims::PythonHeader );
  aims::PythonHeader *ph = dynamic_cast<aims::PythonHeader *>( d->header );
  if( ph )
    {
      std::vector<float>  vs( 4 );
      vs[0] = sizex;
      vs[1] = sizey;
      vs[2] = sizez;
      vs[3] = sizet;
      ph->setProperty( "voxel_size", vs );
    }
}


template < typename T >
inline
void AimsData<T>::setSizeXYZT( const AimsData<T>& other )
{
  if( !d->header )
    setHeader( new aims::PythonHeader );
  aims::PythonHeader *ph = dynamic_cast<aims::PythonHeader *>( d->header );
  if( ph )
    {
      std::vector<float>  vs( 4 );
      vs[0] = other.sizeX();
      vs[1] = other.sizeY();
      vs[2] = other.sizeZ();
      vs[3] = other.sizeT();
      ph->setProperty( "voxel_size", vs );
    }
}


template < typename T >
inline 
const carto::AllocatorContext & AimsData<T>::allocator() const
{
  return _volume->allocatorContext();
}


template < typename T >
inline
const aims::Header* AimsData<T>::header() const
{
  return d->header;
}


template < typename T >
inline
aims::Header* AimsData<T>::header()
{
  return d->header;
}


template<typename T>
inline
void AimsData<T>::setHeader( aims::Header* hdr )
{
  if( hdr != d->header )
    {
      aims::PythonHeader	*mh;
      if( !d->header )
        {
          d->header = mh = new aims::PythonHeader( *_volume );
        }
      else
        {
          mh = static_cast<aims::PythonHeader *>( d->header );
          carto::Object	oldvs;
          try
            {
              // retreive old voxel size (if any)
              oldvs = mh->getProperty( "voxel_size" );
            }
          catch( ... )
            {
            }
          mh->clearProperties();
          if( !oldvs.isNull() )
            /* keep old voxel size which was not part of the dynamic header 
               in the old AimsData */
            mh->setProperty( "voxel_size", oldvs );
        }
      const aims::PythonHeader 
        *ph = dynamic_cast<const aims::PythonHeader *>( hdr );
      if( ph )
        mh->copy( *ph );
      // hdr used to transfor ownership to me
      delete hdr;
    }
}


template < typename T >
inline
typename AimsData<T>::reference 
AimsData<T>::operator [] ( AimsData<T>::size_type n )
{
  return *( begin() + n );
}


template < typename T >
inline
typename AimsData<T>::const_reference 
AimsData<T>::operator [] ( AimsData<T>::size_type n ) const
{
  return *( begin() + n );
}


template < typename T >
inline
typename AimsData<T>::reference 
AimsData<T>::operator () ( AimsData<T>::size_type x, AimsData<T>::size_type y, 
                           AimsData<T>::size_type z, AimsData<T>::size_type t )
{
  return (*_volume)( x + borderWidth(), y + borderWidth(), z + borderWidth(), 
                     t );
}


template < typename T >
inline
typename AimsData<T>::const_reference 
AimsData<T>::operator () ( AimsData<T>::size_type x, AimsData<T>::size_type y, 
                           AimsData<T>::size_type z, 
                           AimsData<T>::size_type t ) const
{
  return (*_volume)( x + borderWidth(), y + borderWidth(), z + borderWidth(), 
                     t );
}


template < typename T >
inline
typename AimsData<T>::reference 
AimsData<T>::operator () ( const Point4d & pt )
{
  return (*this)( pt[0], pt[1], pt[2], pt[3] );
}


template < typename T >
inline
typename AimsData<T>::const_reference 
AimsData<T>::operator () ( const Point4d & pt ) const
{
  return (*this)( pt[0], pt[1], pt[2], pt[3] );
}


template < typename T >
inline
typename AimsData<T>::reference 
AimsData<T>::operator () ( const Point3d & pt )
{
  return (*this)( pt[0], pt[1], pt[2] );
}


template < typename T >
inline
typename AimsData<T>::const_reference 
AimsData<T>::operator () ( const Point3d & pt ) const
{
  return (*this)( pt[0], pt[1], pt[2] );
}


template < typename T >
inline
typename AimsData<T>::reference 
AimsData<T>::operator () ( const Point2d & pt )
{
  return (*this)( pt[0], pt[1] );
}


template < typename T >
inline
typename AimsData<T>::const_reference 
AimsData<T>::operator () ( const Point2d & pt ) const
{
  return (*this)( pt[0], pt[1] );
}


template < typename T >
inline
T AimsData<T>::minimum() const 
{
  T mini;
  const_iterator it = begin() + oFirstPoint();

  mini = *it;
  int x, xm = dimX(), y, ym = dimY(), z, zm = dimZ(), t, tm = dimT();

  for ( t = 0; t < tm; t++ )
  {
    for ( z = 0; z < zm; z++ )
    {
      for ( y = 0; y < ym; y++ )
      {
        for ( x = 0; x < xm; x++ )
        {
          if ( *it < mini )
            mini = *it;
          it++;
        }
        it += oPointBetweenLine();
      }
      it += oLineBetweenSlice();
    }
    it += oSliceBetweenVolume();
  }

  return mini;
}


template < typename T >
inline
T AimsData<T>::maximum() const 
{
  T maxi;
  const_iterator it = begin() + oFirstPoint();

  maxi = *it;
  int x, xm = dimX(), y, ym = dimY(), z, zm = dimZ(), t, tm = dimT();

  for ( t = 0; t < tm; t++ )
  {
    for ( z = 0; z < zm; z++ )
    {
      for ( y = 0; y < ym; y++ )
      {
        for ( x = 0; x < xm; x++ )
        {
          if ( *it > maxi )
            maxi = *it;
          it++;
        }
        it += oPointBetweenLine();
      }
      it += oLineBetweenSlice();
    }
    it += oSliceBetweenVolume();
  }

  return maxi;
}


template < typename T >
inline
T AimsData<T>::minIndex( int* xx, int* yy, int* zz, int* tt ) const
{
  int minxind = 0,minyind = 0,minzind = 0,mintind = 0;
  T mini;
  const_iterator it = begin() + oFirstPoint();

  mini = *it;
  int x, xm = dimX(), y, ym = dimY(), z, zm = dimZ(), t, tm = dimT();

  for ( t = 0; t < tm; t++ )
  {
    for ( z = 0; z < zm; z++ )
    {
      for ( y = 0; y < ym; y++ )
      {
        for ( x = 0; x < xm; x++ )
        {
          if ( *it < mini )
          {
            mini = *it;
            minxind = x;
            minyind = y;
            minzind = z;
            mintind = t;
          }
          it++;        
        }
        it += oPointBetweenLine();
      }
      it += oLineBetweenSlice();
    }
    it += oSliceBetweenVolume();
  }
  if ( xx )
    *xx = minxind;
  if ( yy )
    *yy = minyind;
  if ( zz )
    *zz = minzind;
  if ( tt )
    *tt = mintind;

  return mini; 
}


template < typename T >
inline
T AimsData<T>::maxIndex( int* xx, int* yy, int* zz, int* tt ) const
{
  int maxxind = 0,maxyind = 0,maxzind = 0,maxtind = 0;
  T maxi;
  const_iterator it = begin() + oFirstPoint();

  maxi = *it;
  int x, xm = dimX(), y, ym = dimY(), z, zm = dimZ(), t, tm = dimT();

  for ( t = 0; t < tm; t++ )
  {
    for ( z = 0; z < zm; z++ )
    {
      for ( y = 0; y < ym; y++ )
      {
        for ( x = 0; x < xm; x++ )
        {
          if ( *it > maxi )
          {
            maxi = *it;
            maxxind = x;
            maxyind = y;
            maxzind = z;
            maxtind = t;
          }
          it++;
        }
        it += oPointBetweenLine();
      }
      it += oLineBetweenSlice();
    }
    it += oSliceBetweenVolume();
  }
  if ( xx )
    *xx = maxxind;
  if ( yy )
    *yy = maxyind;
  if ( zz )
    *zz = maxzind;
  if ( tt )
    *tt = maxtind;

  return maxi;
}


template < typename T >
inline
void AimsData<T>::fillBorder( const T& val )
{
  iterator	it;
  int x, xm = dimX(), y, ym = dimY(), z, zm = dimZ(), t, tm = dimT();
  int of = oFirstPoint(), op = oPointBetweenLine(), ol = oLineBetweenSlice();

  for ( t = 0; t < tm; t++ )
  {
    it = begin() + t * oVolume();
    for ( x = 0; x < of; x++ )
      *it++ = val;
    for ( z = 0; z < zm; z++ )
    {
      for ( y = 0; y < ym; y++ )
      {
        it += xm;
        for ( x = 0; x < op; x++ )
          *it++ = val;
      }
      for ( x = 0; x < ol; x++ )
        *it++ = val;
    }
    for ( x = 0; x < of - op - ol; x++ )
      *it++ = val;
  }
}


template<typename T>
AimsData<T> AimsData<T>::clone() const
{
  AimsData<T>	dat( *this );
  dat._volume.reset( new carto::Volume<T>( *_volume ) );
  delete dat.d->header;
  dat.d->header = new aims::PythonHeader( *dat._volume );
  return dat;
}


template < typename T >
inline
AimsData<T> AimsData<T>::cross( const AimsData<T>& other )
{
  ASSERT( dimT() == 1 && other.dimT() == 1 &&
          dimZ() == 1 && other.dimZ() == 1 );
  ASSERT( dimY() == other.dimX() );

  AimsData<T> prod( dimX(), other.dimY(), 1, 1,
                    std::max( borderWidth(), other.borderWidth() ) );
  for ( int y = 0; y < other.dimY(); y++ )
    for ( int x = 0; x < dimX(); x++ )
    {
      prod( x, y ) = T( 0 );
      for ( int k = 0; k < dimY(); k++ )
        prod( x, y ) += (*this)( x, k ) * other( k, y );
    }

  return prod;
}


template < typename T >
inline
AimsData<T>& AimsData<T>::transpose()
{
  AimsData<T> tmp( dimY(), dimX(), dimZ(), dimT(), borderWidth() );
  aims::PythonHeader 
    *ph = dynamic_cast<aims::PythonHeader *>( header() );
  if( ph )
    {
      ph = static_cast<aims::PythonHeader *>( ph->cloneHeader() );
      if( ph->hasProperty( "volume_dimension" ) )
        ph->removeProperty( "volume_dimension" );
      if( ph->hasProperty( "voxel_size" ) )
        ph->removeProperty( "voxel_size" );
      tmp.setHeader( ph );
    }
  tmp.setSizeXYZT( sizeY(), sizeX(), sizeZ(), sizeT() );
  tmp.fillBorder( T( 0 ) );

  int x, xm = dimX(), y, ym = dimY(), z, zm = dimZ(), t, tm = dimT();

  for ( t = 0; t < tm; t++ )
    for ( z = 0; z < zm; z++ )
      for ( y = 0; y < ym; y++ )
        for ( x = 0; x < xm; x++ )
          tmp( y, x, z, t ) = (*this)( x, y, z, t );

  *this = tmp;
  return *this;
}


template < typename T >
inline
AimsData<T> operator + ( const AimsData<T>& firstThing, 
                         const AimsData<T>& secondThing )
{
  return carto::VolumeRef<T>( firstThing.volume() ) 
    + carto::VolumeRef<T>( secondThing.volume() );
}


template < typename T >
inline
AimsData<T> operator - ( const AimsData<T>& firstThing, 
                         const AimsData<T>& secondThing )
{
  return carto::VolumeRef<T>( firstThing.volume() ) - 
    carto::VolumeRef<T>( secondThing.volume() );
}

template < typename T >
inline
AimsData<T> operator * ( const AimsData<T>& firstThing, float scale )
{
  return carto::VolumeRef<T>( firstThing.volume() ) * scale;
}


// macros

#define ForEach4d( thing , x , y , z , t) \
  for ( t = 0; t < thing.dimT(); t++ ) \
    for ( z = 0; z < thing.dimZ(); z++ ) \
      for ( y = 0; y < thing.dimY(); y++ ) \
        for ( x = 0; x < thing.dimX(); x++ )

#define ForEach3d( thing , x , y , z) \
  for ( z = 0; z < thing.dimZ(); z++ ) \
    for ( y = 0; y < thing.dimY(); y++ ) \
      for ( x = 0; x < thing.dimX(); x++ )

#define ForEach2d( thing , x , y) \
  for ( y = 0; y < thing.dimY(); y++ ) \
    for ( x = 0; x < thing.dimX(); x++ )

#define ForEach1d( thing , x) \
  for ( x = 0; x < thing.dimX(); x++ )


#define ForEach4dp( thing , x , y , z , t) \
  for ( t = 0; t < thing->dimT(); t++ ) \
    for ( z = 0; z < thing->dimZ(); z++ ) \
      for ( y = 0; y < thing->dimY(); y++ ) \
        for ( x = 0; x < thing->dimX(); x++ )

#define ForEach3dp( thing , x , y , z) \
  for ( z = 0; z < thing->dimZ(); z++ ) \
    for ( y = 0; y < thing->dimY(); y++ ) \
      for ( x = 0; x < thing->dimX(); x++ )

#define ForEach2dp( thing , x , y) \
  for ( y = 0; y < thing->dimY(); y++ ) \
    for ( x = 0; x < thing->dimX(); x++ )

#define ForEach1dp( thing , x) \
  for ( x = 0; x < thing->dimX(); x++ )


class AimsRGB;
class AimsRGBA;
class DtiTensor;

namespace carto
{
  //  instanciations of Volume classes

  extern template class VolumeProxy<AimsRGB>;
  extern template class Volume<AimsRGB>;
  extern template class VolumeProxy<AimsRGBA>;
  extern template class Volume<AimsRGBA>;
  extern template class VolumeProxy<Point3d>;
  extern template class Volume<Point3d>;
  extern template class VolumeProxy<Point3df>;
  extern template class Volume<Point3df>;
  extern template class VolumeProxy<Point2d>;
  extern template class Volume<Point2d>;
  extern template class VolumeProxy<AimsVector<float,6> >;
  extern template class Volume<AimsVector<float,6> >;
  extern template class VolumeProxy<DtiTensor*>;
  extern template class Volume<DtiTensor*>;

  // VolumeUtil declarations (needed on Mac)
  extern template VolumeRef<AimsRGB> 
  VolumeUtil<AimsRGB>::apply( Scaler<AimsRGB>, const VolumeRef<AimsRGB> & );
  extern template void 
  VolumeUtil<AimsRGB>::selfApply( Scaler<AimsRGB>, VolumeRef<AimsRGB> & );
  extern template VolumeRef<AimsRGB> 
  VolumeUtil<AimsRGB>::apply( std::plus<AimsRGB>, const VolumeRef<AimsRGB> &, 
                              const VolumeRef<AimsRGB> & );
  extern template void 
  VolumeUtil<AimsRGB>::selfApply( std::plus<AimsRGB>, VolumeRef<AimsRGB> &, 
                                  const VolumeRef<AimsRGB> & );
  extern template VolumeRef<AimsRGB> 
  VolumeUtil<AimsRGB>::apply( UnaryFromConstantBinaryFunctor<AimsRGB, 
                              std::plus<AimsRGB> >, 
                              const VolumeRef<AimsRGB> & );
  extern template void 
  VolumeUtil<AimsRGB>::selfApply( UnaryFromConstantBinaryFunctor<AimsRGB, 
                                  std::plus<AimsRGB> >, VolumeRef<AimsRGB> & );
  extern template VolumeRef<AimsRGB> 
  VolumeUtil<AimsRGB>::apply( UnaryFromConstantBinaryFunctor2<AimsRGB, 
                              std::plus<AimsRGB> >, 
                              const VolumeRef<AimsRGB> & );
  extern template VolumeRef<AimsRGB> 
  VolumeUtil<AimsRGB>::apply( std::minus<AimsRGB>, const VolumeRef<AimsRGB> &, 
                              const VolumeRef<AimsRGB> & );
  extern template void 
  VolumeUtil<AimsRGB>::selfApply( std::minus<AimsRGB>, VolumeRef<AimsRGB> &, 
                                  const VolumeRef<AimsRGB> & );
  extern template VolumeRef<AimsRGB> 
  VolumeUtil<AimsRGB>::apply( UnaryFromConstantBinaryFunctor<AimsRGB, 
                              std::minus<AimsRGB> >, 
                              const VolumeRef<AimsRGB> & );
  extern template void 
  VolumeUtil<AimsRGB>::selfApply( UnaryFromConstantBinaryFunctor<AimsRGB, 
                                  std::minus<AimsRGB> >, 
                                  VolumeRef<AimsRGB> & );
  extern template VolumeRef<AimsRGB> 
  VolumeUtil<AimsRGB>::apply( UnaryFromConstantBinaryFunctor2<AimsRGB, 
                              std::minus<AimsRGB> >, 
                              const VolumeRef<AimsRGB> & );

  extern template VolumeRef<AimsRGBA> 
  VolumeUtil<AimsRGBA>::apply( Scaler<AimsRGBA>, const VolumeRef<AimsRGBA> & );
  extern template void 
  VolumeUtil<AimsRGBA>::selfApply( Scaler<AimsRGBA>, VolumeRef<AimsRGBA> & );
  extern template VolumeRef<AimsRGBA> 
  VolumeUtil<AimsRGBA>::apply( std::plus<AimsRGBA>, 
                               const VolumeRef<AimsRGBA> &, 
                               const VolumeRef<AimsRGBA> & );
  extern template void 
  VolumeUtil<AimsRGBA>::selfApply( std::plus<AimsRGBA>, VolumeRef<AimsRGBA> &, 
                                   const VolumeRef<AimsRGBA> & );
  extern template VolumeRef<AimsRGBA> 
  VolumeUtil<AimsRGBA>::apply( UnaryFromConstantBinaryFunctor<AimsRGBA, 
                               std::plus<AimsRGBA> >, 
                               const VolumeRef<AimsRGBA> & );
  extern template void 
  VolumeUtil<AimsRGBA>::selfApply( UnaryFromConstantBinaryFunctor<AimsRGBA, 
                                   std::plus<AimsRGBA> >, 
                                   VolumeRef<AimsRGBA> & );
  extern template VolumeRef<AimsRGBA> 
  VolumeUtil<AimsRGBA>::apply( UnaryFromConstantBinaryFunctor2<AimsRGBA, 
                               std::plus<AimsRGBA> >, 
                               const VolumeRef<AimsRGBA> & );
  extern template VolumeRef<AimsRGBA> 
  VolumeUtil<AimsRGBA>::apply( std::minus<AimsRGBA>, 
                               const VolumeRef<AimsRGBA> &, 
                               const VolumeRef<AimsRGBA> & );
  extern template void 
  VolumeUtil<AimsRGBA>::selfApply( std::minus<AimsRGBA>, 
                                   VolumeRef<AimsRGBA> &, 
                                   const VolumeRef<AimsRGBA> & );
  extern template VolumeRef<AimsRGBA> 
  VolumeUtil<AimsRGBA>::apply( UnaryFromConstantBinaryFunctor<AimsRGBA, 
                               std::minus<AimsRGBA> >, 
                               const VolumeRef<AimsRGBA> & );
  extern template void 
  VolumeUtil<AimsRGBA>::selfApply( UnaryFromConstantBinaryFunctor<AimsRGBA, 
                                   std::minus<AimsRGBA> >, 
                                   VolumeRef<AimsRGBA> & );
  extern template VolumeRef<AimsRGBA> 
  VolumeUtil<AimsRGBA>::apply( UnaryFromConstantBinaryFunctor2<AimsRGBA, 
                               std::minus<AimsRGBA> >, 
                               const VolumeRef<AimsRGBA> & );
}


#endif

