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
#include <aims/rgb/rgb.h>
#include <aims/signalfilter/maxfilter.h>
#include <aims/signalfilter/minfilter.h>
#include <aims/signalfilter/medianfilter.h>
#include <aims/signalfilter/majorityfilter.h>
#include <aims/signalfilter/meanfilter.h>

using namespace aims;
using namespace carto;
using namespace std;

class AimsFilter : public Process {
    public:
      string fileIn;
      string fileOut;
      int32_t dx, dy, dz;
      string type;
    
      AimsFilter();
};

template<class T> bool 
filter( Process & p, const string &, Finder & ) {
  AimsFilter & proc = (AimsFilter &)p;

  // Lecture image
  AimsData<T> in, out;
  Reader< AimsData<T> > dataR( proc.fileIn );
  dataR.read( in );

  // Check dimensions of filter
  proc.dx = (proc.dx > 1 ? proc.dx : 1 );
  proc.dy = (proc.dy > 1 ? proc.dy : 1 );
  proc.dz = (proc.dz > 1 ? proc.dz : 1 );

  if ( proc.dz > in.dimZ() )
  {
     cerr << "Image Z dimension can not be lower than filter Z dimension\n";
     return( 1 );
  }
  else if ( proc.dy > in.dimY() )
  {
     cerr << "Image Y dimension can not be lower than filter Y dimension\n";
     return( 1 );
  }
  else if ( proc.dx > in.dimX() )
  {
     cerr << "Image X dimension can not be lower than filter X dimension\n";
     return( 1 );
  }

  if ( (proc.type == "mean") 
       || (proc.type == "mea") )
  {
    MeanSmoothing< T > smoothing( proc.dx, proc.dy, proc.dz );
    out = smoothing.doit( in );
  }
  else if ( (proc.type == "median") 
            || (proc.type == "med") )
  {
    MedianSmoothing< T > smoothing( proc.dx, proc.dy, proc.dz );
    out = smoothing.doit( in );
  }
  else if ( (proc.type == "minimum") 
            || (proc.type == "min") )
  {
    MinSmoothing< T > smoothing( proc.dx, proc.dy, proc.dz );
    out = smoothing.doit( in );
  }
  else if ( (proc.type == "maximum") 
            || (proc.type == "max") )
  {
    MaxSmoothing< T > smoothing( proc.dx, proc.dy, proc.dz );
    out = smoothing.doit( in );
  }
  else
  {
    MajoritySmoothing< T > smoothing( proc.dx, proc.dy, proc.dz );
    out = smoothing.doit( in );
  }

  // 
  //   Writing smoothed image
  //
  out.setHeader( in.header()->cloneHeader() );
  Writer< AimsData<T> > dataW( proc.fileOut );
  dataW << out;

  return true;
}

AimsFilter::AimsFilter() 
  : Process(), dx(1), dy(1), dz(1)
               
{
  registerProcessType( "Volume", "S8", &filter<int8_t> );
  registerProcessType( "Volume", "U8", &filter<uint8_t> );
  registerProcessType( "Volume", "S16", &filter<int16_t> );
  registerProcessType( "Volume", "U16", &filter<uint16_t> );
  registerProcessType( "Volume", "S32", &filter<int32_t> );
  registerProcessType( "Volume", "U32", &filter<uint32_t> );
  registerProcessType( "Volume", "FLOAT", &filter<float> );
  registerProcessType( "Volume", "DOUBLE", &filter<double> );
  registerProcessType( "Volume", "RGB", &filter<AimsRGB> );
  registerProcessType( "Volume", "RGBA", &filter<AimsRGBA> );
}

int main( int argc, const char **argv )
{
  try {
    AimsFilter proc;
    string  filein, fileout, motionfile, type = "majority";

    AimsApplication app( argc, argv, "Applies a smoothing filter " 
                          "to a volume.");
    app.addOption( proc.fileIn, "-i", "source volume" );
    app.addOption( proc.fileOut, "-o", "destination volume" );
    app.addOption( proc.dx, "--dx", "x dimension of the filter to apply", true );
    app.addOption( proc.dy, "--dy", "y dimension of the filter to apply", true );
    app.addOption( proc.dz, "--dz", "z dimension of the filter to apply", true );
    app.addOption( proc.type, "-t",
                    "Smoothing filter type: mea[n], med[ian], min[imum], "
                    "max[imum], maj[ority] (default = majority)",
                    true );
    app.alias( "--input", "-i" );
    app.alias( "--output", "-o" );
    app.alias( "--type", "-t" );

    app.initialize();

    if( !proc.execute( proc.fileIn ) )
    {
        cerr << "Could not process\n";
        return( EXIT_FAILURE );
    }
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

  return EXIT_SUCCESS; 
}
 
