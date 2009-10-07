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

#include <cstdlib>
#include <aims/getopt/getopt2.h>
#include <aims/data/data.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <list>
#include <string>


using namespace aims;
using namespace carto;
using namespace std;


int main( int argc, const char **argv )
{

  string fileIn;
  list< short > labels;
  string fileOut;
  short background = -1;
  int x, y, z, t;
  bool keepIt;

  AimsApplication app( argc, argv,
     "Select labels from a label data and put it into a new label data into"
     " a new label data" );
  app.addOption( fileIn, "-i", "input label data filename" );
  app.addOptionSeries( labels, "-l",
                       "selected label values", 1 );
  app.addOption( fileOut, "-o", "output label data filename" );
  app.addOption( background, "-b",
                 "background label (default=-1)", 1 );

  try
  {

    app.initialize();

    if ( labels.empty() )
    {

      cerr << "there should be at least 1 label selected" << endl;
      throw;

    }


    Reader< AimsData< short > > reader( fileIn );
    AimsData< short > data;

    cout << "reading input label data" << flush;
    reader >> data;
    cout << endl;

    cout << "selecting labels" << flush;
    list< short >::const_iterator l = labels.begin(), le = labels.end();
    for ( t = 0; t < data.dimT(); t++ )
      for ( z = 0; z < data.dimZ(); z++ )
        for ( y = 0; y < data.dimY(); y++ )
          for ( x = 0; x < data.dimX(); x++ )
          {

            
            short value = data( x, y, z, t );

            l = labels.begin();
            keepIt = false;
            while ( l != le )
            {

              if ( *l == value )
              {

                keepIt = true;

              }
              l ++;

            }
            if ( !keepIt )
            {

              data( x, y, z, t ) = background;

            }

          }
    cout << endl;


    cout << "saving selection" << flush;
    Writer< AimsData< short > > writer( fileOut );
    writer << data;
    cout << endl;

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


