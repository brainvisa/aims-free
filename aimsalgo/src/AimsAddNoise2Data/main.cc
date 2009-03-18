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


#define AIMS_PROCESS_TYPES
#define AIMS_PROCESS_VOLSCALAR

#include <cstdlib>
#include <aims/data/data_g.h>
#include <aims/io/finder.h>
#include <aims/io/process.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/getopt/getopt2.h>
#include <aims/math/math_g.h>
#include <stdexcept>
#include <iostream>
#include <assert.h>

using namespace aims;
using namespace carto;
using namespace std;


template<typename T> bool hasminmax( double &, double & )
{
  return( false );
}


template<> bool hasminmax<int8_t>( double & min, double & max )
{
  min = -128;
  max = 127;
  return( true );
}


template<> bool hasminmax<uint8_t>( double & min, double & max )
{
  min = 0;
  max = 255;
  return( true );
}


template<> bool hasminmax<int16_t>( double & min, double & max )
{
  min = -0x8000;
  max = 0x7fff;
  return( true );
}


template<> bool hasminmax<uint16_t>( double & min, double & max )
{
  min = 0;
  max = 0xffff;
  return( true );
}


template<> bool hasminmax<int32_t>( double & min, double & max )
{
  min = -0x80000000;
  max = 0x7fffffff;
  return( true );
}


template<> bool hasminmax<uint32_t>( double & min, double & max )
{
  min = 0;
  max = 0xffffffff;
  return( true );
}


class AddNoise : public Process
{
public:
  AddNoise( float m, float s, float in, float su, int uni, const string & fout );

  float 	mean;
  float		sigma;
  float 	inf;
  float 	sup;
  int		uniform;
  string	fileout;
};


template<class T>
bool addnoise( Process & p, const string & filename, Finder & finder )
{
  AddNoise		& an = (AddNoise &) p;
  AimsData<T>		data;
  Reader<AimsData<T> >	r( filename );
  string		format = finder.format();
  cout << "reading image : " << flush;
  if( !r.read( data, 0, &format ) )
    return( false );
  cout << "done\n";

  AimsData<T>	out( data.dimX(), data.dimY(), data.dimZ(), data.dimT() );
  out.setSizeXYZT( data );

  int		x, y, z, t;
  double	val, min, max;
  bool		hasbounds = hasminmax<T>( min, max );

  cout << "adding noise : " << flush;
  ForEach4d( data, x, y, z, t )
    {
      val = ( an.uniform ? UniformRandom( an.inf, an.sup ) :
              GaussianRandom( an.mean, an.sigma ) ) + data( x, y, z, t );
      if( hasbounds )
      {
        if ( val < min ) val = min;
        else if ( val > max ) val = max;
      }
      out( x, y, z, t ) = T( val );
    }
  cout << "done" << endl;

  cout << "writing data : " << flush;
  Writer<AimsData<T> > dataW( an.fileout );
  dataW.write( out );
  cout << "done" << endl;

  return( true );
}


AddNoise::AddNoise( float m, float s, float in, float su, int uni, 
	    const string & fout ) 
    : Process(), mean( m ), sigma( s ), inf( in ), sup( su ), uniform( uni ), 
      fileout( fout )
{
      registerProcessType( "Volume", "S8", &addnoise<int8_t> );
      registerProcessType( "Volume", "U8", &addnoise<uint8_t> );
      registerProcessType( "Volume", "S16", &addnoise<int16_t> );
      registerProcessType( "Volume", "U16", &addnoise<uint16_t> );
      registerProcessType( "Volume", "S32", &addnoise<int32_t> );
      registerProcessType( "Volume", "U32", &addnoise<uint32_t> );
      registerProcessType( "Volume", "FLOAT", &addnoise<float> );
      registerProcessType( "Volume", "DOUBLE", &addnoise<double> );
}


int main( int argc, const char **argv )
{
  string	filein, fileout;
  float		mean=0, sigma=0, inf=-1, sup=-1;
  bool		uniform = false;

  AimsApplication	app( argc, argv, "Add gaussian or uniform noise to data" );

  app.addOption( filein, "-i", "input data file" );
  app.addOption( fileout, "-o", "output data file" );
  app.addOption( mean, "-m", "mean of the Gaussian distribution [default=0.0]",
                 true );
  app.addOption( sigma, "-s", "standard deviation of the Gaussian "
                 "distribution", true );
  app.addOption( uniform, "-u", "take a uniform distribution rather than "
                 "gaussian", true );
  app.addOption( inf, "-I", "lowest value of uniform distribution", true );
  app.addOption( sup, "-S", "highest value of uniform distribution", true );
  app.alias( "--input", "-i" );
  app.alias( "--output", "-o" );
  app.alias( "--mean", "-m" );
  app.alias( "--sigma", "-s" );
  app.alias( "--uniform", "-u" );
  app.alias( "--Superior", "-S" );
  app.alias( "--Inferior", "-I" );

  try
    {
      app.initialize();

      if ( uniform )
        assert( inf != sup );
      else
        assert( sigma != 0.0 );

      AddNoise		proc( mean, sigma, inf, sup, uniform, fileout );

      if( !proc.execute( filein ) )
        {
          cerr << "Operation not applicable to this data type\n";
          return( 1 );
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
