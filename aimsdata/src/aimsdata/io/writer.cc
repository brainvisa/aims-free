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
 *  Data writer class
 */

// this source is only here to force instanciation of some
// of the most useful Writer templates

#include <aims/config/aimsdata_config.h>
#include <aims/io/writer_d.h>
#include <aims/data/data.h>
#include <aims/rgb/rgb.h>
#include <aims/bucket/bucket.h>
#include <aims/math/dtitensor.h>
#include <aims/mesh/surface.h>
#include <aims/mesh/texture.h>
#include <graph/graph/graph.h>
#include <aims/resampling/motion.h>
#include <aims/roi/hie.h>

using namespace std;
using namespace carto;
using namespace aims;

template class Writer<AimsData<int8_t> >;
template class Writer<AimsData<uint8_t> >;
template class Writer<AimsData<int16_t> >;
template class Writer<AimsData<uint16_t> >;
template class Writer<AimsData<int32_t> >;
template class Writer<AimsData<uint32_t> >;
template class Writer<AimsData<float> >;
template class Writer<AimsData<double> >;
template class Writer<AimsData<cfloat> >;
template class Writer<AimsData<cdouble> >;
template class Writer<AimsData<AimsRGB> >;
template class Writer<AimsData<AimsRGBA> >;
template class Writer<AimsData<Point3df> >;
template class Writer<AimsData<Point3d> >;
template class Writer<AimsData<AimsVector<float,6> > >;

// ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
template class Writer<AimsData<char> >;
#endif
template class Writer<AimsData<long> >;
template class Writer<AimsData<unsigned long> >;

template class Writer<Volume<int8_t> >;
template class Writer<Volume<uint8_t> >;
template class Writer<Volume<int16_t> >;
template class Writer<Volume<uint16_t> >;
template class Writer<Volume<int32_t> >;
template class Writer<Volume<uint32_t> >;
template class Writer<Volume<float> >;
template class Writer<Volume<double> >;
template class Writer<Volume<cfloat> >;
template class Writer<Volume<cdouble> >;
template class Writer<Volume<AimsRGB> >;
template class Writer<Volume<AimsRGBA> >;
template class Writer<Volume<Point3df> >;
template class Writer<Volume<Point3d> >;
template class Writer<Volume<AimsVector<float,6> > >;
// ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
template class Writer<Volume<char> >;
#endif
template class Writer<Volume<long> >;
template class Writer<Volume<unsigned long> >;

template class Writer<VolumeRef<int8_t> >;
template class Writer<VolumeRef<uint8_t> >;
template class Writer<VolumeRef<int16_t> >;
template class Writer<VolumeRef<uint16_t> >;
template class Writer<VolumeRef<int32_t> >;
template class Writer<VolumeRef<uint32_t> >;
template class Writer<VolumeRef<float> >;
template class Writer<VolumeRef<double> >;
template class Writer<VolumeRef<cfloat> >;
template class Writer<VolumeRef<cdouble> >;
template class Writer<VolumeRef<AimsRGB> >;
template class Writer<VolumeRef<AimsRGBA> >;
template class Writer<VolumeRef<Point3df> >;
template class Writer<VolumeRef<Point3d> >;
template class Writer<VolumeRef<AimsVector<float,6> > >;
// ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
template class Writer<VolumeRef<char> >;
#endif
template class Writer<VolumeRef<long> >;
template class Writer<VolumeRef<unsigned long> >;

template class Writer<AimsBucket<Void> >;
template class Writer<BucketMap<Void> >;
template class Writer<AimsBucket<short> >;
template class Writer<BucketMap<short> >;
template class Writer<AimsBucket<vector<int8_t> > >;
template class Writer<AimsBucket<vector<uint8_t> > >;
template class Writer<AimsBucket<vector<int16_t> > >;
template class Writer<AimsBucket<vector<uint16_t> > >;
template class Writer<AimsBucket<vector<int32_t> > >;
template class Writer<AimsBucket<vector<uint32_t> > >;
template class Writer<AimsBucket<vector<float> > >;
template class Writer<AimsBucket<vector<double> > >;
template class Writer<AimsBucket<float> >;
template class Writer<BucketMap<float> >;
template class Writer<AimsBucket<double> >;
template class Writer<BucketMap<double> >;
template class Writer<AimsBucket<DtiTensor> >;
template class Writer<BucketMap<DtiTensor> >;

