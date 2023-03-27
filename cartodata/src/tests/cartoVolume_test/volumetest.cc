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

#include <cstdlib>
#include <cartodata/volume/volume.h>
#include <cartobase/containers/nditerator.h>
#include <cstdlib>
#include <iostream>
#include <time.h>
#include <vector>


using namespace carto;
using namespace std;

template <typename T>
struct Identity
{
  typedef T type;
};

template <typename T>
struct container
{};

template <typename LEFT, typename RIGHT>
struct result
{
#if __cplusplus >= 201100
  typedef decltype( std::declval<LEFT>() + std::declval<RIGHT>() ) result_type;
#else
  typedef typeof( LEFT() + RIGHT() ) result_type;
#endif
};

template <typename LEFT, typename RIGHT, template<typename> class CONTAINER >
struct result<CONTAINER<LEFT>, RIGHT>
{
  typedef CONTAINER<typename result<LEFT,RIGHT>::result_type> result_type;
};

template <typename LEFT, typename RIGHT, template<typename> class CONTAINER >
struct result<LEFT, CONTAINER<RIGHT> >
{
  typedef CONTAINER<typename result<LEFT,RIGHT>::result_type> result_type;
};

template <typename LEFT, typename RIGHT, template<typename> class CONTAINER1, template <typename> class CONTAINER2>
struct result<CONTAINER1<LEFT>, CONTAINER2<RIGHT> >
{
  typedef CONTAINER1<typename result<LEFT,RIGHT>::result_type> result_type;
};

template <typename T, typename U>
typename result<T,U>::result_type
test ( const container<T> & vol, const U & value )
{
  cout << "c + u" << endl;
  return typename result<T,U>::result_type();
}

template <typename T>
T diff( const T & t1, const T & t2 )
{
  return std::abs( t2 - t1 );
}

template <typename T>
bool equal( const T & t1, const T & t2 )
{
  return diff( t1, t2 ) < 1e-5;
}

#if 0
template <typename T>
inline
T & volume_at( T* data, const vector<long> & strides,
               long x, long y, long z, long t )
{
  return data[ x + y * strides[1] + z * strides[2] + t * strides[3] ];
}

template <typename T>
inline
T & volume_at( T* data, const vector<long> & strides,
               const vector<int> & pos )
{
  size_t offset = 0;
  int n = pos.size();
  for( int i=0; i<n; ++i )
    offset += pos[i] * strides[i];
  return data[ offset ];
}


template <typename T>
class VolumeAccessor
{
public:
  VolumeAccessor( Volume<T> & vol )
    : data( &vol.at( 0 ) ), strides( &vol.getStrides()[0] )
//       nd( vol.getStrides().size() )
  {
  }

  T & operator () ( long x, long y, long z, long t )
  {
    return data[ x * strides[0] + y * strides[1] + z * strides[2]
                 + t * strides[3] ];
  }

  T & operator () ( const vector<int> & pos )
  {
//     size_t offset = 0;
//     offset = 0;
//     blitz::diffType *strides2 = strides;
//     unsigned long *strides2 = strides;
//     int nd = pos.size();

//     switch( pos.size() )
//     {
//       case 1:
//         return data[ pos[0] * strides[0] ];
//       case 2:
//         return data[ pos[0] * strides[0]
//                      + pos[1] * strides[1] ];
//       case 3:
//         return data[ pos[0] * strides[0]
//                      + pos[1] * strides[1]
//                      + pos[2] * strides[2] ];
//       case 4:
//         return data[ pos[0] * strides[0]
//                      + pos[1] * strides[1]
//                      + pos[2] * strides[2]
//                      + pos[3] * strides[3] ];
//       case 5:
//         return data[ pos[0] * strides[0]
//                      + pos[1] * strides[1]
//                      + pos[2] * strides[2]
//                      + pos[3] * strides[3]
//                      + pos[4] * strides[4] ];
//       case 6:
//         return data[ pos[0] * strides[0]
//                      + pos[1] * strides[1]
//                      + pos[2] * strides[2]
//                      + pos[3] * strides[3]
//                      + pos[4] * strides[4]
//                      + pos[5] * strides[5] ];
//       case 7:
//         return data[ pos[0] * strides[0]
//                      + pos[1] * strides[1]
//                      + pos[2] * strides[2]
//                      + pos[3] * strides[3]
//                      + pos[4] * strides[4]
//                      + pos[5] * strides[5]
//                      + pos[6] * strides[6] ];
//       case 8:
//         return data[ pos[0] * strides[0]
//                      + pos[1] * strides[1]
//                      + pos[2] * strides[2]
//                      + pos[3] * strides[3]
//                      + pos[4] * strides[4]
//                      + pos[5] * strides[5]
//                      + pos[6] * strides[6]
//                      + pos[7] * strides[7] ];
//       default:
        size_t offset = 0;
        for( int i=0; i!=pos.size(); ++i )
          offset += pos[i] * strides[i];
        return data[ offset ];
//     }
//     return data[0];
  }

