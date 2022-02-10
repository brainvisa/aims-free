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


// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/information/pdf.h>
#include <aims/math/mathelem.h>
#include <aims/bucket/bucketMap.h>
#include <cartodata/volume/volume.h>
#include <iostream>
#include <list>
#include <math.h>

using namespace std;
using namespace aims;
using namespace carto;

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





void AimsParzenJointPdf( const rc_ptr<Volume<short> >& data1,
                         const rc_ptr<Volume<short> >& data2,
                         rc_ptr<Volume<float> >& p12,
                         rc_ptr<Volume<float> >& p1,
                         rc_ptr<Volume<float> >& p2 )
{
    int levels = p12->getSizeX();

    ASSERT( p12->getSizeY() == levels && p1->getSizeX() == levels && p2->getSizeX() == levels );

    float mini1 = float( data1->min() );
    float maxi1 = float( data1->max() );
    ASSERT( maxi1 != mini1 );

    float mini2 = float( data2->min() );
    float maxi2 = float( data2->max() );
    if ( maxi2 == mini2 )
    {
        cerr << "Warning ! AimsParzenJointPdf : maxi2 == mini2" << endl;
        return;
    }

    float h1 = ( maxi1 - mini1 ) / levels;
    float h2 = ( maxi2 - mini2 ) / levels;

    int i, j, k, n1, n2;
    vector<int> dim = data1->getSize();
    vector<int> p12dim = p12->getSize();

    *p12 = 0.0;

    for( k=0; k<dim[2]; ++k )
      for( j=0; j<dim[1]; ++j )
        for( i=0; i<dim[0]; ++i )
        {
          for( n2=0; n2<p12dim[1]; ++n2 )
            for( n1=0; n1<p12dim[0]; ++n1 )
            {
              p12->at( n1, n2 ) += parzen( mini1 + ( n1 + 0.5 ) * h1 -
                                           float( data1->at( i, j, k ) ), h1 )
                                   * parzen( mini2 + ( n2 + 0.5 ) * h2 -
                                             float( data2->at( i, j, k ) ), h2 );
            }
        }

    float sum=0.0;
    int x, y;
    for( y=0; y<p12dim[1]; ++y )
      for( x=0; x<p12dim[0]; ++x )
        sum += p12->at( x, y );

    if (sum)
      for( y=0; y<p12dim[1]; ++y )
        for( x=0; x<p12dim[0]; ++x )
          p12->at( x, y ) /= sum;

    *p1 = 0.0;
    *p2 = 0.0;
    for ( y = 0; y < levels; y++ )
      for ( x = 0; x < levels; x++ )
      {
        p1->at( x ) += p12->at( x, y );
        p2->at( y ) += p12->at( x, y );
      }
}


void AimsParzenPdf(const rc_ptr<Volume<short> >& data,
                   rc_ptr<Volume<float> >& p)
{
    int levels = p->getSizeX();

    float mini = float( data->min() );
    float maxi = float( data->max() );
    ASSERT( maxi != mini );

    float h = ( maxi - mini ) / levels;

    int i, j, k, n;
    vector<int> dim = data->getSize();
    int nd = p->getSizeX();

    *p = 0.0;

    for( k=0; k<dim[2]; ++k )
      for( j=0; j<dim[1]; ++j )
        for( i=0; i<dim[0]; ++i )
        {
          for( n=0; n<nd; ++n )
            p->at( n ) += parzen(mini + ( n + 0.5 ) * h
                             - float( data->at( i, j, k ) ), h);
        }

    float sum=0.0;
    int x;
    for( x=0; x<nd; ++x )
        sum += p->at( x );

    if (sum)
      for( x=0; x<nd; ++x )
        p->at( x ) /= sum;
}


