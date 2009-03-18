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
#include <aims/io/io_g.h>
#include <aims/data/data_g.h>
#include <aims/getopt/getopt.h>
#include <aims/math/math_g.h>
#include <aims/utility/converter_volume.h>
#include <iostream>
#include <assert.h>

using namespace aims;
using namespace carto;
using namespace std;

BEGIN_USAGE(usage)
  "----------------------------------------------------------------",
  "AimsSNR -i[nput] <filein>                                       ",
  "       [-v[oi] <voi>]                                           ",
  "       [-x <X> -y <Y> -z <Z> -R <R>]                            ",
  "       [-o[utput] <fileout>]                                    ",
  "       [--lineFlag] <output_format>]                                    ",
  "       [-h[elp]]                                                ",
  "----------------------------------------------------------------",
  "SNR of a VOI in a 3D image                                      ",
  "----------------------------------------------------------------",
  "     filein  : origin 3D file                                   ",
  "     voi     : U08 mask file of the V.O.I.                      ",
  "     X,Y,Z   : Point3d center of a spherical ROI                ",
  "     R       : radius of spherical ROI                          ",
  "     fileout : output file [default=stdout]                     ",
  "     output_format : format of the output file [default=detailed]",
  "                     detailed in two columns or just a line a values",
  "----------------------------------------------------------------",
END_USAGE


void Usage( void )
{
  AimsUsage( usage );
}


template<class T>
static bool doit( Process &, const string &, Finder & );


class SNR : public Process
{
public:
  SNR( AimsData<byte> & mk, short cx, short cy, short cz, float r, int lf, const string & fout );

private:
  template<class T>
  friend bool doit( Process &, const string &, Finder & );
  bool snr( AimsData<float> & );

  AimsData<byte>	& mask;
  short			centerX;
  short			centerY;
  short			centerZ;
  float			radius;
  int		        lineflag;
  string		fileout;
};


SNR::SNR( AimsData<byte> & mk, short cx, short cy, short cz, float r, int lf,
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

  Converter<AimsData<T>, AimsData<float> >	conv;
  AimsData<float>		*in = conv( inT );
  bool				res = sp.snr( *in );
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


int main( int argc, char **argv )
{
  char *filein, *filemask = NULL, *fileout = NULL;
  short centerX = -1, centerY = -1, centerZ = -1;
  float radius=-1; 
  int   lineFlag=0;


  AimsOption opt[] = {
  { 'h',"help"  ,AIMS_OPT_FLAG  ,( void* )Usage       ,AIMS_OPT_CALLFUNC,0,},
  { 'i',"input" ,AIMS_OPT_STRING,&filein     ,0                ,1},
  { 'v',"voi"   ,AIMS_OPT_STRING,&filemask   ,0                ,0},
  { 'x',"x"     ,AIMS_OPT_SHORT ,&centerX    ,0                ,0},
  { 'y',"y"     ,AIMS_OPT_SHORT ,&centerY    ,0                ,0},
  { 'z',"z"     ,AIMS_OPT_SHORT ,&centerZ    ,0                ,0},
  { 'R',"R"     ,AIMS_OPT_FLOAT ,&radius     ,0                ,0},
  { ' ',"line"  ,AIMS_OPT_FLAG  ,&lineFlag   ,0                ,0},
  { 'o',"output",AIMS_OPT_STRING,&fileout    ,0                ,0},
  { 0  ,0       ,AIMS_OPT_END   ,0           ,0                ,0}};

  AimsParseOptions( &argc, argv, opt, usage );


  if ( filemask )
    assert( centerX == -1 && centerY == -1 && centerZ == -1 );
  else
    assert( centerX >= 0 && centerY >= 0 && centerZ >= 0 && radius >= 0 );

  AimsData< byte > mask;
  if ( filemask )
  {
    Reader<AimsData<byte> > dataR1( filemask );
    dataR1 >> mask;
  }

  assert( mask.dimT() == 1 );

  SNR	proc( mask, centerX, centerY, centerZ, radius, lineFlag, fileout );
  if ( !proc.execute( filein ) )
    {
      cerr << "Could not process\n";
      Usage();
    }
  return EXIT_SUCCESS ;
}
