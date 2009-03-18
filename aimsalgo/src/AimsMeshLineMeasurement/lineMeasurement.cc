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


#include <cstdlib>
#include <aims/distancemap/meshdistance.h>
#include <aims/getopt/getopt2.h>
#include <aims/mesh/texture.h>
#include <cartobase/config/verbose.h>
#include <cfloat>

using namespace aims;
using namespace aims::meshdistance;
using namespace carto;
using namespace std;


unsigned shortest( Texture1d & otex, unsigned & ip1, unsigned & ip2, 
                   AimsSurfaceTriangle & mesh, const Point3df & point1, 
                   const Point3df & point2, float ratio )
{
  Texture<short>		tx;
  Texture<float>		& otx = otex[0], & dtx = otex[1], 
    & dtx2 = otex[2];
  unsigned			i, n = mesh.vertex().size();
  float			d1, d2, dmin1 = FLT_MAX, dmin2 = FLT_MAX;
  const vector<Point3df>	& vert = mesh.vertex();

  ip1 = 0;
  ip2 = 0;
  tx.reserve( n );
  otx.reserve( n );
  if( verbose )
    cout << "vertices: " << n << endl;

  for( i=0; i<n; ++i )
    {
      tx.push_back( 0 );
      otx.push_back( -1 );
      d1 = (vert[i] - point1).norm();
      if( d1 < dmin1 )
        {
          dmin1 = d1;
          ip1 = i;
        }
      d2 = (vert[i] - point2).norm();
      if( d2 < dmin2 )
        {
          dmin2 = d2;
          ip2 = i;
        }
    }

  // cout << "vertex 1: " << ip1 << " (dist: " << dmin1 << ")" << endl;
  // cout << "vertex 2: " << ip2 << " (dist: " << dmin2 << ")" << endl;

  if( verbose )
    cout << "computing distance maps..." << endl;
  tx.item( ip1 ) = 1;
  otex[1] = MeshDistance( mesh[0], tx, true );
  tx.item( ip1 ) = 0;
  tx.item( ip2 ) = 1;
  otex[2] = MeshDistance( mesh[0], tx, true );

  if( verbose )
    cout << "sorting neighbours..." << endl;
  const vector<AimsVector<uint,3> >	& poly = mesh.polygon();
  unsigned					p = poly.size();
  vector<set<unsigned> >			neigh( n );
  if( verbose )
    cout << "polygons: " << p << endl;
  for( i=0; i<p; ++i )
    {
      const AimsVector<uint,3>	& po = poly[i];
      set<unsigned>			& n1 = neigh[po[0]];
      set<unsigned>			& n2 = neigh[po[1]];
      set<unsigned>			& n3 = neigh[po[2]];
      n1.insert( po[1] );
      n1.insert( po[2] );
      n2.insert( po[0] );
      n2.insert( po[2] );
      n3.insert( po[0] );
      n3.insert( po[1] );
    }

  if( verbose )
    cout << "tracking shortest line..." << endl;
  set<unsigned>::iterator	in, en;
  bool				error = false;
  float				d, dmin, dmint = FLT_MAX, dtarget;
  set<unsigned>			done;
  set<unsigned>::iterator	notdone = done.end();
  unsigned			it = 0;

  i = ip1;
  otx.item(i) = dtx.item(i);
  dtarget = dtx[ ip2 ] * ratio;

  while( !error && i != ip2 )
    {
      if( verbose > 1 )
        cout << i << " " << flush;
      done.insert( i );
      set<unsigned>	& nh = neigh[i];
      dmin = FLT_MAX;
      for( in=nh.begin(), en=nh.end(); in!=en; ++in )
        if( done.find( *in ) == notdone )
          {
            if( *in == ip2 )
              {
                i = ip2;
                dmin = 0;
                break;
              }
            done.insert( *in );
            d = dtx.item( *in ) + dtx2.item( *in );
            if( d < dmin )
              {
                dmin = d;
                i = *in;
                if( verbose > 2 )
                  cout << i << "? " << flush;
              }
          }
      if( dmin == FLT_MAX )
        {
          cerr << "no path" << endl;
          error = true;
        }
      otx.item(i) = dtx.item(i);
      if( fabs( otx[i] - dtarget ) < dmint )
        {
          dmint = fabs( otx[i] - dtarget );
          it = i;
        }
    }
  if( verbose )
    cout << endl;

  return it;
}


