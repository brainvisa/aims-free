
#include "g2dSmoother.h"
#include "mutualInfo.h"
#include <iostream>
#include <math.h>

using namespace std;

void getWindowParzenJointPdf( const short* data1, const short* data2,
                              int size,
                              float** p12, float* p1, float* p2, int levels,
                              Smoother* smoother )
{

  int p, l, l1, l2;

  int size2 = size * size;
  int levels2 = levels * levels;
  float* ptr0 = &p12[ 0 ][ 0 ];
  float* ptr = 0;

  // get extremum of data
  float mini1 = +1e38;
  float maxi1 = -1e38;
  float mini2 = +1e38;
  float maxi2 = -1e38;
  for ( p = 0; p < size2; p++ )
  {

    if ( data1[ p ] < mini1 )
      mini1 = data1[ p ];
    else if ( data1[ p ] > maxi1 )
      maxi1 = data1[ p ];

    if ( data2[ p ] < mini2 )
      mini2 = data2[ p ];
    else if ( data2[ p ] > maxi2 )
      maxi2 = data2[ p ];

  }

  if ( maxi1 == mini1 )
  {

    cerr << "Warning ! getWindowParzenJointPdf : maxi1 == mini1" << endl;
    return;

  }
  if ( maxi2 == mini2 )
  {

    cerr << "Warning ! getWindowParzenJointPdf : maxi2 == mini2" << endl;
    return;

  }

  float h1 = ( maxi1 - mini1 ) / levels;
  float h2 = ( maxi2 - mini2 ) / levels;

  for ( l = 0, ptr = ptr0; l < levels2; l++ )
    *ptr++ = 0.0;

  for ( p = 0; p < size2; p++ )
  {

    l1 = ( int )( ( ( float )data1[ p ] - mini1 ) / h1 );
    l2 = ( int )( ( ( float )data2[ p ] - mini2 ) / h2 );
    if ( l1 == levels )
      l1 --;
    if ( l2 == levels )
      l2 --;

    p12[ l1 ][ l2 ] ++;

  }

  smoother->doIt( ptr0 );

  float proba = 0.0f, sum = 0.0f;

  for ( l = 0, ptr = ptr0; l < levels2; l++ )
    if ( *ptr < 0 )
      *ptr++ = 0.0f;
    else
      sum += *ptr++;

  for ( l = 0, ptr = ptr0; l < levels2; l++ )
    *ptr++ /= sum;

  for ( l = 0; l < levels; l++ )
    p1[ l ] = p2[ l ] = 0.0f;

  for ( l2 = 0; l2 < levels; l2++ )
    for ( l1 = 0; l1 < levels; l1++ )
    {

      proba = p12[ l1 ][ l2 ];
      p1[ l1 ] += proba;
      p2[ l2 ] += proba;

    }

}
