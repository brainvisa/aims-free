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
#include <string>
#include <aims/data/data_g.h>
#include <aims/io/io_g.h>
#include <aims/io/writer.h> 
#include <aims/getopt/getopt2.h>
#include <aims/getopt/getoptProcess.h>
#include <aims/math/math_g.h>
#include <aims/utility/utility_g.h>
#include <aims/signalfilter/maxfilter.h>
#include <aims/signalfilter/minfilter.h>
#include <aims/signalfilter/medianfilter.h>
#include <aims/signalfilter/majorityfilter.h>

using namespace aims;
using namespace carto;
using namespace std;

int main( int argc, const char **argv )
{
  string  filein, fileout, motionfile, smoothingtype = "majority";
  int     dx = 0, dy = 0, dz = 0;

  AimsApplication app( argc, argv, "Applies a smoothing filter " 
                        "to a volume.");
  app.addOption( filein, "-i", "source volume" );
  app.addOption( fileout, "-o", "destination volume" );
  app.addOption( dx, "--dx", "x dimension of the filter to apply", true );
  app.addOption( dy, "--dy", "y dimension of the filter to apply", true );
  app.addOption( dz, "--dz", "z dimension of the filter to apply", true );
  app.addOption( smoothingtype, "-t",
                  "Smoothing filter type: med[ian], min[imum], "
                  "max[imum], maj[ority] (default = majority)",
                  true );
  app.alias( "--input", "-i" );
  app.alias( "--output", "-o" );
  app.alias( "--type", "-t" );
  try
  {
    app.initialize();
  }
  catch( user_interruption & )
  {
    return( EXIT_FAILURE );
  }
  catch( exception & e )
  {
    cerr << e.what() << endl;
    return( EXIT_FAILURE );
  }

  // Check z dimension
  dz = (dz > 1 ? dz : 1);
    
  // Lecture image
  AimsData<short> in, out;
  try
    {
      Reader<AimsData<short> > dataR( filein );
      dataR.read( in );
    }
  catch( exception & e )
    {
      cerr << e.what() << endl;
      exit( 1 );
    }

  if ( (smoothingtype == "median") || (smoothingtype == "med") )
  {
    MedianSmoothing< short > smoothing( dx, dy, dz );
    out = smoothing.doit( in );
  }
  else if ( (smoothingtype == "minimum") || (smoothingtype == "min") )
  {
    MinSmoothing< short > smoothing( dx, dy, dz );
    out = smoothing.doit( in );
  }
  else if ( (smoothingtype == "maximum") || (smoothingtype == "max") )
  {
    MaxSmoothing< short > smoothing( dx, dy, dz );
    out = smoothing.doit( in );
  }
  else
  {
    MajoritySmoothing< short > smoothing( dx, dy, dz );
    out = smoothing.doit( in );
  }

  // 
  //   Writing smoothed image
  //
  out.setHeader( in.header()->cloneHeader() );
  Writer< AimsData<short> > dataWtr( fileout );
  dataWtr << out;

  return EXIT_SUCCESS; 
}
 