unsigned planedoit(  Texture1d & otex, unsigned & ip1, unsigned & ip2, 
                     AimsSurfaceTriangle & mesh, const Point3df & point1, 
                     const Point3df & point2, float ratio, 
                     const Point4df & plane, const Point4df & plane2 )
{
  // normalize plane equation
  Point3df	pl( plane[0], plane[1], plane[2] );
  float		dof = plane[3] / pl.norm();
  pl.normalize();

  // track plane intersection
  const vector<AimsVector<uint, 3> >	& poly = mesh.polygon();
  const vector<Point3df>		& vert = mesh.vertex();
  unsigned				i, n = poly.size(), nv = vert.size();
  Texture<short>			t1;
  Texture<float>			& otx = otex[0], & dtx = otex[1];
  float					dmin1 = FLT_MAX, dmin2 = FLT_MAX;
  float					d1, d2;
  Point3df				p1, p2;

  // project points to plane
  p1 = point1 - ( point1.dot( pl ) + dof ) * pl;
  p2 = point2 - ( point2.dot( pl ) + dof ) * pl;

  if( verbose )
    cout << "vertices: " << n << endl;

  ip1 = 0;
  ip2 = 0;
  t1.reserve( nv );
  otx.reserve( nv );
  for( i=0; i<nv; ++i )
    {
      t1.push_back( 0 );
      otx.push_back( -1 );
      d1 = (vert[i] - p1).norm();
      if( d1 < dmin1 )
        {
          dmin1 = d1;
          ip1 = i;
        }
      d2 = (vert[i] - p2).norm();
      if( d2 < dmin2 )
        {
          dmin2 = d2;
          ip2 = i;
        }
    }

  if( verbose )
    cout << "computing distance map..." << endl;
  t1[ ip1 ] = 1;
  dtx = MeshDistance( mesh[0], t1, true );
  float	dmax = dtx[ ip2 ];
  float	dtarget = dmax * ratio;
  unsigned	it = 0;
  dmin1 = FLT_MAX;

  if( verbose )
    cout << "computing plane intersection..." << endl;

  Point3df	pl2( plane2[0], plane2[1], plane2[2] );

  for( i=0; i<n; ++i )
    {
      const AimsVector<uint, 3>	& p = poly[i];
      if( ( vert[p[0]].dot( pl ) + dof ) 
          * ( vert[p[1]].dot( pl ) + dof ) <= 0 )
        {
          otx[p[0]] = dtx[p[0]];
          otx[p[1]] = dtx[p[1]];
          if( pl2.dot( vert[p[0]] ) + plane2[3] >= 0 
              && fabs( otx[p[0]] - dtarget ) < dmin1 )
            {
              dmin1 = fabs( otx[p[0]] - dtarget );
              it = p[0];
            }
          if( pl2.dot( vert[p[1]] ) + plane2[3] >= 0 
              && fabs( otx[p[1]] - dtarget ) < dmin1 )
            {
              dmin1 = fabs( otx[p[1]] - dtarget );
              it = p[1];
            }
        }
      if( ( vert[p[0]].dot( pl ) + dof ) 
          * ( vert[p[2]].dot( pl ) + dof ) <= 0 )
        {
          otx[p[0]] = dtx[p[0]];
          otx[p[2]] = dtx[p[2]];
          if( pl2.dot( vert[p[0]] ) + plane2[3] >= 0 
              && fabs( otx[p[0]] - dtarget ) < dmin1 )
            {
              dmin1 = fabs( otx[p[0]] - dtarget );
              it = p[0];
            }
          if( pl2.dot( vert[p[2]] ) + plane2[3] >= 0 
              && fabs( otx[p[2]] - dtarget ) < dmin1 )
            {
              dmin1 = fabs( otx[p[2]] - dtarget );
              it = p[2];
            }
        }
      if( ( vert[p[1]].dot( pl ) + dof ) 
          * ( vert[p[2]].dot( pl ) + dof ) <= 0 )
        {
          otx[p[2]] = dtx[p[2]];
          otx[p[1]] = dtx[p[1]];
          if( pl2.dot( vert[p[2]] ) + plane2[3] >= 0 
              && fabs( otx[p[2]] - dtarget ) < dmin1 )
            {
              dmin1 = fabs( otx[p[2]] - dtarget );
              it = p[2];
            }
          if( pl2.dot( vert[p[1]] ) + plane2[3] >= 0 
              && fabs( otx[p[1]] - dtarget ) < dmin1 )
            {
              dmin1 = fabs( otx[p[1]] - dtarget );
              it = p[1];
            }
        }
    }

  return it;
}
 

