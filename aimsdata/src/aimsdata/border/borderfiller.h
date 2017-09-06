#ifndef AIMSDATA_BORDER_BORDERFILLER_H
#define AIMSDATA_BORDER_BORDERFILLER_H

//--- aims -------------------------------------------------------------------
#include <aims/border/borderiterator.h>
#include <aims/math/mathelem.h>                                    // MathUtil
#include <aims/resampling/splineresampler.h>                    // mirrorCoeff
//--- cartodata --------------------------------------------------------------
#include <cartodata/volume/volume.h>                              // VolumeRef
//----------------------------------------------------------------------------

namespace aims
{

  //==========================================================================
  // BORDER FILLER
  //==========================================================================
  /// This class contains static methods to fill the border of a VolumeRef
  /// with chosen values. This is useful for processing algorithms that work
  /// in a neighborhood (filtering...)
  template <typename T>
  class BorderFiller
  {
  public:
    /// Fills the border with a constant value
    static void fillConstant( carto::VolumeRef<T> in, const T & value = 0 );
    /// Fills the border with a "median" value.
    ///
    /// This is the median value of the inside border of equal size:
    /// if the outside border is of size (2, 2, 0) in dimensions x, y, z,
    /// the inside border is also of size (2, 2, 0)
    ///
    /// This can be overriden by the argument \c size.
    static void fillMedian( carto::VolumeRef<T> in, Point4dl size = Point4dl(-1,-1,-1,-1) );
    /// Each voxel of the border is assigned with the nearest inside voxel.
    static void fillNearest( carto::VolumeRef<T> in );
    /// The border is filled by mirroring the inside border of same size.
    static void fillMirror( carto::VolumeRef<T> in );
  };

  template <typename T>
  void BorderFiller<T>::fillConstant( carto::VolumeRef<T> in, const T & value )
  {
    BorderIterator<T> brd( in );
    typename BorderIterator<T>::iterator i;

    for( i = brd.begin(); i != brd.end(); ++i )
      *i = value;
  }

  template <typename T>
  void BorderFiller<T>::fillMedian( carto::VolumeRef<T> in, Point4dl size )
  {
    std::vector<int> bs = in.getBorders();
    if( size[0] < 0 )
      size[0] = ( bs[0] >= bs[1] ? bs[0] : bs[1] );
    if( size[1] < 0 )
      size[1] = ( bs[2] >= bs[3] ? bs[2] : bs[3] );
    if( size[2] < 0 )
      size[2] = ( bs[4] >= bs[5] ? bs[4] : bs[5] );
    if( size[3] < 0 )
      size[3] = ( bs[6] >= bs[7] ? bs[6] : bs[7] );
    BorderIterator<T> brdin( in, true, size );
    T med = aims::MathUtil<T>::median( brdin.begin(), brdin.end() );

    BorderIterator<T> brdout( in );
    typename BorderIterator<T>::iterator i;
    for( i = brdout.begin(); i != brdout.end(); ++i )
      *i = med;
  }

  template <typename T>
  void BorderFiller<T>::fillNearest( carto::VolumeRef<T> in )
  {
    BorderIterator<T> brd( in );
    typename BorderIterator<T>::iterator i;
    Point4dl current;
    for( i = brd.begin(); i != brd.end(); ++i )
    {
      current = i.coordinate();
      if( current[0] < 0 )
        current[0] = 0;
      else if( current[0] >= in.getSizeX() )
        current[0] = in.getSizeX() - 1;
      if( current[1] < 0 )
        current[1] = 0;
      else if( current[1] >= in.getSizeX() )
        current[1] = in.getSizeX() - 1;
      if( current[2] < 0 )
        current[2] = 0;
      else if( current[2] >= in.getSizeX() )
        current[2] = in.getSizeX() - 1;
      if( current[3] < 0 )
        current[3] = 0;
      else if( current[3] >= in.getSizeX() )
        current[3] = in.getSizeX() - 1;
      *i = in( current[0], current[1], current[2], current[3] );
    }
  }


  template <typename T>
  void BorderFiller<T>::fillMirror( carto::VolumeRef<T> in )
  {
    BorderIterator<T> brd( in );
    typename BorderIterator<T>::iterator i;
    Point4dl current;
    for( i = brd.begin(); i != brd.end(); ++i )
    {
      current = i.coordinate();
      current[0] = mirrorCoeff( current[0], in.getSizeX() );
      current[1] = mirrorCoeff( current[1], in.getSizeY() );
      current[2] = mirrorCoeff( current[2], in.getSizeZ() );
      current[3] = mirrorCoeff( current[3], in.getSizeT() );
      *i = in( current[0], current[1], current[2], current[3] );
    }
  }

} // namespace aims

#endif // AIMSDATA_BORDER_BORDERFILLER_H
