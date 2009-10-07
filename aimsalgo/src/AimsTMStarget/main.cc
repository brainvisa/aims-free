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
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/getopt/getopt2.h>
#include <aims/distancemap/meshdistance.h>
#include <aims/distancemap/meshvoronoi.h>
#include <aims/mesh/surfaceOperation.h>
#include <float.h>

using namespace aims;
using namespace carto;
using namespace std;

inline unsigned bestVertex(const vector<Point3df>  & vert, const Point3df target, unsigned nPt)
{
  unsigned			n = vert.size();
  unsigned			i,v = 0;
  float				d,dmin = FLT_MAX; 
  Point3df			v1, barycenter;
  map<float,unsigned> 		distance;
  map<float,unsigned>::iterator 	id, ed;
 
  for( i=0; i<n; ++i )
    {
      v1 = vert[i] - target;
      d = sqrt( v1[0] * v1[0] + v1[1] * v1[1] + v1[2] * v1[2] );
      distance[d] = i;
    }

  //cout << "Closest point to the target: " << (distance.begin()) -> second << " at "
  //   << (distance.begin()) -> first << " mm\n";
  
  i = 0;
  v1 = Point3df(0,0,0);
  for (id = distance.begin(), ed = distance.end() ; id != ed && i < nPt ; ++id, ++i )
      barycenter += vert[id->second];
    
  barycenter[0] /= nPt;
  barycenter[1] /= nPt;
  barycenter[2] /= nPt;

  //cout << "Target: " <<  target[0] << " , " 
  //     <<  target[1] << " , " 
  //     <<  target[2] << endl;

  //cout << "Barycenter: " <<  barycenter[0] << " , "
  //     <<  barycenter[1] << " , " 
  //     <<  barycenter[2] << endl;

  for( i=0; i<n; ++i )
    {
      v1 = vert[i] - target;
      d = sqrt( v1[0] * v1[0] + v1[1] * v1[1] + v1[2] * v1[2] );
      distance[d] = i;

      if( d < dmin )
        {
          dmin = d;
          v = i;
        }
    }

  //cout << "Closest point to the barycenter: " << v << " at  "
  //     << dmin << " mm\n";

  return(v);

}



inline double square( double x )
{
  return( x * x );
}


