/* Copyright (c) 1995-2005 CEA
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
#include <aims/getopt/getopt2.h>
#include <aims/data/data.h>
#include <aims/io/reader.h>
#include <aims/io/tiffR.h>
#include <aims/io/writer.h>
#include <list>
#include <string>


using namespace aims;
using namespace carto;
using namespace std;


int main( int argc, const char **argv )
{

  string fileIn;
  string fileOut;

  AimsApplication app( argc, argv,
     " Lit des données Tiff" );
  app.addOption( fileIn, "-i", "input data filename" );
  app.addOption( fileOut, "-o", "output label data filename", true );

  try
  {

    app.initialize();


    TiffReader<  ushort > reader( fileIn );
    AimsData<  ushort > data;

    reader >> data;


    cout << "DBG TIFF " << data.dimX() << " "
	 << data.dimY() << " "
	 << data.dimZ() << " "
	 << data.dimT() << endl;

    cout  << "DBG TIFF " << data(10, 10, 0,0) << endl;
    cout  << "DBG TIFF " << data(1000, 100, 0,0) << endl;
    cout  << "DBG TIFF " << data(1200, 500, 0,0) << endl;
    cout  << "DBG TIFF " << data(2000, 4000, 0,0) << endl;


    Writer< AimsData<ushort> > w("essai");
    w << data;
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

