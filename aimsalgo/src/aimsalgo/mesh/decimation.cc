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
#include <float.h>

using namespace std;

#define MAX_COS_ALLOWED 0.9
#define MIN_COS_ALLOWED -0.9

struct Flag
{
  bool deadFlag;
  bool featureFlag;
  bool goodToDeleteFlag;
  bool steadyFlag;
  bool unknownFeatureFlagFlag;
  bool usedFlag;
  float error;
  Facet* facet;
  Point3df vertex;
  float min;
  float max;
};

void computesMinMax( vector< Point3df >& vertex, Flag& flag );
void computesFeatureFlag( vector< Point3df >& vertex, Flag& flag,
                          float cosFeatureAngle );
bool computesGoodToDeleteFlags( vector< Facet* >& vfac,
                                float maxClearanceMm,
			        vector< Flag >& vFlag );
void cleanVectors( vector< Facet* >& vfac,
                   vector< Point3df >& vertex,
                   vector< Flag >& vFlag );
bool deletesFacet( Flag& flag,
                   vector< Point3df >& vertex,
                   float maxErrorMm );
bool seeksHoleTriangulation( Flag& flag,
                             vector< Point3df >& vertex,
                             float maxErrorMm,
                             short* tabNeighReorder );
bool checkTriangleValidity( const Point3df& a, // 
                            const Point3df& b, // triangle to check is (abc)
                            const Point3df& c, // 
                            const Point3df& f, // it replace (fca) and (abf)
                            float maxErrorMm,
                            float& cosbac,
                            float& error );
void triangulatesHole( Facet* facet,
                       short* tabNeighReorder );
void removesNeighbor( Facet* facet, Facet* neigh );
void addsNeighborAfterRef( Facet* facet, Facet* neigh, Facet* ref );
void addsNeighborBeforeRef( Facet* facet, Facet* neigh, Facet* ref );
bool isNeighbor( Facet* facet, Facet* neigh );


 
void computesMinMax( vector< Point3df >& vertex, Flag& flag )
{
  flag.min = FLT_MAX;
  flag.max = 0.0;
  float curr;
  for ( int v = 0; v < flag.facet->nNeighbor(); v++ )
  {
    curr = norm( vertex[ flag.facet->pNeighbor( v )->id() ] - flag.vertex );
    if ( curr < flag.min )
      flag.min = curr;
    if ( curr > flag.max )
      flag.max = curr;
  }
}



void computesFeatureFlag( vector< Point3df >& vertex, Flag& flag,
                          float cosFeatureAngle )
{
  flag.featureFlag = false;
  flag.unknownFeatureFlagFlag = false;

  if ( cosFeatureAngle <= -1.0 )
    return;

  Facet *f1 = NULL, *f2 = NULL, *f3 = NULL;
  Point3df a1, a2, a3, n1, n2;
  for ( int v = 0; v < flag.facet->nNeighbor(); v++ )
  {
    // computing feature angle for edge between
    // facet and its (v+1)th neighbor
    f1 = flag.facet->pNeighbor( v );
    f2 = flag.facet->pNeighbor( ( v + 1 ) % flag.facet->nNeighbor() );
    f3 = flag.facet->pNeighbor( ( v + 2 ) % flag.facet->nNeighbor() );
    a1 = vertex[ f1->id() ] - vertex[ f2->id() ];
    a2 = vertex[ flag.facet->id() ] - vertex[ f2->id() ];
    a3 = vertex[ f3->id() ] - vertex[ f2->id() ];
    n1 = crossed( a1, a2 );
    n2 = crossed( a2, a3 );

    // comparing computed angle with given feature angle
    if ( n1.dot( n2 ) / sqrt( norm( n1 ) * norm( n2 ) ) <= cosFeatureAngle )
    {
      flag.featureFlag = true;
      break;
    }
  }
}