int main( int argc, const char **argv )
{
  try
    {
      Reader<AimsSurfaceTriangle>	filein;
      Writer<Texture1d>			fileout;
      Point3df				point1, point2;
      string				mode = "shortest";
      Point4df				plane( 0, 0, 0, 0 );
      Point4df				plane2( 0, 0, 0, 0 );
      float				ratio = 1;

      AimsApplication	app( argc, argv, "Measurement along a line on a " 
                             "mesh" );
      app.addOption( filein, "-i", "input mesh" );
      app.addOption( point1, "-p1", "3D position of the starting point " 
                     "(x,y,z)" );
      app.addOption( point2, "-p2", "3D position of the end point (x,y,z)" );
      app.addOption( fileout, "-o", "output texture line (default: not " 
                     "written)", true );
      app.addOption( mode, "-m", "mode: 'shortest' line, 'plane' line " 
                     "(default: shortest)", true );
      app.addOption( plane, "-pe", "plane equation for 'plane' mode (see -m " 
                     "option) (a,b,c,d) for plane ax+by+cz+d=0", true );
      app.addOption( ratio, "-r", "ratio of the length between p1 and p2 " 
                     "where a point has to be determined (default: 1)", true );
      app.addOption( plane2, "-pe2", "half-space (plane equation) where to " 
                     "look for the target point in case of ambiguity (useful " 
                     "only in 'plane' mode) (default: no restriction)", true );
      app.alias( "--input", "-i" );
      app.alias( "--output", "-o" );
      app.initialize();

      enum Mode
        {
          Shortest, Plane, 
        };
      Mode	pmode;
      if( mode == "shortest" )
        pmode = Shortest;
      else if( mode == "plane" )
        pmode = Plane;
      else
        throw invalid_argument( string( "unknown mode " ) + mode );

      AimsSurfaceTriangle	mesh;
      if( verbose )
        cout << "reading mesh..." << endl;
      filein.read( mesh );

      Texture1d			otex;
      unsigned			it, ip1, ip2;

      switch( pmode )
        {
        case Shortest:
          if( plane != Point4df( 0, 0, 0, 0 ) )
            throw invalid_argument( "pe argument must not be used in " 
                                    "'shortest' mode" );
          it = shortest( otex, ip1, ip2, mesh, point1, point2, ratio );
          break;
        case Plane:
          it = planedoit( otex, ip1, ip2, mesh, point1, point2, ratio, plane, 
                          plane2 );
        }

      const vector<Point3df>	& vert = mesh.vertex();
      cout << "point1: " << ip1 << ", position: " << vert[ip1] << ", dist: " 
           << otex[0][ip1] << endl;
      cout << "point2: " << ip2 << ", position: " << vert[ip2] << ", dist: " 
           << otex[0][ip2] << endl;
      cout << "target: " << it << ", position: " << vert[it] << ", dist: " 
           << otex[0][it] << endl;

      if( !fileout.fileName().empty() )
        {
          if( verbose )
            cout << "writing texture..." << endl;
          fileout.write( otex );
          if( verbose )
            cout << "done." << endl;
        }
    }
  catch( user_interruption &e )
    {
    }
  catch( exception &e )
    {
      cerr << argv[ 0 ] << ": " << e.what() << endl;
      return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}