void AimsWinParzenJointPdf(const rc_ptr<Volume<short> >& data1,
                           const rc_ptr<Volume<short> >& data2,
                           rc_ptr<Volume<float> >& p12,
                           rc_ptr<Volume<float> >& p1,
                           rc_ptr<Volume<float> >& p2,
                           const rc_ptr<Volume<float> >& mask)
{
    int levels = p12->getSizeX();

    ASSERT( p12->getSizeY() == levels && p1->getSizeX() == levels
            && p2->getSizeX() == levels );

    float mini1 = float( data1->min() );
    float maxi1 = float( data1->max() );
    ASSERT(maxi1 != mini1);

    float mini2 = float( data2->min() );
    float maxi2 = float( data2->max() );
    if ( maxi2 == mini2 )
    {
        cerr << "Warning ! AimsWinParzenJointPdf : maxi2 == mini2" << endl;
        return;
    }

    float h1 = ( maxi1 - mini1 ) / levels;
    float h2 = ( maxi2 - mini2 ) / levels;

    int sMask = mask->getSizeX();
    int sMaskdiv2 = sMask / 2;
    ASSERT( sMask % 2 == 1 );

    int i, j, k, n1, n2, v1, v2, val1, val2;
    vector<int> dim = data1->getSize();

    VolumeRef<float> mask2d( sMask, sMask );
    for ( v1 = 0; v1 < sMask; v1++ )
        for ( v2 = 0; v2 < sMask; v2++ )
            mask2d( v1, v2 ) += mask->at( v1 ) * mask->at( v2 );

    *p12 = 0.0;

    for( k=0; k<dim[2]; ++k )
      for( j=0; j<dim[1]; ++j )
        for( i=0; i<dim[0]; ++i )
        {
          n1 = int((float(data1->at(i, j, k)) - mini1) / h1);
          n2 = int((float(data2->at(i, j, k)) - mini2) / h2);
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
                  p12->at(val1, val2) += mask2d(v1, v2);
            }
        }

    float sum=0.0;
    int x, y;
    vector<int> pd = p12->getSize();
    for ( y = 0; y < pd[1]; y++ )
      for ( x = 0; x < pd[0]; x++ )
        sum += p12->at( x, y );

    if (sum)
      for ( y = 0; y < pd[1]; y++ )
        for ( x = 0; x < pd[0]; x++ )
          p12->at( x, y ) /= sum;

    *p1 = 0.0;
    *p2 = 0.0;
    for (y = 0; y < levels; y++)
      for (x = 0; x < levels; x++)
      {
        p1->at( x ) += p12->at( x, y );
        p2->at( y ) += p12->at( x, y );
      }
}


void AimsWinParzenPdf(const rc_ptr<Volume<short> >& data,
                      rc_ptr<Volume<float> >& p,
                      const rc_ptr<Volume<float> >& mask)
{
    int levels = p->getSizeX();

    float mini = float( data->min() );
    float maxi = float( data->max() );
    ASSERT( maxi != mini );

    float h = ( maxi - mini ) / levels;

    int sMask = mask->getSizeX();
    ASSERT( sMask % 2 == 1 );
    int i, j, k, n, v, val;
    vector<int> dim = data->getSize();

    *p = 0.0;

    for( k=0; k<dim[2]; ++k )
      for( j=0; j<dim[1]; ++j )
        for( i=0; i<dim[0]; ++i )
        {
          n = int( ( float( data->at( i, j, k ) ) - mini ) / h );
          if ( n == levels )
              n--;

          for ( v = 0; v < sMask; v++ )
          {
            val = n + v - sMask / 2;
            if ( val >= 0 && val < levels )
              p->at( val ) += mask->at( v );
          }
        }

    float sum=0.0;
    int x, np = p->getSizeX();

    for( x=0; x<np; ++x )
      sum += p->at(x);
    if (sum)
      for( x=0; x<np; ++x )
        p->at(x) /= sum;
}