bool computesGoodToDeleteFlags( vector< Facet* >& vfac,
                                float maxClearanceMm,
			        vector< Flag >& vFlag )
{
  // computes global min and max edge length
  float min = FLT_MAX;
  float max = 0.0;
  int size = (int)vFlag.size();
  int	n;

  for ( n = 0; n < size; n++ )
  {
    if ( vFlag[ n ].min < min )
      min = vFlag[ n ].min;
    if ( vFlag[ n ].max > max )
      max = vFlag[ n ].max;
  }

  // computes GoodToDelete flags
  int nFound = 0;
  float priorityValue = min;
  float step = ( max - min ) / 10.0 + 1.0;
  for ( n = 0; n < size; n++ )
    vFlag[ n ].goodToDeleteFlag = false;
  
  Point3df v1, v2, v3;
  bool deleteFeature=false;

  while ( true )
  {
    priorityValue += step;
    for ( n = 0; n < size; n++ )
    {
      if ( vFlag[ n ].goodToDeleteFlag == false &&
           vFlag[ n ].deadFlag == false &&
           vFlag[ n ].steadyFlag == false &&
           vFlag[ n ].min <= priorityValue &&
           vFlag[ n ].max <= maxClearanceMm )
      {
        if ( vFlag[ n ].featureFlag == false )
        {
          nFound ++;
          vFlag[ n ].goodToDeleteFlag = true;
        }
        else if ( deleteFeature )
        {
          nFound ++;
          vFlag[ n ].goodToDeleteFlag = true;
        }
      }
    }
    if ( nFound >= (int)vfac.size() / 100 + 1 )
      break;
    if ( priorityValue >= max && deleteFeature == true )
      break;
    if ( priorityValue >= max && deleteFeature == false )
    {
      priorityValue = 0;
      deleteFeature = true;
    }
  }

  return nFound != 0 ? true : false;
}




void cleanVectors( vector< Facet* >& vfac,
                   vector< Point3df >& vertex,
                   vector< Flag >& vFlag )
{
  int size = (int)vFlag.size();
  int nDead = 0;
  int	n;

  for ( n = 0; n < size; n++ )
    if ( vFlag[ n ].deadFlag == true )
      nDead ++;

  vector< Point3df > vertex2( size - nDead );
  vector< Facet* > vfac2( size - nDead );
  vector< Flag > vFlag2( size - nDead );

  int counter = 0;
  for ( n = 0; n < size; n++ )
  {
    if ( vFlag[ n ].deadFlag == false )
    {
      vertex2[ counter ] = vFlag[ n ].vertex;
      vfac2[ counter ] = vFlag[ n ].facet;
      vFlag2[ counter ] = vFlag[ n ];
      counter ++;
    }
  }

  uint	m;
  for ( m = 0; m < vfac2.size(); ++m )
    vfac2[m]->id() = m;

  vertex.clear();
  vfac.clear();
  vFlag.clear();

  vertex = vertex2;
  vfac = vfac2;
  vFlag = vFlag2;
}




bool deletesFacet( Flag& flag,
                   vector< Point3df >& vertex,
                   float maxErrorMm )
{
  short tabNeighReorder[ 256 ];

  if ( seeksHoleTriangulation( flag, vertex, maxErrorMm,
                               tabNeighReorder ) == false ) 
    return false;
   
  triangulatesHole( flag.facet, tabNeighReorder );

  return true;
}



// virtual re-triangulation 
// of the facet's deletion hole by ordering the facet's neighbors
//
// Ex :                 1                            
//                    2     4                   
//                                             
//                     5       6               
//                        3                    
//                                             
//     means triangulation : (1 2 4) (2 5 4) (3 6 5) (4 5 6)
//                                             
//     method : - 1 is the first vertex indice, a triangle is
//                assumed with its neighbor in the hole which 
//                append to be 2 and 4. At this point the neighbors
//                of 2 in the hole are 5 and 4 (the hole is reduced
//                to the (2 5 3 6 4) loop
//              - 2 is the second vertex indice, the triangle 
//                (2 5 4) is assumed and the hole is reduced to 
//                (5 3 6 4)
//              - ...
//              
//     At each step the validity of the performed triangle is cheked 
//     (lenght of edges, orientation, flatness).
//
//     The ordering process is ruled by the selection of the  
//     most equilateral triangle to be made in the hole
//

