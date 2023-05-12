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

#ifndef AIMS_data_SPARSEVOLUME_H
#define AIMS_data_SPARSEVOLUME_H

#include <aims/bucket/bucket.h>
#include <cartodata/volume/volume.h>

namespace aims
{

  template <typename T>
  class SparseVolume
  {
  public:
  };


  template <typename T>
  class SparseVolume<carto::Volume<T> >
  {
  public:
    class LowLevelStorage
    {
    public:
      typedef T* iterator;
      typedef const T* const_iterator;

      LowLevelStorage() : _begin(0), _end(0) {}
      LowLevelStorage( carto::VolumeRef<T> vol, int y, int z, int t )
        : _volume( vol ), _begin( &vol->at( 0, y, z, t ) ),
          _end( &vol->at( vol->getSizeX(), y, z, t ) )
      {}
      iterator begin() { return _begin; }
      const_iterator begin() const { return _begin; }
      iterator end() { return _end; }
      const_iterator end() const { return _end; }
      carto::VolumeRef<T> volume() const { return _volume; }

    private:
      carto::VolumeRef<T> _volume;
      T* _begin;
      T* _end;
    };


    class const_LowLevelStorage
    {
    public:
      typedef const T* const_iterator;

      const_LowLevelStorage() : _begin(0), _end(0) {}
      const_LowLevelStorage( const carto::VolumeRef<T> vol, int y, int z,
                             int t )
        : _volume( vol ), _begin( &vol->at( 0, y, z, t ) ),
          _end( &vol->at( vol->getSizeX(), y, z, t ) )
      {}
      const_LowLevelStorage( const LowLevelStorage & other )
        : _volume( other.volume() ), _begin( other.begin() ),
          _end( other.end() )
      {}
      const_iterator begin() const { return _begin; }
      const_iterator end() const { return _end; }
      carto::VolumeRef<T> volume() const { return _volume; }

    private:
      carto::VolumeRef<T> _volume;
      const T* _begin;
      const T* _end;
    };


    class iterator
    {
    public:
      iterator() : _pos( Point4d( 0, 0, 0 ), LowLevelStorage() ) {}
      iterator( carto::VolumeRef<T> vol, int y, int z, int t )
        : _pos( Point4d( 0, y, z, t ), LowLevelStorage( vol, y, z, t ) ) {}
      std::pair<Point4d, LowLevelStorage> & operator * ()
      { return _pos; }
      std::pair<Point4d, LowLevelStorage> *operator -> ()
      { return &_pos; }
      LowLevelStorage & storage() { return _pos.second; }
      bool operator == ( const iterator & x ) const
      {
        return &_pos.second.volume()->at( 0, _pos.first[1], _pos.first[2],
                                   _pos.first[3] )
          == &x._pos.second.volume()->at( 0, x._pos.first[1], x._pos.first[2],
                                   x._pos.first[3] );
      }
      bool operator != ( const iterator & x ) const
      { return !( *this == x ); }
      iterator & operator ++ ()
      {
        int16_t & y = _pos.first[1];
        int16_t & z = _pos.first[2];
        int16_t & t = _pos.first[3];
        ++y;
        if( y >= _pos.second.volume()->getSizeY() )
        {
          y = 0;
          ++z;
          if( z >= _pos.second.volume()->getSizeZ() )
          {
            z = 0;
            ++t;
          }
        }
        _pos.second = LowLevelStorage( _pos.second.volume(), y, z, t );
        return *this;
      }

    private:
      std::pair<Point4d, LowLevelStorage> _pos;
    };

    class const_iterator
    {
    public:
      const_iterator() : _pos( Point4d( 0, 0, 0 ), const_LowLevelStorage() ) {}
      const_iterator( carto::VolumeRef<T> vol, int y, int z, int t )
        : _pos( Point4d( 0, y, z, t ), const_LowLevelStorage( vol, y, z, t ) )
      {}
      const_iterator( iterator other )
      : _pos( other->first, other->second )
      {}
      const std::pair<Point4d, const_LowLevelStorage> & operator * () const
      { return _pos; }
      const std::pair<Point4d, const_LowLevelStorage> *operator -> () const
      { return &_pos; }
      const const_LowLevelStorage & storage() const { return _pos.second; }
      bool operator == ( const const_iterator & x ) const
      {
        return &_pos.second.volume()->at( 0, _pos.first[1], _pos.first[2],
                                   _pos.first[3] )
            == &x._pos.second.volume()->at( 0, x._pos.first[1],
                                     x._pos.first[2], x._pos.first[3] );
      }
      bool operator != ( const const_iterator & x ) const
      { return !( *this == x ); }
      const_iterator & operator ++ ()
      {
        int16_t & y = _pos.first[1];
        int16_t & z = _pos.first[2];
        int16_t & t = _pos.first[3];
        ++y;
        if( y >= _pos.second.volume()->getSizeY() )
        {
          y = 0;
          ++z;
          if( z >= _pos.second.volume()->getSizeZ() )
          {
            z = 0;
            ++t;
          }
        }
        _pos.second = const_LowLevelStorage( _pos.second.volume(), y, z, t );
        return *this;
      }

