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


#include <aims/distancemap/meshdistance.h>
#include <aims/data/data_g.h>
#include <aims/io/io_g.h>
#include <iomanip>
#include <aims/getopt/getopt.h>
#include <aims/mesh/texture.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>

using namespace aims;
using namespace aims::meshdistance;
using namespace std;
 
typedef float float3[3];

BEGIN_USAGE(usage)
  "-------------------------------------------------------------------------",
  "AimsMeshClose   -i[nput] <meshfilein>                                  ",
  "                -v[volume] <mask_volume>                             ",
  "                [-c[close] <radius_close>]                             ",
  "                [-e[rosion] <radius_erosion>]                             ",
  "                [-o[utput] <output_vor_texture>                        ",
  "                [-h[elp]]                                              ",
  "-------------------------------------------------------------------------",
  " Compute geodesic depth on a triangulation.                                      ",
  "-------------------------------------------------------------------------",
  "     meshfilein          : input *.tri or *.mesh file                    ",
  "     brain_envelop       : object definition (binary image relative to the triangualtion )             ",
  "     radius_close        : radius of the closing [default = 10]             ",
  "     radius_erode        : radius of the erosion [default = 5]             ",
  "     output_vor_texture : output *.tex file (voronoi diagram)            ",
  "-------------------------------------------------------------------------",
END_USAGE


//
// Usage
//
void Usage( void )
{
  AimsUsage( usage );
}


int main( int argc, char** argv )
{
  char	*meshfile = 0, *volumefile = 0, *outtexfile = 0;
  float close = 10, erode = 2;

  //
  // Parser of options
  //
  AimsOption opt[] = {
  { 'h',"help"         ,AIMS_OPT_FLAG  ,( void* )Usage           ,AIMS_OPT_CALLFUNC,0},
  { 'i',"input"        ,AIMS_OPT_STRING,&meshfile       ,0                ,1},
  { 'v',"volume"       ,AIMS_OPT_STRING,&volumefile     ,0                ,1},
  { 'o',"output"       ,AIMS_OPT_STRING,&outtexfile     ,0                ,1},
  { 'e',"erosion"      ,AIMS_OPT_FLOAT ,&erode          ,0                ,0},
  { 'c',"closing"      ,AIMS_OPT_FLOAT ,&close          ,0                ,0},
  { 0  ,0              ,AIMS_OPT_END   ,0               ,0                ,0}};

  AimsParseOptions( &argc, argv, opt, usage );

  //
  // read triangulation
  //
  cout << "reading triangulation   : " << flush;
  AimsSurfaceTriangle surface;
  Reader<AimsSurfaceTriangle> triR( meshfile );
  triR >> surface;
  cout << "done" << endl;

  //
  // read image of brain 
  //
  cout << "reading brain  : " << flush;
  AimsData < short > data;
  Reader< AimsData <short> > dataR( volumefile );
  //dataR >> data ;
  dataR.read(data,1);
  data.fillBorder(0);
  cout << "done" << endl;
  

  cout << "mesh vertices : " << surface[0].vertex().size() << endl;
  cout << "mesh polygons : " << surface[0].polygon().size() << endl;
 
   
  TimeTexture<float>	outTex;  
  
  outTex[0] = GeodesicDepth(surface[0],data,close,erode);
 
  // Writting the textures
  cout << "writing texture : " << flush;
  Writer<TimeTexture<float> >	texW( outtexfile );
  texW <<  outTex ;
 
  cout << "done" << endl;
  
  
  
  return( 0 );  
}



