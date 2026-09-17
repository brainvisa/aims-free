
#include <cstdlib>
#include <aims/io/io_g.h>
#include <aims/topology/topoClassifier.h>
#include <aims/getopt/getopt2.h>
#include <aims/getopt/getoptProcess.h>
#include <iostream>

using namespace aims;
using namespace carto;
using namespace std;


template< class T >
static bool doit( Process&, const string&, Finder& );


class Classifier : public Process
{
public:

  Classifier( const string& fileout = string() );
  string fout;

private:

  template< class T >
  friend bool doit( Process&, const string&, Finder& );
};


Classifier::Classifier( const string& fileout ) : Process(), fout( fileout )
{
  registerProcessType( "Volume", "S8", &doit< VolumeRef<int8_t> > );
  registerProcessType( "Volume", "U8", &doit< VolumeRef<uint8_t> > );
  registerProcessType( "Volume", "S16", &doit< VolumeRef<int16_t> > );
  registerProcessType( "Volume", "U16", &doit< VolumeRef<uint16_t> > );
  registerProcessType( "Volume", "S32", &doit< VolumeRef<int32_t> > );
  registerProcessType( "Volume", "U32", &doit< VolumeRef<uint32_t> > );
  registerProcessType( "Bucket", "S16", &doit< BucketMap<int16_t> > );
  registerProcessType( "Bucket", "VOID", &doit< BucketMap<Void> > );
}


template< class T >
bool doit( Process& p, const string& fname, Finder& f )
{
  Classifier& cl = (Classifier &)p;
  T data;
  Reader<T> r( fname );
  string format = f.format();

  cout << "Reading " << fname << "..." << endl;
  if ( !r.read( data, 0, &format ) )  return false;

  cout << "Classifying..." << endl;
  TopologicalClassifier<T> topoClass;
  Writer< typename TopologicalClassifier<T>::ResultType > writer( cl.fout );

  typename TopologicalClassifier<T>::ResultRcType out = topoClass.doit( data );
  return writer.write( topoClass.takeResult( out ) );
}


int main( int argc, const char** argv )
{
  string fileout;
  Classifier proc;
  ProcessInput  filein( proc );

  //
  // Getting options
  //
  AimsApplication app( argc, argv, "Topological classification of each point "
    "of a labeled image" );
  app.addOption( filein, "-i", "input volume or bucket" );
  app.addOption( fileout, "-o", "output volume or bucket" );
  app.alias( "--input", "-i" );
  app.alias( "--output", "-o" );

  try
  {
    app.initialize();
    proc.fout = fileout;
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
