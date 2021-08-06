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

// we don't want to issue a warning
#ifndef AIMSDATA_CLASS_NO_DEPREC_WARNING
#define AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/config/aimsdata_config.h>
#include <soma-io/io/reader_d.h>
#include <soma-io/io/formatdictionary_d.h>
#include <aims/io/reader_d.h>
#include <aims/data/data.h>
#include <aims/rgb/rgb.h>
#include <aims/hsv/hsv.h>

using namespace aims;
using namespace carto;
using namespace std;

#define COMMA ,

AIMS_INSTANTIATE_READER( AimsData<int8_t> );
AIMS_INSTANTIATE_READER( AimsData<uint8_t> );
AIMS_INSTANTIATE_READER( AimsData<int16_t> );
AIMS_INSTANTIATE_READER( AimsData<uint16_t> );
AIMS_INSTANTIATE_READER( AimsData<int32_t> );
AIMS_INSTANTIATE_READER( AimsData<uint32_t> );
AIMS_INSTANTIATE_READER( AimsData<float> );
AIMS_INSTANTIATE_READER( AimsData<double> );
AIMS_INSTANTIATE_READER( AimsData<cfloat> );
AIMS_INSTANTIATE_READER( AimsData<cdouble> );
AIMS_INSTANTIATE_READER( AimsData<AimsRGB> );
AIMS_INSTANTIATE_READER( AimsData<AimsRGBA> );
AIMS_INSTANTIATE_READER( AimsData<AimsHSV> );
AIMS_INSTANTIATE_READER( AimsData<Point3df> );
AIMS_INSTANTIATE_READER( AimsData<Point3d> );
AIMS_INSTANTIATE_READER( AimsData<AimsVector<float COMMA 6> > );

// ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
AIMS_INSTANTIATE_READER( AimsData<char> );
#endif
AIMS_INSTANTIATE_READER( AimsData<int64_t> );
AIMS_INSTANTIATE_READER( AimsData<uint64_t> );


