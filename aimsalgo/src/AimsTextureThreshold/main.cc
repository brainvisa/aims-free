
#include <cstdlib>
#include <aims/io/io_g.h>
#include <aims/data/data_g.h>
#include <aims/getopt/getopt2.h>
#include <aims/utility/utility_g.h>
#include <assert.h>

using namespace aims;
using namespace carto;
using namespace std;


template<class T>
static bool doit( Process &, const string &, Finder & );


class Thresholder : public Process
{
public:
  Thresholder( threshold_t m, const string & fileout, float th1, float th2,  bool b );

private:
  template<class T>
  friend bool doit( Process &, const string &, Finder & );

  threshold_t	mode;
  string	fout;
  float		t1;
  float		t2;
  bool		bin;

};


Thresholder::Thresholder( threshold_t m, const string & fileout, float th1, float th2, bool b ) 
  : Process(), mode( m ), fout( fileout ), t1( th1 ), t2( th2 ), bin( b )
  {
    registerProcessType( "Texture", "S16", &doit<short> );
    registerProcessType( "Texture", "FLOAT", &doit<float> );
  }


template<class T> bool 
doit( Process & p, const string & fname, Finder & f )
{
  Thresholder		&tc = (Thresholder &) p;
  TimeTexture<T>		tex;
  Reader<TimeTexture<T> >	r( fname );
  string	format = f.format();
  cout << "reading " << fname << "...\n";
  if( !r.read( tex ) )
    return( false );
  cout << "reading done\n";

  cout << "thresholding..." << flush;

  AimsTexThreshold<T,short> thresh( tc.mode, (T) tc.t1, (T) tc.t2 );
  if( tc.bin )
    {
      Writer<TimeTexture<short> > writer( tc.fout );
      return( writer.write( thresh.bin( tex ) ) );
    }
  else
    {
      Writer<TimeTexture<T> > writer( tc.fout );
      return( writer.write( thresh( tex ) ) );
    }
  cout << "done\n";

  return (true);
}


int main( int argc, const char **argv )
{
  string filein, fileout, smode;
  float t1 = 0, t2 = 0;
  bool binary = false;
  threshold_t  mode;

  AimsApplication app( argc, argv,
    "Threshold on texture  (each time serie is thresholded)" );
  app.addOption( filein, "-i", "origin file" );
  app.alias( "--input", "-i" );
  app.addOption( fileout, "-o", "output file" );
  app.alias( "--output", "-o" );
  app.addOption( smode, "-m", "mode that can be:\n"
    "lt --> lower than\n"
    "le --> lower or equal to\n"
    "gt --> greater than\n"
    "ge --> greater or equal to\n"
    "eq --> equal to\n"
    "di --> differ\n"
    "be --> between\n"
    "ou --> outside" );
  app.alias( "--mode", "-m" );
  app.addOption( t1, "-t", "first threshold" );
  app.addOption( t2, "-u", "second threshold (if any)", true );
  app.addOption( binary, "-b", "returns a short binary texture "
    "(background : 0 ; object : 1)", true );
  app.alias( "--binary", "-b" );

  try
  {
    app.initialize();

    if      ( string( smode ) == "lt" ) mode = AIMS_LOWER_THAN;
    else if ( string( smode ) == "le" ) mode = AIMS_LOWER_OR_EQUAL_TO;
    else if ( string( smode ) == "gt" ) mode = AIMS_GREATER_THAN;
    else if ( string( smode ) == "ge" ) mode = AIMS_GREATER_OR_EQUAL_TO;
    else if ( string( smode ) == "eq" ) mode = AIMS_EQUAL_TO;
    else if ( string( smode ) == "di" ) mode = AIMS_DIFFER;
    else if ( string( smode ) == "be" ) mode = AIMS_BETWEEN;
    else if ( string( smode ) == "ou" ) mode = AIMS_OUTSIDE;
    else
      AimsError("AimsThreshold : bad mode" );

    Thresholder	proc( mode, fileout, t1, t2, binary );
    if( ! proc.execute( filein ) )
      throw logic_error( "Internal error: execute failed" );
    cout << "done\n";
  }
  catch( user_interruption & )
  {
  }
  catch( exception & e )
  {
    cerr << e.what() << endl;
    return EXIT_FAILURE;
  }

  return( EXIT_SUCCESS );
}
