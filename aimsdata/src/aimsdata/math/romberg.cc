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
