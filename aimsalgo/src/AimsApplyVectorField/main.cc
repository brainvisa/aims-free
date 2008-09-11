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
#include <aims/data/data_g.h>
#include <aims/io/reader.h>
#include <aims/io/finder.h>
#include <aims/math/math_g.h>
#include <aims/vector/vector.h>
#include <aims/mesh/mesh_g.h>
#include <aims/def/path.h>
#include <aims/graph/graphmanip.h>
#include <graph/graph/graph.h>
#include <cartobase/object/attributed.h>
#include <cartobase/stream/directory.h>
#include <iomanip>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace aims;
using namespace carto;
using namespace std;

#define MESH_INPUT 0
#define ARG_INPUT 2

//
// transformation
//
Point3df transformation( const Point3df& point, 
			 const AimsData<float> & fieldx, 
			 const AimsData<float> & fieldy, 
			 const AimsData<float> & fieldz, 
			 const Point3df & targetvx )
{

  int x, y, z;
  x = (int)floor((point[0])/fieldx.sizeX()+0.5);
  y = (int)floor((point[1])/fieldx.sizeY()+0.5);
  z = (int)floor((point[2])/fieldx.sizeZ()+0.5);
  return Point3df( targetvx[0]*(point[0]/fieldx.sizeX() + fieldx(x,y,z)),
		   targetvx[1]*(point[1]/fieldx.sizeY() + fieldy(x,y,z)),
		   targetvx[2]*(point[2]/fieldx.sizeZ() + fieldz(x,y,z)));

}


void transformGraphObject( AttributedObject* v, rc_ptr<GraphElementTable> get, 
                           const AimsData<float> & fieldx, 
                           const AimsData<float> & fieldy, 
                           const AimsData<float> & fieldz, const Point3df &vs )
{
  GraphElementTable::const_iterator	iget;
  map<string,GraphElementCode>::const_iterator	igec, egec;
  rc_ptr<AimsSurfaceTriangle>			mesh;

  iget = get->find( v->getSyntax() );
  if( iget != get->end() )
    for( igec=iget->second.begin(), egec=iget->second.end(); 
         igec!=egec; ++igec )
      {
        const GraphElementCode	& gec = igec->second;
        if( gec.objectType 
            == DataTypeCode<AimsSurfaceTriangle>::objectType() 
            && gec.dataType 
            == DataTypeCode<AimsSurfaceTriangle>::dataType() 
            && v->getProperty( gec.attribute, mesh  ) )
          {
            AimsSurfaceTriangle::iterator	itime, tend;
            for( itime=mesh->begin(), tend = mesh->end(); 
                 itime!=tend; ++itime )
              {
                vector< Point3df > 
                  & vertex = (*itime).second.vertex();
                for( int k = 0; k < (int)vertex.size(); k++ )
                  vertex[ k ] = transformation( vertex[ k ], 
                                                fieldx, 
                                                fieldy, 
                                                fieldz, vs );
              }
            Mesher m;
            m.setSmoothing( 180., 1, 0.1, 0.1, 0.1 );
            m.smooth( *mesh );
          }
      }
}


