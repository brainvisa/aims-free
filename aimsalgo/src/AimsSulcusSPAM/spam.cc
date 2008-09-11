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


#include <aims/io/io_g.h>
#include <aims/getopt/getopt.h>
#include <aims/mesh/texture.h>
#include <aims/distancemap/meshvoronoi.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>

using namespace aims;
using namespace std;


BEGIN_USAGE(usage)
  "-------------------------------------------------------------------------",
  "AimsSulcusSPAM    -t[exture] <input_texture>                              ",
  "                  -s[ulsi_spam]                                          ",
  "                  -o[utput <output_mesh>]                             ",
  "                  -m[esh] <input_mesh>]                             ",
  "                  [-h[elp]]                                              ",
  "-------------------------------------------------------------------------",
  " SPAM               ",
  "-------------------------------------------------------------------------",
  "     input_texture       : object definition                             ",
  "     output_texture      : output *.tex file (blobs)                     ",
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
  char	*intexfile = 0, *outtexfile = 0, *meshfile=0,*outmesh;

  //
  // Parser of options
  //
  AimsOption opt[] = {
    { 'h',"help"         ,AIMS_OPT_FLAG  ,( void* )Usage  ,AIMS_OPT_CALLFUNC,0},
    { 't',"texture"      ,AIMS_OPT_STRING,&intexfile      ,0                ,1},
    { 's',"sulci"        ,AIMS_OPT_STRING,&outtexfile      ,0               ,1},
    { 'm',"mesh"         ,AIMS_OPT_STRING,&meshfile       ,0                ,1},
    { 'o',"output"       ,AIMS_OPT_STRING,&outmesh        ,0                ,1},
    { 0  ,0              ,AIMS_OPT_END   ,0               ,0                ,0}};

  AimsParseOptions( &argc, argv, opt, usage );
  

 
  //	read texture

  TimeTexture<short>	inpTex;
  cout << "reading texture " << intexfile << endl;
  Reader<TimeTexture<short> >	texR( intexfile );
  texR >> inpTex ;


  //
  TimeTexture<short>  itex = inpTex;
  float               m = 0,s = 0;
  unsigned            j, i, nnodes = itex[0].nItem(), ntimes = itex.size();
  TimeTexture<float>  outTex(3,nnodes);

  for (j=0;j<nnodes;++j)
    for (i=0;i<ntimes;++i)
      if (inpTex[i].item(j) != 0)
	itex[i].item(j) = 1;
     
  
  cout << ntimes << " time series, " << nnodes << " nodes" <<endl;

  for (j=0;j<nnodes;++j)
    {
      m = 0;

      for (i=0;i<ntimes;++i)
	  m += itex[i].item(j);

      m =  m / ntimes;
      outTex[0].item(j)= m ;

      s = 0;

      for (i=0;i<ntimes;++i)
	s += (itex[i].item(j) - m) * (itex[i].item(j) - m);
      s = sqrt(s) / ntimes;
      outTex[1].item(j) = s ;
    }
  

  //
  // read triangulation
  //
  cout << "reading triangulation   : " << flush;
  AimsSurfaceTriangle surface,meansurf;
  Reader<AimsSurfaceTriangle> triR( meshfile );
  triR >> surface;
  cout << "done" << endl;
  
  ASSERT( nnodes == surface[0].vertex().size() && ntimes == surface.size() ) ;

  Point3df       			vert ;
 
  cout << "Node average..." << flush;
  for (j=0; j<nnodes;++j)
    {
      vert[0] = 0;
      vert[1] = 0;
      vert[2] = 0;
      for (i = 0; i < ntimes; ++i)
	{
	  vert[0] += surface[i].vertex()[j][0];
	  vert[1] += surface[i].vertex()[j][1];
	  vert[2] += surface[i].vertex()[j][2];	
	}
	  
      vert[0] /= ntimes;
      vert[1] /= ntimes;
      vert[2] /= ntimes;
      m = 0;
      for (i = 0; i < ntimes; ++i)
	m += (surface[i].vertex()[j][0] - vert[0])*(surface[i].vertex()[j][0] - vert[0]) 
	  + (surface[i].vertex()[j][1] - vert[1])*(surface[i].vertex()[j][1] - vert[1])
	  + (surface[i].vertex()[j][2] - vert[2])*(surface[i].vertex()[j][2] - vert[2]);	
      
      m = sqrt(m) / ntimes;
      meansurf[0].vertex().push_back(vert);
      outTex[2].item(j) = m;
      
    }
  cout << "done\n";
  
  
  cout << "Build connectivity" << endl;
  meansurf[0].polygon() = surface[0].polygon(); 

  cout << "writing texture : " << flush;
  Writer<Texture1d>	texW( outtexfile );
  texW.write( outTex );
  cout << "done " << endl;

  cout << "Write the mesh..." << flush;
  Writer<AimsSurfaceTriangle >	meshW( outmesh);
  meshW << meansurf;
  cout << "done" << endl;
  
  return 0;
}
