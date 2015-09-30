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
#include "postscript.h"
#include <aims/getopt/getopt.h>
#include <aims/rgb/rgb.h>
#include <aims/io/finder.h>
#include <aims/io/reader.h>
#include <aims/io/process.h>
#include <aims/data/data.h>
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h> // for JPEG
#ifdef WIN32
  #include <windows.h>
#endif

extern "C"
{
#include <jpeglib.h>
}

using namespace aims;
using namespace std;


BEGIN_USAGE(usage)
  "-------------------------------------------------------------------------",
  "AimsData2Jpg -i[nput] <input>                                            ",
  "             -o[utput] <output>                                          ",
  "             -d[ir] <dir>                                                ",
  "            [-s[lice] <slice>]                                           ",
  "            [-t[ime] <time> ]                                            ",
  "            [-b[egin] <begin>]                                           ",
  "            [-e[nd] <end>]                                               ",
  "            [-m[in] <minimum>]                                           ",
  "            [-M[ax] <maximum>]                                           ",
  "            [-c[olormap] <cmap>]                                         ",
  "            [-I[ncrement] <inc>]                                         ",
  "            [-h[elp]]                                                    ",
  "-------------------------------------------------------------------------",
  " JPEG or PostScript grab on data                                         ",
  " ( S8, U8, S16, U16, S32, U32, FLOAT, DOUBLE )                           ",
  "-------------------------------------------------------------------------",
  "     input         : input data file                                     ",
  "     output        : output file name for *.jpg or *.ps file             ",
  "     dir           : direction of the slice ('x', 'y' or 'z')            ",
  "     slice         : index of the slice to grab [default=all slices]     ",
  "     time          : time of the volume where to select slice [default=0]",
  "     begin         : start the grab from the beginth slice [default=0]   ",
  "     end           : stop the grab at the endth slice [default=dim-1]    ",
  "     minimum       : lower float threshold on data                       ",
  "     maximum       : upper float threshold on data                       ",
  "     cmap          : color map file [default=greyscale]                  ",
  "     inc           : increment of slices between two grabs               ",
  "                                                                         ",
  "     Structure of the cmap file :                                        ",
  "                ncolor          |=> number of entries                    ",
  "                R G B           |=> 1st color                            ",
  "                R G B           |=> 2nd color                            ",
  "                . . .                                                    ",
  "                . . .                                                    ",
  "                . . .                                                    ",
  "                . . .                                                    ",
  "                R G B           |=> last color                           ",
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
static void Usage( void )
{
  AimsUsage( usage );
}


//
// save2Jpeg
//
static void save2Jpeg( const AimsData<int>& buffer, const ColorMap& cmap, 
		       const string & fileOut )
{
  int dimx = buffer.dimX();
  int dimy = buffer.dimY();

  byte* pixels = new byte[ dimx * dimy * 3 ];
  ASSERT( pixels );

  byte* ptr = pixels;
  for ( int y = 0; y < dimy; y++ )
    for ( int x = 0; x < dimx; x++ )
    {
      *ptr++ = (*cmap.rgb)[ buffer(x,dimy - 1 - y) ].red();
      *ptr++ = (*cmap.rgb)[ buffer(x,dimy - 1 - y) ].green();
      *ptr++ = (*cmap.rgb)[ buffer(x,dimy - 1 - y) ].blue();
    }

  string outname = string( fileOut ) + ".jpg";
  FILE *fp = fopen( outname.c_str(), "wb" );
  ASSERT( fp );

  jpeg_compress_struct cinfo;
  jpeg_error_mgr jerr;

  cinfo.err = jpeg_std_error( &jerr );
  jpeg_create_compress( &cinfo );

  jpeg_stdio_dest( &cinfo, fp );
  cinfo.image_width  = dimx;
  cinfo.image_height = dimy;
  cinfo.input_components = 3;
  cinfo.in_color_space = JCS_RGB;
  jpeg_set_defaults( &cinfo );
  jpeg_set_quality( &cinfo, 100, TRUE );
  jpeg_start_compress( &cinfo, TRUE );

  int row_stride = cinfo.image_width * 3;
  JSAMPROW row_pointer[1];

  while ( cinfo.next_scanline < cinfo.image_height )
  { row_pointer[0] = & ((JSAMPLE*)pixels)[ 3 * dimx * dimy - row_stride
                                           * ( cinfo.next_scanline + 1 ) ];
    jpeg_write_scanlines( &cinfo, row_pointer, 1 );
  }

  jpeg_finish_compress( &cinfo );
  fclose( fp );
  jpeg_destroy_compress( &cinfo );

  delete[] pixels;
}


