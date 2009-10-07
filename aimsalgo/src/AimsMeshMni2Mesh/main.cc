/* This software and supporting documentation are distributed by
 *     Institut Federatif de Recherche 49
 *     CEA/NeuroSpin, Batiment 145,
 *     91191 Gif-sur-Yvette cedex
 *     France
 *
 * This software is governed by the CeCILL-B license under
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the
 * terms of the CeCILL-B license as circulated by CEA, CNRS
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
 * knowledge of the CeCILL-B license and that you accept its terms.
 */

 
#include <cstdlib>
#include <aims/io/io_g.h>
#include <aims/mesh/surface.h>
#include <aims/data/data.h>
#include <aims/getopt/getopt.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <fstream>
#include <iomanip>
#include <list>
#include <vector>

using namespace aims;
using namespace std;


typedef float float3[3];

BEGIN_USAGE(usage)
  "-------------------------------------------------------------------------",
  "siMeshMni2Mesh           -i[nput] <meshfilein>                           ",
  "                         -o[utput] <meshfileout>                         ",
  "                         [-h[elp]]                                       ",
  "-------------------------------------------------------------------------",
  " Transform MNI mesh format (.obj) to SHFJ mesh format (.tri or .mesh)    ",
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
  char	 *meshfile = 0,*outmeshfile=0;
  
  //
  // Parser of options
  //
  AimsOption opt[] = {
    { 'h',"help"         ,AIMS_OPT_FLAG  ,( void* )Usage           ,AIMS_OPT_CALLFUNC,0},
    { 'i',"input"        ,AIMS_OPT_STRING,&meshfile       ,0                ,1},
    { 'o',"output"       ,AIMS_OPT_STRING,&outmeshfile    ,0                ,1},
    { 0  ,0              ,AIMS_OPT_END   ,0               ,0                ,0}};

  AimsParseOptions( &argc, argv, opt, usage );

  unsigned				nb_node,nb_triangle;
  vector<Point3df>			vert , normal  ;
  vector< AimsVector<uint,3> >		poly;
  Point3df				X;
  AimsVector<uint,3>			P;

  // read mni mesh 
  cout << "Read MNI mesh \n";
  ifstream 	mf(meshfile);
  string 	l;
  float         f;
  unsigned      i;
  
  mf >> l;
  if (l != "P")
    {
      cout << "Not MNI mesh file" << endl;
      exit(0);
    }
  
  for (i=0; i<5;++i) //amb   diff  spec    spec    opacity
      mf >> f;
    
  
  mf >>  nb_node ;

  
  for (i=0; i<nb_node;++i) //node coordinates
    {
      mf >> X[0];
      mf >> X[1];
      mf >> X[2];
      X[2] += 72; //repere chgmt
      X[1] += 126;
      X[0] += 90;
      X[0] = 180 - X[0]; //flip
      X[1] = 216 - X[1];
      X[2] = 180 - X[2];
      vert.push_back(X);}
      

  for (i=0; i<nb_node;++i)//normal coordinates
    {
      mf >> X[0];
      mf >> X[1];
      mf >> X[2];
      X[0] *= -1;
      X[1] *= -1;
      X[2] *= -1;
      normal.push_back(X);
    }

  mf >>  nb_triangle ;

  mf >> i;
  if (i == 2)
    {
      cout << "Mesh with colored nodes...I try...\n";
      for (i=0; i<4 * nb_node;++i)//r g b alpha
	mf >> f;
    }
  else
    for (i=0; i<4;++i)
      mf >> f; //same r g b alpha for each node 

  for (i=0; i<nb_triangle;++i)//?!
      mf >> f;

  for (i=0; i<nb_triangle;++i)//triangle indices  
      {
	mf >> P[1];//inversed
	mf >> P[0];//inversed
	mf >> P[2];
	poly.push_back(P);
      }

  cout << "Nb of nodes: " <<  nb_node << endl;
  cout << "Nb of triangles: " <<  nb_triangle << endl;


  AimsSurfaceTriangle  mesh;
  mesh[0].vertex() = vert;
  mesh[0].normal() = normal;
  mesh[0].polygon() = poly;
  
  cout << "Write the mesh..." << flush;
  Writer<AimsSurfaceTriangle >	meshW(outmeshfile);
  meshW << mesh;
  cout << "done" << endl;

  return( 0 );
}
