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

#include <cstdlib>
#include <aims/transform/fft1d.h>
#include <cartodata/volume/volume.h>
#include <aims/math/mathelem.h>
#include <math.h>
#include <complex>

using namespace carto;
using namespace std;

// isign = + 1 ==> direct transform
// isign = - 1 ==> inverse transform 

VolumeRef< cfloat >
AimsFFT1D(const rc_ptr<Volume< cfloat > > & vec,int isign)
{
  int n,nn,i,j,m,mmax,istep;
  float wtemp,wr,wpr,wpi,wi,theta;
  float tempr,tempi;
  VolumeRef< cfloat > fft( new Volume<cfloat>( *vec ) );

  nn = vec->getSizeX();
  n  = nn;
  j  = 1;
  for (i=1;i<n;i++)
  { if (j > i)
      swap(fft(j-1),fft(i-1));
    m = n>>1;
    while (m >= 2 && j > m)
    { j -= m;
      m >>= 1;
    }
    j += m;
  }
  mmax = 1;
  while (n > mmax)
  { istep = 2 * mmax;
    theta = 6.28318530717959 / (-(float)isign * (float)mmax * 2.0);
    wtemp = sin(0.5 * theta);
    wpr   = -2.0 * wtemp * wtemp;
    wpi   = sin(theta);
    wr    = 1.0;
    wi    = 0.0;
    for (m=0;m<mmax;m++)
    { for (i=m;i<n;i+=istep)
      { j = i + mmax;
        tempr = wr * real(fft(j)) - wi * imag(fft(j));
        tempi = wr * imag(fft(j)) + wi * real(fft(j));
        fft(j) = cfloat(real(fft(i)) - tempr,
                        imag(fft(i)) - tempi);
        fft(i) += cfloat(tempr,tempi);
      }
      wtemp = wr;
      wr    = wtemp * wpr - wi * wpi + wr;
      wi    = wi * wpr + wtemp * wpi + wi;
    }
    mmax = istep;
  }
  if (isign==-1)
    for(i=0;i<n;i++)
      fft(i) /= (float)nn;

  return fft;
}


VolumeRef<float>
AimsLinearToDecibel( const rc_ptr<Volume<float> > & vec )
{
  VolumeRef<float> dec( vec->getSizeX() );

  for (int x=0;x<vec->getSizeX();x++)
  {
    ASSERT( vec->at(x)>=0 );
    if (vec->at(x)==0)
      dec(x) = -1e38;
    else
      dec(x) = 20 * (float)log10(abs(vec->at(x)));
  }

  return dec;
}
