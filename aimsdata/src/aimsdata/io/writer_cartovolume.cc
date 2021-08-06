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

// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/config/aimsdata_config.h>
#include <aims/io/writer_d.h>
#include <aims/rgb/rgb.h>
#include <aims/hsv/hsv.h>
#include <cartodata/volume/volume.h>

using namespace std;
using namespace carto;
using namespace aims;

#define COMMA ,

AIMS_INSTANTIATE_WRITER( Volume<int8_t> );
AIMS_INSTANTIATE_WRITER( Volume<uint8_t> );
AIMS_INSTANTIATE_WRITER( Volume<int16_t> );
AIMS_INSTANTIATE_WRITER( Volume<uint16_t> );
AIMS_INSTANTIATE_WRITER( Volume<int32_t> );
AIMS_INSTANTIATE_WRITER( Volume<uint32_t> );
AIMS_INSTANTIATE_WRITER( Volume<float> );
AIMS_INSTANTIATE_WRITER( Volume<double> );
AIMS_INSTANTIATE_WRITER( Volume<cfloat> );
AIMS_INSTANTIATE_WRITER( Volume<cdouble> );
AIMS_INSTANTIATE_WRITER( Volume<AimsRGB> );
AIMS_INSTANTIATE_WRITER( Volume<AimsRGBA> );
AIMS_INSTANTIATE_WRITER( Volume<AimsHSV> );
// ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
AIMS_INSTANTIATE_WRITER( Volume<char> );
#endif
AIMS_INSTANTIATE_WRITER( Volume<int64_t> );
AIMS_INSTANTIATE_WRITER( Volume<uint64_t> );
AIMS_INSTANTIATE_WRITER( Volume<Point3df> );
AIMS_INSTANTIATE_WRITER( Volume<Point3d> );
AIMS_INSTANTIATE_WRITER( Volume<AimsVector<float COMMA 6> > );

AIMS_INSTANTIATE_WRITER( VolumeRef<int8_t> );
AIMS_INSTANTIATE_WRITER( VolumeRef<uint8_t> );
AIMS_INSTANTIATE_WRITER( VolumeRef<int16_t> );
AIMS_INSTANTIATE_WRITER( VolumeRef<uint16_t> );
AIMS_INSTANTIATE_WRITER( VolumeRef<int32_t> );
AIMS_INSTANTIATE_WRITER( VolumeRef<uint32_t> );
AIMS_INSTANTIATE_WRITER( VolumeRef<float> );
AIMS_INSTANTIATE_WRITER( VolumeRef<double> );
AIMS_INSTANTIATE_WRITER( VolumeRef<cfloat> );
AIMS_INSTANTIATE_WRITER( VolumeRef<cdouble> );
AIMS_INSTANTIATE_WRITER( VolumeRef<AimsRGB> );
AIMS_INSTANTIATE_WRITER( VolumeRef<AimsRGBA> );
AIMS_INSTANTIATE_WRITER( VolumeRef<AimsHSV> );
// ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
AIMS_INSTANTIATE_WRITER( VolumeRef<char> );
#endif
AIMS_INSTANTIATE_WRITER( VolumeRef<int64_t> );
AIMS_INSTANTIATE_WRITER( VolumeRef<uint64_t> );
AIMS_INSTANTIATE_WRITER( VolumeRef<Point3df> );
AIMS_INSTANTIATE_WRITER( VolumeRef<Point3d> );
AIMS_INSTANTIATE_WRITER( VolumeRef<AimsVector<float COMMA 6> > );

