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


#include "interpolator.h"
#include <aims/getopt/getopt.h>
#include <aims/data/data_g.h>
#include <aims/io/io_g.h>
#include <aims/utility/utility_g.h>
#include <aims/math/math_g.h>
#include <cartobase/allocator/mappingcopy.h>
#include <iomanip>

using namespace aims;
using namespace carto;
using namespace std;

BEGIN_USAGE(usage)
  "-------------------------------------------------------------------------",
  "AimsParamCorrectDTI -d[istorded] <distorded>                             ",
  "                    -o[utput] <fileout>                                  ",
  "                    -p[arameter] <param>                                 ",
  "                   [--memMap]                                            ",
  "                   [-r[ead] <reader>]                                    ",
  "                   [-h[elp]]                                             ",
  "-------------------------------------------------------------------------",
  "Correction of DTI distorsions from an existing parameter file            ",
  "-------------------------------------------------------------------------",
  "     distorded    : source S16 distorded data                            ",
  "     fileout      : destination S16 data file name                       ",
  "     param        : parameter file containing corrections                ",
  "     memMap       : memory mapping activated (obsolete, automatic)       ",
  "-------------------------------------------------------------------------",
END_USAGE


void Usage( void )
{
  AimsUsage( usage );
}


int main( int argc, char* argv[] )
{
  char *fileDist = NULL, *fileOut = NULL, *fileParam = NULL;
  int memMap = 0;
  int x, y, z, t;

  AimsOption opt[] = {
  { 'h',"help"         ,AIMS_OPT_FLAG  ,( void* )Usage          ,AIMS_OPT_CALLFUNC,0},
  { 'd',"distorded"    ,AIMS_OPT_STRING,&fileDist      ,0                ,1},
  { 'o',"output"       ,AIMS_OPT_STRING,&fileOut       ,0                ,1},
  { 'p',"parameter"    ,AIMS_OPT_STRING,&fileParam     ,0                ,1},
  { ' ',"memMap"       ,AIMS_OPT_FLAG  ,&memMap        ,0                ,0},
  { 0  ,0              ,AIMS_OPT_END   ,0              ,0                ,0}};

  AimsParseOptions(&argc,argv,opt,usage);


  //
  // choice of interpolation
  //
  SplineInterpolator interpolator;


  //
  // read data
  //
  cout << "reading distorded image : " << flush;
  AimsData<short> ima;
  Reader<AimsData<short> > dataR( fileDist );
  dataR.setAllocatorContext( AllocatorContext( AllocatorStrategy::ReadOnly ) );
  dataR >> ima;
  cout << "done" << endl;

  //
  // get sizes
  //
  int dimX = ima.dimX();
  int dimY = ima.dimY();
  int dimZ = ima.dimZ();
  int dimT = ima.dimT();


  //
  //  read parameters
  //
  cout << "reading parameters : " << flush;
  ifstream is( fileParam );
  AimsData< Point3df > parameter( dimZ, dimT );
  int tmp;
  for ( z = 0; z < dimZ; z++ )
    for ( t = 0; t < dimT; t++ )
    {
      is >> tmp;
      ASSERT( tmp == z );
      is >> tmp;
      ASSERT( tmp == t );
      is >> parameter( z, t );
    }
  is.close();
  cout << "done" << endl;

  //
  // allocate result and temporary images
  //
  AimsData<short> reg( dimX, dimY, dimZ, dimT );
  reg.setSizeXYZT( ima );

  AimsData<short> Sima( dimX, dimY );
  Sima.setSizeX( ima.sizeX() );
  Sima.setSizeY( ima.sizeY() );

  AimsData<short> Sreg( dimX, dimY );
  Sreg.setSizeX( reg.sizeX() );
  Sreg.setSizeY( reg.sizeY() );


  //
  // output starting parameters
  //
  cout << "slice : " << setw( 3 ) << 0 << flush;
  for ( z = 0; z < dimZ; z++ )
  {
    cout << "\b\b\b" << setw( 3 ) << z + 1 << flush;
    for ( t = 0; t < dimT; t++ )
    {
      ForEach2d( Sima, x, y )
        Sima( x, y ) = ima( x, y, z, t );

      interpolator.doit( Sima, Sreg, parameter( z, t ) );          

      ForEach2d( Sreg, x, y )
        reg( x, y, z, t ) = short( parameter( z, t )[0] * Sreg( x, y ) );
    }
  }
  cout << endl;

  cout << "writing result : " << flush;
  Writer<AimsData<short> > dataW( fileOut );
  dataW << reg;
  cout << "done" << endl;


  return EXIT_SUCCESS;
}
