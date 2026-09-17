
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/data/data.h>
#include <aims/getopt/getopt2.h>
#include <cartobase/stream/fileutil.h>
#include <cartobase/stream/directory.h>

using namespace aims;
using namespace carto;
using namespace std;


int main( int argc, const char **argv )
{
  string filein;
  int first = -1, nbslice = -1, nbrep = -1;
  Writer<AimsData<short> > writer;

  AimsApplication app( argc, argv,
                       "Concatenates all Siemens NEMA EPI images to a 4D data "
                       "file" );
  app.addOption( filein, "-i", "input file - its directory is actually used" );
  app.addOption( writer, "-o", "output file" );
  app.addOption( first, "-f", "first image number in series" );
  app.addOption( nbslice, "-s", "number of slices" );
  app.addOption( nbrep, "-r", "number of repetitions" );
  app.alias( "--input", "-i" );
  app.alias( "--output", "-o" );
  app.alias( "--first", "-f" );
  app.alias( "--slices", "-s" );
  app.alias( "--repetitions", "-r" );

  try
  {
    app.initialize();

    if ( first < 0 || nbslice <= 0 || nbrep <= 0 )
    {
      cerr << "bad or missing first, slices or Repetitions arguments\n";
      return EXIT_FAILURE;
    }
    std::string filebasename( FileUtil::basename( filein ) );
    std::string directoryname( FileUtil::dirname( filein ) );

    Directory	dir( directoryname );
    set<string>	files = dir.files();
    set<string>::iterator	ifl, efl = files.end();
    map< int, string > listname;
    int nfin = first + nbslice * nbrep;

    for( ifl=files.begin(); ifl!=efl; ++ifl )
      {
        const string	& s = *ifl;
        string sno = s.substr( s.rfind( "_" ) + 1,
                              s.rfind( ".nema" ) - s.rfind( "_" ) );
        int no = atoi( sno.c_str() );
        if ( no >= first && no < nfin && ( ( no - first ) % nbslice ) == 0 )
          listname[ no ] = directoryname + FileUtil::separator() + s;
      }

    map< int, string >::const_iterator it = listname.begin();

    cout << "Repetition 1 : " << it->second << endl;
    AimsData< short > in;
    Reader< AimsData<short> > readfirst( it->second );
    readfirst >> in;
    ++it;

    int i, j, k, theRep = 0;
    int dx = in.dimX();
    int dy = in.dimY();
    AimsData< short > res( dx, dy, nbslice, nbrep );
    res.setSizeXYZT( in.sizeX(), in.sizeY(), in.sizeZ() );

    for ( k=0; k<nbslice; k++ )
      for ( j=0; j<dy; j++ )
        for ( i=0; i<dx; i++ )
          res( i, j, k, theRep ) = in( i, j, k );

    theRep++;

    do
      {
        cout << "Repetition " << theRep + 1 << " : " << it->second << endl;
        Reader< AimsData<short> > reader( it->second );
        reader >> in;
        ++it;

        for ( k=0; k<nbslice; k++ )
          for ( j=0; j<dy; j++ )
            for ( i=0; i<dx; i++ )
              res( i, j, k, theRep ) = in( i, j, k );

        theRep++;
      }
    while ( it != listname.end() );

    writer << res;

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

