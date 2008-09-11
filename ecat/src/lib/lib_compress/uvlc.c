/*
 *  Copyright (C) 1993 University of Louvain, Louvain-la-Neuve, Belgium
 *  Telecommunication Laboratory.
 *
 *  This program may be used free of charge by the members
 *  of all academic and/or scientific institutions.
 *       ANY OTHER USE IS PROHIBITED.
 *  It may also be included in any package
 *      -  provided that this text is not removed from
 *      the source code and that reference to this original
 *      work is done ;
 *      - provided that this package is itself available
 *      for use, free of charge, to the members of all
 *      academic and/or scientific institutions.
 *  Nor the author, nor the Universite Catholique de Louvain, in any
 *  way guarantee that this program will fullfill any particular
 *  requirement, nor even that its use will always be harmless.
 *
 *
*/

#include <stdio.h>

void   putin(mot,lmot,input,pin,rin)
int *mot,lmot,*pin,*rin;
FILE *input;
{
int point,nx,ny,pp,rr,nr;
 
pp=*pin;
rr=*rin;
point=pp+lmot;
if(point<=32){*mot=((1<<lmot)-1)&(rr>>pp);*pin=point;}
else{nx=((1<<(32-pp))-1)&(rr>>pp);
     fread(rin,4,1,input); 
     rr=*rin;
     nr=((1<<(point-32))-1)&rr;
     *mot=nx+(nr<<(32-pp));
     *pin=point-32;
    }
}

void itrl(r,m,input,pin,rin)
int *r,m,*pin,*rin;
FILE *input;
{
int rl,nx;
rl=0;
putin(&nx,1,input,pin,rin);
while(nx==0){
rl+=1;
putin(&nx,1,input,pin,rin);
}
nx=0;
putin(&nx,m,input,pin,rin);
*r=(rl<<m)+nx;
}

void putout(mot,lmot,output,pout,rout)
int mot,lmot,*pout,*rout;
FILE *output;
{
int point,nx;

if(lmot>=32)fprintf(stderr,"WARNING\n");
point=*pout+lmot;
if(point<32){*rout=*rout | (mot<<*pout);*pout+=lmot;}
else{nx=mot&((1<<(32-*pout))-1); *rout=*rout | (nx<<*pout);
     fwrite(rout,4,1,output); *rout=mot>>(32-*pout);
     *pout=lmot-(32-*pout);}
}

void prefm(srl,s,r,clas)
int srl[18][1025],s[18],r[18],*clas;
{
int i,j,k,nl,nc,ncmin,kmin;

for(i=0;i<18;i++)
{if(s[i]>1)*clas=i;}

nl=*clas;
if(*clas>16)*clas=16;
if(*clas>15)nl=17;

for(i=nl;i>0;i-=1)
{
ncmin=1000000;
kmin=69;
for(k=0;k<8;k++)
{
nc=0;

for(j=0;j<s[i];j++)
nc+=(srl[i][j]>>k)+k+1;

if(nc < ncmin){ncmin=nc; kmin=k;}
}
r[i]=kmin;
}

}

void  atrl(rl,m,pout,rout,output)

int rl, m, *pout,*rout;
FILE *output;
{
int nz,ix,mot;
nz=rl>>m;
               for(ix=0;ix<nz;ix++)
               putout(0,1,output,pout,rout);
               putout(1,1,output,pout,rout);
              mot=(rl-(nz<<m));
              putout(mot,m,output,pout,rout);
}

int posmsb(n)
int n;
{
int i=17;
while((((n>>(i-1))&1)!=1)&&(i>0)) i=i-1;
return(i);
}

void uvlc(p,l,output,pout,rout) 
  FILE *output;
  int *p,l,*pout,*rout;
{
int
i,j,rl,ix,iax,n,mot,lmot,nz,srl[18][1025],slsb[18][1025],r[18],s[18],clas;

/* Store the RLs */


for(n=0; n<18; n++)
{r[n]=0;s[n]=0;}

for(i=0; i<l; i++)
{
ix=p[i];
iax=abs(ix);
n=posmsb(iax);
if(n>0)
{
srl[n][s[n]]=r[n];
r[n]=0;
nz=((iax)&((1<<(n-1))-1))<<1;
nz+=(ix>>31)&1;
slsb[n][s[n]]=nz;
s[n]+=1;
for(ix=(n+1); ix<18; ix++)
r[ix]+=1;
}
else {
for(ix=1; ix<18; ix++)
r[ix]+=1;
}
}

for(n=17; n>0; n-=1)
{srl[n][s[n]]=r[n]+1; s[n]+=1;}

prefm(srl,s,r,&clas);
if(clas==0)putout(0,1,output,pout,rout);
else{
   putout(1,1,output,pout,rout);
   putout((clas-1),4,output,pout,rout);

   if(clas>15)clas=17;
   for(n=clas; n>0; n-=1)
   {if(s[n]<=1)putout(0,1,output,pout,rout);
     else {putout(1,1,output,pout,rout);
           putout(r[n],3,output,pout,rout);
           for(i=0;i<(s[n]-1);i++)
              {atrl(srl[n][i],r[n],pout,rout,output);
               putout(slsb[n][i],n,output,pout,rout);
               }
           atrl(srl[n][s[n]-1],r[n],pout,rout,output);
          }
    }	
     }
}

void uvld(p,l,input,pin,rin) 
  int *p,l,*pin,*rin;
  FILE *input;
{
int i,ix,n,clas,pr,m,r,rl,rrl,lsb,s,nn,pos1;
int srl[18][1025],val[18][1025],nrl[18],c[18],mrl[18];

for(i=0;i<l;i++)p[i]=0;

for(i=0;i<18;i++)nrl[i]=-1;

putin(&clas,1,input,pin,rin);
if(clas!=0)
{
putin(&clas,4,input,pin,rin);
clas+=1;
nn=0;
if(clas>15)clas=17;
rrl=0;
for(n=clas; n>0; n-=1)
 {
  rl=-1;
  putin(&pr,1,input,pin,rin);
  if(pr==1)
   {
   putin(&m,3,input,pin,rin);
   while(rl<(l-rrl))
   {
    itrl(&r,m,input,pin,rin);
    rl+=r+1;
    nrl[n]+=1;
    srl[n][nrl[n]]=r;
    if(rl<(l-rrl)){
    putin(&s,1,input,pin,rin);
    putin(&lsb,(n-1),input,pin,rin);
    if(s==0)val[n][nrl[n]]=lsb+(1<<(n-1));
    else val[n][nrl[n]]=-(lsb+(1<<(n-1)));
    }
   }
   }
   else {srl[n][0]=l; nrl[n]+=1;}
   rrl+=nrl[n];
  }
 }

for(i=0;i<=clas;i++){mrl[i]=srl[i][0];c[i]=0;}
rl=0;pos1=0;

while(rl<l){
for(n=1;n<=clas;n+=1) if(mrl[n]==0)pos1=n;
while(pos1!=0 && rl<l){
pos1=0; for(n=1;n<=clas;n+=1) if(mrl[n]==0)pos1=n;
n=pos1;
if(rl<l && c[n]<nrl[n])
{p[rl]=val[n][c[n]];
 c[n]+=1;
 mrl[n]=srl[n][c[n]];
 rl+=1;
for(i=clas;i>n;i-=1)mrl[i]-=1; 
 }}
for(i=clas;i>0;i-=1)mrl[i]-=1; 
rl+=1;}

}
