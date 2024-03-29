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

template class VolumeProxy< bool >;
template class VolumeProxy< int8_t >;
template class VolumeProxy< uint8_t >;
// ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
template class VolumeProxy< char >;
#endif
template class VolumeProxy< int16_t >;
template class VolumeProxy< uint16_t >;
template class VolumeProxy< int32_t >;
template class VolumeProxy< uint32_t >;
template class VolumeProxy< int64_t >;
template class VolumeProxy< uint64_t >;
template class VolumeProxy< float >;
template class VolumeProxy< double >;
template class VolumeProxy< cfloat >;
template class VolumeProxy< cdouble >;
#ifdef CARTO_LONG_IS_DISTINCT
template class VolumeProxy< long >;
template class VolumeProxy< unsigned long >;
#endif

template class Volume< bool >;
template class Volume< int8_t >;
template class Volume< uint8_t >;
// ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
template class Volume< char >;
#endif
template class Volume< int16_t >;
template class Volume< uint16_t >;
template class Volume< int32_t >;
template class Volume< uint32_t >;
template class Volume< int64_t >;
template class Volume< uint64_t >;
template class Volume< float >;
template class Volume< double >;
template class Volume< cfloat >;
template class Volume< cdouble >;
#ifdef CARTO_LONG_IS_DISTINCT
template class Volume< long >;
template class Volume< unsigned long >;
#endif

// Mac compiler does not seem to allow indirect instanciation
template class VolumeRef< bool >;
template class VolumeRef< int8_t >;
template class VolumeRef< uint8_t >;
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
template class VolumeRef< char >;
#endif
template class VolumeRef< int16_t >;
template class VolumeRef< uint16_t >;
template class VolumeRef< int32_t >;
template class VolumeRef< uint32_t >;
template class VolumeRef< int64_t >;
template class VolumeRef< uint64_t >;
template class VolumeRef< float >;
template class VolumeRef< double >;
template class VolumeRef< cfloat >;
template class VolumeRef< cdouble >;
#ifdef CARTO_LONG_IS_DISTINCT
template class VolumeRef< long >;
template class VolumeRef< unsigned long >;
#endif

template class Creator<Volume< bool > >;
template class Creator<Volume< int8_t > >;
template class Creator<Volume< uint8_t > >;
// ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
template class Creator<Volume< char > >;
#endif
template class Creator<Volume< int16_t > >;
template class Creator<Volume< uint16_t > >;
template class Creator<Volume< int32_t > >;
template class Creator<Volume< uint32_t > >;
template class Creator<Volume< int64_t> >;
template class Creator<Volume< uint64_t > >;
template class Creator<Volume< float > >;
template class Creator<Volume< double > >;
template class Creator<Volume< cfloat > >;
template class Creator<Volume< cdouble > >;
#ifdef CARTO_LONG_IS_DISTINCT
template class Creator< Volume< long > >;
template class Creator< Volume< unsigned long > >;
#endif

template class Creator<VolumeRef< bool > >;
template class Creator<VolumeRef< int8_t > >;
template class Creator<VolumeRef< uint8_t > >;
// ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
template class Creator<VolumeRef< char > >;
#endif
template class Creator<VolumeRef< int16_t > >;
template class Creator<VolumeRef< uint16_t > >;
template class Creator<VolumeRef< int32_t > >;
template class Creator<VolumeRef< uint32_t > >;
template class Creator<VolumeRef< int64_t > >;
template class Creator<VolumeRef< uint64_t > >;
template class Creator<VolumeRef< float > >;
template class Creator<VolumeRef< double > >;
template class Creator<VolumeRef< cfloat > >;
template class Creator<VolumeRef< cdouble > >;
#ifdef CARTO_LONG_IS_DISTINCT
template class Creator<VolumeRef< long > >;
template class Creator<VolumeRef< unsigned long > >;
#endif

