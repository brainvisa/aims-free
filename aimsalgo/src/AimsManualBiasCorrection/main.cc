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
#include <aims/io/process.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/io/finder.h>
#include <aims/getopt/getopt.h>
#include <iostream>

using namespace aims;
using namespace std;

BEGIN_USAGE(usage)
  "------------------------------------------------------------------------- ",
  "AimsManualBiasCorrection -i[nput]  <filein>                               ",
  "             -o[utput] <fileout>                                          ",
  "             -d[irection] <direction : x, y, or z>                        ",
  "             -f[irst] <first slice value>                                  ",
  "             -l[ast] <last slice value>                                     ",
  "------------------------------------------------------------------------- ",
  "Morphological dilation                                                    ",
  "------------------------------------------------------------------------- ",
  "     filein   : source volume                                             ",
  "     fileout  : destination volume                                        ",
  "     direction: direction of bias correction : x axis, y axis, ou z axis  ",
  "     f        : first slice white matter level                            ",
  "     l        : last slice white matter level                             ",
  "-------------------------------------------------------------------------",
END_USAGE


void Usage( void )
{
  AimsUsage( usage );
}

template<class T>
static bool biasCorrection( Process & p, const string & filename, Finder & f );

class ManualBiasCorrection : public Process
{
public:
  ManualBiasCorrection( const string & fout, const string & direction, float firstSliceLevel, float lastSliceLevel ) ;

  template<class T> 
  bool processBiasCorrection       ( AimsData<T> & data, const string & filename, Finder & f );

private:
  string	_fileout;
  string _direction ;
  float _firstSliceLevel ;
  float _lastSliceLevel ;
};

ManualBiasCorrection::ManualBiasCorrection( const string & fout, const string& direction, 
					    float firstSliceLevel, float lastSliceLevel )
  : Process(), _fileout( fout ), _direction(direction), 
    _firstSliceLevel(firstSliceLevel), _lastSliceLevel(lastSliceLevel)
{
  registerProcessType( "Volume", "S16", &biasCorrection<short> );
  registerProcessType( "Volume", "FLOAT", &biasCorrection<float> );
  registerProcessType( "Volume", "DOUBLE", &biasCorrection<double> );
  // add new types here when morphology operators become templates...
}

template<class T> 
bool biasCorrection( Process & p, const string & filename, Finder & f )
{
  ManualBiasCorrection	& dp = (ManualBiasCorrection &) p;
  AimsData<T>		data;
  return( dp.processBiasCorrection( data, filename, f ) );
}


template<class T> 
bool ManualBiasCorrection::processBiasCorrection( AimsData<T> & data, const string & filename, Finder & f )
{
  Reader< AimsData< T > >	reader( filename );
  string	format = f.format();
  if( !reader.read( data, 1, &format ) )
    return( false );

  AimsData<T> result( data.dimX(), data.dimY(), data.dimZ() ) ;
  result.setSizeXYZT( data.sizeX(), data.sizeY(), data.sizeZ(), 1.) ;
  int dir ;
  if( _direction == "z" ){
    if( data.dimZ() == 1 ){
      cerr << "Wrong image dimensions" << endl ;
      return false ;
    }
    dir = 2 ;
  }
  if( _direction == "y" ){
    if( data.dimY() == 1 ){
      cerr << "Wrong image dimensions" << endl ;
      return false ;
    }
    dir = 1 ;
  }
  if( _direction == "x" ){
    if( data.dimX() == 1 ){
      cerr << "Wrong image dimensions" << endl ;
      return false ;
    }
    dir = 0 ;
  }
  Point3df invDim( 1./(data.dimX() - 1.), 1./(data.dimY() - 1.), 1./(data.dimZ() - 1.) ) ;
  Point3d  p ;
  float normFactor = 2500. / _firstSliceLevel ;
  for( p[2] = 0 ; p[2] < data.dimZ() ; ++p[2] ){
    for( p[1] = 0 ; p[1] < data.dimY() ; ++p[1] )
      for( p[0] = 0 ; p[0] < data.dimX() ; ++p[0] )
	result(p[0], p[1], p[2]) = T( normFactor * ( data(p[0], p[1], p[2]) 
						  - ( _lastSliceLevel - _firstSliceLevel ) * invDim[dir] * p[dir] ) ) ;
  }
  
  Writer< AimsData < T> > 	writer( _fileout );
  return( writer.write( result ) );
}


int main( int argc, char **argv )
{
  char *filein,*fileout, *direction ;
  float first, last;

  AimsOption opt[] = {
  { 'h',"help"      ,AIMS_OPT_FLAG   ,( void* )Usage    ,AIMS_OPT_CALLFUNC,0},
  { 'd',"direction" ,AIMS_OPT_STRING   ,&direction        ,0                ,1},
  { 'f',"first"      ,AIMS_OPT_FLOAT  ,&first             ,0                ,1},
  { 'l',"last"      ,AIMS_OPT_FLOAT  ,&last               ,0                ,1},
  { 'i',"input"     ,AIMS_OPT_STRING ,&filein           ,0                ,1},
  { 'o',"output"    ,AIMS_OPT_STRING ,&fileout          ,0                ,1},
  { 0  ,0           ,AIMS_OPT_END    ,0                 ,0                ,0}};

  AimsParseOptions( &argc, argv, opt, usage );

  ManualBiasCorrection	proc( fileout, direction, first, last );
  if( !proc.execute( filein ) )
    {
      cerr << "Couldn't process\n";
      return( 1 );
    }

  return( EXIT_SUCCESS );
}
