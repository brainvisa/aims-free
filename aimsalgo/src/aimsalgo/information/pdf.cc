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


#include <aims/information/pdf.h>
#include <aims/math/mathelem.h>
#include <aims/data/data.h>
#include <aims/bucket/bucketMap.h>
#include <iostream>
#include <list>
#include <math.h>

using namespace std;
using namespace aims;

inline
double parzen( double u, double h )
{
    return exp( - 0.5 * sqr( u / h ) ) / ( sqrt( 2 * M_PI ) * h );
}

inline
double parzen2( double u, double h, int dim)
{
    return exp( - 0.5 * u / (h * h) ) / ( pow(h, dim));
}





void AimsParzenJointPdf( const AimsData<short>& data1,
                         const AimsData<short>& data2,
                         AimsData<float>& p12, 
                         AimsData<float>& p1, AimsData<float>& p2 )
{
    int levels = p12.dimX();

    ASSERT( p12.dimY() == levels && p1.dimX() == levels && p2.dimX() == levels );

    float mini1 = float( data1.minimum() );
    float maxi1 = float( data1.maximum() );
    ASSERT( maxi1 != mini1 );

    float mini2 = float( data2.minimum() );
    float maxi2 = float( data2.maximum() );
    if ( maxi2 == mini2 )
    {
        cerr << "Warning ! AimsParzenJointPdf : maxi2 == mini2" << endl;
        return;
    }

    float h1 = ( maxi1 - mini1 ) / levels;
    float h2 = ( maxi2 - mini2 ) / levels;

    int i, j, k, n1, n2;

    p12 = 0.0;

    ForEach3d( data1, i, j, k )
    {
        ForEach2d( p12, n1, n2 )
        {
            p12( n1, n2 ) += parzen( mini1 + ( n1 + 0.5 ) * h1 - 
                                     float( data1( i, j, k ) ), h1 )
                           * parzen( mini2 + ( n2 + 0.5 ) * h2 - 
                                     float( data2( i, j, k ) ), h2 );
        }
    }

    float sum=0.0;
    int x, y;
    ForEach2d( p12, x, y )
        sum += p12( x, y );
        
    if (sum)
        ForEach2d( p12, x, y )
            p12( x, y ) /= sum;

    p1 = 0.0;
    p2 = 0.0;
    for ( y = 0; y < levels; y++ )
        for ( x = 0; x < levels; x++ )
        {
            p1( x ) += p12( x, y );
            p2( y ) += p12( x, y );
        }
}


void AimsParzenPdf(const AimsData<short>& data,
                   AimsData<float>& p)
{
    int levels = p.dimX();

    float mini = float( data.minimum() );
    float maxi = float( data.maximum() );
    ASSERT( maxi != mini );

    float h = ( maxi - mini ) / levels;

    int i, j, k, n;

    p = 0.0;

    ForEach3d(data, i, j, k)
    {
        ForEach1d( p, n )
            p( n ) += parzen(mini + ( n + 0.5 ) * h 
                             - float( data( i, j, k ) ), h);
    }

    float sum=0.0;
    int x;
    ForEach1d( p, x )
        sum += p( x );
        
    if (sum)
        ForEach1d( p, x )
            p( x ) /= sum;
}


