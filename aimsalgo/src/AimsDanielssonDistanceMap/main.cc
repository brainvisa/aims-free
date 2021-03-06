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

#include <aims/data/data.h>
#include <aims/distancemap/danielsson.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/getopt/getopt2.h>

using namespace aims;
using namespace carto;
using namespace std;


int main( int argc, const char **argv )
{
  Reader<AimsData<short> > reader;
  Writer<AimsData<float> > writer;

  string c_side;

  AimsApplication app( argc, argv, "Danielsson's distance map on a volume" );
  app.addOption( reader, "-i", "source volume" );
  app.addOption( writer, "-o", "destination volume" );
  app.addOption( c_side, "-s", "OUTSIDE, INSIDE, SIGNED [default=SIGNED]",
                 true );
  app.alias( "--input", "-i" );
  app.alias( "--output", "-o" );
  app.alias( "--side", "-s" );

  try
  {
    app.initialize();

    string side = "SIGNED";
    if ( !c_side.empty() )
    {
      side = c_side;
      ASSERT( side == "INSIDE" || side == "OUTSIDE" || side == "SIGNED" );
    }

    AimsData<short> vol;

    if( !reader.read( vol, 1 ) )
      return EXIT_FAILURE;

    AimsData<float> map;

    if ( side == "INSIDE" )
      map = AimsDanielssonDistanceMap( vol, AIMS_DANIELSSON_INSIDE );
    else if ( side == "OUTSIDE" )
      map = AimsDanielssonDistanceMap( vol, AIMS_DANIELSSON_OUTSIDE );
    else
      map = AimsSignedDanielssonDistanceMap( vol );

    if( ! writer.write( map ) )
    {
      throw logic_error( "Internal error: write failed" );
    }

    return EXIT_SUCCESS;
  }
  catch( user_interruption & )
  {
  }
  catch( exception & e )
  {
    cerr << e.what() << endl;
  }
  return EXIT_FAILURE;
}
