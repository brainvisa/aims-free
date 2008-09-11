/* Copyright (c) 1995-2005 CEA
 *
 *  This software and supporting documentation were developed by
 *      CEA/DSV/SHFJ
 *      4 place du General Leclerc
 *      91401 Orsay cedex
 *      France
 *
 * This software is governed by the CeCILL license version 2 under 
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the 
 * terms of the CeCILL license version 2 as circulated by CEA, CNRS
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
 * knowledge of the CeCILL license version 2 and that you accept its terms.
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
#include <aims/bucket/bucket.h>
#include <aims/math/dtitensor.h>
#include <aims/mesh/surface.h>
#include <aims/mesh/texture.h>
#include <aims/resampling/motion.h>
#include <graph/graph/graph.h>
#include <aims/roi/hie.h>

using namespace aims;
using namespace carto;
using namespace std;

template class Reader<AimsData<int8_t> >;
template class Reader<AimsData<uint8_t> >;
template class Reader<AimsData<int16_t> >;
template class Reader<AimsData<uint16_t> >;
template class Reader<AimsData<int32_t> >;
template class Reader<AimsData<uint32_t> >;
template class Reader<AimsData<float> >;
template class Reader<AimsData<double> >;
template class Reader<AimsData<cfloat> >;
template class Reader<AimsData<cdouble> >;
template class Reader<AimsData<AimsRGB> >;
template class Reader<AimsData<AimsRGBA> >;
template class Reader<AimsData<Point3df> >;
template class Reader<AimsData<Point3d> >;
template class Reader<AimsData<AimsVector<float,6> > >;

// ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
template class Reader<AimsData<char> >;
#endif
template class Reader<AimsData<long> >;
template class Reader<AimsData<unsigned long> >;

template class Reader<Volume<int8_t> >;
template class Reader<Volume<uint8_t> >;
template class Reader<Volume<int16_t> >;
template class Reader<Volume<uint16_t> >;
template class Reader<Volume<int32_t> >;
template class Reader<Volume<uint32_t> >;
template class Reader<Volume<float> >;
template class Reader<Volume<double> >;
template class Reader<Volume<cfloat> >;
template class Reader<Volume<cdouble> >;
template class Reader<Volume<AimsRGB> >;
template class Reader<Volume<AimsRGBA> >;
template class Reader<Volume<Point3df> >;
template class Reader<Volume<Point3d> >;
template class Reader<Volume<AimsVector<float,6> > >;
// ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
template class Reader<Volume<char> >;
#endif
template class Reader<Volume<long> >;
template class Reader<Volume<unsigned long> >;

template class Reader<VolumeRef<int8_t> >;
template class Reader<VolumeRef<uint8_t> >;
template class Reader<VolumeRef<int16_t> >;
template class Reader<VolumeRef<uint16_t> >;
template class Reader<VolumeRef<int32_t> >;
template class Reader<VolumeRef<uint32_t> >;
template class Reader<VolumeRef<float> >;
template class Reader<VolumeRef<double> >;
template class Reader<VolumeRef<cfloat> >;
template class Reader<VolumeRef<cdouble> >;
template class Reader<VolumeRef<AimsRGB> >;
template class Reader<VolumeRef<AimsRGBA> >;
template class Reader<VolumeRef<Point3df> >;
template class Reader<VolumeRef<Point3d> >;
template class Reader<VolumeRef<AimsVector<float,6> > >;
// ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
template class Reader<VolumeRef<char> >;
#endif
template class Reader<VolumeRef<long> >;
template class Reader<VolumeRef<unsigned long> >;

template class Reader<AimsBucket<Void> >;
template class Reader<BucketMap<Void> >;
template class Reader<AimsBucket<short> >;
template class Reader<BucketMap<short> >;
template class Reader<AimsBucket<vector<int8_t> > >;
template class Reader<AimsBucket<vector<uint8_t> > >;
template class Reader<AimsBucket<vector<int16_t> > >;
template class Reader<AimsBucket<vector<uint16_t> > >;
template class Reader<AimsBucket<vector<int32_t> > >;
template class Reader<AimsBucket<vector<uint32_t> > >;
template class Reader<AimsBucket<vector<float> > >;
template class Reader<AimsBucket<vector<double> > >;
template class Reader<AimsBucket<float> >;
template class Reader<BucketMap<float> >;
template class Reader<AimsBucket<double> >;
template class Reader<BucketMap<double> >;
template class Reader<AimsBucket<DtiTensor> >;
template class Reader<BucketMap<DtiTensor> >;

template class Reader<AimsTimeSurface<2,Void> >;
template class Reader<AimsSurfaceTriangle>;
template class Reader<AimsSurfaceFacet>;

template class Reader<Texture1d>;
template class Reader<Texture2d>;
template class Reader<TimeTexture<short> >;
template class Reader<TimeTexture<unsigned> >;
template class Reader<TimeTexture<int> >;
template class Reader<TimeTexture<Point2d> >;

template class Reader<Graph>;
template class Reader<Hierarchy>;
template class Reader<Motion>;
template class Reader<GenericObject>;
template class Reader<Object>;

