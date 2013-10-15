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
                                  AimsSurfaceTriangle& surface,
                                  float rate )
{

    if( _verbose )
        cout << "smoothing "<< flush;

    ASSERT( rate >= 0.0 && rate <= 1.0 );
    
    switch(_smoothType)
    {
        case LAPLACIAN:
            getSmoothedLaplacian( vfac, surface, _smoothFeatureAngle, _smoothIt, rate );
            break;
        case SIMPLESPRING:
            getSmoothedSimpleSpring( vfac, surface, _smoothForce, _smoothIt, rate );
            break;
        case POLYGONSPRING:
            getSmoothedPolygonSpring( vfac, surface, _smoothForce, _smoothIt, rate );
            break;
        case LOWPASS:
            getSmoothedLowPassFilter( vfac, surface, _smoothIt, rate );
            break;
        default:
            cout << "This smoothing type does not exist."<< endl;
            break;
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

void Mesher::getSmoothedLaplacian( const vector< Facet* >& vfac,
                                   AimsSurfaceTriangle& surface,
                                   float featureAngle,
                                   int nIteration,
                                   float rate )
{
    uint id;
    int n, size = (int)vfac.size();
    float alpha, beta, force;
    float curv, curv_mean;
    bool okToMove;
    
    Facet* facet = NULL, *f1 = NULL, *f2 = NULL, *f3 = NULL;
    Point3df a1, a2, a3, n1, n2;
    
    vector< Point3df >& vertex = surface.vertex();
    vector< Point3df > bary( size );
    vector< Point3df > smoothed( size );
    
    float cosFeatureAngle = cos( featureAngle * M_PI / 180.0 );
    
    //
    // If  [i] is not a feature vertex, the displacement of [i] is the average
    // position of his neighboors ponderated by "rate" :
    // -------->                      -------->                -------->
    // vertex[i] = (1 - rate) * vertex[i] + rate * neighb[i]
    //
    // If  [i] is a feature, vertex [i] is not moved.
    //
    
    cout << " laplacian algo " << endl;
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
                    // the facet and its (v+1)th neighbor
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
                bary[ id ] = Point3df(0,0,0);
                for ( int v = 0; v < facet->nNeighbor(); v++ )
                    bary[ id ] += vertex[ facet->pNeighbor( v )->id() ];
                bary[ id ] /= (float)facet->nNeighbor();

                smoothed[ id ] = ( 1 - rate ) * vertex[ id ] + rate * bary[ id ];
            }
            else
                smoothed[ id ] = vertex[ id ];
        }
        swap( smoothed, vertex );
    }
}

void Mesher::getSmoothedSimpleSpring( const vector< Facet* >& vfac,
                                      AimsSurfaceTriangle& surface,
                                      float force,
                                      int nIteration,
                                      float rate )
{
    uint id;
    int n, size = (int)vfac.size();
    Facet* facet = NULL;
    
    vector< Point3df >& vertex = surface.vertex();
    vector< Point3df > bary( size );
    vector< Point3df > smoothed( size );
    vector< Point3df > original( size );
    
    for ( n = 0; n != size; n++ )
    {
        original[ vfac[ n ]->id() ] = vertex[ vfac[ n ]->id() ];
    }
    
    //
    // The displacement of [i] is the average position of his neighboors
    // ponderated by "rate" plus a restoring force dependent on the initial position :
    // -------->                      -------->                -------->
    // vertex[i] = (1 - rate) * vertex[i] + rate * neighb[i] -
    //                                   -------->   ------>
    //             force * rate * (vertex[i] - orig[i])
    //
    
    cout << " simple spring algo " << endl;
    if( _verbose )
        cout << " iteration " << setw(5) << 0 << flush;
    
    for ( int i = 0; i < nIteration; i++ )
    {
        if( _verbose )
        cout << "\b\b\b\b\b" << setw(5) << i + 1 << flush;
        
        for ( n = 0; n != size; n++ )
        {
            facet = vfac[ n ];
            id = facet->id();
            
            smoothed[ id ] = Point3df(0,0,0);
            bary[ id ] = Point3df(0,0,0);
            for ( int v = 0; v < facet->nNeighbor(); v++ )
                bary[ id ] += vertex[ facet->pNeighbor( v )->id() ];
            bary[ id ] /= (float)facet->nNeighbor();

            smoothed[ id ] = ( 1 - rate ) * vertex[ id ] + rate * bary[ id ] -
                               force * rate * (vertex[ id ] - original[ id ]);
        }
        swap( smoothed, vertex );
    }
}

