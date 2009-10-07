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
#include <aims/bucket/sparsevolume.h>
#include <aims/getopt/getopt2.h>

using namespace aims;
using namespace carto;
using namespace std;

int main( int argc, const char **argv )
{
  AimsApplication       app( argc, argv,
                             "Tests the common API for volumes and buckets" );

  try
  {
    app.initialize();

    SparseVolume<BucketMap<int16_t> > bsvol( 10, 10, 10 );
    SparseVolume<BucketMap<Void> > bvvol( 10, 10, 10 );
    SparseVolume<Volume<int16_t> > vsvol( 10, 10, 10 );
    cout << "fill with value 7\n";
    bsvol.fill( 7 );
    bvvol.fill( 7 );
    vsvol.fill( 7 );

    cout << "insertion test: value 10\n";
    bsvol.setValue( 10, 5, 5, 5 );
    bvvol.setValue( 10, 5, 5, 5 );
    vsvol.setValue( 10, 5, 5, 5 );
    cout << "inserted value test (should be 10):\n";
    cout << "bucketS16  value: " << bsvol.at( 5, 5, 5 ) << endl;
    cout << "bucketVoid value: " << bvvol.at( 5, 5, 5 ) << endl;
    cout << "VolumeS16  value: " << vsvol.at( 5, 5, 5 ) << endl;
    if( bsvol.at( 5, 5, 5 ) != 10 || bvvol.at( 5, 5, 5 ) != 10
        || vsvol.at( 5, 5, 5 ) != 10 )
      throw runtime_error( "wrong value read" );
    cout << "background value test (should be 7):\n";
    cout << "bucketS16  value: " << bsvol.at( 2, 2, 2 ) << endl;
    cout << "bucketVoid value: " << bvvol.at( 2, 2, 2 ) << endl;
    cout << "VolumeS16  value: " << vsvol.at( 2, 2, 2 ) << endl;
    if( bsvol.at( 2, 2, 2 ) != 7 || bvvol.at( 2, 2, 2 ) != 7
        || vsvol.at( 2, 2, 2 ) != 7 )
      throw runtime_error( "wrong value read" );

    return EXIT_SUCCESS;
  }
  catch( user_interruption& )
  {
  }
  catch( exception & e )
  {
    cerr << e.what() << endl;
  }

  return EXIT_FAILURE;
}


