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
#include <aims/data/data_g.h>
#include <aims/math/math_g.h>
#include <aims/io/io_g.h>
#include <aims/getopt/getopt.h>
#include <assert.h>

using namespace aims;
using namespace std;

BEGIN_USAGE(usage)
  "-------------------------------------------------------------------------",
  "AimsDistordDti -R[eference] <ref>                                        ",
  "               -d[istorded] <dist>                                       ",
  "               --S <S>                                                   ",
  "               --T0 <T0>                                                 ",
  "               --T1 <T1>                                                 ",
  "              [--interpolator <interpolator>]                            ",
  "              [-h[elp]]                                                  ",
  "-------------------------------------------------------------------------",
  "Distord DTI                                                              ",
  "-------------------------------------------------------------------------",
  "     ref    : source S16 data                                            ",
  "     dist   : distorded S16 data                                         ",
  "     S      : magnification                                              ",
  "     T0,T1  : translation T = T0 + T1 * x                                ",
  "     interpolator : nearest/linear/spline [default=linear]               ",
  "-------------------------------------------------------------------------",
END_USAGE


void Usage( void )
{
  AimsUsage( usage );
}



int main( int argc, char **argv )
{
  char *fileRef = NULL,*fileDist = NULL;
  char* c_interpolator = NULL;
  float scale, T0, T1;

  AimsOption opt[] = {
  { 'h',"help"        ,AIMS_OPT_FLAG  ,( void* )Usage          ,AIMS_OPT_CALLFUNC,0},
  { 'R',"Reference"   ,AIMS_OPT_STRING,&fileRef       ,0                ,1},
  { 'd',"distorded"   ,AIMS_OPT_STRING,&fileDist      ,0                ,1},
  { ' ',"S"           ,AIMS_OPT_FLOAT ,&scale         ,0                ,1},
  { ' ',"T0"          ,AIMS_OPT_FLOAT ,&T0            ,0                ,1},
  { ' ',"T1"          ,AIMS_OPT_FLOAT ,&T1            ,0                ,1},
  { ' ',"interpolator",AIMS_OPT_STRING,&c_interpolator,0                ,0},
  { 0  ,0             ,AIMS_OPT_END   ,0              ,0                ,0}};

  AimsParseOptions( &argc, argv, opt, usage );


  AimsData<int16_t> ref, dist;

  Reader<AimsData<int16_t> > dataR( fileRef );
  if( !dataR.read( ref ) )
    AimsUsage( usage );

  dist = ref.clone();

  Interpolator<3>* interpolator = NULL;
  if ( c_interpolator )
  {
    ASSERT( string( c_interpolator ) == "nearest" ||
            string( c_interpolator ) == "linear"  ||
            string( c_interpolator ) == "spline"    );
    if ( string( c_interpolator ) == "nearest" )
      interpolator = new NNInterpolator;
    else if ( string( c_interpolator ) == "linear" )
      interpolator = new LinearInterpolator;
    else if ( string( c_interpolator ) == "spline" )
      interpolator = new SplineInterpolator;
  }
  else
      interpolator = new LinearInterpolator;

  interpolator->doit( ref, dist, Point3df( scale, T0, T1 ) );

  AimsData<int16_t> data( ref.dimX(), ref.dimY(), 1, 2 );
  data.setSizeX( ref.sizeX() );
  data.setSizeY( ref.sizeY() );

  for ( int y = 0; y < ref.dimY(); y++ )
    for ( int x = 0; x < ref.dimX(); x++ )
    {
      data( x, y, 0, 0 ) = ref( x, y );
      data( x, y, 0, 1 ) = int16_t( dist( x, y ) / scale );
    }

  Writer<AimsData<int16_t> > dataW( fileDist );
  assert( dataW.write( data ) );

  return EXIT_SUCCESS;
}
