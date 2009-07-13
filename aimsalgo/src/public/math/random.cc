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


#include <cstdlib>
#include <aims/math/random.h>
#include <math.h>

using namespace std;

// generate uniform random number between 0.0 and 1.0
AIMSALGOPUB_API double UniformRandom()
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
AIMSALGOPUB_API double NormalRandom()
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

AIMSALGOPUB_API complex<float> 
UniformRandom(const complex<float> &min,const complex<float> &max)
{ return(complex<float>(UniformRandom(real(min),real(max)),
                        UniformRandom(imag(min),imag(max))));
}


AIMSALGOPUB_API complex<double> 
UniformRandom(const complex<double> &min,const complex<double> &max)
{ return(complex<double>(UniformRandom(real(min),real(max)),
                         UniformRandom(imag(min),imag(max))));
}



AIMSALGOPUB_API complex<float> 
GaussianRandom(const complex<float> &mean,const complex<float> &sigma)
{ return(complex<float>(GaussianRandom(real(mean),real(sigma)),
                        GaussianRandom(imag(mean),imag(sigma))));
}

AIMSALGOPUB_API complex<double> 
GaussianRandom(const complex<double> &mean,const complex<double> &sigma)
{ return(complex<double>(GaussianRandom(real(mean),real(sigma)),
                         GaussianRandom(imag(mean),imag(sigma))));
}
