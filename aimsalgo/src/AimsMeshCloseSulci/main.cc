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


#include <aims/distancemap/meshvoronoi.h>
#include <aims/distancemap/meshdistance.h>
#include <aims/data/data_g.h>
#include <aims/io/io_g.h>
#include <aims/math/math_g.h>
#include <aims/getopt/getopt2.h>
#include <aims/vector/vector.h>
#include <aims/mesh/texture.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/connectivity/meshcc.h>
#include <iomanip>
#include <values.h>

using namespace aims;
using namespace aims::meshdistance;
using namespace carto;
using namespace std;

BEGIN_USAGE(usage)
  "-------------------------------------------------------------------------",
  "AimsMeshCloseSulci   -i[nput] <meshfilein>                                  ",
  "                	-t[exture] <input_texture>                             ",
  "                	-T[ime] <time_serie>                                   ",
  "                	[-m[ax]  <max_dilation>]                                ",
  "                	[--c[onnexity] ]                                       ",
  "                	[-o[utput] <output_vor_texture>                        ",
  "                	[-h[elp]]                                              ",
  "-------------------------------------------------------------------------",
  " Close Sulci defined on a texture                                      ",
  "-------------------------------------------------------------------------",
  "     meshfilein          : input *.tri or *.mesh file                    ",
  "     input_texture       : object definition (sulci)                     ",
  "     connexity           : connexity or geodesic euclidean distance      ",
  "     output_vor_texture : output *.tex file (voronoi diagram)            ",
  "-------------------------------------------------------------------------",
END_USAGE

int main( int argc, const char** argv )
{
  string meshfile, intexfile, outtexfile;
  float maxdil = 3;
  bool connexity = false;
  int time = 0;

  //
  // Parser of options
  //
  AimsApplication app( argc, argv, "Close Sulci defined on a texture" );
  app.addOption( meshfile, "-i", "input mesh file" );
  app.alias( "--input", "-i" );
  app.addOption( intexfile, "-t", "object definition (sulci)" );
  app.alias( "--texture", "-t" );
  app.addOption( outtexfile, "-o", "output texture file (voronoi diagram)" );
  app.alias( "--output", "-o" );
  app.addOption( time, "-T", "time step [default: 0]", true );
  app.alias( "--Time", "-T" );
  app.alias( "--time", "-t" );
  app.addOption( maxdil, "-m", "max dilation [default: 3mm]", true );
  app.alias( "--maxdilation", "-m" );
  app.addOption( connexity, "-c", "connectivity or geodesic euclidean distance "
    "[default: geodesic euclidean]", true );
  app.alias( "--connectivity", "-c" );
  app.alias( "--connexity", "-c" );

  try
  {
    app.initialize();

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
    TimeTexture<short>	inpTex; // objects def (labels >0)
    Reader<TimeTexture<short> > texR( intexfile );
    texR >> inpTex;

    cout << "done" << endl;

    cout << "mesh vertices : " << surface[0].vertex().size() << endl;
    cout << "mesh polygons : " << surface[0].polygon().size() << endl;
    cout << "Object texture dim   : " << inpTex[(unsigned)time].nItem() << endl;


    if ( ! (  inpTex[(unsigned)time].nItem() ==  surface[0].vertex().size() ) )
    {
      cout << "The triangulation and the textures must correspond to the same object \n";
      assert( 0 );
    }

   
    unsigned  n=inpTex[(unsigned)time].nItem();
    TimeTexture<short>	outTex(3,n);

    //outTex[1] = AimsMeshLabelConnectedComponent2Texture(surface[0],inpTex[(unsigned)time],(short)1 );
    outTex[0] = CloseSulci<short>(surface[0],inpTex[(unsigned)time],maxdil,connexity,0,-1);
    //outTex[0] = CloseSulci<short>(surface[0],outTex[1],maxdil,connexity,0,-1);

    // Writting the textures
    cout << "writing texture : " << flush;
    Writer<TimeTexture<short> >	texW( outtexfile );
    texW <<  outTex ;

    cout << "done" << endl;
  
  }
  catch( user_interruption & )
  {
  }
  catch( exception & e )
  {
    cerr << e.what() << endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}



