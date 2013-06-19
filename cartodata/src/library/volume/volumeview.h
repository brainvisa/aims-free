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

#ifndef CARTODATA_VOLUME_VOLUMEVIEW_H
#define CARTODATA_VOLUME_VOLUMEVIEW_H

#include <cartodata/volume/volume.h>
#include <soma-io/image/voxelrgb_d.h>
#include <soma-io/image/voxelrgba_d.h>
#include <soma-io/image/voxelhsv.h>
#ifdef USE_SOMA_IO
  #include <soma-io/datasource/bufferdatasource.h>
#else
  #include <cartobase/datasource/bufferdatasource.h>
#endif

namespace carto
{

  /* we'll see this later
  template<typename T> class Reader;
  template<typename T> class Writer;
  */


  template <typename T>
  class VolumeView : public Volume<T>
  {
  public:
    /// 4D int position
    class Position4Di
    {
    public:
      Position4Di( int x = 0, int y = 0, int z = 0, int t = 0 ) 
        : _coords( 4 )
      {
        _coords[0] = x;
        _coords[1] = y;
        _coords[2] = z;
        _coords[3] = t;
      }
      ~Position4Di() {}
      int & operator [] ( int coord ) { return _coords[ coord ]; }
      const int & operator [] ( int coord ) const { return _coords[ coord ]; }

    private:
      std::vector<int>	_coords;
    };

    /// volume view mode (view in allocated, external data)
    VolumeView( rc_ptr<Volume<T> > other, 
                const Position4Di & pos = Position4Di( 0, 0, 0, 0 ), 
                const Position4Di & size = Position4Di( -1, -1, -1, -1 ), 
                const AllocatorContext& allocatorContext 
                = AllocatorContext() );
    virtual ~VolumeView();

    inline rc_ptr<Volume<T> > refVolume() const;
    inline Position4Di posInRefVolume() const;

  private:
    rc_ptr<Volume<T> >	_refvol;
    Position4Di		_pos;
  };


  // implementation

  template<typename T> inline
  VolumeView<T>::VolumeView( rc_ptr<Volume<T> > other, 
                             const Position4Di & pos, 
                             const Position4Di & size, 
                             const AllocatorContext & allocContext )
    : Volume<T>( size[0] >= 0 ? size[0] : 
                 other->allocatorContext().isAllocated() ? other->getSizeX() 
                 : 1, 
                 size[1] >= 0 ? size[1] : 
                 other->allocatorContext().isAllocated() ? other->getSizeY() 
                 : 1, 
                 size[2] >= 0 ? size[2] : 
                 other->allocatorContext().isAllocated() ? other->getSizeZ() 
                 : 1, 
                 size[3] >= 0 ? size[3] : 
                 other->allocatorContext().isAllocated() ? other->getSizeT() 
                 : 1, 
                 other->allocatorContext().isAllocated() ? 
                 AllocatorContext( AllocatorStrategy::NotOwner, 
                                   rc_ptr<DataSource>
                                   ( new BufferDataSource
                                     ( (char *) &(*other)( pos[0], pos[1], 
                                                           pos[2], pos[3] ), 
                                       size[0] * size[1] * size[2] * size[3] 
                                       * sizeof(T) ) ) )
                 : allocContext ), 
      _refvol( other ), _pos( pos )
  {
    if( other->allocatorContext().isAllocated() )
      {
        // fix offsets
#ifdef CARTO_USE_BLITZ
        Volume<T>::_blitz.reference
          ( blitz::Array<T,4>
            ( &Volume<T>::_items[0], 
              blitz::shape( Volume<T>::getSizeX(), Volume<T>::getSizeY(), 
                            Volume<T>::getSizeZ(), Volume<T>::getSizeT() ), 
              blitz::shape( 1, &(*other)( 0, 1, 0 ) - &(*other)( 0, 0, 0 ), 
                            &(*other)( 0, 0, 1 ) - &(*other)( 0, 0, 0 ), 
                            &(*other)( 0, 0, 0, 1 ) - &(*other)( 0, 0, 0 ) ), 
              blitz::GeneralArrayStorage<4>
              ( blitz::shape( 0, 1, 2, 3 ), true ) ) );
#else
        Volume<T>::_lineoffset = &(*other)( 0, 1, 0 ) - &(*other)( 0, 0, 0 );
        Volume<T>::_sliceoffset = &(*other)( 0, 0, 1 ) - &(*other)( 0, 0, 0 );
        Volume<T>::_volumeoffset = &(*other)( 0, 0, 0, 1 ) 
          - &(*other)( 0, 0, 0 );
#endif
      }
  }



  template<typename T> inline
  rc_ptr<Volume<T> > VolumeView<T>::refVolume() const
  {
    return _refvol;
  }


  template<typename T> inline
  typename VolumeView<T>::Position4Di VolumeView<T>::posInRefVolume() const
  {
    return _pos;
  }

 //  instanciations

  extern template class VolumeView<int8_t>;
  extern template class VolumeView<uint8_t>;
  // ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
  extern template class VolumeView<char>;
#endif
  extern template class VolumeView<int16_t>;
  extern template class VolumeView<uint16_t>;
  extern template class VolumeView<int32_t>;
  extern template class VolumeView<uint32_t>;
  extern template class VolumeView<long>;
  extern template class VolumeView<unsigned long>;
  extern template class VolumeView<float>;
  extern template class VolumeView<double>;
  extern template class VolumeView<cfloat>;
  extern template class VolumeView<cdouble>;
  extern template class VolumeView<soma::VoxelRGB>;
  extern template class VolumeView<soma::VoxelRGBA>;
  extern template class VolumeView<soma::VoxelHSV>;

  
} // namespace carto

#endif