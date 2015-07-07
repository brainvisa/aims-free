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
#include <aims/io/io_g.h>
#include <aims/getopt/getopt2.h>
#include <aims/mesh/texture.h>
#include <float.h>
#include <assert.h>



using namespace aims;
using namespace aims::meshdistance;
using namespace carto;
using namespace std;
 
typedef float float3[3];


int main( int argc, const char** argv )
{
  float dist = FLT_MAX;
  bool connexity = false;
  short background = 0 ;
  short forbidden = -1;
  int T = 0;
  Reader<AimsSurfaceTriangle> triR;
  Writer<TimeTexture<short> > texW;
  Reader<TimeTexture<short> > texR;

  AimsApplication app( argc, argv,
                       "Compute the Voronoi diagram of the input (short int) "
                       "seed texture" );
  app.addOption( triR, "-m", "input_mesh" );
  app.addOption( texW, "-o", "output voronoi diagram texture" );
  app.addOption( texR, "-t", "seeds texture definition (short int)" );
  app.addOption( T, "-T", "time step <default = 0>", true );
  app.addOption( background, "-b", "background value <default = 0>", true );
  app.addOption( forbidden, "-f", "value out of the domain <default = -1>",
                 true );
  app.addOption( dist, "-s", "step size <default = infinity>", true );
  app.addOption( connexity, "-c",
                 "connectivity or geodesic euclidean distance "
                 "<default: euclidean>", true );
  app.alias( "--mesh", "-m" );
  app.alias( "--output", "-o" );
  app.alias( "--texture", "-t" );
  app.alias( "--Time", "-T" );
  app.alias( "--background", "-b" );
  app.alias( "--forbidden", "-f" );
  app.alias( "--step", "-s" );
  app.alias( "--connexity", "-c" );
  app.alias( "--connectivity", "-c" );

  try
  {
    app.initialize();

    //
    // read triangulation
    //
    cout << "reading white triangulation   : " << flush;
    AimsSurfaceTriangle surface;
    triR >> surface;
    cout << "done" << endl;

    //
    // read input texture
    //
    cout << "reading texture   : " << flush;
    TimeTexture<short>    seed, voronoi;
    texR >> seed;
    cout << "done" << endl;

    cout << "mesh vertices : " << surface[0].vertex().size() << endl;
    cout << "mesh polygons : " << surface[0].polygon().size() << endl;
    cout << "Object texture dim   : " << seed[0].nItem() << endl;
    cout << "Background label: " << background << " Forbidden label : "
      << forbidden << endl;
    if ( ! (  seed[0].nItem() ==  surface[0].vertex().size() ) )
      {
        cerr << "The triangulation and the textures must correspond to the same object \n";
        assert( 0 );
      }

    cout << "Compute the voronoi diagram\n";
    voronoi[0] = MeshVoronoiT<short>( surface[0], seed[(unsigned) T],
                                      background ,forbidden, dist,
                                      connexity, true );
    cout << "writing texture : " << flush;
    texW <<  voronoi ;
    cout << "done" << endl;

    return EXIT_SUCCESS;
  }
  catch( user_interruption & )
  {
  }
  catch( exception & e )
  {
    cerr << e.what() << endl;
  }

  return EXIT_FAILURE;
}

