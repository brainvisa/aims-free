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


#include <aims/def/general.h>
#include <aims/def/assert.h>
#include <aims/getopt/getopt.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>

using namespace std;

/** The different types of data.
*/
AIMSDATA_API enum AimsDataType
{
  AIMS_RS08BITS,
  AIMS_RU08BITS,
  AIMS_RS16BITS,
  AIMS_RU16BITS,
  AIMS_RS32BITS,
  AIMS_RU32BITS,
  AIMS_RF32BITS,
  AIMS_RF64BITS,
  AIMS_CF32BITS,
  AIMS_CF64BITS,
  AIMS_PT2DS16BITS,
  AIMS_PT2DS32BITS,
  AIMS_PT2DF32BITS,
  AIMS_PT3DS16BITS,
  AIMS_PT3DS32BITS,
  AIMS_PT3DF32BITS,
  AIMS_RGBU08BITS
};

BEGIN_USAGE(usage)
  "----------------------------------------------------------------",
  "AimsSwapEndian -i[nput] <filein>                                ",
  "               -o[utput] <fileout>                              ",
  "               -t[ype] <intype>                                 ",
  "               [-h[elp]]                                        ",
  "----------------------------------------------------------------",
  "convert a UNIX data to a LINUX byte ordered data                ",
  "----------------------------------------------------------------",
  "     filein  : origin file                                      ",
  "     fileout : output file                                      ",
  "     intype  : short,ushort,int,uint,float,double             ",
  "----------------------------------------------------------------",
END_USAGE


void Usage( void )
{
  AimsUsage( usage );
}


int main( int argc, char **argv )
{
  char *filein = NULL, *fileout = NULL, *intype = NULL;
  int type = 0, nElem = 0;
  int nbytes = 0;
  FILE *in, *out;
 
  AimsOption opt[] = {
  { 'h',"help"  ,AIMS_OPT_FLAG  ,( void* )Usage       ,AIMS_OPT_CALLFUNC,0,},
  { 'i',"input" ,AIMS_OPT_STRING,&filein     ,0                ,1},
  { 'o',"output",AIMS_OPT_STRING,&fileout    ,0                ,1},
  { 't',"type"  ,AIMS_OPT_STRING,&intype     ,0                ,1},
  { 0  ,0       ,AIMS_OPT_END   ,0           ,0                ,0}};

  AimsParseOptions( &argc, argv, opt, usage );

  if ( string( intype ) == "short" )
    { 
      type = AIMS_RS16BITS;
      nbytes = 2;
    }
  else if ( string( intype ) == "ushort" )
    {
      type = AIMS_RU16BITS;
      nbytes = 2;
    }
  else if ( string( intype ) == "int" )
    {
      type = AIMS_RS32BITS;
      nbytes = 4;
    }
  else if ( string( intype ) == "uint" )
    {
      type = AIMS_RU32BITS;
      nbytes = 4;
    }
  else if ( string( intype ) == "float" )
    {
      type = AIMS_RF32BITS;
      nbytes = 4;
    }
  else if ( string( intype ) == "double" )
    {
      type = AIMS_RF64BITS;
      nbytes = 8;
    }
  else 
  { 
    AimsWarning("AimsSwapEndian : not a good type" );
    AimsUsage(usage);
  }

  struct stat Stat;
  stat( filein, &Stat );
  int size = Stat.st_size;

  // Pour etre sur que le bon nombre d'element soit present
  ASSERT( ( size % nbytes ) == 0 );

  nElem = size / nbytes;

  char *buf = new char[ size ];
  in  = fopen( filein, "rb" );
  fread( buf, nbytes, nElem, in );
  fclose( in );

  if ( type == AIMS_RS16BITS || type == AIMS_RU16BITS )
  {
    int16_t *buf2 = (int16_t *)buf;
    for ( int i=nElem; i--; )
    {
      *buf2 = ( ( *buf2 & 0xFF00 ) >> 8 ) | ( ( *buf2 & 0x00FF ) << 8 );
      ++buf2;
    }
  }
  else if ( type == AIMS_RS32BITS ||
            type == AIMS_RU32BITS ||
            type == AIMS_RF32BITS  )
  {
    int32_t *buf4 = (int32_t *)buf;
    for ( int i=nElem; i--; )
    {
      *buf4 = ((*buf4 & 0xFF000000) >> 24) | ((*buf4 & 0x00FF0000) >> 8) |
	      ((*buf4 & 0x0000FF00) << 8) | ((*buf4 & 0x000000FF) << 24);
      ++buf4;
    }
  }
  else if ( type == AIMS_RF64BITS )
  {
    int32_t *buf4g = (int32_t *)buf;
    int32_t *buf4d = buf4g + 1;
    int32_t dum;

    for ( int i=nElem; i--; buf4g+=2, buf4d+=2 )
      {
	*buf4g = ((*buf4g & 0xFF000000) >> 24) | ((*buf4g & 0x00FF0000) >> 8) |
	         ((*buf4g & 0x0000FF00) << 8) | ((*buf4g & 0x000000FF) << 24);
	*buf4d = ((*buf4d & 0xFF000000) >> 24) | ((*buf4d & 0x00FF0000) >> 8) |
	         ((*buf4d & 0x0000FF00) << 8) | ((*buf4d & 0x000000FF) << 24);
	dum = *buf4g;
        *buf4g = *buf4d;
	*buf4d = dum;
      }
  }

  out = fopen( fileout, "wb" );
  fwrite( buf, nbytes, nElem, out );
  fclose( out );

  delete[] buf;

  return EXIT_SUCCESS;
}
