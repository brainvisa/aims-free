/* Copyright (c) 1995-2005 CEA
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

/*
 *  Image flip
 */
#include <aims/data/data_g.h>
#include <aims/io/process.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/io/finder.h>
#include <aims/getopt/getopt2.h>
#include <iostream>
#include <aims/utility/flip.h>

using namespace aims;
using namespace carto;
using namespace std;


template<class T>
static bool flip( Process & p, const string & filename, Finder & f );

class Flip : public Process
{
public:
  Flip( const string & fout, const string& mode );

  template<class T> 
  bool flip_m( AimsData<T> & data, const string & filename, Finder & f );

private:
  string	fileout;
  string        mode ;
};

Flip::Flip( const string & fout, const string& mm )
	      : Process(), fileout( fout ), mode (mm)
{
  registerProcessType( "Volume", "U8", &flip<uint8_t> );
  registerProcessType( "Volume", "S8", &flip<int8_t> );
  registerProcessType( "Volume", "U16", &flip<uint16_t> );
  registerProcessType( "Volume", "S16", &flip<int16_t> );
  registerProcessType( "Volume", "S32", &flip<int32_t> );
  registerProcessType( "Volume", "FLOAT", &flip<float> );
  registerProcessType( "Volume", "DOUBLE", &flip<double> );
}

template<class T> 
bool flip( Process & p, const string & filename, Finder & f )
{
  Flip	& dp = (Flip &) p;
  AimsData<T>		data;
  return( dp.flip_m( data, filename, f ) );
}


template<class T> 
bool Flip::flip_m( AimsData<T> & data, const string & filename, Finder & f )
{
  Reader<AimsData<T> >	reader( filename );
  string	format = f.format();
  if( !reader.read( data, 1, &format ) )
    return( false );
  AimsData<T> outimage ;
  
  AimsFlip<T> flip ;
  
  if( mode == "XX" )
    outimage = flip.doXX(data) ;
  else if( mode == "YY" )
      outimage = flip.doYY(data) ;
  else if( mode == "ZZ" )
      outimage = flip.doZZ(data) ;
  else if( mode == "XXYY" )
      outimage = flip.doXXYY(data) ;
  else if( mode == "XXZZ" )
      outimage = flip.doXXZZ(data) ;
  else if( mode == "YYZZ" )
      outimage = flip.doYYZZ(data) ;
  else if( mode == "XXYYZZ" )
      outimage = flip.doXXYYZZ(data) ;
  else if( mode == "XY" )
      outimage = flip.doXY(data) ;
  else if( mode == "XZ" )
      outimage = flip.doXZ(data) ;
  else if( mode == "YZ" )
      outimage = flip.doYZ(data) ;
  else
      throw runtime_error("Wrong mode") ;

  Writer< AimsData<T> >	writer( fileout );
  return( writer.write( outimage ) );
}


int main( int argc, const char **argv )
{
  try
    {
      string filein,fileout, mode;

      AimsApplication app(argc, argv, "Image flip" ) ;
      app.addOption( filein, "-i", "input image to flip" ) ;
      app.alias( "--input", "-i" );
      app.addOption( fileout, "-o", "output flipped image" ) ;
      app.alias( "--output", "-o" );
      app.addOption( mode, "-m", "flip image XX, YY, ZZ, XXYY, XXZZ, YYZZ, XXYYZZ, or flip axes : XY, XZ, YZ ") ;
  
      app.initialize() ;

      Flip	proc( fileout, mode );
      if( !proc.execute( filein ) )
        {
          cerr << "Couldn't process\n";
          return( 1 );
        }

    }
  catch( user_interruption & )
    {
      return EXIT_FAILURE;
    }
  catch( exception & e )
    {
      cerr << e.what() << endl;
      return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
