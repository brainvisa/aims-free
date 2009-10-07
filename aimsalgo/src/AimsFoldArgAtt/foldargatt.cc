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
#include <aims/getopt/getopt2.h>
#include <aims/data/data.h>
#include <aims/resampling/motion.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/io/motionR.h>
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
      Reader<AimsData<int16_t> >	skelr;
      Reader<Graph>			graphr;
      Writer<Graph>			graphw;
      string				motionfname, apcfilename;
      int16_t				inside = 0, outside = 11;
      bool				nomesh = false;
      string                            gver;
#ifdef _WIN32
      // on windows, disable threading because something is going wrong in it
      int  nthreads = 1;
#else
      int  nthreads = 0;
#endif

      AimsApplication	app( argc, argv, "Builds cortical folds graph " 
                             "attributes - replaces VipFoldArgAtt" );
      app.addOption( skelr, "-i", "skeleton image" );
      app.addOption( graphr, "-g", "fold graph" );
      app.addOption( graphw, "-o", "output fold graph [default: input]", 
                     true );
      app.addOption( motionfname, "-m", "transformation to Talairach " 
                     "[default: none]", true );
      app.addOption( nomesh, "-n", "don't generate meshes", true );
      app.alias( "--nomesh", "-n" );
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

      if( graphw.fileName().empty() )
        graphw.setFileName( graphr.fileName() );

      AimsData<int16_t>	skel;
      skelr.read( skel, 1 );

      Graph	graph;
      graphr.read( graph, -1 );

      auto_ptr<Motion>	motion;
      if( !motionfname.empty() )
        {
          motion.reset( new Motion );
          MotionReader	mr( motionfname );
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
      fatt.doAll();

      if( graphw.fileName() != graphr.fileName() )
        graph.setProperty( "filename_base", 
                           FileUtil::removeExtension
                           ( FileUtil::basename( graphw.fileName() ) ) 
                           + ".data" );
      graphw.write( graph, true );
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


