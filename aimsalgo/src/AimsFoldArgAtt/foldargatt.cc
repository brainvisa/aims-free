
#include <cstdlib>
#include <aims/getopt/getopt2.h>
#include <aims/data/data.h>
#include <aims/transformation/affinetransformation3d.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/io/apcreader.h>
#include <aims/foldgraph/foldgraphattributes.h>
#include <graph/graph/graph.h> 
#include <cartobase/stream/fileutil.h>
#include <cartobase/config/version.h>

using namespace aims;
using namespace carto;
using namespace std;


int main( int argc, const char** argv )
{
  try
    {
      Reader<AimsData<int16_t> > skelr;
      Reader<Graph> graphr;
      Writer<Graph> graphw;
      Mesher::SmoothingType smoothType = Mesher::LOWPASS;
      string smoothTypeStr = "lowpass";
      string motionfname, apcfilename;
      int16_t inside = 0, outside = 11;
      bool nomesh = false;
      string gver;
      
#ifdef _WIN32
      // on windows, disable threading because something is going wrong in it
      int  nthreads = 1;
#else
      int  nthreads = 0;
#endif

      AimsApplication app( argc, argv, "Builds cortical folds graph " 
                           "attributes - replaces VipFoldArgAtt" );
      app.addOption( skelr, "-i", "skeleton image" );
      app.addOption( graphr, "-g", "fold graph" );
      app.addOption( graphw, "-o", "output fold graph [default: input]", 
                     true );
      app.addOption( motionfname, "-m", "transformation to Talairach " 
                     "[default: none]", true );
      app.addOption( nomesh, "-n", "don't generate meshes", true );
      app.alias( "--nomesh", "-n" );
      app.addOption( smoothTypeStr, "--smoothType", "mesh smoothing alorithm's type "
                     "(if meshes are generated) : laplacian or lowpass  "
                     "[default=lowpass]", true );
      app.addOption( apcfilename, "--apc", "set AC/PC/IH points in graph "
                     "from a .APC file [default: don't set them]", true );
      app.addOption( inside, "-li", "'inside' label on skeleton image " 
                     "[default: 0]", true );
      app.addOption( outside, "-lo", "'outside' label on skeleton image " 
                     "[default: 11]", true );
      app.addOption( gver, "--graphversion",
                     "output graph version [default: "
                     + cartobaseShortVersion() + "]", true );
      app.addOption( nthreads, "--threads", string( "limit threads usage. " \
        "code: 0: one thread per CPU; 1: mono-threaded; n>0: use exactly n " \
        "threads; -n: use one thread per CPU, up to n max. Default: " ) 
        + toString( nthreads ) + ". Note: no effect on Windows, threading " \
        "is disabled", true );

      app.initialize();

#ifdef _WIN32
      nthreads = 1;
#endif

      if( smoothTypeStr == "laplacian" )
        smoothType = Mesher::LAPLACIAN;
      else if ( smoothTypeStr == "lowpass" )
        smoothType = Mesher::LOWPASS;
      else
      {
        cout << "This smoothing type doesn't exist, check if it is correctly written or look at the command's help." << endl;
        return EXIT_FAILURE;
      }
      
      if( graphw.fileName().empty() )
        graphw.setFileName( graphr.fileName() );

      AimsData<int16_t>	skel;
      skelr.read( skel, 1 );

      Graph	graph;
      graphr.read( graph, -1 );

      unique_ptr<AffineTransformation3d> motion;
      if( !motionfname.empty() )
        {
          motion.reset( new AffineTransformation3d );
          Reader<AffineTransformation3d> mr( motionfname );
          mr.read( *motion );
        }

      if( !apcfilename.empty() )
        {
          APCReader	apcr( apcfilename );
          Object	apc = apcr.read();
          vector<float>	x;
          vector<int>	y(3);
          // WARNING, they are in mm (but as int), why??
          if( apc->getProperty( "ACmm", x ) )
            {
              y[0] = (int) rint( x[0] );
              y[1] = (int) rint( x[1] );
              y[2] = (int) rint( x[2] );
            }
          else
            {
              apc->getProperty( "AC", x );
              y[0] = (int) rint( x[0] * skel.sizeX() );
              y[1] = (int) rint( x[1] * skel.sizeY() );
              y[2] = (int) rint( x[2] * skel.sizeZ() );
            }
          graph.setProperty( "anterior_commissure", y );
          if( apc->getProperty( "PCmm", x ) )
            {
              y[0] = (int) rint( x[0] );
              y[1] = (int) rint( x[1] );
              y[2] = (int) rint( x[2] );
            }
          else
            {
              apc->getProperty( "PC", x );
              y[0] = (int) rint( x[0] * skel.sizeX() );
              y[1] = (int) rint( x[1] * skel.sizeY() );
              y[2] = (int) rint( x[2] * skel.sizeZ() );
            }
          graph.setProperty( "posterior_commissure", y );
          if( apc->getProperty( "IHmm", x ) )
            {
              y[0] = (int) rint( x[0] );
              y[1] = (int) rint( x[1] );
              y[2] = (int) rint( x[2] );
            }
          else
            {
              apc->getProperty( "IH", x );
              y[0] = (int) rint( x[0] * skel.sizeX() );
              y[1] = (int) rint( x[1] * skel.sizeY() );
              y[2] = (int) rint( x[2] * skel.sizeZ() );
            }
          graph.setProperty( "interhemi_point", y );
        }

      vector<int> gversion;
      string::size_type l = 0, ls = 0;
      int x;
      while( ls < gver.length() )
      {
        l = gver.find( '.', ls );
        if( l == string::npos )
          l = gver.length();
        stringstream ss( gver.substr( ls, l - ls ) );
        ss >> x;
        gversion.push_back( x );
        ls = l + 1;
      };

      FoldGraphAttributes	fatt( skel, graph, motion.get(), inside, 
                                      outside, !nomesh, gversion );
      fatt.setMaxThreads( nthreads );
      if( smoothType == Mesher::LAPLACIAN )
      {
        fatt.mesher().setSmoothing( smoothType, 5, 0.4 );
        fatt.mesher().setDecimation( 100., 5., 3., 180.0);
      }
      else if ( smoothType == Mesher::LOWPASS )
      {
        fatt.mesher().setSmoothing( smoothType, 50, 0.4 );
        fatt.mesher().setDecimation( 100., 2., 3., 180.0);
      }
      fatt.doAll();

      if( graphw.fileName() != graphr.fileName() )
        graph.setProperty( "filename_base", 
                           FileUtil::removeExtension
                           ( FileUtil::basename( graphw.fileName() ) ) 
                           + ".data" );

      Object opts = Object::value( Dictionary() );
      opts->setProperty( "force_global", true );
      graphw.setOptions( opts );
      graphw.write( graph );
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