void AimsWinParzenJointPdf(const AimsData<short>& data1,
                           const AimsData<short>& data2,
                           AimsData<float>& p12, 
                           AimsData<float>& p1, AimsData<float>& p2,
                           const AimsData<float>& mask)
{
    int levels = p12.dimX();

    ASSERT(p12.dimY() == levels && p1.dimX() == levels && p2.dimX() == levels);

    float mini1 = float( data1.minimum() );
    float maxi1 = float( data1.maximum() );
    ASSERT(maxi1 != mini1);

    float mini2 = float( data2.minimum() );
    float maxi2 = float( data2.maximum() );
    if ( maxi2 == mini2 )
    {
        cerr << "Warning ! AimsWinParzenJointPdf : maxi2 == mini2" << endl;
        return;
    }

    float h1 = ( maxi1 - mini1 ) / levels;
    float h2 = ( maxi2 - mini2 ) / levels;

    int sMask = mask.dimX();
    int sMaskdiv2 = sMask / 2;
    ASSERT( sMask % 2 == 1 );

    int i, j, k, n1, n2, v1, v2, val1, val2;

    AimsData<float> mask2d( sMask, sMask );
    for ( v1 = 0; v1 < sMask; v1++ )
        for ( v2 = 0; v2 < sMask; v2++ )
            mask2d( v1, v2 ) += mask( v1 ) * mask( v2 );

    p12 = 0.0;

    ForEach3d( data1, i, j, k )
    {
        n1 = int((float(data1(i, j, k)) - mini1) / h1);
        n2 = int((float(data2(i, j, k)) - mini2) / h2);
        if ( n1 == levels )
            n1--;
        if ( n2 == levels )
            n2--;

        for (v1 = 0; v1 < sMask; v1++)
            for (v2 = 0; v2 < sMask; v2++)
            {
                val1 = n1 + v1 - sMaskdiv2;
                val2 = n2 + v2 - sMaskdiv2;
                if (val1 >= 0 && val1 < levels &&
                    val2 >= 0 && val2 < levels)
                    p12(val1, val2) += mask2d(v1, v2);
            }
    }

    float sum=0.0;
    int x, y;
    ForEach2d(p12, x, y)
        sum += p12( x, y );
        
    if (sum)
        ForEach2d(p12, x, y)
            p12( x, y ) /= sum;

    p1 = 0.0;
    p2 = 0.0;
    for (y = 0; y < levels; y++)
        for (x = 0; x < levels; x++)
        {
            p1( x ) += p12( x, y );
            p2( y ) += p12( x, y );
        }
}


void AimsWinParzenPdf(const AimsData<short>& data,
                      AimsData<float>& p, const AimsData<float>& mask)
{
    int levels = p.dimX();

    float mini = float( data.minimum() );
    float maxi = float( data.maximum() );
    ASSERT( maxi != mini );

    float h = ( maxi - mini ) / levels;

    int sMask = mask.dimX();
    ASSERT( sMask % 2 == 1 );
    int i, j, k, n, v, val;

    p = 0.0;

    ForEach3d( data, i, j, k )
    {
        n = int( ( float( data( i, j, k ) ) - mini ) / h );
        if ( n == levels )
            n--;

        for ( v = 0; v < sMask; v++ )
        {
            val = n + v - sMask / 2;
            if ( val >= 0 && val < levels )
                p( val ) += mask( v );
        }
    }

    float sum=0.0;
    int x;
    ForEach1d(p, x)
        sum += p(x);
    if (sum)
        ForEach1d(p, x)
            p(x) /= sum;
}

