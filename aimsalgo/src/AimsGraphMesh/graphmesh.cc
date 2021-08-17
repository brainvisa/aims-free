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
#include <aims/io/aimsGraphR.h>
#include <aims/io/aimsGraphW.h>
#include <aims/graph/graphmanip.h>
#include <cartobase/config/paths.h>
#include <aims/mesh/mesher.h>
#include <aims/mesh/surfaceOperation.h>
#include <aims/bucket/bucketMap.h>
#include <aims/utility/converter_bucket.h>
#include <graph/graph/graph.h>
#include <graph/graph/greader.h>
#include <graph/graph/gwriter.h>
#include <cartobase/object/sreader.h>
#include <cartobase/object/syntax.h>
#include <cartobase/smart/rcptr.h>


using namespace aims;
using namespace carto;
using namespace std;


bool meshIt( carto::AttributedObject* ao, const vector<string> & meshatt, 
	     const vector<string> & tomesh, set<string> & syntaxes,
	     uint minFacetNumber)
{
  vector<string>::const_iterator		ib, eb = tomesh.end();
  rc_ptr<BucketMap<Void> >			bk;
  vector<pair<string, BucketMap<Void> *> >	bcks;
  Mesher					mesher;
  const float	deciMaxClearance = 5., deciMaxError = 0.5;

  mesher.setDecimation( 99, deciMaxClearance, deciMaxError, 120.0 );
  mesher.setMinFacetNumber( minFacetNumber );
  mesher.setSmoothing( Mesher::LOWPASS, 50, 0.4 );

  Point3d				bmax( 0, 0, 0 );
  BucketMap<Void>::Bucket::iterator	ibm, ebm;
  vector<pair<string, BucketMap<Void> *> >::iterator	ib2, eb2;
  unsigned				i, n = meshatt.size(), j;

  //	scan for buckets and search bounding box
  for( ib=tomesh.begin(), i=0; ib!=eb; ++ib, ++i )
    if( ao->getProperty( *ib, bk ) )
      if( !bk->begin()->second.empty() )
	{
	  //cout << "bucket " << *ib << " found\n";
	  if( i < n )
	    j = i;
	  else
	    j = 0;
	  bcks.push_back( pair<string, BucketMap<Void> *>( meshatt[j], 
							   bk.get() ) );
	  for( ibm=bk->begin()->second.begin(), ebm=bk->begin()->second.end(); 
	       ibm!=ebm; ++ibm )
	    {
	      const Point3d	&p = ibm->first;
	      //cout << p << endl;
	      if( bmax[0] < p[0] )
		bmax[0] = p[0];
	      if( bmax[1] < p[1] )
		bmax[1] = p[1];
	      if( bmax[2] < p[2] )
		bmax[2] = p[2];
	    }
	}
  if( bcks.empty() )
    return( false );

  syntaxes.insert( ao->getSyntax() );
  VolumeRef<short>	vol( bmax[0] + 1, bmax[1] + 1, bmax[2] + 1, 1, 1 );
  vol.fillBorder( -1 );
  vol.fill( 0 );
  RawConverter<BucketMap<Void>, VolumeRef<short> >	conv;
  vol.setVoxelSize( bcks[0].second->sizeX(), bcks[0].second->sizeY(),
                    bcks[0].second->sizeZ(), 1 );

  for( ib2=bcks.begin(), eb2=bcks.end(); ib2!=eb2; ++ib2 )
    conv.printToVolume( *ib2->second, vol );

  // OK, now mesh it
  map<size_t, list<AimsSurfaceTriangle> >	meshl;
  mesher.doit( vol, meshl );
  AimsSurfaceTriangle	*s = new AimsSurfaceTriangle;
  SurfaceManip::meshMerge( *s, meshl.begin()->second );
  ao->setProperty( meshatt[0], rc_ptr<AimsSurfaceTriangle>( s ) );
  return( true );
}


void meshThings( Graph & g, const vector<string> & meshatt, 
		 const vector<string> & tomesh, uint minFacetNumber )
{
  if( tomesh.empty() || meshatt.empty() )
    return;

  Graph::iterator		iv, ev = g.end();
  const set<Edge *>		& edges = g.edges();
  set<Edge *>::const_iterator	ie, ee = edges.end();
  set<string>			synts;
  set<string>::iterator		is, es = synts.end();

  for( iv=g.begin(); iv!=ev; ++iv )
    meshIt( *iv, meshatt, tomesh, synts, minFacetNumber );
  for( ie=edges.begin(); ie!=ee; ++ie )
    meshIt( *ie, meshatt, tomesh, synts, minFacetNumber );

  rc_ptr<map<string,map<string,GraphElementCode> > > 
    aot( new map<string,map<string,GraphElementCode> > );
  unsigned	im, em = meshatt.size();

  g.getProperty( "aims_objects_table", aot );

  for( im=0; im!=em; ++im )
    {
      GraphElementCode				gec;
      string					id = meshatt[im];
      if( id.substr( 0, 5 ) == "aims_" )
	id.erase( 0, 5 );
      gec.id = id;
      gec.attribute = meshatt[im];
      gec.objectType = "Mesh";
      gec.dataType = "VOID";
      gec.storageType = GraphElementCode::Global;
      gec.global_index_attribute = id + "_label";
      gec.local_file_attribute = id + "_filename";
      gec.global_filename = id + ".mesh";

      for( is=synts.begin(); is!=es; ++is )
	{
	  gec.syntax = *is;
	  (*aot)[ *is ][ id ] = gec;
	}
    }
  g.setProperty( "aims_objects_table", aot );
}


