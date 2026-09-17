
#include <cstdlib>
#include <aims/math/random.h>
#include <math.h>

using namespace std;

// generate uniform random number between 0.0 and 1.0
double UniformRandom()
{
  static int *_idum = NULL;
  int    k;
  float   ans;
  
  if (_idum == NULL)
  { _idum = new int;
    srand(time(NULL));
    *_idum = - rand();
  }   

  *_idum ^= 123459876;
  k      = (*_idum) / 127773;
  *_idum  = 16807 * (*_idum - k * 127773) - 2836 * k;
  if (*_idum<0) *_idum += 2147483647;
  ans    = (1.0 / 2147483647) * (*_idum);
  *_idum ^= 123459876;

  return((double)ans);
}

// generate normal gaussian random number with mean=0.0,sigma=1.0
double NormalRandom()
{ static int   iset=0;
  static float gset;
  float  fac,rsq,v1,v2;
  
  if (iset==0)
  { do
    { v1  = 2.0 * UniformRandom() - 1.0;
      v2  = 2.0 * UniformRandom() - 1.0;
      rsq = v1*v1 + v2*v2;
    }
    while (rsq>=1.0 || rsq==0.0);
    fac = sqrt(-2.0 * log((double)rsq) / rsq);
    gset = v1 * fac;
    iset = 1;
    return((double)(v2*fac));
  } 
  else
  { iset = 0;
    return((double)gset);
  }
}

complex<float>
UniformRandom(const complex<float> &min,const complex<float> &max)
{ return(complex<float>(UniformRandom(real(min),real(max)),
                        UniformRandom(imag(min),imag(max))));
}


complex<double>
UniformRandom(const complex<double> &min,const complex<double> &max)
{ return(complex<double>(UniformRandom(real(min),real(max)),
                         UniformRandom(imag(min),imag(max))));
}



complex<float>
GaussianRandom(const complex<float> &mean,const complex<float> &sigma)
{ return(complex<float>(GaussianRandom(real(mean),real(sigma)),
                        GaussianRandom(imag(mean),imag(sigma))));
}

complex<double>
GaussianRandom(const complex<double> &mean,const complex<double> &sigma)
{ return(complex<double>(GaussianRandom(real(mean),real(sigma)),
                         GaussianRandom(imag(mean),imag(sigma))));
}
