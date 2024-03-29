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
#include <cartodata/volume/volumebase_d.h>
#include <cartodata/volume/volumeref_d.h>
#include <cartodata/volume/volumeutil_d.h>
//--- cartobase --------------------------------------------------------------
#include <cartobase/object/object_d.h>
#include <cartobase/type/voxelrgb.h>
#include <cartobase/type/voxelrgba.h>
#include <cartobase/type/voxelhsv.h>
//--- std --------------------------------------------------------------------
#include <functional>
//----------------------------------------------------------------------------

using namespace carto;

namespace
{
  typedef AimsVector<float,6> vectorf6;
}

namespace carto
{

template class VolumeProxy< std::map<int, float> >;
template class VolumeProxy< VoxelRGB >;
template class VolumeProxy< VoxelRGBA >;
template class VolumeProxy< VoxelHSV >;
template class VolumeProxy< Point3df >;
template class VolumeProxy< Point3d >;
template class VolumeProxy< Point3dd >;
template class VolumeProxy< Point2d >;
template class VolumeProxy< vectorf6 >;

template class Volume< std::map<int, float> >;
template class Volume< VoxelRGB >;
template class Volume< VoxelRGBA >;
template class Volume< VoxelHSV >;
template class Volume< Point3df >;
template class Volume< Point3d >;
template class Volume< Point3dd >;
template class Volume< Point2d >;
template class Volume< vectorf6 >;

template class VolumeRef< std::map<int, float> >;
template class VolumeRef< VoxelRGB >;
template class VolumeRef< VoxelRGBA >;
template class VolumeRef< VoxelHSV >;
template class VolumeRef< Point3df >;
template class VolumeRef< Point3d >;
template class VolumeRef< Point3dd >;
template class VolumeRef< Point2d >;
template class VolumeRef< vectorf6 >;

template class Creator<Volume< std::map<int, float> > >;
template class Creator< Volume< VoxelRGB > >;
template class Creator< Volume< VoxelRGBA > >;
template class Creator< Volume< VoxelHSV > >;
template class Creator< Volume< Point3df > >;
template class Creator< Volume< Point3d > >;
template class Creator< Volume< Point3dd > >;
template class Creator< Volume< Point2d > >;
template class Creator< Volume< vectorf6 > >;

template class Creator<VolumeRef< std::map<int, float> > >;
template class Creator< VolumeRef< VoxelRGB > >;
template class Creator< VolumeRef< VoxelRGBA > >;
template class Creator< VolumeRef< VoxelHSV > >;
template class Creator< VolumeRef< Point3df > >;
template class Creator< VolumeRef< Point3d > >;
template class Creator< VolumeRef< Point3dd > >;
template class Creator< VolumeRef< Point2d > >;
template class Creator< VolumeRef< vectorf6 > >;

template class VolumeUtilBase< std::map<int, float>, false >;
template class VolumeUtilBase< VoxelRGB, false >;
template class VolumeUtilBase< VoxelRGBA, false >;
template class VolumeUtilBase< VoxelHSV, false >;
template class VolumeUtilBase< Point3df, false >;
template class VolumeUtilBase< Point3d, false >;
template class VolumeUtilBase< Point3dd, false >;
template class VolumeUtilBase< Point2d, false >;
template class VolumeUtilBase< vectorf6, false >;

template class VolumeUtil< std::map<int, float> >;
template class VolumeUtil< VoxelRGB >;
template class VolumeUtil< VoxelRGBA >;
template class VolumeUtil< VoxelHSV >;
template class VolumeUtil< Point3df >;
template class VolumeUtil< Point3d >;
template class VolumeUtil< Point3dd >;
template class VolumeUtil< Point2d >;
template class VolumeUtil< vectorf6 >;

template VolumeRef< std::map<int, float> > transpose<std::map<int, float> >( const Volume< std::map<int, float> > & );
template VolumeRef< VoxelRGB > transpose<VoxelRGB>( const Volume< VoxelRGB > & );
template VolumeRef< VoxelRGBA > transpose<VoxelRGBA>( const Volume< VoxelRGBA > & );
template VolumeRef< VoxelHSV > transpose<VoxelHSV>( const Volume< VoxelHSV > & );
template VolumeRef< Point3df > transpose<Point3df>( const Volume< Point3df > & );
template VolumeRef< Point3d > transpose<Point3d>( const Volume< Point3d > & );
template VolumeRef< Point3dd > transpose<Point3dd>( const Volume< Point3dd > & );
template VolumeRef< Point2d > transpose<Point2d>( const Volume< Point2d > & );
template VolumeRef< vectorf6 > transpose<vectorf6>( const Volume< vectorf6 > & );

template VolumeRef< std::map<int, float> > transpose<std::map<int, float> >( const rc_ptr<Volume< std::map<int, float> > > &, bool );
template VolumeRef< VoxelRGB > transpose<VoxelRGB>( const rc_ptr<Volume< VoxelRGB > > &, bool );
template VolumeRef< VoxelRGBA > transpose<VoxelRGBA>( const rc_ptr<Volume< VoxelRGBA > > &, bool );
template VolumeRef< VoxelHSV > transpose<VoxelHSV>( const rc_ptr<Volume< VoxelHSV > > &, bool );
template VolumeRef< Point3df > transpose<Point3df>( const rc_ptr<Volume< Point3df > > &, bool );
template VolumeRef< Point3d > transpose<Point3d>( const rc_ptr<Volume< Point3d > > &, bool );
template VolumeRef< Point3dd > transpose<Point3dd>( const rc_ptr<Volume< Point3dd > > &, bool );
template VolumeRef< Point2d > transpose<Point2d>( const rc_ptr<Volume< Point2d > >&, bool );
template VolumeRef< vectorf6 > transpose<vectorf6>( const rc_ptr<Volume< vectorf6 > > &, bool );

} // namespace carto

