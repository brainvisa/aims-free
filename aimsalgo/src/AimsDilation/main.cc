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
#include <aims/morphology/morphology_g.h>
#include <aims/io/process.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/io/finder.h>
#include <aims/getopt/getopt.h>
#include <cstdlib>
#include <iostream>

using namespace aims;
using namespace std;

BEGIN_USAGE(usage)
  "-------------------------------------------------------------------------",
  "AimsDilation -i[nput]  <filein>                                          ",
  "             -o[utput] <fileout>                                         ",
  "             -e[radius] <radius>                                         ",
  "             [-x[mask] <xxx>] [-y[mask] <yyy>] [-z[mask] <zzz>]          ",
  "             [-f[actor] <fff>]                                           ",
  "             [-h[elp]]                                                   ",
  "-------------------------------------------------------------------------",
  "Morphological dilation                                                   ",
  "-------------------------------------------------------------------------",
  "     filein  : source volume                                             ",
  "     fileout : destination volume                                        ",
  "     radius  : radius of the structuring element                         ",
  "     xxx     : X size of the distance mask [default=3]                   ",
  "     yyy     : Y size of the distance mask [default=3]                   ",
  "     zzz     : Z size of the distance mask [default=3]                   ",
  "     fff     : chamfer multiplication factor [default=50]                ",
  "-------------------------------------------------------------------------",
END_USAGE


void Usage( void )
{
  AimsUsage( usage );
}

template<class T>
static bool dilat( Process & p, const string & filename, Finder & f );

class Dilatation : public Process
{
public:
  Dilatation( const string & fout, int xm, int ym, int zm, float rad, float fac );

  template<class T> 
  bool dilat_m( T & data, const string & filename, Finder & f );

private:
  string	fileout;
  int		xmask;
  int		ymask;
  int		zmask;
  float		radius;
  float		factor;
};

Dilatation::Dilatation( const string & fout, int xm, int ym, int zm,
	      float rad,  float fac )
	      : Process(), fileout( fout ), xmask( xm ), 
			    ymask( ym ), zmask( zm ), radius( rad ), 
			    factor( fac )
{
  registerProcessType( "Volume", "S16", &dilat<AimsData<short> > );
  // add new types here when morphology operators become templates...
}

template<class T> 
bool dilat( Process & p, const string & filename, Finder & f )
{
  Dilatation	& dp = (Dilatation &) p;
  T		data;
  return( dp.dilat_m( data, filename, f ) );
}


template<class T> 
bool Dilatation::dilat_m( T & data, const string & filename, Finder & f )
{
  Reader<T>	reader( filename );
  string	format = f.format();
  if( !reader.read( data, 1, &format ) )
    return( false );

  T		dil;
  dil = AimsMorphoChamferDilation( data, radius, xmask, ymask, zmask, factor );

  Writer<T>	writer( fileout );
  return( writer.write( dil ) );
}


int main( int argc, char **argv )
{
  char *filein,*fileout;
  int xmask = 3, ymask = 3, zmask = 3;
  float radius, factor = 50;


  AimsOption opt[] = {
  { 'h',"help"    ,AIMS_OPT_FLAG  ,( void* )Usage    ,AIMS_OPT_CALLFUNC,0},
  { 'x',"xmask"   ,AIMS_OPT_INT   ,&xmask   ,0                ,0},
  { 'y',"ymask"   ,AIMS_OPT_INT   ,&ymask   ,0                ,0},
  { 'z',"zmask"   ,AIMS_OPT_INT   ,&zmask   ,0                ,0},
  { 'e',"eradius" ,AIMS_OPT_FLOAT ,&radius  ,0                ,1},
  { 'f',"factor"  ,AIMS_OPT_FLOAT ,&factor  ,0                ,0},
  { 'i',"input"   ,AIMS_OPT_STRING,&filein  ,0                ,1},
  { 'o',"output"  ,AIMS_OPT_STRING,&fileout ,0                ,1},
  { 0  ,0         ,AIMS_OPT_END   ,0        ,0                ,0}};

  AimsParseOptions( &argc, argv, opt, usage );

  Dilatation	proc( fileout, xmask, ymask, zmask, radius, factor );
  if( !proc.execute( filein ) )
    {
      cerr << "Couldn't process\n";
      return( 1 );
    }

  return( EXIT_SUCCESS );
}
