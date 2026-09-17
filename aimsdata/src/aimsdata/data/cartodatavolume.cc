// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

// compilation of some Volume classes on Aims types

#include <cartodata/volume/volume_d.h>
#include <cartodata/volume/volumeview.h>
#include <aims/rgb/rgb.h>
#include <aims/hsv/hsv.h>
#include <aims/io/datatypecode.h>
#include <aims/vector/vector.h>
#include <aims/math/dtitensor.h>
#include <aims/data/volumeoperators_aims.h>
#include <cartodata/volume/volumeutil_d.h>
#include <cartobase/object/object_d.h>
#include <set>
#include <map>


namespace carto
{

  template class carto::VolumeProxy< DtiTensor * >;
  template class carto::Volume< DtiTensor * >;

  template class carto::VolumeProxy< std::set<float> >;
  template class carto::Volume< std::set<float> >;

  template class carto::VolumeProxy< std::map<int, float> >;
  template class carto::Volume< std::map<int, float> >;

  // Mac LLVM compiler does not seem to allow indirect instanciation
  template class carto::VolumeRef< DtiTensor * >;
  template class carto::VolumeRef< std::set<float> >;
  template class carto::VolumeRef< std::map<int, float> >;

  INSTANTIATE_GENERIC_OBJECT_TYPE( VolumeRef< DtiTensor* > )
  INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr<Volume< DtiTensor* > > )


  template class Creator<Volume<DtiTensor*> >;
  template class Creator<VolumeRef<DtiTensor*> >;

} // namespace carto
