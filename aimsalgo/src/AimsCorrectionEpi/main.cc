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



#include <cstdlib>
#include "contextEPI.h"
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

bool almostEqual( const float &a, const float &b )
{
  if ( a > b ) {
    if ( b == 0.0 ) return a < 0.01;
    return a / b < 1.01;
  } else {
    if ( a == 0.0 ) return b < 0.01;
    return b / a < 1.01;
  }
}


int main( int argc, const char* argv[] )
{

  int result = EXIT_SUCCESS;

  try
  {

    Reader< AimsData< short > > t2Reader;
    Reader< AimsData< short > > dwReader;
    Writer< AimsData< short > > dwWriter;
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
                 "Correction of echoplanar distortions in DW MR image\n"
                 "Base on a model of geometric distortion characterized by:\n"
                 "- a scaling S,\n"
                 "- a translation T0,\n"
                 "- a shearing T1\n" );
    application.addOption( t2Reader, "-t2", "T2 image" );
    application.addOption( dwReader, "-dw", "Diffusion-weighted image" );
    application.addOption( dwWriter, "-o",
                           "Corrected Diffusion-weighted image" );
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
                           "memory mapping activated (default=no)",
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

    // reading distorded image
    if ( verbose )
      cout << "reading T2 (" << t2Reader.fileName() << ") and DW ("
           << dwReader.fileName() << ") data : " << flush;
    AimsData<short> t2, dw;
    if ( memMap )
    {

      t2Reader.setAllocatorContext( AllocatorContext
                                    ( AllocatorStrategy::ReadOnly, 
                                      DataSource::none(), false, 0.1 ) );
      dwReader.setAllocatorContext( AllocatorContext
                                    ( AllocatorStrategy::ReadOnly ) );

    }
    t2Reader >> t2;
    dwReader >> dw;
    if ( verbose )
      cout << flush;

    // consistency of T2 and DW images
    if ( verbose )
      cout << "check consistency of T2 and DW images : " << flush;
    if ( ( t2.dimX() != dw.dimX() ) ||
         ( t2.dimY() != dw.dimY() ) ||
         ( t2.dimZ() != dw.dimZ() ) ||
         !almostEqual( t2.sizeX(), dw.sizeX() ) ||
         !almostEqual( t2.sizeY(), dw.sizeY() ) ||
         !almostEqual( t2.sizeZ(), dw.sizeZ() ) )
    {

      throw runtime_error( "T2 and DW image are not consistent" );

    }
    if ( verbose )
      cout << endl;

    if ( verbose )
      cout << "check that images are squared : " << flush;
    if ( t2.dimX() != t2.dimY() )
    {

      throw runtime_error( "T2 and DW image are not squared" );

    }
    if ( verbose )
      cout << endl;

    int size = t2.dimX();
    int numLocation = t2.dimZ();
    int nOrientation = dw.dimT();

    // get in plane resolution & slice thickness
    if ( verbose )
      cout << "check that images have same x and y resolutions : " << flush;
    if ( !almostEqual( t2.sizeX(), t2.sizeY() ) )
    {

      throw runtime_error( "T2 and DW image do not have sizeX = sizeY" );

    }
    if ( verbose )
      cout << endl;

    float inPlaneResolution = t2.sizeX();

    // choice of allocator
    Allocator al;
    if ( memMap )
      al = Allocator( &MappingCopyAllocator::singleton() );
    // allocate result and temporary images
    AimsData< short > reg( size, size, numLocation, nOrientation, 0, al );
    reg.setHeader( dw.header()->cloneHeader() );
    reg.setSizeXYZT( dw );

    // need to backup (S,TO,T1)
    AimsData< float > S( numLocation, nOrientation );
    AimsData< float > T0( numLocation, nOrientation );
    AimsData< float > T1( numLocation, nOrientation );

    try
    {

      if ( verbose )
        cout << "processing echoplanar distortion correction : " << flush;


      // build multithreading context
      ContextEpi context( nOrientation,
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

      short** dwImages = new short*[ nOrientation ];
      short** outImages = new short*[ nOrientation ];


      for ( int l = 0; l < numLocation; l++ )
      {

        // get T2 image buffer, DW image buffers and output image buffers
        short* t2Image = &t2( 0, 0, l );
        for ( int o = 0; o < nOrientation; o++ )
        {

          dwImages[ o ] = &dw( 0, 0, l, o );
          outImages[ o ] = &reg( 0, 0, l, o );

        }

        context.setLocation( t2Image, dwImages, outImages, l );
        ThreadedLoop loop( &context, 1, 0, nOrientation, 1 );
        loop.launch();

      }

      

      delete [] outImages;
      delete [] dwImages;
      if ( verbose )
        cout << "done" << flush;

    }
    catch ( exception& )
    {

      throw runtime_error( "multithreaded correction failed" );

    }

    // storing results
    if ( verbose )
      cout << "saving corrected data (" << dwWriter.fileName() << ") : "
           << flush;
    dwWriter << reg;
    if ( verbose )
      cout << "done" << flush;

    if ( !parameterName.empty() )
    {

      cout << "saving corrected parameters (" << parameterName << ") : "
           << flush;
      std::ofstream os( parameterName.c_str() );
      for ( int l = 0; l < numLocation; l++ )
      {

        for ( int o = 0; o < nOrientation; o++ )
        {

           os << S( l, o ) << "\t"
              << T0( l, o ) << "\t"
              << T1( l, o ) << std::endl;

        }
        os << std::endl;

      }
      os.close();    
      cout << "done" << flush;

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