void AimsJointPdf( const rc_ptr<Volume<short> >& data1,
                   const rc_ptr<Volume<short> >& data2,
                   rc_ptr<Volume<float> >& p12,
                   rc_ptr<Volume<float> >& p1, rc_ptr<Volume<float> >& p2 )
{
    int levels = p12->getSizeX();

    ASSERT( p12->getSizeY() == levels && p1->getSizeX() == levels
            && p2->getSizeX() == levels );

    // TODO: data1 and data2 must necessarily have the same voxel size. It would
    // probably be better to use the lower voxel sizes and field of view 
    // to determine the step and the grid to use during pdf updates
    ASSERT( data1->getVoxelSize() == data2->getVoxelSize() );

    int dx = min(data1->getSizeX(), data2->getSizeX()),
        dy = min(data1->getSizeY(), data2->getSizeY()),
        dz = min(data1->getSizeZ(), data2->getSizeZ());

    float mini1 = float( data1->min() );
    float maxi1 = float( data1->max() );
    float maxi2 = float( data2->max() );
    float mini2 = float( data2->min() );

    double h1 = (( mini1 == maxi1) ? 1 : ( maxi1 - mini1 ) / levels);
    double h2 = (( mini2 == maxi2) ? 1 : ( maxi2 - mini2 ) / levels);

    int i, j, k, n1, n2;

    *p12 = 0.0;

    for(k = 0; k < dz; ++k)
        for(j = 0; j < dy; ++j)
            for(i = 0; i < dx; ++i)
            {
                n1 = int( ( float( data1->at( i, j, k ) ) - mini1 ) / h1 );
                n2 = int( ( float( data2->at( i, j, k ) ) - mini2 ) / h2 );
                if ( n1 == levels )
                    n1--;
                if ( n2 == levels )
                    n2--;

                p12->at( n1, n2 )++;
            }

    float sum=0.0;
    int x, y;
    vector<int> pd = p12->getSize();

    for (y = 0; y<pd[1]; y++)
      for (x = 0; x<pd[0]; x++)
        sum += p12->at( x, y );

    if (sum)
      for (y = 0; y<pd[1]; y++)
        for (x = 0; x<pd[0]; x++)
          p12->at( x, y ) /= sum;

    *p1 = 0.0;
    *p2 = 0.0;
    for ( y = 0; y < levels; y++ )
      for ( x = 0; x < levels; x++ )
        {
            // Due to floatting point arithmetic, probability
            // can be a little bit over 1. Min function is used
            // to fix the issue
            p1->at(x) = std::min((double)p1->at(x) + p12->at( x, y ),
                                 (double)1.0);
            p2->at(y) = std::min((double)p2->at(y) + p12->at( x, y ),
                                 (double)1.0);
        }
}



void AimsPdf( const rc_ptr<Volume<short> >& data, rc_ptr<Volume<float> >& p )
{
    int levels = p->getSizeX();


    float mini = float( data->min() );
    float maxi = float( data->max() );
    ASSERT( maxi != mini );

    float h = ( maxi - mini ) / levels;

    int i, j, k, n;
    vector<int> dim = data->getSize();

    *p = 0.0;

    for( k=0; k<dim[2]; ++k )
      for( j=0; j<dim[1]; ++j )
        for( i=0; i<dim[0]; ++i )
        {
          n = int((float(data->at(i, j, k )) - mini) / h);
          if (n == levels)
              n--;

          p->at(n)++;
        }

    float sum=0.0;
    int x, np=p->getSizeX();
    for( x=0; x<np; ++x )
      sum += p->at( x );

    if (sum) 
      for( x=0; x<np; ++x )
        p->at( x ) /= sum;
}


static float minMask(const rc_ptr<Volume<short> >& d)
{
    short mini;
    line_NDIterator<short> it( &d->at( 0 ), d->getSize(), d->getStrides() );
    short *t, *tt;

    mini = 32767;
    for( ; !it.ended(); ++it )
    {
      t = &*it;
      for( tt=t + it.line_length(); t!=tt; it.inc_line_ptr( t ) )
        if ( (*t < mini)  && (*t != -32768))
          mini = *t;
    }

    return ( (float)mini );
}


