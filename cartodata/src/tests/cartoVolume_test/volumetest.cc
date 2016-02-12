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
  typedef typeof( LEFT() + RIGHT() ) result_type;
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

template <typename T, typename U>
typename result<T,U>::result_type
test ( const container<T> & vol, const container<U> & other )
{
  cout << "c + c" << endl;
  return typename result<T,U>::result_type();
}

int main( int /*argc*/, char** /*argv*/ )
{
  test( container<float>(), container<float>() );
  int	result = EXIT_SUCCESS;

  cout << "-- Test 1: regular volume: vol1 ( 10, 10, 10 ) --" << endl;
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

  cout << "-- Test 2: volume view: vol2 ( 6, 6, 6 ) in vol1 --" << endl;
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

  cout << "-- Test 3: virtual volume: vol3 ( 10, 10, 10 ) --" << endl;
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

  cout << "-- Test 4: volume view: vol4 ( 6, 6, 6 ) in vol3 --" << endl;
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


  cout << "-- Test 5: operators -- " << endl;
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


  cout << "-- Test 6: speed test --" << endl;
  // allocate a 16 MB volume
  VolumeRef<int16_t>	vol6( 256, 256, 128 );
  int		n, nn = 0, x, y, z, t, nx = vol6->getSizeX(),
    ny = vol6->getSizeY(), nz = vol6->getSizeZ(), nt = vol6->getSizeT();
  long long     sz = nx * ny * nz * nt;
  cout << "accessors : " << flush;
  clock_t	ck = clock();
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
  cout << "iterators : " << flush;
  ck = clock();
  Volume<int16_t>::iterator	i, e = vol6->end();
  for( n=0; n<nn; ++n )
    {
      for( i=vol6->begin(); i!=e; ++i )
        ++(*i);
    }
  ck2 = float( clock() - ck ) / CLOCKS_PER_SEC;
  cout << nn << " x 8M voxels in " << ck2
       << "s : " << sz * nn / ck2 << " vox/s" << endl;
  cout << "pointers : " << flush;
  ck = clock();
  int16_t	*p, *pp;
  for( n=0; n<nn; ++n )
    {
      pp = &vol6->at( 0, 0, 128 );
      for( p=&vol6->at( 0 ); p!=pp; ++p )
        ++(*p);
    }
  ck2 = float( clock() - ck ) / CLOCKS_PER_SEC;
  cout << nn << " x 8M voxels in " << ck2
       << "s : " << sz * nn / ck2 << " vox/s" << endl;


  cout << "===========\n";
  cout << "Overall result: "
    << ( result == EXIT_SUCCESS ? "OK\n" : "FAILED\n" );

  return result;
}