void AimsJointPdf( const AimsData<short>& data1,
                   const AimsData<short>& data2,
                   AimsData<float>& p12, 
                   AimsData<float>& p1, AimsData<float>& p2 )
{
    int levels = p12.dimX();

    ASSERT( p12.dimY() == levels && p1.dimX() == levels && p2.dimX() == levels );

    // TODO: data1 and data2 must necessarily have the same voxel size. It would
    // probably be better to use the lower voxel sizes and field of view 
    // to determine the step and the grid to use during pdf updates
    ASSERT( data1.sizeX() == data2.sizeX()
         && data1.sizeY() == data2.sizeY()
         && data1.sizeZ() == data2.sizeZ() );

    int dx = min(data1.dimX(), data2.dimX()), 
        dy = min(data1.dimY(), data2.dimY()), 
        dz = min(data1.dimZ(), data2.dimZ());

    float mini1 = float( data1.minimum() );
    float maxi1 = float( data1.maximum() );
    float mini2 = float( data2.minimum() );
    float maxi2 = float( data2.maximum() );

    double h1 = (( mini1 == maxi1) ? 1 : ( maxi1 - mini1 ) / levels);
    double h2 = (( mini2 == maxi2) ? 1 : ( maxi2 - mini2 ) / levels);

    int i, j, k, n1, n2;

    p12 = 0.0;

    for(k = 0; k < dz; ++k)
        for(j = 0; j < dy; ++j)
            for(i = 0; i < dx; ++i)
            {
                n1 = int( ( float( data1( i, j, k ) ) - mini1 ) / h1 );
                n2 = int( ( float( data2( i, j, k ) ) - mini2 ) / h2 );
                if ( n1 == levels )
                    n1--;
                if ( n2 == levels )
                    n2--;

                p12( n1, n2 )++;
            }

    float sum=0.0;
    int x, y;
    ForEach2d( p12, x, y )
        sum += p12( x, y );
        
    if (sum)
        ForEach2d( p12, x, y )
            p12( x, y ) /= sum;

    p1 = 0.0;
    p2 = 0.0;
    for ( y = 0; y < levels; y++ )
        for ( x = 0; x < levels; x++ )
        {
            // Due to floatting point arithmetic, probability
            // can be a little bit over 1. Min function is used
            // to fix the issue
            p1(x) = std::min((double)p1(x) + p12( x, y ), (double)1.0);
            p2(y) = std::min((double)p2(y) + p12( x, y ), (double)1.0);
        }
}



void AimsPdf( const AimsData<short>& data, AimsData<float>& p )
{
    int levels = p.dimX();


    float mini = float( data.minimum() );
    float maxi = float( data.maximum() );
    ASSERT( maxi != mini );

    float h = ( maxi - mini ) / levels;

    int i, j, k, n;

    p = 0.0;

    ForEach3d(data, i, j, k)
    {
        n = int((float(data(i, j, k )) - mini) / h);
        if (n == levels)
            n--;

        p(n)++;
    }

    float sum=0.0;
    int x;
    ForEach1d( p, x )
        sum += p( x );
        
    if (sum) 
        ForEach1d( p, x )
            p( x ) /= sum;
}


static float minMask(const AimsData<short>& d)
{
    short mini;
    AimsData<short>::const_iterator it = d.begin() + d.oFirstPoint();
    

    mini = 32767;
    int dt = d.dimT(), dz = d.dimZ(), dy =  d.dimY(), dx = d.dimX();
    for ( int t = 0; t < dt; t++ )
    {
        for ( int z = 0; z < dz; z++ )
        {
            for ( int y = 0; y < dy; y++ )
            {
                for ( int x = 0; x < dx; x++ )
                {
                    if ( (*it < mini)  && (*it != -32768))
                        mini = *it;
                    it++;
                }
                it += d.oPointBetweenLine();
            }
            it += d.oLineBetweenSlice();
        }
        it += d.oSliceBetweenVolume();
    }
    
    return ( (float)mini );           
}


