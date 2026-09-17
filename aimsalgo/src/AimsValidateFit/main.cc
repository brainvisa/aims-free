
#include <cstdlib>
#include <aims/data/data_g.h>
#include <aims/io/io_g.h>
#include <aims/math/math_g.h>
#include <aims/utility/utility_g.h>
#include <aims/rgb/rgb.h>
#include <aims/getopt/getopt2.h>
#include <iomanip>

using namespace aims;
using namespace carto;
using namespace std;

int main( int argc, const char** argv )
{
  try
    {
      AimsApplication	app( argc, argv, "A test program I think, nothing " 
                             "useful" );
      app.initialize();
    }
  catch( user_interruption &e )
    {
      return EXIT_SUCCESS;
    }
  catch( std::exception &e )
    {
      cerr << argv[ 0 ] << ": " << e.what() << endl;
      return EXIT_FAILURE;
    }

  vector< Point3df* > distrib;
  distrib = AimsIcosahedronDiscreteSphericDistribution( 512 );
  cout << "real number of directions : " << distrib.size() << endl;

  
  AimsData< Point3df > direction( distrib.size() );

  int x, y, t, d = 0;
  for ( vector< Point3df* >::const_iterator it=distrib.begin();
        it!=distrib.end();it++ )
    direction( d++ ) = **it; 

  for ( vector< Point3df* >::const_iterator it=distrib.begin();
        it!=distrib.end();it++ )
    delete *it;

  AimsData< Trieder > trieder( direction.dimX() );

  for ( d = 0; d < direction.dimX(); d++ )
    trieder( d ) = AimsOrthonormalTrieder( direction( d ) );

  AimsData< AimsData<float> > tensor( direction.dimX() );
  for ( d = 0; d < direction.dimX(); d++ )
    tensor( d ) = AimsEigenToMatrix( trieder( d ), 1.0e-9, 0.5e-9, 0.5e-9 );


  AimsData< AimsVector<float,6> > B;
  Reader< AimsData<AimsVector<float,6> > >	dataR( "bmatrix" );
  dataR >> B;

  AimsData<short> brute( 26, 25, 1, 31 );
  AimsData<::byte> mask( 26, 25, 1 );
  brute = 0;
  mask = 0;
  for ( t = 0; t < 31; t++ )
  {
    for ( d = 0; d < direction.dimX(); d++ )
    {
      x = d % 26;
      y = d / 26;
      brute( x, y, 0, t ) = short( 4000.0 * 
                                   exp( - B( t )[ 0 ] * tensor(d)( 0, 0 )
                                        - 2.0 * B( t )[ 1 ] * tensor(d)( 0, 1 )
                                        - 2.0 * B( t )[ 2 ] * tensor(d)( 0, 2 )
                                        - B( t )[ 3 ] * tensor(d)( 1, 1 )
                                        - 2.0 * B( t )[ 4 ] * tensor(d)( 1, 2 )
                                        - B( t )[ 5 ] * tensor(d)( 2, 2 ) ) );
      mask( x, y, 0 ) = 1;
    }
  }

  Writer<AimsData<int16_t> > dataW1( "brute" );
  dataW1 << brute;
  Writer<AimsData<uint8_t> > dataW2( "maskU8" );
  dataW2 << mask;

  AimsData< AimsRGB > rgb( 26, 25 );
  rgb = AimsRGB( 0, 0, 0 );
  for ( d = 0; d < direction.dimX(); d++ )
  {
    x = d % 26;
    y = d / 26;
    rgb( x, y ).red()   = ::byte( fabs( direction( d ).item( 0 ) ) * 255.0 );
    rgb( x, y ).green() = ::byte( fabs( direction( d ).item( 1 ) ) * 255.0 );
    rgb( x, y ).blue()  = ::byte( fabs( direction( d ).item( 2 ) ) * 255.0 );
  }
  Writer<AimsData<AimsRGB> > dataW3( "direction_RGB" );
  dataW3 << rgb;
}


// compilation of some Volume classes on Aims types

#include <cartodata/volume/volume_d.h>

template class carto::Volume< Trieder >;
template class carto::Volume< AimsData<float> >;


