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

//--- aims ---------------------------------------------------------------------
#include <aims/rgb/rgb.h>
#include <aims/hsv/hsv.h>
#include <aims/data/cartodatavolume.h>
//--- cartodata ----------------------------------------------------------------
#include <cartodata/volume/volume.h>
#include <cartodata/io/volumeformatreader_d.h>
#include <cartodata/io/volumeformatwriter_d.h>
//--- soma-io ------------------------------------------------------------------
#include <soma-io/config/soma_config.h>
#include <soma-io/io/reader_d.h>
#include <soma-io/io/writer_d.h>
#include <soma-io/io/formatdictionary_d.h>
#include <soma-io/reader/formatreader_d.h>
//--- cartobase ----------------------------------------------------------------
#include <cartobase/smart/rcptr.h>
//------------------------------------------------------------------------------

using namespace aims;
using namespace soma;
using namespace carto;

namespace soma {

//==============================================================================
//   R E A D E R
//==============================================================================

//=== READER<VOLUME> ===========================================================

template class Reader<Volume<int8_t> >;
template class Reader<Volume<int16_t> >;
template class Reader<Volume<int32_t> >;
template class Reader<Volume<uint8_t> >;
template class Reader<Volume<uint16_t> >;
template class Reader<Volume<uint32_t> >;
template class Reader<Volume<float> >;
template class Reader<Volume<double> >;
// template class Reader<Volume<AimsRGB> >;
// template class Reader<Volume<AimsRGBA> >;
// template class Reader<Volume<AimsHSV> >;

//=== READER<VOLUMEREF> ========================================================

template class Reader<VolumeRef<int8_t> >;
template class Reader<VolumeRef<int16_t> >;
template class Reader<VolumeRef<int32_t> >;
template class Reader<VolumeRef<uint8_t> >;
template class Reader<VolumeRef<uint16_t> >;
template class Reader<VolumeRef<uint32_t> >;
template class Reader<VolumeRef<float> >;
template class Reader<VolumeRef<double> >;
// template class Reader<VolumeRef<AimsRGB> >;
// template class Reader<VolumeRef<AimsRGBA> >;
// template class Reader<VolumeRef<AimsHSV> >;

//==============================================================================
//   W R I T E R
//==============================================================================

//=== WRITER<VOLUME> ===========================================================

template class Writer<Volume<int8_t> >;
template class Writer<Volume<int16_t> >;
template class Writer<Volume<int32_t> >;
template class Writer<Volume<uint8_t> >;
template class Writer<Volume<uint16_t> >;
template class Writer<Volume<uint32_t> >;
template class Writer<Volume<float> >;
template class Writer<Volume<double> >;
// template class Writer<Volume<AimsRGB> >;
// template class Writer<Volume<AimsRGBA> >;
// template class Writer<Volume<AimsHSV> >;

//=== WRITER<VOLUMEREF> ========================================================

template class Writer<VolumeRef<int8_t> >;
template class Writer<VolumeRef<int16_t> >;
template class Writer<VolumeRef<int32_t> >;
template class Writer<VolumeRef<uint8_t> >;
template class Writer<VolumeRef<uint16_t> >;
template class Writer<VolumeRef<uint32_t> >;
template class Writer<VolumeRef<float> >;
template class Writer<VolumeRef<double> >;
// template class Writer<VolumeRef<AimsRGB> >;
// template class Writer<VolumeRef<AimsRGBA> >;
// template class Writer<VolumeRef<AimsHSV> >;

//==============================================================================
//   F O R M A T D I C T I O N A R Y
//==============================================================================

//=== FORMATDICTIONARY<VOLUME> =================================================

template class FormatDictionary<Volume<int8_t> >;
template class FormatDictionary<Volume<int16_t> >;
template class FormatDictionary<Volume<int32_t> >;
template class FormatDictionary<Volume<uint8_t> >;
template class FormatDictionary<Volume<uint16_t> >;
template class FormatDictionary<Volume<uint32_t> >;
template class FormatDictionary<Volume<float> >;
template class FormatDictionary<Volume<double> >;
// template class FormatDictionary<Volume<AimsRGB> >;
// template class FormatDictionary<Volume<AimsRGBA> >;
// template class FormatDictionary<Volume<AimsHSV> >;

//=== FORMATDICTIONARY<VOLUMEREF> ==============================================

template class FormatDictionary<VolumeRef<int8_t> >;
template class FormatDictionary<VolumeRef<int16_t> >;
template class FormatDictionary<VolumeRef<int32_t> >;
template class FormatDictionary<VolumeRef<uint8_t> >;
template class FormatDictionary<VolumeRef<uint16_t> >;
template class FormatDictionary<VolumeRef<uint32_t> >;
template class FormatDictionary<VolumeRef<float> >;
template class FormatDictionary<VolumeRef<double> >;
// template class FormatDictionary<VolumeRef<AimsRGB> >;
// template class FormatDictionary<VolumeRef<AimsRGBA> >;
// template class FormatDictionary<VolumeRef<AimsHSV> >;

//==============================================================================
//   F O R M A T R E A D E R 
//==============================================================================

//=== FORMATREADER<VOLUME> =====================================================

template class FormatReader<Volume<int8_t> >;
template class FormatReader<Volume<int16_t> >;
template class FormatReader<Volume<int32_t> >;
template class FormatReader<Volume<uint8_t> >;
template class FormatReader<Volume<uint16_t> >;
template class FormatReader<Volume<uint32_t> >;
template class FormatReader<Volume<float> >;
template class FormatReader<Volume<double> >;
// template class FormatReader<Volume<AimsRGB> >;
// template class FormatReader<Volume<AimsRGBA> >;
// template class FormatReader<Volume<AimsHSV> >;

//=== FORMATREADER<VOLUMEREF> ==================================================

template class FormatReader<VolumeRef<int8_t> >;
template class FormatReader<VolumeRef<int16_t> >;
template class FormatReader<VolumeRef<int32_t> >;
template class FormatReader<VolumeRef<uint8_t> >;
template class FormatReader<VolumeRef<uint16_t> >;
template class FormatReader<VolumeRef<uint32_t> >;
template class FormatReader<VolumeRef<float> >;
template class FormatReader<VolumeRef<double> >;
// template class FormatReader<VolumeRef<AimsRGB> >;
// template class FormatReader<VolumeRef<AimsRGBA> >;
// template class FormatReader<VolumeRef<AimsHSV> >;

//=== VOLUMEFORMATREADER =======================================================

template class VolumeFormatReader<int8_t>;
template class VolumeFormatReader<int16_t>;
template class VolumeFormatReader<int32_t>;
template class VolumeFormatReader<uint8_t>;
template class VolumeFormatReader<uint16_t>;
template class VolumeFormatReader<uint32_t>;
template class VolumeFormatReader<float>;
template class VolumeFormatReader<double>;
// template class VolumeFormatReader<AimsRGB>;
// template class VolumeFormatReader<AimsRGBA>;
// template class VolumeFormatReader<AimsHSV>;

//=== VOLUMEREFFORMATREADER ====================================================

template class VolumeRefFormatReader<int8_t>;
template class VolumeRefFormatReader<int16_t>;
template class VolumeRefFormatReader<int32_t>;
template class VolumeRefFormatReader<uint8_t>;
template class VolumeRefFormatReader<uint16_t>;
template class VolumeRefFormatReader<uint32_t>;
template class VolumeRefFormatReader<float>;
template class VolumeRefFormatReader<double>;
// template class VolumeRefFormatReader<AimsRGB>;
// template class VolumeRefFormatReader<AimsRGBA>;
// template class VolumeRefFormatReader<AimsHSV>;

//==============================================================================
//   F O R M A T R E A D E R 
//==============================================================================

//=== VOLUMEFORMATWRITER =======================================================

template class VolumeFormatWriter<int8_t>;
template class VolumeFormatWriter<int16_t>;
template class VolumeFormatWriter<int32_t>;
template class VolumeFormatWriter<uint8_t>;
template class VolumeFormatWriter<uint16_t>;
template class VolumeFormatWriter<uint32_t>;
template class VolumeFormatWriter<float>;
template class VolumeFormatWriter<double>;
// template class VolumeFormatWriter<AimsRGB>;
// template class VolumeFormatWriter<AimsRGBA>;
// template class VolumeFormatWriter<AimsHSV>;

//=== VOLUMEREFFORMATWRITER ====================================================

template class VolumeRefFormatWriter<int8_t>;
template class VolumeRefFormatWriter<int16_t>;
template class VolumeRefFormatWriter<int32_t>;
template class VolumeRefFormatWriter<uint8_t>;
template class VolumeRefFormatWriter<uint16_t>;
template class VolumeRefFormatWriter<uint32_t>;
template class VolumeRefFormatWriter<float>;
template class VolumeRefFormatWriter<double>;
// template class VolumeRefFormatWriter<AimsRGB>;
// template class VolumeRefFormatWriter<AimsRGBA>;
// template class VolumeRefFormatWriter<AimsHSV>;

}