void AimsJointMaskPdf(const AimsData<short>& data1,
                      const AimsData<short>& data2,
                      AimsData<float>& p12, 
                      AimsData<float>& p1, AimsData<float>& p2 )
{
    int levels = p12.dimX();

    ASSERT( p12.dimY() == levels && p1.dimX() == levels && p2.dimX() == levels );

    // TODO: data1 and data2 must necessarily have the same voxel size. It would
    // probably be better to use the lower voxel sizes and field of view 
    // to determine the step and the grid to use during pdf updates
    ASSERT( data1.sizeX() == data2.sizeX()
         && data1.sizeY() == data2.sizeY()
         && data1.sizeZ() == data2.sizeZ() );

    int dx = min(data1.dimX(), data2.dimX()), 
        dy = min(data1.dimY(), data2.dimY()), 
        dz = min(data1.dimZ(), data2.dimZ());

    //  float mini1 = float( data1.minimum() );
    double mini1 = minMask( data1 );
    double maxi1 = double( data1.maximum() );
    double mini2 = minMask( data2 );
    double maxi2 = double( data2.maximum() );
 
    double h1 = (( mini1 == maxi1) ? 1 : ( maxi1 - mini1 ) / levels);
    double h2 = (( mini2 == maxi2) ? 1 : ( maxi2 - mini2 ) / levels);

    int i, j, k, n1, n2;

    p12 = 0.0;
    #ifdef DEBUG
    cout << "DEBUG>>min1 max1 min2 max2 " << mini1 << " " << maxi1 <<" " <<mini2<<" "<<maxi2<<endl;
    #endif

    for(k = 0; k < dz; ++k)
        for(j = 0; j < dy; ++j)
            for(i = 0; i < dx; ++i)
            {
                if (data1(i, j, k ) != short(-32768) && data2(i,j,k) != short(-32768))
                {
                    n1 = int( ( double( data1( i, j, k ) ) - mini1 ) / h1 );
                    n2 = int( ( double( data2( i, j, k ) ) - mini2 ) / h2 );
                    if ( n1 == levels )
                        n1--;
                    if ( n2 == levels )
                        n2--;
                    
                    p12( n1, n2 )++;
                }
            }

    double sum=0.0;
    int x, y;
    
    ForEach2d( p12, x, y )
        sum += p12( x, y );
    
    if (sum)
        ForEach2d( p12, x, y )
            p12( x, y ) /= float(sum);

    p1 = 0.0;
    p2 = 0.0;
    for( y = 0; y < levels; y++)
        for ( x = 0; x < levels; x++ )
        {
            // Due to floatting point arithmetic, probability
            // can be a little bit over 1. Min function is used
            // to fix the issue
            p1(x) = std::min((double)p1(x) + p12( x, y ), (double)1.0);
            p2(y) = std::min((double)p2(y) + p12( x, y ), (double)1.0);
        }
}