void AimsJointMaskPdf(const rc_ptr<Volume<short> >& data1,
                      const rc_ptr<Volume<short> >& data2,
                      rc_ptr<Volume<float> >& p12,
                      rc_ptr<Volume<float> >& p1,
                      rc_ptr<Volume<float> >& p2 )
{
    int levels = p12->getSizeX();

    ASSERT( p12->getSizeY() == levels && p1->getSizeX() == levels
            && p2->getSizeX() == levels );

    // TODO: data1 and data2 must necessarily have the same voxel size. It would
    // probably be better to use the lower voxel sizes and field of view 
    // to determine the step and the grid to use during pdf updates
    ASSERT( data1->getVoxelSize() == data2->getVoxelSize() );

    int dx = min(data1->getSizeX(), data2->getSizeX()),
        dy = min(data1->getSizeY(), data2->getSizeY()),
        dz = min(data1->getSizeZ(), data2->getSizeZ());

    //  float mini1 = float( data1->min() );
    double mini1 = minMask( data1 );
    double maxi1 = double( data1->max() );
    double mini2 = minMask( data2 );
    double maxi2 = double( data2->max() );
 
    double h1 = (( mini1 == maxi1) ? 1 : ( maxi1 - mini1 ) / levels);
    double h2 = (( mini2 == maxi2) ? 1 : ( maxi2 - mini2 ) / levels);

    int i, j, k, n1, n2;

    *p12 = 0.0;
    #ifdef DEBUG
    cout << "DEBUG>>min1 max1 min2 max2 " << mini1 << " " << maxi1 <<" " <<mini2<<" "<<maxi2<<endl;
    #endif

    for(k = 0; k < dz; ++k)
        for(j = 0; j < dy; ++j)
            for(i = 0; i < dx; ++i)
            {
                if (data1->at(i, j, k ) != short(-32768) && data2->at(i,j,k) != short(-32768))
                {
                    n1 = int( ( double( data1->at( i, j, k ) ) - mini1 ) / h1 );
                    n2 = int( ( double( data2->at( i, j, k ) ) - mini2 ) / h2 );
                    if ( n1 == levels )
                        n1--;
                    if ( n2 == levels )
                        n2--;

                    p12->at( n1, n2 )++;
                }
            }

    double sum=0.0;
    int x, y;
    vector<int> pd = p12->getSize();
    
    for(y = 0; y<pd[1]; ++y)
      for(x = 0; x<pd[0]; ++x)
        sum += p12->at( x, y );

    if (sum)
      for(y = 0; y<pd[1]; ++y)
        for(x = 0; x<pd[0]; ++x)
          p12->at( x, y ) /= float(sum);

    *p1 = 0.0;
    *p2 = 0.0;
    for( y = 0; y < levels; y++)
        for ( x = 0; x < levels; x++ )
        {
            // Due to floatting point arithmetic, probability
            // can be a little bit over 1. Min function is used
            // to fix the issue
            p1->at(x) = std::min((double)p1->at(x) + p12->at( x, y ),
                                 (double)1.0);
            p2->at(y) = std::min((double)p2->at(y) + p12->at( x, y ),
                                 (double)1.0);
        }
}


