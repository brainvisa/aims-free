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
#include <aims/data/data_g.h>
#include <aims/io/io_g.h>
#include <aims/getopt/getopt.h>
#include <aims/utility/utility_g.h>
#include <aims/mesh/mesh_g.h>

using namespace aims;
using namespace std;

#define NLABEL 32767

BEGIN_USAGE(usage)
  "--------------------------------------------------------------------------",
  "AimsMeshBrain  -i[nput]  <filein>                                         ",
  "              [-o[utput] <fileout>]                                       ",
  "              [--deciMaxClearance <clearance>]                            ",
  "              [--deciMaxError <error>]            u                        ",
  "              [--minFacetNumber <fnumber>]                                ",
  "              [--ascii]                                                   ",
  "              [-h[elp]]                                                   ",
  "--------------------------------------------------------------------------",
  " Computes the brain surface from a segmented image and saves it in a      ",
  " *.mesh or *.tri format file                                              ",
  "--------------------------------------------------------------------------",
  "-i[nput]            : input short volume label is file <filein>           ",
  "                      (level 0 is assumed to be the back)                 ",
  "-o[utput]           : output prefix name for *.tri or *.mesh is <fileout> ",
  "                      [default=filein]                                    ",
  "--deciMaxClearance  : maximum clearance expected in the resulting mesh is ",
  "                      <clearance> mm [default= 5 mm]                      ",
  "--deciMaxError      : maximum error distance from the original data       ",
  "                      expected is <error> mm , [default = 3 mm]           ",
  "--minFacetNumber    : minimum number of facets to allow decimation        ",
  "                      [default=50]                                        ",
  "--ascii             : write *.mesh file in ASCII mode [default=binar]     ",
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
  float deciMaxClearance = 5.0;
  float	deciMaxError = 3.0;
  uint minFacetNumber = 50;
  int asciiFlag = 0;

  AimsOption opt[] = {
  { 'h',"help"             ,AIMS_OPT_FLAG  ,( void* )Usage        ,AIMS_OPT_CALLFUNC,0},
  { 'i',"input"            ,AIMS_OPT_STRING,&fileIn        ,0              ,1},
  { 'o',"output"           ,AIMS_OPT_STRING,&fileOut       ,0              ,0},
  { ' ' ,"deciMaxClearance",AIMS_OPT_FLOAT ,&deciMaxClearance,0            ,0},
  { ' ' ,"deciMaxError"    ,AIMS_OPT_FLOAT ,&deciMaxError  ,0              ,0},
  { ' ',"minFacetNumber"   ,AIMS_OPT_WORD  ,&minFacetNumber,0              ,0},
  { ' ',"ascii"            ,AIMS_OPT_FLAG  ,&asciiFlag     ,0              ,0},
  { 0  ,0                  ,AIMS_OPT_END   ,0              ,0              ,0}};

  AimsParseOptions( &argc, argv, opt, usage );

  ASSERT( deciMaxClearance >= 0.0 );
  ASSERT( deciMaxError >= 0.0 );

  string	fout;
  if ( fileOut == NULL )
    {
      fout = fileIn;
      string::size_type	pos = fout.rfind( '.' );
      string::size_type	pos2 = fout.rfind( '/' );
      if( pos != string::npos && ( pos2 == string::npos || pos2 < pos ) )
	fout.erase( pos, fout.length() - pos );
    }
  else
    fout = fileOut;

  //
  // load label volume
  //
  cout << "reading image      : " << flush;
  AimsData<short> data;
  Reader<AimsData<short> > dataR( fileIn );
  dataR.read( data, 1 );
  cout << "done" << endl;

  cout << "resolutions        : (";
  cout << data.sizeX() << " mm,";
  cout << data.sizeY() << " mm,";
  cout << data.sizeZ() << " mm)" << endl;
  data.fillBorder(-1);

  Mesher mesher;
  mesher.setDecimation( 100.0, deciMaxClearance, deciMaxError, 180.0 );
  mesher.setMinFacetNumber( minFacetNumber );
  AimsSurfaceTriangle surface;
  mesher.getBrain( data, surface );

  // compute the surface mesh
  cout << "surface            : " << flush 
       << mesher.surfaceOfInterface( surface ) << " mm2" << endl; 

  // save the triangulation
  Writer<AimsSurfaceTriangle> surfaceW( fout );
  surfaceW.write( surface, asciiFlag );

  return EXIT_SUCCESS;
}
