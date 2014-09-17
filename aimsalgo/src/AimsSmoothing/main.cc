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
#include <aims/data/data_g.h>                                      // AimsData
#include <aims/io/io_g.h>                             // Reader Writer Process
#include <aims/getopt/getopt2.h>                            // AimsApplication
#include <aims/getopt/getoptProcess.h>     // AimsApplication: possible inputs
#include <aims/rgb/rgb.h>                                           // AimsRGB
#include <aims/signalfilter/filter_linear.h>               // LinFilterFactory
#include <aims/signalfilter/filter_nonlinear.h>         // NonLinFilterFactory
#include <aims/connectivity/structuring_element.h>             // ShapeFactory
//--- carto ------------------------------------------------------------------
#include <cartobase/object/object.h>
#include <cartobase/object/property.h>
//--- std --------------------------------------------------------------------
#include <cstdlib>
#include <string>
//----------------------------------------------------------------------------

using namespace aims::strel;
using namespace aims;
using namespace carto;
using namespace std;

template <typename T> bool doit( Process &, const string &, Finder & );

class FilterProcess : public Process
{
  public:
    string fileOut;
    string type;
    string shape;
    vector<double> amplitude;
    string unit;
    Object options;
    int32_t dx, dy, dz;
    string bv;

    FilterProcess():
      fileOut(""),
      type("majority"),
      shape("cube"),
      amplitude(3,1.),
      unit("v"),
      options(carto::none()),
      dx(1),
      dy(1),
      dz(1),
      bv("")
    {
      registerProcessType( "Volume", "S8",      &doit<int8_t> );
      registerProcessType( "Volume", "U8",      &doit<uint8_t> );
      registerProcessType( "Volume", "S16",     &doit<int16_t> );
      registerProcessType( "Volume", "U16",     &doit<uint16_t> );
      registerProcessType( "Volume", "S32",     &doit<int32_t> );
      registerProcessType( "Volume", "U32",     &doit<uint32_t> );
      registerProcessType( "Volume", "FLOAT",   &doit<float> );
      registerProcessType( "Volume", "DOUBLE",  &doit<double> );
      registerProcessType( "Volume", "RGB",     &doit<AimsRGB> );
      registerProcessType( "Volume", "RGBA",    &doit<AimsRGBA> );
    }
};

template <typename T> bool
doit( Process & p, const string & fname, Finder & )
{
  FilterProcess & proc = (FilterProcess &) p;

  // Read image
  AimsData<T> in, out;
  Reader< AimsData<T> > dataR( fname );
  cout << "Reading volume...";
  if( !dataR.read( in ) )
      return( false );
  cout << " done" << endl;

  // bg value
  T defaultval;
  carto::stringTo( proc.bv, defaultval );
  in.fillBorder( defaultval );

  // Compatibility with old parameters
  if( proc.dx > 1 || proc.dy > 1 || proc.dz > 1 ) {
    proc.amplitude[0] = ( proc.dx > 1 ? (double) proc.dx : 1. ) / 2.;
    proc.amplitude[1] = ( proc.dy > 1 ? (double) proc.dy : 1. ) / 2.;
    proc.amplitude[2] = ( proc.dz > 1 ? (double) proc.dz : 1. ) / 2.;
  }
  if( proc.unit[0] == 'm' ) {
    proc.amplitude[0] /= (double) in.sizeX();
    proc.amplitude[1] /= (double) in.sizeY();
    proc.amplitude[2] /= (double) in.sizeZ();
  }

  // Load StructuringElement
  StructuringElementRef shape( ShapeFactory::create( proc.shape, proc.amplitude, true ) );
  if( !shape ) {
    cerr << "Unknown shape " << proc.shape << endl;
    return false;
  }

  // Load Filter
  rc_ptr<ImageAlgorithmInterface<T> > algo(0);
  algo.reset( NonLinFilterFactory<T>::create( proc.type, *shape, proc.options ) );
  if( !algo )
    algo.reset( LinFilterFactory<T>::create( proc.type, *shape, proc.options ) );
  if( !algo ) {
    cerr << "Unknown filter " << proc.type << endl;
    return false;
  }

  // Do processing
  out = algo->execute(in);

  // File Writing
  cout << "Writing volume...";
  Writer<AimsData<T> > dataW( proc.fileOut );
  int result = dataW.write( out );
  cout << " done" << endl;
  return result;
}