template class VolumeUtilBase< bool, true >;
template class VolumeUtilBase< int8_t, true >;
template class VolumeUtilBase< uint8_t, true >;
// ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
template class VolumeUtilBase< char, true >;
#endif
template class VolumeUtilBase< int16_t, true >;
template class VolumeUtilBase< uint16_t, true >;
template class VolumeUtilBase< int32_t, true >;
template class VolumeUtilBase< uint32_t, true >;
template class VolumeUtilBase< int64_t, true >;
template class VolumeUtilBase< uint64_t, true >;
template class VolumeUtilBase< float, true >;
template class VolumeUtilBase< double, true >;
template class VolumeUtilBase< cfloat, false >;
template class VolumeUtilBase< cdouble, false >;
#ifdef CARTO_LONG_IS_DISTINCT
template class VolumeUtilBase< long, true >;
template class VolumeUtilBase< unsigned long, true >;
#endif

template class VolumeUtil< bool >;
template class VolumeUtil< int8_t >;
template class VolumeUtil< uint8_t >;
// ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
template class VolumeUtil< char >;
#endif
template class VolumeUtil< int16_t >;
template class VolumeUtil< uint16_t >;
template class VolumeUtil< int32_t >;
template class VolumeUtil< uint32_t >;
template class VolumeUtil< int64_t >;
template class VolumeUtil< uint64_t >;
template class VolumeUtil< float >;
template class VolumeUtil< double >;
template class VolumeUtil< cfloat >;
template class VolumeUtil< cdouble >;
#ifdef CARTO_LONG_IS_DISTINCT
template class VolumeUtil< long >;
template class VolumeUtil< unsigned long >;
#endif

template VolumeRef< bool > matrix_product<bool>(
  const Volume< bool > &, const Volume< bool > & );
template VolumeRef< int8_t > matrix_product(
  const Volume< int8_t > &, const Volume< int8_t > & );
template VolumeRef< uint8_t > matrix_product(
  const Volume< uint8_t > &, const Volume< uint8_t > & );
// ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
template VolumeRef< char > matrix_product(
  const Volume< char > &, const Volume< char > & );
#endif
template VolumeRef< int16_t > matrix_product(
  const Volume< int16_t > &, const Volume< int16_t > & );
template VolumeRef< uint16_t > matrix_product(
  const Volume< uint16_t > &, const Volume< uint16_t > & );
template VolumeRef< int32_t > matrix_product(
  const Volume< int32_t > &, const Volume< int32_t > & );
template VolumeRef< uint32_t > matrix_product(
  const Volume< uint32_t > &, const Volume< uint32_t > & );
template VolumeRef< int64_t > matrix_product(
  const Volume< int64_t > &, const Volume< int64_t > & );
template VolumeRef< uint64_t > matrix_product(
  const Volume< uint64_t > &, const Volume< uint64_t > & );
template VolumeRef< float > matrix_product(
  const Volume< float > &, const Volume< float > & );
template VolumeRef< double > matrix_product(
  const Volume< double > &, const Volume< double > & );
#ifdef CARTO_LONG_IS_DISTINCT
template VolumeRef< long > matrix_product(
  const Volume< long > &, const Volume< long > & );
template VolumeRef< unsigned long > matrix_product(
  const Volume< unsigned long > &, const Volume< unsigned long > & );
#endif

template VolumeRef< bool > matrix_product<bool>(
  const rc_ptr<Volume< bool > > &, const rc_ptr<Volume< bool > > & );
template VolumeRef< int8_t > matrix_product(
  const rc_ptr<Volume< int8_t > > &, const rc_ptr<Volume< int8_t > > & );
template VolumeRef< uint8_t > matrix_product(
  const rc_ptr<Volume< uint8_t > > &, const rc_ptr<Volume< uint8_t > > & );
// ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
template VolumeRef< char > matrix_product(
  const rc_ptr<Volume< char > > &, const rc_ptr<Volume< char > > & );
