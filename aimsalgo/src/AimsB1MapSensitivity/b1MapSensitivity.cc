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


#include <aims/data/data_g.h>
#include <aims/io/io_g.h>
#include <aims/getopt/getopt2.h>
#include <exception>
#include <iomanip>
#include <math.h>

using namespace std;
using namespace carto;
using namespace aims;


int main( int argc, const char* argv[] )
{

  int result = EXIT_SUCCESS;

  try
  {

    std::string fileNameIn1;
    std::string fileNameIn2;
    std::string fileNameOut;
    int coilCount = 4;
    bool ratio = false;
    float threshold = 0.1;
    bool verbose = false;

    // option(s) management
    AimsApplication 
    application( argc, argv,
     "B1 map sensitivity calculation from 2 echo data sets (30deg + 60deg)" );
    application.addOption( fileNameIn1, "-i1",
                           "30deg image / surface image " );
    application.addOption( fileNameIn2, "-i2",
                           "60deg image / body image " );
    application.addOption( fileNameOut, "-o",
                           "Output sensitivity map" );
    application.addOption( coilCount, "-c",
                           "Coil count (default=4)", 1 );
    application.addOption( threshold, "-t",
                           "Threshold on input data (default=0.1)", 1 );
    application.addOption( ratio, "-ratio",
                           "Using ratio method instead of 30deg+60deg method",
                           true );
    application.addOption( verbose, "-verbose",
                           "Show as much information as possible", true );
    application.initialize();


    int x, y, z, c;

    // reading 1st data
    if ( verbose )
      if ( ratio )
        cout << "reading surface coil data \'" << fileNameIn1 << "\': "
             << flush;
      else
        cout << "reading 30deg data \'" << fileNameIn1 << "\': "
             << flush;
    
    Reader< AimsData< short > > reader1( fileNameIn1 );
    AimsData< short > tempEcho1;

    reader1 >> tempEcho1;

    AimsData< short > echo1( tempEcho1.dimX(),
                             tempEcho1.dimY(),
                             tempEcho1.dimZ() / ( coilCount + 1 ),
                             coilCount + 1 );
    echo1.setSizeX( tempEcho1.sizeX() );
    echo1.setSizeY( tempEcho1.sizeY() );
    echo1.setSizeZ( tempEcho1.sizeZ() );

    for ( z = 0; z < tempEcho1.dimZ(); z++ )
    {

      for ( y = 0; y < tempEcho1.dimY(); y++ )
      {

        for ( x = 0; x < tempEcho1.dimX(); x++ )
        {

          echo1( x, y, z / ( coilCount + 1 ), z % ( coilCount + 1) ) =
            tempEcho1( x, y, z );
        }

      }

    }

    if ( verbose )
      cout << "done" << endl;

    // reading 2nd data
    if ( verbose )
      if ( ratio )
        cout << "reading body coil data \'" << fileNameIn2 << "\': "
             << flush;
      else
        cout << "reading 60deg data \'" << fileNameIn2 << "\': "
             << flush;
    
    Reader< AimsData< short > > reader2( fileNameIn2 );
    AimsData< short > tempEcho2;

    reader2 >> tempEcho2;

    AimsData< short > echo2;
    if ( !ratio )
    {

      echo2 = AimsData< short >( tempEcho2.dimX(),
                                 tempEcho2.dimY(),
                                 tempEcho2.dimZ() / ( coilCount + 1 ),
                                 coilCount + 1 );
      echo2.setSizeX( tempEcho2.sizeX() );
      echo2.setSizeY( tempEcho2.sizeY() );
      echo2.setSizeZ( tempEcho2.sizeZ() );

      for ( z = 0; z < tempEcho2.dimZ(); z++ )
      {

        for ( y = 0; y < tempEcho2.dimY(); y++ )
        {

          for ( x = 0; x < tempEcho2.dimX(); x++ )
          {

            echo2( x, y, z / ( coilCount + 1 ), z % ( coilCount + 1) ) =
              tempEcho2( x, y, z );
          }

        }

      }

    }
    else
    {

      echo2 = tempEcho2;

    }

    if ( verbose )
      cout << "done" << endl;

  Writer< AimsData< short > > w1( "echo1" );
  Writer< AimsData< short > > w2( "echo2" );
  w1 << echo1;
  w2 << echo2;

    // processing sensitivity

    AimsData< float > sensitivity( echo1.dimX(),
                                   echo1.dimY(),
                                   echo1.dimZ(),
                                   echo1.dimT() );
    sensitivity.setSizeX( echo1.sizeX() );
    sensitivity.setSizeY( echo1.sizeY() );
    sensitivity.setSizeZ( echo1.sizeZ() );
    sensitivity.setSizeT( echo1.sizeT() );

    sensitivity = 0.0;

    if ( ratio )
    {


      float maxEcho1 = 0.0, maxEcho2 = 0.0;

      for ( z = 0; z < echo1.dimZ(); z++ )
      {

        for ( y = 0; y < echo1.dimY(); y++ )
        {

          for ( x = 0; x < echo1.dimX(); x++ )
          {

            if ( echo1( x, y, z, echo1.dimT() - 1 ) > maxEcho1 )
            {

              maxEcho1 = echo1( x, y, z, echo1.dimT() - 1 );

            }

          }

        }

      }


      for ( z = 0; z < echo2.dimZ(); z++ )
      {

        for ( y = 0; y < echo2.dimY(); y++ )
        {

          for ( x = 0; x < echo2.dimX(); x++ )
          {

            if ( echo2( x, y, z ) > maxEcho2 )
            {

              maxEcho2 = echo2( x, y, z );

            }

          }

        }

      }

      for ( c = 0; c < echo1.dimT(); c++ )
      {

        for ( z = 0; z < echo1.dimZ(); z++ )
        {

          for ( y = 0; y < echo1.dimY(); y++ )
          {

            for ( x = 0; x < echo1.dimX(); x++ )
            {

              if ( ( ( echo1( x, y, z, c ) / maxEcho1 ) > threshold ) &&
                   ( ( echo2( x, y, z ) / maxEcho2 ) > threshold ) )
              {

                 sensitivity( x, y, z, c ) =
                                         ( echo1( x, y, z, c ) / maxEcho1 ) /
                                         ( echo2( x, y, z ) / maxEcho2 );

              }

            }

          }

        }

      }

    }
    else
    {

      for ( c = 0; c < echo1.dimT(); c++ )
      {

        for ( z = 0; z < echo1.dimZ(); z++ )
        {

          for ( y = 0; y < echo1.dimY(); y++ )
          {

            for ( x = 0; x < echo1.dimX(); x++ )
            {

              if ( ( echo1( x, y, z, c ) > threshold ) &&
                   ( echo2( x, y, z, c ) > threshold ) )
              {

                 sensitivity( x, y, z, c ) = /*acos(*/
                                       ( float )echo1( x, y, z, c ) /
                                       ( 2 * ( float )echo2( x, y, z, c ) )/* ) /
                                       M_PI*/;

              }

            }

          }

        }

      }

    }

    // savinbg data
    Writer< AimsData< float > > writer( fileNameOut );
    writer << sensitivity;
    
  }
  catch ( user_interruption& e )
  {
  }
  catch ( std::exception& e )
  {

    cerr << argv[ 0 ] << ": " << e.what() << endl;
    result = EXIT_FAILURE;

  }

  return result;

}
