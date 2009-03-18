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
#include <aims/data/data.h>
#include <aims/io/io_g.h>
#include <aims/getopt/getopt.h>
#include <aims/vector/vector.h>
#include <aims/mesh/texture.h>
#include <aims/def/path.h>
#include <aims/data/pheader.h>
#include <aims/mesh/surface.h>
#include <aims/bucket/bucket.h>
#include <graph/graph/gwriter.h>
#include <graph/graph/graph.h>
#include <cartobase/object/sreader.h>
#include <cartobase/stream/directory.h>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>

using namespace aims;
using namespace carto;
using namespace std;


BEGIN_USAGE(usage)
  "-------------------------------------------------------------------------",
  "AimsMeshCC2Arg    -i[nput] <meshfilein>                                  ",
  "                  -v[olume] <input_volume>                               ", 
  "                  -t[exture] <input_texture>                             ",
  "                  [-o[utput] <output_arg>]                               ",
  "                  [-h[elp]]                                              ",
  "-------------------------------------------------------------------------",
  " Converts Connected Component texture to graph                           ",
  "-------------------------------------------------------------------------",
  "     meshfilein          : input *.tri or *.mesh file                    ",
  "     input_volume        : for voxel sizes                               ", 
  "     output_arg          : output *.arg file                             ",
  "     input_texture       : CC definition                             ",
  "-------------------------------------------------------------------------",
END_USAGE


//
// Usage
//
void Usage( void )
{
  AimsUsage( usage );
}


class lessPoint
{
public:
  bool operator () ( const Point3d & p1, const Point3d & p2 )
    {
      return( p1[0] < p2[0] || ( p1[0] == p2[0] 
				 && ( p1[1] < p2[1] 
				      || ( p1[1] == p2[1] && p1[2] < p2[2] ) ) ) );
    }
};


