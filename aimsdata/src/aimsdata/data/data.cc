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
