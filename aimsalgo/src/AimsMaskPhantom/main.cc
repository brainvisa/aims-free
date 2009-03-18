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
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/data/data_g.h>
#include <aims/getopt/getopt.h>
#include <aims/math/math_g.h>

using namespace aims;
using namespace std;

BEGIN_USAGE(usage)
  "----------------------------------------------------------------",
  "AimsMaskPhantom  -i[nput] <filein>                              ",
  "                [-o[utput] <fileout>]                           ",
  "                [--centerX <cx>]                                ",
  "                [--centerY <cy>]                                ",
  "                [--centerZ <cz>]                                ",
  "                [--radius <R>]                                  ",
  "                [--withSphere]                                  ",
  "                 --bandX <bx>                                   ",
  "                 --bandW <bw>                                   ",
  "                [-r[eader] <reader>]                            ",
  "                [-h[elp]]                                       ",
  "----------------------------------------------------------------",
  "Special masking of DTI Phantom for Anne Darquie                 ",
  "----------------------------------------------------------------",
  "  filein        : origin file                                   ",
  "  fileout       : output file [default=filein]                  ",
  "  cx, cy, cz, R : center and radius of the sphere for masking   ",
  "                  Eddy current [default=no masking]             ",
  "  --withSphere  : mask Eddy Current with sphere [default=no]    ",
  "  bx, bw        : x-coordinate and width of the band to mask the",
  "                  air buble                                     ",
  "----------------------------------------------------------------",
END_USAGE


void Usage( void )
{
  AimsUsage( usage );
}


int main( int argc, char** argv )
{
  char *fileIn = NULL, *fileOut = NULL;
  int centerX = -1, centerY = -1, centerZ = -1;
  float radius = 1.0;
  int bandX = 0, bandW = 0;
  int withSphere = 0;

  AimsOption opt[] = {
  { 'h',"help"      ,AIMS_OPT_FLAG  ,( void* )Usage       ,AIMS_OPT_CALLFUNC,0,},
  { 'i',"input"     ,AIMS_OPT_STRING,&fileIn     ,0                ,1},
  { 'o',"output"    ,AIMS_OPT_STRING,&fileOut    ,0                ,0},
  { ' ',"centerX"   ,AIMS_OPT_INT   ,&centerX    ,0                ,0}, 
  { ' ',"centerY"   ,AIMS_OPT_INT   ,&centerY    ,0                ,0}, 
  { ' ',"centerZ"   ,AIMS_OPT_INT   ,&centerZ    ,0                ,0}, 
  { ' ',"radius"    ,AIMS_OPT_FLOAT ,&radius     ,0                ,0}, 
  { ' ',"bandX"     ,AIMS_OPT_INT   ,&bandX      ,0                ,0}, 
  { ' ',"bandW"     ,AIMS_OPT_INT   ,&bandW      ,0                ,0}, 
  { ' ',"withSphere",AIMS_OPT_FLAG  ,&withSphere ,0                ,0},
  { 0  ,0           ,AIMS_OPT_END   ,0           ,0                ,0}};

  AimsParseOptions( &argc, argv, opt, usage );

  if ( fileOut == NULL )
    fileOut = fileIn;

  AimsData<int16_t> data;

  Reader<AimsData<int16_t> > dataR( fileIn );
  dataR >> data;

  if ( withSphere )
  {
    ASSERT( centerX >= 0 && centerX < data.dimX() );
    ASSERT( centerY >= 0 && centerY < data.dimY() );
    ASSERT( centerZ >= 0 && centerZ < data.dimZ() );
    ASSERT( radius > 0.0 );
  }
  ASSERT( bandX >= 0 && bandX < data.dimX() );
  ASSERT( bandW >= 0 );


  int x = 0, y = 0, z = 0, t = 0;
  if ( withSphere )
    ForEach4d( data, x, y, z, t )
    {
      if ( sqr( ( x-centerX ) * data.sizeX() ) +
           sqr( ( y-centerY ) * data.sizeY() ) +
           sqr( ( z-centerZ ) * data.sizeZ() ) > sqr( radius ) ||
           abs( x - bandX ) <= bandW )
        data( x, y, z, t ) = 0;
    }
  else
    ForEach4d( data, x, y, z, t )
    {
      if ( abs( x - bandX ) <= bandW )
        data( x, y, z, t ) = 0;
    }


  Writer<AimsData<int16_t> > dataW( fileOut );
  dataW << data;

  return EXIT_SUCCESS;
}
