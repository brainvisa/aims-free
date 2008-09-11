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


#include <aims/io/io_g.h>
#include <aims/data/data_g.h>
#include <aims/getopt/getopt2.h>
#include <aims/utility/utility_g.h>
#include <aims/def/path.h>
#include "activPixel.h"
#include "activPixelR.h"
#include "pix2vol.h"

using namespace aims;
using namespace carto;
using namespace std;

int main( int argc, const char **argv )
{
  try
    {
      Reader<AimsData<short> >	filein;
      string			pixfile;
      Writer<AimsData<short> >	fileout;
      short label = 255;
      Point3df trans;
      AimsApplication	app( argc, argv, "Convert an Activ pixel list file "
			     "into a volume" );
      trans[0] = trans[1] = trans[2] = 0.0f;
      app.addOption( filein, "-i", "origin file" );
      app.addOption( pixfile, "-p", "activ pixels list file" );
      app.addOption( fileout, "-o", "output image [default=input]", true );
      app.addOption( label, "-l", "filling value [default=255]", true );
      app.addOption( trans[0], "-x", 
		     "value of translation along x axis [default=0.0]", true );
      app.addOption( trans[1], "-y", 
		     "value of translation along y axis [default=0.0]", true );
      app.addOption( trans[2], "-z", 
		     "value of translation along z axis [default=0.0]", true );

      app.initialize();

      if( fileout.fileName().empty() )
	fileout.setFileName( filein.fileName() );

      cout << endl << "Reading template file : " << filein.fileName() << endl;
      AimsData< short >		vol;
      filein.read( vol );

      cout << "Reading pixel list file : " << pixfile << endl;
      AimsActivPixelReader reader( pixfile );
      AimsActivPixel pixels;
      reader >> pixels;

      cout << "Converting to volume..." << endl;
      AimsData< short > res = AimsActivPixel2Volume( vol, pixels, trans, 
						     label );

      cout << "Writing file : " << fileout.fileName() << endl;
      fileout.write( res );
    }
  catch( user_interruption &e )
    {
    }
  catch( std::exception &e )
    {
      cerr << argv[ 0 ] << ": " << e.what() << endl;
      return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}
