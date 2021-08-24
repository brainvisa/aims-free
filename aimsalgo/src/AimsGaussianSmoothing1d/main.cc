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
#include <aims/io/process.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/io/finder.h>
#include <aims/data/data_g.h>
#include <aims/data/pheader.h>
#include <aims/getopt/getopt2.h>
#include <aims/utility/utility_g.h>
#include <aims/signalfilter/signalfilter_g.h>
#include <cstdlib>

using namespace aims;
using namespace carto;
using namespace std;



template<class T>
static bool gauss( Process & p, const string & filename, Finder & f );


class GaussFilt1d : public Process
{
public:
  GaussFilt1d( const string& fout,
               const string& direction,
               float sigma );

  template<class T>
  friend bool gauss( Process & p, const string & filename, Finder & f );
  template<class T>
  bool gauss_m( AimsData<T> & data, const string & filename, aims::Finder & f );

private:
  string    fileout;
  string    direction;
  float     sigma;
};


GaussFilt1d::GaussFilt1d( const string & fout,
                          const string& dir,
                          float s )
            : Process(),
              fileout( fout ),
              direction( dir ),
              sigma( s )
{
  registerProcessType( "Volume", "S8", &gauss<int8_t> );
  registerProcessType( "Volume", "U8", &gauss<uint8_t> );
  registerProcessType( "Volume", "S16", &gauss<int16_t> );
  registerProcessType( "Volume", "U16", &gauss<uint16_t> );
  registerProcessType( "Volume", "S32", &gauss<int32_t> );
  registerProcessType( "Volume", "U32", &gauss<uint32_t> );
  registerProcessType( "Volume", "FLOAT", &gauss<float> );
  registerProcessType( "Volume", "DOUBLE", &gauss<double> );
}

template<class T> 
bool gauss( Process & p, const string & filename, Finder & f )
{
  GaussFilt1d	& dp = (GaussFilt1d &) p;
  AimsData<T>		data;
  return( dp.gauss_m( data, filename, f ) );
}


template<class T> 
bool GaussFilt1d::gauss_m( AimsData<T> & data, const string & filename, 
			   aims::Finder & f )
{


  if ( direction != "x" &&
       direction != "X" &&
       direction != "y" &&
       direction != "Y" &&
       direction != "z" &&
       direction != "Z" )
    {
      cerr << "direction should be one of x,X,y,Y,z,Z\n";
      return( false );
    }
  

  const Header* h = f.header();
  const PythonHeader	*hdr 
    = dynamic_cast<const PythonHeader *>( h );

  if( !hdr )
    {
      cerr << "file header is incorrect (wrong file ?)\n";
      return( false );
    }


  Reader<AimsData<T> >	reader( filename );
  string		format = f.format();
  if( !reader.read( data, 1, &format ) )
    return( false );

  vector<float>	vs;
  if( !hdr->getProperty( "voxel_size", vs ) )
    {
      cerr << "file header doesn't provide voxel size - taking (1,1,1)\n";
      vs.push_back( 1 );
      vs.push_back( 1 );
      vs.push_back( 1 );
    }

  AimsData<T> out;

  if ( direction == "x" || direction == "X" )
  {

    if ( sigma <= 0.0 )
      sigma = vs[ 0 ];

    AimsData< float > dataF( data.dimX(),
                             data.dimY(),
                             data.dimZ(),
                             data.dimT() );

    Converter< VolumeRef<T>, VolumeRef<float> > conv;
    conv.convert( data, dataF );

    if( data.dimX() != 1 )
    {

      GaussianLines glin;
      glin.doit( dataF, GCoef( sigma ) );

    }

    Converter< VolumeRef<float>, VolumeRef<T> > conv2;
    out = AimsData<T>( data.dimX(),
                       data.dimY(),
                       data.dimZ(),
                       data.dimT() );
    conv2.convert( dataF, out );

  }
  else if ( direction == "y" || direction == "Y" )
  {

    if ( sigma <= 0.0 )
      sigma = vs[ 1 ];

    AimsData< float > dataF( data.dimX(),
                             data.dimY(),
                             data.dimZ(),
                             data.dimT() );

    Converter< VolumeRef<T>, VolumeRef<float> > conv;
    conv.convert( data, dataF );

    if( data.dimY() != 1 )
    {

      GaussianColumns gcol;
      gcol.doit( dataF, GCoef( sigma ) );

    }

    Converter< VolumeRef<float>, VolumeRef<T> > conv2;
    out = AimsData<T>( data.dimX(),
                       data.dimY(),
                       data.dimZ(),
                       data.dimT() );
    conv2.convert( dataF, out );

  }
  else if ( direction == "z" || direction == "Z" )
  {

    if ( sigma <= 0.0 )
      sigma = vs[ 2 ];

    AimsData< float > dataF( data.dimX(),
                             data.dimY(),
                             data.dimZ(),
                             data.dimT() );

    Converter< VolumeRef<T>, VolumeRef<float> > conv;
    conv.convert( data, dataF );

    if( data.dimZ() != 1 )
    {

      GaussianSlices gsli;
      gsli.doit( dataF, GCoef( sigma ) );

    }

    Converter< VolumeRef<float>, VolumeRef<T> > conv2;
    out = AimsData<T>( data.dimX(),
                       data.dimY(),
                       data.dimZ(),
                       data.dimT() );
    conv2.convert( dataF, out );

  }
  	
  Writer<AimsData<T> >	writer( fileout );
  return( writer.write( out ) );

}


int main( int argc, const char **argv )
{
  try
  {
    std::string filein, fileout;
    std::string direction;
    float       sigma = 0.0f;

    AimsApplication app( argc, argv,
                         "1D Deriche's recursive gaussian smoothing filter.\n" );
    app.addOption( filein,       "-i", "Source volume" );
    app.addOption( fileout,      "-o", "Destination volume" );
    app.addOption( direction,    "-d", "Direction of smoothing: "
                                       "x, y, z" );
    app.addOption( sigma,        "-s", "Standard deviation of the gaussian function\n"
                                       "[default = corresponding voxel size]", true );
    app.alias( "--input", "-i" );
    app.alias( "--output", "-o" );
    app.alias( "--direction", "-d" );
    app.alias( "--sigma", "-s" );

    app.initialize();

    GaussFilt1d proc( fileout, direction, sigma );
    if( !proc.execute( filein ) )
    {
        cerr << "Couldn't process\n";
        return( EXIT_FAILURE );
    }
  }
  catch( user_interruption & )
  {
    return( EXIT_FAILURE );
  }
  catch( exception & e )
  {
    cerr << e.what() << endl;
    return( EXIT_FAILURE );
  }

  return EXIT_SUCCESS;
}
