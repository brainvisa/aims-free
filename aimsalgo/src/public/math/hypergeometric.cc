/*
MHG is a collection of MATLAB functions written in C for computing the
hypergeometric function of a matrix argument.

MHG is Copyright (c) 2004 Plamen Koev

This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2 of the License, or (at your
option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

If you use MHG in any program or publication, please acknowledge
its author by adding a reference to:

Plamen Koev and Alan Edelman, The Efficient Evaluation of the
Hypergeometric Function of a Matrix Argument, Mathematics of Computation
75 (2006), 833-846.


  ref :

  http://www-math.mit.edu/~plamen/files/cdekk.pdf
  http://www-math.mit.edu/~plamen/software/mhgref.html
*/

#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include <aims/math/hypergeometric.h>


double hyp0f1(double a, double *x, int n, int MAX)
{
	return hypergeometric(NULL, 0, &a, 1, x, NULL, n, 2, MAX);
}

double hyp1f1(double a, double b, double *x, int n, int MAX)
{
	return hypergeometric(&a, 1, &b, 1, x, NULL, n, 2, MAX);
}

/**
    p :     real array
    np :    size of p
    q :     real array
    nq :    size of q
    x :     real array
    y :     real array (optional)
    n :     size of x and y
    alpha : positive real, 2 for real matrices
    MAX :   number of iteration (10 : good default value)
 */
