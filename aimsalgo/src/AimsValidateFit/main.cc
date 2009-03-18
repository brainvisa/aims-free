/* Copyright (c) 1995-2007 CEA
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
  AimsData<byte> mask( 26, 25, 1 );
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
    rgb( x, y ).red()   = byte( fabs( direction( d ).item( 0 ) ) * 255.0 );
    rgb( x, y ).green() = byte( fabs( direction( d ).item( 1 ) ) * 255.0 );
    rgb( x, y ).blue()  = byte( fabs( direction( d ).item( 2 ) ) * 255.0 );
  }
  Writer<AimsData<AimsRGB> > dataW3( "direction_RGB" );
  dataW3 << rgb;
}


// compilation of some Volume classes on Aims types

#include <cartodata/volume/volume_d.h>

template class carto::Volume< Trieder >;
template class carto::Volume< AimsData<float> >;


