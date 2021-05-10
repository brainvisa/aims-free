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
    static void fillConstant( carto::VolumeRef<T> in, const T & value = 0, 
                              bool unreadOnly = false );
    /// Fills the border with a "median" value.
    ///
    /// This is the median value of the inside border of equal size:
    /// if the outside border is of size (2, 2, 0) in dimensions x, y, z,
    /// the inside border is also of size (2, 2, 0)
    ///
    /// This can be overriden by the argument \c size.
    static void fillMedian( carto::VolumeRef<T> in, 
                            Point4dl size = Point4dl(-1,-1,-1,-1), 
                            bool unreadOnly = false );
    /// Each voxel of the border is assigned with the nearest inside voxel.
    static void fillNearest( carto::VolumeRef<T> in, bool unreadOnly = false );
    /// The border is filled by mirroring the inside border of same size.
    static void fillMirror( carto::VolumeRef<T> in, bool unreadOnly = false );
    /// Build a view that exposes data filled at reading time. Borders of this view
    /// correpond to data unfilled at reading time (i.e. borders that are outside of 
    /// the read image domain). This is useful to fill borders of partialy read \c carto::VolumeRef.
    static carto::VolumeRef<T> buildUnfilledBorderView(const carto::VolumeRef<T> & vol);
  };

  template <typename T>
  void BorderFiller<T>::fillConstant( carto::VolumeRef<T> in, const T & value, 
                                      bool unreadOnly )
  {
    carto::VolumeRef<T> vol;
    if (unreadOnly)
        vol = aims::BorderFiller<T>::buildUnfilledBorderView(in);
    else
        vol = in;
    
    BorderIterator<T> brd( vol );
    typename BorderIterator<T>::iterator i;

    for( i = brd.begin(); i != brd.end(); ++i )
      *i = value;
  }

  template <typename T>
  void BorderFiller<T>::fillMedian( carto::VolumeRef<T> in, Point4dl size, 
                                    bool unreadOnly )
  {
    carto::VolumeRef<T> vol;
    if (unreadOnly)
        vol = aims::BorderFiller<T>::buildUnfilledBorderView(in);
    else
        vol = in;
    
    std::vector<int> bs = vol.getBorders();
    if( size[0] < 0 )
      size[0] = ( bs[0] >= bs[1] ? bs[0] : bs[1] );
    if( size[1] < 0 )
      size[1] = ( bs[2] >= bs[3] ? bs[2] : bs[3] );
    if( size[2] < 0 )
      size[2] = ( bs[4] >= bs[5] ? bs[4] : bs[5] );
    if( size[3] < 0 )
      size[3] = ( bs[6] >= bs[7] ? bs[6] : bs[7] );
    BorderIterator<T> brdin( vol, true, size );
    T med = aims::MathUtil<T>::median( brdin.begin(), brdin.end() );

    BorderIterator<T> brdout( vol );
    typename BorderIterator<T>::iterator i;
    for( i = brdout.begin(); i != brdout.end(); ++i )
      *i = med;
  }

  template <typename T>
  void BorderFiller<T>::fillNearest( carto::VolumeRef<T> in, 
                                     bool unreadOnly )
  {
    carto::VolumeRef<T> vol;
    if (unreadOnly)
        vol = aims::BorderFiller<T>::buildUnfilledBorderView(in);
    else
        vol = in;
    
    BorderIterator<T> brd( vol );
    typename BorderIterator<T>::iterator i;
    Point4dl current;
    for( i = brd.begin(); i != brd.end(); ++i )
    {
      current = i.coordinate();
      if( current[0] < 0 )
        current[0] = 0;
      else if( current[0] >= vol.getSizeX() )
        current[0] = vol.getSizeX() - 1;
      if( current[1] < 0 )
        current[1] = 0;
      else if( current[1] >= vol.getSizeX() )
        current[1] = vol.getSizeX() - 1;
      if( current[2] < 0 )
        current[2] = 0;
      else if( current[2] >= vol.getSizeX() )
        current[2] = vol.getSizeX() - 1;
      if( current[3] < 0 )
        current[3] = 0;
      else if( current[3] >= vol.getSizeX() )
        current[3] = vol.getSizeX() - 1;
      *i = vol( current[0], current[1], current[2], current[3] );
    }
  }


  template <typename T>
  void BorderFiller<T>::fillMirror( carto::VolumeRef<T> in,
                                    bool unreadOnly )
  {
    carto::VolumeRef<T> vol;
    if (unreadOnly)
        vol = buildUnfilledBorderView(in);
    else
        vol = in;
        
    BorderIterator<T> brd( vol );
    typename BorderIterator<T>::iterator i;
    Point4dl current;
    for( i = brd.begin(); i != brd.end(); ++i )
    {
      current = i.coordinate();
      current[0] = mirrorCoeff( current[0], vol.getSizeX() );
      current[1] = mirrorCoeff( current[1], vol.getSizeY() );
      current[2] = mirrorCoeff( current[2], vol.getSizeZ() );
      current[3] = mirrorCoeff( current[3], vol.getSizeT() );
      *i = vol( current[0], current[1], current[2], current[3] );
    }
  }
  
  template<typename T>
  carto::VolumeRef<T> BorderFiller<T>::buildUnfilledBorderView(
      const carto::VolumeRef<T> & vol)
  {
    if (!vol.isNull()) {
        const carto::VolumeRef<T> parent1 = vol.refVolume();
        if (!parent1.isNull()
            && parent1.allocatorContext().isAllocated()) {
            const carto::VolumeRef<T> parent2 = parent1.refVolume();
            if (!parent2.isNull() 
                && !parent2.allocatorContext().isAllocated()) {
                typedef typename carto::Volume<T>::Position Position;
                Position pos = vol.posInRefVolume().toVector();
                Position pos1 = parent1.posInRefVolume().toVector();
                Position size = vol.getSize();
                Position size1 = parent1.getSize();
                Position size2 = parent2.getSize();
            
                size_t dims = pos.size();
            
                for(size_t d=0; d<dims; ++d) {
                    pos[d] =std::max(pos1[d], 0) - pos1[d];
                    size[d] = std::min(size1[d], size2[d] - pos1[d]) - pos[d];
                }
                
                return carto::VolumeRef<T>(vol.refVolume(), pos, size);
            }
        }
    }
    
    return vol;
}

} // namespace aims

#endif // AIMSDATA_BORDER_BORDERFILLER_H
