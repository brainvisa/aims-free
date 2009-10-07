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
#include <aims/math/mathelem.h>
#include <iomanip>
#include <math.h>

using namespace std;

void Mesher::getSmoothedVertices( const vector< Facet* >& vfac,
                                  vector< Point3df >& vertex,
                                  float featureAngle,
                                  int nIteration,
                                  float xDeplFactor,
                                  float yDeplFactor,
                                  float zDeplFactor )
{

  if( _verbose )
    cout << "smoothing "<< flush;

  ASSERT( xDeplFactor >= 0.0 && xDeplFactor <= 1.0 &&
          yDeplFactor >= 0.0 && yDeplFactor <= 1.0 &&
          zDeplFactor >= 0.0 && zDeplFactor <= 1.0 );

  float cosFeatureAngle = cos( featureAngle * M_PI / 180.0 );

  //
  // If  [i] is not a feature vertex, the displacement of [i] is the average
  // position of his neighboors ponderated by deplFactor :
  // -------->                         -------->                    -------->
  // vertex[i][0] = (1- xDeplfactor) * vertex[i][0] + xDeplfactor * neighb[i][0]
  // ...
  // If  [i] is a feature, vertex [i] is not moved.
  //
  uint id;
  int n, size = (int)vfac.size();
  bool okToMove;
  Facet* facet = NULL, *f1 = NULL, *f2 = NULL, *f3 = NULL;
  Point3df a1, a2, a3, n1, n2;

  vector< Point3df > smoothed( size );

  if( _verbose )
    cout << " iteration " << setw(5) << 0 << flush;
  for ( int i = 0; i < nIteration; i++ )
  {
    if( _verbose )
      cout << "\b\b\b\b\b" << setw(5) << i + 1 << flush;
    for ( n = 0; n != size; n++ )
    { 
      okToMove = true;
      facet = vfac[ n ];
      id = facet->id();

      if ( featureAngle != 180.0 )
        for ( int v = 0; v < facet->nNeighbor(); v++ )
        { 
          // computing feature angle for edge between
          // ith facet and its (v+1)th neighbor
          f1 = facet->pNeighbor( v );
          f2 = facet->pNeighbor( ( v + 1 ) % facet->nNeighbor() );
          f3 = facet->pNeighbor( ( v + 2 ) % facet->nNeighbor() );
          a1 = vertex[ f1->id() ] - vertex[ f2->id() ];
          a2 = vertex[ id ] - vertex[ f2->id() ];
          a3 = vertex[ f3->id() ] - vertex[ f2->id() ];
          n1 = crossed( a1, a2 );
          n2 = crossed( a2, a3 );

          // comparing computed angle with given feature angle
          if ( n1.dot( n2 ) / sqrt( norm( n1 ) * norm( n2 ) ) 
               <=  cosFeatureAngle )
          {
            okToMove = false;
            break;
          }
        }

      if (okToMove)
      {
        smoothed[ id ] = Point3df(0,0,0);
        for ( int v = 0; v < facet->nNeighbor(); v++ )
          smoothed[ id ] += vertex[ facet->pNeighbor( v )->id() ];
        smoothed[ id ] /= (float)facet->nNeighbor();

	smoothed[ id ][ 0 ] = ( 1 - xDeplFactor ) * vertex[ id ][ 0 ] +
		              xDeplFactor * smoothed[ id ][ 0 ];
	smoothed[ id ][ 1 ] = ( 1 - yDeplFactor ) * vertex[ id ][ 1 ] +
		              yDeplFactor * smoothed[ id ][ 1 ];
	smoothed[ id ][ 2 ] = ( 1 - zDeplFactor ) * vertex[ id ][ 2 ] +
		              zDeplFactor * smoothed[ id ][ 2 ];
      }
      else
        smoothed[ id ] = vertex[ id ];
    }
    swap( smoothed, vertex );
  }

  if( _verbose )
    {
      cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b"<< flush;
      cout << "               "<< flush;
      cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b"<< flush;

      cout << "\b\b\b\b\b\b\b\b\b\b\b"<<flush;
      cout << "           "<<flush;
      cout << "\b\b\b\b\b\b\b\b\b\b\b"<<flush;
    }
}