    private:
      std::pair<Point4d, const_LowLevelStorage> _pos;
    };

    SparseVolume( int sizeX = 1, int sizeY = 1, int sizeZ = 1, int sizeT = 1,
                  const carto::AllocatorContext& allocatorContext
                  = carto::AllocatorContext(), bool allocated = true );
    SparseVolume( int sizeX, int sizeY, int sizeZ, int sizeT, T* buffer );
    SparseVolume( const carto::VolumeRef< T >& other );
    SparseVolume( const carto::Volume< T >& other );
    SparseVolume( const carto::rc_ptr<carto::Volume<T> > & x );
    virtual ~SparseVolume();
    carto::VolumeRef<T> data() const { return _data; }
    void reset( carto::Volume<T> *x );
    template <typename U>
    static SparseVolume<carto::Volume<T> > alloc(
        const SparseVolume<carto::Volume<U> > & other );
    const T & background() const { return _background; }
    void setBackground( const T & value ) { _background = value; }
    std::vector<int> getSize() const;
    const carto::PropertySet& header() const
    { return _data->header(); }
    carto::PropertySet& header() { return _data->header(); }
    std::vector<float> voxelSize() const;

    void fill( const T & value )
    { _data->fill( value ); setBackground( value ); }
    iterator begin() { return iterator( data(), 0, 0, 0 ); }
    iterator end() { return iterator( data(), 0, 0, data()->getSizeT() ); }
    const_iterator begin() const { return const_iterator( data(), 0, 0, 0 ); }
    const_iterator end() const
    { return const_iterator( data(), 0, 0, data()->getSizeT() ); }
    const T & at( int x, int y = 0, int z = 0, int t = 0 ) const
    { return _data->at( x, y, z, t ); }
    void setValue( const T & value,  int x, int y = 0, int z = 0, int t = 0 )
    { _data->at( x, y, z, t ) = value; }
    const T & at( const Point3d & p ) const
    { return _data->at( p[0], p[1], p[2], 0 ); }
    void setValue( const T & value,  const Point3d & p )
    { _data->at( p[0], p[1], p[2], 0 ) = value; }
    /// checks volume bounds before returning value
    const T & checkedAt( const Point3d & p ) const
    { return checkedAt( p[0], p[1], p[2] ); }
    const T & checkedAt( int x, int y = 0, int z = 0, int t = 0 ) const;
    static Point4d position
        ( const const_iterator & i,
          const typename const_LowLevelStorage::const_iterator & iv )
    { Point4d x( i->first ); x[0] = iv - i->storage().begin(); return x; }
    static Point3d position3d
        ( const const_iterator & i,
          const typename const_LowLevelStorage::const_iterator & iv )
    { return Point3d( iv - i->second.begin(), i->first[1], i->first[2] ); }
    static const T & at
        ( const typename const_LowLevelStorage::const_iterator & iv )
    { return *iv; }

  private:
    carto::VolumeRef<T> _data;
    T _background;
  };


  template <typename T>
  class SparseVolume<BucketMap<T> >
  {
  public:
    typedef T VoxelType;
    typedef typename BucketMap<T>::Bucket LowLevelStorage;
    typedef const typename BucketMap<T>::Bucket const_LowLevelStorage;
    typedef typename BucketMap<T>::iterator iterator;
    typedef typename BucketMap<T>::const_iterator const_iterator;

    SparseVolume( int sizeX = 1, int sizeY = 1, int sizeZ = 1, int sizeT = 1,
                  const carto::AllocatorContext& allocatorContext
                  = carto::AllocatorContext(), bool allocated = true );
    SparseVolume( const carto::rc_ptr<BucketMap<T> >& other );
    SparseVolume( const BucketMap< T >& other );
    virtual ~SparseVolume();
    carto::rc_ptr<BucketMap<T> > data() const { return _data; }
    void reset( BucketMap<T> *x );
    template <typename U>
    static SparseVolume<BucketMap<T> > alloc(
        const SparseVolume<BucketMap<U> > & other );
    const T & background() const { return _background; }
    void setBackground( const T & x ) { _background = x; }
    std::vector<int> getSize() const;
    const carto::PropertySet& header() const
    { return _data->header().getValue(); }
    carto::PropertySet& header()
    { return _data->header().getValue(); }
    std::vector<float> voxelSize() const;