void AimsJointPVPdf(const AimsData<short>& data1,
                    const AimsData<short>& data2,
                    const AimsData<PVItem>& comb,
                    AimsData<float>& p12, 
                    AimsData<float>& p1, AimsData<float>& p2 )
{
    int levels = p12.dimX();
    const int  TWO_THEN_SIXTEEN  = 65536;
    const float TWO_THEN_SIXTEEN_CUBE = 65536.0 * 65536.0 * 65536.0;


    ASSERT( p12.dimY() == levels &&
        p1.dimX() == levels &&
        p2.dimX() == levels );
    
    //   float mini1 = float( data1.minimum() );
    float mini1 = minMask( data1 );
    float maxi1 = float( data1.maximum() );
    ASSERT( maxi1 != mini1 );

    //   float mini2 = float( data2.minimum() );
    float mini2 = minMask( data2 );
    float maxi2 = float( data2.maximum() );
    if ( maxi2 == mini2 )
    {
        cerr << "Warning ! AimsJointPdf : maxi2 == mini2" << endl;
        return;
    }

    float h1 = ( maxi1 - mini1 ) / levels;
    float h2 = ( maxi2 - mini2 ) / levels;
    #ifdef DEBUG
    cout << "DEBUG>> min1 max1 min2 max2 " << mini1 << " " << maxi1 << " " << mini2 << " " << maxi2 << endl;
    #endif
    int  n1, n2;

    p12 = 0.0;

    int dx = data2.dimX(); int oS = data2.oSlice();

    AimsData<PVItem>::const_iterator it;
    AimsData<short>::const_iterator  it1;
    AimsData<short>::const_iterator  n2ptr;
    float                            tmp;

    int pvf = 0;
    for (it = comb.begin(), it1 = data1.begin(); it < comb.end(); it++, it1++, pvf++)
        if ( it->offset !=  -1L && *it1 != -32768)
        {
            n1 = (int) ((*it1 - mini1)/h1);
            if (n1 == levels) n1--;

            n2ptr =  data2.begin() + it->offset;
            n2 = (int) (( *n2ptr - mini2)/h2);
            if (n2 == levels) n2--;     
            tmp = (float)(TWO_THEN_SIXTEEN - it->x) *
                (float)(TWO_THEN_SIXTEEN - it->y) *
                (float)(TWO_THEN_SIXTEEN - it->z);
            p12( n1, n2 ) += tmp / TWO_THEN_SIXTEEN_CUBE;


            n2ptr++;
            n2 = (int) (( *n2ptr - mini2)/h2);
            if (n2 == levels) n2--;
            tmp = (float)(it->x) *
                (float)(TWO_THEN_SIXTEEN - it->y) *
                (float)(TWO_THEN_SIXTEEN - it->z);
            p12( n1, n2 ) += tmp / TWO_THEN_SIXTEEN_CUBE;


            n2ptr += dx;
            n2 = (int) (( *n2ptr - mini2)/h2);
            if (n2 == levels) n2--;
            tmp = (float)(it->x) *
                (float)(it->y) *
                (float)(TWO_THEN_SIXTEEN - it->z);
            p12( n1, n2 ) += tmp / TWO_THEN_SIXTEEN_CUBE;


            n2ptr--;
            n2 = (int) (( *n2ptr - mini2)/h2);
            if (n2 == levels) n2--;
            tmp = (float)(TWO_THEN_SIXTEEN - it->x) *
                (float)(it->y) *
                (float)(TWO_THEN_SIXTEEN - it->z);
            p12( n1, n2 ) += tmp / TWO_THEN_SIXTEEN_CUBE;

            n2ptr += oS - dx;
            n2 = (int) (( *n2ptr - mini2)/h2);
            if (n2 == levels) n2--;
            tmp = (float)(TWO_THEN_SIXTEEN - it->x) *
                (float)(TWO_THEN_SIXTEEN - it->y)*
                (float)(it->z);
            p12( n1, n2 ) += tmp / TWO_THEN_SIXTEEN_CUBE;

            n2ptr++;
            n2 = (int) (( *n2ptr - mini2)/h2);
            if (n2 == levels) n2--;
            tmp = (float)(it->x) * 
                (float)(TWO_THEN_SIXTEEN - it->y) *
                (float)(it->z);
            p12( n1, n2 ) += tmp / TWO_THEN_SIXTEEN_CUBE;

            n2ptr +=dx;
            n2 = (int) (( *n2ptr - mini2)/h2);
            if (n2 == levels) n2--;
            tmp = (float)(it->x)*
                (float)(it->y)*
                (float)(it->z);
            p12( n1, n2 ) += tmp / TWO_THEN_SIXTEEN_CUBE;

            n2ptr--;
            n2 = (int) (( *n2ptr - mini2)/h2);
            if (n2 == levels) n2--;
            tmp = (float)(TWO_THEN_SIXTEEN - it->x) *
                (float)(it->y)*
                (float)(it->z);
            p12( n1, n2 ) += tmp / TWO_THEN_SIXTEEN_CUBE;                  
        }
    

    // Normalization
    float sum=0.0;
    int x, y;
    ForEach2d( p12, x, y )
        sum += p12( x, y );
        
    if (sum)
        ForEach2d( p12, x, y )
            p12( x, y ) /= sum;

    // Partial pdf's
    p1 = 0.0;
    p2 = 0.0;
    for ( y = 0; y < levels; y++ )
        for ( x = 0; x < levels; x++ )
        {
            p1( x ) += p12( x, y );
            p2( y ) += p12( x, y );
        }
}

