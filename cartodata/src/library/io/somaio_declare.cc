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
#include <cartodata/volume/volume.h>
#include <cartodata/io/volumeformatreader_d.h>
#include <cartodata/io/volumeformatwriter_d.h>
//--- soma-io ----------------------------------------------------------------
#include <soma-io/config/soma_config.h>
#include <soma-io/io/reader_d.h>
#include <soma-io/io/writer_d.h>
#include <soma-io/io/formatdictionary_d.h>
#include <soma-io/reader/formatreader_d.h>
#include <soma-io/vector/vector.h>
#include <soma-io/image/imagereader_d.h>
#include <soma-io/image/imagewriter_d.h>
//--- cartobase --------------------------------------------------------------
#include <cartobase/smart/rcptr.h>
#include <cartobase/type/voxelrgb.h>
#include <cartobase/type/voxelrgba.h>
#include <cartobase/type/voxelhsv.h>
//----------------------------------------------------------------------------

using namespace soma;
using namespace carto;

namespace
{
  typedef AimsVector<float,6> vectorf6;
}


namespace soma
{

//============================================================================
//   R E A D E R
//============================================================================

//=== READER<VOLUME> =========================================================

template class Reader<Volume<int8_t> >;
template class Reader<Volume<int16_t> >;
template class Reader<Volume<int32_t> >;
template class Reader<Volume<int64_t> >;
template class Reader<Volume<uint8_t> >;
template class Reader<Volume<uint16_t> >;
template class Reader<Volume<uint32_t> >;
template class Reader<Volume<uint64_t> >;
template class Reader<Volume<float> >;
template class Reader<Volume<double> >;
template class Reader<Volume<VoxelRGB> >;
template class Reader<Volume<VoxelRGBA> >;
template class Reader<Volume<VoxelHSV> >;
template class Reader<Volume<std::complex<float> > >;
template class Reader<Volume<std::complex<double> > >;
template class Reader<Volume<Point3df> >;
template class Reader<Volume<Point3d> >;
template class Reader<Volume<Point2d> >;
template class Reader<Volume<vectorf6> >;
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
// char may be a different type from int8_t and uint8_t
template class Reader<Volume<char> >;
#endif

//=== READER<VOLUMEREF> ======================================================

template class Reader<VolumeRef<int8_t> >;
template class Reader<VolumeRef<int16_t> >;
template class Reader<VolumeRef<int32_t> >;
template class Reader<VolumeRef<int64_t> >;
template class Reader<VolumeRef<uint8_t> >;
template class Reader<VolumeRef<uint16_t> >;
template class Reader<VolumeRef<uint32_t> >;
template class Reader<VolumeRef<uint64_t> >;
template class Reader<VolumeRef<float> >;
template class Reader<VolumeRef<double> >;
template class Reader<VolumeRef<VoxelRGB> >;
template class Reader<VolumeRef<VoxelRGBA> >;
template class Reader<VolumeRef<VoxelHSV> >;
template class Reader<VolumeRef<std::complex<float> > >;
template class Reader<VolumeRef<std::complex<double> > >;
template class Reader<VolumeRef<Point3df> >;
template class Reader<VolumeRef<Point3d> >;
template class Reader<VolumeRef<Point2d> >;
template class Reader<VolumeRef<vectorf6> >;
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
// char may be a different type from int8_t and uint8_t
template class Reader<VolumeRef<char> >;
#endif

//============================================================================
//   W R I T E R
//============================================================================

//=== WRITER<VOLUME> =========================================================

template class Writer<Volume<int8_t> >;
template class Writer<Volume<int16_t> >;
template class Writer<Volume<int32_t> >;
template class Writer<Volume<int64_t> >;
template class Writer<Volume<uint8_t> >;
template class Writer<Volume<uint16_t> >;
template class Writer<Volume<uint32_t> >;
template class Writer<Volume<uint64_t> >;
template class Writer<Volume<float> >;
template class Writer<Volume<double> >;
template class Writer<Volume<VoxelRGB> >;
template class Writer<Volume<VoxelRGBA> >;
template class Writer<Volume<VoxelHSV> >;
template class Writer<Volume<std::complex<float> > >;
template class Writer<Volume<std::complex<double> > >;
template class Writer<Volume<Point3df> >;
template class Writer<Volume<Point3d> >;
template class Writer<Volume<Point2d> >;
template class Writer<Volume<vectorf6> >;
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
// char may be a different type from int8_t and uint8_t
template class Writer<Volume<char> >;
#endif

//=== WRITER<VOLUMEREF> ======================================================

template class Writer<VolumeRef<int8_t> >;
template class Writer<VolumeRef<int16_t> >;
template class Writer<VolumeRef<int32_t> >;
template class Writer<VolumeRef<int64_t> >;
template class Writer<VolumeRef<uint8_t> >;
template class Writer<VolumeRef<uint16_t> >;
template class Writer<VolumeRef<uint32_t> >;
template class Writer<VolumeRef<uint64_t> >;
template class Writer<VolumeRef<float> >;
template class Writer<VolumeRef<double> >;
template class Writer<VolumeRef<VoxelRGB> >;
template class Writer<VolumeRef<VoxelRGBA> >;
template class Writer<VolumeRef<VoxelHSV> >;
template class Writer<VolumeRef<std::complex<float> > >;
template class Writer<VolumeRef<std::complex<double> > >;
template class Writer<VolumeRef<Point3df> >;
template class Writer<VolumeRef<Point3d> >;
template class Writer<VolumeRef<Point2d> >;
template class Writer<VolumeRef<vectorf6> >;
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
// char may be a different type from int8_t and uint8_t
template class Writer<VolumeRef<char> >;
#endif

//============================================================================
//   F O R M A T D I C T I O N A R Y
//============================================================================

//=== FORMATDICTIONARY<VOLUME> ===============================================

template class FormatDictionary<Volume<int8_t> >;
template class FormatDictionary<Volume<int16_t> >;
template class FormatDictionary<Volume<int32_t> >;
template class FormatDictionary<Volume<int64_t> >;
template class FormatDictionary<Volume<uint8_t> >;
template class FormatDictionary<Volume<uint16_t> >;
template class FormatDictionary<Volume<uint32_t> >;
template class FormatDictionary<Volume<uint64_t> >;
template class FormatDictionary<Volume<float> >;
template class FormatDictionary<Volume<double> >;
template class FormatDictionary<Volume<VoxelRGB> >;
template class FormatDictionary<Volume<VoxelRGBA> >;
template class FormatDictionary<Volume<VoxelHSV> >;
template class FormatDictionary<Volume<std::complex<float> > >;
template class FormatDictionary<Volume<std::complex<double> > >;
template class FormatDictionary<Volume<Point3df> >;
template class FormatDictionary<Volume<Point3d> >;
template class FormatDictionary<Volume<Point2d> >;
template class FormatDictionary<Volume<vectorf6> >;
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
// char may be a different type from int8_t and uint8_t
template class FormatDictionary<Volume<char> >;
#endif

//=== FORMATDICTIONARY<VOLUMEREF> ============================================

template class FormatDictionary<VolumeRef<int8_t> >;
template class FormatDictionary<VolumeRef<int16_t> >;
template class FormatDictionary<VolumeRef<int32_t> >;
template class FormatDictionary<VolumeRef<int64_t> >;
template class FormatDictionary<VolumeRef<uint8_t> >;
template class FormatDictionary<VolumeRef<uint16_t> >;
template class FormatDictionary<VolumeRef<uint32_t> >;
template class FormatDictionary<VolumeRef<uint64_t> >;
template class FormatDictionary<VolumeRef<float> >;
template class FormatDictionary<VolumeRef<double> >;
template class FormatDictionary<VolumeRef<VoxelRGB> >;
template class FormatDictionary<VolumeRef<VoxelRGBA> >;
template class FormatDictionary<VolumeRef<VoxelHSV> >;
template class FormatDictionary<VolumeRef<std::complex<float> > >;
template class FormatDictionary<VolumeRef<std::complex<double> > >;
template class FormatDictionary<VolumeRef<Point3df> >;
template class FormatDictionary<VolumeRef<Point3d> >;
template class FormatDictionary<VolumeRef<Point2d> >;
template class FormatDictionary<VolumeRef<vectorf6> >;
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
// char may be a different type from int8_t and uint8_t
template class FormatDictionary<VolumeRef<char> >;
#endif

//============================================================================
//   F O R M A T R E A D E R 
//============================================================================

//=== FORMATREADER<VOLUME> ===================================================

template class FormatReader<Volume<int8_t> >;
template class FormatReader<Volume<int16_t> >;
template class FormatReader<Volume<int32_t> >;
template class FormatReader<Volume<int64_t> >;
template class FormatReader<Volume<uint8_t> >;
template class FormatReader<Volume<uint16_t> >;
template class FormatReader<Volume<uint32_t> >;
template class FormatReader<Volume<uint64_t> >;
template class FormatReader<Volume<float> >;
template class FormatReader<Volume<double> >;
template class FormatReader<Volume<VoxelRGB> >;
template class FormatReader<Volume<VoxelRGBA> >;
template class FormatReader<Volume<VoxelHSV> >;
template class FormatReader<Volume<std::complex<float> > >;
template class FormatReader<Volume<std::complex<double> > >;
template class FormatReader<Volume<Point3df> >;
template class FormatReader<Volume<Point3d> >;
template class FormatReader<Volume<Point2d> >;
template class FormatReader<Volume<vectorf6> >;
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
// char may be a different type from int8_t and uint8_t
template class FormatReader<Volume<char> >;
#endif

//=== FORMATREADER<VOLUMEREF> ================================================

template class FormatReader<VolumeRef<int8_t> >;
template class FormatReader<VolumeRef<int16_t> >;
template class FormatReader<VolumeRef<int32_t> >;
template class FormatReader<VolumeRef<int64_t> >;
template class FormatReader<VolumeRef<uint8_t> >;
template class FormatReader<VolumeRef<uint16_t> >;
template class FormatReader<VolumeRef<uint32_t> >;
template class FormatReader<VolumeRef<uint64_t> >;
template class FormatReader<VolumeRef<float> >;
template class FormatReader<VolumeRef<double> >;
template class FormatReader<VolumeRef<VoxelRGB> >;
template class FormatReader<VolumeRef<VoxelRGBA> >;
template class FormatReader<VolumeRef<VoxelHSV> >;
template class FormatReader<VolumeRef<std::complex<float> > >;
template class FormatReader<VolumeRef<std::complex<double> > >;
template class FormatReader<VolumeRef<Point3df> >;
template class FormatReader<VolumeRef<Point3d> >;
template class FormatReader<VolumeRef<Point2d> >;
template class FormatReader<VolumeRef<vectorf6> >;
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
// char may be a different type from int8_t and uint8_t
template class FormatReader<VolumeRef<char> >;
#endif

//=== VOLUMEFORMATREADER =====================================================

template class VolumeFormatReader<int8_t>;
template class VolumeFormatReader<int16_t>;
template class VolumeFormatReader<int32_t>;
template class VolumeFormatReader<int64_t>;
template class VolumeFormatReader<uint8_t>;
template class VolumeFormatReader<uint16_t>;
template class VolumeFormatReader<uint32_t>;
template class VolumeFormatReader<uint64_t>;
template class VolumeFormatReader<float>;
template class VolumeFormatReader<double>;
template class VolumeFormatReader<VoxelRGB>;
template class VolumeFormatReader<VoxelRGBA>;
template class VolumeFormatReader<VoxelHSV>;
template class VolumeFormatReader<std::complex<float> >;
template class VolumeFormatReader<std::complex<double> >;
template class VolumeFormatReader<Point3df>;
template class VolumeFormatReader<Point3d>;
template class VolumeFormatReader<Point2d>;
template class VolumeFormatReader<vectorf6>;
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
// char may be a different type from int8_t and uint8_t
template class VolumeFormatReader<char>;
#endif

//=== VOLUMEREFFORMATREADER ==================================================

template class VolumeRefFormatReader<int8_t>;
template class VolumeRefFormatReader<int16_t>;
template class VolumeRefFormatReader<int32_t>;
template class VolumeRefFormatReader<int64_t>;
template class VolumeRefFormatReader<uint8_t>;
template class VolumeRefFormatReader<uint16_t>;
template class VolumeRefFormatReader<uint32_t>;
template class VolumeRefFormatReader<uint64_t>;
template class VolumeRefFormatReader<float>;
template class VolumeRefFormatReader<double>;
template class VolumeRefFormatReader<VoxelRGB>;
template class VolumeRefFormatReader<VoxelRGBA>;
template class VolumeRefFormatReader<VoxelHSV>;
template class VolumeRefFormatReader<std::complex<float> >;
template class VolumeRefFormatReader<std::complex<double> >;
template class VolumeRefFormatReader<Point3df>;
template class VolumeRefFormatReader<Point3d>;
template class VolumeRefFormatReader<Point2d>;
template class VolumeRefFormatReader<vectorf6>;
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
// char may be a different type from int8_t and uint8_t
template class VolumeRefFormatReader<char>;
#endif

//============================================================================
//   F O R M A T R E A D E R 
//============================================================================

//=== VOLUMEFORMATWRITER =====================================================

template class VolumeFormatWriter<int8_t>;
template class VolumeFormatWriter<int16_t>;
template class VolumeFormatWriter<int32_t>;
template class VolumeFormatWriter<int64_t>;
template class VolumeFormatWriter<uint8_t>;
template class VolumeFormatWriter<uint16_t>;
template class VolumeFormatWriter<uint32_t>;
template class VolumeFormatWriter<uint64_t>;
template class VolumeFormatWriter<float>;
template class VolumeFormatWriter<double>;
template class VolumeFormatWriter<VoxelRGB>;
template class VolumeFormatWriter<VoxelRGBA>;
template class VolumeFormatWriter<VoxelHSV>;
template class VolumeFormatWriter<std::complex<float> >;
template class VolumeFormatWriter<std::complex<double> >;
template class VolumeFormatWriter<Point3df>;
template class VolumeFormatWriter<Point3d>;
template class VolumeFormatWriter<Point2d>;
template class VolumeFormatWriter<vectorf6>;
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
// char may be a different type from int8_t and uint8_t
template class VolumeFormatWriter<char>;
#endif

//=== VOLUMEREFFORMATWRITER ==================================================

template class VolumeRefFormatWriter<int8_t>;
template class VolumeRefFormatWriter<int16_t>;
template class VolumeRefFormatWriter<int32_t>;
template class VolumeRefFormatWriter<int64_t>;
template class VolumeRefFormatWriter<uint8_t>;
template class VolumeRefFormatWriter<uint16_t>;
template class VolumeRefFormatWriter<uint32_t>;
template class VolumeRefFormatWriter<uint64_t>;
template class VolumeRefFormatWriter<float>;
template class VolumeRefFormatWriter<double>;
template class VolumeRefFormatWriter<VoxelRGB>;
template class VolumeRefFormatWriter<VoxelRGBA>;
template class VolumeRefFormatWriter<VoxelHSV>;
template class VolumeRefFormatWriter<std::complex<float> >;
template class VolumeRefFormatWriter<std::complex<double> >;
template class VolumeRefFormatWriter<Point3df>;
template class VolumeRefFormatWriter<Point3d>;
template class VolumeRefFormatWriter<Point2d>;
template class VolumeRefFormatWriter<vectorf6>;
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
// char may be a different type from int8_t and uint8_t
template class VolumeRefFormatWriter<char>;
#endif

//=== IMAGEREADER ============================================================

template class ImageReader<Point3df>;
template class ImageReader<Point3d>;
template class ImageReader<Point2d>;
template class ImageReader<AimsVector<float, 6> >;

//=== IMAGEWRITER ============================================================

template class ImageWriter<Point3df>;
template class ImageWriter<Point3d>;
template class ImageWriter<Point2d>;
template class ImageWriter<AimsVector<float, 6> >;

}
