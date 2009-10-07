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
#include "contextEPI.h"
#include "interpolation.h"
#include <aims/def/assert.h>
#include <aims/getopt/getopt2.h>
#include <aims/data/data_g.h>
#include <aims/io/io_g.h>
#include <cartobase/allocator/mappingcopy.h>
#include <cartobase/thread/thread_g.h>
#include <string>
#include <iostream>
#include <iomanip>


using namespace std;
using namespace carto;
using namespace aims;


int main( int argc, const char* argv[] )
{

  int result = EXIT_SUCCESS;

  try
  {

    Reader< AimsData< short > > reader;
    std::string descriptionName = "";
    Writer< AimsData< short > > writer;
    int sizeDivider = 1;
    int levels = 64;
    string smoothingType = "discrete";
    float sigma = 3;
    Point3df deltaP( 0.05, 1.0, 0.01 );
    float error = 0.005;
    bool memMap = false;
    bool verbose = false;
    string parameterName = "";

    // option(s) management
    AimsApplication 
    application( argc, argv,
                "T2 based correction of echoplanar distortions in DW MR image\n"
                "Base on a model of geometric distortion characterized by:\n"
                "- a scaling S,\n"
                "- a translation T0,\n"
                "- a shearing T1\n" );
    application.addOption( reader, "-i", "T2+DW image" );
    application.addOption( descriptionName, "-d", "Description file" );
    application.addOption( writer, "-o",
                           "Corrected T2+DW image" );
    application.addOption( sizeDivider, "-divider",
                           "divider for subresolution calculations (default=1)",
                           1 );
    application.addOption( levels, "-levels",
                        "number of graylevels in MI calculations (default=64)",
                           1 );
    application.addOption( smoothingType, "-smoothing",
                           "pdf smoothing deriche/discrete (default=discrete)",
                           1 );
    application.addOption( sigma, "-sigma",
                           "discrete : gaussian mask size (default=3)\n"
                           "deriche  : gaussian std dev. (default=3.0mm)",
                           1 );
    application.addOption( deltaP[ 0 ], "-deltaS",
                           "variation of S (default=0.05)",
                           1 );
    application.addOption( deltaP[ 1 ], "-deltaT0",
                           "variation of T0 (default=1.00)",
                           1 );
    application.addOption( deltaP[ 2 ], "-deltaT1",
                           "variation of T1 (default=0.01)",
                           1 );
    application.addOption( error, "-error",
                           "tolerance on results (default=0.005)",
                           1 );
    application.addOption( memMap, "-memMap",
                           "memory mapping activated (obsolete, automatic)",
                           1 );
    application.addOption( parameterName, "-parameters",
                           "(S,T0,T1) parameter text file name "
                           "[default=not saved]",
                           1 );
    application.addOption( verbose, "-verbose",
                           "show as much information as possible",
                           1 );
    application.initialize();

    if ( smoothingType != "deriche" &&
         smoothingType != "discrete" )
    {

      throw runtime_error( "smoothing shoud be deriche or discrete" );

    }

    if ( descriptionName.empty() )
    {

      throw runtime_error( "T2 indices description file name empty" );

    }

    // reading distorded image
    if ( verbose )
      cout << "reading T2+DW (" << reader.fileName() << ") data : " << flush;
    AimsData<short> raw;
    reader.setAllocatorContext( AllocatorContext
                                ( AllocatorStrategy::ReadOnly, 
                                  DataSource::none(), false, 0.5 ) );
    reader >> raw;
    if ( verbose )
      cout << "done" << std::endl;


    if ( verbose )
      cout << "check that images are squared : " << flush;
    if ( raw.dimX() != raw.dimY() )
    {

      throw runtime_error( "T2+DW image is not squared" );

    }
    if ( verbose )
      cout << "done" << std::endl;

    int size = raw.dimX();
    int numLocation = raw.dimZ();
    int numRank = raw.dimT();

    // get in plane resolution & slice thickness
    if ( verbose )
      cout << "check that images have same x and y resolutions : " << flush;
    if ( raw.sizeX() != raw.sizeY() )
    {

      throw runtime_error( "T2+DW image does not have sizeX = sizeY" );

    }
    if ( verbose )
      cout << "done" << std::endl;

    float inPlaneResolution = raw.sizeX();


    // reading T2 indices
    if ( verbose )
      cout << "reading T2 indices description file name : " << flush;
    int numT2;
    ifstream is( descriptionName.c_str() );
    is >> numT2;
    vector< int > t2Indices( numT2 );
    for ( int t2 = 0; t2 < numT2; t2++ )
      is >> t2Indices[ t2 ];
    is.close();
    if ( verbose )
      cout << "done" << std::endl;

    if ( verbose )
    {

      cout << "T2 indices : " << flush;
      for ( int t2 = 0; t2 < numT2; t2++ )
        cout << t2Indices[ t2 ] << " ";
      cout << endl;

    }


    // need to backup (S,TO,T1)
    AimsData< float > S( numLocation, numT2 - 1 );
    AimsData< float > T0( numLocation, numT2 - 1 );
    AimsData< float > T1( numLocation, numT2 - 1 );


    // estimating correction parameters
    try
    {

      if ( verbose )
        cout << "processing echoplanar distortion correction : " << flush;

      // build multithreading context
      ContextEpi context( numT2,
                          size,
                          sizeDivider,
                          inPlaneResolution,
                          levels,
                          ( double )error,
                          deltaP,
                          S, T0, T1,
			  verbose );

      // choose pdf smoother
      if ( smoothingType == "deriche" )
        context.setDericheGaussianSmoothing( sigma );
      else
        context.setDiscreteGaussianSmoothing( ( int )sigma );

      short** t2Images = new short*[ numT2 ];
      for ( int l = 0; l < numLocation; l++ )
      {

        // get T2 images buffer
        for ( int t2 = 0; t2 < numT2; t2++ )
        {

          t2Images[ t2 ] = &raw( 0, 0, l, t2Indices[ t2 ] );

        }

        context.setLocation( t2Images, l );
        ThreadedLoop loop( &context, 1, 0, numT2 - 1, 1 );
        loop.launch();

      }

      delete [] t2Images;
      if ( verbose )
        cout << "done" << endl;

    }
    catch ( exception& )
    {

      throw runtime_error( "multithreaded correction failed" );

    }

    // resampling data
    if ( verbose )
        cout << "resampling data : " << flush;

    // allocate result and temporary images
    AimsData< short > reg( size, size, numLocation, numRank );
    reg.setHeader( raw.header()->cloneHeader() );
    reg.setSizeXYZT( raw );

    for ( int l = 0; l < numLocation; l++ )
    {

      linearInterpolation( &raw( 0, 0, l, t2Indices[ 0 ] ),
                           &reg( 0, 0, l, t2Indices[ 0 ] ),
                           size, inPlaneResolution,
                           AimsVector< float, 3 >( 1.0, 0.0, 0.0 ) );
      for ( int r = t2Indices[ 0 ]; r < t2Indices[ 1 ]; r++ )
      {

        linearInterpolation( &raw( 0, 0, l, r ),
                             &reg( 0, 0, l, r ),
                             size, inPlaneResolution,
                             AimsVector< float, 3 >( 1.0, 0.0, 0.0 ) );

      }

      for ( int t2 = 0; t2 < numT2 - 1; t2++ )
      {

        AimsVector< float, 3 > param( S( l, t2 ), T0( l, t2 ), T1( l, t2 ) );
        for ( int r = t2Indices[ t2 + 1 ];
              r < ( t2 == numT2 - 2 ? numRank : t2Indices[ t2 + 2 ] ); r++ )
        {

          linearInterpolation( &raw( 0, 0, l, r ),
                               &reg( 0, 0, l, r ),
                               size, inPlaneResolution, param );

        }

      }

    }

    if ( verbose )
      cout << "done" << endl;


    // storing results
    if ( verbose )
      cout << "saving corrected data (" << writer.fileName() << ") : "
           << flush;
    writer << reg;
    if ( verbose )
      cout << "done" << endl;

    if ( !parameterName.empty() )
    {

      cout << "saving corrected parameters (" << parameterName << ") : "
           << flush;
      std::ofstream os( parameterName.c_str() );
      for ( int l = 0; l < numLocation; l++ )
      {

        os << 1.0 << "\t" << 0.0 << "\t" << 0.0 << std::endl;
        for ( int t2 = 0; t2 < numT2 - 1; t2++ )
        {

           os << S( l, t2 ) << "\t"
              << T0( l, t2 ) << "\t"
              << T1( l, t2 ) << std::endl;

        }
        os << std::endl;

      }
      os.close();
      cout << "done" << endl;

    }

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
