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
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/graph/graphmanip.h>
#include <aims/distancemap/voronoi.h>
#include <aims/resampling/motion.h>
#include <graph/graph/graph.h>

using namespace aims;
using namespace carto;
using namespace std;

int main( int argc, const char** argv )
{
  try
    {
      AimsApplication	app( argc, argv, 
			     "Transfers labelling from a buckets graph to " 
                             "another graph.\n" 
                             "Graph structures don't need to match, transfer " 
                             "is based on a Voronoi diagram" );
      Reader<Graph>			rg1, rg2;
      Writer<Graph>			wg;
      Writer<AimsData<int16_t> >	wv;
      string				labelsfile;
      string				labelatt = "name";
      app.addOption( rg1, "-i", "input graph to set labels into" );
      app.alias( "--input1", "-i" );
      app.addOption( rg2, "-j", "input labelled graph to take labels from" );
      app.alias( "--input2", "-j" );
      app.addOption( wg, "-o", "output labelled graph [default=input]", 
                     true );
      app.alias( "--output", "-o" );
      app.addOption( labelatt, "-l", "label attribute [default=name]", true );
      app.alias( "--label", "-l" );
      app.addOption( wv, "-v", "output voronoi diagram (in input2 space) "
                     "[default=not saved]", true );
      app.alias( "--voronoi", "-v" );
      app.addOption( labelsfile, "-m", 
                     "output labels map file [default=not saved]", true );
      app.alias( "--map", "-m" );
      app.initialize();

      cout << "reading " << rg1.fileName() << endl;
      Graph			*g = rg1.read();
      cout << rg1.fileName() << " read\n";
      cout << "reading " << rg2.fileName() << endl;
      Graph			*h = rg2.read();
      cout << rg2.fileName() << " read\n";

      // retreive underlying volume dimensions
      vector<int>		bbmaxg, bbmaxh;
      vector<float>		vsg, vsh;
      if( !g->getProperty( "boundingbox_max", bbmaxg ) )
        {
          cerr << "no volmume bounding box in graph " << rg1.fileName() 
               << endl;
          return EXIT_FAILURE;
        }
      if( !g->getProperty( "voxel_size", vsg ) )
        {
          cerr << "no voxel size in graph " << rg1.fileName() << endl;
          return EXIT_FAILURE;
        }
      if( !h->getProperty( "boundingbox_max", bbmaxh ) )
        {
          cerr << "no volmume bounding box in graph " << rg2.fileName() 
               << endl;
          return EXIT_FAILURE;
        }
      if( !h->getProperty( "voxel_size", vsh ) )
        {
          cerr << "no voxel size in graph " << rg2.fileName() << endl;
          return EXIT_FAILURE;
        }

      // handle transformations
      Motion  gToTal = GraphManip::talairach( *g );
      Motion  hToTal = GraphManip::talairach( *h );
      Motion vsgm;
      vsgm.rotation()(0,0) = vsg[0];
      vsgm.rotation()(1,1) = vsg[1];
      vsgm.rotation()(2,2) = vsg[2];
      Motion vshm;
      vshm.rotation()(0,0) = vsh[0];
      vshm.rotation()(1,1) = vsh[1];
      vshm.rotation()(2,2) = vsh[2];
      Motion gToh = vshm.inverse() * hToTal.inverse() * gToTal * vsgm;
      // gToh goes from g to h in voxels

      Point3df bbmingm( 0, 0, 0 ), bbmaxgm( bbmaxg[0], bbmaxg[1], bbmaxg[2] );
      Point3df bbminht, bbmaxht;
      transformBoundingBox( gToh, bbmingm, bbmaxgm, bbminht, bbmaxht );

      Point3d	dims( bbmaxh[0]+1, bbmaxh[1]+1, bbmaxh[2]+1 );
      if( dims[0] < bbmaxht[0]+1 )
        dims[0] = bbmaxht[0]+1;
      if( dims[1] < bbmaxht[1]+1 )
        dims[1] = bbmaxht[1]+1;
      if( dims[2] < bbmaxht[2]+1 )
        dims[2] = bbmaxht[2]+1;

      Point3df	vs( vsh[0], vsh[1], vsh[2] );

      /*
      if( dims[0] * vs[0] < (bbmaxg[0]+1) * vsg[0] )
        dims[0] = (int) ceil( (bbmaxg[0]+1) * vsg[0] / vs[0] );
      if( dims[1] * vs[1] < (bbmaxg[1]+1) * vsg[1] )
        dims[1] = (int) ceil( (bbmaxg[1]+1) * vsg[1] / vs[1] );
      if( dims[2] * vs[2] < (bbmaxg[2]+1) * vsg[2] )
        dims[2] = (int) ceil( (bbmaxg[2]+1) * vsg[2] / vs[2] );
      */

      // voronoi in h space
      cout << "volume dim: " << dims << endl;

      AimsData<short>	voro( dims[0], dims[1], dims[2] );
      voro.setSizeXYZT( vs[0], vs[1], vs[2] );
      voro = -10;

      Graph::iterator			ig, eg = g->end();
      Graph::const_iterator		ih, eh = h->end();
      map<string, int>			labels;
      map<int, string>			rlabels;
      map<string, int>::iterator	il, el = labels.end();
      int				current = 0, lmax = 0;
      string				l;
      rc_ptr<GraphElementTable>		get;
      SyntaxedInterface			*si;

      if( !h->getProperty( "aims_objects_table", get ) )
        {
          cerr << "no buckets in graph " << rg2.fileName() << endl;
          return EXIT_FAILURE;
        }

      GraphElementTable::const_iterator	iget, eget = get->end();
      map<string, GraphElementCode>::const_iterator	ic, ec;
      rc_ptr<BucketMap<Void> >				bck;
      BucketMap<Void>::Bucket::const_iterator		ib, eb;
      bool						bfound;

      // project h graph buckets in the volume for voronoi

      cout << "scanning " << rg2.fileName() << ", nodes: " << h->order() 
           << endl;
      for( ih=h->begin(); ih!=eh; ++ih )
        if( (*ih)->getProperty( labelatt, l ) )
          {
            si = (*ih)->getInterface<SyntaxedInterface>();
            if( si )
              {
                iget = get->find( si->getSyntax() );
                if( iget != eget )
                  {
                    il = labels.find( l );
                    if( il == labels.end() )
                      {
                        labels[ l ] = lmax;
                        rlabels[ lmax ] = l;
                        current = lmax;
                        ++lmax;
                      }
                    else
                      current = il->second;
                    bfound = false;
                    for( ic=iget->second.begin(), ec=iget->second.end(); 
                         ic != ec; ++ic )
                      {
                        const GraphElementCode	& gec = ic->second;
                        if( gec.objectType == "Bucket" )
                          {
                            if( (*ih)->getProperty( gec.attribute, bck ) )
                              {
                                for( ib=bck->begin()->second.begin(), 
                                       eb=bck->begin()->second.end(); 
                                     ib!=eb; ++ib )
                                  voro( ib->first ) = current;
                                bfound = true;
                              }
                          }
                        else if( gec.objectType == "Volume" 
                                 && gec.dataType == "S16" )
                          {
                            cout << "warning: labels image not implemented\n";
                          }
                      }
                    if( !bfound )
                      cout << "warning: node " << si->getSyntax() 
                           << " of label " << l << " has no bucket" << endl;
                  }
              }
          }
      cout << lmax << " labels in map" << endl;

      // voronoi
      cout << "computing voronoi map..." << endl;
      voro = AimsVoronoiFrontPropagation( voro, -10, -11, 3, 3, 3, 50 );

      if( !wv.fileName().empty() )
        wv.write( voro );
      if( !labelsfile.empty() )
        {
          ofstream	f( labelsfile.c_str() );
          map<int,string>::const_iterator	i, e = rlabels.end();
          for( i=rlabels.begin(); i!=e; ++i )
            f << i->first << "\t" << i->second << endl;
        }

      // compare voronoi to g graph buckets

      cout << "setting labels in graph " << rg1.fileName() << endl;
      Point3d p;
      if( !g->getProperty( "aims_objects_table", get ) )
        {
          cerr << "no buckets in graph " << rg1.fileName() << endl;
          return EXIT_FAILURE;
        }

      for( ig=g->begin(); ig!=eg; ++ig )
      {
        (*ig)->getProperty( labelatt, l );
        si = (*ig)->getInterface<SyntaxedInterface>();
        if( si )
          {
            iget = get->find( si->getSyntax() );
            if( iget != eget )
              {
                il = labels.find( l );
                if( il == labels.end() )
                {
                  cerr << "warning: label " << l << " present in graph "
                        << rg1.fileName() << " but not in "
                        << rg2.fileName() << endl;
                  current = 0;
                }
                else
                  current = il->second;
                map<int, int>	count;
                for( ic=iget->second.begin(), ec=iget->second.end();
                      ic != ec; ++ic )
                  {
                    const GraphElementCode	& gec = ic->second;
                    if( gec.objectType == "Bucket" )
                      {
                        if( (*ig)->getProperty( gec.attribute, bck ) )
                          {
                            for( ib=bck->begin()->second.begin(),
                                    eb=bck->begin()->second.end();
                                  ib!=eb; ++ib )
                              {
                                p = gToh.transform( ib->first );
                                if( p[0] >= 0 && p[1] >= 0 && p[2] >= 0 )
                                  ++count[ voro( p ) ];
                                // else: point outside voronoi

                                /*
                                p[0] = (int) rint( ib->first[0]
                                                    * vsg[0] / vs[0] );
                                p[1] = (int) rint( ib->first[1]
                                                    * vsg[1] / vs[1] );
                                p[2] = (int) rint( ib->first[2]
                                                    * vsg[2] / vs[2] );
                                ++count[ voro( p ) ];
                                */
                              }
                          }
                      }
                    else if( gec.objectType == "Volume"
                              && gec.dataType == "S16" )
                      {
                        cout << "warning: labels image not "
                              << "implemented\n";
                      }
                  }
                if( count.empty() )
                  cerr << "warning: a node " << si->getSyntax()
                        << " of label " << l << " has no bucket\n";
                else
                  {
                    if( count.size() == 1 )
                      current = count.begin()->first;
                    else
                      {
                        int	tot = 0, c = 0;
                        current = -1;
                        map<int, int>::const_iterator
                          ix, ex = count.end();
                        for( ix=count.begin(); ix!=ex; ++ix )
                          {
                            tot += ix->second;
                            if( ix->second > c )
                              {
                                current = ix->first;
                                c = ix->second;
                              }
                          }
                        if( float(c) / tot < 1 )
                          cerr << "a node of label " << l
                                << " is only "
                                << int( float(c) * 100 / tot )
                                << "% inside label "
                                << rlabels[ current ]
                                << " of graph " << rg2.fileName()
                                << endl;
                      }
                    if( rlabels.find( current ) == rlabels.end() )
                      cout << "label ID " << current << " not found\n";
                    else
                      (*ig)->setProperty( labelatt,
                                          rlabels[ current ] );
                  }
              }
          }
      }

      g->setProperty( "filename_base", string( "*" ) );

      if( wg.fileName().empty() )
        wg.setFileName( rg1.fileName() );
      cout << "writing graph " << wg.fileName() << endl;
      wg.write( *g );
      cout << "done." << endl;
      delete g;
      delete h;
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


