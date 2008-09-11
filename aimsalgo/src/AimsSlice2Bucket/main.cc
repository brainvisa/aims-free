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


#include <aims/getopt/getopt.h>
#include <aims/rgb/rgb.h>
#include <aims/io/io_g.h>
#include <aims/data/data.h>
#include <aims/bucket/bucket.h>
#include <fstream>
#include <string>
#include <stdio.h>
#include <assert.h>

using namespace aims;
using namespace std;


BEGIN_USAGE(usage)
  "-------------------------------------------------------------------------",
  "AimsSlice2Bucket -i[nput] <input>                                        ",
  "                 -o[utput] <output>                                      ",
  "                 -d[ir] <dir>                                            ",
  "                [-m[in] <minimum>]                                       ",
  "                [-M[ax] <maximum>]                                       ",
  "                [-I[ncrement] <inc>]                                     ",
  "                [-a[scii]]                                               ",
  "                [-h[elp]]                                                ",
  "-------------------------------------------------------------------------",
  " Save slices of a mask to buckets                                        ",
  "-------------------------------------------------------------------------",
  "     input         : input S16 or U8 mask data file                      ",
  "     output        : output file name for *.bck file                     ",
  "     dir           : direction of the slice ('x', 'y' or 'z')            ",
  "     minimum       : lower slice index [default=0]                       ",
  "     maximum       : upper slice index [default=dim-1]                   ",
  "     inc           : increment of slices between two slices [default=1]  ",
  "     ascii         : save buckets in ASCII mode [default=binary]         ",
  "-------------------------------------------------------------------------",
END_USAGE



struct ColorMap
{
  string name;
  int size;
  vector<AimsRGB>* rgb;
};


//
// Usage
//
void Usage( void )
{
  AimsUsage( usage );
}


template<typename T> static bool s2b( Process &, const string &, Finder & );

class Slice2Bck : public Process
{
public:
  Slice2Bck( const string & out, const string & d, int incr, int m, int M, 
	     int asc );
  virtual ~Slice2Bck() {}
  bool s2b_cont( AimsData<byte> & mask );

  string	fileOut;
  string	direction;
  int		inc;
  int		minimum;
  int		maximum;
  int		ascii;
};


Slice2Bck::Slice2Bck( const string & out, const string & d, int incr, int m, 
		      int M, int asc ) 
  : Process(), fileOut( out ), direction( d ), inc( incr ), minimum( m ), 
    maximum( M ), ascii( asc )
{
  registerProcessType( "Volume", "S8", &s2b<int8_t> );
  registerProcessType( "Volume", "U8", &s2b<uint8_t> );
  registerProcessType( "Volume", "S16", &s2b<int16_t> );
  registerProcessType( "Volume", "U16", &s2b<uint16_t> );
  registerProcessType( "Volume", "S32", &s2b<int32_t> );
  registerProcessType( "Volume", "S32", &s2b<uint32_t> );
  registerProcessType( "Volume", "FLOAT", &s2b<float> );
  registerProcessType( "Volume", "DOUBLE", &s2b<double> );
}


