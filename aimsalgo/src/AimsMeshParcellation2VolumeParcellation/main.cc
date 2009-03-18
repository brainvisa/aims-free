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
#include <aims/distancemap/meshparcellation.h>
#include <aims/distancemap/meshdistance.h>
#include <aims/distancemap/projection.h>
#include <aims/data/data_g.h>
#include <aims/io/io_g.h>
#include <aims/math/math_g.h>
#include <iomanip>
#include <aims/vector/vector.h>
#include <aims/mesh/texture.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/connectivity/meshcc.h>
#include <fstream>
#include <float.h>
#include <aims/def/path.h>
#include <cartobase/object/sreader.h>
#include <cartobase/plugin/plugin.h>
#include <aims/getopt/getopt2.h>

using namespace aims;
using namespace aims::meshdistance;
using namespace carto;
using namespace std;


int main( int argc, char** argv )
{
  
  unsigned	time=0;
  short label_vol = 100;

  string fileout,volfile;
  Reader< TimeTexture<short> >	texR;
  Reader<AimsSurfaceTriangle >  triR;

  AimsApplication	app( argc, (const char**) argv, "Project a mesh parcellation "
			     "to a volume" );

  PluginLoader::load();

  //
  // Parser of options
  //
  app.addOption( texR, "-t", "input label (short) texture" );
  app.alias( "-texture", "-t" );
  app.addOption( triR, "-m", "input mesh" );
  app.alias( "-mesh", "-m" );
  app.addOption( fileout, "-o", "output parcellation volume" );
  app.alias( "-output", "-o" );
  app.addOption( volfile, "-v", "input volume file" );
  app.alias( "-volume", "-v" );
  app.addOption( label_vol, "-l", "label of the object in the volume (default = 100)", true );
  app.alias( "-label", "-l" );
  app.addOption( time, "-T", "make graph from time T of input texture", true );
  app.alias( "-Time", "-T" );
  app.alias( "-time", "-T" );

  try
    {
      app.initialize();
      //
      // read triangulation
      //
      cout << "reading white triangulation   : " << flush;
      AimsSurfaceTriangle surface;
      Reader<AimsSurfaceTriangle> triread( triR );
      triread >> surface;
      cout << "done" << endl;
 
      //
      // read input texture
      //
      cout << "reading texture   : " << flush;
      TimeTexture<short>	inpTex; 
      Reader<TimeTexture<short> > texread( texR );
      texread >> inpTex; 
      cout << "done" << endl;

      cout << "mesh vertices : " << surface[0].vertex().size() << endl;
      cout << "mesh polygons : " << surface[0].polygon().size() << endl;
      cout << "Object texture dim   : " << inpTex[0].nItem() << endl;
  
      if ( ! (  inpTex[0].nItem() ==  surface[0].vertex().size() ) )
        {
          cerr << "The triangulation and the textures must correspond to the same object \n";
          assert( 0 );
        }

      cout << "Reading object volume   : " << volfile << endl;
      AimsData<short> inpVol;
      Reader<AimsData<short> > triGV( volfile );
      triGV >> inpVol;


      cout << "Mesh parcellation to volume parcellation" << endl;
      AimsData<short>    parcellationVolume;

      parcellationVolume = MeshParcellation2Volume( inpVol,inpTex[time],surface[0],label_vol,0 );


      cout << "Write parcellation volume\n";
      Writer<AimsData<short> >	imaW( fileout );
      imaW <<  parcellationVolume ;   
  
      return EXIT_SUCCESS;
    }
  catch( user_interruption &e )
    {
    }
  catch( exception & e )
    {
      cerr << e.what() << endl;
    }

  return EXIT_FAILURE;
}

 
