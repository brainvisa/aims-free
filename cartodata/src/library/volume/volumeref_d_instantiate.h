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

#ifndef CARTODATA_VOLUME_VOLUMEREF_D_INSTANTIATE_H
#define CARTODATA_VOLUME_VOLUMEREF_D_INSTANTIATE_H

//--- cartodata --------------------------------------------------------------
#include <cartodata/volume/volumeref.h>
//--- cartobase --------------------------------------------------------------
#include <cartobase/type/types.h>
#include <cartobase/type/voxelrgb.h>
#include <cartobase/type/voxelhsv.h>
//----------------------------------------------------------------------------

namespace carto
{

//============================================================================
//   I N S T A N C I A T I O N
//============================================================================

  extern template class Creator<VolumeRef<bool> >;
  extern template class Creator<VolumeRef<int8_t> >;
  extern template class Creator<VolumeRef<uint8_t> >;
  // ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
  extern template class Creator<VolumeRef<char> >;
#endif
  extern template class Creator<VolumeRef<int16_t> >;
  extern template class Creator<VolumeRef<uint16_t> >;
  extern template class Creator<VolumeRef<int32_t> >;
  extern template class Creator<VolumeRef<uint32_t> >;
  extern template class Creator<VolumeRef<int64_t> >;
  extern template class Creator<VolumeRef<uint64_t> >;
  extern template class Creator<VolumeRef<float> >;
  extern template class Creator<VolumeRef<double> >;
  extern template class Creator<VolumeRef<cfloat> >;
  extern template class Creator<VolumeRef<cdouble> >;
  extern template class Creator<VolumeRef< std::map<int, float> > >;
  extern template class Creator<VolumeRef<soma::VoxelRGB> >;
  extern template class Creator<VolumeRef<soma::VoxelRGBA> >;
  extern template class Creator<VolumeRef<soma::VoxelHSV> >;
  extern template class Creator<VolumeRef<Point3df> >;
  extern template class Creator<VolumeRef<Point3d> >;
  extern template class Creator<VolumeRef<Point3dd> >;
  extern template class Creator<VolumeRef<Point2d> >;
  extern template class Creator<VolumeRef<AimsVector<float, 6> > >;
#ifdef CARTO_LONG_IS_DISTINCT
  extern template class Creator<VolumeRef<long> >;
  extern template class Creator<VolumeRef<unsigned long> >;
#endif

  DECLARE_GENERIC_OBJECT_TYPE( VolumeRef< bool > )
  DECLARE_GENERIC_OBJECT_TYPE( VolumeRef< int8_t > )
  DECLARE_GENERIC_OBJECT_TYPE( VolumeRef< uint8_t > )
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
  DECLARE_GENERIC_OBJECT_TYPE( VolumeRef< char > )
#endif
  DECLARE_GENERIC_OBJECT_TYPE( VolumeRef< int16_t > )
  DECLARE_GENERIC_OBJECT_TYPE( VolumeRef< uint16_t > )
  DECLARE_GENERIC_OBJECT_TYPE( VolumeRef< int32_t > )
  DECLARE_GENERIC_OBJECT_TYPE( VolumeRef< uint32_t > )
  DECLARE_GENERIC_OBJECT_TYPE( VolumeRef< int64_t > )
  DECLARE_GENERIC_OBJECT_TYPE( VolumeRef< uint64_t > )
  DECLARE_GENERIC_OBJECT_TYPE( VolumeRef< float > )
  DECLARE_GENERIC_OBJECT_TYPE( VolumeRef< double > )
  DECLARE_GENERIC_OBJECT_TYPE( VolumeRef< cfloat > )
  DECLARE_GENERIC_OBJECT_TYPE( VolumeRef< cdouble > )
  DECLARE_GENERIC_OBJECT_TYPE( VolumeRef< soma::VoxelRGB > )
  DECLARE_GENERIC_OBJECT_TYPE( VolumeRef< soma::VoxelRGBA > )
  DECLARE_GENERIC_OBJECT_TYPE( VolumeRef< soma::VoxelHSV > )
  DECLARE_GENERIC_OBJECT_TYPE( VolumeRef< Point3df > )
  DECLARE_GENERIC_OBJECT_TYPE( VolumeRef< Point3d > )
  DECLARE_GENERIC_OBJECT_TYPE( VolumeRef< Point3dd > )
  DECLARE_GENERIC_OBJECT_TYPE( VolumeRef< Point2d > )
#define CARTO_COMA ,
  DECLARE_GENERIC_OBJECT_TYPE( VolumeRef< AimsVector<float CARTO_COMA 6 > > )
#undef CARTO_COMA
#ifdef CARTO_LONG_IS_DISTINCT
  DECLARE_GENERIC_OBJECT_TYPE( VolumeRef< long > )
  DECLARE_GENERIC_OBJECT_TYPE( VolumeRef< unsigned long > )
#endif

} // namespace carto

#endif // CARTODATA_VOLUME_VOLUMEREF_D_INSTANTIATE_H