void AimsJointPVPdf(const rc_ptr<Volume<short> >& data1,
                    const rc_ptr<Volume<short> >& data2,
                    const rc_ptr<Volume<PVItem> >& comb,
                    rc_ptr<Volume<float> >& p12,
                    rc_ptr<Volume<float> >& p1,
                    rc_ptr<Volume<float> >& p2 )
{
    int levels = p12->getSizeX();
    const int  TWO_THEN_SIXTEEN  = 65536;
    const float TWO_THEN_SIXTEEN_CUBE = 65536.0 * 65536.0 * 65536.0;

    ASSERT( p12->getSizeY() == levels &&
        p1->getSizeX() == levels &&
        p2->getSizeX() == levels );
    
    //   float mini1 = float( data1->min() );
    float mini1 = minMask( data1 );
    float maxi1 = float( data1->max() );
    ASSERT( maxi1 != mini1 );

    //   float mini2 = float( data2.minimum() );
    float mini2 = minMask( data2 );
    float maxi2 = float( data2->max() );
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

    *p12 = 0.0;

    const_NDIterator<PVItem> it( &comb->at( 0 ), comb->getSize(),
                                 comb->getStrides() );
    const_NDIterator<short>  it1( &data1->at( 0 ), data1->getSize(),
                                  data1->getStrides() );
    vector<unsigned long> n2str = data2->getStrides();
    short *n2ptr;

    float                    tmp;

//     cout << endl;

    int pvf = 0;
    for( ; !it.ended(); ++it, ++it1, pvf++)
        if ( it->offset != -1L && *it1 != -32768)
        {
            n1 = (int) ((*it1 - mini1)/h1);
            if (n1 == levels) n1--;

            n2ptr =  &data2->at(0) + it->offset;

            n2 = (int) (( *n2ptr - mini2 ) / h2 );
            if (n2 == levels) n2--;
            tmp = (float)(TWO_THEN_SIXTEEN - it->x) *
                (float)(TWO_THEN_SIXTEEN - it->y) *
                (float)(TWO_THEN_SIXTEEN - it->z);
            p12->at( n1, n2 ) += tmp / TWO_THEN_SIXTEEN_CUBE;


            n2ptr += n2str[0];
            n2 = (int) (( *n2ptr - mini2)/h2);
            if (n2 == levels) n2--;
            tmp = (float)(it->x) *
                (float)(TWO_THEN_SIXTEEN - it->y) *
                (float)(TWO_THEN_SIXTEEN - it->z);
            p12->at( n1, n2 ) += tmp / TWO_THEN_SIXTEEN_CUBE;


            n2ptr += n2str[1];
            n2 = (int) (( *n2ptr - mini2)/h2);
            if (n2 == levels) n2--;
            tmp = (float)(it->x) *
                (float)(it->y) *
                (float)(TWO_THEN_SIXTEEN - it->z);
            p12->at( n1, n2 ) += tmp / TWO_THEN_SIXTEEN_CUBE;


            n2ptr -= n2str[0];
            n2 = (int) (( *n2ptr - mini2)/h2);
            if (n2 == levels) n2--;
            tmp = (float)(TWO_THEN_SIXTEEN - it->x) *
                (float)(it->y) *
                (float)(TWO_THEN_SIXTEEN - it->z);
            p12->at( n1, n2 ) += tmp / TWO_THEN_SIXTEEN_CUBE;

            n2ptr += n2str[2] - n2str[1];
            n2 = (int) (( *n2ptr - mini2)/h2);
            if (n2 == levels) n2--;
            tmp = (float)(TWO_THEN_SIXTEEN - it->x) *
                (float)(TWO_THEN_SIXTEEN - it->y)*
                (float)(it->z);
            p12->at( n1, n2 ) += tmp / TWO_THEN_SIXTEEN_CUBE;

            n2ptr += n2str[0];
            n2 = (int) (( *n2ptr - mini2)/h2);
            if (n2 == levels) n2--;
            tmp = (float)(it->x) *
                (float)(TWO_THEN_SIXTEEN - it->y) *
                (float)(it->z);
            p12->at( n1, n2 ) += tmp / TWO_THEN_SIXTEEN_CUBE;

            n2ptr += n2str[1];
            n2 = (int) (( *n2ptr - mini2)/h2);
            if (n2 == levels) n2--;
            tmp = (float)(it->x)*
                (float)(it->y)*
                (float)(it->z);
            p12->at( n1, n2 ) += tmp / TWO_THEN_SIXTEEN_CUBE;

            n2ptr -= n2str[0];
            n2 = (int) (( *n2ptr - mini2)/h2);
            if (n2 == levels) n2--;
            tmp = (float)(TWO_THEN_SIXTEEN - it->x) *
                (float)(it->y)*
                (float)(it->z);
            p12->at( n1, n2 ) += tmp / TWO_THEN_SIXTEEN_CUBE;
        }

    // Normalization
    float sum = carto::sum( p12 );

    if (sum)
      *p12 /= sum;

    // Partial pdf's
    *p1 = 0.0;
    *p2 = 0.0;
    int x, y;
    for ( y = 0; y < levels; y++ )
        for ( x = 0; x < levels; x++ )
        {
            p1->at( x ) += p12->at( x, y );
            p2->at( y ) += p12->at( x, y );
        }
}