void Mesher::getSmoothedPolygonSpring( const vector< Facet* >& vfac,
                                       AimsSurfaceTriangle& surface,
                                       float force,
                                       int nIteration,
                                       float rate )
{
    uint id;
    int n, size = (int)vfac.size();
    Facet* facet = NULL;
    
    Point3df vec1, vec2, b;
    Point3df Normal, Normal_min;
    int k1, k2;
    float d, dist, dist_min;
    
    vector< Point3df >& vertex = surface.vertex();
    vector< Point3df > bary( size );
    vector< Point3df > smoothed( size );
    vector< Point3df > original( size );
    vector< Point3df > movedorig( size );
    Point3df movedbary;
    
    for ( n = 0; n != size; n++ )
    {
        original[ vfac[ n ]->id() ] = vertex[ vfac[ n ]->id() ];
    }
    
    //
    // The displacement of [i] is the average position of his neighboors
    // ponderated by "rate" plus a restoring force dependent on the nearest
    // initial polygon :
    // -------->                      -------->                -------->
    // vertex[i] = (1 - rate) * vertex[i] + rate * neighb[i] -
    //                                   -------->   --------->
    //             force * rate * (vertex[i] - neworig[i])
    //
    
    cout << " polygon spring algo " << endl;
    if( _verbose )
        cout << " iteration " << setw(5) << 0 << flush;
    
    for ( int i = 0; i < nIteration; i++ )
    {
        if( _verbose )
        cout << "\b\b\b\b\b" << setw(5) << i + 1 << flush;
        
        for ( n = 0; n != size; n++ )
        {
            facet = vfac[ n ];
            id = facet->id();
            dist_min = 1000.;
            
            smoothed[ id ] = Point3df(0,0,0);
            bary[ id ] = Point3df(0,0,0);
            for ( int v = 0; v < facet->nNeighbor(); v++ )
                bary[ id ] += vertex[ facet->pNeighbor( v )->id() ];
            bary[ id ] /= (float)facet->nNeighbor();
            
            movedbary = ( 1 - rate ) * vertex[ id ] + rate * bary[ id ];
            
            for ( int v = 0; v < facet->nNeighbor(); v++ )
            {
                k1 = facet->pNeighbor( v )->id();
                k2 = facet->pNeighbor( ( v + 1 ) % facet->nNeighbor() )->id();
                vec1 = original[ k2 ] - original[ k1 ];
                vec2 = original[ k1 ] - original[ id ];
                b  = crossed( vec1, vec2 );
                Normal = -b/norm( b );

                d = - Normal.dot( original[ id ] );
                dist = Normal.dot( movedbary ) + d;

                if ( fabs(dist) < dist_min )
                {
                    dist_min = dist;
                    Normal_min = Normal;
                }
            }
            movedorig[ id ] = - dist_min * Normal_min + movedbary;

            smoothed[ id ] = movedbary - force * rate * (vertex[ id ] - movedorig[ id ]);
        }
        swap( smoothed, vertex );
    }
}

void Mesher::getSmoothedLowPassFilter( const vector< Facet* >& vfac,
                                       AimsSurfaceTriangle& surface,
                                       int nIteration,
                                       float rate )
{
    uint id;
    int n, size = (int)vfac.size();
    Facet* facet = NULL;
    float newDeplFactor;
    
    vector< Point3df >& vertex = surface.vertex();
    vector< Point3df > bary( size );
    vector< Point3df > smoothed( size );
    
    //
    // The displacement of [i] is the average position of his neighboors
    // ponderated by "rate" then ponderated by "-1.02 * rate" (Taubin's Method) :
    // -------->                     -------->                -------->
    // vertex[i] = (1 - rate) * vertex[i] + rate * neighb[i]
    // ----------->                               ----------->                          ----------->
    // newvertex[i] = (1 - (-1.02 * rate) * newvertex[i] + (-1.02 * rate) * newneighb[i]
    //
    
    if( _verbose )
        cout << " iteration " << setw(5) << 0 << flush;
    
    for ( int i = 0; i < nIteration; i++ )
    {
        if( _verbose )
        cout << "\b\b\b\b\b" << setw(5) << i + 1 << flush;
        
        for ( n = 0; n != size; n++ )
        {
            facet = vfac[ n ];
            id = facet->id();

            smoothed[ id ] = Point3df(0,0,0);
            bary[ id ] = Point3df(0,0,0);
            for ( int v = 0; v < facet->nNeighbor(); v++ )
                bary[ id ] += vertex[ facet->pNeighbor( v )->id() ];
            bary[ id ] /= (float)facet->nNeighbor();

            smoothed[ id ] = ( 1 - rate ) * vertex[ id ] + rate * bary[ id ];
        }
        swap( smoothed, vertex );
        for ( n = 0; n != size; n++ )
        {
            facet = vfac[ n ];
            id = facet->id();
            newDeplFactor = -1.02*rate;
            
            bary[ id ] = Point3df(0,0,0);
            for ( int v = 0; v < facet->nNeighbor(); v++ )
                bary[ id ] += vertex[ facet->pNeighbor( v )->id() ];
            bary[ id ] /= (float)facet->nNeighbor();

            smoothed[ id ] = ( 1 - newDeplFactor ) * vertex[ id ] + newDeplFactor * bary[ id ];
        }
        swap( smoothed, vertex );
    }
}
