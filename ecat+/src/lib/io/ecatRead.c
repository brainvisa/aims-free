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

/*{{{  titre*/
/****************************************************************************
 * PROJECT     : E.C.A.T.S.H.F.J.         Library
 * MODULE NAME : interface.c             * TYPE     : Source
 * AUTHOR      : FROUIN V.               * CREATION : 19/05/2000
 * VERSION     : 0.1                     * REVISION :
 * LANGUAGE    : C++                     * EXAMPLE  :
 * DEVICE      : 
 ****************************************************************************
 *
 * DESCRIPTION : Ecat6-7 data reader class
 *               Version d'apres codes les codes de C. Comtat.
 *    
 *    EcatReadVolume_S16() (lit un volume (une frame) de short brut) 
 *
 *    EcatReadVolume_S16() (lit un volume (une frame) de float)
 *
 ****************************************************************************
 * REVISIONS :  DATE  |    AUTHOR    |       DESCRIPTION
 *--------------------|--------------|---------------------------------------
 *       07/09/ 2000  | COMTAT.C     |  Capable de lire les formats PetImage
 *                    |              |  ainsi que de gerer le multi-lit,
 *                    |              |  en concatenant les differentes
 *                    |              |  positions de lit
 ****************************************************************************/
/*}}}  */

#include <string.h>
#include <limits.h>
#include <float.h>
#include <math.h>
#include <sys/types.h>
#include <ecat/kernel/matrix.h>
#include <ecat+/io/io.h>

/* from machine_indep.c in ecat library */
int file_data_to_host(char *dptr, int nblks, int data_type);