template class Writer<AimsTimeSurface<2,Void> >;
template class Writer<AimsSurfaceTriangle>;
template class Writer<AimsSurfaceFacet>;

template class Writer<Texture1d>;
template class Writer<Texture2d>;
template class Writer<TimeTexture<short> >;
template class Writer<TimeTexture<int> >;
template class Writer<TimeTexture<unsigned int> >;
template class Writer<TimeTexture<Point2d> >;

template class Writer<Graph>;
template class Writer<Hierarchy>;
template class Writer<Motion>;
template class Writer<GenericObject>;
template class Writer<Object>;


template bool 
GenericWriter::write< AimsData<int8_t> >( const AimsData<int8_t> &, 
					bool, const string * );
template bool 
GenericWriter::write< AimsData<uint8_t> >( const AimsData<uint8_t> &, 
					bool, const string * );
template bool 
GenericWriter::write< AimsData<int16_t> >( const AimsData<int16_t> &, 
					 bool, const string * );
template bool 
GenericWriter::write< AimsData<uint16_t> >( const AimsData<uint16_t> &, 
					  bool, const string * );
template bool 
GenericWriter::write< AimsData<int32_t> >( const AimsData<int32_t> &, 
					bool, const string * );
template bool 
GenericWriter::write< AimsData<uint32_t> >( const AimsData<uint32_t> &, 
					 bool, const string * );
template bool 
GenericWriter::write< AimsData<float> >( const AimsData<float> &, 
					 bool, const string * );
template bool 
GenericWriter::write< AimsData<double> >( const AimsData<double> &, 
					  bool, const string * );
template bool 
GenericWriter::write< AimsData<cfloat> >( const AimsData<cfloat> &, 
					  bool, const string * );
template bool 
GenericWriter::write< AimsData<cdouble> >( const AimsData<cdouble> &, 
					   bool, const string * );
template bool 
GenericWriter::write< AimsData<AimsRGB> >( const AimsData<AimsRGB> &, 
					   bool, const string * );
template bool 
GenericWriter::write< AimsData<AimsRGBA> >( const AimsData<AimsRGBA> &, 
					    bool, const string * );
template bool 
GenericWriter::write< AimsData<Point3df> >( const AimsData<Point3df> &, 
					    bool, const string * );
template bool 
GenericWriter::write< AimsData< AimsVector<float,6> > >( const AimsData< AimsVector<float,6> > &, 
							 bool, const string * );

// to be removed after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
template bool
GenericWriter::write< AimsData<char> >( const AimsData<char> &, 
					bool, const string * );
#endif
template bool
GenericWriter::write< AimsData<long> >( const AimsData<long> &, 
					bool, const string * );
template bool
GenericWriter::write< AimsData<unsigned long> >
( const AimsData<unsigned long> &, bool, const string * );


template bool
GenericWriter::write< Volume<int8_t> >( const Volume<int8_t> &, 
					bool, const string * );
template bool 
GenericWriter::write< Volume<uint8_t> >( const Volume<uint8_t> &, 
                                         bool, const string * );
template bool 
GenericWriter::write< Volume<int16_t> >( const Volume<int16_t> &, 
					 bool, const string * );
template bool 
GenericWriter::write< Volume<uint16_t> >( const Volume<uint16_t> &, 
					  bool, const string * );
template bool 
GenericWriter::write< Volume<int32_t> >( const Volume<int32_t> &, 
                                         bool, const string * );
template bool 
GenericWriter::write< Volume<uint32_t> >( const Volume<uint32_t> &, 
                                          bool, const string * );