bool Slice2Bck::s2b_cont( AimsData<byte> & mask )
{
  if ( minimum == -1 )
    minimum = 0;

  AimsBucket< Void > bucket;
  bucket.setSizeX( mask.sizeX() );
  bucket.setSizeY( mask.sizeY() );
  bucket.setSizeZ( mask.sizeZ() );
  bucket.setSizeT( mask.sizeT() );
  AimsBucketItem< Void > item;
  bool	ok = true;
  int x, y, z;

  cout << "grabing : " << flush;

  if ( direction == "x" )
    {
      if ( maximum == -1 )
	maximum = mask.dimX() - 1;
      minimum = max( minimum, 0 );
      maximum = min( maximum, mask.dimX() - 1 );
      for ( x = minimum; x <= maximum; x+= inc )
	{
	  bucket.erase();
	  for ( z = 0; z < mask.dimZ(); z++ )
	    for ( y = 0; y < mask.dimY(); y++ )
	      if ( mask( x, y, z ) )
		{
		  item.location() = Point3d( x, y, z );
		  bucket.push_back( item );
		}
	  char filename[256];
	  sprintf( filename, "%s_%03d", fileOut.c_str(), x );
	  cout << filename << " " << flush;
	  Writer<AimsBucket< Void > > bucketW( filename );
	  ok &= bucketW.write( bucket, ascii );
	}
    }
  else if ( direction == "y" )
    {
      if ( maximum == -1 )
	maximum = mask.dimY() - 1;
      minimum = max( minimum, 0 );
      maximum = min( maximum, mask.dimY() - 1 );
      for ( y = minimum; y <= maximum; y+= inc )
	{
	  bucket.erase();
	  for ( z = 0; z < mask.dimZ(); z++ )
	    for ( x = 0; x < mask.dimX(); x++ )
	      if ( mask( x, y, z ) )
		{
		  item.location() = Point3d( x, y, z );
		  bucket.push_back( item );
		}
	  char filename[256];
	  sprintf( filename, "%s_%03d", fileOut.c_str(), y );
	  cout << filename << " " << flush;
	  Writer<AimsBucket< Void > > bucketW( filename );
	  ok &= bucketW.write( bucket, ascii );
	}
    }
  else if ( direction == "z" )
    {
      if ( maximum == -1 )
	maximum = mask.dimZ() - 1;
      minimum = max( minimum, 0 );
      maximum = min( maximum, mask.dimZ() - 1 );
      for ( z = minimum; z <= maximum; z+= inc )
	{
	  bucket.erase();
	  for ( y = 0; y < mask.dimY(); y++ )
	    for ( x = 0; x < mask.dimX(); x++ )
	      if ( mask( x, y, z ) )
		{
		  item.location() = Point3d( x, y, z );
		  bucket.push_back( item );
		}
	  char filename[256];
	  sprintf( filename, "%s_%03d", fileOut.c_str(), z );
	  cout << filename << " " << flush;
	  Writer<AimsBucket< Void > > bucketW( filename );
	  ok &= bucketW.write( bucket, ascii );
	}
    }

  if( ok )
    cout << "done" << endl;
  return ok;
}


template<typename T> 
bool s2b( Process & p, const string & fname, Finder & f )
{
  cout << "reading data : " << flush;
  AimsData<T>	in;
  string	format = f.format();
  Reader<AimsData<T> >	r( fname );
  if( !r.read( in, 0, &format ) )
    return false;

  Slice2Bck	& sp = (Slice2Bck &) p;
  AimsData< byte > mask( in.dimX(), in.dimY(), in.dimZ() );
  int x, y, z;
  mask.setSizeXYZT( in.sizeX(), in.sizeY(), in.sizeZ(), in.sizeT() );
  mask = false;
  ForEach3d( mask, x, y, z )
    if ( in( x, y, z ) )
      mask ( x, y, z ) = true;
  cout << "done" << endl;
  return sp.s2b_cont( mask );	// continue with non-template function
}


//
// main
//
int main( int argc, char** argv )
{
  char *fileIn = NULL, *fileOut = NULL, *dir = NULL;
  int inc = 1;
  int minimum = -1, maximum = -1;
  int ascii = 0;

  //
  // Parser of options
  //
  AimsOption opt[] = {
  { 'h',"help"         ,AIMS_OPT_FLAG  ,( void* )Usage   ,AIMS_OPT_CALLFUNC,0},
  { 'i',"input"        ,AIMS_OPT_STRING,&fileIn          ,0                ,1},
  { 'o',"output"       ,AIMS_OPT_STRING,&fileOut         ,0                ,1},
  { 'd',"dir"          ,AIMS_OPT_STRING,&dir             ,0                ,1},
  { 'm',"min"          ,AIMS_OPT_INT   ,&minimum         ,0                ,0},
  { 'M',"Max"          ,AIMS_OPT_INT   ,&maximum         ,0                ,0},
  { 'I',"Increment"    ,AIMS_OPT_INT   ,&inc             ,0                ,0},
  { 'a',"ascii"        ,AIMS_OPT_FLAG  ,&ascii           ,0                ,0},
  { 0  ,0              ,AIMS_OPT_END   ,0                ,0               ,0}};

  AimsParseOptions( &argc, argv, opt, usage );

  string direction = string( dir );
  assert( direction == "x" || direction == "y" || direction == "z" );

  Slice2Bck	proc( fileOut, direction, inc, minimum, maximum, ascii );
  if( !proc.execute( fileIn ) )
    {
      cerr << "Failed\n";
      return 1;
    }
  return EXIT_SUCCESS;
}