#endif
template VolumeRef< int16_t > matrix_product(
  const rc_ptr<Volume< int16_t > > &, const rc_ptr<Volume< int16_t > > & );
template VolumeRef< uint16_t > matrix_product(
  const rc_ptr<Volume< uint16_t > > &, const rc_ptr<Volume< uint16_t > > & );
template VolumeRef< int32_t > matrix_product(
  const rc_ptr<Volume< int32_t > > &, const rc_ptr<Volume< int32_t > > & );
template VolumeRef< uint32_t > matrix_product(
  const rc_ptr<Volume< uint32_t > > &, const rc_ptr<Volume< uint32_t > > & );
template VolumeRef< int64_t > matrix_product(
  const rc_ptr<Volume< int64_t > > &, const rc_ptr<Volume< int64_t > > & );
template VolumeRef< uint64_t > matrix_product(
  const rc_ptr<Volume< uint64_t > > &, const rc_ptr<Volume< uint64_t > > & );
template VolumeRef< float > matrix_product(
  const rc_ptr<Volume< float > > &, const rc_ptr<Volume< float > > & );
template VolumeRef< double > matrix_product(
  const rc_ptr<Volume< double > > &, const rc_ptr<Volume< double > > & );
#ifdef CARTO_LONG_IS_DISTINCT
template VolumeRef< long > matrix_product(
  const rc_ptr<Volume< long > > &, const rc_ptr<Volume< long > > & );
template VolumeRef< unsigned long > matrix_product(
  const rc_ptr<Volume< unsigned long > > &, const rc_ptr<Volume< unsigned long > > & );
#endif

template VolumeRef< bool > transpose<bool>( const Volume< bool > & );
template VolumeRef< int8_t > transpose<int8_t>( const Volume< int8_t > & );
template VolumeRef< uint8_t > transpose<uint8_t>( const Volume< uint8_t > & );
// ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
template VolumeRef< char > transpose<char>( const Volume< char > & );
#endif
template VolumeRef< int16_t > transpose<int16_t>( const Volume< int16_t > & );
template VolumeRef< uint16_t > transpose<uint16_t>( const Volume< uint16_t > & );
template VolumeRef< int32_t > transpose<int32_t>( const Volume< int32_t > & );
template VolumeRef< uint32_t > transpose<uint32_t>( const Volume< uint32_t > & );
template VolumeRef< int64_t > transpose<int64_t>( const Volume< int64_t > & );
template VolumeRef< uint64_t > transpose<uint64_t>( const Volume< uint64_t > & );
template VolumeRef< float > transpose<float>( const Volume< float > & );
template VolumeRef< double > transpose<double>( const Volume< double > & );
#ifdef CARTO_LONG_IS_DISTINCT
template VolumeRef< long > transpose<long>( const Volume< long > & );
template VolumeRef< unsigned long > transpose<unsigned long>( const Volume< unsigned long > & );
#endif

template VolumeRef< bool > transpose<bool>( const rc_ptr<Volume< bool > > &, bool );
template VolumeRef< int8_t > transpose<int8_t>( const rc_ptr<Volume< int8_t > > &, bool );
template VolumeRef< uint8_t > transpose<uint8_t>( const rc_ptr<Volume< uint8_t > > &, bool );
// ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
template VolumeRef< char > transpose<char>( const rc_ptr<Volume< char > > &, bool );
#endif
template VolumeRef< int16_t > transpose<int16_t>( const rc_ptr<Volume< int16_t > > &, bool );
template VolumeRef< uint16_t > transpose<uint16_t>( const rc_ptr<Volume< uint16_t > > &, bool );
template VolumeRef< int32_t > transpose<int32_t>( const rc_ptr<Volume< int32_t > > &, bool );
template VolumeRef< uint32_t > transpose<uint32_t>( const rc_ptr<Volume< uint32_t > > &, bool );
template VolumeRef< int64_t > transpose<int64_t>( const rc_ptr<Volume< int64_t > > &, bool );
template VolumeRef< uint64_t > transpose<uint64_t>( const rc_ptr<Volume< uint64_t > > &, bool );
template VolumeRef< float > transpose<float>( const rc_ptr<Volume< float > > &, bool );
template VolumeRef< double > transpose<double>( const rc_ptr<Volume< double > > &, bool );
#ifdef CARTO_LONG_IS_DISTINCT
template VolumeRef< long > transpose<long>( const rc_ptr<Volume< long > > &, bool );
template VolumeRef< unsigned long > transpose<unsigned long>( const rc_ptr<Volume< unsigned long > > &, bool );
#endif


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