/*{{{  EcatReadVolume_S16*/
/* ------------------------------------------------------------------------*/
short* EcatReadVolume_S16(UnifiedEcatInfo *uei , int t)
/* ------------------------------------------------------------------------*/
{
  /*{{{  declarations*/
  MatrixFile        *mptr;
  int                framegate;
  struct MatDir      matdir;
  struct Matval      matval;
  struct matdirnode *matnode;
  Image_subheader   *sh;
  int                matrix, foundframegate, currentframegate;
  int                nblks, data_size, file_pos, matnum,z,x,y,bed,first_bed,last_bed;
  short             *sdata, *sdata_plane, *sdata_bed;
  float             *fdata,scale_factor,min,max;
  unsigned int       label_map;
  int                bin;
  int                bed_motion,plane_overlap,nwbplane,plane;
  
  /*}}}  */

  fdata = NULL;
  sdata_plane = NULL;
  sdata = NULL;
  sdata_bed = NULL;
  nblks = 0;
  nwbplane = 0;
  plane_overlap = 0;
  bed_motion = 0;
  if (t >= EcatSizeT(uei)) {
    printf("EcatReadVolume_S16 : volume requested (%0d) should be smaller than %0d\n",
	   t,EcatSizeT(uei));
    return ECATSHFJ_FAIL;
  }
  /* need to re-open file */
  if ((mptr = matrix_open(uei->fname,MAT_READ_ONLY, MAT_UNKNOWN_FTYPE)) == NULL) {
    printf(" EcatReadVolume_S16> !!! Error: can not re-open %s\n",uei->fname);
    return ECATSHFJ_FAIL;
  }
  if ( EcatFileType( uei ) != PetVolume && 
       EcatFileType( uei ) != PetImage &&
       EcatFileType( uei ) != PetProjection) {
    printf("EcatReadVolume_S16 : input file %s is not an image or a projection (file type is %0d\n",
	    uei->fname, EcatFileType( uei ));
    return ECATSHFJ_FAIL;
  }

  /*{{{  Selection of the frame/gate according to type*/
  framegate = EcatRealFrameGate(uei,t);
  matnode = mptr->dirlist->first;
  foundframegate = 0;
  for (matrix=0; matrix < mptr->dirlist->nmats; matrix++) {
    mat_numdoc(matnode->matnum,&matval);
    if (matval.plane == 1 && matval.data == 0 && matval.bed == 0) {
      switch(uei->dyn_type) {
	case ECATSHFJ_GATED :
	  currentframegate = matval.gate;
	  break;
	case ECATSHFJ_FRAMED :
	default :
	  currentframegate = matval.frame;
	  break;
      }
      if (currentframegate == framegate) {
	foundframegate = 1;
	matnum = matnode->matnum;
	break;
      }
    }
    matnode = matnode->next;
  }
  if (!foundframegate) {
    printf("EcatReadVolume_S16 : frame/gate %d does not exists in image %s\n", framegate, mptr->fname);
    return ECATSHFJ_FAIL;
  }
  /*{{{  multi-bed*/
  first_bed = 0;
  last_bed = 0;
  if (mptr->mhptr->num_bed_pos > 0) { /* Multi-bed acquisition */
    mat_numdoc(matnum,&matval);
    for (bed=1;bed<=mptr->mhptr->num_bed_pos;bed++) {
      if (matrix_find(mptr,mat_numcod(matval.frame,matval.plane,matval.gate,0,bed),&matdir) == 0)
	last_bed = bed;
    }
    if (last_bed > first_bed) {
      if (mptr->mhptr->bed_offset[0] > 0.0) bed_motion = 1;
      else bed_motion = -1;
      plane_overlap = (int) ((float) mptr->mhptr->num_planes -
			     (float) bed_motion*((mptr->mhptr->bed_offset[0])/mptr->mhptr->plane_separation) + 0.5);
      if (plane_overlap < 0) {
	printf("EcatREadVolume_S16  : !!! Error : negative number of plane overlap: %0d\n",plane_overlap);
	return ECATSHFJ_FAIL;
      }
      nwbplane = (mptr->mhptr->num_planes) + (last_bed - first_bed)*(mptr->mhptr->num_planes - plane_overlap);
      if (nwbplane != uei->size.z) {
	printf("EcatReadVolume_S16 : total number of planes (%0d) disagrees with uei size.z (%0d)\n",
	       nwbplane,uei->size.z);
	return ECATSHFJ_FAIL;
      }
    }
  }
  /*}}}  */
  /*}}}  */

  if (last_bed == first_bed) {
    /*{{{  mono-bed volume*/
    /*{{{  Allocation du buffer de sortie*/
    data_size = uei->size.x * uei->size.y * uei->size.z;
    if ( EcatFileType( uei ) == PetVolume || EcatFileType( uei ) == PetProjection) {
      nblks = (data_size*sizeof(short) + MatBLKSIZE - 1)/MatBLKSIZE;
      sdata = (short *)calloc(nblks*MatBLKSIZE/sizeof(short), sizeof(short));
    } else if ( EcatFileType( uei ) == PetImage) {
      nblks = (uei->size.x*uei->size.y*sizeof(short) + MatBLKSIZE - 1)/MatBLKSIZE;
      sdata_plane = (short *)calloc(nblks*MatBLKSIZE/sizeof(short), sizeof(short));
      if (sdata_plane == NULL) {
	printf("EcatReadVolume_S16 : Can't calloc sdata_plane\n");
	return ECATSHFJ_FAIL;
      }
      sdata = (short *) calloc(data_size, sizeof(short));
      if (strcmp(EcatCalibUnit(uei),ECATSHFJ_LABELS) != 0) {
	fdata = (float *) calloc(data_size, sizeof(float));
	if (fdata == NULL) {
	  printf("EcatReadVolume_S16 : Can't calloc fdata\n");
	  return ECATSHFJ_FAIL;
	}
      }
    }
    if (sdata == NULL) {
      printf("EcatReadVolume_S16 : Can't calloc sdata\n");
      return ECATSHFJ_FAIL;
    }
    if ((sh = (Image_subheader*) calloc(1,sizeof(Image_subheader)) ) == NULL) {
      printf("EcatReadVolume_S16 : can't calloc sh\n");
      return ECATSHFJ_FAIL;
    }
    /*}}}  */
    
    if ( EcatFileType( uei ) == PetVolume || EcatFileType( uei ) == PetProjection) {
      /*{{{  lecture volume*/
      if (matrix_find(mptr,matnum,&matdir)) {
	printf("EcatReadVolume_S16 : can not find matrix %0d in %s\n",matnum,mptr->fname);
	return ECATSHFJ_FAIL;
      } else if (mat_read_image_subheader(mptr->fptr,mptr->mhptr,matdir.strtblk,sh)) {
	printf("EcatReadVolume_S16 : can not read PetVolume subheader %0d in '%s'\n",
		matnum,mptr->fname);
	return ECATSHFJ_FAIL;
      }
      if (sh->data_type != SunShort && sh->data_type != VAX_Ix2) {
	printf("EcatReadVolume_S16 : unsuported data type in %s: %0d (only %0d and %0d are currently supported)\n",
	       mptr->fname,sh->data_type,SunShort,VAX_Ix2);
	return ECATSHFJ_FAIL;
      }
      file_pos = matdir.strtblk*MatBLKSIZE;
      if (fseek(mptr->fptr,file_pos,0) == EOF) {
	printf("EcatReadVolume_S16 : error with fseek position %0d for %s\n",file_pos,mptr->fname);
	return ECATSHFJ_FAIL;
      } else if ((int) fread(sdata,sizeof(short), data_size, mptr->fptr)!=data_size) {
	printf("EcatReadVolime_S16 : Error reading %0d matrix from %s\n", matnum, mptr->fname);
	return ECATSHFJ_FAIL;
      } else if (file_data_to_host((char *)sdata,nblks,sh->data_type) != 0) {
	printf("EcatReadVolume_S16 : Error with data_to_host fro %s\n",mptr->fname);
	return ECATSHFJ_FAIL;
      }
      /*}}}  */
    } else if ( EcatFileType( uei ) == PetImage) {
      /*{{{  lecture plan a plan*/
      for (z=0;z<uei->size.z;z++) {
	switch(uei->dyn_type) {
	  case ECATSHFJ_GATED :
	    matnum = mat_numcod(1,z+1,framegate,0,0);
	    break;
	  case ECATSHFJ_FRAMED :
	  default :
	    matnum = mat_numcod(framegate,z+1,1,0,0);
	    break;
	}
	if (matrix_find(mptr,matnum,&matdir)) {
	  printf("EcatReadVolume_S16 : can not find matrix %0d in %s\n",matnum,mptr->fname);
	  return ECATSHFJ_FAIL;
	}
	if (mat_read_image_subheader(mptr->fptr,mptr->mhptr,matdir.strtblk,sh)) {
	  printf("EcatReadVolume_S16 : can not read PetVolume subheader %0d in '%s'\n",
		  matnum,mptr->fname);
	  return ECATSHFJ_FAIL;
	}
	if (sh->data_type != SunShort && sh->data_type != VAX_Ix2) {
	  printf("EcatReadVolume_S16 : unsuported data type in %s: %0d (only %0d and %0d are currently supported)\n",
	         mptr->fname,sh->data_type,SunShort,VAX_Ix2);
	  return ECATSHFJ_FAIL;
	}
	file_pos = matdir.strtblk*MatBLKSIZE;
	if (fseek(mptr->fptr,file_pos,0) == EOF) {
	  printf("EcatReadVolume_S16 : error with fseek position %0d for %s\n",file_pos,mptr->fname);
	  return ECATSHFJ_FAIL;
	} else if ((int) fread(sdata_plane,sizeof(short),uei->size.x*uei->size.y, mptr->fptr) !=
			       uei->size.x*uei->size.y) {
	  printf("EcatReadVolime_S16 : Error reading %0d matrix from %s\n", matnum, mptr->fname);
	  return ECATSHFJ_FAIL;
	} else if (file_data_to_host((char *)sdata_plane,nblks,sh->data_type) != 0) {
	  printf("EcatReadVolume_S16 : Error with data_to_host fro %s\n",mptr->fname);
	  return ECATSHFJ_FAIL;
	}
	if (z==0) {
	  if (fabs(sh->scale_factor - 1.0) < FLT_MIN) {
	    label_map = 1;
	  } else {
	    label_map = 0;
	  }
	}
	label_map = label_map; /* compilation warning */
	if (strcmp(EcatCalibUnit(uei),ECATSHFJ_LABELS) == 0) {
	  if (fabs(sh->scale_factor - 1.0) > FLT_MIN) {
	    printf("EcatReadVolume_S16 : Error for labels map : scale factor != 1 for plane %0d and matrix %0d\n",
		   z,t);
	    return ECATSHFJ_FAIL;
	  }
	  for (bin=0;bin<uei->size.x*uei->size.y;bin++)
	    sdata[bin+z*uei->size.x*uei->size.y] = sdata_plane[bin];
	} else {
	  for (bin=0;bin<uei->size.x*uei->size.y;bin++)
	    fdata[bin+z*uei->size.x*uei->size.y] = sh->scale_factor*sdata_plane[bin];
	}
      }
      free(sdata_plane);
      if (strcmp(EcatCalibUnit(uei),ECATSHFJ_LABELS) != 0) {
	min=fdata[0];
	max=fdata[0];
	for (bin=1;bin<data_size;bin++) {
	  if (fdata[bin]>max) max=fdata[bin];
	  if (fdata[bin]<min) min=fdata[bin];
	}
	scale_factor = 1.0;
	if ((max > 0.0) && (max > -min))     scale_factor = max/32767;
	else if ((min < 0.0) &&(min < -max)) scale_factor = -min/32767.0;
	for (bin=0; bin<data_size; bin++) sdata[bin] = (short) (fdata[bin]/scale_factor);
	EcatVolScale(uei,t) = scale_factor;
      } else {
	EcatVolScale(uei,t) = 1.0;
      }
      
      free(fdata);
      /*}}}  */
    }
    /*}}}  */
  } else {
    /*{{{  multi-bed volumes*/
    if (strcmp(EcatCalibUnit(uei),ECATSHFJ_LABELS) == 0) {
      printf("EcatReadVolume_S16 : multi-bed labels not supported\n");
      return ECATSHFJ_FAIL;
    }
    /*{{{  Allocation du buffer de sortie*/
    data_size = uei->size.x * uei->size.y * nwbplane;
    if ( EcatFileType( uei ) == PetVolume || EcatFileType( uei ) == PetProjection) {
      nblks = ((uei->size.x*uei->size.y*mptr->mhptr->num_planes)*sizeof(short) + MatBLKSIZE - 1)/MatBLKSIZE;
      sdata_bed = (short *)calloc(nblks*MatBLKSIZE/sizeof(short), sizeof(short));
      if (sdata_bed == NULL) {
	printf("EcatReadVolume_S16 : Can't calloc sdata_bed\n");
	return ECATSHFJ_FAIL;
      }
    } else if (EcatFileType( uei ) == PetImage) {
      nblks = (uei->size.x*uei->size.y*sizeof(short) + MatBLKSIZE - 1)/MatBLKSIZE;
      sdata_plane = (short *)calloc(nblks*MatBLKSIZE/sizeof(short), sizeof(short));
      if (sdata_plane == NULL) {
	printf("EcatReadVolume_S16 : Can't calloc sdata_plane\n");
	return ECATSHFJ_FAIL;
      }
    }
    sdata = (short *)calloc(data_size, sizeof(short));
    if (sdata == NULL) {
      printf("EcatReadVolume_S16 : Can't calloc sdata\n");
      return ECATSHFJ_FAIL;
    }
    fdata = (float *) calloc(data_size, sizeof(float));
    if (fdata == NULL) {
      printf("EcatReadVolume_S16 : Can't calloc fdata\n");
      return ECATSHFJ_FAIL;
    }
    if ((sh = (Image_subheader*) calloc(1,sizeof(Image_subheader)) ) == NULL) {
      printf("EcatReadVolume_S16 : can't calloc sh\n");
      return ECATSHFJ_FAIL;
    }
    /*}}}  */
    
    for (bed=first_bed;bed<=last_bed;bed++) {
      if (EcatFileType( uei ) == PetVolume || EcatFileType( uei ) == PetProjection) {
	/*{{{  lecture volume*/
	switch(uei->dyn_type) {
	  case ECATSHFJ_GATED :
	    if (bed_motion > 0)
	      matnum = mat_numcod(1,1,framegate,0,bed);
	    else
	      matnum = mat_numcod(1,1,framegate,0,last_bed+first_bed-bed);
	    break;
	  case ECATSHFJ_FRAMED :
	  default :
	    if (bed_motion > 0)
	      matnum = mat_numcod(framegate,1,1,0,bed);
	    else
	      matnum = mat_numcod(framegate,1,1,0,last_bed+first_bed-bed);
	    break;
	}
	if (matrix_find(mptr,matnum,&matdir)) {
	  printf("EcatReadVolume_S16 : can not find matrix %0d in %s\n",matnum,mptr->fname);
	  free(fdata); free(sdata); free(sdata_bed);
	  return ECATSHFJ_FAIL;
	} else if (mat_read_image_subheader(mptr->fptr,mptr->mhptr,matdir.strtblk,sh)) {
	  printf("EcatReadVolume_S16 : can not read PetVolume subheader %0d in '%s'\n",
		  matnum,mptr->fname);
	  free(fdata); free(sdata); free(sdata_bed);
	  return ECATSHFJ_FAIL;
	}
	if (sh->data_type != SunShort && sh->data_type != VAX_Ix2) {
	  printf("EcatReadVolume_S16 : unsuported data type in %s: %0d (only %0d and %0d are currently supported)\n",
	         mptr->fname,sh->data_type,SunShort,VAX_Ix2);
	  free(fdata); free(sdata); free(sdata_bed);
	  return ECATSHFJ_FAIL;
	} else if (sh->x_dimension != uei->size.x || sh->y_dimension != uei->size.y || sh->z_dimension != mptr->mhptr->num_planes) {
	  printf("EcatReadVolume_S16 : image size uncoherent in %s relative to uei structure\n",
		 uei->fname);
	  free(fdata); free(sdata); free(sdata_bed);
	  return ECATSHFJ_FAIL;
	}
	file_pos = matdir.strtblk*MatBLKSIZE;
	if (fseek(mptr->fptr,file_pos,0) == EOF) {
	  printf("EcatReadVolume_S16 : error with fseek position %0d for %s\n",file_pos,mptr->fname);
	  free(fdata); free(sdata); free(sdata_bed);
	  return ECATSHFJ_FAIL;
	} else if ((int) fread(sdata_bed,sizeof(short),uei->size.x*uei->size.y*mptr->mhptr->num_planes,mptr->fptr) !=
		   uei->size.x*uei->size.y*mptr->mhptr->num_planes) {
	  printf("EcatReadVolime_S16 : Error reading %0d matrix from %s\n", matnum, mptr->fname);
	  free(fdata); free(sdata); free(sdata_bed);
	  return ECATSHFJ_FAIL;
	} else if (file_data_to_host((char *)sdata_bed,nblks,sh->data_type) != 0) {
	  printf("EcatReadVolume_S16 : Error with data_to_host fro %s\n",mptr->fname);
	  free(fdata); free(sdata); free(sdata_bed);
	  return ECATSHFJ_FAIL;
	}
	for (z=0;z<mptr->mhptr->num_planes;z++) {
	  plane = bed*(mptr->mhptr->num_planes - plane_overlap) + z;
	  if (bed > 0 && z < plane_overlap)
	    scale_factor = ((float)z + 1.)/((float)plane_overlap + 1.);
	  else if (bed < last_bed && z > (mptr->mhptr->num_planes - 1) - plane_overlap)
	    scale_factor = (float)(mptr->mhptr->num_planes - z)/((float)plane_overlap + 1.);
	  else
	    scale_factor = 1.0;
	  for (y=0;y<uei->size.y;y++) for (x=0;x<uei->size.x;x++) {
	    fdata[x+y*uei->size.x+plane*uei->size.x*uei->size.y] +=
	      scale_factor*sh->scale_factor*sdata_bed[x+y*uei->size.x+z*uei->size.x*uei->size.y];
	  }
	}
	/*}}}  */
      } else if (EcatFileType( uei ) == PetImage) {
	/*{{{  lecture plan a plan*/
	for (z=0;z<mptr->mhptr->num_planes;z++) {
	  switch(uei->dyn_type) {
	    case ECATSHFJ_GATED :
	      if (bed_motion > 0)
		matnum = mat_numcod(1,z+1,framegate,0,bed);
	      else
		matnum = mat_numcod(1,z+1,framegate,0,last_bed+first_bed-bed);
	      break;
	    case ECATSHFJ_FRAMED :
	    default :
	      if (bed_motion > 0)
		matnum = mat_numcod(framegate,z+1,1,0,bed);
	      else
		matnum = mat_numcod(framegate,z+1,1,0,last_bed+first_bed-bed);
	      break;
	  }
	  if (matrix_find(mptr,matnum,&matdir)) {
	    printf("EcatReadVolume_S16 : can not find matrix %0d in %s\n",matnum,mptr->fname);
	    return ECATSHFJ_FAIL;
	  }
	  if (mat_read_image_subheader(mptr->fptr,mptr->mhptr,matdir.strtblk,sh)) {
	    printf("EcatReadVolume_S16 : can not read PetVolume subheader %0d in '%s'\n",
		    matnum,mptr->fname);
	    return ECATSHFJ_FAIL;
	  }
	  if (sh->data_type != SunShort && sh->data_type != VAX_Ix2) {
	    printf("EcatReadVolume_S16 : unsuported data type in %s: %0d (only %0d and %0d are currently supported)\n",
	           mptr->fname,sh->data_type,SunShort,VAX_Ix2);
	    return ECATSHFJ_FAIL;
	  }
	  file_pos = matdir.strtblk*MatBLKSIZE;
	  if (fseek(mptr->fptr,file_pos,0) == EOF) {
	    printf("EcatReadVolume_S16 : error with fseek position %0d for %s\n",file_pos,mptr->fname);
	    return ECATSHFJ_FAIL;
	  } else if ((int) fread(sdata_plane,sizeof(short),uei->size.x*uei->size.y, mptr->fptr) !=
				 uei->size.x*uei->size.y) {
	    printf("EcatReadVolime_S16 : Error reading %0d matrix from %s\n", matnum, mptr->fname);
	    return ECATSHFJ_FAIL;
	  } else if (file_data_to_host((char *)sdata_plane,nblks,sh->data_type) != 0) {
	    printf("EcatReadVolume_S16 : Error with data_to_host fro %s\n",mptr->fname);
	    return ECATSHFJ_FAIL;
	  }
	  plane = bed*(mptr->mhptr->num_planes - plane_overlap) + z;
	  if (bed > 0 && z < plane_overlap)
	    scale_factor = ((float)z + 1.)/((float)plane_overlap + 1.);
	  else if (bed < last_bed && z > (mptr->mhptr->num_planes - 1) - plane_overlap)
	    scale_factor = (float)(mptr->mhptr->num_planes - z)/((float)plane_overlap + 1.);
	  else
	    scale_factor = 1.0;
	  for (y=0;y<uei->size.y;y++) for (x=0;x<uei->size.x;x++) {
	    fdata[x+y*uei->size.x+plane*uei->size.x*uei->size.y] +=
	      scale_factor*sh->scale_factor*sdata_plane[x+y*uei->size.x];
	  }
	}
	/*}}}  */
      }
    }
    if (EcatFileType( uei ) == PetVolume || EcatFileType( uei ) == PetProjection)
      free(sdata_bed);
    else if (EcatFileType( uei ) == PetImage)
      free(sdata_plane);
    min=fdata[0];
    max=fdata[0];
    for (bin=1;bin<data_size;bin++) {
      if (fdata[bin]>max) max=fdata[bin];
      if (fdata[bin]<min) min=fdata[bin];
    }
    scale_factor = 1.0;
    if ((max > 0.0) && (max > -min))     scale_factor = max/32767;
    else if ((min < 0.0) &&(min < -max)) scale_factor = -min/32767.0;
    for (bin=0; bin<data_size; bin++) sdata[bin] = (short) (fdata[bin]/scale_factor);
    EcatVolScale(uei,t) = scale_factor;
    free(fdata);
    /*}}}  */
  }
  free(sh);
  matrix_close(mptr);
  return( sdata );
}
/*}}}  */