bool seeksHoleTriangulation( Flag& flag,
                             vector< Point3df >& vertex,
                             float maxErrorMm,
                             short* tabNeighReorder )
{
  int i,j,k;
  Facet* facet = flag.facet;

  // according to the mode of deletion (feature deletion or normal deletion)
  // the best-triangle criterion is not the same :
  // - triangles are built ordered by increasing angle for normal deletion, 
  // - triangles are built the less erroneous as possible for feature deletion

  float initialBestScore;

  if ( flag.featureFlag )
  {
    // the criterion is a minimal distance value
    initialBestScore = FLT_MAX;
  }
  else
  {
    // the criterion is a maximum cosinus value
    initialBestScore = -2;
  }

  // temporary data storage for recording the virtual re-triangulation
  int nNeighReorder = 0;
  bool tabNeighUsedFlag[ 256 ];
  bool tabBadNeighFlag[ 256 ];

  for ( i = 0; i < facet->nNeighbor(); i++ )
  {
    tabBadNeighFlag[ i ] = false;
    tabNeighUsedFlag[ i ] = false;
    tabNeighReorder[ i ] = -1;
  }

  // virtual re-triangulation subloop
  bool cancelSuppression = false;
  int currentBestNeigh;
  float currentBestNeighScore;
  float cosbac, error;
  Point3df a, b, c, f;
  while ( nNeighReorder < facet->nNeighbor() - 2 && cancelSuppression == false )
  {
    // initalizes the next neighbor selection 
    currentBestNeigh = -1; // no candidate found yet
    currentBestNeighScore = initialBestScore;
    for ( i = 0; i < facet->nNeighbor(); i++ ) 
    {
      // already used flags cannot be used again
      if ( tabNeighUsedFlag[ i ] == true )
        continue;
      // bad neighbors cannot be used (cf further)

      if ( tabBadNeighFlag[ i ] == true )
        continue;

      // Test the triangle which vertex are the ith neighbor of
      // facet (a) and its 2 neighbors in the hole (b and c) 
      a = vertex[ facet->pNeighbor( i )->id() ];

      // seeks the superior neighbor of neighbor i which is unused : b
      for ( j = ( i + 1 ) % facet->nNeighbor(); tabNeighUsedFlag[ j ] == true;
            j = ( j + 1 ) % facet->nNeighbor() ) {}
      b = vertex[ facet->pNeighbor( j )->id() ];

      // seeks the inferior neighbor of neighbor i which is unused : c
      for ( k = ( i + facet->nNeighbor() - 1 ) % facet->nNeighbor();
            tabNeighUsedFlag[ k ] == true;
            k = ( k + facet->nNeighbor() - 1 ) % facet->nNeighbor() ) {}
      c = vertex[ facet->pNeighbor( k )->id() ];

      // checks if edge (bc) already exists
      if ( nNeighReorder != facet->nNeighbor() - 3 )
        if ( k != ( j + 1 ) % facet->nNeighbor() &&
             j != ( k + 1 ) % facet->nNeighbor()   )
        {
          if ( isNeighbor( facet->pNeighbor( j ), facet->pNeighbor( k ) ) )
            continue;
        }

      // checks new triangle
      f = vertex[ facet->id() ];
      if ( checkTriangleValidity( a, b, c, f, maxErrorMm,
                                  cosbac, error ) == false )
        continue;

      // save i indice if it append to generate a more interesting
      // triangle than the one already saved
      if ( flag.featureFlag )
      {
        // the criterion for the-best-triangle-selection is the minimal error
        if ( currentBestNeighScore > error )
        {
          currentBestNeigh = i;
          currentBestNeighScore = error;
        }
      }
      else
      {
         // the criterion fot the-best-triangle-selection is the
         // minimal angle (abc) : the maximal cosinus	      
         if ( currentBestNeighScore < cosbac )
         {
           currentBestNeigh = i;
           currentBestNeighScore = cosbac;
         }
      }
    }

    // cheks if a candidate has been found
    if ( currentBestNeigh == -1 )
    {
      // if no candidate has been found and triangulation
      // has not started then cancels deletion
      if ( nNeighReorder == 0 )
      {
        cancelSuppression = true;
        break;
      }

      // if no candidate has been found but triangulation
      // has begun, triangulation process step back and avoid the
      // troublesome neighbor (BadNeighFlag = true)
      nNeighReorder --;
      for ( j = 0; j < facet->nNeighbor(); j++ )
        if ( tabNeighReorder[ j ] == nNeighReorder )
          break;
      tabBadNeighFlag[ j ] = true;
      tabNeighReorder[ j ] = -1;
      tabNeighUsedFlag[ j ] = false;
      continue;
    }

    // save the currentbestneigh as the next vertex indice for the
    // hole triangulation
    tabNeighUsedFlag[ currentBestNeigh ] = true;
    tabNeighReorder[ currentBestNeigh ] = nNeighReorder;
    nNeighReorder ++;
  }

  // cheks if re-triangulation is not cancelled
  if ( cancelSuppression == true ) 
    return false;

  return true;
}



                 
bool checkTriangleValidity( const Point3df& a, // 
                            const Point3df& b, // triangle to check is (abc)
                            const Point3df& c, // 
                            const Point3df& f, // it replace (fca) and (abf)
                            float maxErrorMm,
                            float& cosbac,
                            float& error )
{
  // computes edges (ab) (ac) and (bc)
  Point3df vab( b - a );
  Point3df vac( c - a );
  Point3df vbc( c - b );

  // exits if triangle (abc) is flat
  if ( norm( vab ) == 0.0 || norm( vac ) == 0.0 || norm( vbc ) == 0.0 )
  {
    cerr << "Warning : flat triangle encountered" << endl;
    return false;
  }

  // computes (abc) normal
  Point3df nabc( crossed( vab, vac ) );
  float normv = norm( nabc );

  // cheks the flatness  of (abc)
  if ( normv == 0.0 )
    return false;
  nabc /= normv;

  // cheks the orientation  of (abc) respect to the two triangles
  // it substitute : (fca) and (fab)
  Point3df vaf( f - a );
  Point3df nfca( crossed( vaf, vac ) );
  if ( nfca.dot( nabc ) <= 0 )
    return false;
  Point3df nfab( crossed( vab, vaf ) );
  if ( nfab.dot( nabc ) <= 0 )
    return false;

  // computes the error generated by building (abc). An evaluation 
  // of this error is the distance between f and the edge (bc)
  Point3df vbf( f - b );
  error = vbf.dot( crossed( vaf, vbc ) ) * vbf.dot( crossed( vaf, vbc ) ); 
  if ( error > maxErrorMm )
    return false;

  // computes the cosinus of (cab)
  cosbac = vab.dot( vac ) / ( norm( vab ) * norm( vac ) );
  // (cab) must be a little bit from flat
  if ( cosbac > MAX_COS_ALLOWED )
    return false;
  if ( cosbac < MIN_COS_ALLOWED ) 
    return false;

  return true;
}


