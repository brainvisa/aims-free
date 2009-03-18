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
#include <aims/getopt/getopt.h>
#include <aims/utility/utility_g.h>
#include <aims/mesh/mesh_g.h>
#include <aims/io/io_g.h>

using namespace aims;
using namespace std;

BEGIN_USAGE( usage )
  "--------------------------------------------------------------------------",
  "AimsMeshSmoothing  -i[nput]  <filein>                                     ",
  "                  [-o[utput] <fileout>]                                   ",
  "                  [-n[Iteration] <niter>]                                 ",
  "                  [-I[sotropic]]                                          ",
  "                  [-r[ate] <rate>]                                        ",
  "                  [-x[rate] <rate>]                                       ",
  "                  [-y[rate] <rate>]                                       ",
  "                  [-z[rate] <rate>]                                       ",
  "                  [-f[eatureAngle] <angle>]                               ",
  "                  [--tri]                                                 ",
  "                  [--ascii]                                               ",
  "                  [-h[elp]]                                               ",
  "--------------------------------------------------------------------------",
  " Smoothes a triangulation                                                 ",
  "--------------------------------------------------------------------------",
  "-i[nput]            : prefix name for input triangulation is <filein>     ",
  "-o[utput]           : prefix name for output triangulation is <fileout>   ",
  "                      [default=filein]                                    ",
  "-n[Iteration]       : number of iterations is <nb> [default=10]           ",
  "-I[sotropic]        : type of smoothing is isotropic [default=anisotropic]",
  "-r[ate]             : moving factor at each iteration for isotropic       ",
  "                      smoothing is <rate> [default=0.2]                   ",
  "-x[rate]            : moving factor on x axis is <rate> [default=0.2]     ",
  "-y[rate]            : moving factor on y axis is <rate> [default=0.2]     ",
  "-z[rate]            : moving factor on z axis is <rate> [default=0.2]     ",
  "-f[eatureAngle]     : feature angle is <angle> degree, between 0 and 180  ",
  "                      [default=180]                                       ",
  "--tri               : *.tri file format [default=*.mesh]                  ",
  "--ascii             : write *.mesh in ASCII [default=binar]               ",
  "-h[elp]             : display the current help message                    ",
  "--------------------------------------------------------------------------",
END_USAGE


void Usage( void )
{
  AimsUsage( usage );
}


int main( int argc, char* argv[] )
{ 
  char *fileIn = NULL, *fileOut = NULL;
  int nIteration = 10;
  int isotropic = 0;
  float rate = 0.2;
  float xRate = 0.2;
  float yRate = 0.2;
  float zRate = 0.2;
  float featureAngle = 180.0;
  int triFlag = 0;
  int asciiFlag = 0;

  AimsOption opt[] = {
  { 'h',"help"      ,AIMS_OPT_FLAG  ,( void* )Usage           ,AIMS_OPT_CALLFUNC,0},
  { 'i',"input"     ,AIMS_OPT_STRING,&fileIn         ,0                ,1},
  { 'o',"output"    ,AIMS_OPT_STRING,&fileOut        ,0                ,0},
  { 'n',"nIteration",AIMS_OPT_INT   ,&nIteration     ,0                ,0},
  { 'I',"Isotropic" ,AIMS_OPT_FLAG  ,&isotropic      ,0	              ,0},
  { 'r',"rate"      ,AIMS_OPT_FLOAT ,&rate           ,0                ,0},
  { 'x',"xRate"     ,AIMS_OPT_FLOAT ,&xRate	        ,0                ,0},
  { 'y',"yRate"     ,AIMS_OPT_FLOAT ,&yRate	        ,0                ,0},
  { 'z',"zRate"     ,AIMS_OPT_FLOAT ,&zRate	        ,0                ,0},
  { 'f',"feature"   ,AIMS_OPT_FLOAT ,&featureAngle   ,0                ,0},
  { ' ',"tri"       ,AIMS_OPT_FLAG  ,&triFlag        ,0                ,0},
  { ' ',"ascii"     ,AIMS_OPT_FLAG  ,&asciiFlag      ,0                ,0},
  { 0  ,0           ,AIMS_OPT_END   ,0               ,0                ,0}};

  AimsParseOptions( &argc, argv, opt, usage );

  //
  // chek options
  //
  ASSERT( nIteration > 0 );
  ASSERT( featureAngle >= 0.0 && featureAngle <= 180.0 );
  ASSERT( rate >= 0.0 && rate <= 1.0 );
  ASSERT( xRate >= 0.0 && xRate <= 1.0 );
  ASSERT( yRate >= 0.0 && yRate <= 1.0 );
  ASSERT( zRate >= 0.0 && zRate <= 1.0 );

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
  // smooth mesh
  //
  cout << "smoothing mesh          : " << flush;
  Mesher mesher;
  if ( isotropic )
    mesher.setSmoothing( featureAngle, nIteration, rate, rate, rate );
  else
    mesher.setSmoothing( featureAngle, nIteration, xRate, yRate, zRate );
  mesher.smooth( surface );
  cout << "done" << endl;


  //
  // save triangulation
  //
  cout << "saving triangulation    : " << flush;
  Writer<AimsSurfaceTriangle> triW( fileOut );
  triW << surface;
  cout << "done" << endl;

  return EXIT_SUCCESS;
}

