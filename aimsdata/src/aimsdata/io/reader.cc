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

/*
 *  Data reader class
 */
// this source is only here to force instanciation of some
// of the most useful Reader templates

#include <aims/config/aimsdata_config.h>
#include <aims/io/reader_d.h>
#include <aims/data/data.h>
#include <aims/rgb/rgb.h>
#include <aims/hsv/hsv.h>
#include <aims/bucket/bucket.h>
#include <aims/math/dtitensor.h>
#include <aims/mesh/surface.h>
#include <aims/mesh/texture.h>
#include <aims/resampling/motion.h>
#include <graph/graph/graph.h>
#include <aims/roi/hie.h>
#include <aims/sparsematrix/sparseordensematrix.h>

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
AIMS_INSTANTIATE_READER( AimsData<long> );
AIMS_INSTANTIATE_READER( AimsData<unsigned long> );

AIMS_INSTANTIATE_READER( Volume<int8_t> );
AIMS_INSTANTIATE_READER( Volume<uint8_t> );
AIMS_INSTANTIATE_READER( Volume<int16_t> );
AIMS_INSTANTIATE_READER( Volume<uint16_t> );
AIMS_INSTANTIATE_READER( Volume<int32_t> );
AIMS_INSTANTIATE_READER( Volume<uint32_t> );
AIMS_INSTANTIATE_READER( Volume<float> );
AIMS_INSTANTIATE_READER( Volume<double> );
AIMS_INSTANTIATE_READER( Volume<cfloat> );
AIMS_INSTANTIATE_READER( Volume<cdouble> );
AIMS_INSTANTIATE_READER( Volume<AimsRGB> );
AIMS_INSTANTIATE_READER( Volume<AimsRGBA> );
AIMS_INSTANTIATE_READER( Volume<AimsHSV> );
AIMS_INSTANTIATE_READER( Volume<Point3df> );
AIMS_INSTANTIATE_READER( Volume<Point3d> );
AIMS_INSTANTIATE_READER( Volume<AimsVector<float COMMA 6> > );
// ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
AIMS_INSTANTIATE_READER( Volume<char> );
#endif
AIMS_INSTANTIATE_READER( Volume<long> );
AIMS_INSTANTIATE_READER( Volume<unsigned long> );

AIMS_INSTANTIATE_READER( VolumeRef<int8_t> );
AIMS_INSTANTIATE_READER( VolumeRef<uint8_t> );
AIMS_INSTANTIATE_READER( VolumeRef<int16_t> );
AIMS_INSTANTIATE_READER( VolumeRef<uint16_t> );
AIMS_INSTANTIATE_READER( VolumeRef<int32_t> );
AIMS_INSTANTIATE_READER( VolumeRef<uint32_t> );
AIMS_INSTANTIATE_READER( VolumeRef<float> );
AIMS_INSTANTIATE_READER( VolumeRef<double> );
AIMS_INSTANTIATE_READER( VolumeRef<cfloat> );
AIMS_INSTANTIATE_READER( VolumeRef<cdouble> );
AIMS_INSTANTIATE_READER( VolumeRef<AimsRGB> );
AIMS_INSTANTIATE_READER( VolumeRef<AimsRGBA> );
AIMS_INSTANTIATE_READER( VolumeRef<AimsHSV> );
AIMS_INSTANTIATE_READER( VolumeRef<Point3df> );
AIMS_INSTANTIATE_READER( VolumeRef<Point3d> );
AIMS_INSTANTIATE_READER( VolumeRef<AimsVector<float COMMA 6> > );
// ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
AIMS_INSTANTIATE_READER( VolumeRef<char> );
#endif
AIMS_INSTANTIATE_READER( VolumeRef<long> );
AIMS_INSTANTIATE_READER( VolumeRef<unsigned long> );

AIMS_INSTANTIATE_READER( AimsBucket<Void> );
AIMS_INSTANTIATE_READER( BucketMap<Void> );
AIMS_INSTANTIATE_READER( AimsBucket<short> );
AIMS_INSTANTIATE_READER( BucketMap<short> );
AIMS_INSTANTIATE_READER( AimsBucket<uint16_t> );
AIMS_INSTANTIATE_READER( BucketMap<uint16_t> );
AIMS_INSTANTIATE_READER( AimsBucket<int32_t> );
AIMS_INSTANTIATE_READER( BucketMap<int32_t> );
AIMS_INSTANTIATE_READER( AimsBucket<uint32_t> );
AIMS_INSTANTIATE_READER( BucketMap<uint32_t> );
AIMS_INSTANTIATE_READER( AimsBucket<vector<int8_t> > );
AIMS_INSTANTIATE_READER( AimsBucket<vector<uint8_t> > );
AIMS_INSTANTIATE_READER( AimsBucket<vector<int16_t> > );
AIMS_INSTANTIATE_READER( AimsBucket<vector<uint16_t> > );
AIMS_INSTANTIATE_READER( AimsBucket<vector<int32_t> > );
AIMS_INSTANTIATE_READER( AimsBucket<vector<uint32_t> > );
AIMS_INSTANTIATE_READER( AimsBucket<vector<float> > );
AIMS_INSTANTIATE_READER( AimsBucket<vector<double> > );
AIMS_INSTANTIATE_READER( AimsBucket<float> );
AIMS_INSTANTIATE_READER( BucketMap<float> );
AIMS_INSTANTIATE_READER( AimsBucket<double> );
AIMS_INSTANTIATE_READER( BucketMap<double> );
AIMS_INSTANTIATE_READER( AimsBucket<DtiTensor> );
AIMS_INSTANTIATE_READER( BucketMap<DtiTensor> );

AIMS_INSTANTIATE_READER( AimsTimeSurface<2 COMMA Void> );
AIMS_INSTANTIATE_READER( AimsSurfaceTriangle);
AIMS_INSTANTIATE_READER( AimsSurfaceFacet);

AIMS_INSTANTIATE_READER( Texture1d);
AIMS_INSTANTIATE_READER( Texture2d);
AIMS_INSTANTIATE_READER( TimeTexture<double> );
AIMS_INSTANTIATE_READER( TimeTexture<short> );
AIMS_INSTANTIATE_READER( TimeTexture<unsigned> );
AIMS_INSTANTIATE_READER( TimeTexture<int> );
AIMS_INSTANTIATE_READER( TimeTexture<Point2d> );

AIMS_INSTANTIATE_READER( Graph);
AIMS_INSTANTIATE_READER( Hierarchy);
AIMS_INSTANTIATE_READER( Motion);
AIMS_INSTANTIATE_READER( GenericObject);
AIMS_INSTANTIATE_READER( Object);
AIMS_INSTANTIATE_READER( SparseMatrix);
AIMS_INSTANTIATE_READER( SparseOrDenseMatrix);
