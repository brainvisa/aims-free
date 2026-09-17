
#include <cstdlib>
#include <aims/data/data_g.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h> 
#include <aims/getopt/getoptProcess.h>
#include <aims/math/math_g.h>
#include <aims/utility/utility_g.h>
#include <aims/signalfilter/medianfilter.h>
#include <aims/signalfilter/filteringfunction_element_d.h>

using namespace aims;
using namespace carto;
using namespace std;


class Algo : public Process
{
public:
  Algo();

  template<typename T>
  static bool doit( Process &, const string &, Finder & );

  string        fileout;
  Point3d t;
};


Algo::Algo()
  : Process(), t( 3, 3, 3 )
{
  registerProcessType( "Volume", "S8", &doit<int8_t> );
  registerProcessType( "Volume", "U8", &doit<uint8_t> );
  registerProcessType( "Volume", "S16", &doit<int16_t> );
  registerProcessType( "Volume", "U16", &doit<uint16_t> );
  registerProcessType( "Volume", "S32", &doit<int32_t> );
  registerProcessType( "Volume", "U32", &doit<uint32_t> );
  registerProcessType( "Volume", "FLOAT", &doit<float> );
  registerProcessType( "Volume", "DOUBLE", &doit<double> );
  registerProcessType( "Volume", "RGB", &doit<AimsRGB> );
  registerProcessType( "Volume", "RGBA", &doit<AimsRGBA> );
}


template<typename T> bool
Algo::doit( Process & p, const string & fname, Finder & /*f*/ )
{
  Algo                & mp = (Algo &) p;
  Reader<AimsData<T> > reader( fname );
  AimsData<T> in, out;
  reader.read( in );

  MedianSmoothing<T> ms( mp.t[0], mp.t[1], (in.dimZ() > 1 ? mp.t[2] : 1) );
  out = ms.doit( in );

  out.setHeader( in.header()->cloneHeader() );
  Writer<AimsData<T> > writer( mp.fileout );
  writer.write( out );

  return true;
}


int main( int argc, const char **argv )
{
  Algo proc;
  ProcessInput filein( proc );
  string fileout;

  AimsApplication       app( argc, argv, "Median filter smoothing" );

  app.addOption( filein, "-i", "source volume" );
  app.addOption( fileout, "-o", "destination volume" );
  app.addOption( proc.t[0], "-x", "X size of the filter mask [default=3]",
                 true );
  app.addOption( proc.t[1], "-y", "Y size of the filter mask [default=3]",
                 true );
  app.addOption( proc.t[2], "-z", "Z size of the filter mask [default=3]",
                 true );

  app.alias( "--input", "-i" );
  app.alias( "--output", "-o" );
  app.alias( "--dx", "-x" );
  app.alias( "--dy", "-y" );
  app.alias( "--dz", "-z" );

  try
  {
    app.initialize();

    proc.fileout = fileout;
    proc.execute( filein.filename );

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

