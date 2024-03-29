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


#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/data/data.h>
#include <aims/bucket/bucket.h>
#include <aims/getopt/getopt2.h>

using namespace aims;
using namespace carto;
using namespace std;


int main( int argc, const char **argv )
{
  Reader<AimsBucket<DtiTensor> >	bucketR;
  Reader<AimsData< ::byte > > dataR1;

  string fileout;
  int t = 0, npoint = 0;
  short centerX = -1, centerY = -1, centerZ = -1;
  float radius = -1;

  AimsApplication app( argc, argv, "SNR of a VOI in a tensor image" );
  app.addOption( bucketR, "-i", "dtitensor bucket file" );
  app.addOption( dataR1, "-v", "U8 data file of the V.O.I.", true );
  app.addOption( centerX, "-x", "Point3d center of a spherical ROI", true );
  app.addOption( centerY, "-y", "Point3d center of a spherical ROI", true );
  app.addOption( centerZ, "-z", "Point3d center of a spherical ROI", true );
  app.addOption( radius, "-R", "radius of spherical ROI", true );
  app.addOption( fileout, "-o", "output file [default=stdout]", true );
  app.alias( "--input", "-i" );
  app.alias( "--voi", "-v" );
  app.alias( "--output", "-o" );

  try
  {
    app.initialize();

    if( !dataR1.fileName().empty() )
      assert( centerX == -1 && centerY == -1 && centerZ == -1 );
    else
      assert( centerX >= 0 && centerY >= 0 && centerZ >= 0 && radius >= 0 );

    AimsData< ::byte > mask;
    if( !dataR1.fileName().empty() )
    {
      dataR1.read( mask );
    }

    AimsBucket<DtiTensor> bucket;
    list< AimsBucketItem<DtiTensor> >::iterator it;

    cout << "reading tensor bucket : " << flush;
    bucketR >> bucket;
    cout << "done" << endl;


    //
    // mean and power
    //
    float mean[6], power[6];

    mean[0] = mean[1] = mean[2] = mean[3] = mean[4] = mean[5] = 0.0;
    power[0] = power[1] = power[2] = power[3] = power[4] = power[5] = 0.0;

    if( !dataR1.fileName().empty() )
    {
      for ( t = 0; t < 6; t++ )
      {
        npoint = 0;

        for ( it = bucket[0].begin(); it != bucket[0].end(); it++ )
          if ( it->value().category() <= DtiTensor::NO_PROBLEM )
            if ( mask( it->location() ) )
            {
              npoint++;
              mean[t] += it->value().base().coef().item( t );
              power[t] += sqr( it->value().base().coef().item( t ) );
            }
        assert( npoint > 0 );
        mean[t] /= float( npoint );
        power[t] /= float( npoint );
      }
    }
    else
    {
      for ( t = 0; t < 6; t++ )
      {
        npoint = 0;

        for ( it = bucket[0].begin(); it != bucket[0].end(); it++ )
          if ( it->value().category() <= DtiTensor::NO_PROBLEM )
            if ( sqr( float( it->location().item( 0 ) ) - float( centerX ) ) +
                sqr( float( it->location().item( 1 ) ) - float( centerY ) ) +
                sqr( float( it->location().item( 2 ) ) - float( centerZ ) )
              < sqr( radius ) )
            {
              npoint++;
              mean[t] += it->value().base().coef().item( t );
              power[t] += sqr( it->value().base().coef().item( t ) );
            }
        assert( npoint > 0 );
        mean[t] /= float( npoint );
        power[t] /= float( npoint );
      }
    }


    //
    // standard deviation
    //
    float sigma[6];

    sigma[0] = sigma[1] = sigma[2] = sigma[3] = sigma[4] = sigma[5] = 0.0;

    if( !dataR1.fileName().empty() )
    {
      for ( t = 0; t < 6; t++ )
      {
        for ( it = bucket[0].begin(); it != bucket[0].end(); it++ )
          if ( it->value().category() <= DtiTensor::NO_PROBLEM )
            if ( mask( it->location() ) )
              sigma[t] += ( it->value().base().coef().item( t ) - mean[t] ) *
                          ( it->value().base().coef().item( t ) - mean[t] );
        sigma[t] = float( ::sqrt( double( sigma[t] ) ) ) / float( ::sqrt(  npoint ) );
      }
    }
    else
    {
      for ( t = 0; t < 6; t++ )
      {
        for ( it = bucket[0].begin(); it != bucket[0].end(); it++ )
          if ( it->value().category() <= DtiTensor::NO_PROBLEM )
            if ( sqr( float( it->location().item( 0 ) ) - float( centerX ) ) +
                sqr( float( it->location().item( 1 ) ) - float( centerY ) ) +
                sqr( float( it->location().item( 2 ) ) - float( centerZ ) )
              < sqr( radius ) )
              sigma[t] += ( it->value().base().coef().item( t ) - mean[t] ) *
                          ( it->value().base().coef().item( t ) - mean[t] );
        sigma[t] = float( ::sqrt( double( sigma[t] ) ) ) / float( ::sqrt(  npoint ) );
      }
    }

    if ( fileout.empty() )
    {
      cout << "number of point(s) : " << npoint << endl;
      cout << "meanXX             : " << mean[0] << endl;
      cout << "meanXY             : " << mean[1] << endl;
      cout << "meanXZ             : " << mean[2] << endl;
      cout << "meanYY             : " << mean[3] << endl;
      cout << "meanYZ             : " << mean[4] << endl;
      cout << "meanZZ             : " << mean[5] << endl;
      cout << "powerXX            : " << power[0] << endl;
      cout << "powerXY            : " << power[1] << endl;
      cout << "powerXZ            : " << power[2] << endl;
      cout << "powerYY            : " << power[3] << endl;
      cout << "powerYZ            : " << power[4] << endl;
      cout << "powerZZ            : " << power[5] << endl;
      cout << "sigmaXX            : " << sigma[0] << endl;
      cout << "sigmaXY            : " << sigma[1] << endl;
      cout << "sigmaXZ            : " << sigma[2] << endl;
      cout << "sigmaYY            : " << sigma[3] << endl;
      cout << "sigmaYZ            : " << sigma[4] << endl;
      cout << "sigmaZZ            : " << sigma[5] << endl;
      cout << "SNRXX              : " << power[0] / sqr( sigma[0] ) << "  "
          << 10.0 * log ( power[0] / sqr( sigma[0] ) ) << "dB" << endl;
      cout << "SNRXY              : " << power[1] / sqr( sigma[1] ) << "  "
          << 10.0 * log ( power[1] / sqr( sigma[1] ) ) << "dB" << endl;
      cout << "SNRXZ              : " << power[2] / sqr( sigma[2] ) << "  "
          << 10.0 * log ( power[2] / sqr( sigma[2] ) ) << "dB" << endl;
      cout << "SNRYY              : " << power[3] / sqr( sigma[3] ) << "  "
          << 10.0 * log ( power[3] / sqr( sigma[3] ) ) << "dB" << endl;
      cout << "SNRYZ              : " << power[4] / sqr( sigma[4] ) << "  "
          << 10.0 * log ( power[4] / sqr( sigma[4] ) ) << "dB" << endl;
      cout << "SNRZZ              : " << power[5] / sqr( sigma[5] ) << "  "
          << 10.0 * log ( power[5] / sqr( sigma[5] ) ) << "dB" << endl;
    }
    else
    {
      ofstream os( fileout.c_str() );
      os << "number of point(s) : " << npoint << endl;
      os << "meanXX             : " << mean[0] << endl;
      os << "meanXY             : " << mean[1] << endl;
      os << "meanXZ             : " << mean[2] << endl;
      os << "meanYY             : " << mean[3] << endl;
      os << "meanYZ             : " << mean[4] << endl;
      os << "meanZZ             : " << mean[5] << endl;
      os << "powerXX            : " << power[0] << endl;
      os << "powerXY            : " << power[1] << endl;
      os << "powerXZ            : " << power[2] << endl;
      os << "powerYY            : " << power[3] << endl;
      os << "powerYZ            : " << power[4] << endl;
      os << "powerZZ            : " << power[5] << endl;
      os << "sigmaXX            : " << sigma[0] << endl;
      os << "sigmaXY            : " << sigma[1] << endl;
      os << "sigmaXZ            : " << sigma[2] << endl;
      os << "sigmaYY            : " << sigma[3] << endl;
      os << "sigmaYZ            : " << sigma[4] << endl;
      os << "sigmaZZ            : " << sigma[5] << endl;
      os << "SNRXX              : " << power[0] / sqr( sigma[0] ) << "  "
        << 10.0 * log ( power[0] / sqr( sigma[0] ) ) << "dB" << endl;
      os << "SNRXY              : " << power[1] / sqr( sigma[1] ) << "  "
        << 10.0 * log ( power[1] / sqr( sigma[1] ) ) << "dB" << endl;
      os << "SNRXZ              : " << power[2] / sqr( sigma[2] ) << "  "
        << 10.0 * log ( power[2] / sqr( sigma[2] ) ) << "dB" << endl;
      os << "SNRYY              : " << power[3] / sqr( sigma[3] ) << "  "
        << 10.0 * log ( power[3] / sqr( sigma[3] ) ) << "dB" << endl;
      os << "SNRYZ              : " << power[4] / sqr( sigma[4] ) << "  "
        << 10.0 * log ( power[4] / sqr( sigma[4] ) ) << "dB" << endl;
      os << "SNRZZ              : " << power[5] / sqr( sigma[5] ) << "  "
        << 10.0 * log ( power[5] / sqr( sigma[5] ) ) << "dB" << endl;
      os.close();
    }

    return EXIT_SUCCESS;
  }
  catch( user_interruption & )
  {
  }
  catch( exception & e )
  {
    cerr << e.what() << endl;
  }
  return EXIT_FAILURE;
}