int main( int argc, const char** argv )
{
  string	meshfile, outtexfile, landfile;
  bool		asciiFlag = false;
  unsigned nPt = 10 ; 
      
  AimsApplication	app( argc, argv, 
			     "Creates texture target for TMS aiming" );
  app.addOption( meshfile, "-i", "input mesh file (head)" );
  app.addOption( outtexfile, "-o", "output target texture file" );
  app.addOption( nPt, "-n", "Neighbourhood size for the target position estimation" 
                 "\n[default = 10]",true );
  app.addOption( landfile, "-l", "landmarks positions file" );
  app.addOption( asciiFlag, "--ascii", "write texture file in ASCII mode "
		 "\n[default=binary]", true );
  app.alias( "--input", "-i" );
  app.alias( "--output", "-o" );
  app.alias( "--landmarks", "-l" );

  try
    {
      app.initialize();

      //
      // read triangulation
      //
      cout << "reading triangulation   : " << flush;
      AimsSurfaceTriangle surface;
      Reader<AimsSurfaceTriangle> triR( meshfile );
      triR >> surface;
      cout << "done" << endl;

      const unsigned	NCP = 6;	// nb of control points
      enum CtrlPoints
	{
	  nasion = 0, 
	  vertex, 
	  inion, 
	  lear, 
	  rear, 
	  target
	};
      const string	ctrlName[] = 
	{
	  "nasion", 
	  "vertex", 
	  "controlpoint", 
	  "lear", 
	  "rear", 
	  "target"
	};

      Point3df		p[ NCP ];
      bool		pread[ NCP ];
      unsigned		i, j;
      ifstream		lmarks( landfile.c_str() );
      ASSERT( !!lmarks );
      string		id;

      for( i=0; i<NCP; ++i )
	pread[ i ] = false;

      while( !lmarks.eof() )
	{
	  lmarks >> id;
	  if( id == "nasion:" )
	    {
	      lmarks >> p[nasion][0] >> p[nasion][1] >> p[nasion][2];
	      pread[ nasion ] = true;
	    }
	  else if( id == "vertex:" )
	    {
	      lmarks >> p[vertex][0] >> p[vertex][1] >> p[vertex][2];
	      pread[ vertex ] = true;
	    }
	  else if( id == "eperon:" || id == "inion:" )
	    {
	      lmarks >> p[inion][0] >> p[inion][1] >> p[inion][2];
	      pread[ inion ] = true;
	    }
	  else if( id == "og:" )
	    {
	      lmarks >> p[lear][0] >> p[lear][1] >> p[lear][2];
	      pread[ lear ] = true;
	    }
	  else if( id == "od:" )
	    {
	      lmarks >> p[rear][0] >> p[rear][1] >> p[rear][2];
	      pread[ rear ] = true;
	    }
	  else if( id == "target:" )
	    {
	      lmarks >> p[target][0] >> p[target][1] >> p[target][2];
	      pread[ target ] = true;
	    }
	  else
	    cerr << "unknown label " << id << endl;
	}
      for( i=0; i<NCP; ++i )
	if( !pread[i] )
	  {
	    cout << "Control point " << ctrlName[i] << " not specified\n";
	  }

      //	target

      TimeTexture<float>	ttex;
      Texture<float>		& tex = ttex[0];
      Texture<float>		& textemp = ttex[1]; //to compute temp distance
      Texture<float>		& texplane = ttex[2]; //to compute temp plane
      const vector<Point3df>	& vert = surface[0].vertex();
      unsigned			n = vert.size();
      float			eps = 2.;
      unsigned			v = 0;
      float			dmin = FLT_MAX; //, dmax = 0;
      Point3df			norm;
      Point3df			v1, v2, v3;
      float			d, d1, d2, d3;
      Texture<short>		texseed(n);

      cout << "computing target distance map\n";

      SurfaceManip::nearestPointToMesh( p[target], surface, nPt, &v );

      texseed.item( v ) = 1;

      tex = meshdistance::MeshDistance( surface[0], texseed, false );

      //search for the nearest point to nasion and inion

      float		dminP[ NCP ];
      unsigned	vindex[ NCP ];

      for( j=0; j<NCP; ++j )
	{
	  dminP[j] = FLT_MAX;
	  vindex[j] = 0;
	}

      dminP[target] = dmin;
      vindex[target] = v;

      for( i=0; i<n; ++i )
	{
	  for( j=0; j<NCP; ++j )
	    if( j != target )
	      {
		v1 = vert[i] - p[j];
		d = sqrt( v1[0] * v1[0] + v1[1] * v1[1] + v1[2] * v1[2] );
		if( d < dminP[j] )
		  {
		    dminP[j] = d;
		    vindex[j] = i;
		  }

		/*if(vert[i][2]<(p[eperon][2]-2)) // ?? pourquoi ça ?
		  {
		  v2 = vert[i] - p[2];
		  d = sqrt( v2[0] * v2[0] + v2[1] * v2[1] + v2[2] * v2[2] );
		  if( d < dmin_eperon )
		  {
		  dmin_eperon = d;
		  veperon = i;
		  }
		  }*/
	      }
	}

      for( j=0; j<NCP; ++j )
	p[j] = vert[ vindex[j] ];

      cout << "control points : " << endl;
      for( i=0; i<6; ++i )
	if( pread[i] )
	  cout << ctrlName[i] << " : " << p[i] << endl;

      //	first plane equations pts nasion - vertex - inion
      if( pread[ inion ] )
	{
	  v1 = p[vertex] - p[nasion];
	  v2 = p[inion] - p[nasion];
	  norm[0] = v1[1] * v2[2] - v1[2] * v2[1];
	  norm[1] = v1[2] * v2[0] - v1[0] * v2[2];
	  norm[2] = v1[0] * v2[1] - v1[1] * v2[0];
	  d = 1. / sqrt( norm[0] * norm[0] + norm[1] * norm[1] 
			 + norm[2] * norm[2] );
	  norm[0] *= d;
	  norm[1] *= d;
	  norm[2] *= d;
	  d1 = norm[0] * p[nasion][0] + norm[1] * p[nasion][1] 
	    + norm[2] * p[nasion][2];
	}
      else
	{
	  cout << "computing inter_hemispheric plane without inion\n";
	  Point3df	midear = p[lear] + p[rear];
	  midear *= 0.5;
	  v1 = p[vertex] - p[nasion];
	  v2 = midear - p[nasion];
	  norm[0] = v1[1] * v2[2] - v1[2] * v2[1];
	  norm[1] = v1[2] * v2[0] - v1[0] * v2[2];
	  norm[2] = v1[0] * v2[1] - v1[1] * v2[0];
	  //norm = p[lear] - p[rear];
	  d = 1. / sqrt( norm[0] * norm[0] + norm[1] * norm[1] 
			 + norm[2] * norm[2] );
	  norm[0] *= d;
	  norm[1] *= d;
	  norm[2] *= d;
	  d1 = norm[0] * p[nasion][0] + norm[1] * p[nasion][1] 
	    + norm[2] * p[nasion][2];

	  // geodesic distance to nasion
	  Texture<short>	texdisnas;
	  texdisnas.reserve( n );
	  for( i=0; i<n; ++i )
	    texdisnas.push_back( 0 );
	  texdisnas.item( vindex[ nasion ] ) = 1;	// seed
	  Texture<float>	texdn 
	    = meshdistance::MeshDistance( surface[0], texdisnas, false );
	  //nearest point of interhemispheric plane and 20cm from nasion (up)
	  float	d = FLT_MAX, z0 = p[nasion][0], x, dnasion = 300;
	  unsigned	k = 0;
	  for( i=0; i<n; ++i )
	    if( vert[i][2] < z0 )
	      {
		x = sqrt( square( texdn.item(i) - dnasion ) 
			  + square( norm[0] * vert[i][0] 
				    + norm[1] * vert[i][1] 
				    + norm[2] * vert[i][2] - d1 ) );
		if( x < d )
		  {
		    d = x;
		    k = i;
		  }
	      }
	  //cout << "d : " << d << endl;
	  //cout << "k : " << k << endl;
	  vindex[ inion ] = k;
	  p[inion] = vert[k];

	  cout << "control point on mid-plane : " << p[ inion ] << ", " 
	       << texdn.item( vindex[inion] ) 
	       << " mm from nasion on interhemispheric plane\n";
	}

      texplane.reserve( n );
      textemp.reserve( n );
      cout << "computing distance to interhemispheric plane\n";
      dmin = FLT_MAX;
      for( i=0; i<n; ++i )
	{
	  d2 = norm[0] * vert[i][0] + norm[1] * vert[i][1] 
	    + norm[2] * vert[i][2];
	  if( fabs( d1 - d2 ) < eps )
	    {
	      if( vert[i][2] <= p[lear][2] || vert[i][2] <= p[inion][2] )
		{
		  texplane.push_back( 100 ) ;
		}
	      else 
		{
		  texplane.push_back( 0 );
		}
	      d3 = tex.item(i);
	      if( d3 < dmin )
		{
		  dmin = d3;
		}
	    }
	  else
	    {
	      texplane.push_back( 0 );
	    }
	  if( fabs( d1 - d2 ) < 6 )
	    {
	      if( vert[i][2] <= p[lear][2] || vert[i][2] <= p[inion][2] )
		{
		  textemp.push_back( 0 );
		}
	      else 
		{
		  textemp.push_back( meshdistance::MESHDISTANCE_FORBIDDEN);
		}
	    }
	  else
	    {
	      textemp.push_back( meshdistance::MESHDISTANCE_FORBIDDEN);
	    }
	}

      cout << "Geodesic distance target - inter-hemispheric plane : " << dmin 
	   << " mm" << endl;
    
      textemp.item( vindex[nasion] ) = 100;
      cout << "computing distance to nasion to find middle point\n";

      for( i=0; i<n; ++i )
	texseed.item(i) = (short) textemp.item(i);
      textemp = meshdistance::MeshDistance( surface[0], texseed, false );

      float l_nasion_inion;
      l_nasion_inion = textemp.item( vindex[inion] );
      cout << "longueur geodesique nasion - pt controle : " 
	   << l_nasion_inion <<" mm\n";

      dmin = FLT_MAX;
      float dmiddle = FLT_MAX;
      unsigned vmiddle = 0;
      for( i=0; i<n; ++i )
	{
	  if( texplane.item(i) == 100 )
	    {
	      d2 = textemp.item(i);
	      if (fabs(d2 - l_nasion_inion/2)<dmiddle) 
		{
		  dmiddle = fabs(d2 - l_nasion_inion/2);
		  vmiddle = i;
		}	      
	    }
	}

      p[vertex] = vert[vmiddle];
      vindex[vertex] = vmiddle;
      cout << "Position of modified vertex : " << p[vertex] << endl;
      for( i=0; i<NCP; ++i )
	if( i != target )
	  {
	    float birdl;
	    v1 = p[i] - p[target];
	    birdl = sqrt( v1[0] * v1[0] + v1[1] * v1[1] + v1[2] * v1[2] );
	    cout << "Geodesic distance target - " << ctrlName[i] << " : " 
		 << tex.item( vindex[i] ) << " mm" << endl;
	    cout << "Bird fly distance target - " << ctrlName[i] << " : " 
		 << birdl << " mm" << endl;
	  }
 
      //	plane pts vertex - lear - rear
      Point3df	norm2;
      v1 = p[lear] - p[vertex];
      v2 = p[rear] - p[vertex];
      norm2[0] = v1[1] * v2[2] - v1[2] * v2[1];
      norm2[1] = v1[2] * v2[0] - v1[0] * v2[2];
      norm2[2] = v1[0] * v2[1] - v1[1] * v2[0];
      d2 = 1. / sqrt( norm2[0] * norm2[0] + norm2[1] * norm2[1] 
		      + norm2[2] * norm2[2] );
      norm2[0] *= d2;
      norm2[1] *= d2;
      norm2[2] *= d2;
      d2 = norm2[0] * p[vertex][0] + norm2[1] * p[vertex][1] 
	+ norm2[2] * p[vertex][2];

      dmin = FLT_MAX;
      for( i=0; i<n; ++i )
	{
	  d = norm2[0] * vert[i][0] + norm2[1] * vert[i][1] 
	    + norm2[2] * vert[i][2];
	  if( fabs( d - d2 ) < eps )
	    {
	      texplane.item(i) = 200;
	      d = tex.item(i);
	      if( d < dmin )
		{
		  dmin = d;
		}
	    }
	}
      cout << "Distance to ear plane : " << dmin << " mm" << endl;

      for( i=0; i<n; ++i )
	{
	  if(texplane.item(i)>0) tex.item(i)=0;
	}
      Writer<Texture1d>	tw( outtexfile );
      tw.write( ttex, asciiFlag );
    }
  catch( user_interruption & )
    {
      return EXIT_FAILURE;
    }
  catch( exception & e )
    {
      cerr << e.what() << endl;
      return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}