    void fill( const T& value )
    { _data->clear(); setBackground( value ); }
    const T & at( int x, int y, int z, int t ) const;
    void setValue( const T & value,  int x, int y, int z, int t );
    const T & at( int x, int y = 0, int z = 0 ) const;
    void setValue( const T & value,  int x, int y = 0, int z = 0 );
    const T & at( const Point3d & p ) const;
    void setValue( const T & value,  const Point3d & p );
    const T & checkedAt( const Point3d & p ) const
    { return at( p ); }
    const T & checkedAt( int x, int y = 0, int z = 0 ) const
    { return at( x, y, z ); }
    const T & checkedAt( int x, int y, int z, int t ) const
    { return at( x, y, z, t ); }
    iterator begin() { return data()->begin(); }
    const_iterator begin() const { return data()->begin(); }
    iterator end() { return data()->end(); }
    const_iterator end() const { return data()->end(); }
    static const Point3d & position3d
        ( const const_iterator &,
          const typename const_LowLevelStorage::const_iterator & iv )
    { return iv->first; }
    static const T & at
        ( const typename const_LowLevelStorage::const_iterator & iv )
    { return iv->second; }

  private:
    carto::rc_ptr<BucketMap<T> > _data;
    mutable typename BucketMap<T>::Bucket *_firstbck;
    T _background;
  };


  template <>
  class SparseVolume<BucketMap<Void> >
  {
  public:
    typedef int VoxelType;
    typedef BucketMap<Void>::Bucket LowLevelStorage;
    typedef const BucketMap<Void>::Bucket const_LowLevelStorage;
    typedef BucketMap<Void>::iterator iterator;
    typedef BucketMap<Void>::const_iterator const_iterator;

    SparseVolume( int sizeX = 1, int sizeY = 1, int sizeZ = 1, int sizeT = 1,
                  const carto::AllocatorContext& allocatorContext
                  = carto::AllocatorContext(), bool allocated = true );
    SparseVolume( const carto::rc_ptr<BucketMap<Void> > & other );
    SparseVolume( const BucketMap<Void> & other );
    virtual ~SparseVolume();
    carto::rc_ptr<BucketMap<Void> > data() const { return _data; }
    void reset( BucketMap<Void> *x );
    template <typename U>
    static SparseVolume<BucketMap<Void> > alloc(
        const SparseVolume<BucketMap<U> > & other );
    const VoxelType & background() const { return _background; }
    void setBackground( const VoxelType & x ) { _background = x; }
    std::vector<int> getSize() const;
    const carto::PropertySet& header() const
    { return _data->header().getValue(); }
    carto::PropertySet& header()
    { return _data->header().getValue(); }
    std::vector<float> voxelSize() const;

    void fill( const VoxelType & value )
    { _data->clear(); setBackground( value ); }
    const VoxelType & at( int x, int y = 0, int z = 0, int t = 0 ) const;
    void setValue( const VoxelType & value,  int x, int y = 0, int z = 0,
                   int t = 0 );
    const VoxelType & at( const Point3d & p ) const;
    void setValue( const VoxelType & value,  const Point3d & p );
    const VoxelType & checkedAt( const Point3d & p ) const
    { return at( p ); }
    const VoxelType & checkedAt( int x, int y = 0, int z = 0 ) const
    { return at( x, y, z ); }
    const VoxelType & checkedAt( int x, int y, int z, int t ) const
    { return at( x, y, z, t ); }
    iterator begin() { return data()->begin(); }
    const_iterator begin() const { return data()->begin(); }
    iterator end() { return data()->end(); }
    const_iterator end() const { return data()->end(); }
    static const Point3d & position3d
        ( const const_iterator &,
          const const_LowLevelStorage::const_iterator & iv )
    { return iv->first; }
    static const Void & at( const const_LowLevelStorage::const_iterator
        & iv )
    { return iv->second; }

  private:
    carto::rc_ptr<BucketMap<Void> > _data;
    VoxelType _background;
  };


  // ---

