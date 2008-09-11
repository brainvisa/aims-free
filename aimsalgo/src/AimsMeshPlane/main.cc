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


#include <aims/distancemap/meshparcellation.h>
#include <aims/distancemap/meshdistance.h>
#include <aims/data/data_g.h>
#include <aims/io/io_g.h>
#include <aims/math/math_g.h>
#include <aims/getopt/getopt.h>
#include <aims/vector/vector.h>
#include <aims/mesh/texture.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/connectivity/meshcc.h>
#include <iomanip>

using namespace aims;
using namespace aims::meshdistance;
using namespace std;
 
 
typedef float float3[3];

BEGIN_USAGE(usage)
  "-------------------------------------------------------------------------",
  "AimsMeshPlane     -n                                 ",
  "                  [-t[exture] <output_tex>                        ",
  "                  -m[mesh] ",
 "                  [-h[elp]]                                              ",
  "-------------------------------------------------------------------------",
  " Build a mesh plane with sulci                              ",
  "-------------------------------------------------------------------------",
  "     meshfilein          : input *.tri or *.mesh file                    ",
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
  char	*meshfile = 0, *texfile = 0;
  unsigned n=100, l1,l2,l3,l4,l5,l6,l7;

  l1 = 10; l2= 20; l3= 30; l4= 40;l5 = 50; l6= 60; l7= 70;

  //
  // Parser of options
  //
  AimsOption opt[] = {
  { 'h',"help"         ,AIMS_OPT_FLAG  ,( void* )Usage           ,AIMS_OPT_CALLFUNC,0},
  { 'm',"mesh"         ,AIMS_OPT_STRING,&meshfile        ,0                ,1},
  { 't',"texture"      ,AIMS_OPT_STRING,&texfile         ,0                ,1},
  { 'n',"n"            ,AIMS_OPT_INT   ,&n               ,0                ,0},
  { 0  ,0              ,AIMS_OPT_END   ,0               ,0                ,0}};

  AimsParseOptions( &argc, argv, opt, usage );


  AimsSurfaceTriangle	mesh;
  Point3df		Xi,Xf;
  

  Xi[0] = 0;Xi[1] = 0;Xi[2] = 0;
  Xf[0] = 50;Xf[1] = 50;Xf[2] = 0;

  
  mesh = MakePlaneMesh(n,Xi,Xf);
  
  cout << "mesh vertices : " << mesh[0].vertex().size() << endl;
  cout << "mesh polygons : " << mesh[0].polygon().size() << endl;

  TimeTexture<short>	tex(1,n*n);  
  unsigned i,j,i1,i2,i3,i3b,i4,j1,j2,j3,j4;
 
  cout << "Object texture dim   : " << tex[0].nItem() << endl;
  
  i1 = 5;
  i2 = 30;
  i3 = 50;
  i3b = 60;
  i4 = 95;
 
  j1 = 5; 
  j2 = 35;
  j3= 65;
  j4= 95 ;

  for (j=0;j<n;++j)
    for (i=0;i<n;++i)  
      {
	/*
	if (i>i3 && i<i4 && j==j1)
	tex[0].item(i+j*n)=(short)l6;

	if (i>i3b && i<i4-20 && j==j2)
	  tex[0].item(i+j*n)=(short)l5;
	if (i>i4-15 && i<i4-8 && j==j2)
	  tex[0].item(i+j*n)=(short)l5;
	if (i>i4-5 && i<i4 && j==j2)
	  tex[0].item(i+j*n)=(short)l5;

	
	if (i>i3b && i<i4-30 && j==j3)
	  tex[0].item(i+j*n)=(short)l4;
	if (i>i4-25 && i<i4-15 && j==j3)
	  tex[0].item(i+j*n)=(short)l4;
	if (i>i4-10 && i<i4 && j==j3)
	  tex[0].item(i+j*n)=(short)l4;

	if (i>i3b && i<i4 && j==j4)
	  tex[0].item(i+j*n)=(short)l3;
	*/

	if (j>j1 && j<j4-30 && i==i1+30)
	  tex[0].item(i+j*n)=(short)l1;
	
	if (j>j4-20 && j<j4 && i==i1+30)
	  tex[0].item(i+j*n)=(short)l1;
	
	if (j>j1 && j<j4-50 && i==i2+30)
	  tex[0].item(i+j*n)=(short)l2;
	
	if (j>j4-50 && j<j4 && i==i2+30)
	  tex[0].item(i+j*n)=(short)l7;
      } 

  //Writting the textures
  cout << "writing texture : " << flush;
  Writer<TimeTexture<short> >	texW( texfile );
  texW <<  tex ;
  cout << "done" << endl;
  
  // Writting the mesh
  cout << "writing mesh : " << flush;
  Writer<AimsSurfaceTriangle >	texM( meshfile );
  texM <<  mesh ;
  cout << "done" << endl;

  return( 0 );  
}



