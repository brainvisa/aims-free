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
#include <cartodata/io/volumeutilio_d.h>
//--- soma-io ----------------------------------------------------------------
#include <soma-io/config/soma_config.h>
#include <soma-io/io/reader_d.h>
#include <soma-io/io/writer_d.h>
#include <soma-io/io/formatdictionary_d.h>
#include <soma-io/reader/formatreader_d.h>
#include <soma-io/image/imagereader_d.h>
#include <soma-io/image/imagewriter_d.h>
//--- cartobase --------------------------------------------------------------
#include <cartobase/smart/rcptr.h>
//----------------------------------------------------------------------------
#include <aims/vector/vector.h>
// #include <aims/math/dtitensor.h>

using namespace soma;
using namespace carto;

namespace
{
  typedef AimsVector<float,6> vectorf6;
}


namespace soma {

//============================================================================
//   R E A D E R
//============================================================================

//=== READER<VOLUME> =========================================================

template class Reader<Volume<Point3df> >;
template class Reader<Volume<Point3d> >;
template class Reader<Volume<Point2d> >;
template class Reader<Volume<vectorf6> >;
// template class Reader<Volume<DtiTensor*> >;

//=== READER<VOLUMEREF> ======================================================

template class Reader<VolumeRef<Point3df> >;
template class Reader<VolumeRef<Point3d> >;
template class Reader<VolumeRef<Point2d> >;
template class Reader<VolumeRef<vectorf6> >;
// template class Reader<VolumeRef<DtiTensor*> >;

//============================================================================
//   W R I T E R
//============================================================================

//=== WRITER<VOLUME> =========================================================

template class Writer<Volume<Point3df> >;
template class Writer<Volume<Point3d> >;
template class Writer<Volume<Point2d> >;
template class Writer<Volume<vectorf6> >;
// template class Writer<Volume<DtiTensor*> >;

//=== WRITER<VOLUMEREF> ======================================================

template class Writer<VolumeRef<Point3df> >;
template class Writer<VolumeRef<Point3d> >;
template class Writer<VolumeRef<Point2d> >;
template class Writer<VolumeRef<vectorf6> >;
// template class Writer<VolumeRef<DtiTensor*> >;

//============================================================================
//   F O R M A T D I C T I O N A R Y
//============================================================================

//=== FORMATDICTIONARY<VOLUME> ===============================================

template class FormatDictionary<Volume<Point3df> >;
template class FormatDictionary<Volume<Point3d> >;
template class FormatDictionary<Volume<Point2d> >;
template class FormatDictionary<Volume<vectorf6> >;
// template class FormatDictionary<Volume<DtiTensor*> >;

//=== FORMATDICTIONARY<VOLUMEREF> ============================================

template class FormatDictionary<VolumeRef<Point3df> >;
template class FormatDictionary<VolumeRef<Point3d> >;
template class FormatDictionary<VolumeRef<Point2d> >;
template class FormatDictionary<VolumeRef<vectorf6> >;
// template class FormatDictionary<VolumeRef<DtiTensor*> >;

//============================================================================
//   F O R M A T R E A D E R
//============================================================================

//=== FORMATREADER<VOLUME> ===================================================

template class FormatReader<Volume<Point3df> >;
template class FormatReader<Volume<Point3d> >;
template class FormatReader<Volume<Point2d> >;
template class FormatReader<Volume<vectorf6> >;
// template class FormatReader<Volume<DtiTensor*> >;

//=== FORMATREADER<VOLUMEREF> ================================================

template class FormatReader<VolumeRef<Point3df> >;
template class FormatReader<VolumeRef<Point3d> >;
template class FormatReader<VolumeRef<Point2d> >;
template class FormatReader<VolumeRef<vectorf6> >;
// template class FormatReader<VolumeRef<DtiTensor*> >;

//=== VOLUMEFORMATREADER =====================================================

template class VolumeFormatReader<Point3df>;
template class VolumeFormatReader<Point3d>;
template class VolumeFormatReader<Point2d>;
template class VolumeFormatReader<vectorf6>;
// template class VolumeFormatReader<DtiTensor*>;

//=== VOLUMEREFFORMATREADER ==================================================

template class VolumeRefFormatReader<Point3df>;
template class VolumeRefFormatReader<Point3d>;
template class VolumeRefFormatReader<Point2d>;
template class VolumeRefFormatReader<vectorf6>;
// template class VolumeRefFormatReader<DtiTensor*>;

//============================================================================
//   F O R M A T R E A D E R
//============================================================================

//=== VOLUMEFORMATWRITER =====================================================

template class VolumeFormatWriter<Point3df>;
template class VolumeFormatWriter<Point3d>;
template class VolumeFormatWriter<Point2d>;
template class VolumeFormatWriter<vectorf6>;
// template class VolumeFormatWriter<DtiTensor*>;

//=== VOLUMEREFFORMATWRITER ==================================================

template class VolumeRefFormatWriter<Point3df>;
template class VolumeRefFormatWriter<Point3d>;
template class VolumeRefFormatWriter<Point2d>;
template class VolumeRefFormatWriter<vectorf6>;
// template class VolumeRefFormatWriter<DtiTensor*>;

//============================================================================
//   V O L U M E U T I L I O
//============================================================================

template class VolumeUtilIO<Point3df>;
template class VolumeUtilIO<Point3d>;
template class VolumeUtilIO<Point2d>;
template class VolumeUtilIO<vectorf6>;
// template class VolumeUtilIO<DtiTensor*>;

//============================================================================
//   I M A G E R E A D E R
//============================================================================

template class ImageReader<Point3df>;
template class ImageReader<Point3d>;
template class ImageReader<Point2d>;
template class ImageReader<AimsVector<float, 6> >;

template class ImageWriter<Point3df>;
template class ImageWriter<Point3d>;
template class ImageWriter<Point2d>;
template class ImageWriter<AimsVector<float, 6> >;

}
