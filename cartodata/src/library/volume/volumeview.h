#ifndef CARTODATA_VOLUME_VOLUMEVIEW_H
#define CARTODATA_VOLUME_VOLUMEVIEW_H

#include <cartodata/volume/volume.h>

namespace carto
{

  /// View into a Volume
  /// \deprecated
  /// Do not use anymore. Use Volume, which now manages views, instead.
  template <typename T>
  class __attribute__((__deprecated__("Use Volume, which now manages views, instead."))) VolumeView
    : public Volume<T>
  {
  public:
    typedef typename Volume<T>::Position4Di Position4Di;

    VolumeView( rc_ptr<Volume<T> > other,
                const Position4Di & pos = Position4Di( 0, 0, 0, 0 ),
                const Position4Di & size = Position4Di( -1, -1, -1, -1 ),
                const AllocatorContext& allocatorContext
                = AllocatorContext() )
    : Volume<T>( other, pos, size, allocatorContext )
    {
    }

    ~VolumeView() {}

  };

} // namespace carto

#endif