template bool 
GenericWriter::write< Volume<float> >( const Volume<float> &, 
                                       bool, const string * );
template bool 
GenericWriter::write< Volume<double> >( const Volume<double> &, 
                                        bool, const string * );
template bool 
GenericWriter::write< Volume<cfloat> >( const Volume<cfloat> &, 
                                        bool, const string * );
template bool 
GenericWriter::write< Volume<cdouble> >( const Volume<cdouble> &, 
                                         bool, const string * );
template bool 
GenericWriter::write< Volume<AimsRGB> >( const Volume<AimsRGB> &, 
                                         bool, const string * );
template bool 
GenericWriter::write< Volume<AimsRGBA> >( const Volume<AimsRGBA> &, 
                                          bool, const string * );
template bool 
GenericWriter::write< Volume<Point3df> >( const Volume<Point3df> &, 
                                          bool, const string * );
template bool 
GenericWriter::write< Volume< AimsVector<float,6> > >
( const Volume< AimsVector<float,6> > &, bool, const string * );

// to be removed after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
template bool
GenericWriter::write< Volume<char> >( const Volume<char> &, 
                                      bool, const string * );
#endif
template bool
GenericWriter::write< Volume<long> >( const Volume<long> &, 
                                      bool, const string * );
template bool
GenericWriter::write< Volume<unsigned long> >( const Volume<unsigned long> &, 
                                               bool, const string * );


template bool 
GenericWriter::write< AimsBucket<Void> >( const AimsBucket<Void> &, 
					  bool, const string * );
template bool 
GenericWriter::write< BucketMap<Void> >( const BucketMap<Void> &, 
					 bool, const string * );
template bool 
GenericWriter::write< AimsBucket<short> >( const AimsBucket<short> &, 
					   bool, const string * );
template bool 
GenericWriter::write< BucketMap<short> >( const BucketMap<short> &, 
					  bool, const string * );
template bool 
GenericWriter::write< AimsBucket<float> >( const AimsBucket<float> &, 
					   bool, const string * );
template bool 
GenericWriter::write< BucketMap<float> >( const BucketMap<float> &, 
					  bool, const string * );
template bool 
GenericWriter::write< AimsBucket<DtiTensor> >( const AimsBucket<DtiTensor> &, 
					       bool, const string * );
template bool 
GenericWriter::write< BucketMap<DtiTensor> >( const BucketMap<DtiTensor> &, 
					      bool, const string * );


template bool
GenericWriter::write< AimsTimeSurface<2,Void> >
( const AimsTimeSurface<2,Void> &, bool, const string * );
template bool
GenericWriter::write< AimsSurfaceTriangle >( const AimsSurfaceTriangle &, 
					     bool, const string * );
template bool
GenericWriter::write< AimsSurfaceFacet >( const AimsSurfaceFacet &, 
                                          bool, const string * );

template bool
GenericWriter::write< Texture1d >( const Texture1d &, 
				   bool, const string * );
template bool
GenericWriter::write< Texture2d >( const Texture2d &, 
				   bool, const string * );
template bool
GenericWriter::write< TimeTexture<short> >( const TimeTexture<short> &, 
					    bool, const string * );
template bool
GenericWriter::write< TimeTexture<int> >( const TimeTexture<int> &, 
					  bool, const string * );
template bool
GenericWriter::write< TimeTexture<unsigned int> >( const TimeTexture<unsigned int> &,
					       bool, const string * );
template bool
GenericWriter::write< TimeTexture<Point2d> >( const TimeTexture<Point2d> &, 
					      bool, const string * );

template bool
GenericWriter::write< Graph >( const Graph &, 
			       bool, const string * );
template bool
    GenericWriter::write< Hierarchy >( const Hierarchy &,
                                       bool, const string * );
template bool
GenericWriter::write< Motion >( const Motion &, 
                                bool, const string * );
template bool
GenericWriter::write< GenericObject >( const GenericObject &,
                                       bool, const string * );
template bool
GenericWriter::write< Object >( const Object &,
                                bool, const string * );