int main( int argc, char** argv )
{
  char	*meshfile = 0, *intexfile = 0, *outargfile = 0, *volfile = 0;

  //
  // Parser of options
  //
  AimsOption opt[] = {
  { 'h',"help"         ,AIMS_OPT_FLAG  ,(void *) Usage  ,AIMS_OPT_CALLFUNC,0},
  { 'i',"input"        ,AIMS_OPT_STRING,&meshfile       ,0                ,1},
  { 'o',"output"       ,AIMS_OPT_STRING,&outargfile     ,0                ,1},
  { 't',"texture"      ,AIMS_OPT_STRING,&intexfile      ,0                ,1},
  { 'v',"volume"       ,AIMS_OPT_STRING,&volfile        ,0                ,1}, 
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
  // read input texture
  //
  cout << "reading texture   : " << flush;
  TimeTexture<float>	inpTex;
  Reader<Texture1d> texR( intexfile );
  texR >> inpTex;
  cout << "done" << endl;

  //
  // read volume
  //
  Finder	f;
  assert( f.check( volfile ) );
  const PythonHeader 
    *hd = dynamic_cast<const PythonHeader *>( f.header() );
  if( !hd )
    {
      cerr << "can't get info on " << volfile << endl;
      exit(EXIT_FAILURE);
    }
  vector<float>	vs = hd->getProperty( "voxel_size" )->value<vector<float> >();
  cout << "voxel size : " << vs[0] << ", " << vs[1] << ", " << vs[2] << endl;

  cout << "mesh vertices : " << surface[0].vertex().size() << endl;
  cout << "mesh polygons : " << surface[0].polygon().size() << endl;
  cout << "texture dim   : " << inpTex[0].nItem() << endl;

  unsigned	i, n = inpTex.nItem();
  float		tmax = -1;

  for( i=0; i<n; ++i )
    {
      if( inpTex.item( i ) > tmax )
	tmax = inpTex.item( i );
    }
  cout << "nb of nodes : " << tmax;

  const vector<Point3df>	&vert = surface.vertex();
  Graph				gr( "ClusterArg" );
  unsigned			nn = (unsigned) tmax, val;
  map<unsigned, Vertex *>		vmap;
  map<unsigned, AimsBucket<Void> *>	bmap;
  map<unsigned, AimsSurfaceTriangle *>	smap;
  map<unsigned, set<Point3d, lessPoint> >		pmap;
  AimsBucket<Void>			*bck;

  for( i=1; i<nn; ++i )
    {
      vmap[i] = gr.addVertex( "cluster" );
      bck = new AimsBucket<Void>;
      bmap[i] = bck;
      bck->setSizeXYZT( vs[0], vs[1], vs[2], 1 );
      smap[i] = new AimsSurfaceTriangle;
    }

  gr.setProperty( "voxel_size", vs );
  gr.setProperty( "ClusterArg_VERSION", string( "1.0" ) );
  gr.setProperty( "type.bck", string( "cluster.bck" ) );
  gr.setProperty( "cluster.bck", string( "cluster bucket_filename" ) );

  string	fbase, aname = outargfile;
  string::size_type	pos2;

  string::size_type	pos = aname.rfind( '/' );
  if( pos == string::npos )
    fbase = "";
  else
    {
      fbase = aname.substr( pos+1, aname.length() - pos - 1 );
      aname.erase( 0, pos );
    }
  pos2 = aname.rfind( '.' );
  if( pos2 != string::npos )
    aname.erase( pos2, aname.length() - pos2 );
  aname += ".data";
  fbase += aname;

  gr.setProperty( "filename_base", fbase );

  // ...

  vector<int>	bbmin, bbmax;
  bbmin.push_back( INT_MAX );
  bbmin.push_back( INT_MAX );
  bbmin.push_back( INT_MAX );
  bbmax.push_back( INT_MIN );
  bbmax.push_back( INT_MIN );
  bbmax.push_back( INT_MIN );

  for( i=0; i<n; ++i )
    {
      val = (unsigned) inpTex.item( i );
      if( val >= 1 )
	{
	  const Point3df	& p = vert[i];
	  Point3d		p3( (int) ( p[0]/vs[0] + 0.5 ), 
				    (int) ( p[1]/vs[1] + 0.5 ), 
				    (int) ( p[2]/vs[2] + 0.5 ) );
	  pmap[val].insert( p3 );
	  if( p3[0] < bbmin[0] )
	    bbmin[0] = p3[0];
	  if( p3[1] < bbmin[1] )
	    bbmin[1] = p3[1];
	  if( p3[2] < bbmin[2] )
	    bbmin[2] = p3[2];
	  if( p3[0] > bbmax[0] )
	    bbmax[0] = p3[0];
	  if( p3[1] > bbmax[1] )
	    bbmax[1] = p3[1];
	  if( p3[2] > bbmax[2] )
	    bbmax[2] = p3[2];
	}
    }

  gr.setProperty( "boundingbox_min", bbmin );
  gr.setProperty( "boundingbox_max", bbmax );

  set<Point3d>::const_iterator	ip, ep;
  AimsBucketItem<Void>	bi;

  for( i=1; i<nn; ++i )
    {
      AimsBucket<Void>	& b = *bmap[i];
      for( ip=pmap[i].begin(), ep=pmap[i].end(); ip!=ep; ++ip )
	{
	  bi.location() = *ip;
	  b.push_back( bi );
	}
    }

  /* pour plus tard...
  const vector<Point3d>	& poly= surface.polygon();
  unsigned		nt = poly.size();

  for( i=0; i<nt; ++i )
    {
      const Point3d	& po = poly[i];
      if( inpTex.item( po[0] ) == inpTex.item( po[1]) 
	  && inpTex.item( po[0] ) == inpTex.item( po[2] ) )
	;
    }
  */

  // save

  char		num[10];
  Vertex	*v;

  Directory	dir( fbase );
  dir.mkdir();
  cout << "writing buckets...\n";

  for( i=1; i<nn; ++i )
    {
      sprintf( num, "%d", i );
      string	bname = string( "bucket_" ) + num + ".bck";
      Writer<AimsBucket<Void> >	bw( fbase + "/" + bname );
      bw << *bmap[i];
      v = vmap[i];
      v->setProperty( "bucket_filename", bname );
      v->setProperty( "name", string( "unknown" ) );
    }

  SyntaxSet	ss;
  try
    {
      SyntaxReader	sr( Path::singleton().syntax() + "/graph.stx" );
      sr >> ss;
    }
  catch( exception & e )
    {
      cerr << e.what() << endl;
      exit(EXIT_FAILURE);
    }

  try
    {
      GraphWriter	gw( outargfile, ss );
      gw << gr;
    }
  catch( exception & e )
    {
      cerr << e.what() << endl;
      exit(EXIT_FAILURE);
    }

  cout << "done" << endl;

  return 0;
}