double	hypergeometric(double *p, int np, double *q, int nq,
	double *x, double *y, int n, double alpha, int MAX)
{ 
   int    h,sl,nmu,i,j,k,*f;
   double *Sx, *xn, *prodx;
   double *Sy, *yn, *prody;
   double c, *z, *coef, *kt, *mt, *blm, zn, dn,
          t, q1, q2, cc, output = 0.;

   int lg, *D, heap, *l, w, *mu, *d, *g, *ww, *lmd,
        slm, nhstrip, gz;
   
  coef=(double*)malloc(sizeof(double)*(MAX+1)); 
  
  for (i=1;i<=MAX;i++) coef[i]=0;  /* set to zero, these are the coefficients of the polynomial */ 
  coef[0]=1; /* free term equals one */
  
  w = 0; /* index of the zero partition, currently l*/

  /* figure out the number of partitions |kappa|<= MAX with at most n parts */

  f=(int*) calloc (MAX+2,sizeof(int));
  for (i=1;i<=MAX+1;i++) f[i]=i;
	
  for (i=2;i<n;i++) for (j=i+1;j<=MAX+1;j++) f[j]+=f[j-i];

  w=f[MAX+1];
  free(f);
  
  D     = (int*) calloc(w+1,sizeof(int));
  Sx    = (double*) calloc(n*(w+1),sizeof(double));
  xn    = (double*) malloc(sizeof(double)*(n+1)*(MAX+2));
  prodx = (double*) malloc(sizeof(double)*(n+1));
  prodx[1]=x[0];
  for (i=2;i<=n;i++) prodx[i]=prodx[i-1]*x[i-1];
  for (i=1; i<=n; i++) {
    Sx[n+i-1]=1;
    xn[(MAX+2)*i+1]=1;
    for (j=2;j<=MAX+1;j++) xn[(MAX+2)*i+j]=xn[(MAX+2)*i+j-1]*x[i-1];
  }

  if (y!=NULL) {
     Sy    = (double*) calloc(n*(w+1),sizeof(double));
     yn    = (double*) malloc(sizeof(double)*(n+1)*(MAX+2));
     prody = (double*) malloc(sizeof(double)*(n+1));
     prody[1]=y[0];
     for (i=2;i<=n;i++) prody[i]=prody[i-1]*y[i-1];
     
     for (i=1; i<=n; i++) {
        Sy[n+i-1]=1;
        yn[(MAX+2)*i+1]=1;
        for (j=2;j<=MAX+1;j++) yn[(MAX+2)*i+j]=yn[(MAX+2)*i+j-1]*y[i-1];
     }
  }
  
  l     = (int*)calloc(n+1,sizeof(int));
  
  
  l[0]=MAX; 
  /* this is what limits l[1] by the second element of MAX if needed and 
     allows for the check l[i]<l[i-1] to be OK even for i=1 */
  
  z     = (double*)malloc((n+1) * sizeof(double));
  for (i=1;i<=n;i++) z[i]=1;
  
  mu    = (int*)calloc(n+1,sizeof(int));
  kt    = (double*)calloc(n+1,sizeof(double));
  for (i=1;i<=n;i++) kt[i]=-i;
  
  ww    = (int*)malloc((n+1) * sizeof(int));
  for (i=1;i<=n;i++) ww[i]=1;
  
  d     = (int*)calloc(n,sizeof(int));
  g     = (int*)calloc(n+1,sizeof(int));
  mt    = (double*)calloc(n+1,sizeof(double));
  blm   = (double*)calloc(n+1,sizeof(double));
  lmd   = (int*) calloc(n+1,sizeof(int));
  
  heap  = MAX+2;
  cc=1;
  h=1;
  sl=1;  /* sl= sum(l) */
  
  while (h>0) {
      if ((l[h]<l[h-1]) && (MAX>=sl) && (z[h]!=0)) {
          
          l[h]++;
          
          if ((l[h]==1) && (h>1) && (h<n)) {
              D[ww[h]]=heap;
              ww[h]=heap;
              k=MAX-sl+l[h];
              if (k>l[h-1]) k=l[h-1];
              heap+=k;
          }
          else ww[h]++;
          w=ww[h];
              
          /* Update Q */
          c=(1-h)/alpha+l[h]-1;
          zn=alpha;
          dn=kt[h]+h+1;
          for (j=0;j<np;j++)  zn*=p[j]+c;
          for (j=0;j<nq;j++)  dn*=q[j]+c;
          if (y!=NULL) {
              zn*=alpha*l[h];
              dn*=n+alpha*c;
              for (j=1;j<h;j++) {
                  t=kt[j]-kt[h];
                  zn*=t;
                  dn*=t-1;
              }
              zn/=dn;
              dn=1; /* trying to prevent overflow */
          }
          kt[h]+=alpha;
          for (j=1;j<h;j++) {
              t=kt[j]-kt[h];
              zn*=t;
              dn*=t+1;
          }
          z[h]*=zn/dn;
          
          /* Working hard only when l has less than n parts */
          
          
          if (h<n) {
              t=h+1-alpha; cc=1; for (j=1; j<=h;j++) cc*=(t+kt[j])/(h+kt[j]); 
              
              /* computing the index of l-ones(1,h) */
              nmu=l[1]; k=2; while ((k<=h)&&(l[k]>1)) nmu=D[nmu]+l[k++]-2;
                
              Sx[w*n+h-1]=cc*prodx[h]*Sx[nmu*n+h-1];

              if (y!=NULL) Sy[w*n+h-1]=cc*prody[h]*Sy[nmu*n+h-1];
              cc=1; /* this way we can just update from 1 in the h=n case*/

              d[h-1]--; /* technically this has to execute only when h>1 
                           but is OK if it is always executed; d[0] will 
                           end up being -MAX at the end of the code */

              d[h]=l[h];  /* for (k=1;k<h;k++) d[k]=l[k]-l[k+1]; 
                             this happens automatically now via updates */
              
              lg=0; for (k=1;k<=h;k++) if (d[k]>0) {lg++; g[lg]=k;}
              slm=1; /* this is sum(l-mu) */
              nhstrip=1; for (k=1;k<=lg;k++) nhstrip*=d[g[k]]+1; nhstrip--;

              memcpy(&mu[1],&l[1],sizeof(int)*h);
              memcpy(&mt[1],&kt[1],sizeof(double)*h);
              for (k=1;k<=lg;k++) { blm[k]=1; lmd[k]=l[g[k]]-d[g[k]]; }
              
              for (i=1;i<=nhstrip;i++) {
                  j=lg;
                  gz=g[lg];
                  while (mu[gz]==lmd[j]) {
                      mu[gz]=l[gz];
                      mt[gz]=kt[gz];
                      slm-=d[gz];
                      j--;
                      gz=g[j];
                  }
                  t=kt[gz]-mt[gz];
                  
                  zn=1+t;
                  dn=t+alpha;
                  for (k=1; k<gz; k++) {
                      q1=mt[k]-mt[gz];
                      q2=kt[k]-mt[gz];
                      zn*=(alpha-1+q1)*(1+q2);
                      dn*=q1*(alpha+q2); 
                  }
                  blm[j]*=zn/dn;
                                    
                  mu[gz]--;
                  mt[gz]-=alpha;
                  slm++;

                  for (k=j+1;k<=lg;k++) blm[k]=blm[j];    
                  
                  /* next, find the index of mu */
                  nmu=mu[1]+1; for (k=2;k<=h-(mu[h]==0);k++) nmu=D[nmu]+mu[k]-1;
                  
                  for (k=h+1; k<=n;k++) 
                      Sx[w*n+k-1]+=blm[j]*Sx[nmu*n+k-2]*xn[k*(MAX+2)+slm];
                  
                  if (y!=NULL) for (k=h+1; k<=n;k++)
                      Sy[w*n+k-1]+=blm[j]*Sy[nmu*n+k-2]*yn[k*(MAX+2)+slm];
             }
             
             for (k=h; k<n; k++) Sx[w*n+k]+=Sx[w*n+k-1];
             if (y!=NULL) {
                  for (k=h; k<n; k++) Sy[w*n+k]+=Sy[w*n+k-1];
                  coef[sl]+=z[h]*Sx[w*n+n-1]*Sy[w*n+n-1];
             }
             else coef[sl]+=z[h]*Sx[w*n+n-1];

          } /* of "if h<n" */
          else {
              /* computing the index of the partition l-l[n]*ones(1,n) */
              nmu=l[1]-l[n]+1;
              k=2; while ((k<n)&&(l[k]>l[n])) nmu=D[nmu]+l[k++]-1-l[n];
              /* cc is 1 if l[n]==1, (guaranteed by the h<n case); 
                 we then update from the previous */ 
            
              if (y!=NULL) {
                  t=(1/alpha+l[n]-1)/l[n];
                  for (k=1;k<n;k++) t*=(1+kt[k]-kt[n])/(alpha+kt[k]-kt[n]);
                  cc*=t*t*prodx[n]*prody[n];
                  coef[sl]+=z[n]*cc*Sx[nmu*n+n-1]*Sy[nmu*n+n-1];
              }
              else {
                  cc*=(1/alpha+l[n]-1)*prodx[n]/l[n];
                  for (k=1;k<n;k++) cc*=(1+kt[k]-kt[n])/(alpha+kt[k]-kt[n]);
                  coef[sl]+=z[n]*cc*Sx[nmu*n+n-1];
              }
          }
          if (h<n) {
              z[h+1]=z[h];
              h++;
              ww[h]=w;
          }
          sl++;
      }
      else { 
          sl-=l[h];
          l[h]=0;
          kt[h]=-h;
          h--;
      }
  } /* of while h>0 */
  
  for (i=0;i<MAX+1;i++) output += coef[i];  

  free(lmd);
  free(blm);
  free(mt);
  free(g);
  free(d);
  free(ww);
  free(kt);
  free(mu);
  free(z);
  free(l);
  if (y!=NULL) { free(prody); free(yn); free(Sy); }
  free(prodx);
  free(xn);
  free(Sx);
  free(D);
  free(coef);

  return output;
}


