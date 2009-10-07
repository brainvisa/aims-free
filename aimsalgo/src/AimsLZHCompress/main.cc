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
#include <aims/compression/compression_g.h>
#include <aims/getopt/getopt.h>
#include <iostream>
#include <stdio.h>
#include <time.h>

using namespace std;

BEGIN_USAGE(usage)
  "-------------------------------------------------------------------------",
  "AimsLZHCompress -i[nput]  <filein>                                       ",
  "               [-o[utput] <fileout>]                                     ",
  "               [-d]                                                      ",
  "               [-h[elp]]                                                 ",
  "-------------------------------------------------------------------------",
  "LZH light compressor                                                     ",
  "-------------------------------------------------------------------------",
  "     filein  : source file                                               ",
  "     fileout : destination file  [default=filein.lzh]                    ",
  "     d       : decompress [default=compress]                             ",
  "-------------------------------------------------------------------------",
END_USAGE


#define BLOCKSIZE 4000000


void Usage( void )
{
  AimsUsage( usage );
}


string removeExtension( const string& name )
{
  string res = name;
  string ext="";
  if ( res.length() > 4 )
    ext = res.substr( int(res.length() - 4), 4 );
  if (ext == ".lzh" )
    res = res.substr( 0, res.length() - 4 );
  return res;
}


int main( int argc, char** argv )
{
  char *filein, *fileout = NULL;
  int decompress = 0;

  AimsOption opt[] = {
  { 'h',"help"      ,AIMS_OPT_FLAG  ,( void* )Usage      ,AIMS_OPT_CALLFUNC,0},
  { 'i',"input"     ,AIMS_OPT_STRING,&filein    ,0                ,1},
  { 'o',"output"    ,AIMS_OPT_STRING,&fileout   ,0                ,0},
  { 'd',"decompress",AIMS_OPT_FLAG  ,&decompress,0                ,0},
  { 0  ,0           ,AIMS_OPT_END   ,0          ,0                ,0}};

  AimsParseOptions( &argc, argv, opt, usage );

  if( !decompress )
  {        
    FILE *fp = fopen( filein, "rb" );
    ASSERT( fp );
    fseek( fp, 0, SEEK_END );
    size_t rawLen = (size_t)ftell( fp );
    fseek( fp, 0, SEEK_SET );
    byte* rawBlock = new byte[ rawLen ];
    byte* compBlock = new byte[ LZHLCompressor::calcMaxBuf( rawLen ) ];
    fread( rawBlock, 1, rawLen, fp );
    fclose( fp );

    clock_t start = clock();

    LZHLCompressor compressor;

    int compLen = 0;
    for( size_t i = 0; i < rawLen ; i += BLOCKSIZE )
    {
      int l = ( (BLOCKSIZE <= int(rawLen-i) ) ? BLOCKSIZE : int(rawLen-i) );
      compLen += compressor.compress( ( byte* )( compBlock + compLen ),
                                      ( const byte* )( rawBlock + i ), l );
    }

    clock_t finish = clock();

    cout << finish - start << " ticks" << endl;

    if ( fileout == NULL )
      fileout = filein;

    string outname = string( fileout ) + ".lzh";
    fp = fopen( outname.c_str(), "wb" );
    ASSERT( fp );

    // rawLen is stored as a byte sequence to avoid problems 
    // with little_endian/big_endian
    for ( int i = 0; i < 4 ; i++ )
      fputc( ( byte )( rawLen >> i * 8 ), fp );
    fwrite( compBlock, 1, compLen, fp );
    fclose( fp );

    remove( filein );
  }
  else
  {
    FILE *fp = fopen( filein, "rb" );
    ASSERT( fp );
    fseek( fp, 0, SEEK_END );
    size_t fileLen = (size_t)ftell( fp );
    fseek( fp, 0, SEEK_SET );
    if ( fileLen < 4 )
      abort();
    int rawLen = 0;
    for( int i = 0; i < 4 ; i++ )
      rawLen |= ( fgetc( fp ) << i * 8 );
    int compLen = fileLen - 4;

    byte* rawBlock = new byte[ rawLen ];
    ASSERT( rawBlock );
    byte* compBlock = new byte[ fileLen ];
    ASSERT( compBlock );
    fread( compBlock, 1, fileLen, fp );
    fclose( fp );

    clock_t start = clock();

    size_t srcSz = compLen;
    size_t dstSz = rawLen;

    LZHLDecompressor decompressor;
    for ( ; ; )
    {
      bool Ok = decompressor.decompress( (byte*)( rawBlock + rawLen - dstSz ),
                                         &dstSz,
                                         (byte*)(compBlock + compLen - srcSz ),
                                         &srcSz );
      ASSERT( Ok ); 
      if ( srcSz == 0 )
        break;
    }

    clock_t finish = clock();
    cout << finish - start << " ticks" << endl;

    string outname;
    if ( fileout == NULL )
      outname = removeExtension( filein );
    else
      outname = string( fileout );

    fp = fopen( outname.c_str(), "wb" );
    ASSERT( fp );
    fwrite( rawBlock, 1, rawLen, fp );
    fclose( fp );

    remove( filein );
  }

  return EXIT_SUCCESS;
}
