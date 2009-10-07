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
#include <iomanip>

using namespace std;

const uint NULL_TRI = 4000000000U;

void Mesher::getFacet( const AimsSurfaceTriangle& surface, 
                       vector< Facet* >& vfac )
{
  ASSERT( vfac.size() == 0 );

  if( _verbose )
    cout << "building facets from mesh : " << flush;

  const vector< AimsVector< uint, 3 > >& triangle = surface.polygon();
  const vector< Point3df >& vertex = surface.vertex();

  int	n;

  for ( n = 0; n < (int)triangle.size(); n++ )
  {
    ASSERT( triangle[ n ][ 0 ] != triangle[ n ][ 1 ] &&
            triangle[ n ][ 0 ] != triangle[ n ][ 2 ] &&
            triangle[ n ][ 1 ] != triangle[ n ][ 2 ] &&
            triangle[ n ][ 0 ] < triangle.size() &&
            triangle[ n ][ 1 ] < triangle.size() &&
            triangle[ n ][ 2 ] < triangle.size()  );
  }


  // map of facet initialisation, a facet is create for each vertex
  vfac.reserve( vertex.size() );

  Facet* facet = NULL;
  Point3d zero( 0, 0, 0 );
  for ( n = 0; n < (int)vertex.size(); n++ )
  {
    facet = new Facet;
    facet->offset() = 0;
    facet->type() = 255;
    facet->location() = zero;
    facet->setNeighbor( 0 );
    // the face identifier is the vertex indice in the vector of vertex
    facet->id() = (uint)n;
    vfac.push_back( facet );
  }

  // computes neighborhood by first seeking for each facet the 
  // triangles that share the facet's vertex
  vector< AimsVector< uint, 3 > > *linkedTriangles 
    = new vector< AimsVector< uint, 3 > >[ vfac.size() ];
  for ( n = 0; n < (int)triangle.size(); n++ )
  {
    // the linkedTriangles table saves the list of triangles sharing
    // each facet vertex
    const AimsVector< uint, 3 >& tri = triangle[ n ];
    linkedTriangles[ tri[ 0 ] ].push_back( tri );
    linkedTriangles[ tri[ 1 ] ].push_back( tri );
    linkedTriangles[ tri[ 2 ] ].push_back( tri );
  }
  
  // then for each facet the neighborhood is constructed
  // from this list of neighbors triangles
  if( _verbose )
    cout << setw( 6 ) << 0 << flush;

  AimsVector< uint, 3 > nullTri( NULL_TRI, NULL_TRI, NULL_TRI );
  uint lastInd = 0, currInd = 0;
  for ( n = 0; n < (int)vfac.size(); n++ )
  {
    vector< AimsVector< uint, 3 > >& linkedTriOfFacet = linkedTriangles[ n ];

    ASSERT( linkedTriOfFacet.size() != 0 );

    facet = vfac[ n ];
    facet->setNeighbor( (int)linkedTriOfFacet.size() );

    // the first two neighbors are computed from the first triangle 
    if ( linkedTriOfFacet[ 0 ][ 0 ] == facet->id() )
    {
      lastInd = linkedTriOfFacet[ 0 ][ 1 ];
      currInd = linkedTriOfFacet[ 0 ][ 2 ];
    }
    else if ( linkedTriOfFacet[ 0 ][ 1 ] == facet->id() )
    {
      lastInd = linkedTriOfFacet[ 0 ][ 2 ];
      currInd = linkedTriOfFacet[ 0 ][ 0 ];
    }
    else if ( linkedTriOfFacet[ 0 ][ 2 ] == facet->id() )
    {
      lastInd = linkedTriOfFacet[ 0 ][ 0 ];
      currInd = linkedTriOfFacet[ 0 ][ 1 ];
    }
    facet->pNeighbor( 0 ) = vfac[ lastInd ];
    facet->pNeighbor( 1 ) = vfac[ currInd ];

    linkedTriOfFacet[ 0 ] = nullTri;

    bool found = false;
    int nNeighbor = 2;
    while ( nNeighbor < facet->nNeighbor() )
    {
      // seeks the next triangles which include vertex indices 
      // lastInd and currInd
      found = false;
      for ( int v = 0; v < (int)linkedTriOfFacet.size(); v++ )
      {
        // tests if the vth triangle is the next one in the cycle surrounding 
        if ( linkedTriOfFacet[ v ] == nullTri ) 
          continue;
        for ( int j = 0; j < 3; j++ )
        {
          if ( linkedTriOfFacet[ v ][ j ] == currInd )
          {
            // the vth triangle is the good one, its jth vertex is the last
            // neighbor found,so its ( j + 1 ) % 3 vertex is saved as the next
            // neighbor
            currInd = linkedTriOfFacet[ v ][ ( j + 1 ) % 3 ];
            facet->pNeighbor( nNeighbor++ ) = vfac[ currInd ];
            linkedTriOfFacet[ v ] = nullTri;
            found = true;
          }
          if ( found == true )
            break;
        }
        if ( found == true )
          break;
      }
      if ( found == false )
      {
        if( _verbose )
          cout << "Warning: non cyclic neighborhood ... trying to recover"
               << endl;
        break;
      }
    }
    if( _verbose )
      cout << "\b\b\b\b\b\b" << setw( 6 ) << n << flush;
  }
  if( _verbose )
    cout << "\b\b\b\b\b\b" << "      " << "\b\b\b\b\b\b" << flush;

  for ( n = 0; n < (int)vertex.size(); n++ )
    linkedTriangles[ n ].clear();

  // Checking mesh
  for ( n = 0; n < (int)vfac.size(); n++ )
    ASSERT( vfac[ n ]->nNeighbor() > 0 && vfac[ n ]->nNeighbor() < 256 );

  if( _verbose )
    {
      cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b"
           << flush;
      cout << "                            " << flush;
      cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b"
           << flush;
    }

  delete[] linkedTriangles;
}
