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

int main(int argc,char *argv[])
{
  char   in_file[MAXFNAMESIZE],out_file[MAXFNAMESIZE];
  int    matrix,data_size,bin,x,y,z,verbose,
	 start_x,end_x,delta_x,start_y,end_y,delta_y,start_z,end_z,delta_z;
  float *fdata,*fdata_reverse;
  short *sdata,*sdata_reverse;
  UnifiedEcatInfo *uei_in,*uei_out;
  int reverse_composante[3],nmatrix;
  extern char *optarg;
  int         option;

  in_file[0] = out_file[0] = '\0';
  for (bin=0;bin<3;bin++) reverse_composante[bin] = 0;
  verbose = 0;
  nmatrix = 0;
  while ((option = getopt(argc,argv,"i:n:o:vxyz")) != EOF) {
    switch (option) {
      case 'h' :
	printf(" usage : EcatVolReverse -i(nput) -o(utput) -x(reverse) -y(reverse) -z(reverse) \n");
	printf("                        -n(# of volumes, default all) -v(verbose)\n");
	exit(EXIT_FAILURE);
      case 'i' :
	if (strlen(optarg) >= MAXFNAMESIZE) {
	  printf("  !!! Error: input parameter '%s' should contain less than %0d characters (%0x)\n",
		 optarg,MAXFNAMESIZE,strlen(optarg));
	  exit(EXIT_FAILURE);
	}
	if (sscanf(optarg,"%s",in_file) !=1) {
	  printf("error decoding -i %s\n",optarg);
	  exit(EXIT_FAILURE);
	}
	break;
      case 'n' :
	if (sscanf(optarg,"%d",&nmatrix) !=1) {
	  printf("error decoding -n %s\n",optarg);
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
     case 'v' :
       verbose = 1;
       break;
     case 'x' :
       reverse_composante[0] = 1;
       break;
     case 'y' :
       reverse_composante[1] = 1;
       break;
     case 'z' :
       reverse_composante[2] = 1;
       break;
    }
  }
  if (in_file[0] == '\0' || out_file[0] == '\0') {
    printf(" usage : EcatVolReverse -i(nput) -o(utput) [-x(reverse), -y(reverse), -z(reverse)]\n");
    exit(EXIT_FAILURE);
  }
  if ((uei_in = EcatOpen(in_file,"r")) == ECATSHFJ_FAIL) {
    printf(" !!! Error: can not open %s\n",in_file);
    exit(EXIT_FAILURE);
  }
  if (verbose) printf(" EcatVolReverse> input file %s opened\n",in_file);
  if ((uei_out = EcatOpen(out_file,"w")) == ECATSHFJ_FAIL) {
    printf(" !!! Error: can not create %s\n",out_file);
    EcatClose(uei_in);
    exit(EXIT_FAILURE);
  }
  if (verbose) printf(" EcatVolReverse> output file %s created\n",out_file);
  if (EcatUeiCopy(uei_in,uei_out) != ECATSHFJ_OK) {
    printf(" !!! Error: can not copy EcatUnifiedInfo from input file to output file\n");
    EcatClose(uei_in);
    EcatClose(uei_out);
    exit(EXIT_FAILURE);
  }
  if (nmatrix < 1 || nmatrix > EcatSizeT(uei_in)) nmatrix = EcatSizeT(uei_in);
  if (verbose) printf(" EcatVolReverse> Ecat information copied from input data to output data\n");
  if (verbose) printf(" EcatVolReverse> units : %s\n",uei_out->calibUnits);
  if (reverse_composante[0]) {
    start_x = EcatSizeX(uei_in) - 1;
    end_x   = -1;
    delta_x = -1;
  } else {
    start_x = 0;  
    end_x   = EcatSizeX(uei_in);
    delta_x = 1;
  }
  if (reverse_composante[1]) {
    start_y = EcatSizeY(uei_in) - 1;
    end_y   = -1;
    delta_y = -1;
  } else {
    start_y = 0;  
    end_y   = EcatSizeY(uei_in);
    delta_y = 1;
  }
  if (reverse_composante[2]) {
    start_z = EcatSizeZ(uei_in) - 1;
    end_z   = -1;
    delta_z = -1;
  } else {
    start_z = 0;  
    end_z   = EcatSizeZ(uei_in);
    delta_z = 1;
  }
  data_size=EcatSizeX(uei_in)*EcatSizeY(uei_in)*EcatSizeZ(uei_in);
  for (matrix=0; matrix<nmatrix; matrix++) {
    if (fabs(EcatVolScale(uei_in,matrix) - 1.0) < FLT_MIN) {
      printf(" volume %0d: integer data\n",matrix);
      if ((sdata = EcatReadVolume_S16(uei_in,matrix)) == ECATSHFJ_FAIL) {
	printf(" !!! Error: can not read matrix %0d in %s\n",matrix,in_file);
	EcatClose(uei_in);
	EcatClose(uei_out);
	exit(EXIT_FAILURE);
      }
      sdata_reverse = (short*) calloc(data_size,sizeof(short));
      bin = 0;
      for (z=start_z;z!=end_z;z+=delta_z) for (y=start_y;y!=end_y;y+=delta_y) for (x=start_x;x!=end_x;x+=delta_x) {
	sdata_reverse[x+y*EcatSizeX(uei_in)+z*EcatSizeX(uei_in)*EcatSizeY(uei_in)] = sdata[bin];
	bin++;
      }
      if (EcatWriteVolume_S16(uei_out, sdata_reverse, matrix) != ECATSHFJ_OK) {
	printf(" !!! Error: can not write matrix %0d in %s\n",matrix,out_file);
	EcatClose(uei_in);
	EcatClose(uei_out);
	exit(EXIT_FAILURE);
      }
      free(sdata);
      free(sdata_reverse);
    } else {
      printf(" volume %0d: floating point data\n",matrix);
      if ((fdata = EcatReadVolume_FLOAT(uei_in,matrix)) == ECATSHFJ_FAIL) {
	printf(" !!! Error: can not read matrix %0d in %s\n",matrix,in_file);
	EcatClose(uei_in);
	EcatClose(uei_out);
	exit(EXIT_FAILURE);
      }
      fdata_reverse = (float*) calloc(data_size,sizeof(float));
      bin = 0;
      for (z=start_z;z!=end_z;z+=delta_z) for (y=start_y;y!=end_y;y+=delta_y) for (x=start_x;x!=end_x;x+=delta_x) {
	fdata_reverse[x+y*EcatSizeX(uei_in)+z*EcatSizeX(uei_in)*EcatSizeY(uei_in)] = fdata[bin];
	bin++;
      }
      if (EcatWriteVolume_FLOAT(uei_out, fdata_reverse, matrix) != ECATSHFJ_OK) {
	printf(" !!! Error: can not write matrix %0d in %s\n",matrix,out_file);
	EcatClose(uei_in);
	EcatClose(uei_out);
	exit(EXIT_FAILURE);
      }
      free(fdata);
      free(fdata_reverse);
    }
  }
  EcatClose(uei_in);
  EcatClose(uei_out);
  exit(EXIT_SUCCESS);
}
