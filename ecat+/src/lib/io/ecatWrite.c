/* Copyright (c) 1995-2005 CEA
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

/*{{{  titre*/
/****************************************************************************
 * PROJECT     : E.C.A.T.S.H.F.J.         Library
 * MODULE NAME : interface.c             * TYPE     : Source
 * AUTHOR      : FROUIN V.               * CREATION : 07/09/2000
 * VERSION     : 0.1                     * REVISION :
 * LANGUAGE    : C++                     * EXAMPLE  :
 * DEVICE      : 
 ****************************************************************************
 *
 * DESCRIPTION : Ecat6-7 data write class
 *               Version d'apres codes les codes de C. Contat.
 *    
 *    EcatWriteVolume_S16() (ecrit un volume (une frame) de short brut)
 *
 *    EcatWriteVolume_FLOAT() (ecrit un volume (une frame) de float)
 *
 *
 ****************************************************************************
 * REVISIONS :  DATE  |    AUTHOR    |       DESCRIPTION
 *--------------------|--------------|---------------------------------------
 *              / /   |              |
 ****************************************************************************/
/*}}}  */

#include <string.h>
#include <limits.h>
#include <math.h>
#include <sys/types.h>
#include <ecat/kernel/matrix.h>
#include <ecat+/io/io.h>

/* from machine_indep.c in ecat library */
int file_data_to_host(char *dptr, int nblks, int data_type);
/* from matrix_extra.c in ecat library */
int mh_update(MatrixFile *mptr);

/*{{{  EcatWriteVolume_S16*/
/* ------------------------------------------------------------------------*/
int EcatWriteVolume_S16(UnifiedEcatInfo *uei , short *indata, int t)
/* ------------------------------------------------------------------------*/
{
    int              nblks,blkno,file_pos,matnum;
    short            min,max;
    MatrixFile      *mptr,*mptr_orig;
    struct MatDir    matdir, matdir_orig,dir_entry;
    Image_subheader *sh;
    size_t           data_size, bin;

    if (t >= EcatSizeT(uei)) {
      printf(" EcatWriteVolume_S16> !!! Error: volume number (%0d) should be smaller than %0d\n",
	     t,EcatSizeT(uei));
      return ECATSHFJ_FAIL;
    }
    /* Need to re-open output file */
    if ((mptr = matrix_open(uei->fname,MAT_OPEN_EXISTING, MAT_UNKNOWN_FTYPE)) == NULL) {
      printf(" EcatWriteVolume_S16> !!! Error: can not re-open %s\n",uei->fname);
      return ECATSHFJ_FAIL;
    }

    /* Correct type (PetVolume) for output file ? */
    if (mptr->mhptr->file_type != PetVolume) {
      printf(" EcatWriteVolume_S16> !!! Error: '%s' is not a PetVolume matrix file (%0d)\n",
	     mptr->fname,mptr->mhptr->file_type);
      return ECATSHFJ_FAIL;
    }

    /* Determination of matnum(frame,plane=1,gate,data=0,bed=0) */
    if (EcatRealFrameGate(uei,t) > 0) {
      switch(uei->dyn_type)
	{
	case ECATSHFJ_GATED :
	  matnum = mat_numcod(1,1,EcatRealFrameGate(uei,t),0,0);
	  break;
	case ECATSHFJ_FRAMED :
	default :
	  matnum = mat_numcod(EcatRealFrameGate(uei,t),1,1,0,0);
	  break;
      }
    } else matnum = mat_numcod(t+1,1,1,0,0);

    /* Are the sizes provided in uei ? */
    data_size=EcatSizeX(uei)*EcatSizeY(uei)*EcatSizeZ(uei);
    if (data_size <= 0.0) {
      printf(" EcatWriteVolume_S16 > !!! Error: image size should be provided in struct uei (uei->size)\n");
      return ECATSHFJ_FAIL;
    } else if (EcatVoxSizeX(uei)*EcatVoxSizeY(uei)*EcatVoxSizeZ(uei) <= 0.0) {
      printf(" EcatWriteVolume_S16 > !!! Error: voxel size should be provided in struct uei (uei->voxelsize)\n");
      return ECATSHFJ_FAIL;
    }

    /* entry in directory */
    nblks = (data_size*sizeof(short)+MatBLKSIZE-1)/MatBLKSIZE;
    if (matrix_find(mptr,matnum,&matdir) == -1) {
      /* new entry */
      blkno = mat_enter(mptr->fptr,mptr->mhptr,matnum,nblks);
      dir_entry.matnum = matnum;
      dir_entry.strtblk = blkno;
      dir_entry.endblk  = dir_entry.strtblk + nblks;
      dir_entry.matstat = 1;
      insert_mdir(dir_entry,mptr->dirlist);
      matdir = dir_entry;
    } else blkno = matdir.strtblk; /* overwrite */

    /* Write Volume sub-header */
    if ((sh = (Image_subheader*) calloc(1,sizeof(Image_subheader)) ) == NULL) {
      printf("EcatWriteVolume_S16: can't calloc sh\n");
      return ECATSHFJ_FAIL;
    }
    if (EcatVersion(uei->original_filename) == 7) {
      if ((mptr_orig = matrix_open(uei->original_filename, MAT_READ_ONLY, MAT_UNKNOWN_FTYPE)) != NULL) {
	if ((mptr_orig->mhptr->file_type == PetVolume || mptr_orig->mhptr->file_type == PetImage) &&
	    (matrix_find(mptr_orig,matnum,&matdir_orig) == 0)) {
	  if (mat_read_image_subheader(mptr_orig->fptr,mptr_orig->mhptr,matdir_orig.strtblk,sh)) {
	    printf("EcatWriteVolume_S16 : can not read PetVolume subheader %0d in '%s'\n",
		    matnum,mptr_orig->fname);
	    free(sh);
	    return ECATSHFJ_FAIL;
	  }
	}
	matrix_close(mptr_orig);
      }
    }
    sh->data_type = SunShort;
    sh->num_dimensions = 3;
    sh->x_dimension = EcatSizeX(uei);
    sh->y_dimension = EcatSizeY(uei);
    sh->z_dimension = EcatSizeZ(uei);
    sh->x_pixel_size = EcatVoxSizeX(uei)/10.0;
    sh->y_pixel_size = EcatVoxSizeY(uei)/10.0;
    sh->z_pixel_size = EcatVoxSizeZ(uei)/10.0;

    sh->frame_duration = EcatVolDurTime(uei,t);
    sh->frame_start_time = EcatVolStartTime(uei,t);
    sh->processing_code = EcatProcCode(uei);;

    min=indata[0];
    max=indata[0];
    for (bin=1;bin<data_size;bin++) {
      if (indata[bin]>max) max=indata[bin];
      if (indata[bin]<min) min=indata[bin];
    }
    if (EcatVolScale(uei,t) > 0.0)
      sh->scale_factor = EcatVolScale(uei,t);
    else
      sh->scale_factor = 1.0;
    sh->image_min = min;
    sh->image_max = max;
    mat_write_image_subheader(mptr->fptr,mptr->mhptr,matdir.strtblk,sh);

    /* write data */
    file_pos = blkno*MatBLKSIZE;
    if (fseek(mptr->fptr,file_pos,0) == EOF) {
      printf(" EcatWriteVolume_S16> !!! error with fseek position %0d for %s\n",file_pos,mptr->fname);
      free(sh);
      return ECATSHFJ_FAIL;
    } else if (file_data_to_host((char *)indata,nblks,sh->data_type) != 0) {
      printf("EcatWriteVolume_S16 : Error with data_to_host from %s\n",mptr->fname);
      free(sh);
      return ECATSHFJ_FAIL;
    } else if (fwrite(indata,sizeof(short),data_size,mptr->fptr)!=data_size) {
      printf(" EcatWriteVolume_S16> !!! error writting matrix %0d in %s\n",matnum,mptr->fname);
      free(sh);
      return ECATSHFJ_FAIL;
    }
    mh_update(mptr);
    matrix_close(mptr);
    free(sh);
    return ECATSHFJ_OK;
}
/*}}}  */

