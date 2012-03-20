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


#include <aims/mesh/mesher.h>
#include <aims/mesh/surfaceOperation.h>
#include <iomanip>
#include <stdio.h>

using namespace aims;
using namespace std;


void Mesher::getBrain( const AimsData<short>& thing,
                       AimsSurfaceTriangle& surface, bool insideinterface )
{
  map< size_t, list< MapOfFacet > > interface;
  map< size_t, list< MapOfFacet > >::iterator	ii, ei = interface.end();

  getInterface( interface, thing );

  int sizeMax = 0;
  bool first = true;
  list< MapOfFacet >::iterator i, iBig;
  for ( ii=interface.begin(); ii!=ei; ++ii )
    if ( ii->second.size() )
      for ( i = ii->second.begin(); i != ii->second.end(); ++i )
      {
        if( first )
        {
          first = false;
          if( insideinterface )
            continue; // skip 1st interface
            // FIXME: probably not a good test...
        }
        if ( (int)i->size() > sizeMax )
        {
          sizeMax = (int)i->size();
          iBig = i;
        }
      }

  if( sizeMax != 0 )
    getMeshFromMapOfFacet(thing, surface, *iBig);
  if( insideinterface )
    // change ext/int notions for internal mesh
    SurfaceManip::invertSurfacePolygons( surface );
  clear( interface );
}

//the code is duplicated in order to allow some fine tuning
// of smoothing operations...
//the following method assumes white matter is set to 0;
//the surrounding cube is unfortunatelly also tracked which
//is expensive.
//We should refine getInterface( interface, thing ); latter


void Mesher::getWhite( const AimsData<short>& thing,
                       AimsSurfaceTriangle& surface )
{
  map< size_t, list< MapOfFacet > > interface;

  getInterface( interface, thing );

  list< MapOfFacet >::iterator i, iBig;
  uint sizebig=0;
  map< size_t, list< MapOfFacet > >::iterator	ii, ei = interface.end();

  int	nlist = 0;
  int	nsurface = 0;
  int	n;
  for ( ii=interface.begin(); ii!=ei; ++ii )
    if ( ii->second.size() )
      {
	nlist++;
	for ( i = ii->second.begin(), i++; i != ii->second.end(); i++ )
	  {
	    if(i->size()>sizebig)
	      {
		iBig = i;
		sizebig = i->size();
	      }
	    nsurface++;
	  }
      }
  if(nlist!=1)
    cout << "warning, input to Mesher::getWhite is not a binary image as supposed"<<endl;

    cout << "Input to Mesher::number of 6-connected interfaces: "<<nsurface<<endl;

  cout << "processing mesh    : " << flush;

  cout << "reduced neighborhood " << flush;
  getReducedNeighOutwardCycles( *iBig );
  cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\v" << flush;

  cout << "extended neighborhood" << flush;
  getExtendedNeighOfFacet( *iBig );
  cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\v" << flush;

  // creates of vector of pointers to facet
  vector< Facet* > vfac( iBig->size() );
  MapOfFacet::iterator f;
  int k;
  for ( f = iBig->begin(), k = 0; f != iBig->end(); f++, k++ )
  {
    vfac[ k ] = f->second;
    vfac[ k ]->id() = k;
  }

  // gets the coordinates of the center of each facet
  cout << "vertices             " << flush;
  getVertices( vfac, surface.vertex(), 
                     thing.sizeX(), thing.sizeY(), thing.sizeZ() );        
  cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\v" << flush;

  // smoothes the vertex coordinates
  //  getSmoothedVertices( vfac, surface.vertex(), 180.0, 5, 0.4, 0.4, 0.4 );
  //  getSmoothedVertices( vfac, surface.vertex(), 180.0, 5, 0.4, 0.4, 0.4 );

  // decimates the mesh
  float initialMeshSize = (float)vfac.size();
  float currentAngle = 180.0;
  //  for ( int n = 0; n < 3; n++ )
  cout << "deciMaxClearance:" << _deciMaxClearance << endl << flush;
  cout << "deciMaxError:" << _deciMaxError << endl << flush;
  getDecimatedVertices( vfac, surface.vertex(), 99.0,
			_deciMaxClearance, _deciMaxError, 
			currentAngle );

  float finalMeshSize = (float)vfac.size();

  // smoothes the vertex coordinates
  //  getSmoothedVertices( vfac, surface.vertex(), 180.0, 3, 0.2, 0.2, 0.2 );
  //  getSmoothedVertices( vfac, surface.vertex(), 180.0, 5, 0.2, 0.2, 0.2 );

  // gets the normals by taking the average of all the 
  // normals of the neighbors of a facet
  cout << "normals              " << flush;
  getNormals( vfac, surface.vertex(), surface.normal() );        
  cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\v" << flush;
  // reverse the normal signs because the 6-connectivity based
  // surface tracking algorithm led to reverse the binary image
  vector< Point3df >& normal = surface.normal();
  n = (int)normal.size();
  int	z;
  for ( z = 0; z < n; z++ )
    {
      normal[z] = - normal[z];
    }


  // gets the triangles with a SLAVE/MASTER algorithm
  cout << "triangles            " << flush;
  getTriangles( vfac, surface.polygon() );
  cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\v" << flush;

  cout << "done ( " 
       << ( 1.0 - finalMeshSize / initialMeshSize ) * 100.0
       << "% reduction )" << endl;

  // reverse the polygon order because the 6-connectivity based
  // surface tracking algorithm led to reverse the binary image

  vector< AimsVector< uint,3> >& triangle = surface.polygon();
  int sizeT = (int)triangle.size();
  uint  buf_vertex;
  for ( n = 0; n < sizeT; n++ )
  {
    buf_vertex = triangle[ n ][0];
    triangle[n][0] = triangle[n][2];
    triangle[n][2] = buf_vertex;
  }

  clear( interface );
}
