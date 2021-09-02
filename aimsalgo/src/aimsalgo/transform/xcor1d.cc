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

#include <aims/transform/fft1d.h>
#include <cartodata/volume/volume.h>
#include <aims/math/mathelem.h>
#include <math.h>
#include <complex>

using namespace aims;
using namespace carto;
using namespace std;

VolumeRef<float> AimsXCor1d( const rc_ptr<Volume<float> > & data1,
                             const rc_ptr<Volume<float> > & data2 )
{
  ASSERT( data1->getSizeY() == 1 && data1->getSizeZ() == 1
          && data1->getSizeT() == 1 && data2->getSize() == data1->getSize() );

  ASSERT( data1->min() != data1->max() &&
          data2->min() != data2->max()   );

  int k, n;
  float mean1 = 0.0f;
  n = data1->getSizeX();
  for( k=0; k<n; ++k )
    mean1 += data1->at( k );
  mean1 /= float( data1->getSizeX() );

  float mean2 = 0.0f;
  for( k=0, n=data2->getSizeX(); k<n; ++k )
    mean2 += data2->at( k );
  mean2 /= float( data2->getSizeX() );


  VolumeRef<cfloat> cdata1( 2 * data1->getSizeX() );
  cdata1.fill( cfloat( 0.0, 0.0 ) );
  n = data1->getSizeX();

  for( k=0; k<n; ++k )
    cdata1( k ) = cfloat( data1->at( k ) - mean1 );


  VolumeRef<cfloat> cdata2( 2 * data2->getSizeX() );
  cdata2 = cfloat( 0.0 );
  n = data2->getSizeX();
  for( k=0; k<n; ++k )
    cdata2( k ) = cfloat( data2->at( k ) - mean2 );


  float sum1 = 0.0f;
  n = cdata1->getSizeX();
  for( k=0; k<n; ++k )
    sum1 += sqr( real( cdata1( k ) ) );

  float sum2 = 0.0f;
  n = cdata2->getSizeX();
  for( k=0; k<n; ++k )
    sum2 += sqr( real( cdata2( k ) ) );

  float sum = sqrt( sum1 * sum2 );

  //cout << sum1 << endl << sum2 << endl;

  VolumeRef<cfloat> fftCdata1 = AimsFFT1D( cdata1 );
  VolumeRef<cfloat> fftCdata2 = AimsFFT1D( cdata2 );

  VolumeRef<cfloat> fftCXcor( cdata1->getSizeX() );
  n = fftCXcor->getSizeX();
  for( k=0; k<n; ++k )
    fftCXcor( k ) = fftCdata1( k ) * conj( fftCdata2( k ) );

  VolumeRef<cfloat> CXcor = AimsFFT1D( fftCXcor, -1 );

  VolumeRef<float> xcor( CXcor->getSizeX() );
  n = xcor->getSizeX();
  for( k=0; k<n; ++k )
    xcor( k ) = real( CXcor( k ) ) / sum;

  return xcor;
}