//
// save2Postscript
//
void save2Postscript( const AimsData<int>& buffer, const ColorMap& cmap, 
                      const string & fileOut )
{
  int dimx = buffer.dimX();
  int dimy = buffer.dimY();

  byte* pixels = new byte[ dimx * dimy * 3 ];
  ASSERT( pixels );

  byte* ptr = pixels;
  for ( int y = 0; y < dimy; y++ )
    for ( int x = 0; x < dimx; x++ )
    {
      *ptr++ = (*cmap.rgb)[ buffer(x,dimy - 1 - y) ].red();
      *ptr++ = (*cmap.rgb)[ buffer(x,dimy - 1 - y) ].green();
      *ptr++ = (*cmap.rgb)[ buffer(x,dimy - 1 - y) ].blue();
    }

  string outname = string( fileOut ) + ".ps";

  writePostscript( pixels, dimx, dimy, 0, 0, true, outname );

  delete[] pixels;
}


static void save( const AimsData<int>& buffer, const ColorMap& cmap, 
		  const string & fileOut, bool psmode )
{
  if( psmode )
    save2Postscript( buffer, cmap, fileOut );
  else
    save2Jpeg( buffer, cmap, fileOut );
}


//
// createBuffer
//
template <class T>
void createBuffer( const AimsData<T>& in, float& minimum, float& maximum,
                   int slice, int time, const string& direction,
                   const ColorMap& cmap, const string & fileOut,
                   int begin, int end, int inc, bool psmode )
{
  ASSERT( time >= 0 && time < in.dimT() );

  if ( minimum < -1e+37 ) minimum = float( in.minimum() );
  if ( maximum > +1e+37 ) maximum = float( in.maximum() );
  ASSERT( minimum != maximum );

  cout << "minimum   : " << minimum << endl;
  cout << "maximum   : " << maximum << endl;
  cout << "direction : " << direction << endl;
  cout << "grabbing  : " << flush;

  if ( direction == "x" )
  {
    if ( slice == -1 )
    {
      AimsData<int> buffer( in.dimY(), in.dimZ() );
      if ( end == -1 )
        end = in.dimX() - 1;
      begin = max( begin, 0 );
      end   = min( end, in.dimX() - 1 );
      for ( int x = begin; x <= end; x+=inc )
      {
        for ( int z = 0; z < in.dimZ(); z++ )
          for ( int y = 0; y < in.dimY(); y++ )
            if ( float( in(x,y,z,time) ) <= minimum )
              buffer( y, z ) = 0;
            else if ( float( in(x,y,z,time) ) >= maximum )
              buffer( y, z ) = int( cmap.size - 1 );
            else
              buffer( y, z ) = int( cmap.size * ( in(x,y,z,time) - minimum )
                                              / ( maximum - minimum ) );
        char filename[256];
        sprintf( filename, "%s_%03d", fileOut.c_str(), x );
        save( buffer, cmap, filename, psmode );
      }
    }
    else
    {
      ASSERT( slice >= 0 && slice < in.dimX() );
      AimsData<int> buffer( in.dimY(), in.dimZ() );
      for ( int z = 0; z < in.dimZ(); z++ )
        for ( int y = 0; y < in.dimY(); y++ )
          if ( float( in(slice,y,z,time) ) <= minimum )
            buffer( y, z ) = 0;
          else if ( float( in(slice,y,z,time) ) >= maximum )
            buffer( y, z ) = int( cmap.size - 1 );
          else
            buffer( y, z ) = int( cmap.size * ( in(slice,y,z,time) - minimum )
                                            / ( maximum - minimum ) );
      save( buffer, cmap, fileOut, psmode );
    }
  }
  else if ( direction == "y" )
  {
    if ( slice == -1 )
    {
      AimsData<int> buffer( in.dimX(), in.dimZ() );
      if ( end == -1 )
        end = in.dimY() - 1;
      begin = max( begin, 0 );
      end   = min( end, in.dimY() - 1 );
      for ( int y = begin; y <= end; y+=inc )
      {
        for ( int z = 0; z < in.dimZ(); z++ )
          for ( int x = 0; x < in.dimX(); x++ )
            if ( float( in(x,y,z,time) ) <= minimum )
              buffer( x, z ) = 0;
            else if ( float( in(x,y,z,time) ) >= maximum )
              buffer( x, z ) = int( cmap.size - 1 );
            else
              buffer( x, z ) = int( cmap.size * ( in(x,y,z,time) - minimum )
                                              / ( maximum - minimum ) );
        char filename[256];
        sprintf( filename, "%s_%03d", fileOut.c_str(), y );
        save( buffer, cmap, filename, psmode );
      }
    }
    else
    {
      ASSERT( slice >= 0 && slice < in.dimY() );
      AimsData<int> buffer( in.dimX(), in.dimZ() );
      for ( int z = 0; z < in.dimZ(); z++ )
        for ( int x = 0; x < in.dimX(); x++ )
          if ( float( in(x,slice,z,time) ) <= minimum )
            buffer( x, z ) = 0;
          else if ( float( in(x,slice,z,time) ) >= maximum )
            buffer( x, z ) = int( cmap.size - 1 );
          else
            buffer( x, z ) = int( cmap.size * ( in(x,slice,z,time) - minimum )
                                            / ( maximum - minimum ) );
      save( buffer, cmap, fileOut, psmode );
    }
  }
  else if ( direction == "z" )
  {
    if ( slice == -1 )
    {
      AimsData<int> buffer( in.dimX(), in.dimY() );
      if ( end == -1 )
        end = in.dimZ() - 1;
      begin = max( begin, 0 );
      end   = min( end, in.dimZ() - 1 );
      for ( int z = begin; z <= end; z+=inc )
      {
        for ( int y = 0; y < in.dimY(); y++ )
          for ( int x = 0; x < in.dimX(); x++ )
            if ( float( in(x,y,z,time) ) <= minimum )
              buffer( x, y ) = 0;
            else if ( float( in(x,y,z,time) ) >= maximum )
              buffer( x, y ) = int( cmap.size - 1 );
            else
              buffer( x, y ) = int( cmap.size * ( in(x,y,z,time) - minimum )
                                              / ( maximum - minimum ) );
        char filename[256];
        sprintf( filename, "%s_%03d", fileOut.c_str(), z );
        save( buffer, cmap, filename, psmode );
      }
    }
    else
    {
      ASSERT( slice >= 0 && slice < in.dimZ() );
      AimsData<int> buffer( in.dimX(), in.dimY() );
      for ( int y = 0; y < in.dimY(); y++ )
        for ( int x = 0; x < in.dimX(); x++ )
          if ( float( in(x,y,slice,time) ) <= minimum )
            buffer( x, y ) = 0;
          else if ( float( in(x,y,slice,time) ) >= maximum )
            buffer( x, y ) = int( cmap.size - 1 );
          else
            buffer( x, y ) = int( cmap.size * ( in(x,y,slice,time) - minimum )
                                            / ( maximum - minimum ) );
      save( buffer, cmap, fileOut, psmode );
    }
  }
  cout << "done" << endl;
}


