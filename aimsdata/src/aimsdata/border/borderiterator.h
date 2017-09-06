#ifndef AIMSDATA_BORDER_BORDERITERATOR_H
#define AIMSDATA_BORDER_BORDERITERATOR_H


#include <aims/vector/vector.h>                                      // Point*
#include <cartodata/volume/volume.h>                              // VolumeRef
#include <iostream>

namespace aims
{
  //==========================================================================
  // BORDER ITERATOR
  //==========================================================================
  /// Represents the border of a volume. This border can be outside (if
  /// volume with borders are used and enough memory is allocated) or inside.
  /// It is defined by a Volume Ref, a border size and its direction (inside
  /// or outside)
  ///
  /// Iterators (const and non const) are defined for this class. They allow
  /// to easily run through all border voxels. Those iterators implement the
  /// classic iterator operators (assignment, increment, ...). They also
  /// have a coordinates() method that returns the position of the current
  /// voxel in the volume.
  template <typename T>
  class BorderIterator
  {
  public:
    /// Border constructor
    ///
    /// \param in  Input volume whise border voxels to iterate
    /// \param inside  If false consider "outside" border (with coordinates
    ///                outside the volume dimensions).
    ///                If true consider "inside" border
    /// \param size  border size in each dimension. If \c inside is true,
    ///              size must be >= 0 in each dim. If \c inside is false and
    ///              a size dim has negative value, the maximum possible
    ///              border in this dimension is computed.
    ///
    /// \code
    /// // 1 voxel inside border in a 3D volume:
    /// Border b1( in, true, Point4dl(1,1,1,0) );
    ///
    /// // 2 voxels outside border in a 2D volume
    /// Border b2( in, false, Point4dl(2,2,0,0) );
    ///
    /// // All existing border in a volume
    /// Border b3( in );
    /// \endcode
    BorderIterator( carto::VolumeRef<T> in, bool inside = false,
                    const Point4dl & size = Point4dl( -1, -1, -1, -1 ) ):
      _volume(in),
      _inside(inside),
      _sizeinf(size),
      _sizesup(size)
    {
      if( !inside )
      {
        std::vector<int> borders = _volume.getBorders();
        if( size[0] < 0 )
        {
          _sizeinf[0] = borders[0];
          _sizesup[0] = borders[1];
        }
        if( size[1] < 0 )
        {
          _sizeinf[1] = borders[2];
          _sizesup[1] = borders[3];
        }
        if( size[2] < 0 )
        {
          _sizeinf[2] = borders[4];
          _sizesup[2] = borders[5];
        }
        if( size[3] < 0 )
        {
          _sizeinf[3] = borders[6];
          _sizesup[3] = borders[7];
        }
      }
    }
    BorderIterator( carto::VolumeRef<T> in, bool inside,
                    const Point4dl & sizeinf,
                    const Point4dl & sizesup ):
      _volume(in),
      _inside(inside),
      _sizeinf(sizeinf),
      _sizesup(sizesup)
    {
      if( !inside )
      {
        std::vector<int> borders = _volume.getBorders();
        if( _sizeinf[0] < 0 )
          _sizeinf[0] = borders[0];
        if( _sizesup[0] < 0 )
          _sizesup[0] = borders[1];

        if( _sizeinf[1] < 0 )
          _sizeinf[1] = borders[2];
        if( _sizesup[1] < 0 )
          _sizesup[1] = borders[3];

        if( _sizeinf[2] < 0 )
          _sizeinf[2] = borders[4];
        if( _sizesup[1] < 0 )
          _sizesup[2] = borders[5];

        if( _sizeinf[3] < 0 )
          _sizeinf[3] = borders[6];
        if( _sizesup[1] < 0 )
          _sizesup[3] = borders[7];
      }
    }

    BorderIterator():
      _volume( (carto::Volume<T>*)0 ),
      _inside(false),
      _sizeinf(),
      _sizesup()
    {}

  public:
    class iterator;