// effective re-triangulation based on virtual re-triangulation
// subloop results

void triangulatesHole( Facet* facet,
                       short*	tabNeighReorder )
{
  int i, j, k;
  bool tabNeighUsedFlag[ 256 ];
  Facet *f1 = NULL, *f2 = NULL, *f3 = NULL;

  for ( i = 0; i < facet->nNeighbor(); i++ )
    tabNeighUsedFlag[ i ] = false;

  for ( i = 0; i < facet->nNeighbor() - 3; i++ )
  {
    // seeks the neighbor which order is i
    for ( k = 0; k < facet->nNeighbor() && tabNeighReorder[ k ] != i; k++ ) {}
    f1 = facet->pNeighbor( k );
    tabNeighUsedFlag[ k ] = true;

    // facet is deleted from f1's neighborhood
    removesNeighbor( f1, facet );

    // seeks its 2 hole neighbors
    for ( j = ( k + 1 ) % facet->nNeighbor(); tabNeighUsedFlag[ j ] == true;
          j = ( j + 1 ) % facet->nNeighbor() ) {}
    f2 = facet->pNeighbor( j );

    for ( j = ( k + facet->nNeighbor() - 1 ) % facet->nNeighbor();
          tabNeighUsedFlag[ j ] == true; 
          j = ( j + facet->nNeighbor() - 1 ) % facet->nNeighbor() ) {}
    f3 = facet->pNeighbor( j );

    // triangle is (f1 f2 f3)
    // f2 is add to f3's neighborhood before facet
    addsNeighborBeforeRef( f3, f2, facet );

    // f3 is add to f2's neighborood after facet
    addsNeighborAfterRef( f2, f3, facet );
  }

  // the last triangle is made by removing facet from the neighborood of the
  // last 3 facets of tneighreorder first facet of the last triangle
  for ( i = 0; i < facet->nNeighbor(); i++ )
    if ( tabNeighUsedFlag[ i ] == false )
      removesNeighbor( facet->pNeighbor( i ), facet );
}