typedef struct {
     double alpha, *xn, *yn, *p, *q, s, *jx, *jy, *z;
     int n, np, nq, MAX, *D, *U, heap, *l, *lc, w, *m, *mc, wm;
} glob;


void jack1(int k, double aux, int sumlm, glob* a) {

int  mi1,start,i,j,r,wc;
double q0,q1,q2,aux1,t,dn;

   if ((*a).lc[1]==1) start=2;
   else start=(*a).lc[1];

   if (k==0) i=1;
   else i=k;


   while (i<=(*a).mc[1]) {
      if (i<(*a).MAX) mi1=(*a).m[i]-(*a).m[i+1];
      else mi1=(*a).m[i];
      if (mi1>0) {
        t=(*a).l[i]-(*a).m[i];
        aux1=aux*(1+(*a).alpha*t);
        dn=t+1;
        t=i-(*a).alpha * (*a).m[i];
        for (r=1; r<i; r++) {
           q0=t-r;
           q1=q0+(*a).alpha * ((*a).l[r]+1);
           q2=q0+(*a).alpha * (*a).m[r];
           aux1*=(q1+1-(*a).alpha)*(q2+(*a).alpha);
           dn*=q1*q2;
        }

        for (r=1;r<(*a).m[i];r++) {
           q1=(*a).mc[r]-t-(*a).alpha*r;
           aux1*=(q1+(*a).alpha);
           dn*=q1;
        }
        aux1/=dn;

		wc=(*a).wm;

        (*a).mc[(*a).m[i]]--;
        (*a).m[i]--;
		(*a).wm=(*a).U[(*a).wm*(*a).MAX+i-1];

        if ((*a).m[i]>0)
           jack1(i,aux1,sumlm+1,a);
        else 
           if ((*a).jy==NULL)
              for (j=start-1;j<(*a).n;j++)     /* LOGGGG */
				  if ((*a).jx[(*a).w*(*a).n + j]==-exp(1000))
   				      (*a).jx[(*a).w*(*a).n + j]=(*a).jx[(*a).wm* (*a).n + j-1] +log(aux1) + (*a).xn[j*((*a).MAX+1) + sumlm+1];
				  else
  				      (*a).jx[(*a).w*(*a).n + j]+=
 				         log(1+exp((*a).jx[(*a).wm* (*a).n + j-1]-(*a).jx[(*a).w*(*a).n + j]) 
						     * aux1 * exp((*a).xn[j*((*a).MAX+1) + sumlm+1]));
		   else
			   for (j=start-1;j<(*a).n;j++) {
   				  if ((*a).jx[(*a).w*(*a).n + j]==-exp(1000))
   				      (*a).jx[(*a).w*(*a).n + j]=(*a).jx[(*a).wm* (*a).n + j-1] +log(aux1) + (*a).xn[j*((*a).MAX+1) + sumlm+1];
				  else
  				      (*a).jx[(*a).w*(*a).n + j]+=
 				         log(1+exp((*a).jx[(*a).wm* (*a).n + j-1]-(*a).jx[(*a).w*(*a).n + j]) 
						     * aux1 * exp((*a).xn[j*((*a).MAX+1) + sumlm+1]));

				  if ((*a).jy[(*a).w*(*a).n + j]==-exp(1000))
   				      (*a).jy[(*a).w*(*a).n + j]=(*a).jy[(*a).wm* (*a).n + j-1] +log(aux1) + (*a).yn[j*((*a).MAX+1) + sumlm+1];
				  else
  				      (*a).jy[(*a).w*(*a).n + j]+=
 				         log(1+exp((*a).jy[(*a).wm* (*a).n + j-1]-(*a).jy[(*a).w*(*a).n + j]) 
						     * aux1 * exp((*a).yn[j*((*a).MAX+1) + sumlm+1]));
			   }

        (*a).m[i]++;
        (*a).mc[(*a).m[i]]++;
		(*a).wm=wc;
      }
      i++;
   }

   if ((*a).jy==NULL)
      if (k==0)
         for (i=start-1;i<(*a).n;i++)   /* LOGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG */
			 if ((*a).jx[(*a).w * (*a).n+i]==-exp(1000))
                (*a).jx[(*a).w * (*a).n+i]=(*a).jx[(*a).w * (*a).n +i-1];
			 else
  			    (*a).jx[(*a).w * (*a).n+i]+=
			       log(1+exp((*a).jx[(*a).w * (*a).n +i-1]-(*a).jx[(*a).w * (*a).n+i]));
      else
         for (i=start-1;i<(*a).n;i++)
			 if ((*a).jx[(*a).w * (*a).n+i]==-exp(1000))
                (*a).jx[(*a).w * (*a).n+i]=(*a).jx[(*a).wm*(*a).n + i-1] + log(aux) + (*a).xn[i*((*a).MAX+1)+sumlm];
			 else
                (*a).jx[(*a).w * (*a).n+i]+= log(1+
		             exp((*a).jx[(*a).wm*(*a).n + i-1]-(*a).jx[(*a).w * (*a).n+i]) 
					    * aux * exp((*a).xn[i*((*a).MAX+1)+sumlm]));
   else
      if (k==0)
		  for (i=start-1;i<(*a).n;i++) {  /* LOGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG */
			 if ((*a).jx[(*a).w * (*a).n+i]==-exp(1000))
                (*a).jx[(*a).w * (*a).n+i]=(*a).jx[(*a).w * (*a).n +i-1];
			 else
  			    (*a).jx[(*a).w * (*a).n+i]+=
			       log(1+exp((*a).jx[(*a).w * (*a).n +i-1]-(*a).jx[(*a).w * (*a).n+i]));

			 if ((*a).jy[(*a).w * (*a).n+i]==-exp(1000))
                (*a).jy[(*a).w * (*a).n+i]=(*a).jy[(*a).w * (*a).n +i-1];
			 else
  			    (*a).jy[(*a).w * (*a).n+i]+=
			       log(1+exp((*a).jy[(*a).w * (*a).n +i-1]-(*a).jy[(*a).w * (*a).n+i]));
		  }
      else
		  for (i=start-1;i<(*a).n;i++) {
			 if ((*a).jx[(*a).w * (*a).n+i]==-exp(1000))
                (*a).jx[(*a).w * (*a).n+i]=(*a).jx[(*a).wm*(*a).n + i-1] + log(aux) + (*a).xn[i*((*a).MAX+1)+sumlm];
			 else
                (*a).jx[(*a).w * (*a).n+i]+= log(1+
		             exp((*a).jx[(*a).wm*(*a).n + i-1]-(*a).jx[(*a).w * (*a).n+i]) 
					    * aux * exp((*a).xn[i*((*a).MAX+1)+sumlm]));

			 if ((*a).jy[(*a).w * (*a).n+i]==-exp(1000))
                (*a).jy[(*a).w * (*a).n+i]=(*a).jy[(*a).wm*(*a).n + i-1] + log(aux) + (*a).yn[i*((*a).MAX+1)+sumlm];
			 else
                (*a).jy[(*a).w * (*a).n+i]+= log(1+
		             exp((*a).jy[(*a).wm*(*a).n + i-1]-(*a).jy[(*a).w * (*a).n+i]) 
					    * aux * exp((*a).yn[i*((*a).MAX+1)+sumlm]));
		  }
}