    /// Border: const iterator
    class const_iterator
    {
      //--- constructors -----------------------------------------------------
    public:
      /// Default constructor
      /// Create a non initialized iterator than cannot be incremented or
      /// accessed.
      const_iterator();
      /// Copy constructor from a const_iterator
      const_iterator( const const_iterator & other );
      /// Copy constructor from an iterator
      const_iterator( const iterator & other );
    protected:
      /// Utility constructor: returns the iterator at position begin() if \c
      /// end is false, and end() if \c end is false.
      const_iterator( BorderIterator<T> & b, bool end = false );
      /// Utility constructor: returns the iterator at position \c current
      const_iterator( BorderIterator<T> & b, const Point4dl & current );

      //--- operators --------------------------------------------------------
    public:
      /// assignment from const_iterator
      const_iterator & operator=( const const_iterator & other );
      /// assignment from iterator
      const_iterator & operator=( const iterator & other );
      /// increment (prefix)
      const_iterator & operator++();
      /// increment (suffix)
      const_iterator   operator++(int);
      const T & operator*() const;
      const T * operator->() const;
      bool operator==( const const_iterator & other ) const;
      bool operator==( const iterator & other ) const;
      bool operator!=( const const_iterator & other ) const;
      bool operator!=( const iterator & other ) const;

      //--- member method ----------------------------------------------------
      /// Returns the coordinates of the current point in the linked volume
      Point4dl coordinate() const { return _current; }
      /// Returns a reference to the linked volume
      carto::VolumeRef<T> volume() { return _border._volume; }

      //--- members ----------------------------------------------------------
    protected:
      BorderIterator<T>     _border;
      Point4dl              _current;

      //--- helpers ----------------------------------------------------------
    protected:
      /// returns true if the \c p is in the border described by _border
      bool isBorder( const Point4dl & p ) const;
      /// returns true if \c p is in the image but not in the border.
      bool isNonBorderImage( const Point4dl & p ) const;
      /// Sets the coordinate (in the dimension described by \c move) at its
      /// most inferior (i.e. left) value.
      Point4dl reset( const Point4dl & p,
                      const Point4dl & move ) const;
      /// Computes the next move. If \c move was in the X dimension (\c move
      /// == (1,0,0,0)) the next move is in the Y dimension, and so on. If
      /// \c move was in the T dimension, nextMove() returns (0,0,0,0).
      Point4dl nextMove( const Point4dl & move ) const;
      /// computeNext(p) returns the next point in the border.
      /// \param p     previous position
      /// \param move  direction in which to seek the next point.
      /// The method is always called with \c move 's default value from the
      /// outside, but calls iteself recursively with other values.
      Point4dl computeNext( const Point4dl & p ) const;
      /// Point value for the "end" iterator (which points to no voxel).
      /// In this implemention, its value is
      /// (MAX_LONG,MAX_LONG,MAX_LONG,MAX_LONG)
      Point4dl end() const;
      /// Gives the size of the non-border volume in the X dimension.
      /// It is used to jump the non border volume when iterating on voxels.
      long jumpX() const;

      //--- friends ----------------------------------------------------------
      friend class BorderIterator<T>;
      friend class iterator;
    };

    /// Border: iterator
    class iterator: public const_iterator
    {
      //--- constructors -----------------------------------------------------
    public:
      /// Default constructor
      /// Create a non initialized iterator than cannot be incremented or
      /// accessed.
      iterator();
      /// Copy constructor from an iterator
      iterator( const iterator & other );
    protected:
      /// Utility constructor: returns the iterator at position begin() if \c
      /// end is false, and end() if \c end is false.
      iterator( BorderIterator<T> & b, bool end = false );
      /// Utility constructor: returns the iterator at position \c current
      iterator( BorderIterator<T> & b, const Point4dl & current );

      //--- operators --------------------------------------------------------
    public:
      /// assignment from iterator
      iterator & operator=( const iterator & other );
      /// increment (prefix)
      iterator & operator++();
      iterator   operator++(int);
      T & operator*();
      T * operator->();