instantiate_volutil( int8_t )
instantiate_volutil2( int8_t, std::plus<int8_t> )
instantiate_volutil2( int8_t, std::minus<int8_t> )
instantiate_volutil2( int8_t, std::multiplies<int8_t> )
instantiate_volutil2( int8_t, std::divides<int8_t> )

instantiate_volutil( uint8_t )
instantiate_volutil2( uint8_t, std::plus<uint8_t> )
instantiate_volutil2( uint8_t, std::minus<uint8_t> )
instantiate_volutil2( uint8_t, std::multiplies<uint8_t> )
instantiate_volutil2( uint8_t, std::divides<uint8_t> )

// ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
instantiate_volutil( char )
instantiate_volutil2( char, std::plus<char> )
instantiate_volutil2( char, std::minus<char> )
instantiate_volutil2( char, std::multiplies<char> )
instantiate_volutil2( char, std::divides<char> )
#endif

instantiate_volutil( int16_t )
instantiate_volutil2( int16_t, std::plus<int16_t> )
instantiate_volutil2( int16_t, std::minus<int16_t> )
instantiate_volutil2( int16_t, std::multiplies<int16_t> )
instantiate_volutil2( int16_t, std::divides<int16_t> )

instantiate_volutil( uint16_t )
instantiate_volutil2( uint16_t, std::plus<uint16_t> )
instantiate_volutil2( uint16_t, std::minus<uint16_t> )
instantiate_volutil2( uint16_t, std::multiplies<uint16_t> )
instantiate_volutil2( uint16_t, std::divides<uint16_t> )

instantiate_volutil( int32_t )
instantiate_volutil2( int32_t, std::plus<int32_t> )
instantiate_volutil2( int32_t, std::minus<int32_t> )
instantiate_volutil2( int32_t, std::multiplies<int32_t> )
instantiate_volutil2( int32_t, std::divides<int32_t> )

instantiate_volutil( uint32_t )
instantiate_volutil2( uint32_t, std::plus<uint32_t> )
instantiate_volutil2( uint32_t, std::minus<uint32_t> )
instantiate_volutil2( uint32_t, std::multiplies<uint32_t> )
instantiate_volutil2( uint32_t, std::divides<uint32_t> )

instantiate_volutil( int64_t )
instantiate_volutil2( int64_t, std::plus<int64_t> )
instantiate_volutil2( int64_t, std::minus<int64_t> )
instantiate_volutil2( int64_t, std::multiplies<int64_t> )
instantiate_volutil2( int64_t, std::divides<int64_t> )

instantiate_volutil( uint64_t )
instantiate_volutil2( uint64_t, std::plus<uint64_t> )
instantiate_volutil2( uint64_t, std::minus<uint64_t> )
instantiate_volutil2( uint64_t, std::multiplies<uint64_t> )
instantiate_volutil2( uint64_t, std::divides<uint64_t> )

#ifdef CARTO_LONG_IS_DISTINCT
instantiate_volutil( long )
instantiate_volutil2( long, std::plus<long> )
instantiate_volutil2( long, std::minus<long> )
instantiate_volutil2( long, std::multiplies<long> )
instantiate_volutil2( long, std::divides<long> )

