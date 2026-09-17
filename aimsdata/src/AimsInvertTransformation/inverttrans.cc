#include <aims/getopt/getopt2.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/transformation/affinetransformation3d.h>
#include <cartobase/stream/fileutil.h>

using namespace aims;
using namespace carto;
using namespace std;


int main( int argc, const char** argv )
{
  try
    {
      string	r;
      string	w;
      AimsApplication	app( argc, argv, "Inverts an Aims/anatomist "
			     "transformation" );
      app.addOption( r, "-i", "input transformation" );
      app.addOption( w, "-o", "output (inverted) transformation "
		     "(default: <input>_inverted.trm)", true );
      app.initialize();

      AffineTransformation3d m;
      Reader<AffineTransformation3d> mr( r );
      mr.read( m );
      m = *m.inverse();
      if( w.empty() )
      {
        w = FileUtil::removeExtension( r ) + "_inverted.trm";
      }
      Writer<AffineTransformation3d> mw( w );
      mw.write( m );
    }
  catch( user_interruption & )
    {
    }
  catch( exception & e )
    {
      cerr << e.what() << endl;
    }
}

