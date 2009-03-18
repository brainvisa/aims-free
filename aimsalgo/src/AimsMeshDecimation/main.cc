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
#include <aims/mesh/mesh_g.h>
#include <aims/io/io_g.h> 
#include <aims/utility/utility_g.h>
#include <aims/getopt/getopt.h>

using namespace aims;
using namespace std;


BEGIN_USAGE( usage )
  "--------------------------------------------------------------------------",
  "AimsDecimation  -i[nput]  <filein>                                        ",
  "               [-o[utput] <fileout>]                                      ",
  "               [--smooth]                                                 ",
  "               [--smoothIt <nb iterations>]                               ",
  "               [--smoothRate <smoothing rate>                             ",
  "               [--smoothAngle <angle>                                     ",
  "               [--deciReductionRate <rate>]                               ",
  "               [--deciMaxClearance <clearance>]                           ",
  "               [--deciMaxError <error>]                                   ",
  "               [--deciAngle <angle>]                                      ",
  "               [--deciWithSmoothing]                                      ",
  "               [--tri]                                                    ",
  "               [--minFacetNumber <fnumber>]                               ",
  "               [--ascii]                                                  ",
  "               [-h[elp]]                                                  ",
  "--------------------------------------------------------------------------",
  " Decimates a triangulation                                                ",
  "--------------------------------------------------------------------------",
  "-i[nput]             : prefix name for input triangulation is <filein>    ",
  "-o[utput]            : prefix name for output triangulation is <fileout>  ",
  "                       [default=filein]                                   ",
  "--smooth             : smoothes the mesh [default is no smoothing]        ",
  "--smoothIt           : smoothing number of iterations is <nb iterations>  ",
  "                       [default=10]                                       ",
  "--smoothRate         : smoothing moving factor at each iteration is       ",
  "                       <smoothing rate> [default=0.2]                     ",
  "--smoothAngle        : smoothing feature angle is <angle> degree ,        ",
  "                       between 0 and 180 degree, [default=180]            ",
  "--deciReductionRate  : decimation reduction rate expected is              ",
  "                       <reduction rate> % [default=90%]                   ",
  "--deciMaxClearance   : maximum clearance of the decimated mesh expected is",
  "                       <clearance> mm [default=10]                        ",
  "--deciMaxError       : maximum error distance from the original mesh      ",
  "                       expected is <error> mm , [default=10]              ",
  "--deciAngle          : feature angle is <angle> degree, between 0 and 180 ",
  "                       degree, [default=180]                              ",
  "--tri                : *.tri file format [default=*.mesh]                 ",
  "--minFacetNumber     : minimum number of facets to allow decimation       ",
  "                       [default=50]                                       ",
  "--ascii              : write *.mesh in ASCII [default=binar]              ",
  "-h[elp]              : display the current help message                   ",
  "--------------------------------------------------------------------------",
END_USAGE


void Usage( void )
{
  AimsUsage( usage );
}


int main( int argc, char* argv[] )
{ 
  char *fileIn = NULL, *fileOut = NULL;
  int smoothFlag = 0;
  int smoothIt = 10;
  float smoothRate = 0.2;
  float smoothAngle = 180.0;
  float deciReductionRate = 99.0;
  float deciMaxClearance = 10.0;
  float deciMaxError = 10.0 ;
  float deciAngle = 180.0;
  int triFlag = 0;
  uint minFacetNumber = 50;
  int asciiFlag = 0;

  //
  // parse options
  //
  AimsOption opt[] = {
  { 'h',"help"              ,AIMS_OPT_FLAG  ,( void* )Usage       ,AIMS_OPT_CALLFUNC,0},
  { 'i',"input"             ,AIMS_OPT_STRING,&fileIn           ,0          ,1},
  { 'o',"output"            ,AIMS_OPT_STRING,&fileOut          ,0          ,0},
  { ' ',"smooth"            ,AIMS_OPT_FLAG  ,&smoothFlag       ,0          ,0},
  { ' ',"smoothIt"          ,AIMS_OPT_INT   ,&smoothIt         ,0          ,0},
  { ' ',"smoothRate"        ,AIMS_OPT_FLOAT ,&smoothRate       ,0          ,0},
  { ' ',"smoothAngle"       ,AIMS_OPT_FLOAT ,&smoothAngle      ,0          ,0},
  { ' ',"deciReductionRate" ,AIMS_OPT_FLOAT ,&deciReductionRate,0          ,0},
  { ' ',"deciMaxClearance"  ,AIMS_OPT_FLOAT ,&deciMaxClearance ,0          ,0},
  { ' ',"deciMaxError"      ,AIMS_OPT_FLOAT ,&deciMaxError     ,0          ,0},
  { ' ',"deciAngle"         ,AIMS_OPT_FLOAT ,&deciAngle        ,0          ,0},
  { ' ',"tri"               ,AIMS_OPT_FLAG  ,&triFlag          ,0          ,0},
  { ' ',"minFacetNumber"    ,AIMS_OPT_WORD  ,&minFacetNumber   ,0          ,0},
  { ' ',"ascii"             ,AIMS_OPT_FLAG  ,&asciiFlag        ,0          ,0},
  { 0  ,0                   ,AIMS_OPT_END   ,0                 ,0          ,0}};

  AimsParseOptions( &argc, argv, opt, usage );

  //
  // check options
  //
  ASSERT( smoothIt > 0 );
  ASSERT( smoothAngle >= 0.0 && smoothAngle <= 180.0 );
  ASSERT( smoothRate >= 0.0 && smoothRate <= 1.0 );
  ASSERT( deciReductionRate >= 0.0 && deciReductionRate <= 100.0 );
  ASSERT( deciMaxClearance >= 0.0 );
  ASSERT( deciMaxError >= 0.0 );
  ASSERT( deciAngle >= 0.0 && deciAngle <= 180.0 );


  if ( fileOut == NULL )
    fileOut = fileIn;

  cout << endl;

  //
  // read triangulation
  //
  cout << "reading triangulation   : " << flush;
  AimsSurfaceTriangle surface;
  Reader<AimsSurfaceTriangle> triR( fileIn );
  triR >> surface;
  cout << "done" << endl;


  //
  // decimating mesh
  //
  cout << "decimating mesh         : " << flush;
  Mesher mesher;
  if ( smoothFlag )
    mesher.setSmoothing( smoothAngle, smoothIt, 
                         smoothRate, smoothRate, smoothRate );
  mesher.setDecimation( deciReductionRate, deciMaxClearance, 
                        deciMaxError, deciAngle );
  mesher.setMinFacetNumber( minFacetNumber );
  float rate = mesher.decimate( surface );
  cout << "done ( " << rate << "% ) " << endl;


  //
  // save triangulation
  //
  Writer<AimsSurfaceTriangle> triW( fileOut );
  triW.write( surface, asciiFlag );
  cout << "done" << endl;


  return EXIT_SUCCESS;
}

