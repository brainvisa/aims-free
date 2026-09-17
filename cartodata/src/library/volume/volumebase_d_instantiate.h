#ifndef CARTODATA_VOLUME_VOLUMEBASE_D_INSTANTIATE_H
#define CARTODATA_VOLUME_VOLUMEBASE_D_INSTANTIATE_H

//--- cartodata --------------------------------------------------------------
#include <cartodata/volume/volumebase.h>
//--- cartobase --------------------------------------------------------------
#include <cartobase/smart/rcptr.h>
#include <cartobase/type/types.h>
#include <cartobase/type/voxelrgb.h>
#include <cartobase/type/voxelhsv.h>
#include <soma-io/vector/vector.h>
//----------------------------------------------------------------------------

namespace carto {

//============================================================================
//   I N S T A N C I A T I O N
//============================================================================

  extern template class Volume<bool>;
  extern template class Volume<int8_t>;
  extern template class Volume<uint8_t>;
  // ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
  extern template class Volume<char>;
#endif
  extern template class Volume<int16_t>;
  extern template class Volume<uint16_t>;
  extern template class Volume<int32_t>;
  extern template class Volume<uint32_t>;
  extern template class Volume<int64_t>;
  extern template class Volume<uint64_t>;
  extern template class Volume<float>;
  extern template class Volume<double>;
  extern template class Volume<cfloat>;
  extern template class Volume<cdouble>;
  extern template class Volume< std::map<int, float> >;
  extern template class Volume< soma::VoxelRGB >;
  extern template class Volume< soma::VoxelRGBA >;
  extern template class Volume< soma::VoxelHSV >;
  extern template class Volume<Point3df>;
  extern template class Volume<Point3d>;
  extern template class Volume<Point3dd>;
  extern template class Volume<Point2d>;
  extern template class Volume<AimsVector<float,6> >;
#ifdef CARTO_LONG_IS_DISTINCT
  extern template class Volume<long>;
  extern template class Volume<unsigned long>;
#endif

  extern template class Creator<Volume<bool> >;
  extern template class Creator<Volume<int8_t> >;
  extern template class Creator<Volume<uint8_t> >;
  // ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
  extern template class Creator<Volume<char> >;
#endif
  extern template class Creator<Volume<int16_t> >;
  extern template class Creator<Volume<uint16_t> >;
  extern template class Creator<Volume<int32_t> >;
  extern template class Creator<Volume<uint32_t> >;
  extern template class Creator<Volume<int64_t> >;
  extern template class Creator<Volume<uint64_t> >;
  extern template class Creator<Volume<float> >;
  extern template class Creator<Volume<double> >;
  extern template class Creator<Volume<cfloat> >;
  extern template class Creator<Volume<cdouble> >;
  extern template class Creator<Volume< std::map<int, float> > >;
  extern template class Creator<Volume<soma::VoxelRGB> >;
  extern template class Creator<Volume<soma::VoxelRGBA> >;
  extern template class Creator<Volume<soma::VoxelHSV> >;
  extern template class Creator<Point3df>;
  extern template class Creator<Point3d>;
  extern template class Creator<Point3dd>;
  extern template class Creator<Point2d>;
  extern template class Creator<AimsVector<float,6> >;
#ifdef CARTO_LONG_IS_DISTINCT
  extern template class Creator<Volume<long> >;
  extern template class Creator<Volume<unsigned long> >;
#endif

  DECLARE_GENERIC_OBJECT_TYPE( rc_ptr<Volume< bool > > )
  DECLARE_GENERIC_OBJECT_TYPE( rc_ptr<Volume< int8_t > > )
  DECLARE_GENERIC_OBJECT_TYPE( rc_ptr<Volume< uint8_t > > )
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
  DECLARE_GENERIC_OBJECT_TYPE( rc_ptr<Volume< char > > )
#endif
  DECLARE_GENERIC_OBJECT_TYPE( rc_ptr<Volume< int16_t > > )
  DECLARE_GENERIC_OBJECT_TYPE( rc_ptr<Volume< uint16_t > > )
  DECLARE_GENERIC_OBJECT_TYPE( rc_ptr<Volume< int32_t > > )
  DECLARE_GENERIC_OBJECT_TYPE( rc_ptr<Volume< uint32_t > > )
  DECLARE_GENERIC_OBJECT_TYPE( rc_ptr<Volume< int64_t > > )
  DECLARE_GENERIC_OBJECT_TYPE( rc_ptr<Volume< uint64_t > > )
  DECLARE_GENERIC_OBJECT_TYPE( rc_ptr<Volume< float > > )
  DECLARE_GENERIC_OBJECT_TYPE( rc_ptr<Volume< double > > )
  DECLARE_GENERIC_OBJECT_TYPE( rc_ptr<Volume< cfloat > > )
  DECLARE_GENERIC_OBJECT_TYPE( rc_ptr<Volume< cdouble > > )
  DECLARE_GENERIC_OBJECT_TYPE( rc_ptr<Volume< soma::VoxelRGB > > )
  DECLARE_GENERIC_OBJECT_TYPE( rc_ptr<Volume< soma::VoxelRGBA > > )
  DECLARE_GENERIC_OBJECT_TYPE( rc_ptr<Volume< soma::VoxelHSV > > )
  DECLARE_GENERIC_OBJECT_TYPE( rc_ptr<Volume< Point3df > > )
  DECLARE_GENERIC_OBJECT_TYPE( rc_ptr<Volume< Point3d > > )
  DECLARE_GENERIC_OBJECT_TYPE( rc_ptr<Volume< Point3dd > > )
  DECLARE_GENERIC_OBJECT_TYPE( rc_ptr<Volume< Point2d > > )
#define CARTO_COMA ,
  DECLARE_GENERIC_OBJECT_TYPE( rc_ptr<Volume< AimsVector<float CARTO_COMA 6> > > )
#undef CARTO_COMA
#ifdef CARTO_LONG_IS_DISTINCT
  DECLARE_GENERIC_OBJECT_TYPE( rc_ptr<Volume< long > > )
  DECLARE_GENERIC_OBJECT_TYPE( rc_ptr<Volume< unsigned long > > )
#endif

} // namespace carto

#endif // CARTODATA_VOLUME_VOLUMEBASE_D_INSTANTIATE_H
