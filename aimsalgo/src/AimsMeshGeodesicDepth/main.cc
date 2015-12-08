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


#include <aims/distancemap/meshdistance.h>
#include <aims/data/data_g.h>
#include <aims/io/io_g.h>
#include <iomanip>
#include <aims/getopt/getopt2.h>
#include <aims/mesh/texture.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>

using namespace aims;
using namespace aims::meshdistance;
using namespace carto;
using namespace std;


int main( int argc, const char** argv )
{
  string meshfile, volumefile, outtexfile;
  float close = 10, erode = 2;

  //
  // Parser of options
  //
  AimsApplication app( argc, argv,
                       "Compute geodesic depth on a triangulation" );
  app.addOption( meshfile, "-i", "input mesh file" );
  app.alias( "--input", "-i" );
  app.addOption( outtexfile, "-o", "output depth texture file" );
  app.alias( "--output", "-o" );
  app.addOption( volumefile, "-v",
                 "brain hull definition (binary image relative to the "
                 "triangualtion)" );
  app.alias( "--volume", "-v" );
  app.addOption( close, "-c", "radius of the closing [default = 10]", true );
  app.alias( "--close", "-c" );
  app.addOption( erode, "-e", "radius of the erosion [default = 5]", true );
  app.alias( "--erosion", "-e" );

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