int main( int argc, const char **argv )
{
  string		basename;
  bool			global = false;
  bool			local = false;
  uint			minFacetNumber = 50 ;
  vector<string>	tomesh, meshatt;
  Reader<Graph>		grd;
  Writer<Graph>		gwr;

  AimsApplication	app( argc, argv, "Performs graph storage conversion "
			     "and sub-buckets meshing\nThis command is an "
			     "improved version of AimsGraphConvert");

  app.addOption( grd, "-i", "input data graph" );
  app.alias( "-input", "-i" );
  app.addOption( gwr, "-o", "output data graph (default: same as input)", 
		 true );
  app.alias( "-output", "-o" );
  app.addOption( basename, "-b", "directory where to put sub-data in "
		 "(default: <output>.data)", true );
  app.alias( "-basename", "-b" );
  app.addOption( global, "-g", "save sub-objects in 'global' mode "
		 "(1 file for all objects of same type)", true );
  app.alias( "-global", "-g" );
  app.addOption( local, "-l", "save sub-objects in 'local' mode "
		 "(each object with a specific file) (contradictory with -g)", 
		 true );
  app.alias( "-local", "-l" );
  app.addOptionSeries( meshatt, "-m", "mesh some objects into the given " 
		       "attribute\nNeeds at least one -mb option " 
		       "(default: aims_Tmtktri)" );
  app.alias( "-mesh", "-m" );
  app.addOptionSeries( tomesh, "-mb", "mesh buckets of given attribute "
		       "(experts manips). Needs also a -m option.\n"
		       "Specify several -mb <attrib> to mesh several buckets" 
		       "(default: all)" );
  app.alias( "-meshbucket", "-mb" );
  app.addOption( minFacetNumber, "-f", 
		 "minimum number of facets to mesh (default 50)", true );

  Graph		g( "ClusterArg" );
  SyntaxSet	syntax;

  try
    {
      app.initialize();

      if( gwr.fileName().empty() )
	gwr.setFileName( grd.fileName() );

      if( global && local )
	{
	  cerr << "cant't use both -g and -l options\n";
	  return EXIT_FAILURE;
	}

      if( ( !tomesh.empty() && meshatt.empty() )
	  || ( tomesh.empty() && !meshatt.empty() ) )
	{
	  cerr << "-m and -mb options need to be used together\n";
	  return EXIT_FAILURE;
	}

      // syntax
      SyntaxReader	sr( Paths::findResourceFile(
        "nomenclature/syntax/graph.stx", "aims" ) );
      sr >> syntax;

      // read
      //GraphReader	grd( filein, syntax );
      grd.read( g );
      //AimsGraphReader	agr( filein );
      //agr.readElements( g, true );

      string		base;

      if( !basename.empty() )
	base = basename;
      else
	{
	  base = gwr.fileName();
	  string::size_type	pos = base.rfind( '.' );
	  if( pos != string::npos )
	    base.erase( pos, base.length() - pos );
	  pos = base.rfind( '/' );
	  if( pos != string::npos )
	    base.erase( 0, pos + 1 );
	  base += ".data";
	}
      cout << "filename_base : " << base << endl;
      if( !base.empty() )
	g.setProperty( "filename_base", base );
      else if( g.hasProperty( "filename_base" ) )
	g.removeProperty( "filename_base" );

      // bucket/mesh attributes
      if( tomesh.empty() )
	{
	  bool needsatt = false;
	  if( meshatt.empty() || meshatt.size() != 1 )
	    {
	      //needsatt = true;
	      meshatt.clear();
	      // temp
	      meshatt.push_back( "aims_Tmtktri" );
	    }
	  rc_ptr<GraphElementTable>	get;
	  if( !g.getProperty( "aims_objects_table", get ) )
	    {
	      cerr << "Graph has no objects\n";
	      return EXIT_FAILURE;
	    }
	  GraphElementTable::const_iterator	iget, eget = get->end();
	  map<string,GraphElementCode>::const_iterator	igec, egec;
	  DataTypeCode<BucketMap<Void> >	dtc;

	  for( iget=get->begin(); iget!=eget; ++iget )
	    for( igec=iget->second.begin(), egec=iget->second.end(); 
		 igec!=egec; ++igec )
	      {
		const GraphElementCode	& gec = igec->second;
		if( gec.objectType == dtc.objectType() 
		    && gec.dataType == dtc.dataType() )
		  {
		    tomesh.push_back( gec.attribute );
		    if( needsatt )
		      meshatt.push_back( gec.attribute + "_mesh" );
		  }
	      }
	}

      // mesh conversions
      meshThings( g, meshatt, tomesh, minFacetNumber );

      // write
      // must use low-level writers to use specific modes
      AimsGraphWriter			agw( gwr.fileName() );
      AimsGraphWriter::SavingMode	sm = AimsGraphWriter::Keep;
      if( global )
	sm = AimsGraphWriter::Global;
      else if( local )
	sm = AimsGraphWriter::Local;

      agw.writeElements( g, sm, sm, false );
      GraphWriter	gw( gwr.fileName(), syntax );
      gw.write( g );

      return EXIT_SUCCESS;
    }
  catch( user_interruption &e )
    {
    }
  catch( exception & e )
    {
      cerr << e.what() << endl;
      return( 1 );
    }

  return EXIT_FAILURE;
}
