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
