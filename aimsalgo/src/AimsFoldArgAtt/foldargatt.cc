/* Copyright (c) 1995-2007 CEA
 *
 *  This software and supporting documentation were developed by
 *      CEA/DSV/SHFJ
 *      4 place du General Leclerc
 *      91401 Orsay cedex
 *      France
 *
 * This software is governed by the CeCILL license version 2 under 
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the 
 * terms of the CeCILL license version 2 as circulated by CEA, CNRS
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
 * knowledge of the CeCILL license version 2 and that you accept its terms.
 */


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

      app.initialize();

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

      FoldGraphAttributes	fatt( skel, graph, motion.get(), inside, 
                                      outside, !nomesh );
      // this in case the graph topology has changed (folds splits...)
      fatt.rebuildCorticalRelations();
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


