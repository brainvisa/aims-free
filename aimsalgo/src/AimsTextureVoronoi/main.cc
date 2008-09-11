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


#include <aims/distancemap/meshvoronoi.h>
#include <aims/io/io_g.h>
#include <aims/getopt/getopt.h>
#include <aims/mesh/texture.h>
#include <float.h>
#include <assert.h>



using namespace aims;
using namespace aims::meshdistance;
using namespace carto;
using namespace std;
 
typedef float float3[3];

BEGIN_USAGE(usage)
  "-------------------------------------------------------------------------",
  "AimsTextureVoronoi   -m[esh] <input_mesh>                                ",
  "                     -t[exture] <input_texture>                          ",
  "                     -o[utput] <output_vor_texture>                      ",
  "                     [-s[tep] <step>]                                    ",
  "                     [-b[ackground] <background_value>]                  ",
  "                     [-f[orbidden] <forbidden_value>]                    ",
  "                     [-T[ime]   <time serie> default = 0]                ",
  "                     [--c[onnexity] ]                                    ",
  "                     [-h[elp]]                                           ",
  "-------------------------------------------------------------------------",
  " Compute the Voronoi diagram of the input (short int) seed texture       ",
  "-------------------------------------------------------------------------",
  "     input_mesh          : input *.tri or *.mesh file                    ",
  "     input_texture       : definition of the seed (short int)",
  "     background          : background value <default = 0>",
  "     forbidden           : value out of the domain <default = -1>",
  "     size                : step size <default = infinity>",
  "     connexity           : connexity or geodesic euclidean distance      ",
  "     output_vor_texture : output voronoi diagram texture   ",
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
  char	 *meshfile = 0, *intexfile = 0, *outtexfile = 0;
  float dist = FLT_MAX;
  bool connexity = false;
  short background = 0 ;
  short forbidden = -1;
  int T = 0;
  //
  // Parser of options
  //
  AimsOption opt[] = {
    { 'h',"help"         ,AIMS_OPT_FLAG  ,( void* )Usage           ,AIMS_OPT_CALLFUNC,0},
    { 'm',"mesh"         ,AIMS_OPT_STRING,&meshfile       ,0                ,1},
    { 'o',"output"       ,AIMS_OPT_STRING,&outtexfile     ,0                ,1},
    { 't',"texture"      ,AIMS_OPT_STRING,&intexfile      ,0                ,1},
    { 'T',"Time"         ,AIMS_OPT_INT   ,&T              ,0                ,0},
    { 'b',"background"   ,AIMS_OPT_INT   ,&background     ,0                ,0},
    { 'f',"forbidden"    ,AIMS_OPT_INT   ,&forbidden      ,0                ,0},
    { 's',"step"         ,AIMS_OPT_FLOAT ,&dist           ,0                ,0},
    { ' ',"connexity"    ,AIMS_OPT_FLAG  ,&connexity      ,0                ,0},    
    { 0  ,0              ,AIMS_OPT_END   ,0               ,0                ,0}};

  AimsParseOptions( &argc, argv, opt, usage );
  
  //
  // read triangulation
  //
  cout << "reading white triangulation   : " << flush;
  AimsSurfaceTriangle surface;
  Reader<AimsSurfaceTriangle> triR( meshfile );
  triR >> surface;
  cout << "done" << endl;
  
  //
  // read input texture
  //
  cout << "reading texture   : " << flush;
  TimeTexture<short>	seed, voronoi; 
  Reader<TimeTexture<short> > texR( intexfile );
  texR >> seed; 
  cout << "done" << endl;
  
  cout << "mesh vertices : " << surface[0].vertex().size() << endl;
  cout << "mesh polygons : " << surface[0].polygon().size() << endl;
  cout << "Object texture dim   : " << seed[0].nItem() << endl;
  cout << "Background label: " << background << " Forbidden label : " << forbidden << endl;
  if ( ! (  seed[0].nItem() ==  surface[0].vertex().size() ) )
    {
      cerr << "The triangulation and the textures must correspond to the same object \n";
      assert( 0 );
    }
  
  cout << "Compute the voronoi diagram\n";
  voronoi[0] = MeshVoronoiT<short>( surface[0], seed[(unsigned) T], background ,forbidden, dist ,connexity,true );
  cout << "writing texture : " << flush;
  Writer<TimeTexture<short> >	texW( outtexfile );
  texW <<  voronoi ;
  cout << "done" << endl;

  return( 0 );  
}

 
