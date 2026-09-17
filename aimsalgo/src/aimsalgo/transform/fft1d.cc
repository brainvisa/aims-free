
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