// utilities
#if 0

#define instantiate_volutil( T ) \
template class VolumeUtil<T>; \
template VolumeRef<T> \
VolumeUtil<T>::apply( Scaler<T,double>, const VolumeRef<T> & ); \
template void \
VolumeUtil<T>::selfApply( Scaler<T,double>, VolumeRef<T> & ); \
template VolumeRef<T> \
VolumeUtil<T>::apply( Scaler<T,float>, const VolumeRef<T> & ); \
template void \
VolumeUtil<T>::selfApply( Scaler<T,float>, VolumeRef<T> & ); \
template VolumeRef<T> \
VolumeUtil<T>::apply( Scaler<T,long>, const VolumeRef<T> & ); \
template void \
VolumeUtil<T>::selfApply( Scaler<T,long>, VolumeRef<T> & ); \
template VolumeRef<T> \
VolumeUtil<T>::apply( std::negate<T>, const VolumeRef<T> & ); \
template VolumeRef<T> \
VolumeUtil<T>::apply( Divider<T,double>, const VolumeRef<T> & ); \
template void \
VolumeUtil<T>::selfApply( Divider<T,double>, VolumeRef<T> & ); \
template VolumeRef<T> \
VolumeUtil<T>::apply( Divider<T,float>, const VolumeRef<T> & ); \
template void \
VolumeUtil<T>::selfApply( Divider<T,float>, VolumeRef<T> & ); \
template VolumeRef<T> \
VolumeUtil<T>::apply( Divider<T,long>, const VolumeRef<T> & ); \
template void \
VolumeUtil<T>::selfApply( Divider<T,long>, VolumeRef<T> & );

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

instantiate_volutil( VoxelRGB )
instantiate_volutil2( VoxelRGB, std::plus<VoxelRGB> )
instantiate_volutil2( VoxelRGB, std::minus<VoxelRGB> )
instantiate_volutil2( VoxelRGB, std::multiplies<VoxelRGB> )
instantiate_volutil2( VoxelRGB, std::divides<VoxelRGB> )

