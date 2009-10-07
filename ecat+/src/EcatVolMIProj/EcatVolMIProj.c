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

#include <ecat+/io/io.h>
#include <stdio.h>
#include <string.h>
#include <float.h>
#include <math.h>
#include <unistd.h>

#define MAXFNAMESIZE 200

int   nangle=32;
float theta=0.0;

int usage()
{
  printf(" usage : EcatVolMIProj -i(nput) -o(utput) [-t thet]a  [-a #angles], default=32]\n");
  printf("\n");
  printf("                       -i input file in 3dSinogram format\n");
  printf("                       -o output file in PetProjection format (*.p)\n");
  printf("                       -t polar angle in radian, default = %g\n",theta);
  printf("                       -a number of projection angles, default = %0d\n",nangle);
  exit(EXIT_FAILURE);
}

int  forward_proj3d_view(float *, float *, float , float , float ,
			 float , float , float , int , int ,int ,
			 float , float , float ,
			 float , float , int , int );

int main(int argc,char *argv[])
{
  char   in_file[MAXFNAMESIZE],out_file[MAXFNAMESIZE];
  int    matrix,bin,data_size_in,data_size_out,angle,verbose;
  float *fdata,*fdata_proj;
  float fi,cosfi,sinfi,x_offset,y_offset,z_offset;
  UnifiedEcatInfo *uei_in,*uei_out;
  extern char *optarg;
  int         option;

  /*{{{  initialisations*/
  if (argc < 2) usage();
  in_file[0] = out_file[0] = '\0';
  nangle = 32;
  theta = 0.0;
  x_offset = y_offset = z_offset = 0.0;
  verbose = 0;
  while ((option = getopt(argc,argv,"a:i:o:t:v")) != EOF) {
    switch (option) {
      case 'a' :
	if (sscanf(optarg,"%d",&nangle) != 1) {
	  printf(" !!! Error: input parameter '%s' should be a positive integer\n",optarg);
	  exit(EXIT_FAILURE);
	}
	break;
      case 'h' :
	usage();
      case 'i' :
	if (strlen(optarg) >= MAXFNAMESIZE) {
	  printf("  !!! Error: input parameter '%s' should contain less than %0d characters (%0x)\n",
		 optarg,MAXFNAMESIZE,strlen(optarg));
	  exit(EXIT_FAILURE);
	}
	if (sscanf(optarg,"%s",in_file) !=1) {
	  printf("error decoding -o %s\n",optarg);
	  exit(EXIT_FAILURE);
	}
	break;
      case 'o' :
	if (strlen(optarg) >= MAXFNAMESIZE) {
	  printf("  !!! Error: input parameter '%s' should contain less than %0d characters (%0x)\n",
		 optarg,MAXFNAMESIZE,strlen(optarg));
	  exit(EXIT_FAILURE);
	}
	if (sscanf(optarg,"%s",out_file) !=1) {
	  printf("error decoding -o %s\n",optarg);
	  exit(EXIT_FAILURE);
	}
	break;
      case 't' :
	if (sscanf(optarg,"%f",&theta) != 1) {
	  printf(" !!! Error: input parameter '-t %s' should be a floating number\n",optarg);
	  exit(EXIT_FAILURE);
	}
	break;
      case 'v' :
	verbose = 1;
	break;
    }
  }
  if (in_file[0] == '\0' || out_file[0] == '\0') usage();
  if (nangle < 1) {
    printf(" !!! Error: number of angles (%0d) should be a strictly positive integer\n",nangle);
    exit(EXIT_FAILURE);
  }
  /*}}}  */
  /*{{{  preparation*/
  if ((uei_in = EcatOpen(in_file,"r")) == ECATSHFJ_FAIL) {
    printf(" !!! Error: can not open %s\n",in_file);
    exit(EXIT_FAILURE);
  }
  if ((uei_out = EcatOpen(out_file,"w")) == ECATSHFJ_FAIL) {
    printf(" !!! Error: can not create %s\n",out_file);
    EcatClose(uei_in);
    exit(EXIT_FAILURE);
  }
  if (EcatUeiCopy(uei_in,uei_out) != ECATSHFJ_OK) {
    printf(" !!! Error: can not copy EcatUnifiedInfo from input file to output file\n");
    EcatClose(uei_in);
    EcatClose(uei_out);
    exit(EXIT_FAILURE);
  }
  EcatVoxSizeY(uei_out) = EcatVoxSizeX(uei_in);
  EcatVoxSizeZ(uei_out) = EcatVoxSizeX(uei_in);
  EcatSizeY(uei_out) = (EcatSizeZ(uei_in)*EcatVoxSizeZ(uei_in))/EcatVoxSizeY(uei_out);
  EcatSizeZ(uei_out) = nangle;
  data_size_in  = EcatSizeX(uei_in)*EcatSizeY(uei_in)*EcatSizeZ(uei_in);
  data_size_out = EcatSizeX(uei_out)*EcatSizeY(uei_out)*EcatSizeZ(uei_out);
  printf(" EcatVolMIProj: volume projected into %0d angles of %0d X %0d pixels each (%g X %g mm^2)\n",
	 EcatSizeZ(uei_out),EcatSizeX(uei_out),EcatSizeY(uei_out),
	 EcatVoxSizeX(uei_out),EcatVoxSizeY(uei_out));
  /*}}}  */
  for (matrix=0; matrix<EcatSizeT(uei_in); matrix++) {
    if ((fdata = EcatReadVolume_FLOAT(uei_in,matrix)) == ECATSHFJ_FAIL) {
      printf(" !!! Error: can not read matrix %0d in %s\n",matrix,in_file);
      EcatClose(uei_in);
      EcatClose(uei_out);
      exit(EXIT_FAILURE);
    }
    if (EcatCalib(uei_out) > 0.0) for (bin=0;bin<data_size_in;bin++) fdata[bin] *= EcatCalib(uei_out);
    fdata_proj = (float*) calloc(data_size_out,sizeof(float));
    for (angle=0;angle<nangle;angle++) {
      fi = angle*M_PI*2.0/nangle;
      cosfi = cos(fi);
      sinfi = sin(fi);
      if (forward_proj3d_view(fdata,fdata_proj+angle*EcatSizeX(uei_out)*EcatSizeY(uei_out),
			      cosfi,sinfi,theta,
			      EcatVoxSizeX(uei_in),EcatVoxSizeY(uei_in),EcatVoxSizeZ(uei_in),
			      EcatSizeX(uei_in),EcatSizeY(uei_in),EcatSizeZ(uei_in),
			      x_offset,y_offset,z_offset,
			      EcatVoxSizeX(uei_out),EcatVoxSizeY(uei_out),
			      EcatSizeX(uei_out),EcatSizeY(uei_out)) != 1) {
	printf(" !!! Error: can not project angle %g\n",fi);
	EcatClose(uei_in);
	EcatClose(uei_out);
	exit(EXIT_FAILURE);
      }
      if (verbose) printf(" EcatVolMIProj: matrix %0d and angle %0d is projected\n",matrix,angle);
    }
    if (EcatWriteVolume_FLOAT(uei_out, fdata_proj, matrix) != ECATSHFJ_OK) {
      printf(" !!! Error: can not write matrix %0d in %s\n",matrix,out_file);
      EcatClose(uei_in);
      EcatClose(uei_out);
      exit(EXIT_FAILURE);
    }
    free(fdata);
    free(fdata_proj);
  }
  EcatClose(uei_in);
  EcatClose(uei_out);
  exit(EXIT_SUCCESS);
}