void summation(int i, int ms, glob* a) {

  int j,m,ii,mm,lj1,lj2,jj, wold;
  double zn,dn,c,d,e,f,g;

  wold=(*a).w;
  m=ms;
  if (i>1) if ((*a).l[i-1]<m) m=(*a).l[i-1];
  for (ii=1;ii<m+1;ii++) {
	 if ((ii==1)&&(i>1)) {
		 (*a).D[(*a).w]=(*a).heap;
		 (*a).w=(*a).heap;
		 (*a).heap+=m;
	 }
	 else (*a).w++;

     (*a).l[i]=ii;
     (*a).lc[ii]++;        /* update conjugate partition */

     for (j=1;j<=(*a).lc[1];j++) {
		 if (j<(*a).lc[1]) lj1=(*a).l[j+1];
		 else lj1=0;
		 
		 if ((*a).l[j]>lj1) {
			 mm=(*a).l[1];
			 if (j==1) mm--;
			 for (jj=2;jj<=(*a).lc[1];jj++) {
				 if (jj==j) lj2=(*a).l[jj]-2;
				 else lj2=(*a).l[jj]-1;
				 if (lj2>=0) mm=(*a).D[mm]+lj2;
			 }
			 (*a).U[(*a).w*(*a).MAX+j-1]=mm;
		 }
	 }

     dn=1;
	 zn=1;
     c=-(i-1)/(*a).alpha+(*a).l[i]-1;
     for (j=0;j<(*a).np;j++)  zn*=(*a).p[j]+c;
     for (j=0;j<(*a).nq;j++)  dn*=(*a).q[j]+c;

     d=(*a).l[i]*(*a).alpha-i;             /* update j_lambda  */
     for (j=1;j<(*a).l[i];j++) {
        e=d-j*(*a).alpha+(*a).lc[j];
        g=e+1;
        zn*=(g-(*a).alpha)*e;
        dn*=g*(e+(*a).alpha);
     }
     for (j=1;j<i;j++) {
        f=(*a).l[j]*(*a).alpha-j-d;
        g=f+(*a).alpha;
        e=f*g;
        zn*=e-f;
        dn*=g+e;
     }
	 (*a).z[i]*=zn/dn;

     if ((*a).lc[1]==1) {
		 (*a).jx[(*a).w*(*a).n]=
			 (*a).xn[1] + (*a).jx[((*a).w-1)*(*a).n] + log(1+(*a).alpha*((*a).l[1]-1)); /* LOGGGGG */
		 if ((*a).jy!=NULL) 
		 (*a).jy[(*a).w*(*a).n]=
			 (*a).yn[1] + (*a).jy[((*a).w-1)*(*a).n] + log(1+(*a).alpha*((*a).l[1]-1));
	 }

	 memcpy((*a).m,(*a).l,((*a).MAX+1)*sizeof(int));
	 memcpy((*a).mc,(*a).lc,((*a).MAX+1)*sizeof(int));
	 (*a).wm=(*a).w;
	 jack1(0,1,0,a);

     if ((*a).jy==NULL)  
		 (*a).s +=log(1+(*a).z[i]* exp((*a).jx[(*a).w*(*a).n+(*a).n-1]-(*a).s)); /* LOGGGGGGGGGGGGGGGG */
	 else {
		 (*a).z[i]/=((*a).n+(*a).alpha*c);
		 (*a).s +=log(1+(*a).z[i] * exp( (*a).jx[(*a).w*(*a).n+(*a).n-1] +(*a).jy[(*a).w*(*a).n+(*a).n-1] - (*a).s));
	 }
	 if ((ms>ii)&&(i<(*a).n)) {
		 (*a).z[i+1]=(*a).z[i];
	     summation(i+1,ms-ii,a);
	 }
  }
  (*a).l[i]=0;
  for (ii=1; ii<m+1; ii++) (*a).lc[ii]--;
  (*a).w = wold;
}



