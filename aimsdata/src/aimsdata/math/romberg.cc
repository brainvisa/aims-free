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

/*
 *  Romberg's numerical integration.
 */
#include <cstdlib>
#include <aims/math/romberg.h>
#include <aims/math/lagrange.h>
#include <math.h>

using namespace aims;
using namespace std;

void AimsTrapezeIntegration(float (*func)(float),
                            float a,float b,int n,float *s)
{ float  x,tnm,sum,del;
  int   it,j;

  if (n==1)
    *s = 0.5 * (b - a) * ((*func)(a) + (*func)(b));
  else
  { for (it=1,j=1;j<n-1;j++) it <<= 1;
    tnm = it;
    del = (b - a) / tnm;
    x   = a + 0.5 * del;
    for(sum=0.0,j=1;j<=it;j++,x+=del)
      sum += (*func)(x);
    *s = 0.5 * (*s + (b - a) * sum / tnm);
  }
}


float AimsRombergIntegration(float (*func)(float),float a,float b,
                             float eps,int jmax,int k)
{ float ss,dss;
  vector<float> s(jmax), h(jmax);

  if (a==b) return 0.0;
  
  h[0] = 1.0;
  for (int j=0;j<jmax;j++)
  { AimsTrapezeIntegration(func,a,b,j+1,&(s[j]));
    if (j+1>=k)
    {
      vector<float> tmph(k), tmps(k);
      for (int kk=0;kk<k;kk++)
      {
        tmph[kk] = h[j-k+kk+1];
        tmps[kk] = s[j-k+kk+1];
      }
      ss = AimsLagrangeInterpolation( tmph, tmps, 0.0f, &dss );
      if (fabs(dss) <= eps * fabs(ss)) return(ss);
    }
    s[j+1] = s[j];
    h[j+1] = 0.25 * h[j];
  }
  return 0.0;
}