void removesNeighbor( Facet* facet, Facet* neigh )
{
  Facet* pNeighbor[ 256 ];

  int v, j;
  for ( v = 0, j = 0; v < facet->nNeighbor(); v++ )
  {
    if ( facet->pNeighbor( v ) != neigh )
      pNeighbor[ j++ ] = facet->pNeighbor( v );
  }

  facet->setNeighbor( pNeighbor, j );
}




void addsNeighborAfterRef( Facet* facet, Facet* neigh, Facet* ref )
{
  Facet* pNeighbor[ 256 ];

  int v, j;
  for ( v = 0, j = 0; v < facet->nNeighbor(); v++ )
  {
    if ( facet->pNeighbor( v ) == ref )
    {
      pNeighbor[ j++ ] = ref;
      pNeighbor[ j++ ] = neigh;
    }
    else
      pNeighbor[ j++ ] = facet->pNeighbor( v );
  }

  facet->setNeighbor( pNeighbor, j );
}




void addsNeighborBeforeRef( Facet* facet, Facet* neigh, Facet* ref )
{
  Facet* pNeighbor[ 256 ];

  int v, j;
  for ( v = 0, j = 0; v < facet->nNeighbor(); v++ )
  {
    if ( facet->pNeighbor( v ) == ref )
    {
      pNeighbor[ j++ ] = neigh;
      pNeighbor[ j++ ] = ref;
    }
    else
      pNeighbor[ j++ ] = facet->pNeighbor( v );
  }

  facet->setNeighbor( pNeighbor, j );
}




bool isNeighbor( Facet* facet, Facet* neigh )
{
  for ( int v = 0; v < facet->nNeighbor(); v++ )
    if ( facet->pNeighbor( v ) == neigh )
      return true;
  return false;
}


