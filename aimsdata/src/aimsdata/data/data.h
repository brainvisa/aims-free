/*
 *  4D-data template class
 */
#ifndef AIMS_DATA_DATA_H
#define AIMS_DATA_DATA_H

#include <aims/data/cartodatavolume.h>
#include <aims/rgb/rgb.h>
#include <aims/hsv/hsv.h>

// this part will not be always included in future releases
#include <aims/data/volumemanip.h>

namespace carto {

DECLARE_GENERIC_OBJECT_TYPE( AimsData<bool> )
DECLARE_GENERIC_OBJECT_TYPE( AimsData<int8_t> )
DECLARE_GENERIC_OBJECT_TYPE( AimsData<uint8_t> )
DECLARE_GENERIC_OBJECT_TYPE( AimsData<int16_t> )
DECLARE_GENERIC_OBJECT_TYPE( AimsData<uint16_t> )
DECLARE_GENERIC_OBJECT_TYPE( AimsData<int32_t> )
DECLARE_GENERIC_OBJECT_TYPE( AimsData<uint32_t> )
DECLARE_GENERIC_OBJECT_TYPE( AimsData<int64_t> )
DECLARE_GENERIC_OBJECT_TYPE( AimsData<uint64_t> )
DECLARE_GENERIC_OBJECT_TYPE( AimsData<float> )
DECLARE_GENERIC_OBJECT_TYPE( AimsData<double> )
DECLARE_GENERIC_OBJECT_TYPE( AimsData< std::complex<float> > )
DECLARE_GENERIC_OBJECT_TYPE( AimsData< std::complex<double> > )
DECLARE_GENERIC_OBJECT_TYPE( AimsData<AimsRGB> )
DECLARE_GENERIC_OBJECT_TYPE( AimsData<AimsRGBA> )
DECLARE_GENERIC_OBJECT_TYPE( AimsData<AimsHSV> )
DECLARE_GENERIC_OBJECT_TYPE( AimsData<Point3d> )
DECLARE_GENERIC_OBJECT_TYPE( AimsData<Point3df> )
DECLARE_GENERIC_OBJECT_TYPE( AimsData<Point3dd> )
#define _TMP_ AimsData< AimsVector<float, 6> > 
DECLARE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_
#ifdef CARTO_LONG_IS_DISTINCT
DECLARE_GENERIC_OBJECT_TYPE( AimsData<long> )
DECLARE_GENERIC_OBJECT_TYPE( AimsData<unsigned long> )
#endif

DECLARE_GENERIC_OBJECT_TYPE( rc_ptr< AimsData<bool> > )
DECLARE_GENERIC_OBJECT_TYPE( rc_ptr< AimsData<int8_t> > )
DECLARE_GENERIC_OBJECT_TYPE( rc_ptr< AimsData<uint8_t> > )
DECLARE_GENERIC_OBJECT_TYPE( rc_ptr< AimsData<int16_t> > )
DECLARE_GENERIC_OBJECT_TYPE( rc_ptr< AimsData<uint16_t> > )
DECLARE_GENERIC_OBJECT_TYPE( rc_ptr< AimsData<int32_t> > )
DECLARE_GENERIC_OBJECT_TYPE( rc_ptr< AimsData<uint32_t> > )
DECLARE_GENERIC_OBJECT_TYPE( rc_ptr< AimsData<int64_t> > )
DECLARE_GENERIC_OBJECT_TYPE( rc_ptr< AimsData<uint64_t> > )
DECLARE_GENERIC_OBJECT_TYPE( rc_ptr< AimsData<float> > )
DECLARE_GENERIC_OBJECT_TYPE( rc_ptr< AimsData<double> > )
DECLARE_GENERIC_OBJECT_TYPE( rc_ptr< AimsData< std::complex<double> > > )
DECLARE_GENERIC_OBJECT_TYPE( rc_ptr< AimsData< std::complex<float> > > )
DECLARE_GENERIC_OBJECT_TYPE( rc_ptr< AimsData<AimsRGB> > )
DECLARE_GENERIC_OBJECT_TYPE( rc_ptr< AimsData<AimsRGBA> > )
DECLARE_GENERIC_OBJECT_TYPE( rc_ptr< AimsData<AimsHSV> > )
DECLARE_GENERIC_OBJECT_TYPE( rc_ptr< AimsData<Point3d> > )
DECLARE_GENERIC_OBJECT_TYPE( rc_ptr< AimsData<Point3df> > )
DECLARE_GENERIC_OBJECT_TYPE( rc_ptr< AimsData<Point3dd> > )
#define _TMP_ rc_ptr< AimsData< AimsVector<float, 6> > >
DECLARE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_
#ifdef CARTO_LONG_IS_DISTINCT
DECLARE_GENERIC_OBJECT_TYPE( rc_ptr< AimsData<long> > )
DECLARE_GENERIC_OBJECT_TYPE( rc_ptr< AimsData<unsigned long> > )
#endif

} // namespace carto


#endif
