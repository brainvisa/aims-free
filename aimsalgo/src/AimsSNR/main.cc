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
#include <aims/io/io_g.h>
#include <aims/data/data_g.h>
#include <aims/getopt/getopt2.h>
#include <aims/math/math_g.h>
#include <aims/utility/converter_volume.h>
#include <iostream>
#include <assert.h>

using namespace aims;
using namespace carto;
using namespace std;

template<class T>
static bool doit( Process &, const string &, Finder & );


class SNR : public Process
{
public:
  SNR( AimsData<::byte> & mk, short cx, short cy, short cz, float r, bool lf,
       const string & fout );

private:
  template<class T>
  friend bool doit( Process &, const string &, Finder & );
  bool snr( AimsData<float> & );

  AimsData<::byte>	& mask;
  short			centerX;
  short			centerY;
  short			centerZ;
  float			radius;
  bool		        lineflag;
  string		fileout;
};


SNR::SNR( AimsData<::byte> & mk, short cx, short cy, short cz, float r, bool lf,
          const string & fout )
    : Process(), mask( mk ), centerX( cx ), centerY( cy ), centerZ( cz ), 
      radius( r ), lineflag(lf), fileout( fout )
{
  registerProcessType( "Volume", "S8", &doit<int8_t> );
  registerProcessType( "Volume", "U8", &doit<uint8_t> );
  registerProcessType( "Volume", "S16", &doit<int16_t> );
  registerProcessType( "Volume", "U16", &doit<uint16_t> );
  registerProcessType( "Volume", "S32", &doit<int32_t> );
  registerProcessType( "Volume", "U32", &doit<uint32_t> );
  registerProcessType( "Volume", "FLOAT", &doit<float> );
  registerProcessType( "Volume", "DOUBLE", &doit<double> );
}


template<class T>
bool doit( Process & p, const string & fname, Finder & f )
{
  SNR			& sp = (SNR &) p;
  string		format = f.format();
  Reader<AimsData<T> >	r( fname );
  AimsData<T>		inT;

  if( !r.read( inT, 0, &format ) )
    return false;

  Converter<VolumeRef<T>, VolumeRef<float> >	conv;
  VolumeRef<float>		*in = conv( inT );
  AimsData<float> ind( *in );
  bool				res = sp.snr( ind );
  delete in;
  return res;
}