double loghyp0f1(double a, double *x, int n, int MAX)
{
	return loghypergeometric(NULL, 0, &a, 1, x, NULL, n, 2, MAX);
}

double loghyp1f1(double a, double b, double *x, int n, int MAX)
{
	return loghypergeometric(&a, 1, &b, 1, x, NULL, n, 2, MAX);
}

double	loghypergeometric(double *p, int np, double *q, int nq,
	double *x, double *y, int n, double alpha, int MAX)
{
  int i,j,k,*f,ss,minn;
  glob *a;

  a=(glob*)malloc(sizeof(glob));
  (*a).n=n;
  (*a).MAX=MAX;
  (*a).alpha=alpha;
  (*a).p = p;
  (*a).q = q;
  (*a).np = np;
  (*a).nq = nq;
  (*a).s = 0;   /* CHANGED FROM 1 to 0 LOGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG */
  (*a).w = 0; /* index of the zero partition, currently l*/

  /* figure out the number of partitions |kappa|<= MAX with at most n parts */

  j=MAX+1;
  f=(int*) calloc (j*j,sizeof(int));
  for (i=1;i<j;i++) f[j+i]=1;
	
  ss=j;

  for (i=2;i<MAX+1;i++) {
     if (i+1<n+1) minn=i+1;
     else minn=n+1;
     for (k=2;k<minn;k++) {
        f[k*j+i]=f[(k-1)*j+i-1]+f[k*j+i-k];
        ss+=f[k*j+i];
     }
  }

  free(f);
  i=ss;

  (*a).jx=(double*) calloc(n*i,sizeof(double));
  for (j=0;j<n*i;j++) (*a).jx[j]=-exp(1000); /* LOGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG */
  if (y!=NULL) {
	  (*a).jy=(double*) calloc(n*i,sizeof(double));
      for (j=0;j<n*i;j++) (*a).jy[j]=-exp(1000); /* LOGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG */
  }
  else (*a).jy=NULL;
  (*a).D=(int*) calloc(i,sizeof(int));
  (*a).U=(int*) calloc(MAX*i,sizeof(int));
  (*a).heap = MAX+1;


  (*a).xn=(double*) malloc(sizeof(double)*n*(MAX+1));
  for (i=0; i<n; i++) {
    (*a).jx[i]=0;              /* LOGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG */
    (*a).xn[(MAX+1)*i]=0;
    for (j=1;j<MAX+1;j++) (*a).xn[(MAX+1)*i+j]=(*a).xn[(MAX+1)*i+j-1]+log(x[i]);
  }

  if (y!=NULL) {
     (*a).yn=(double*) malloc(sizeof(double)*n*(MAX+1));
     for (i=0; i<n; i++) {
       (*a).jy[i]=0;
       (*a).yn[(MAX+1)*i]=0;
       for (j=1;j<MAX+1;j++) (*a).yn[(MAX+1)*i+j]=(*a).yn[(MAX+1)*i+j-1]+log(y[i]);
 	 }
  }

  (*a).z  =(double*)malloc((MAX+1) * sizeof(double));
  (*a).z[1]=1;
  (*a).l  =(int*)calloc(MAX+1,sizeof(int));
  (*a).lc =(int*)calloc(MAX+1,sizeof(int));
  (*a).m  =(int*)calloc(MAX+1,sizeof(int));
  (*a).mc =(int*)calloc(MAX+1,sizeof(int));

  summation(1,MAX,a);

  free((*a).mc);
  free((*a).m);
  free((*a).lc);
  free((*a).l);
  free((*a).z);
  if (y!=NULL) free((*a).jy);
  free((*a).xn);
  free((*a).U);
  free((*a).D);
  free((*a).jx);

  free(a);
  return (*a).s;
}

