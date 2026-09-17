
#include <aims/math/bessel.h>
#include <math.h>

double AimsBessel0(double x,int order)
{ double tp=1.0,s;
  int  k,l;

  for (k=1;k<order;k++)
  { s=1.0;
    for (l=1;l<=k;l++) 
      s*=((x/2.0)/(double)l);
    tp += s * s;
  }
  return tp;
}
