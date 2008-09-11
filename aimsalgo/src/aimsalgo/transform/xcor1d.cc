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


#include <aims/transform/fft1d.h>
#include <aims/data/data.h>
#include <aims/math/mathelem.h>
#include <math.h>
#include <complex>

using namespace aims;
using namespace std;

AimsData<float> AimsXCor1d( const AimsData<float>& data1,
                            const AimsData<float>& data2 )
{
  ASSERT( data1.dimY() == 1 && data1.dimZ() == 1 && data1.dimT() == 1 &&
          hasSameDim( data2, data1 ) );

  ASSERT( data1.minimum() != data1.maximum() &&
          data2.minimum() != data2.maximum()   );

  int k;
  float mean1 = 0.0f;
  ForEach1d( data1, k )
    mean1 += data1( k );
  mean1 /= float( data1.dimX() );

  float mean2 = 0.0f;
  ForEach1d( data2, k )
    mean2 += data2( k );
  mean2 /= float( data2.dimX() );


  AimsData<cfloat> cdata1( 2 * data1.dimX() );
  cdata1 = cfloat( 0.0 );
  ForEach1d( data1, k )
    cdata1( k ) = cfloat( data1( k ) - mean1 );


  AimsData<cfloat> cdata2( 2 * data2.dimX() );
  cdata2 = cfloat( 0.0 );
  ForEach1d( data2, k )
    cdata2( k ) = cfloat( data2( k ) - mean2 );


  float sum1 = 0.0f;
  ForEach1d( cdata1, k )
    sum1 += sqr( real( cdata1( k ) ) );

  float sum2 = 0.0f;
  ForEach1d( cdata2, k )
    sum2 += sqr( real( cdata2( k ) ) );

  float sum = sqrt( sum1 * sum2 );

  //cout << sum1 << endl << sum2 << endl;

  AimsData<cfloat> fftCdata1 = AimsFFT1D( cdata1 );
  AimsData<cfloat> fftCdata2 = AimsFFT1D( cdata2 );

  AimsData<cfloat> fftCXcor( cdata1.dimX() );
  ForEach1d( fftCXcor, k )
    fftCXcor( k ) = fftCdata1( k ) * conj( fftCdata2( k ) );

  AimsData<cfloat> CXcor = AimsFFT1D( fftCXcor, -1 );

  AimsData<float> xcor( CXcor.dimX() );
  ForEach1d( xcor, k )
    xcor( k ) = real( CXcor( k ) ) / sum;

  return xcor;
}
