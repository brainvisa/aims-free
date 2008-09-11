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


#include <aims/mesh/geometric.h>
#include <aims/data/data_g.h>
#include <aims/io/io_g.h>
#include <aims/math/math_g.h>
#include <aims/vector/vector.h>
#include <aims/mesh/texture.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/getopt/getopt2.h>
#include <iostream>
#include <iomanip>

using namespace aims;
using namespace carto;
using namespace std;

typedef float float3[3];


int main( int argc, const char** argv )
{
  
  try
    {
      string  meshfile, outtexfile;
      string  method = "fem";
      float ratio= 0.95;

      AimsApplication	app( argc, argv, "Estime the mean curvature of a mesh" );
      app.addOption( meshfile, "-i", "input file" );
      app.alias( "--input", "-i" );
      app.addOption( outtexfile, "-o", "output texture" );
      app.alias( "--output", "-o" );
      app.addOption( method, "-m", "method" );
      app.alias( "--method", "-m" );
      app.addOption( ratio, "-r", "proportion (%) of point to be thresholded ", true );
      app.alias( "--ratio", "-r" );
      app.initialize();



      //
      // read triangulation
      //
      cout << "reading triangulation   : " << flush;
      AimsSurfaceTriangle surface;
      Reader<AimsSurfaceTriangle> triR( meshfile );
      triR >> surface;
      cout << "done" << endl;

      TimeTexture<float>	outTex;

  
      CurvatureFactory CF;
      Curvature * curv = CF.createCurvature(surface,method);
      cout << "processing..." << flush;
      outTex[0] = curv->doIt();
      cout << "done\n";
      curv->regularize(outTex[0],ratio);
      curv->getTextureProperties(outTex[0]);
      delete curv;
  
      cout << "writing texture : " << flush;
      Writer<Texture1d>	texW( outtexfile );
      texW.write( outTex );
      cout << "done " << endl;

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


