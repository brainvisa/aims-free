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

//--- aims -------------------------------------------------------------------
#include <aims/getopt/getopt2.h>                     // create application
#include <aims/connectivity/connectivity.h>          // connectivity enumeration
#include <aims/connectivity/structuring_element.h>   // structuring element class
#include <aims/data/data_g.h>
#include <aims/vector/vector.h>
#include <aims/io/io_g.h>
//--- cartobase --------------------------------------------------------------
#include <cartobase/object/object.h>                 // options
#include <cartobase/config/verbose.h>                // verbosity level and cartoMsg
//--- system -----------------------------------------------------------------
#include <iostream>
#include <vector>
//----------------------------------------------------------------------------

using namespace carto;
using namespace aims;
using namespace std;


int main( int argc, const char** argv )
{
  try
  {
    string fileOut;
    string type = "26";
    bool usecenter = false;
    double amplitude = 1.;
    
    
    //=== APPLICATION ========================================================
    AimsApplication  app( argc, argv, "Test for structuring elements" );
    app.addOption( type, "-c", "structuring element connectivity 4xy / 4xz / 4yz / 6 / 8xy / 8xz / 8yz / 18 / 26 / cube / sphere [default: 26]", true );
    app.addOption( usecenter, "-u", "uses the element himself in the structuring element [default: false]", true );
    app.addOption( amplitude, "-a", "structuring element amplitude (number of voxels) [default: 1]", true );
    app.addOption( fileOut, "-o", "output image" );
    app.alias( "-v", "--verbose" );
    app.alias( "-d", "--debugLevel" );

    app.initialize();
    
    StructuringElement se = StructuringElement::getStructuringElement( type, 
                                                                               Point3d(amplitude, 
                                                                                       amplitude, 
                                                                                       amplitude),
                                                                               amplitude );

    AimsData<int16_t> out( (int)(2 * amplitude + 1),
                           (int)(2 * amplitude + 1),
                           (int)(2 * amplitude + 1) );
    out = 0;
    
    aims::StructuringElement::iterator ic, ie = se.end();
    
    for (ic = se.begin(); ic != ie; ++ic){
      if (verbose)
        std::cout << "Add point :" << *ic << std::endl;
      out(*ic) = 1;
    }
    
    Writer<AimsData<int16_t> > w(fileOut);
    w << out;

    cartoMsg( 1, "OK", "STRUCTURINGELEMENT_TEST" );
    
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