bool SNR::snr( AimsData<float> & in )
{
  int x = 0, y = 0, z = 0, npoint = 0;

  //
  // mean and power
  //
  float mean = 0.0, power = 0.0;
  bool	hasmask = ( mask.dimX() != 1 ) || ( mask.dimY() != 1 ) 
    || ( mask.dimZ() != 1 );

  if( hasmask )
  {
    ForEach3d( in, x, y, z )
      if ( mask( x, y, z ) )
      {
        npoint++;
        mean += in( x, y, z );
        power += sqr( in( x, y, z ) ); 
      }
    assert( npoint > 0 );
    mean /= float( npoint );
    power /= float( npoint );
  }
  else
  {
    ForEach3d( in, x, y, z )
      if ( sqr( float( x ) - float( centerX ) ) +
           sqr( float( y ) - float( centerY ) ) +
           sqr( float( z ) - float( centerZ ) ) < sqr( radius ) )
      {
        npoint++;
        mean += in( x, y, z );
        power += sqr( in( x, y, z ) ); 
      }
    assert( npoint > 0 );
    mean /= float( npoint );
    power /= float( npoint );
  }


  //
  // standard deviation
  //
  float sigma=0.0;

  if ( hasmask )
  {
    ForEach3d( in, x, y, z )
      if ( mask( x, y, z ) )
        sigma += ( in( x, y, z ) - mean ) * 
                 ( in( x, y, z ) - mean );
    sigma = float( ::sqrt( double( sigma ) ) ) / float( ::sqrt(  npoint ) );
  }
  else
  {
    ForEach3d( in, x, y, z )
      if ( sqr( float( x ) - float( centerX ) ) +
           sqr( float( y ) - float( centerY ) ) +
           sqr( float( z ) - float( centerZ ) ) < sqr( radius ) )
        sigma += ( in( x, y, z ) - mean ) * 
                 ( in( x, y, z ) - mean );
    sigma = float( ::sqrt( double( sigma ) ) ) / float( ::sqrt(  npoint ) );
  }

  
  if ( fileout.empty() )
    {
      if (!lineflag)
	{
	  cout << "number of point(s) : " << npoint << endl;
	  cout << "mean               : " << mean << endl;
	  cout << "power              : " << power << endl;
	  cout << "standard deviation : " << sigma << endl;
	  cout << "SNR                : " << power / sqr( sigma  ) << "  "
	       <<  10.0 * log ( power / sqr( sigma ) ) << "dB" << endl;
	}
      else
	{
	  cout  << npoint << " " 
		<<  mean  <<  " "  
		<< power  <<  " " 
		<< sigma <<  " " 
		<< power / sqr( sigma  ) << "  "
		<<  10.0 * log ( power / sqr( sigma ) )  << endl;
	}
    }
  else
    {
      ofstream os( fileout.c_str() );

      if (!lineflag)
	{
	  os << "number of point(s) : " << npoint << endl;
	  os << "mean               : " << mean << endl;
	  os << "power              : " << power << endl;
	  os << "standard deviation : " << sigma << endl;
	  os << "SNR                : " << power / sqr( sigma  ) << "  "
	     <<  10.0 * log ( power / sqr( sigma ) ) << "dB" << endl;
	}
      else
	{    
	  os  << npoint << " " 
	      <<  mean  <<  " "  
	      << power  <<  " " 
	      << sigma <<  " " 
	      << power / sqr( sigma  ) << "  "
	      <<  10.0 * log ( power / sqr( sigma ) )  << endl;
    
	  os.close();
	}
    }

  return( true );
}


int main( int argc, const char **argv )
{
  string filein, filemask, fileout;
  short centerX = -1, centerY = -1, centerZ = -1;
  float radius = -1;
  bool   lineFlag = false;

  AimsApplication app( argc, argv, "SNR of a VOI in a 3D image" );
  app.addOption( filein, "-i", "3D volume file" );
  app.alias( "--input", "-i" );
  app.addOption( filemask, "-v", "U08 mask file of the V.O.I.", true );
  app.alias( "--voi", "-v" );
  app.addOption( centerX, "-x", "X coord of center of a spherical ROI", true );
  app.addOption( centerY, "-y", "Y coord of center of a spherical ROI", true );
  app.addOption( centerZ, "-z", "Z coord of center of a spherical ROI", true );
  app.addOption( radius, "-R", "radius of a spherical ROI", true );
  app.addOption( fileout, "-o", "output file [default=stdout]", true );
  app.alias( "--output", "-o" );
  app.addOption( lineFlag, "--line",
                 "format of the output file [default=detailed]\n"
                 "detailed in two columns or just a line a value", true );

  try
  {
    app.initialize();

    if( !filemask.empty() )
      assert( centerX == -1 && centerY == -1 && centerZ == -1 );
    else
      assert( centerX >= 0 && centerY >= 0 && centerZ >= 0 && radius >= 0 );

    AimsData< ::byte > mask;
    if( !filemask.empty() )
    {
      Reader<AimsData<::byte> > dataR1( filemask );
      dataR1 >> mask;
    }

    assert( mask.dimT() == 1 );

    SNR	proc( mask, centerX, centerY, centerZ, radius, lineFlag, fileout );
    if ( !proc.execute( filein ) )
    {
      cerr << "Could not process\n";
      return EXIT_FAILURE;
    }
  }
  catch( user_interruption & )
  {
  }
  catch( exception & e )
  {
    cerr << e.what() << endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS ;
}
