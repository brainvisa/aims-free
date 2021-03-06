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


#include <aims/io/io_g.h>
#include <aims/getopt/getopt2.h>
#include <aims/mesh/texture.h>
#include <aims/distancemap/meshvoronoi.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>

using namespace aims;
using namespace carto;
using namespace std;


int main( int argc, const char** argv )
{
  string outtexfile, meshfile, outmesh;

  //
  // Parser of options
  //
  AimsApplication app( argc, argv, "Average a time series mesh.\n"
    "WARNING: Be sure that the nodes at different time serie correspond to "
    "the same point..." );
  app.addOption( meshfile, "-m", "input time series mesh" );
  app.alias( "--mesh", "-m" );
  app.addOption( outmesh, "-o", "output mean mesh" );
  app.alias( "--output", "-o" );
  app.addOption( outtexfile, "-v", "variability output texture", true );
  app.alias( "--variability", "-v" );

  try
  {
    app.initialize();

    //
    // read triangulation
    //
    cout << "reading triangulation   : " << flush;
    AimsSurfaceTriangle surface,meansurf;
    Reader<AimsSurfaceTriangle> triR( meshfile );
    triR >> surface;
    cout << "done" << endl;

    unsigned            j, i, nnodes = surface.vertex().size();
    unsigned            ntimes = surface.size();
    TimeTexture<float>  outTex(1,nnodes);
    float m;

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
        outTex[0].item(j) = m;

      }
    cout << "done\n";


    cout << "Build connectivity" << endl;
    meansurf[0].polygon() = surface[0].polygon();

    if( !outtexfile.empty() )
    {
      cout << "writing texture : " << flush;
      Writer<Texture1d>	texW( outtexfile );
      texW.write( outTex );
      cout << "done " << endl;
    }

    cout << "Write the mesh..." << flush;
    Writer<AimsSurfaceTriangle >	meshW( outmesh);
    meshW << meansurf;
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


