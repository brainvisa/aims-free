// we don't want to issue a warning while compiling the obsolete classes
// themselves...
#ifndef AIMSDATA_CLASS_NO_DEPREC_WARNING
#define AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <cartobase/object/object_d.h>
#include <aims/data/data.h>
 
namespace carto {

INSTANTIATE_GENERIC_OBJECT_TYPE( AimsData<bool> )
INSTANTIATE_GENERIC_OBJECT_TYPE( AimsData<int8_t> )
INSTANTIATE_GENERIC_OBJECT_TYPE( AimsData<uint8_t> )
INSTANTIATE_GENERIC_OBJECT_TYPE( AimsData<int16_t> )
INSTANTIATE_GENERIC_OBJECT_TYPE( AimsData<uint16_t> )
INSTANTIATE_GENERIC_OBJECT_TYPE( AimsData<int32_t> )
INSTANTIATE_GENERIC_OBJECT_TYPE( AimsData<uint32_t> )
INSTANTIATE_GENERIC_OBJECT_TYPE( AimsData<int64_t> )
INSTANTIATE_GENERIC_OBJECT_TYPE( AimsData<uint64_t> )
INSTANTIATE_GENERIC_OBJECT_TYPE( AimsData<float> )
INSTANTIATE_GENERIC_OBJECT_TYPE( AimsData<double> )
INSTANTIATE_GENERIC_OBJECT_TYPE( AimsData< std::complex<float> > )
INSTANTIATE_GENERIC_OBJECT_TYPE( AimsData< std::complex<double> > )
INSTANTIATE_GENERIC_OBJECT_TYPE( AimsData<AimsRGB> )
INSTANTIATE_GENERIC_OBJECT_TYPE( AimsData<AimsRGBA> )
INSTANTIATE_GENERIC_OBJECT_TYPE( AimsData<AimsHSV> )
INSTANTIATE_GENERIC_OBJECT_TYPE( AimsData<Point3d> )
INSTANTIATE_GENERIC_OBJECT_TYPE( AimsData<Point3df> )
INSTANTIATE_GENERIC_OBJECT_TYPE( AimsData<Point3dd> )
#define _TMP_ AimsData< AimsVector<float, 6> > 
INSTANTIATE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_
#ifdef CARTO_LONG_IS_DISTINCT
INSTANTIATE_GENERIC_OBJECT_TYPE( AimsData<long> )
INSTANTIATE_GENERIC_OBJECT_TYPE( AimsData<unsigned long> )
#endif

INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr< AimsData<bool> > )
INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr< AimsData<int8_t> > )
INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr< AimsData<uint8_t> > )
INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr< AimsData<int16_t> > )
INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr< AimsData<uint16_t> > )
INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr< AimsData<int32_t> > )
INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr< AimsData<uint32_t> > )
INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr< AimsData<int64_t> > )
INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr< AimsData<uint64_t> > )
INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr< AimsData<float> > )
INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr< AimsData<double> > )
INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr< AimsData< std::complex<double> > > )
INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr< AimsData< std::complex<float> > > )
INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr< AimsData<AimsRGB> > )
INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr< AimsData<AimsRGBA> > )
INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr< AimsData<AimsHSV> > )
INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr< AimsData<Point3d> > )
INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr< AimsData<Point3df> > )
INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr< AimsData<Point3dd> > )
#define _TMP_ rc_ptr< AimsData< AimsVector<float, 6> > >
INSTANTIATE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_
#ifdef CARTO_LONG_IS_DISTINCT
INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr< AimsData<long> > )
INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr< AimsData<unsigned long> > )
#endif

} // namespace carto