void AimsJointPVPdf(const aims::BucketMap<short>&               data1,
                    const AimsData<short>&                      data2,
                    const vector< PVVectorItem  >& comb,
                    AimsData<float>& p12, 
                    AimsData<float>& p1, AimsData<float>& p2 )
{
    const int TWO_THEN_SIXTEEN = 65536;
    const float TWO_THEN_SIXTEEN_CUBE = 65536.0 * 65536.0 * 65536.0;
    int levels = p12.dimX();

    ASSERT(p12.dimY() == levels &&
           p1.dimX() == levels &&
           p2.dimX() == levels);
    
    BucketMap<short>::Bucket::const_iterator itb, itblast= data1.begin()->second.end() ;
    
    float maxi1 = data1.begin()->second.begin()->second ;
    float mini1 = data1.begin()->second.begin()->second ;
    for(itb=data1.begin()->second.begin(); itb != itblast; ++itb)
        {
            if (maxi1 < itb->second ) maxi1 = itb->second;
            if (mini1 > itb->second ) mini1 = itb->second;
        }


    float maxi2 = data2.maximum();
    float mini2 = minMask( data2 );


    
    if ( (maxi2 == mini2) ||  (maxi1 == mini1) )
        {
            cerr << "Warning ! AimsJointPdf : maxiData == miniData" << endl;
            return;
        }

    float h1 = ( maxi1 - mini1 ) / levels;
    float h2 = ( maxi2 - mini2 ) / levels;


    int  n1, n2;
    int dx = data2.dimX(); int oS = data2.oSlice();
    p12 = 0.0;

    float                            tmp;

    AimsData<short>::const_iterator  n2ptr;

    int i;
    for(itb=data1.begin()->second.begin(), i=0; itb != itblast; ++itb, ++i)
        if ( (comb[i]).offset !=  -1L )
        {
            unsigned short deltax = (comb[i]).x;
            unsigned short deltay = (comb[i]).y;
            unsigned short deltaz = (comb[i]).z;

            n1 = (int) ((itb->second - mini1)/h1);
            if (n1 == levels) n1--;

            n2ptr =  data2.begin() + (comb[i]).offset;
            n2 = (int) (( *n2ptr - mini2)/h2);
            if (n2 == levels) n2--;     
            tmp = (float)(TWO_THEN_SIXTEEN - deltax) *
                (float)(TWO_THEN_SIXTEEN - deltay) *
                (float)(TWO_THEN_SIXTEEN - deltaz);
            p12( n1, n2 ) += tmp / TWO_THEN_SIXTEEN_CUBE;



            n2ptr++;
            n2 = (int) (( *n2ptr - mini2)/h2);
            if (n2 == levels) n2--;
            tmp = (float)(deltax) *
                (float)(TWO_THEN_SIXTEEN - deltay) *
                (float)(TWO_THEN_SIXTEEN - deltaz);
            p12( n1, n2 ) += tmp / TWO_THEN_SIXTEEN_CUBE;


            n2ptr += dx;
            n2 = (int) (( *n2ptr - mini2)/h2);
            if (n2 == levels) n2--;
            tmp = (float)(deltax) *
                (float)(deltay) *
                (float)(TWO_THEN_SIXTEEN - deltaz);
            p12( n1, n2 ) += tmp / TWO_THEN_SIXTEEN_CUBE;


            n2ptr--;
            n2 = (int) (( *n2ptr - mini2)/h2);
            if (n2 == levels) n2--;
            tmp = (float)(TWO_THEN_SIXTEEN - deltax) *
                (float)(deltay) *
                (float)(TWO_THEN_SIXTEEN - deltaz);
            p12( n1, n2 ) += tmp / TWO_THEN_SIXTEEN_CUBE;

            n2ptr += oS - dx;
            n2 = (int) (( *n2ptr - mini2)/h2);
            if (n2 == levels) n2--;
            tmp = (float)(TWO_THEN_SIXTEEN - deltax) *
                (float)(TWO_THEN_SIXTEEN - deltay)*
                (float)(deltaz);
            p12( n1, n2 ) += tmp / TWO_THEN_SIXTEEN_CUBE;

            n2ptr++;
            n2 = (int) (( *n2ptr - mini2)/h2);
            if (n2 == levels) n2--;
            tmp = (float)(deltax) * 
                (float)(TWO_THEN_SIXTEEN - deltay) *
                (float)(deltaz);
            p12( n1, n2 ) += tmp / TWO_THEN_SIXTEEN_CUBE;

            n2ptr +=dx;
            n2 = (int) (( *n2ptr - mini2)/h2);
            if (n2 == levels) n2--;
            tmp = (float)(deltax)*
                (float)(deltay)*
                (float)(deltaz);
            p12( n1, n2 ) += tmp / TWO_THEN_SIXTEEN_CUBE;

            n2ptr--;
            n2 = (int) (( *n2ptr - mini2)/h2);
            if (n2 == levels) n2--;
            tmp = (float)(TWO_THEN_SIXTEEN - deltax) *
                (float)(deltay)*
                (float)(deltaz);
            p12( n1, n2 ) += tmp / TWO_THEN_SIXTEEN_CUBE;              
        }

    
    // Normalization
    float sum=0.0;
    int x, y;
    ForEach2d( p12, x, y )
        sum += p12( x, y );
    if (sum)
        ForEach2d( p12, x, y ) p12( x, y ) /= sum;

    // Partial pdf's
    p1 = 0.0;
    p2 = 0.0;
    for ( y = 0; y < levels; y++ )
        for ( x = 0; x < levels; x++ )
        {
            p1( x ) += p12( x, y );
            p2( y ) += p12( x, y );
        }
}