//
// main
//
int main(int argc,const char **argv)
{
  string fileIn, fileOut, target, fileTransf;
  int inputFlag = ARG_INPUT;

  //
  // Parser of options
  //

  AimsApplication	app( argc, argv, "Apply a vector field to a simple mesh or to all meshes of a fold graph" );

  app.addOption( fileIn, "-i", "input mesh/graph data" );
  app.alias( "-input", "-i" );

  app.addOption( fileOut, "-o", "output mesh/graph (default: trans_input)", 
		 true );
  app.alias( "-output", "-o" );

  app.addOption( target, "-t", "target volume header" );
  app.alias( "-target", "-t" );

  app.addOption( fileTransf, "-v", "vector field prefix (will add .x, .y and .z)" );
  app.alias( "-vectorfield", "-v" );

  try
    {
      app.initialize();

      Finder	f;
      if( !f.check( fileIn ) )
        f.launchException();

      if( f.objectType() == "Graph" )
        inputFlag = ARG_INPUT;
      else
        inputFlag = MESH_INPUT;
  
      if( fileOut.empty() )
        fileOut = "trans_" + fileIn;

      //
      // read triangulation or arg
      //
      AimsSurfaceTriangle surface;
      Graph		foldgraph;

      if (inputFlag == MESH_INPUT)
        {
          Reader<AimsSurfaceTriangle>	surfaceR( fileIn );
          if( surfaceR.read( surface ) )
            {
              cout << "Input " << fileIn << " read as mesh\n";
            }
        }
      else
        {
          cout << "reading fold ARG  : " << flush;
          Reader<Graph>	gr( fileIn );
          gr.read( foldgraph );
          cout << "done\n";
        }

      //
      // loading transformation
      //

      cout << "reading target header  : " << flush;

      assert( f.check( target ) );
      const PythonHeader	*hdr 
        = dynamic_cast<const PythonHeader *>( f.header() );

      if( !hdr )
        {
          cerr << "target header of " << target << " is not valid\n";
          return( 1 );
        }

      vector<float>	vsv;
      Point3df	vs;
      assert( hdr->getProperty( "voxel_size", vsv ) );
      vs = Point3df( vsv[0], vsv[1], vsv[2] );
      cout << "target voxel size:" << vs[0] << " " << vs[1] << " " << vs[2] 
           << endl;


      cout << "reading vector field   : " << flush;
      string vfx, vfy, vfz;
      vfx = string(fileTransf) + ".x";
      vfy = string(fileTransf) + ".y";
      vfz = string(fileTransf) + ".z";

      AimsData<float> fieldx, fieldy, fieldz;
      try
        {
          Reader<AimsData<float> > xreader( vfx );
          xreader >> fieldx;
          Reader<AimsData<float> > yreader( vfy );
          yreader >> fieldy;
          Reader<AimsData<float> > zreader( vfz );
          zreader >> fieldz;
        }
      catch( exception & )
        {
          cerr << "vector field has to be 3 GIS FLOAT images" << endl;
          throw;
        }
      cout << "done" << endl;

      //
      // processing transformation
      //

      cout << "processing transformation : " << endl;
      if( inputFlag == MESH_INPUT )
        {
          AimsSurfaceTriangle::iterator	itime, tend = surface.end();

          for( itime=surface.begin(); itime!=tend; ++itime )
            {
              vector< Point3df >& vertex = (*itime).second.vertex();
              for ( int k = 0; k < (int)vertex.size(); k++ )
                {
                  vertex[ k ] = transformation( vertex[ k ], fieldx, fieldy, 
                                                fieldz, vs );
                }
            }
 
          Mesher m;
          m.setSmoothing( 180., 1, 0.1, 0.1, 0.1 );
          m.smooth( surface );

          cout << "done" << endl;

          //
          // save triangulation
          //
          cout << "saving triangulation    : " << flush;
          Writer<AimsSurfaceTriangle> triW( fileOut );
          triW.write( surface, 0 );
        }
      else
        {
          rc_ptr<GraphElementTable>	get;
          if( foldgraph.getProperty( "aims_objects_table", get ) )
            {
              Graph::iterator	iv, ev = foldgraph.end();
              GraphElementTable::const_iterator	iget, eget = get->end();

              cout << "transforming...."<<endl;
              for( iv=foldgraph.begin(); iv!=ev; ++iv )
                transformGraphObject( *iv, get, fieldx, fieldy, fieldz, vs );
              const set<Edge *>	& edges = foldgraph.edges();
              set<Edge *>::const_iterator	ie, ee = edges.end();
              for( ie=edges.begin(); ie!=ee; ++ie )
                transformGraphObject( *ie, get, fieldx, fieldy, fieldz, vs );

              cout << "done" << endl;

              foldgraph.setProperty( "filename_base", string( "*" ) );
              Writer<Graph>	gw( fileOut );
              gw.write( foldgraph );
            }
          else
            cout << "no Aims objects registered in graph" << endl;
        }
  
      cout << "done" << endl;
    }
  catch( user_interruption & )
    {
    }
  catch( exception & e )
    {
      cerr << e.what() << endl;
      return( EXIT_FAILURE );
    }

  return EXIT_SUCCESS;
}
