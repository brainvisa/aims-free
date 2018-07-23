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
/* 
	 pcpet.c
	 	pcpet (-in param)	in	out

	param is a file containing
	skipdpcm (int)
	Long (int)
	skipdpcm==1 if skip adpcm
	skipdpcm==0 if not
	Long = Length used for the UVLC (<1025)
*/

#include <ecat/lib_compress/uvlc.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* main routine */
void pcpet(skipdpcm, Long, input_filename, output_filename, ncol,nline)
int skipdpcm, Long;
char *input_filename, *output_filename;
int ncol,nline;
{
	int *p1,*p2,*pn,i,j,i1,i2,j1,pp2[1024],d1,d2,d3,d4,dmin,*d;
	int pred,l,pout,rout,lo,hi,indd,length,pp[1024], nind,flag;
	int pin,rin,pind[8],ntot,nx,pp3[1024],freq[65536],ix;
	short ij;
	FILE *input, *output, *filein;
	int *f;

	nind=0;
	input = fopen(input_filename,"r");
	output = fopen(output_filename,"w");

					/* get input header from input*/
/*
	rewind(input);
	fread(&ncol,4,1,input);	
	fread(&nline,4,1,input);	
*/

	p1=(int *) malloc(sizeof(int)*ncol*nline);
	d=(int *) malloc(sizeof(int)*ncol*nline);
	p2=(int *) malloc(sizeof(int)*ncol*nline);

	for(i=0; i<nline; i++)
		for(j=0; j<ncol; j++)
		{
	 		if ( fread(&ij,2,1,input) == 1 )	
			  p2[i*ncol+j]=ij;
		}

/*
	rewind(output);
	fwrite(&ncol,4,1,output);	
	fwrite(&nline,4,1,output);	
*/
	pout=0;
	rout=0;
	length=0;

	for(i=0; i<nline; i++)
		for(j=0; j<ncol; j++)
		{
			dmin=1000;d1=1000;d2=1000;d3=1000;d4=1000;
			if(j>1)d1=abs(p2[i*ncol+j-1]-p2[i*ncol+j-2]);
			if(i>1)d2=abs(p2[(i-1)*ncol+j]-p2[(i-2)*ncol+j]);
			if(i>1&&j>1)d3=abs(p2[(i-1)*ncol+j-1]-p2[(i-2)*ncol+j-2]);
			if(i>1&&j<(ncol-2))d4=abs(p2[(i-1)*ncol+j+1]-p2[(i-2)*ncol+j+2]);
			if(d1<dmin)
			{
				dmin=d1;p1[i*ncol+j]=p2[i*ncol+j]-p2[i*ncol+j-1];
			}
			if(d2<dmin)
			{
				dmin=d2;p1[i*ncol+j]=p2[i*ncol+j]-p2[(i-1)*ncol+j];
			}
			if(d3<dmin)
			{
				dmin=d3;p1[i*ncol+j]=p2[i*ncol+j]-p2[(i-1)*ncol+j-1];
			}
			if(d4<dmin)
			{
				dmin=d4;p1[i*ncol+j]=p2[i*ncol+j]-p2[(i-1)*ncol+j+1];
			}
			if(dmin>=1000)
			{
				dmin=0;
				if(j>0)p1[i*ncol+j]=p2[i*ncol+j]-p2[i*ncol+j-1];
				else p1[i*ncol+j]=p2[i*ncol+j];
			}
			if(skipdpcm==0) pp[length]=p1[i*ncol+j];
			else pp[length]=p2[i*ncol+j];
			length+=1;
			if(length==Long)
			{
				uvlc(pp,Long,output,&pout,&rout);
				nind+=1;
				length=0;
			}

		}

	if(length!=0)
	{
		for(i1=length; i1<Long; i1++)pp[i1]=0;
		uvlc(pp,Long,output,&pout,&rout);
		nind+=1;
	}

	fwrite(&rout,4,1,output); 
	fclose(output);
	fclose(input);
}