  template <typename T>
  inline
  SparseVolume<carto::Volume<T> >::SparseVolume( int sizeX, int sizeY,
                                                 int sizeZ, int sizeT,
                                                 const carto::AllocatorContext&
                                                 allocatorContext,
                                                 bool allocated )
    : _data( sizeX, sizeY, sizeZ, sizeT, allocatorContext, allocated ),
      _background( 0 )
  {
    if( allocated )
      _data->fill( _background );
  }


  template <typename T>
  inline
  SparseVolume<carto::Volume<T> >::SparseVolume( int sizeX, int sizeY,
                                                 int sizeZ, int sizeT,
                                                 T* buffer )
    : _data( new carto::Volume<T>( sizeX, sizeY, sizeZ, sizeT, buffer ) ),
      _background( 0 )
  {
  }


  template <typename T>
  inline
  SparseVolume<carto::Volume<T> >::SparseVolume( const carto::VolumeRef< T >&
                                                 other )
    : _data( other ),
      _background( 0 )
  {
  }


  template <typename T>
  inline
  SparseVolume<carto::Volume<T> >::SparseVolume( const carto::Volume< T >& 
                                                 other )
    : _data( new carto::Volume<T>( other ) ),
      _background( 0 )
  {
  }


  template <typename T>
  inline
  SparseVolume<carto::Volume<T> >::SparseVolume
    ( const carto::rc_ptr<carto::Volume<T> > & x )
    : _data( x ),
      _background( 0 )
  {
  }


  template <typename T>
  inline
  SparseVolume<carto::Volume<T> >::~SparseVolume()
  {
  }


  template <typename T>
  inline
  std::vector<int> SparseVolume<carto::Volume<T> >::getSize() const
  {
    std::vector<int> sz( 4 );
    sz[0] = _data->getSizeX();
    sz[1] = _data->getSizeY();
    sz[2] = _data->getSizeZ();
    sz[3] = _data->getSizeT();

    return sz;
  }


  template <typename T>
  inline
  std::vector<float> SparseVolume<carto::Volume<T> >::voxelSize() const
  {
    std::vector<float> vs( 4 );
    int i = 0;
    try
    {
      carto::Object j, v = header().getProperty( "voxel_size" );
      for( j=v->objectIterator(); j->isValid(); j->next(), ++i )
        vs[i] = j->currentValue()->getScalar();
    }
    catch( ... )
    {
    }
    for( ; i<4; ++i )
      vs[i] = 1.;
    return vs;
  }


  template <typename T>
  inline const T &
  SparseVolume<carto::Volume<T> >::checkedAt( int x, int y, int z,
                                              int t ) const
  {
    if( x < 0 || y < 0 || z < 0 || t < 0 || x >= _data->getSizeX()
        || y >= _data->getSizeY() || z >= _data->getSizeZ()
        || t >= _data->getSizeT() )
      return _background;
    return _data->at( x, y, z, t );
  }


  template <typename T>
  inline void
  SparseVolume<carto::Volume<T> >::reset( carto::Volume<T> *d )
  {
    _data.reset( d );
  }


  template <typename T> template <typename U>
  inline SparseVolume<carto::Volume<T> >
  SparseVolume<carto::Volume<T> >::alloc
      ( const SparseVolume<carto::Volume<U> > & other )
  {
    carto::Volume<U> & src = *other.data();
    const carto::AllocatorContext *ac = &src.allocatorContext();
    // look for the upper-level allocator, because views have an "Unallocated"
    // allocator
    if( src.refVolume().get() )
    {
      carto::Volume<U> *rvol = &src;
      while( rvol->refVolume().get() )
      {
        rvol = rvol->refVolume().get();
        if( rvol->allocatorContext().isAllocated() )
          ac = &rvol->allocatorContext();
        else
          break;
      }
    }
    std::unique_ptr<carto::AllocatorContext> pac;
    if( ac->allocatorType() == carto::AllocatorStrategy::Unallocated )
    {
      // if src is unallocated (does not own its data), we still need to
      // allocate a volume.
      pac.reset( new carto::AllocatorContext );
      ac = pac.get();
    }
    carto::Volume<T> *data = new carto::Volume<T>( src.getSizeX(),
        src.getSizeY(), src.getSizeZ(), src.getSizeT(), *ac );
    data->copyHeaderFrom( src.header() );
    data->fill( other.background() );
    SparseVolume<carto::Volume<T> > svol( data );
    svol.setBackground( other.background() );
    return svol;
  }


  // ------