instantiate_volutil( unsigned long )
instantiate_volutil2( unsigned long, std::plus<unsigned long> )
instantiate_volutil2( unsigned long, std::minus<unsigned long> )
instantiate_volutil2( unsigned long, std::multiplies<unsigned long> )
instantiate_volutil2( unsigned long, std::divides<unsigned long> )
#endif

instantiate_volutil( float )
instantiate_volutil2( float, std::plus<float> )
instantiate_volutil2( float, std::minus<float> )
instantiate_volutil2( float, std::multiplies<float> )
instantiate_volutil2( float, std::divides<float> )

instantiate_volutil( double )
instantiate_volutil2( double, std::plus<double> )
instantiate_volutil2( double, std::minus<double> )
instantiate_volutil2( double, std::multiplies<double> )
instantiate_volutil2( double, std::divides<double> )

instantiate_volutil( cfloat )
instantiate_volutil2( cfloat, std::plus<cfloat> )
instantiate_volutil2( cfloat, std::minus<cfloat> )
instantiate_volutil2( cfloat, std::multiplies<cfloat> )
instantiate_volutil2( cfloat, std::divides<cfloat> )

instantiate_volutil( cdouble )
instantiate_volutil2( cdouble, std::plus<cdouble> )
instantiate_volutil2( cdouble, std::minus<cdouble> )
instantiate_volutil2( cdouble, std::multiplies<cdouble> )
instantiate_volutil2( cdouble, std::divides<cdouble> )

#endif

namespace carto
{

  INSTANTIATE_GENERIC_OBJECT_TYPE( VolumeRef< bool > )
  INSTANTIATE_GENERIC_OBJECT_TYPE( VolumeRef< int8_t > )
  INSTANTIATE_GENERIC_OBJECT_TYPE( VolumeRef< uint8_t > )
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
  INSTANTIATE_GENERIC_OBJECT_TYPE( VolumeRef< char > )
#endif
  INSTANTIATE_GENERIC_OBJECT_TYPE( VolumeRef< int16_t > )
  INSTANTIATE_GENERIC_OBJECT_TYPE( VolumeRef< uint16_t > )
  INSTANTIATE_GENERIC_OBJECT_TYPE( VolumeRef< int32_t > )
  INSTANTIATE_GENERIC_OBJECT_TYPE( VolumeRef< uint32_t > )
  INSTANTIATE_GENERIC_OBJECT_TYPE( VolumeRef< int64_t > )
  INSTANTIATE_GENERIC_OBJECT_TYPE( VolumeRef< uint64_t > )
  INSTANTIATE_GENERIC_OBJECT_TYPE( VolumeRef< float > )
  INSTANTIATE_GENERIC_OBJECT_TYPE( VolumeRef< double > )
  INSTANTIATE_GENERIC_OBJECT_TYPE( VolumeRef< cfloat > )
  INSTANTIATE_GENERIC_OBJECT_TYPE( VolumeRef< cdouble > )
#ifdef CARTO_LONG_IS_DISTINCT
  INSTANTIATE_GENERIC_OBJECT_TYPE( VolumeRef< long > )
  INSTANTIATE_GENERIC_OBJECT_TYPE( VolumeRef< unsigned long > )
#endif

  INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr<Volume< bool > > )
  INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr<Volume< int8_t > > )
  INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr<Volume< uint8_t > > )
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
  INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr<Volume< char > > )
#endif
  INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr<Volume< int16_t > > )
  INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr<Volume< uint16_t > > )
  INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr<Volume< int32_t > > )
  INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr<Volume< uint32_t > > )
  INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr<Volume< int64_t > > )
  INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr<Volume< uint64_t > > )
  INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr<Volume< float > > )
  INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr<Volume< double > > )
  INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr<Volume< cfloat > > )
  INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr<Volume< cdouble > > )
#ifdef CARTO_LONG_IS_DISTINCT
  INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr<Volume< long > > )
  INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr<Volume< unsigned long > > )
