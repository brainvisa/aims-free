
#include <cartobase/config/verbose.h>
#include <aims/getopt/getopt2.h>
#include <aims/getopt/getoptProcess.h>
#include <aims/io/reader.h>
#include <aims/io/finder.h>
#include <aims/io/process.h>
#include <aims/roi/roiIterator.h>
#include <aims/utility/masscenter.h>

using namespace aims;
using namespace carto;
using namespace std;

template<typename T> 
static bool masscenter( Process &, const string &, Finder & );

class MassCenter : public Process
{
public:
  MassCenter( bool bin = false, const string & roi = "" );
  virtual ~MassCenter();

  bool   binary;
  string roi;
};

MassCenter::MassCenter( bool bin, const string & roi ) : binary( bin ), roi(roi)
{
  registerProcessType( "Volume", "S8", &masscenter<int8_t> );
  registerProcessType( "Volume", "U8", &masscenter<uint8_t> );
  registerProcessType( "Volume", "S16", &masscenter<int16_t> );
  registerProcessType( "Volume", "U16", &masscenter<int16_t> );
  registerProcessType( "Volume", "S32", &masscenter<int32_t> );
  registerProcessType( "Volume", "U32", &masscenter<int32_t> );
  registerProcessType( "Volume", "FLOAT", &masscenter<float> );
  registerProcessType( "Volume", "DOUBLE", &masscenter<double> );
}


MassCenter::~MassCenter()
{
}

template<typename T>
bool masscenter( Process & p, const string & filein, Finder & f )
{
  VolumeRef<T>   data;
  Reader<Volume<T> >  r( filein );
  string    format = f.format();
  r.setAllocatorContext( AllocatorContext( AllocatorStrategy::ReadOnly, 
                                           DataSource::none(), false, 0.01 ) );
  data.reset( r.read( 0, &format ) );
  
  MassCenter & m = ((MassCenter &) p);
  bool   bin = m.binary;
  string roi = m.roi;
  
  if (!roi.empty()) {
      rc_ptr<RoiIterator> roiIterator = getRoiIterator( roi );
      MassCenters<T>( data,
                      roiIterator,
                      bin).doit();
  }
  else {
    MassCenters<T>( data, bin).doit();
  }
  return( true );
}

int main( int argc, const char **argv )
{
  verbose = true;
  bool bin = false;
  string  roi = "";
  MassCenter proc;
  ProcessInput filein( proc );

  AimsApplication app( argc, argv,
                       "Computes position of the mass center of the image. "
                       "Also computes volume and mass of the data. \n\n"
                       "Change in AIMS 5.2: \n"
                       "Mass center position is now in mm, which is logical "
                       "and consistent with volumes which are in mm3. They "
                       "used to be in voxels, which was as a bug." );
  app.addOption( filein, "-i", "input data" );
  app.alias( "--input", "-i" );
  app.addOption( bin, "-b", "consider input image as binary data", true );
  app.alias( "--binary", "-b" );
  app.addOption( roi, "-r", "input ROI file (either ROI graph or image)", true );
  app.alias( "--roi", "-r" );
  app.alias( "-v", "--verbose" );

  try
    {
      app.initialize();
      proc.binary = bin;
      proc.roi = roi;
      if( !proc.execute( filein.filename ) )
        cout << "Couldn't process file - aborted\n";
    }
  catch( user_interruption &e )
    {
    }
  catch( exception & e )
    {
      cerr << e.what() << endl;
      return( 1 );
    }

  return EXIT_SUCCESS;
}