      //--- friends ----------------------------------------------------------
      friend class BorderIterator<T>;
    };

  public:
    iterator        begin()       { return iterator(*this); }
    iterator        end()         { return iterator(*this,true); }
    const_iterator  begin() const { return const_iterator(*this); }
    const_iterator  end()   const { return const_iterator(*this,true); }

    bool operator==( const BorderIterator<T> & other ) const
    {
      return  ( _inside == other._inside ) &&
              ( _sizeinf == other._sizeinf ) &&
              ( _sizesup == other._sizesup ) &&
              ( _volume.get()== other._volume.get() );
    }

  protected:
    carto::VolumeRef<T>   _volume;
    bool                  _inside;
    Point4dl              _sizeinf;
    Point4dl              _sizesup;

    friend class const_iterator;
    friend class iterator;
  };

  //--- const_iterator -------------------------------------------------------
  template <typename T>
  BorderIterator<T>::const_iterator::const_iterator():
    _border(),
    _current()
  {}

  template <typename T>
  BorderIterator<T>::const_iterator::const_iterator( BorderIterator<T> & b, bool end ):
    _border(b),
    _current( (end ? this->end() : Point4dl(0,0,0,0) ) )
  {
    if( !_border._inside && !end )
    {
      const Point4dl & start = _border._sizeinf;
      _current = Point4dl( -start[0], -start[1], -start[2], -start[3] );
      if( !isBorder(_current) )
        _current = computeNext( _current );
    }
  }

  template <typename T>
  BorderIterator<T>::const_iterator::const_iterator( BorderIterator<T> & b,
                                             const Point4dl & current ):
    _border(b),
    _current(current)
  {}

  template <typename T>
  BorderIterator<T>::const_iterator::const_iterator(
    const typename BorderIterator<T>::const_iterator & other ):
    _border(other._border),
    _current(other._current)
  {}

  template <typename T>
  BorderIterator<T>::const_iterator::const_iterator(
    const typename BorderIterator<T>::iterator & other ):
    _border(other._border),
    _current(other._current)
  {}

  template <typename T>
  typename BorderIterator<T>::const_iterator & BorderIterator<T>::const_iterator::operator=(
    const typename BorderIterator<T>::const_iterator & other )
  {
    _border = other._border;
    _current = other._current;
    return *this;
  }

  template <typename T>
  typename BorderIterator<T>::const_iterator & BorderIterator<T>::const_iterator::operator=(
    const typename BorderIterator<T>::iterator & other )
  {
    _border = other._border;
    _current = other._current;
    return *this;
  }

  template <typename T>
  const T & BorderIterator<T>::const_iterator::operator*() const
  {
    return _border._volume( _current[0], _current[1], _current[2], _current[3] );
  }

  template <typename T>
  const T * BorderIterator<T>::const_iterator::operator->() const
  {
    return &( _border._volume( _current[0], _current[1], _current[2], _current[3] ) );
  }

  template <typename T>
  bool BorderIterator<T>::const_iterator::operator==( const const_iterator & other ) const
  {
    return (_border == other._border) && (_current == other._current);
  }

  template <typename T>
  bool BorderIterator<T>::const_iterator::operator==( const iterator & other ) const
  {
    return (_border == other._border) && (_current == other._current);
  }

  template <typename T>
  bool BorderIterator<T>::const_iterator::operator!=( const const_iterator & other ) const
  {
    return !(*this == other);
  }

  template <typename T>
  bool BorderIterator<T>::const_iterator::operator!=( const iterator & other ) const
  {
    return !(*this == other);
  }

  template <typename T>
  Point4dl BorderIterator<T>::const_iterator::end() const
  {
    return Point4dl( std::numeric_limits<long>::max(),
                     std::numeric_limits<long>::max(),
                     std::numeric_limits<long>::max(),
                     std::numeric_limits<long>::max() );
  }

  template <typename T>
  long BorderIterator<T>::const_iterator::jumpX() const
  {
    if( _border._inside )
      return _border._volume.getSizeX() - _border._sizesup[0];
    else
      return _border._volume.getSizeX();
  }