  template <typename T>
  inline
  SparseVolume<BucketMap<T> >::SparseVolume( int, int, int, int,
                                             const carto::AllocatorContext&,
                                             bool )
    : _data( new BucketMap<T> ),
      _background( 0 )
  {
    typename BucketMap<T>::iterator i = _data->find( 0 );
    if( i != _data->end() )
      _firstbck = &i->second;
    else
      _firstbck = 0;
  }


  template <typename T>
  inline
  SparseVolume<BucketMap<T> >::SparseVolume( const carto::rc_ptr<BucketMap<T> >
                                             & other )
    : _data( other ),
      _background( 0 )
  {
    typename BucketMap<T>::iterator i = _data->find( 0 );
    if( i != _data->end() )
      _firstbck = &i->second;
    else
      _firstbck = 0;
  }


  template <typename T>
  inline
  SparseVolume<BucketMap<T> >::SparseVolume( const BucketMap< T >& other )
    : _data( new BucketMap<T>( other ) ),
      _background( 0 )
  {
    typename BucketMap<T>::iterator i = _data->find( 0 );
    if( i != _data->end() )
      _firstbck = &i->second;
    else
      _firstbck = 0;
  }


  template <typename T>
  inline
  SparseVolume<BucketMap<T> >::~SparseVolume()
  {
  }


  template <typename T>
  inline std::vector<int>
  SparseVolume<BucketMap<T> >::getSize() const
  {
    std::vector<int> sz(4);
    if( _data->empty() )
    {
      sz[0] = 0;
      sz[1] = 0;
      sz[2] = 0;
      sz[3] = 0;
      return sz;
    }
    sz[3] = _data->rbegin()->first + 1;
    int & x = sz[0];
    int & y = sz[1];
    int & z = sz[2];
    typename BucketMap<T>::const_iterator ib, eb = _data->end();
    typename BucketMap<T>::Bucket::const_iterator i, e;
    for( ib=_data->begin(); ib!=eb; ++ib )
      for( i=ib->second.begin(), e=ib->second.end(); i!=e; ++i )
      {
        const Point3d & p = i->first;
        if( p[0] > x )
          x = p[0];
        if( p[1] > y )
          y = p[1];
        if( p[2] > z )
          z = p[2];
      }
    ++x;
    ++y;
    ++z;

    return sz;
  }


  template <typename T>
  inline
  std::vector<float> SparseVolume<BucketMap<T> >::voxelSize() const
  {
    std::vector<float> vs( 4 );
    vs[0] = _data->sizeX();
    vs[1] = _data->sizeY();
    vs[2] = _data->sizeZ();
    vs[3] = _data->sizeT();
    return vs;
  }


  template <typename T>
  inline const T &
  SparseVolume<BucketMap<T> >::at( int x, int y, int z, int t ) const
  {
    typename BucketMap<T>::const_iterator ib = _data->find( t );
    if( ib == _data->end() )
      return _background;
    typename BucketMap<T>::Bucket::const_iterator i
      = ib->second.find( Point3d( x, y, z ) );
    if( i != ib->second.end() )
      return i->second;
    return _background;
  }


  template <typename T>
  inline const T &
  SparseVolume<BucketMap<T> >::at( const Point3d & p ) const
  {
    if( !_firstbck )
      return _background;
    typename BucketMap<T>::Bucket::const_iterator i
      = _firstbck->find( p );
    if( i != _firstbck->end() )
      return i->second;
    return _background;
  }


  template <typename T>
  inline const T &
  SparseVolume<BucketMap<T> >::at( int x, int y, int z ) const
  {
    return at( Point3d( x, y, z ) );
  }


  template <typename T>
  inline void
  SparseVolume<BucketMap<T> >::setValue( const T & value, int x, int y, int z,
                                         int t )
  {
    (*_data)[t][ Point3d( x, y, z ) ] = value;
  }


  template <typename T>
  inline void
  SparseVolume<BucketMap<T> >::setValue( const T & value, const Point3d & p )
  {
    if( !_firstbck )
      _firstbck = &(*_data)[0];
    (*_firstbck)[ p ] = value;
  }


  template <typename T>
  inline void
  SparseVolume<BucketMap<T> >::setValue( const T & value, int x, int y, int z )
  {
    setValue( value, Point3d( x, y, z ) );
  }


  template <typename T>
  inline void
  SparseVolume<BucketMap<T> >::reset( BucketMap<T>* d )
  {
    _data.reset( d );
    typename BucketMap<T>::iterator i = _data->find( 0 );
    if( i != _data->end() )
      _firstbck = &i->second;
    else
      _firstbck = 0;
  }