  T* data;
//   blitz::diffType *strides;
  unsigned long *strides;
//   int nd;
//   size_t offset;
};
#endif


template <typename T, typename U>
typename result<T,U>::result_type
test ( const container<T> & /* vol */, const container<U> & /* other */ )
{
  cout << "c + c" << endl;
  return typename result<T,U>::result_type();
}

int main( int /*argc*/, const char** /*argv*/ )
{
  test( container<float>(), container<float>() );
  int   result = EXIT_SUCCESS;
  int ntest = 0;

  cout << "-- Test " << ntest++ << ": volume copy --" << endl;
  carto::Volume<int16_t> first( 5, 5, 5 );
  carto::Volume<int16_t> second( 3, 3, 3 );
  first = second;
  int sizex = 0, sizey = 0, sizez = 0;
  first.header().getProperty("sizeX", sizex );
  first.header().getProperty("sizeY", sizey );
  first.header().getProperty("sizeZ", sizez );
  cout << "copied volume dims: " << sizex << ", " << sizey << ", " << sizez
    << endl;
  if( sizex != 3 || sizey != 3 || sizez != 3 )
  {
    cerr << "*** error in copied sizes ***" << endl;
    result = EXIT_FAILURE;
  }
  cout << endl;

  cout << "-- Test " << ntest++ << ": regular volume: vol1 ( 10, 10, 10 ) --"
       << endl;
  VolumeRef<int16_t>	vol1( new Volume<int16_t>( 10, 10, 10 ) );
  cout << "vol1 allocated. filling with value 5" << endl;
  vol1->fill( 5 );
  cout << "status: isAllocated() : " << vol1->allocatorContext().isAllocated()
       << endl;
  cout << "pointer 1st voxel : " << &(*vol1)( 0 ) << endl;
  cout << "datasource : " << vol1->allocatorContext().dataSource() << endl;
  cout << "dimensions: " << vol1->getSizeX() << ", " << vol1->getSizeY()
       << ", " << vol1->getSizeZ() << ", " << vol1->getSizeT() << std::endl;
  cout << "value at pos( 5,5,5 ) (should be 5) : " << (*vol1)( 5, 5, 5 )
       << " [op.() on VolumeRef: " << vol1( 5, 5, 5 ) << " ]" << endl << endl;
  if( (*vol1)( 5, 5, 5 ) != 5 )
    {
      cerr << "*** error ***" << endl;
      result = EXIT_FAILURE;
    }

  cout << "-- Test " << ntest++ << ": volume view: vol2 ( 6, 6, 6 ) in vol1 --"
       << endl;
  VolumeRef<int16_t>	vol2
    ( new Volume<int16_t>( vol1,
                           Volume<int16_t>::Position4Di( 2, 2, 2 ),
                           Volume<int16_t>::Position4Di( 6, 6, 6, 1 )
                          ) );
  cout << "vol2, view in vol1: allocated" << endl;
  cout << "status: isAllocated() : " << vol2->allocatorContext().isAllocated()
       << endl;
  cout << "pointer 1st voxel : " << &(*vol2)( 0 ) << endl;
  cout << "datasource : " << vol2->allocatorContext().dataSource() << endl;
  cout << "dimensions: " << vol2->getSizeX() << ", " << vol2->getSizeY()
       << ", " << vol2->getSizeZ() << ", " << vol2->getSizeT() << std::endl;
  cout << "1st voxel: " << &(*vol2)( 0 ) << endl;
  cout << "value at pos( 1, 1, 1 ) (should be 5) : " << (*vol2)( 1, 1, 1 )
       << endl;
  if( (*vol2)( 1, 1, 1 ) != 5 )
    {
      cerr << "*** error ***" << endl;
      result = EXIT_FAILURE;
    }
  cout << "filling vol2 with value 10" << endl;

  vol2->fill( 10 );
  cout << "min: " << carto::min( vol2 ) << ", max: " << carto::max( vol2 )
       << endl;

  cout << "value at pos( 1, 1, 1 ) (should be 10) : " << (*vol2)( 1, 1, 1 )
       << endl;
  if( (*vol2)( 1, 1, 1 ) != 10 )
    {
      cerr << "*** error ***" << endl;
      result = EXIT_FAILURE;
    }
  cout << "vol1 value at pos( 1, 1, 1 ) (should be 5) : "
       << (*vol1)( 1, 1, 1 ) << endl;
  if( (*vol1)( 1, 1, 1 ) != 5 )
    {
      cerr << "*** error ***" << endl;
      result = EXIT_FAILURE;
    }
  cout << "vol1 value at pos( 5, 5, 5 ) (should be 10) : "
       << (*vol1)( 5, 5, 5 ) << endl << endl;
  if( (*vol1)( 5, 5, 5 ) != 10 )
    {
      cerr << "*** error ***" << endl;
      result = EXIT_FAILURE;
    }
  cout << "writing value 53 at pos( 4,4,4 ) in vol1" << endl;
  (*vol1)( 4, 4, 4 ) = 53;
  cout << "reading vol1( 4, 4, 4 ): " << (*vol1)( 4, 4, 4 ) << endl;
  cout << "reading vol1( 2, 2, 2 ): " << (*vol2)( 2, 2, 2 ) << "  ";
  if( (*vol2)( 2, 2, 2 ) != 53 || (*vol1)( 4, 4, 4 ) != 53 )
    {
      cout << "FAILED" << endl;
      cerr << "*** error ***" << endl;
      result = EXIT_FAILURE;
    }
  else cout << "OK";
  cout << endl;
  cout << "writing value 85 at pos( 4,4,4 ) in vol2" << endl;
  (*vol2)( 4, 4, 4 ) = 85;
  cout << "reading vol2( 4, 4, 4 ): " << (*vol2)( 4, 4, 4 ) << endl;
  cout << "reading vol1( 6, 6, 6 ): " << (*vol1)( 6, 6, 6 ) << "  ";
  if( (*vol1)( 6, 6, 6 ) != 85 || (*vol2)( 4, 4, 4 ) != 85 )
    {
      cout << "FAILED" << endl;
      cerr << "*** error ***" << endl;
      result = EXIT_FAILURE;
    }
  else cout << "OK";
  cout << endl << endl;

  cout << "-- Test " << ntest++ << ": virtual volume: vol3 ( 10, 10, 10 ) --"
       << endl;
  VolumeRef<int16_t>	vol3
    ( new Volume<int16_t>( 10, 10, 10, 1, AllocatorContext(), false ) );
  cout << "vol3 allocated" << endl;
  cout << "status: isAllocated() : " << vol3->allocatorContext().isAllocated()
       << endl;
  cout << "pointer 1st voxel : " << &(*vol3)( 0 ) << endl;
  cout << "datasource : " << vol3->allocatorContext().dataSource() << endl;
  cout << "dimensions: " << vol3->getSizeX() << ", " << vol3->getSizeY()
       << ", " << vol3->getSizeZ() << ", " << vol3->getSizeT() << std::endl;
  cout << endl;

  cout << "-- Test " << ntest++ << ": volume view: vol4 ( 6, 6, 6 ) in vol3 --"
       << endl;
  VolumeRef<int16_t>	vol4
    ( new Volume<int16_t>( vol3,
                           Volume<int16_t>::Position4Di( 2, 2, 2 ),
                           Volume<int16_t>::Position4Di( 6, 6, 6, 1 )
                          ) );
  cout << "status: isAllocated() : " << vol4->allocatorContext().isAllocated()
       << endl;
  cout << "pointer 1st voxel : " << &(*vol4)( 0 ) << endl;
  cout << "datasource : " << vol4->allocatorContext().dataSource() << endl;
  cout << "dimensions: " << vol4->getSizeX() << ", " << vol4->getSizeY()
       << ", " << vol4->getSizeZ() << ", " << vol4->getSizeT() << std::endl;
  vol4->fill( 15 );
  cout << "value at pos( 1, 1, 1 ) (should be 15) : " << (*vol4)( 1, 1, 1 )
       << endl;
  if( (*vol4)( 1, 1, 1 ) != 15 )
  {
    cerr << "*** error ***" << endl;
    result = EXIT_FAILURE;
  }
  cout << endl;


  cout << "-- Test " << ntest++ << ": volume with strides -- " << endl;
  vector<int16_t> vec9( 3*4*5, 0 );
  vector<int> dims9( 3 );
  dims9[0] = 3;
  dims9[1] = 4;
  dims9[2] = 5;
  vector<long> strides9( 3 );
  strides9[0] = 4 * 5; // stored as z, y, x
  strides9[1] = 5;
  strides9[2] = 1;

  VolumeRef<int16_t> vol9( new Volume<int16_t>( dims9, &vec9[0], &strides9 ) );
  vol9->fill( 1 );
  // set value 87 at position (1, 2, 3)
  vec9[ 1 * 20 + 2 * 5 + 3 ] = 87;
  cout << "value at pos( 1, 2, 3 ) (should be 87): " << (*vol9)( 1, 2, 3 )
       << endl;
  if( (*vol9)( 1, 2, 3 ) != 87 )
  {
    cerr << "*** error ***" << endl;
    result = EXIT_FAILURE;
  }
  cout << endl;

  // iterators
  NDIterator<int16_t> it9( &vol9->at( 0 ), dims9, strides9 );
  unsigned long long n9 = 0;
  for( ; !it9.ended(); ++it9 )
    n9 += *it9;
  cout << "sum via NDIterartor: (should be " << 3 * 4 * 5 + 86 << "): " << n9
       << endl;
  if( n9 != 3 * 4 * 5 + 86 )
  {
    cerr << "*** error ***" << endl;
    result = EXIT_FAILURE;
  }
  cout << endl;

  line_NDIterator<int16_t> lit9( &vol9->at( 0 ), dims9, strides9, true );
  cout << "line_iterator is_contiguous (should be true): "
       << lit9.is_contiguous() << endl;
  cout << "line_iterator line_direction (should be 2): "
       << lit9.line_direction() << endl;
  cout << "line_iterator line_length (should be 5): "
       << lit9.line_length() << endl;
  if( !lit9.is_contiguous() || lit9.line_direction() != 2
      || lit9.line_length() != 5 )
  {
    cerr << "*** error ***" << endl;
    result = EXIT_FAILURE;
  }

  line_NDIterator<int16_t> lit9b( &vol9->at( 0 ), dims9, strides9 );
  cout << "line_iterator2 is_contiguous (should be false): "
       << lit9b.is_contiguous() << endl;
  cout << "line_iterator2 line_direction (should be 0): "
       << lit9b.line_direction() << endl;
  cout << "line_iterator line_length (should be 60): "
       << lit9b.line_length() << endl;
  if( lit9b.is_contiguous() || lit9b.line_direction() != 0
      || lit9b.line_length() != 60 )
  {
    cerr << "*** error ***" << endl;
    result = EXIT_FAILURE;
  }

  n9 = 0;
  int16_t *p9, *pp9;
  for( ; !lit9.ended(); ++lit9 )
  {
    p9 = &*lit9;
    for( pp9=p9 + lit9.line_length(); p9!=pp9; lit9.inc_line_ptr( p9 ) )
      n9 += *p9;
  }
  cout << "sum via line_NDIterartor: (should be " << 3 * 4 * 5 + 86 << "): "
       << n9 << endl;
  if( n9 != 3 * 4 * 5 + 86 )
  {
    cerr << "*** error ***" << endl;
    result = EXIT_FAILURE;
  }
  cout << endl;


  cout << "-- Test " << ntest++ << ": operators -- " << endl;
  cout << "vol1" << endl << vol1 << endl;
  VolumeRef<int16_t> vol5 = vol1 + vol1;
  cout << "vol5 = vol1 + vol1" << endl << vol5 << endl;
  cout << "value at pos( 5,5,5 ) (should be " << vol1->at( 5, 5, 5 ) * 2
       << ") : " << (*vol5)( 5, 5, 5 )
       << endl << endl;
  if( (*vol5)( 5, 5, 5 ) != vol1->at( 5, 5, 5 ) * 2 )
  {
    cerr << "*** error ***" << endl;
    result = EXIT_FAILURE;
  }
  vol5 += 5.;
  cout << "vol5 += 5." << endl << vol5 << endl;


  cout << "-- Test " << ntest++ << ": volume transpose -- " << endl;
  {
    VolumeRef<float> vol10( 3, 4, 2 );
    // fill volume
    line_NDIterator<float> it( &vol10->at( 0 ), vol10->getSize(),
                               vol10->getStrides() );
    float *p, *pp;
    float value = 0.;

    for( ; !it.ended(); ++it )
    {
      p = &*it;
      for( pp=p + it.line_length(); p!=pp; it.inc_line_ptr( p ) )
      {
        *p = value;
        value += 1.1;
      }
    }
    if( !equal( vol10->at( 1, 2 ), 7.7f ) )
    {
      cerr << "*** error in volume fill ***" << endl;
      cerr << vol10->at( 1, 2 ) << " != 7.7\n";
      result = EXIT_FAILURE;
    }

    VolumeRef<float> trans = transpose( *vol10 );
    if( trans->getSize()[0] != 4 || trans->getSize()[1] != 3
        || trans->getSize()[2] != 2 )
    {
      cerr << "*** error in transpose (copy) size ***" << endl;
      result = EXIT_FAILURE;
    }
    if( !equal( trans->at( 2, 1 ), 7.7f )
        || !equal( trans->at( 2, 1, 1 ), 20.9f )
        || !equal( trans->at( 1, 2, 1 ), 18.7f ) )
    {
      cerr << "*** error in transpose (copy) values ***" << endl;
      result = EXIT_FAILURE;
      /*
      cerr << trans->at( 2, 1 ) << ", " << trans->at( 2, 1, 1 ) << ", " << trans->at( 1, 2, 1 ) << endl;
      cerr << vol10->at( 1, 2 ) << ", " << vol10->at( 1, 2, 1 ) << ", " << vol10->at( 2, 1, 1 ) << endl;
      */
    }

    // transpose with copy (same as above)
    trans = transpose( vol10, true );
    if( trans->getSize()[0] != 4 || trans->getSize()[1] != 3
        || trans->getSize()[2] != 2 )
    {
      cerr << "*** error in transpose (ref/copy) size ***" << endl;
      result = EXIT_FAILURE;
    }
    if( !equal( trans->at( 2, 1 ), 7.7f )
        || !equal( trans->at( 2, 1, 1 ), 20.9f )
        || !equal( trans->at( 1, 2, 1 ), 18.7f ) )
    {
      cerr << "*** error in transpose (ref/copy) values ***" << endl;
      result = EXIT_FAILURE;
      /*
      cerr << trans->at( 2, 1 ) << ", " << trans->at( 2, 1, 1 ) << ", " << trans->at( 1, 2, 1 ) << endl;
      cerr << vol10->at( 1, 2 ) << ", " << vol10->at( 1, 2, 1 ) << ", " << vol10->at( 2, 1, 1 ) << endl;
      */
    }
    vol10->at( 1, 2 ) = 15.8;
    if( !equal( trans->at( 2, 1 ), 7.7f ) )
    {
      cerr << "*** error in transpose (ref/copy) copy ***" << endl;
      result = EXIT_FAILURE;
    }
    vol10->at( 1, 2 ) = 7.7;

    // transpose shared
    trans = transpose( vol10 );
    if( trans->getSize()[0] != 4 || trans->getSize()[1] != 3
        || trans->getSize()[2] != 2 )
    {
      cerr << "*** error in transpose (shared) size ***" << endl;
      result = EXIT_FAILURE;
    }
    if( !equal( trans->at( 2, 1 ), 7.7f )
        || !equal( trans->at( 2, 1, 1 ), 20.9f )
        || !equal( trans->at( 1, 2, 1 ), 18.7f ) )
    {
      cerr << "*** error in transpose (shared) values ***" << endl;
      result = EXIT_FAILURE;
      /*
      cerr << trans->at( 2, 1 ) << ", " << trans->at( 2, 1, 1 ) << ", " << trans->at( 1, 2, 1 ) << endl;
      cerr << vol10->at( 1, 2 ) << ", " << vol10->at( 1, 2, 1 ) << ", " << vol10->at( 2, 1, 1 ) << endl;
      */
    }
    vol10->at( 1, 2 ) = 15.8;
    if( !equal( trans->at( 2, 1 ), 15.8f ) )
    {
      cerr << "*** error in transpose (shared) sharing ***" << endl;
      result = EXIT_FAILURE;
    }
    vol10->at( 1, 2 ) = 7.7;
  }

  cout << "-- Test " << ntest++ << ": N-D iterators test --" << endl;
  vector<int> dims( 8, 1 );
  dims[0] = 3;
  dims[1] = 3;
  dims[2] = 3;
  dims[3] = 3;
  dims[4] = 3;
  dims[5] = 3;
  dims[6] = 3;
  dims[7] = 2;
  VolumeRef<int16_t> vol7( new Volume<int16_t>( dims ) );
  if( vol7->getSize() != dims )
  {
    cerr << "*** error in 8D volume dims ***" << endl;
    result = EXIT_FAILURE;
    size_t dim, ndim = vol7->getSize().size();
    for( dim=0; dim<ndim; ++dim )
      cerr << vol7->getSize()[dim] << ", ";
    cerr << "\n expected:\n";
    for( dim=0; dim<dims.size(); ++dim )
      cerr << dims[dim] << ", ";
    cerr << endl;
  }
  size_t dim, ndim = vol7->getSize().size();
  vector<long> strides = vol7->getStrides();
  NDIterator<int16_t> it( &vol7->at( 0 ), vol7->getSize(), strides );
  size_t cnt = 0;
  for( ; !it.ended(); ++it, ++cnt )
  {
  }
  cout << "items count: " << cnt << endl;
  size_t nit = 1;
  for( dim=0; dim<ndim; ++dim )
    nit *= dims[dim];
  if( cnt != nit )
  {
    cerr << "*** error in 8D volume iteration count ***" << endl;
    result = EXIT_FAILURE;
    cerr << "iter count: " << cnt << ", should be: " << nit << endl;
  }
//   if( it.position() != dims )
//   {
//     cerr << "*** error in 8D volume iteration end position ***" << endl;
//     result = EXIT_FAILURE;
//     cerr << "position:\n";
//   }
//   if( it.offset() != nit )
//   {
//     cerr << "*** error in 8D volume iteration end offset ***" << endl;
//     result = EXIT_FAILURE;
//     cout << "offset: " << it.offset() << ", should be: " << nit << endl;
//   }

  line_NDIterator<int16_t> it2( &vol7->at( 0 ), vol7->getSize(), strides );
  cnt = 0;
  for( ; !it2.ended(); ++it2, ++cnt )
  {
  }
  cout << "line items count: " << cnt << endl;
  if( cnt != nit / dims[0] )
  {
    cerr << "*** error in 8D volume line iteration count ***" << endl;
    result = EXIT_FAILURE;
    cerr << "iter count: " << cnt << ", should be: " << nit / dims[0] << endl;
  }


  // matrix_product test
  cout << "-- Test " << ntest++ << ": matrix_product test --" << endl;

  VolumeRef<float> mat1( 3, 4 );
  VolumeRef<float> mat2( 5, 3);
  VolumeRef<float> mat3;

  try
  {
    mat3 = matrix_product( mat1, mat2 );
    // if we are here the test hasn't fired the correct exception...
    cerr << "matrix_product dimensions check did not operate" << endl;
    result = EXIT_FAILURE;
  }
  catch( runtime_error & )
  {
    // OK this should fail...
  }
  mat2 = VolumeRef<float>( 4, 2 );
  mat1.fill( 0. );
  mat2.fill( 0. );
  mat1(0, 0) = 1.;
  mat1(1, 1) = 1.5;
  mat1(2, 2) = .8;
  mat1(2, 3) = 2.;
  mat2(0, 0) = -0.5;
  mat2(1, 1) = 1.2;
  mat2(2, 0) = 1.1;
  mat2(3, 1) = 0.7;
  // std::cout << mat1 << endl;
  // std::cout << mat2 << endl;
  mat3 = matrix_product( mat1, mat2 );
  // std::cout << mat3 << endl;
  VolumeRef<float> mat4( 3, 2 );
  mat4( 0, 0 ) = -0.5;
  mat4( 0, 1 ) = 0;
  mat4( 1, 0 ) = 0;
  mat4( 1, 1 ) = 1.8;
  mat4( 2, 0 ) = 0.88;
  mat4( 2, 1 ) = 1.4;

  mat4 -= mat3;
  // cout << min( mat4 ) << ", " << max( mat4 ) << endl;
  if( min( mat4 ) < -0.001 || max( mat4 ) > 0.001 )
  {
    cerr << "matrix_product is buggy" << endl;
    result = EXIT_FAILURE;
  }


  cout << "-- Test " << ntest++ << ": fast allocation test --" << endl;
  {
    int    testtime = 2;
    float  ck2;
    long   nn = 0;
    clock_t ck = clock();
    float t1;

    do
    {
      VolumeRef<int16_t> vol( 10, 10, 1, 1 );
      ++nn;
    }
    while( clock() - ck < testtime * CLOCKS_PER_SEC );
    ck2 = float( clock() - ck ) / CLOCKS_PER_SEC;
    cout << nn << " regular allocations in " << ck2
         << "s : " << nn / ck2 << " alloc/s" << endl;
    t1 = nn / ck2;

    nn = 0;
    ck = clock();

    do
    {
      VolumeRef<int16_t> vol( 10, 10, 1, 1, AllocatorContext::fast() );
      ++nn;
    }
    while( clock() - ck < testtime * CLOCKS_PER_SEC );
    ck2 = float( clock() - ck ) / CLOCKS_PER_SEC;
    cout << nn << " fast allocations in " << ck2
         << "s : " << nn / ck2 << " alloc/s" << endl;
    t1 = nn / ck2 / t1;
    cout << "speed factor gain: " << t1 << endl;
    // t1 should be about 10, thus being under 3 means there is a problem
    if( t1 < 3. )
    {
      cerr << "Fast allocation should be faster than that.\n";
      result = EXIT_FAILURE;
    }
  }


  cout << "-- Test " << ntest++ << ": operators +/- test --" << endl;
  {
    VolumeRef<float> v1( 10, 10, 10 );
    VolumeRef<float> v2( 12, 12, 12 );

    v1->fill(0);
    v2->fill(0);
    v1->at( 3, 4, 5 ) = 10;
    v1->at( 4, 4, 5 ) = 11;
    v1->at( 5, 4, 5 ) = 12;
    v1->at( 3, 5, 5 ) = 20;
    v1->at( 4, 5, 5 ) = 21;
    v2->at( 3, 4, 5 ) = 0.1;
    v2->at( 4, 4, 5 ) = 0.11;
    v2->at( 5, 4, 5 ) = 0.12;
    v2->at( 3, 5, 5 ) = 0.2;
    v2->at( 5, 5, 5 ) = 0.5;
    v2->at( 11, 11, 11 ) = 45.23;

    VolumeRef<float> v3 = v1 + v2;
    if( v3->getSize()[0] != 12 || v3->getSize()[1] != 12
        || v3->getSize()[2] != 12 )
    {
      cerr << "operator + does not allocate the right size: "
           << v3->getSize()[0] << ", " << v3->getSize()[1] << ", "
           << v3->getSize()[2] << ", should be 12, 12, 12\n";
      result = EXIT_FAILURE;
    }
    if( fabs( v3->at( 3, 4, 5 ) - 10.1 ) > 1e-5
        || fabs( v3->at( 4, 4, 5 ) - 11.11 ) > 1e-5
        || fabs( v3->at( 4, 5, 5 ) - 21 ) > 1e-5 )
    {
      cerr << "erroneous addition.\n";
      cerr << v3->at( 3, 4, 5 ) << ", " << v3->at( 4, 4, 5 ) << ", "
           << v3->at( 4, 5, 5 ) << endl;
      result = EXIT_FAILURE;
    }
    /* TODO: fix this test
    if( v3->at( 11, 11, 11 ) != 45.23 )
    {
      cerr << "addition does not preserve the region outside of volumes "
           << "intersection.\n";
      cerr << v3->at( 11, 11, 11 ) << endl;
      result = EXIT_FAILURE;
    }
    */
  }


  cout << "-- Test " << ntest++ << ": speed test --" << endl;
  // allocate a 16 MB volume
  VolumeRef<int16_t>	vol6( 256, 256, 128 );
  int		n, nn = 0, x, y, z, t, nx = vol6->getSizeX(),
    ny = vol6->getSizeY(), nz = vol6->getSizeZ(), nt = vol6->getSizeT();
  long long     sz = nx * ny * nz * nt;
  clock_t ck;

  cout << "accessors : " << flush;
//   cout << "value before: " << vol6->at( 200, 200, 100 ) << endl;
  ck = clock();
  float		ck2;
  int		testtime = 5;

  do
  {
    for( t=0; t<nt; ++t )
      for( z=0; z<nz; ++z )
        for( y=0; y<ny; ++y )
          for( x=0; x<nx; ++x )
            ++vol6->at( x, y, z, t );
    ++nn;
  }
  while( clock() - ck < testtime * CLOCKS_PER_SEC );
  ck2 = float( clock() - ck ) / CLOCKS_PER_SEC;
  cout << nn << " x 8M voxels in " << ck2
       << "s : " << sz * nn / ck2 << " vox/s" << endl;

  cout << "vector accessors (blitz++) : " << flush;
//   cout << "value before: " << vol6->at( 200, 200, 100 ) << endl;
  ck = clock();
  vector<int> pos6( 4, 0 );
  int & x6t = pos6[3];
  int & x6z = pos6[2];
  int & x6y = pos6[1];
  int & x6x = pos6[0];
//   for( n=0; n<nn; ++n )
  nn = 0;
  do
  {
    for( x6t=0; x6t<nt; ++x6t )
      for( x6z=0; x6z<nz; ++x6z )
        for( x6y=0; x6y<ny; ++x6y )
          for( x6x=0; x6x<nx; ++x6x )
            ++vol6->at( pos6 );
    ++nn;
  }
  while( clock() - ck < testtime * CLOCKS_PER_SEC );
  ck2 = float( clock() - ck ) / CLOCKS_PER_SEC;
  cout << nn << " x 8M voxels in " << ck2
       << "s : " << sz * nn / ck2 << " vox/s" << endl;
//   cout << "value after: " << vol6->at( 200, 200, 100 ) << endl;

#if 0
  cout << "eumlated builtin accessors (no blitz++) : " << flush;
  cout << "value before: " << vol6->at( 200, 200, 100 ) << endl;
  ck = clock();
  vector<long> strides6 = vol6->getStrides();
  int16_t *data6 = &vol6->at( 0 );
  VolumeAccessor<int16_t> acc( *vol6 );
//   for( n=0; n<nn; ++n )
  nn = 0;
  do
  {
    for( t=0; t<nt; ++t )
      for( z=0; z<nz; ++z )
        for( y=0; y<ny; ++y )
          for( x=0; x<nx; ++x )
            ++acc( x, y, z, t );
//             ++volume_at( data6, strides6, x, y, z, t );
    ++nn;
  }
  while( clock() - ck < testtime * CLOCKS_PER_SEC );
  ck2 = float( clock() - ck ) / CLOCKS_PER_SEC;
  cout << nn << " x 8M voxels in " << ck2
       << "s : " << sz * nn / ck2 << " vox/s" << endl;
  cout << "value after: " << vol6->at( 200, 200, 100 ) << endl;

  cout << "eumlated vector builtin accessors (no blitz++) : " << flush;
  cout << "value before: " << vol6->at( 200, 200, 100 ) << endl;
  ck = clock();
//   for( n=0; n<nn; ++n )
  nn = 0;
  do
  {
    for( x6t=0; x6t<nt; ++x6t )
      for( x6z=0; x6z<nz; ++x6z )
        for( x6y=0; x6y<ny; ++x6y )
          for( x6x=0; x6x<nx; ++x6x )
            ++acc( pos6 );
//             ++volume_at( data6, strides6, pos6 );
    ++nn;
  }
  while( clock() - ck < testtime * CLOCKS_PER_SEC );
  ck2 = float( clock() - ck ) / CLOCKS_PER_SEC;
  cout << nn << " x 8M voxels in " << ck2
       << "s : " << sz * nn / ck2 << " vox/s" << endl;
//   cout << "value after: " << vol6->at( 200, 200, 100 ) << endl;
#endif

  cout << "iterators : " << flush;
  ck = clock();
  Volume<int16_t>::iterator	i, e = vol6->end();
//   for( n=0; n<nn; ++n )
  nn = 0;
  do
  {
    for( i=vol6->begin(); i!=e; ++i )
      ++(*i);
    ++nn;
  }
  while( clock() - ck < testtime * CLOCKS_PER_SEC );
  ck2 = float( clock() - ck ) / CLOCKS_PER_SEC;
  cout << nn << " x 8M voxels in " << ck2
       << "s : " << sz * nn / ck2 << " vox/s" << endl;
  cout << "pointers : " << flush;
  ck = clock();
  int16_t	*p, *pp;
//   for( n=0; n<nn; ++n )
  nn = 0;
  do
  {
    pp = &vol6->at( 0, 0, 128 );
    for( p=&vol6->at( 0 ); p!=pp; ++p )
      ++(*p);
    ++nn;
  }
  while( clock() - ck < testtime * CLOCKS_PER_SEC );
  ck2 = float( clock() - ck ) / CLOCKS_PER_SEC;
  cout << nn << " x 8M voxels in " << ck2
       << "s : " << sz * nn / ck2 << " vox/s" << endl;

  cout << "N-D iterators : " << flush;
  strides = vol6->getStrides();
  int stride0 = strides[0];
  ck = clock();
//   for( n=0; n<nn; ++n )
  nn = 0;
  do
  {
    NDIterator<int16_t> it4( &vol6->at( 0 ), vol6->getSize(), strides );
    for( ; !it4.ended(); ++it4 )
      ++(*it4);
    ++nn;
  }
  while( clock() - ck < testtime * CLOCKS_PER_SEC );
  ck2 = float( clock() - ck ) / CLOCKS_PER_SEC;
  cout << nn << " x 8M voxels in " << ck2
       << "s : " << sz * nn / ck2 << " vox/s" << endl;

  int16_t	*pp3 = 0;

  cout << "line iterators : " << flush;
  ck = clock();
  nn = 0;
  do
  {
    line_NDIterator<int16_t> it3( &vol6->at( 0 ), vol6->getSize(), strides );
    pp3 = &vol6->at( 0 );
    for( ; !it3.ended(); ++it3 )
    {
      p = &*it3;
      pp3 = p;
      for( pp=p + it3.line_size() * strides[0]; p!=pp; it3.inc_line_ptr( p ) )
        ++(*p);
    }
    ++nn;
  }
  while( clock() - ck < testtime * CLOCKS_PER_SEC );
  ck2 = float( clock() - ck ) / CLOCKS_PER_SEC;
  cout << nn << " x 8M voxels in " << ck2
       << "s : " << sz * nn / ck2 << " vox/s" << endl;

  cout << "-- Test " << ntest++ << ": fill methods --" << endl;
  vector<int> dims8, pos, view_dims;
  dims8.push_back( 15 );
  dims8.push_back( 15 );
  dims8.push_back( 15 );
  pos.push_back( 2 );
  pos.push_back( 3 );
  pos.push_back( 1 );
  view_dims.push_back( 10 );
  view_dims.push_back( 8 );
  view_dims.push_back( 9 );
  VolumeRef<int16_t> rvol( dims8 );
  VolumeRef<int16_t> vol8( rvol, pos, view_dims );
  if( vol8.getSizeX() != 10 || vol8.getSizeY() != 8 || vol8.getSizeZ() != 9
      || vol8.getSizeT() != 1 )
  {
    cerr << "wrong volume view size: ";
    for( int i=0; i<vol8.getSize().size(); ++i )
      cerr << vol8.getSize()[i] << ", ";
    cerr << "instead of 10, 8, 9, 1\n";
    result = EXIT_FAILURE;
  }
  vol8->fill( 24 );
  vol8->fillBorder( 18 );
  int sum8 = sum<int, int16_t>( vol8 );
  if( sum8 != 24 * 10 * 8 * 9 )
  {
    cerr << "fill(): wrong sum: " << sum8 << " instead of " << 24 * 10 * 8 * 9
         << endl;
    cerr << "vol 0: " << vol8->at(0) << ", " << vol8->at( 5, 5, 5 ) << endl;
    result = EXIT_FAILURE;
  }
  sum8 = sum<int, int16_t>( vol8->refVolume() );
  int expected8 = 18 * 15 * 15 * 15 + (24 - 18) * 10 * 8 * 9;
  if( sum8 != expected8 )
  {
    cerr << "fillBorder(): wrong sum: " << sum8 << " instead of " << expected8
         << endl;
    result = EXIT_FAILURE;
  }


  cout << "===========\n";
  cout << "Overall result: "
    << ( result == EXIT_SUCCESS ? "OK\n" : "FAILED\n" );

  return result;
}