  template <typename T>
  bool BorderIterator<T>::const_iterator::isBorder( const Point4dl & p ) const
  {
    if( _border._inside )
    {
      // outside allocated volume
      if( ( p[0] < 0 ) || ( p[0] >= _border._volume.getSizeX() ) ||
          ( p[1] < 0 ) || ( p[1] >= _border._volume.getSizeY() ) ||
          ( p[2] < 0 ) || ( p[2] >= _border._volume.getSizeZ() ) ||
          ( p[3] < 0 ) || ( p[3] >= _border._volume.getSizeT() ) )
        return false;
      // inside borders
      else if( ( p[0] < _border._sizeinf[0] ) ||
               ( p[0] >= _border._volume.getSizeX() - _border._sizesup[0] + 1 ) ||
               ( p[1] < _border._sizeinf[1] ) ||
               ( p[1] >= _border._volume.getSizeY() - _border._sizesup[1] + 1 ) ||
               ( p[2] < _border._sizeinf[2] ) ||
               ( p[2] >= _border._volume.getSizeZ() - _border._sizesup[2] + 1 ) ||
               ( p[3] < _border._sizeinf[3] ) ||
               ( p[3] >= _border._volume.getSizeT() - _border._sizesup[3] + 1 ) )
        return true;
      // inside non-border volume
      else
        return false;
    }
    else
    {
      // outside allocated volume
      if( ( p[0] < -_border._sizeinf[0] ) ||
          ( p[0] >= _border._volume.getSizeX() + _border._sizesup[0] ) ||
          ( p[1] < -_border._sizeinf[1] ) ||
          ( p[1] >= _border._volume.getSizeY() + _border._sizesup[1] ) ||
          ( p[2] < -_border._sizeinf[2] ) ||
          ( p[2] >= _border._volume.getSizeZ() + _border._sizesup[2] ) ||
          ( p[3] < -_border._sizeinf[3] ) ||
          ( p[3] >= _border._volume.getSizeT() + _border._sizesup[3] ) )
        return false;
      // inside border
      else if( ( p[0] < 0 ) || ( p[0] >= _border._volume.getSizeX() ) ||
               ( p[1] < 0 ) || ( p[1] >= _border._volume.getSizeY() ) ||
               ( p[2] < 0 ) || ( p[2] >= _border._volume.getSizeZ() ) ||
               ( p[3] < 0 ) || ( p[3] >= _border._volume.getSizeT() ) )
        return true;
      // inside non-border volume
      else
        return false;
    }
  }

  template <typename T>
  bool BorderIterator<T>::const_iterator::isNonBorderImage( const Point4dl & p ) const
  {
    if( _border._inside )
    {
      return( _border._sizeinf[0] <= p[0] && p[0] < _border._volume.getSizeX() - _border._sizesup[0] &&
              _border._sizeinf[1] <= p[1] && p[1] < _border._volume.getSizeY() - _border._sizesup[1] &&
              _border._sizeinf[2] <= p[2] && p[2] < _border._volume.getSizeZ() - _border._sizesup[2] &&
              _border._sizeinf[3] <= p[3] && p[3] < _border._volume.getSizeT() - _border._sizesup[3] );
    }
    else
    {
      return( 0 <= p[0] && p[0] < _border._volume.getSizeX() &&
              0 <= p[1] && p[1] < _border._volume.getSizeY() &&
              0 <= p[2] && p[2] < _border._volume.getSizeZ() &&
              0 <= p[3] && p[3] < _border._volume.getSizeT() );
    }
  }

  template <typename T>
  Point4dl BorderIterator<T>::const_iterator::reset(
    const Point4dl & p, const Point4dl & move ) const
  {
    Point4dl newp = p;

    if( _border._inside )
    {
      if( move[0] > 0 )
        newp[0] = 0;
      else if( move[1] > 0 )
        newp[1] = 0;
      else if( move[2] > 0 )
        newp[2] = 0;
      else if( move[3] > 0 )
        newp[3] = 0;
    }
    else
    {
      if( move[0] > 0 )
        newp[0] = -_border._sizeinf[0];
      else if( move[1] > 0 )
        newp[1] = -_border._sizeinf[1];
      else if( move[2] > 0 )
        newp[2] = -_border._sizeinf[2];
      else if( move[3] > 0 )
        newp[3] = -_border._sizeinf[3];
    }

    return newp;
  }