template<class T>
static bool doit( Process &, const string &, Finder & );


class JpegConv : public Process
{
public:
  JpegConv( float min, float max, int sl, int t, const string & dir, 
	    ColorMap & cm, const string & fout, int beg, int en, 
	    int incr, bool ps ) ;
  template<class T> bool convert( T & data );

private:
  float 	minimum;
  float 	maximum;
  int		slice;
  int		time;
  string	direction;
  ColorMap	& cmap;
  string	fileout;
  int		begin;
  int		end;
  int		inc;
  bool		psmode;
};


JpegConv::JpegConv( float min, float max, int sl, int t, const string & dir, 
          ColorMap & cm, const string & fout, int beg, int en, 
          int incr, bool ps ) 
  : Process(), minimum( min ), maximum( max ), slice( sl ), time( t ), 
    direction( dir ), cmap( cm ), fileout( fout ), begin( beg ), end( en ), 
    inc( incr ), psmode( ps )
{
  registerProcessType( "Volume", "S8", &doit<AimsData<int8_t> > );
  registerProcessType( "Volume", "U8", &doit<AimsData<uint8_t> > );
  registerProcessType( "Volume", "S16", &doit<AimsData<int16_t> > );
  registerProcessType( "Volume", "U16", &doit<AimsData<uint16_t> > );
  registerProcessType( "Volume", "S32", &doit<AimsData<int32_t> > );
  registerProcessType( "Volume", "U32", &doit<AimsData<uint32_t> > );
  registerProcessType( "Volume", "FLOAT", &doit<AimsData<float> > );
  registerProcessType( "Volume", "DOUBLE", &doit<AimsData<double> > );
}