void AimsJointPVPdf(const aims::BucketMap<short>&               data1,
                    const rc_ptr<Volume<short> >&               data2,
                    const vector< PVVectorItem >& comb,
                    rc_ptr<Volume<float> >& p12,
                    rc_ptr<Volume<float> >& p1,
                    rc_ptr<Volume<float> >& p2 )
{
    const int TWO_THEN_SIXTEEN = 65536;
    const float TWO_THEN_SIXTEEN_CUBE = 65536.0 * 65536.0 * 65536.0;
    int levels = p12->getSizeX();

    ASSERT(p12->getSizeY() == levels &&
           p1->getSizeX() == levels &&
           p2->getSizeX() == levels);
    
    BucketMap<short>::Bucket::const_iterator itb, itblast= data1.begin()->second.end() ;
    
    float maxi1 = data1.begin()->second.begin()->second ;
    float mini1 = data1.begin()->second.begin()->second ;
    for(itb=data1.begin()->second.begin(); itb != itblast; ++itb)
        {
            if (maxi1 < itb->second ) maxi1 = itb->second;
            if (mini1 > itb->second ) mini1 = itb->second;
        }


    float maxi2 = data2->max();
    float mini2 = minMask( data2 );


    
    if ( (maxi2 == mini2) ||  (maxi1 == mini1) )
        {
            cerr << "Warning ! AimsJointPdf : maxiData == miniData" << endl;
            return;
        }

    float h1 = ( maxi1 - mini1 ) / levels;
    float h2 = ( maxi2 - mini2 ) / levels;


    int  n1, n2;
    *p12 = 0.0;

    float                            tmp;

    int i;
    for(itb=data1.begin()->second.begin(), i=0; itb != itblast; ++itb, ++i)
        if ( (comb[i]).offset !=  -1L )
        {
            unsigned short deltax = (comb[i]).x;
            unsigned short deltay = (comb[i]).y;
            unsigned short deltaz = (comb[i]).z;

            n1 = (int) ((itb->second - mini1)/h1);
            if (n1 == levels) n1--;

            vector<int> pos( 3 );
            pos[0] = comb[i].x;
            pos[1] = comb[i].y;
            pos[2] = comb[i].z;
            n2 = (int) (( data2->at( pos ) - mini2)/h2);
            if (n2 == levels) n2--;     
            tmp = (float)(TWO_THEN_SIXTEEN - deltax) *
                (float)(TWO_THEN_SIXTEEN - deltay) *
                (float)(TWO_THEN_SIXTEEN - deltaz);
            p12->at( n1, n2 ) += tmp / TWO_THEN_SIXTEEN_CUBE;


            ++pos[0];
            n2 = (int) (( data2->at( pos ) - mini2)/h2);
            if (n2 == levels) n2--;
            tmp = (float)(deltax) *
                (float)(TWO_THEN_SIXTEEN - deltay) *
                (float)(TWO_THEN_SIXTEEN - deltaz);
            p12->at( n1, n2 ) += tmp / TWO_THEN_SIXTEEN_CUBE;


            ++pos[1];
            n2 = (int) (( data2->at( pos ) - mini2)/h2);
            if (n2 == levels) n2--;
            tmp = (float)(deltax) *
                (float)(deltay) *
                (float)(TWO_THEN_SIXTEEN - deltaz);
            p12->at( n1, n2 ) += tmp / TWO_THEN_SIXTEEN_CUBE;


            --pos[0];
            n2 = (int) (( data2->at( pos ) - mini2)/h2);
            if (n2 == levels) n2--;
            tmp = (float)(TWO_THEN_SIXTEEN - deltax) *
                (float)(deltay) *
                (float)(TWO_THEN_SIXTEEN - deltaz);
            p12->at( n1, n2 ) += tmp / TWO_THEN_SIXTEEN_CUBE;

            ++pos[2];
            --pos[1];
            n2 = (int) (( data2->at( pos ) - mini2)/h2);
            if (n2 == levels) n2--;
            tmp = (float)(TWO_THEN_SIXTEEN - deltax) *
                (float)(TWO_THEN_SIXTEEN - deltay)*
                (float)(deltaz);
            p12->at( n1, n2 ) += tmp / TWO_THEN_SIXTEEN_CUBE;

            ++pos[0];
            n2 = (int) (( data2->at( pos ) - mini2)/h2);
            if (n2 == levels) n2--;
            tmp = (float)(deltax) * 
                (float)(TWO_THEN_SIXTEEN - deltay) *
                (float)(deltaz);
            p12->at( n1, n2 ) += tmp / TWO_THEN_SIXTEEN_CUBE;

            ++pos[1];
            n2 = (int) (( data2->at( pos ) - mini2)/h2);
            if (n2 == levels) n2--;
            tmp = (float)(deltax)*
                (float)(deltay)*
                (float)(deltaz);
            p12->at( n1, n2 ) += tmp / TWO_THEN_SIXTEEN_CUBE;

            --pos[0];
            n2 = (int) (( data2->at( pos ) - mini2)/h2);
            if (n2 == levels) n2--;
            tmp = (float)(TWO_THEN_SIXTEEN - deltax) *
                (float)(deltay)*
                (float)(deltaz);
            p12->at( n1, n2 ) += tmp / TWO_THEN_SIXTEEN_CUBE;
        }

    
    // Normalization
    float sum = carto::sum( p12 );
    int x, y;
    if (sum)
      *p12 /= sum;

    // Partial pdf's
    *p1 = 0.0;
    *p2 = 0.0;
    for ( y = 0; y < levels; y++ )
        for ( x = 0; x < levels; x++ )
        {
            p1->at( x ) += p12->at( x, y );
            p2->at( y ) += p12->at( x, y );
        }
}