/*{{{  EcatWriteVolume_FLOAT*/
/* ------------------------------------------------------------------------*/
int EcatWriteVolume_FLOAT(UnifiedEcatInfo *uei , float *indata, int t)
/* ------------------------------------------------------------------------*/
{
    float            min,max,scale_factor;
    int              data_size,bin,nblks;
    short           *sdata;

    data_size=EcatSizeX(uei)*EcatSizeY(uei)*EcatSizeZ(uei);
    if (data_size <= 0.0) {
      printf(" EcatWriteVolume_FLOAT > !!! Error: image size should be provided in struct uei (uei->size)\n");
      return ECATSHFJ_FAIL;
    }

    if (EcatDataCalibrated(uei)) {
      /* Only if data are in Bq/cc is the field mh->calibration_factor used */
      if ((strcmp(EcatCalibUnit(uei),ECATSHFJ_BQCC) == 0) && (EcatCalib(uei) > 0.0)) {
	for (bin=0;bin<data_size;bin++) indata[bin] /= EcatCalib(uei);
      }
    } else {
      if ((strcmp(EcatCalibUnit(uei),ECATSHFJ_BQCC) != 0) && (EcatCalib(uei) > 0.0)) {
	for (bin=0;bin<data_size;bin++) indata[bin] *= EcatCalib(uei);
      }
    }

    min=indata[0];
    max=indata[0];
    for (bin=1;bin<data_size;bin++) {
      if (indata[bin]>max) max=indata[bin];
      if (indata[bin]<min) min=indata[bin];
    }
    scale_factor = 1.0;
    if ((max > 0.0) && (max > -min))     scale_factor = max/32767;
    else if ((min < 0.0) &&(min < -max)) scale_factor = -min/32767.0;
    EcatVolScale(uei,t) = scale_factor;

    /* conversion to short data data */
    nblks = (data_size*sizeof(short)+MatBLKSIZE-1)/MatBLKSIZE;
    sdata = (short *)calloc(nblks*MatBLKSIZE/sizeof(short), sizeof(short));
    for (bin=0; bin<data_size; bin++) sdata[bin] = (short) (indata[bin]/scale_factor);
    if (EcatWriteVolume_S16(uei, sdata, t) != ECATSHFJ_OK) {
      printf(" EcatWriteVolume_Float > !!! Error: can not write data to %s\n",uei->fname);
      free(sdata);
      return ECATSHFJ_FAIL;
    }
    free(sdata);
    return ECATSHFJ_OK;
}
/*}}}  */