template<class T>
bool doit( Process & p, const string & fname, Finder & f )
{
  JpegConv	& jp = (JpegConv &) p;
  T		data;
  Reader<T>	r( fname );
  string	format = f.format();
  if( !r.read( data, 0, &format ) )
    return false;
  return jp.convert( data );
}


template<class T> bool JpegConv::convert( T & data )
{
  createBuffer( data, minimum, maximum, slice, time, direction, cmap, fileout,
		begin, end, inc, psmode );
  return true;
}


//
// main
//
int main( int argc, char **argv )
{
  char *fileIn = NULL, *fileOut = NULL, *dir = NULL,
       *colorMap = NULL;
  int slice = -1, time = 0, inc = 1, begin = -1, end = -1;
  float minimum = -1e38, maximum = +1e38;
  ColorMap cmap;

  //
  // Parser of options
  //
  AimsOption opt[] = {
  { 'h',"help"         ,AIMS_OPT_FLAG  ,( void* )Usage            ,AIMS_OPT_CALLFUNC,0},
  { 'i',"input"        ,AIMS_OPT_STRING,&fileIn          ,0                ,1},
  { 'o',"output"       ,AIMS_OPT_STRING,&fileOut         ,0                ,1},
  { 'd',"dir"          ,AIMS_OPT_STRING,&dir             ,0                ,1},
  { 's',"slice"        ,AIMS_OPT_INT   ,&slice           ,0                ,0},
  { 't',"time"         ,AIMS_OPT_INT   ,&time            ,0                ,0},
  { 'b',"begin"        ,AIMS_OPT_INT   ,&begin           ,0                ,0},
  { 'e',"end"          ,AIMS_OPT_INT   ,&end             ,0                ,0},
  { 'm',"min"          ,AIMS_OPT_FLOAT ,&minimum         ,0                ,0},
  { 'M',"Max"          ,AIMS_OPT_FLOAT ,&maximum         ,0                ,0},
  { 'c',"colormap"     ,AIMS_OPT_STRING,&colorMap        ,0                ,0},
  { 'I',"Increment"    ,AIMS_OPT_INT   ,&inc             ,0                ,0},
  { 0  ,0              ,AIMS_OPT_END   ,0                ,0                ,0}};

  AimsParseOptions( &argc, argv, opt, usage );

  string direction = string( dir );
  ASSERT( direction == "x" || direction == "y" || direction == "z" );

  if ( colorMap != NULL )
  {
    string tmp;
    int tp;
    ifstream is( colorMap );
    ASSERT( is );

    is >> cmap.name;
    is >> cmap.size;
    cmap.rgb = new vector<AimsRGB>( cmap.size );

    is >> tmp;
    ASSERT( tmp == "Red" );
    for ( int k = 0; k < cmap.size; k++ )
    {
      is >> tp;
      (*cmap.rgb)[ k ].red() = byte( tp );
    }

    is >> tmp;
    ASSERT( tmp == "Green" );
    for ( int k = 0; k < cmap.size; k++ )
    {
      is >> tp;
      (*cmap.rgb)[ k ].green() = byte( tp );
    }

    is >> tmp;
    ASSERT( tmp == "Blue" );
    for ( int k = 0; k < cmap.size; k++ )
    {
      is >> tp;
      (*cmap.rgb)[ k ].blue() = byte( tp );
    }

    is.close();
  }
  else
  {
    cmap.name = "AIMS_GRAYSCALE";
    cmap.size = 256;
    cmap.rgb  = new vector<AimsRGB>( 256 );
    for ( int k = 0; k < cmap.size; k++ )
      (*cmap.rgb)[ k ] = AimsRGB( byte( k ), byte( k ), byte( k ) );
  }
  cout << "color map  : " << cmap.name << endl;
  cout << "color size : " << cmap.size << endl;


  if ( begin == -1 )
    begin = 0;

  bool		psmode = false;
  string	fileout = fileOut;
  string::size_type	pos = fileout.rfind( '.' );
  if( pos != string::npos )
    {
      string	ext = fileout.substr( pos+1, fileout.length()-pos-1 );
      if( ext == "ps" )
	{
	  psmode = true;
	  fileout.erase( pos, fileout.length() - pos );
	}
      else if( ext == "jpg" )
	fileout.erase( pos, fileout.length() - pos );
    }

  JpegConv	proc( minimum, maximum, slice, time, direction, cmap, fileout,
		      begin, end, inc, psmode );
  if( !proc.execute( fileIn ) )
    {
      cerr << "Couldn't process\n";
      return 1;
    }
  return EXIT_SUCCESS;
}
