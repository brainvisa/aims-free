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
#include <aims/rgb/rgb.h>


using namespace aims;
using namespace carto;
using namespace std;


int main( int argc, const char **argv )
{
  string fileInR, fileInG, fileInB, fileOut;

  AimsApplication app( argc, argv,
                  "Merge 3 AimsData<byte> files to an AimsData<AimsRGB>" );
  app.addOption( fileInR, "-r", "input U8 data R channel" );
  app.addOption( fileInG, "-g", "input U8 data G channel" );
  app.addOption( fileInB, "-b", "input U8 data B channel" );
  app.addOption( fileOut, "-o", "output data" );

  try
  {

    app.initialize();

    Reader< AimsData< byte > > reader;
    AimsData< byte > dataR, dataG, dataB;

    cout << "reading R channel";
    reader.setFileName( fileInR );
    reader >> dataR;
    cout << endl;

    cout << "reading G channel";
    reader.setFileName( fileInG );
    reader >> dataG;
    cout << endl;

    cout << "reading B channel";
    reader.setFileName( fileInB );
    reader >> dataB;
    cout << endl;

    if ( !( dataR.dimX() == dataG.dimX() &&
            dataG.dimX() == dataB.dimX() &&
            dataR.dimY() == dataG.dimY() &&
            dataG.dimY() == dataB.dimY() &&
            dataR.dimZ() == dataG.dimZ() &&
            dataG.dimZ() == dataB.dimZ() &&
            dataR.sizeX() == dataG.sizeX() &&
            dataG.sizeX() == dataB.sizeX() &&
            dataR.sizeY() == dataG.sizeY() &&
            dataG.sizeY() == dataB.sizeY() &&
            dataR.sizeZ() == dataG.sizeZ() &&
            dataG.sizeZ() == dataB.sizeZ() ) )
    {

      cerr << "R/G/B channel do not have same dim and size" << endl;
      throw;
      
    }


    cout << "building RGB data";
    int x, y, z, t;
    AimsData< AimsRGB > dataRgb( dataR.dimX(),
                                 dataR.dimY(),
                                 dataR.dimZ(),
                                 dataR.dimT() );
    dataRgb.setSizeX( dataR.sizeX() );
    dataRgb.setSizeY( dataR.sizeY() );
    dataRgb.setSizeZ( dataR.sizeZ() );
    dataRgb.setSizeT( dataR.sizeT() );
    cout << endl;

    for ( t = 0; t < dataRgb.dimT(); t++ )
      for ( z = 0; z < dataRgb.dimZ(); z++ )
        for ( y = 0; y < dataRgb.dimY(); y++ )
          for ( x = 0; x < dataRgb.dimX(); x++ )
            dataRgb( x, y, z, t ) = AimsRGB( dataR( x, y, z, t ),
                                             dataG( x, y, z, t ),
                                             dataB( x, y, z, t ) );
    cout << endl;

    cout << "saving RGB data";
    Writer< AimsData< AimsRGB > > writer( fileOut);
    writer.write(dataRgb);
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