  template <typename T>
  Point4dl BorderIterator<T>::const_iterator::nextMove(
    const Point4dl & move ) const
  {
    if( move[0] > 0 )
      return Point4dl(0,1,0,0);
    else if( move[1] > 0 )
      return Point4dl(0,0,1,0);
    else if( move[2] > 0 )
      return Point4dl(0,0,0,1);
    else if( move[3] > 0 )
      return Point4dl(0,0,0,0);

    // error
    return move;
  }

  template <typename T>
  Point4dl BorderIterator<T>::const_iterator::computeNext(
    const Point4dl & current ) const
  {
    Point4dl move( 1, 0, 0, 0 );
    Point4dl next = current + move;
    while( !isBorder(next) && next != end() )
    {
      // if next is inside: jump inside image
      if( isNonBorderImage( next ) ) {
        next[0] = jumpX();
        move = Point4dl( 1, 0, 0, 0 );
      }
      // else: reset studied dim at line begin, and change dim
      else
      {
        next = reset( next, move );
        move = nextMove( move );
        if( move == Point4dl(0, 0, 0, 0) )
          next = end();
        else
          next = next + move;
      }
    }

    return next;
  }

  template <typename T>
  typename BorderIterator<T>::const_iterator & BorderIterator<T>::const_iterator::operator++()
  {
    _current = computeNext( _current );
    return *this;
  }

  template <typename T>
  typename BorderIterator<T>::const_iterator BorderIterator<T>::const_iterator::operator++(int)
  {
    typename BorderIterator<T>::const_iterator prev = *this;
    _current = computeNext( _current );
    return prev;
  }

  //--- iterator -------------------------------------------------------------
  template <typename T>
  BorderIterator<T>::iterator::iterator():
    const_iterator()
  {}

  template <typename T>
  BorderIterator<T>::iterator::iterator( BorderIterator<T> & b, bool end ):
    const_iterator(b,end)
  {}

  template <typename T>
  BorderIterator<T>::iterator::iterator( BorderIterator<T> & b, const Point4dl & current ):
    const_iterator(b, current)
  {}

  template <typename T>
  BorderIterator<T>::iterator::iterator(
    const typename BorderIterator<T>::iterator & other ):
    const_iterator(other)
  {}

  template <typename T>
  typename BorderIterator<T>::iterator & BorderIterator<T>::iterator::operator=(
    const typename BorderIterator<T>::iterator & other )
  {
    const_iterator::operator=( other );
    return *this;
  }

  template <typename T>
  T & BorderIterator<T>::iterator::operator*()
  {
    return const_iterator::_border._volume(
      const_iterator::_current[0],
      const_iterator::_current[1],
      const_iterator::_current[2],
      const_iterator::_current[3] );
  }

  template <typename T>
  T * BorderIterator<T>::iterator::operator->()
  {
    return &( const_iterator::_border._volume(
      const_iterator::_current[0],
      const_iterator::_current[1],
      const_iterator::_current[2],
      const_iterator::_current[3] ) );
  }

  template <typename T>
  typename BorderIterator<T>::iterator & BorderIterator<T>::iterator::operator++()
  {
    const_iterator::_current = const_iterator::computeNext(
      const_iterator::_current );
    return *this;
  }

  template <typename T>
  typename BorderIterator<T>::iterator BorderIterator<T>::iterator::operator++(int)
  {
    typename BorderIterator<T>::iterator prev = *this;
    const_iterator::_current = const_iterator::computeNext(
      const_iterator::_current );
    return prev;
  }

} // namespace aims

#endif // AIMSDATA_BORDER_BORDERITERATOR_H