instantiate_volutil( VoxelRGBA )
instantiate_volutil2( VoxelRGBA, std::plus<VoxelRGBA> )
instantiate_volutil2( VoxelRGBA, std::minus<VoxelRGBA> )
instantiate_volutil2( VoxelRGBA, std::multiplies<VoxelRGBA> )
instantiate_volutil2( VoxelRGBA, std::divides<VoxelRGBA> )

instantiate_volutil( VoxelHSV )
instantiate_volutil2( VoxelHSV, std::plus<VoxelHSV> )
instantiate_volutil2( VoxelHSV, std::minus<VoxelHSV> )
instantiate_volutil2( VoxelHSV, std::multiplies<VoxelHSV> )
instantiate_volutil2( VoxelHSV, std::divides<VoxelHSV> )

instantiate_volutil( Point3df )
instantiate_volutil2( Point3df, std::plus<Point3df> )
instantiate_volutil2( Point3df, std::minus<Point3df> )

instantiate_volutil( Point3d )
instantiate_volutil2( Point3d, std::plus<Point3d> )
instantiate_volutil2( Point3d, std::minus<Point3d> )

instantiate_volutil( Point3dd )
instantiate_volutil2( Point3dd, std::plus<Point3dd> )
instantiate_volutil2( Point3dd, std::minus<Point3dd> )

instantiate_volutil( Point2d )
instantiate_volutil2( Point2d, std::plus<Point2d> )
instantiate_volutil2( Point2d, std::minus<Point2d> )

instantiate_volutil( vectorf6 )
instantiate_volutil2( vectorf6, std::plus<vectorf6> )
instantiate_volutil2( vectorf6, std::minus<vectorf6> )

#endif

namespace carto
{

  INSTANTIATE_GENERIC_OBJECT_TYPE( VolumeRef< soma::VoxelRGB > )
  INSTANTIATE_GENERIC_OBJECT_TYPE( VolumeRef< soma::VoxelRGBA > )
  INSTANTIATE_GENERIC_OBJECT_TYPE( VolumeRef< soma::VoxelHSV > )
  INSTANTIATE_GENERIC_OBJECT_TYPE( VolumeRef< Point3df > )
  INSTANTIATE_GENERIC_OBJECT_TYPE( VolumeRef< Point3d > )
  INSTANTIATE_GENERIC_OBJECT_TYPE( VolumeRef< Point3dd > )
  INSTANTIATE_GENERIC_OBJECT_TYPE( VolumeRef< Point2d > )
  INSTANTIATE_GENERIC_OBJECT_TYPE( VolumeRef< vectorf6 > )

  INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr<Volume< soma::VoxelRGB > > )
  INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr<Volume< soma::VoxelRGBA > > )
  INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr<Volume< soma::VoxelHSV > > )
  INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr<Volume< Point3df > > )
  INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr<Volume< Point3d > > )
  INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr<Volume< Point3dd > > )
  INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr<Volume< Point2d > > )
  INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr<Volume< vectorf6 > > )


  //--------------------------------------------------------------------------
  // Copy functions
  //--------------------------------------------------------------------------

#if 0

#define instantiate_copy( T )                                                \
  template void transfer( const carto::VolumeRef<T> & src,                   \
                          carto::VolumeRef<T> & dst );                       \
  template carto::VolumeRef<T> deepcopy( const carto::VolumeRef<T> & src,    \
                                         bool copy_full_structure = true );  \
  template carto::VolumeRef<T> copy( const carto::VolumeRef<T> & src );

#define COMMA ,

  instantiate_copy( std::map<int COMMA float> )
  instantiate_copy( VoxelRGB )
  instantiate_copy( VoxelRGBA )
  instantiate_copy( VoxelHSV )

#endif

} // namespace carto

