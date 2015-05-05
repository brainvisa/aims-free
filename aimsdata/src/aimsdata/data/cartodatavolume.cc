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

#include <aims/data/data.h>

// compilation of some Volume classes on Aims types

#include <cartodata/volume/volume_d.h>
#include <cartodata/volume/volumeview.h>
#include <aims/rgb/rgb.h>
#include <aims/hsv/hsv.h>
#include <aims/io/datatypecode.h>
#include <aims/vector/vector.h>
#include <aims/math/dtitensor.h>
#include <aims/data/volumeoperators_aims.h>
#include <cartodata/volume/volumeutil_d.h>
#include <cartobase/object/object_d.h>
#include <set>
#include <map>

using namespace carto;

#define instantiate_volutil( T ) \
template class VolumeUtil<T>; \
template VolumeRef<T> \
VolumeUtil<T>::apply( Scaler<T, double>, const VolumeRef<T> & ); \
template void \
VolumeUtil<T>::selfApply( Scaler<T, double>, VolumeRef<T> & ); \
template VolumeRef<T> \
VolumeUtil<T>::apply( Scaler<T, float>, const VolumeRef<T> & ); \
template void \
VolumeUtil<T>::selfApply( Scaler<T, float>, VolumeRef<T> & ); \
template VolumeRef<T> \
VolumeUtil<T>::apply( Scaler<T, long>, const VolumeRef<T> & ); \
template void \
VolumeUtil<T>::selfApply( Scaler<T, long>, VolumeRef<T> & ); \
template VolumeRef<T> \
VolumeUtil<T>::apply( Divider<T, double>, const VolumeRef<T> & ); \
template void \
VolumeUtil<T>::selfApply( Divider<T, double>, VolumeRef<T> & ); \
template VolumeRef<T> \
VolumeUtil<T>::apply( Divider<T, long>, const VolumeRef<T> & ); \
template void \
VolumeUtil<T>::selfApply( Divider<T, long>, VolumeRef<T> & );

#define instantiate_volutil2( T, BinaryFunction ) \
template VolumeRef<T> \
VolumeUtil<T>::apply( BinaryFunction, const VolumeRef<T> &, \
                      const VolumeRef<T> & ); \
template void \
VolumeUtil<T>::selfApply( BinaryFunction, VolumeRef<T> &, \
                          const VolumeRef<T> & ); \
template VolumeRef<T> \
VolumeUtil<T>::apply( UnaryFromConstantBinaryFunctor<T, \
                      BinaryFunction >, const VolumeRef<T> & ); \
template void \
VolumeUtil<T>::selfApply( UnaryFromConstantBinaryFunctor<T, \
                          BinaryFunction >, VolumeRef<T> & ); \
template VolumeRef<T> \
VolumeUtil<T>::apply( UnaryFromConstantBinaryFunctor2<T, \
                      BinaryFunction >, const VolumeRef<T> & ); \


namespace carto
{

  template<> template<> VolumeRef<AimsRGB> 
  VolumeUtil<AimsRGB>::apply( std::negate<AimsRGB>, 
                              const VolumeRef<AimsRGB> & o )
  {
    return apply( Scaler<AimsRGB, long>( -1 ), o );
  }

  template<> template<> VolumeRef<AimsRGBA> 
  VolumeUtil<AimsRGBA>::apply( std::negate<AimsRGBA>, 
                               const VolumeRef<AimsRGBA> & o )
  {
    return apply( Scaler<AimsRGBA, long>( -1 ), o );
  }

  template<> template<> VolumeRef<AimsHSV> 
  VolumeUtil<AimsHSV>::apply( std::negate<AimsHSV>, 
                              const VolumeRef<AimsHSV> & o )
  {
    return apply( Scaler<AimsHSV, long>( -1 ), o );
  }
}

template class carto::VolumeProxy< AimsRGB >;
template class carto::Volume< AimsRGB >;
instantiate_volutil( AimsRGB )
instantiate_volutil2( AimsRGB, std::plus<AimsRGB> )
instantiate_volutil2( AimsRGB, std::minus<AimsRGB> )

template class carto::VolumeProxy< AimsRGBA >;
template class carto::Volume< AimsRGBA >;
instantiate_volutil( AimsRGBA )
instantiate_volutil2( AimsRGBA, std::plus<AimsRGBA> )
instantiate_volutil2( AimsRGBA, std::minus<AimsRGBA> )

template class carto::VolumeProxy< AimsHSV >;
template class carto::Volume< AimsHSV >;
instantiate_volutil( AimsHSV )
instantiate_volutil2( AimsHSV, std::plus<AimsHSV> )
instantiate_volutil2( AimsHSV, std::minus<AimsHSV> )

template class carto::VolumeProxy< Point3df >;
template class carto::Volume< Point3df >;
// template class VolumeUtil<Point3df>;
template VolumeRef<Point3df> 
VolumeUtil<Point3df>::apply( std::negate<Point3df>, 
                             const VolumeRef<Point3df> & );
instantiate_volutil( Point3df )
instantiate_volutil2( Point3df, std::plus<Point3df> )
instantiate_volutil2( Point3df, std::minus<Point3df> )

template class carto::VolumeProxy< Point3d >;
template class carto::Volume< Point3d >;
// template class VolumeUtil<Point3d>;
template VolumeRef<Point3d>
VolumeUtil<Point3d>::apply( std::negate<Point3d>, 
                            const VolumeRef<Point3d> & );
instantiate_volutil( Point3d )
instantiate_volutil2( Point3d, std::plus<Point3d> )
instantiate_volutil2( Point3d, std::minus<Point3d> )

template class carto::VolumeProxy< Point2d >;
template class carto::Volume< Point2d >;
// template class VolumeUtil<Point2d>;
template VolumeRef<Point2d> 
VolumeUtil<Point2d>::apply( std::negate<Point2d>, 
                            const VolumeRef<Point2d> & );
instantiate_volutil( Point2d )
instantiate_volutil2( Point2d, std::plus<Point2d> )
instantiate_volutil2( Point2d, std::minus<Point2d> )

typedef AimsVector<float,6> vectorf6;
template class carto::VolumeProxy< vectorf6 >;
template class carto::Volume< vectorf6 >;
// template class VolumeUtil<vectorf6>;
template VolumeRef<vectorf6> 
VolumeUtil<vectorf6>::apply( std::negate<vectorf6>, 
                             const VolumeRef<vectorf6> & );
instantiate_volutil( vectorf6 )
instantiate_volutil2( vectorf6, std::plus<vectorf6> )
instantiate_volutil2( vectorf6, std::minus<vectorf6> )

template class carto::VolumeProxy< DtiTensor * >;
template class carto::Volume< DtiTensor * >;

template class carto::VolumeProxy< std::set<float> >;
template class carto::Volume< std::set<float> >;

template class carto::VolumeProxy< std::map<int, float> >;
template class carto::Volume< std::map<int, float> >;

namespace carto
{
  INSTANTIATE_GENERIC_OBJECT_TYPE( VolumeRef< Point3df > )
  INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr<Volume< Point3df > > )

}