void AimsGeneralizedKnnParzenPdf( aims::knn::Database &db,
                                  rc_ptr<Volume<float> > &pdf, unsigned int k )
{
    int        x, y, z;
    double        dx, dy, dz;
    double        h, sum, val, dist;
    int        dim = db.dim();
    std::vector<double>    vec(dim);
    aims::knn::KnnGlobalFriedman  knn(db, k);
    std::pair<std::vector<unsigned int>, std::vector<double> > res;
    knn.precompute();

    *pdf = 0.;
    sum = 0.;

    vector<int> pd = pdf->getSize();

    for ( z=0; z<pd[2]; ++z )
      for ( y=0; y<pd[1]; ++y )
        for ( x=0; x<pd[0]; ++x )
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
              pdf->at(x, y, z) += val;
              sum += val;
          }
        }
    if (sum)
      *pdf /= sum;
}

void  AimsKnnPdf(aims::knn::Database &db,
      rc_ptr<Volume<float> > &pdf, unsigned int k)
{
    int        x, y, z;
    double        h, sum, val;
    int        dim = db.dim();
    std::vector<double>    vec(dim);
    aims::knn::KnnGlobalFriedman  knn(db, k);
    std::pair<std::vector<unsigned int>, std::vector<double> > res;
    knn.precompute();

    *pdf = 0.;
    sum = 0.;

    vector<int> pd = pdf->getSize();

    for ( z=0; z<pd[2]; ++z )
      for ( y=0; y<pd[1]; ++y )
        for ( x=0; x<pd[0]; ++x )
        {
          vec[0] = x;
          vec[1] = y;
          vec[2] = z;
          res = knn.find(vec);
          h = res.second[0];
          if (h == 0) h = 10e-10;
          h = pow(h, dim); // proportional to volume
          val = k / h;
          pdf->at(x, y, z) = val;
          sum += val;
        }
    
    if (sum)
      *pdf /= sum;
}


// compilation of some Volume classes on Aims types

#include <cartodata/volume/volume_d.h>

template class carto::Volume<PVItem>;
template class carto::VolumeProxy<PVItem>;
template class carto::VolumeRef<PVItem>;
template class carto::Creator<carto::Volume< PVItem> >;