//
// main code of decimation algorithm
//
void Mesher::getDecimatedVertices( vector< Facet* >& vfac,
                                   vector< Point3df >& vertex,
                                   float reductionRatePercent,
                                   float maxClearanceMm,
                                   float maxErrorMm,
                                   float minFeatureEdgeAngleDegree )
{
  if( _verbose )
    cout << "decimating " << flush;

  // flags and constants initialization
  int n, size = (int)vfac.size();
  int rehashStep = 1000;
  int rehashValue = rehashStep;
  bool endDecimationLoop = false;
  int nDecimated = 0;
  int nToDecimate = (int)( (float)size * reductionRatePercent / 100.0 + 0.5 );
  float cosFeatureAngle = cos( minFeatureEdgeAngleDegree * M_PI / 180.0 );
  if ( minFeatureEdgeAngleDegree >= 180.0 )
    cosFeatureAngle = -100.0;

  // init flags
  vector< Flag > vFlag( size );
  for ( n = 0; n < size; n++ )
  {
    vFlag[ n ].deadFlag	= false;
    vFlag[ n ].featureFlag = false;
    vFlag[ n ].unknownFeatureFlagFlag = false;
    vFlag[ n ].steadyFlag = false;
    vFlag[ n ].error = 0.0;
    vFlag[ n ].facet = vfac[ n ];
    vFlag[ n ].vertex = vertex[ vfac[ n ]->id() ];
  }

  for ( n = 0; n != size; n++ )
  {
    computesFeatureFlag( vertex, vFlag[ n ], cosFeatureAngle );
    computesMinMax( vertex, vFlag[ n ] );
  }


  
  // main loop
  int id;
  if( _verbose )
    cout << setw(6) << nDecimated << flush;
  do
  {
    // refreshing complex and feature flags
    for ( n = 0; n != (int)vFlag.size(); n++ )
    {
      if ( vFlag[ n ].deadFlag == false )
        if ( vFlag[ n ].unknownFeatureFlagFlag )
        {
          computesFeatureFlag( vertex, vFlag[ n ], cosFeatureAngle );
          computesMinMax( vertex, vFlag[ n ] );
        }
    }

    // refreshes main flag map
    if ( computesGoodToDeleteFlags( vfac, maxClearanceMm, vFlag ) == false )
    { 
      endDecimationLoop = true;
      break;
    }

    // begins main subloop
    for ( n = 0; n != (int)vFlag.size(); n++ )
    {
      // selects Good-To-Delete facets
      if ( vFlag[ n ].goodToDeleteFlag == false )
        continue;

      // tries to remove facet from the mesh
      if ( deletesFacet( vFlag[ n ], vertex, maxErrorMm ) == false )
      {
        vFlag[ n ].steadyFlag = true;
        continue;
      }

      // sets dead flag
      vFlag[ n ].deadFlag = true;

      // set feature and complex flags unknown
      for ( int v = 0; v < vFlag[ n ].facet->nNeighbor(); v++ )
      {
        id =vFlag[n].facet->pNeighbor( v )->id();
        vFlag[ id ].steadyFlag = false;
        vFlag[ id ].unknownFeatureFlagFlag = true;
        vFlag[ id ].goodToDeleteFlag = false;
      }

      // counts the current deletion
      nDecimated ++;
      if ( nDecimated >= nToDecimate )
      {
        endDecimationLoop = true;
        break;
      }
      if ( nDecimated % 100 == 0 && _verbose )
        cout << "\b\b\b\b\b\b" << setw( 6 ) << nDecimated << flush;
    }

    // rehash data once
    if ( nDecimated > rehashValue )
    {
      cleanVectors( vfac, vertex, vFlag );
      rehashValue += rehashStep;
    }
  } 
  while ( endDecimationLoop == false );

  if( _verbose )
    cout << "\b\b\b\b\b\b" << flush;
  //cout << "\b\b\b\b\b\b" << "      " << "\b\b\b\b\b\b" << flush;

  // removes dead data generated by the decimation
  cleanVectors( vfac, vertex, vFlag );

  if( _verbose )
    cout << "\b\b\b\b\b\b\b\b\b\b\b" << flush;
  //cout << "           " << flush;
  //cout << "\b\b\b\b\b\b\b\b\b\b\b" << flush;

}
