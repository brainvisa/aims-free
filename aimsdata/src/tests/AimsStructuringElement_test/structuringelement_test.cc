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
//--- cartodata --------------------------------------------------------------
#include <cartodata/volume/volume.h>                 // volume/volumeref
//--- cartobase --------------------------------------------------------------
#include <cartobase/object/object.h>                 // options
#include <cartobase/config/verbose.h>                // verbosity level and cartoMsg
//--- system -----------------------------------------------------------------
#include <iostream>
#include <vector>
#include <exception>
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
    vector<double> amplitude(3,1.);

    //--- APPLICATION --------------------------------------------------------
    AimsApplication  app( argc, argv, "Test for structuring elements" );
    app.addOption( type, "-c", "structuring element connectivity "
                   "( 4xy / 4xz / 4yz / 6 / 8xy / 8xz / 8yz / 18 / 26 / ... ) "
                   "or shape ( cube / sphere / cross / ... ) "
                   "[default: 26]", true );
    app.addOption( usecenter, "-u", "uses the element himself in the "
                   "structuring element [default: false]", true );
    app.addOptionSeries( amplitude, "-a", "shape amplitude in voxels "
                         " (unused with connectivity) [default: 1 1 1]",
                         0, 3 );
    app.addOption( fileOut, "-o", "output image" );
    app.alias( "-v", "--verbose" );
    app.alias( "-d", "--debugLevel" );
    app.initialize();
    //------------------------------------------------------------------------

    //--- STREL IMAGE GENERATION ---------------------------------------------
    Point3d center( (int16_t) amplitude[0],
                    (int16_t) amplitude[1],
                    (int16_t) amplitude[2] );

    rc_ptr<StructuringElement> se;
    se.reset( strel::ShapeFactory::create(type,center,amplitude,usecenter) );
    if( !se.get() ) {
      se.reset( strel::ConnectivityFactory::create(type,center,usecenter) );
    }
    if( !se.get() ) {
      cout << type << " not found" << endl;
    } else {
      AimsData<int16_t> out( (int)(2 * amplitude[0] + 1),
                             (int)(2 * amplitude[1] + 1),
                             (int)(2 * amplitude[2] + 1) );
      out = 0;

      aims::StructuringElement::iterator ic, ie = se->end();
      
      for (ic = se->begin(); ic != ie; ++ic){
        if (verbose)
          std::cout << "Add point :" << *ic << std::endl;
        out(*ic) = 1;
      }

      Writer<AimsData<int16_t> > w(fileOut);
      w << out;
    }
    //------------------------------------------------------------------------

#if 0
    //--- STRUCTUREDVOLUME ---------------------------------------------------
    if( verbose )
      cout << "Testing StructuredVolume..." << endl;
    VolumeRef<int16_t> vol( new Volume<int16_t>(3,3,3) );
    VolumeRef<int16_t> win( new Volume<int16_t>(
      vol,
      Volume<int16_t>::Position4Di(1,1,1),
      Volume<int16_t>::Position4Di(1,1,1) ) );
    vol->fill( 0 );
    win->at(1,0,0) = 1;
    win->at(-1,0,0) = 1;
    win->at(0,1,0) = 1;
    win->at(0,-1,0) = 1;
    win->at(0,0,1) = 1;
    win->at(0,0,-1) = 1;
    StructuringElementRef con( strel::ConnectivityFactory::create("6") );
    StructuredVolume<int16_t> svol( win, *con );
    StructuredVolume<int16_t>::const_iterator i, e = svol.end();
    for( i=svol.begin(); i!=e; ++i ) {
      if( *i != 1 ) throw logic_error( "Bad iteration in structured volume" );
    }
    if( verbose)
      cout << "ok" << endl;
    //------------------------------------------------------------------------
#endif

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
