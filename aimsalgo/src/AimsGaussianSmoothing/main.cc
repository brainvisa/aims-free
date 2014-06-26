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
#include <aims/getopt/getoptProcess.h>
#include <aims/utility/utility_g.h>
#include <aims/signalfilter/signalfilter_g.h>
#include <cstdlib>

using namespace aims;
using namespace carto;
using namespace std;


template<class T>
static bool gauss( Process & p, const string & filename, Finder & f );


class GaussFilt : public Process
{
public:
  GaussFilt( const string & fout="", float xs=0., float ys=0., float zs=0. );

  template<class T>
  friend bool gauss( Process & p, const string & filename, Finder & f );
  template<class T>
  bool gauss_m( AimsData<T> & data, const string & filename,
                aims::Finder & f );

  string fileout;
  float xsigma;
  float ysigma;
  float zsigma;
};


GaussFilt::GaussFilt( const string & fout, float xs, float ys, float zs )
  : Process(), fileout( fout ), xsigma( xs ), ysigma( ys ), zsigma( zs )
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
  GaussFilt	& dp = (GaussFilt &) p;
  AimsData<T>		data;
  return( dp.gauss_m( data, filename, f ) );
}


template<class T> 
bool GaussFilt::gauss_m( AimsData<T> & data, const string & filename, 
                         aims::Finder & f )
{
  const Header		*h = f.header();
  const PythonHeader	*hdr 
    = dynamic_cast<const PythonHeader *>( h );

  if( !hdr )
    {
      cerr << "file header is incorrect (wrong file ?)\n";
      return( false );
    }

  vector<float>	vs;
  if( !hdr->getProperty( "voxel_size", vs ) )
    {
      cerr << "file header doesn't provide voxel size - taking (1,1,1)\n";
      vs.push_back( 1 );
      vs.push_back( 1 );
      vs.push_back( 1 );
    }

  float	stmp = ( vs[0] > vs[1] ) ? vs[0] : vs[1];
  stmp = ( vs[2] > stmp ) ? vs[2] : stmp;

  if ( xsigma <= 0.0f )  xsigma = stmp;
  if ( ysigma <= 0.0f )  ysigma = xsigma;
  if ( zsigma <= 0.0f )  zsigma = xsigma;

  Reader<AimsData<T> >	reader( filename );
  string		format = f.format();
  if( !reader.read( data, 0, &format ) )
    return( false );

  AimsData<T> out;
  if( data.dimZ() != 1 )
  {
    Gaussian3DSmoothing<T>	g3ds( xsigma, ysigma, zsigma );
    out = g3ds.doit( data );
  }
  else
  {
    Gaussian2DSmoothing<T>	g2ds( xsigma, ysigma );
    out = g2ds.doit( data );
  }

  Writer<AimsData<T> >	writer( fileout );
  return( writer.write( out ) );
}


int main( int argc, const char **argv )
{
  GaussFilt     proc;
  ProcessInput filein( proc );

  AimsApplication app( argc, argv,
    "3D Deriche's recursive gaussian smoothing filter" );

  app.addOption( filein, "-i", "source volume" );
  app.addOption( proc.fileout, "-o", "destination volume" );
  app.addOption( proc.xsigma, "-x",
    "X standard deviation of the gaussian filter [default=largest voxel size]",
    true );
  app.addOption( proc.ysigma, "-y",
    "Y standard deviation of the gaussian filter [default=largest voxel size]",
    true );
  app.addOption( proc.zsigma, "-z",
    "Z standard deviation of the gaussian filter [default=largest voxel size]",
    true );

  app.alias( "--input", "-i" );
  app.alias( "--output", "-o" );
  app.alias( "--xsigma", "-x" );
  app.alias( "--ysigma", "-y" );
  app.alias( "--zsigma", "-z" );

  try
  {
    app.initialize();

    proc.execute( filein.filename );

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