/*{{{  EcatReadVolume_FLOAT*/
/* ------------------------------------------------------------------------*/
float* EcatReadVolume_FLOAT( UnifiedEcatInfo *uei, int t)
/* ------------------------------------------------------------------------*/
{
  unsigned int       bin,data_size;
  short             *sdata;
  float             *fdata;



  if (strcmp(EcatCalibUnit(uei),ECATSHFJ_LABELS) == 0) {
    printf(" EcatReadVolume_FLOAT> !!! WARNING: labels converted to floating point\n");
    strncpy(EcatCalibUnit(uei),ECATSHFJ_UNKN,127);
  }
  if ((sdata = EcatReadVolume_S16(uei,t)) == ECATSHFJ_FAIL) {
    printf(" EcatReadVolume_FLOAT> !!! Error: can not read volume %0d from %s\n",t,uei->fname);
    return ECATSHFJ_FAIL;
  }

  /* Quantified data */
  data_size = EcatSizeX(uei)*EcatSizeY(uei)*EcatSizeZ(uei);
  fdata = (float *) calloc(data_size, sizeof(float));
  if (fdata == NULL) {
    printf("EcatReadVolume_FLOAT : Can't calloc fdata\n");
    return ECATSHFJ_FAIL;
  }
  for (bin=0;bin<data_size;bin++) fdata[bin] = EcatVolScale(uei,t)*sdata[bin];
  free(sdata);
  EcatVolScale(uei,t) = 1.0;
  return(fdata);
}
/*}}}  */