void	AimsGeneralizedKnnParzenPdf(aims::knn::Database &db,
                AimsData<float> &pdf, unsigned int k)
{
    int        x, y, z;
    double        dx, dy, dz;
    double        h, sum, val, dist;
    int        dim = db.dim();
    std::vector<double>    vec(dim);
    aims::knn::KnnGlobalFriedman  knn(db, k);
    std::pair<std::vector<unsigned int>, std::vector<double> > res;
    knn.precompute();

    pdf = 0.;
    sum = 0.;

    ForEach3d(pdf, x, y, z )
    {
        vec[0] = x;
        vec[1] = y;
        vec[2] = z;
        res = knn.find(vec);
        h = res.second[0];
        if (h == 0) h = 10e-10;
        const std::vector<unsigned int> &id = res.first;

        for (int i = id.size() - 1; i >= 0; --i)
        {
            const double	*d = db[id[i]];
            dx = (x - d[0]);
            dy = (y - d[1]);
            dz = (z - d[2]);
            dist = dx * dx + dy * dy + dz * dz;
            val = parzen2(dist, h, dim);
            pdf(x, y, z) += val;
            sum += val;
        }
    }
    if (sum)
        ForEach3d(pdf, x, y, z )
            pdf(x, y, z) /= sum;
}

void  AimsKnnPdf(aims::knn::Database &db,
      AimsData<float> &pdf, unsigned int k)
{
    int        x, y, z;
    double        h, sum, val;
    int        dim = db.dim();
    std::vector<double>    vec(dim);
    aims::knn::KnnGlobalFriedman  knn(db, k);
    std::pair<std::vector<unsigned int>, std::vector<double> > res;
    knn.precompute();

    pdf = 0.;
    sum = 0.;

    ForEach3d(pdf, x, y, z )
    {
        vec[0] = x;
        vec[1] = y;
        vec[2] = z;
        res = knn.find(vec);
        h = res.second[0];
        if (h == 0) h = 10e-10;
        h = pow(h, dim); // proportional to volume
        val = k / h;
        pdf(x, y, z) = val;
        sum += val;
    }
    
    if (sum)
        ForEach3d(pdf, x, y, z )
            pdf(x, y, z) /= sum;
}


// compilation of some Volume classes on Aims types

#include <cartodata/volume/volume_d.h>

template class carto::Volume<PVItem>;
template class carto::VolumeProxy<PVItem>;
template class carto::VolumeRef<PVItem>;
template class carto::Creator<carto::Volume< PVItem> >;