  template <typename T> template <typename U>
  inline SparseVolume<BucketMap<T> >
  SparseVolume<BucketMap<T> >::alloc
      ( const SparseVolume<BucketMap<U> > & other )
  {
    BucketMap<U> & src = *other.data();
    BucketMap<T> *data = new BucketMap<T>;
    data->setSizeXYZT( src.sizeX(), src.sizeY(), src.sizeZ(), src.sizeT() );
    data->header().getValue() = src.header().getValue();
    return SparseVolume<BucketMap<T> >( carto::rc_ptr<BucketMap<T> >( data ) );
  }

  // --------


  inline
  SparseVolume<BucketMap<Void> >::SparseVolume
    ( int, int, int, int, const carto::AllocatorContext&, bool )
    : _data( new BucketMap<Void> ),
      _background( 0 )
  {
  }


  inline
  SparseVolume<BucketMap<Void> >::SparseVolume( const
    carto::rc_ptr<BucketMap<Void> > & other )
    : _data( other ),
      _background( 0 )
  {
  }


  inline
  SparseVolume<BucketMap<Void> >::SparseVolume( const BucketMap<Void> & other )
    : _data( new BucketMap<Void>( other ) ),
      _background( 0 )
  {
  }


  inline
  SparseVolume<BucketMap<Void> >::~SparseVolume()
  {
  }


  inline std::vector<int>
  SparseVolume<BucketMap<Void> >::getSize() const
  {
    std::vector<int> sz(4);
    sz[3] = _data->rbegin()->first + 1;
    int & x = sz[0];
    int & y = sz[1];
    int & z = sz[2];
    BucketMap<Void>::const_iterator ib, eb = _data->end();
    BucketMap<Void>::Bucket::const_iterator i, e;
    for( ib=_data->begin(); ib!=eb; ++ib )
      for( i=ib->second.begin(), e=ib->second.end(); i!=e; ++i )
      {
        const Point3d & p = i->first;
        if( p[0] > x )
          x = p[0];
        if( p[1] > y )
          y = p[1];
        if( p[2] > z )
          z = p[2];
      }
    ++x;
    ++y;
    ++z;

    return sz;
  }


  inline
  std::vector<float> SparseVolume<BucketMap<Void> >::voxelSize() const
  {
    std::vector<float> vs( 4 );
    vs[0] = _data->sizeX();
    vs[1] = _data->sizeY();
    vs[2] = _data->sizeZ();
    vs[3] = _data->sizeT();
    return vs;
  }


  inline const int &
  SparseVolume<BucketMap<Void> >::at( int x, int y, int z, int ) const
  {
    return at( Point3d( x, y, z ) );
  }


  inline const int &
  SparseVolume<BucketMap<Void> >::at( const Point3d & p ) const
  {
    BucketMap<Void>::const_iterator ib, eb = _data->end();
    for( ib=_data->begin(); ib!=eb; ++ib )
    {
      BucketMap<Void>::Bucket::const_iterator i
        = ib->second.find( p );
      if( i != ib->second.end() )
        return ib->first;
    }
    return _background;
  }


  inline void
  SparseVolume<BucketMap<Void> >::setValue( const int & value, int x, int y,
                                            int z, int )
  {
    setValue( value, Point3d( x, y, z ) );
  }


  inline void
  SparseVolume<BucketMap<Void> >::setValue( const int & value,
                                            const Point3d & p )
  {
    // erase first
    BucketMap<Void>::iterator ib, eb = _data->end();
    for( ib=_data->begin(); ib!=eb; ++ib )
      ib->second.erase( p );
    if( value != background() )
      (*_data)[value][p] = Void();
  }


  inline void
  SparseVolume<BucketMap<Void> >::reset( BucketMap<Void>* d )
  {
    _data.reset( d );
  }


  template <typename U>
  inline SparseVolume<BucketMap<Void> >
  SparseVolume<BucketMap<Void> >::alloc
      ( const SparseVolume<BucketMap<U> > & other )
  {
    BucketMap<U> & src = *other.data();
    BucketMap<Void> *data = new BucketMap<Void>;
    data->setSizeXYZT( src.sizeX(), src.sizeY(), src.sizeZ(), src.sizeT() );
    data->header().getValue() = src.header().getValue();
    return SparseVolume<BucketMap<Void> >
        ( carto::rc_ptr<BucketMap<Void> >( data ) );
  }

}

#endif



