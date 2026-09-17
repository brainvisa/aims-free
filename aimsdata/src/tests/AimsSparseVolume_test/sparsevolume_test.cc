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