int main( int argc, const char **argv )
{
  try
  {
    FilterProcess proc;
    ProcessInput  pi( proc );
    double  gabSigma = 1.0, gabTheta = 0.0, gabLambda = 1.0,
            gabPsi = 0.0, gabGamma = 1.0;
    bool    gabReal = true;

    AimsApplication app( argc, argv, "Image filtering\n"
      "This command allows one to apply a range of filtering "
      "algorithms. The user should choose a filter type, "
      "a structuring element (in non-linear cases), and eventually "
      "filter-specific options.\n" );
    app.addOption( pi, "-i", "source volume" );
    app.addOption( proc.fileOut, "-o", "destination volume" );
    app.addOption( proc.type, "-t", "Smoothing filter type: "
      "mea[n], med[ian], min[imum], max[imum], maj[ority], gab[or] "
      "(default = majority)",
      true );
    app.addOption( proc.shape, "-s", "Structuring element type [default:cube]", true );
    app.addOptionSeries( proc.amplitude, "-a", "Structuring element amplitude [default:1.]", 0, 3 );
    app.addOption( proc.unit, "-u", "Amplitude unit: v[oxel], m[m] [default:v]", true );
    app.addOption( gabSigma, "--gabSigma", "Gabor Filter: standard deviation of the gaussian function (mm)", true );
    app.addOption( gabTheta, "--gabTheta", "Gabor Filter: rotation of the referential (deg)", true );
    app.addOption( gabLambda, "--gabLambda", "Gabor Filter: wavelength of the sinusoidal function (mm)", true );
    app.addOption( gabPsi, "--gabPsi", "Gabor Filter: phase offset of the sinusoidal function (rad)", true );
    app.addOption( gabGamma, "--gabGamma", "Gabor Filter: aspect ratio of the referential", true );
    app.addOption( gabReal, "--gabReal", "Gabor Filter: if true returns the real part, else immaginary", true );
    app.addOption( proc.bv, "--bv", "Background value to use", true );
    app.addOption( proc.dx, "--dx", "x dimension of the filter to apply (voxels) (deprecated)", true );
    app.addOption( proc.dy, "--dy", "y dimension of the filter to apply (voxels) (deprecated)", true );
    app.addOption( proc.dz, "--dz", "z dimension of the filter to apply (voxels) (deprecated)", true );
    app.alias( "--input", "-i" );
    app.alias( "--output", "-o" );
    app.alias( "--type", "-t" );
    app.alias( "--amplitude", "-a" );
    app.alias( "--unit", "-u" );
    app.alias( "--strelement", "-s" );

    app.initialize();

    proc.options = Object::value( PropertySet() );
    if( proc.type == "gab" || proc.type == "gabor" )
    {
      proc.options->setProperty( "sigma", gabSigma );
      proc.options->setProperty( "theta", gabTheta );
      proc.options->setProperty( "lambda", gabLambda );
      proc.options->setProperty( "psi", gabPsi );
      proc.options->setProperty( "gamma", gabGamma );
      proc.options->setProperty( "real", gabReal );
    }
    if( proc.amplitude[0] < 0 )
      proc.amplitude[0] = 1.;
    if( proc.amplitude[1] < 0 )
      proc.amplitude[1] = proc.amplitude[0];
    if( proc.amplitude[2] < 0 )
      proc.amplitude[2] = proc.amplitude[1];

    if( !proc.execute( pi.filename ) )
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

