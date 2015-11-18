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
