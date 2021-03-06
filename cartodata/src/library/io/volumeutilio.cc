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

//--- cartodata --------------------------------------------------------------
#include <cartodata/io/volumeutilio_d.h>
//--- soma-io ----------------------------------------------------------------
#include <soma-io/config/soma_config.h>
//--- cartobase --------------------------------------------------------------
#include <cartobase/type/voxelrgb.h>
#include <cartobase/type/voxelrgba.h>
#include <cartobase/type/voxelhsv.h>
//----------------------------------------------------------------------------

using namespace soma;
using namespace carto;

namespace soma {

template class VolumeUtilIO<int8_t>;
template class VolumeUtilIO<int16_t>;
template class VolumeUtilIO<int32_t>;
template class VolumeUtilIO<int64_t>;
template class VolumeUtilIO<uint8_t>;
template class VolumeUtilIO<uint16_t>;
template class VolumeUtilIO<uint32_t>;
template class VolumeUtilIO<uint64_t>;
template class VolumeUtilIO<float>;
template class VolumeUtilIO<double>;
template class VolumeUtilIO<VoxelRGB>;
template class VolumeUtilIO<VoxelRGBA>;
template class VolumeUtilIO<VoxelHSV>;
template class VolumeUtilIO<std::complex<float> >;
template class VolumeUtilIO<std::complex<double> >;
template class VolumeUtilIO<Point3df>;
template class VolumeUtilIO<Point3d>;
template class VolumeUtilIO<Point2d>;
template class VolumeUtilIO<AimsVector<float, 6> >;
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
// char may be a different type from int8_t and uint8_t
template class VolumeUtilIO<char>;
#endif

}
