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
#include <aims/io/io_g.h>
#include <aims/io/writer.h> 
#include <aims/getopt/getopt.h>
#include <aims/math/math_g.h>
#include <aims/utility/utility_g.h>
#include <aims/signalfilter/medianfilter.h>

using namespace aims;   
using namespace std;   

BEGIN_USAGE(usage)
  "---------------------------------------------------------------------------",
  "AimsMedianSmoothing   -i[nput]   < input file Image SHORT >                ",
  "                      -o[output] < fileout (NO EXTENSION) >                ",
  "                      --dx[dimemsion]  < X direction >   [default=3]       ",
  "                      --dy[dimemsion]  < Y direction >   [default=3]       ",
  "                      --dz[dimemsion]  < Z direction >   [default=1]       ",
  "                      [-h[elp]]                                            ",
  "                                                                           ",
  "---------------------------------------------------------------------------",
  "                       Filtering inline command                            ",
  "---------------------------------------------------------------------------",
  "                                                                           ",
  "        dx, dy, dz    : mask form   (2/3 dimensions)   ODD NUMBER !!!      ",
  "                                                                           ",
  "---------------------------------------------------------------------------",
END_USAGE


void Usage( void )
{
  AimsUsage( usage );
}

int main( int argc, char **argv )
{
  char *fileIn = NULL, *fileOut = NULL;
  Point3d t=3;

  AimsOption opt[] = {
  { 'h',"help"             ,AIMS_OPT_FLAG                    ,( void* )Usage  ,AIMS_OPT_CALLFUNC,0},
  { 'i',"input"            ,AIMS_OPT_STRING,&fileIn          ,0               ,1},
  { 'o',"output"           ,AIMS_OPT_STRING,&fileOut         ,0               ,1},
  { ' ',"dx"               ,AIMS_OPT_SHORT ,&t[0]            ,0               ,0},
  { ' ',"dy"               ,AIMS_OPT_SHORT ,&t[1]            ,0               ,0},
  { ' ',"dz"               ,AIMS_OPT_SHORT ,&t[2]            ,0               ,0},
  { 0  ,0                  ,AIMS_OPT_END                     ,0               ,0                 ,0}};

  AimsParseOptions( &argc, argv, opt, usage ); 
    
  //  Lecture image
  AimsData<short> in, out;
  try
    {
      Reader<AimsData<short> > dataR( fileIn ); 
      dataR.read( in );
    }
  catch( exception & e )
    {
      cerr << e.what() << endl;
      exit( 1 );
    }   
     
  MedianSmoothing< short > ms(t[0], t[1], (in.dimZ() > 1 ? t[2] : 1) ); 
  out = ms.doit( in );

  // 
  //   Ecriture de l'image des voxels sélectionnés
  //
  out.setHeader( in.header()->cloneHeader() );
  Writer< AimsData<short> > dataWtr( fileOut );
  dataWtr << out;


  return EXIT_SUCCESS; 
}
 