#endif


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

  instantiate_copy( bool )
  instantiate_copy( int8_t )
  instantiate_copy( uint8_t )
  instantiate_copy( int16_t )
  instantiate_copy( uint16_t )
// ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
  instantiate_copy( char )
#endif
  instantiate_copy( int32_t )
  instantiate_copy( uint32_t )
  instantiate_copy( int64_t )
  instantiate_copy( uint64_t )
  instantiate_copy( float )
  instantiate_copy( double )
  instantiate_copy( cfloat )
  instantiate_copy( cdouble )
#ifdef CARTO_LONG_IS_DISTINCT
  instantiate_copy( long )
  instantiate_copy( unsigned long )
#endif

#endif

} // namespace carto

//----------------------------------------------------------------------------
// Streams
//----------------------------------------------------------------------------

namespace carto
{

  //--- VolumeOStream --------------------------------------------------------

  VolumeOStream::VolumeOStream( std::ostream & ostream ):
    _maxT(5),
    _maxZ(5),
    _maxY(5),
    _maxX(5),
    _ostream(ostream)
  {}

  VolumeOStream::VolumeOStream( const VolumeOStream & other ):
    _maxT(other._maxT),
    _maxZ(other._maxZ),
    _maxY(other._maxY),
    _maxX(other._maxX),
    _ostream( other.ostream() )
  {}

  VolumeOStream::~VolumeOStream()
  {
  }

  std::ostream & VolumeOStream::ostream() const{ return _ostream; }

  const size_t & VolumeOStream::maxT() const { return _maxT; }
  const size_t & VolumeOStream::maxZ() const { return _maxZ; }
  const size_t & VolumeOStream::maxY() const { return _maxY; }
  const size_t & VolumeOStream::maxX() const { return _maxX; }

  size_t & VolumeOStream::maxT() { return _maxT; }
  size_t & VolumeOStream::maxZ() { return _maxZ; }
  size_t & VolumeOStream::maxY() { return _maxY; }
  size_t & VolumeOStream::maxX() { return _maxX; }

  //--- VolumeOStreamSetter --------------------------------------------------

  VolumeOStreamSetter::VolumeOStreamSetter():
    _maxT(5),
    _maxZ(5),
    _maxY(5),
    _maxX(5)
  {}

  const size_t & VolumeOStreamSetter::maxT() const { return _maxT; }
  const size_t & VolumeOStreamSetter::maxZ() const { return _maxZ; }
  const size_t & VolumeOStreamSetter::maxY() const { return _maxY; }
  const size_t & VolumeOStreamSetter::maxX() const { return _maxX; }

  size_t & VolumeOStreamSetter::maxT() { return _maxT; }
  size_t & VolumeOStreamSetter::maxZ() { return _maxZ; }
  size_t & VolumeOStreamSetter::maxY() { return _maxY; }
  size_t & VolumeOStreamSetter::maxX() { return _maxX; }

  //--- setMaxDim ------------------------------------------------------------

  VolumeOStreamSetter setMaxDim( size_t m )
  {
    VolumeOStreamSetter out;
    out.maxT() = m;
    out.maxZ() = m;
    out.maxY() = m;
    out.maxX() = m;
    return out;
  }

  VolumeOStreamSetter setMaxDim( size_t mx, size_t my, size_t mz, size_t mt )
  {
    VolumeOStreamSetter out;
    out.maxT() = mt;
    out.maxZ() = mz;
    out.maxY() = my;
    out.maxX() = mx;
    return out;
  }

  //--- operator<< -----------------------------------------------------------

  VolumeOStream operator<< ( std::ostream & out,
                                    const VolumeOStreamSetter & setter )
  {
    VolumeOStream volstream( out );
    volstream.maxT() = setter.maxT();
    volstream.maxZ() = setter.maxZ();
    volstream.maxY() = setter.maxY();
    volstream.maxX() = setter.maxX();
    return volstream;
  }

} // namespace carto
