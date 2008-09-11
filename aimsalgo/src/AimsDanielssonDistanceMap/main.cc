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


#include <aims/data/data_g.h>
#include <aims/distancemap/distancemap_g.h>
#include <aims/io/io_g.h>
#include <aims/getopt/getopt.h>
#include <assert.h>

using namespace aims;
using namespace std;


BEGIN_USAGE(usage)
  "-------------------------------------------------------------------------",
  "AimsDanielssonDistanceMap  -i[nput] <filein>                             ",
  "                           -o[utput] <fileout>                           ",
  "                          [-s[ide] <S>] [-h[elp]]                        ",
  "-------------------------------------------------------------------------",
  "Danielsson's distance map on a volume                                    ",
  "-------------------------------------------------------------------------",
  "     filein  : source volume                                             ",
  "     fileout : destination volume                                        ",
  "     S       : OUTSIDE, INSIDE, SIGNED [default=SIGNED]                  ",
  "-------------------------------------------------------------------------",
END_USAGE


void Usage( void )
{
  AimsUsage( usage );
}


int main( int argc, char **argv )
{
  char *filein = NULL, *fileout = NULL;
  char* c_side = NULL;

  AimsOption opt[] = {
  { 'h',"help"    ,AIMS_OPT_FLAG  ,( void* )Usage    ,AIMS_OPT_CALLFUNC,0},
  { 's',"side"    ,AIMS_OPT_STRING,&c_side  ,0                ,0},
  { 'i',"input"   ,AIMS_OPT_STRING,&filein  ,0                ,1},
  { 'o',"output"  ,AIMS_OPT_STRING,&fileout ,0                ,1},
  { 0  ,0         ,AIMS_OPT_END   ,0        ,0                ,0}};

  AimsParseOptions( &argc, argv, opt, usage );

  string side = "SIGNED";
  if ( c_side != NULL )
  {
    side = string( c_side );
    ASSERT( side == "INSIDE" || side == "OUTSIDE" || side == "SIGNED" );
  }

  AimsData<short> vol;
 
  Reader<AimsData<short> > reader( filein );
  if( !reader.read( vol, 1 ) )
    AimsUsage( usage );
 
  AimsData<float> map;

  if ( side == "INSIDE" )
    map = AimsDanielssonDistanceMap( vol, AIMS_DANIELSSON_INSIDE );
  else if ( side == "OUTSIDE" )
    map = AimsDanielssonDistanceMap( vol, AIMS_DANIELSSON_OUTSIDE );
  else
    map = AimsSignedDanielssonDistanceMap( vol );

  Writer<AimsData<float> > writer( fileout );
  assert( writer.write( map ) );

  return EXIT_SUCCESS;
}
