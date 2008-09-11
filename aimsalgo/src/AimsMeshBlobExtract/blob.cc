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


#include <aims/scalespace/meshBlob.h>
#include <aims/io/io_g.h>
#include <aims/getopt/getopt.h>
#include <aims/mesh/texture.h>
#include <aims/connectivity/meshcc.h>


using namespace aims;
using namespace std;


BEGIN_USAGE(usage)
  "-------------------------------------------------------------------------",
  "AimsMeshBlobExtract  -i[nput] <meshfilein>                             ",
  "                  -t[exture] <input_texture>                             ",
  "                  [-T[ime]   <time serie> default = 0]                   ",
  "                  [-o[utput] <output_texture>]                      ",
  "                  [-h[elp]]                                              ",
  "-------------------------------------------------------------------------",
  " Extract blobs                                                           ",
  "-------------------------------------------------------------------------",
  "     meshfilein          : input *.tri or *.mesh file                    ",
  "     output_texture      : output *.tex file (blobs)                     ",
  "     input_texture       : object definition                             ",
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
  char	*meshfile = 0, *intexfile = 0, *outtexfile = 0;
  int	triFlag = 0;
  int	asciiFlag = 0;
  int T=0;

  //
  // Parser of options
  //
  AimsOption opt[] = {
  { 'h',"help"         ,AIMS_OPT_FLAG  ,( void* )Usage  ,AIMS_OPT_CALLFUNC,0},
  { 'i',"input"        ,AIMS_OPT_STRING,&meshfile       ,0                ,1},
  { 'o',"output"       ,AIMS_OPT_STRING,&outtexfile     ,0                ,1},
  { 'T',"Time"         ,AIMS_OPT_INT   ,&T              ,0                ,0},
  { 't',"texture"      ,AIMS_OPT_STRING,&intexfile      ,0                ,1},
  { ' ',"tri"          ,AIMS_OPT_FLAG  ,&triFlag        ,0                ,0},
  { ' ',"ascii"        ,AIMS_OPT_FLAG  ,&asciiFlag      ,0                ,0},
  { 0  ,0              ,AIMS_OPT_END   ,0               ,0                ,0}};

  AimsParseOptions( &argc, argv, opt, usage );

  //
  // read triangulation
  //
  cout << "reading triangulation   : " << flush;
  AimsSurfaceTriangle surface;
  Reader<AimsSurfaceTriangle>	triR( meshfile );
  triR >> surface;
  cout << "done" << endl;

  //
  // read input texture
  //
  cout << "reading texture   : " << flush;
  TimeTexture<float>	inpTex;
  Reader<Texture1d> texR( intexfile );
  texR >> inpTex;
  cout << "done" << endl;

  cout << "mesh vertices : " << surface[0].vertex().size() << endl;
  cout << "mesh polygons : " << surface[0].polygon().size() << endl;
  cout << "texture dim   : " << inpTex[T].nItem() << endl;

  TimeTexture<float>			outTex;
  map<int, ScaleSpace::BlobDescriptor>	limits;

  ScaleSpace	ss;
  Texture<int>	texint = ss.meshBlobExtract( surface[0], inpTex[T], limits );

  for( unsigned i=0, n=texint.nItem(); i<n; ++i )
    outTex[0].push_back( texint.item( i ) ); 
    
 
  cout << "writing texture : " << flush;
  Writer<Texture1d>	texW( outtexfile );
  texW.write( outTex, asciiFlag );
  cout << "done" << endl;

  return( 0 );
}


