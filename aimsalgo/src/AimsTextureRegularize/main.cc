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
#include <aims/math/math_g.h>
#include <iomanip>
#include <aims/getopt/getopt2.h>
#include <aims/mesh/texture.h>
#include <aims/mesh/geometric.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>

using namespace aims;
using namespace carto;
using namespace std;

   
   

int main( int argc, const char** argv )
{
  try
    {
      string  outtexfile, curvtexfile;
      float ratio= 0.995;

      AimsApplication	app( argc, argv, "Regularize the texture" );
      app.addOption( curvtexfile, "-t", "texture file" );
      app.alias( "--texture", "-t" );
      app.addOption( outtexfile, "-o", "output texture" );
      app.alias( "--output", "-o" );
      app.addOption( ratio, "-r", "proportion (%) of point to be thresholded ", true );
      app.alias( "--ratio", "-r" );
      app.initialize();

     
      //
      // read input curv texture
      //
      cout << "reading texture   : " << flush;
      TimeTexture<float>	curvTex;
      Reader<TimeTexture<float> > ctexR( curvtexfile );
      ctexR >> curvTex;
      cout << "done" << endl;

      cout << "texture dim   : " << curvTex[0].nItem() << endl;

      TimeTexture<float>	outTex;
      Curvature::regularize(curvTex[0],ratio);
      Curvature::getTextureProperties(curvTex[0]);
        
      cout << "writing texture : " << flush;
      Writer<TimeTexture<float> >	texW( outtexfile);
      texW << outTex;
      cout << "done" << endl;

      return( 0 );
    }
  catch( user_interruption & )
    {
    }
  catch( exception & e )
    {
      cerr << e.what() << endl;
    }
  return 1;
}


