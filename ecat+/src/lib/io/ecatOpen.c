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
 * MODULE NAME : ecatHeader.c            * TYPE     : Source
 * AUTHOR      : FROUIN V.               * CREATION : 19/05/2000
 * VERSION     : 0.1                     * REVISION :
 * LANGUAGE    : C++                     * EXAMPLE  :
 * DEVICE      : 
 ****************************************************************************
 *
 * DESCRIPTION : Ecat6-7 data reader class
 *               Version d'apres codes les codes de C. Comtat.
 *
 ****************************************************************************
 * REVISIONS :  DATE  |    AUTHOR    |       DESCRIPTION
 *--------------------|--------------|---------------------------------------
 *       07/09/2000   | COMTAT.C     |  EcatClose : en cas d'ecriture, le
 *                    |              |  main header est mis a jour lors de
 *                    |              |  fermeture du fichier
 *                    |              |
 *                    |              |  EcatOpen : gere le multi-lit
 *                    |              |
 *                    |              |  NEW : EcatVersion ()
 *                    |              |        EcatUeiCopy ()
 *                    |              |
 *--------------------|--------------|---------------------------------------
 *       07/09/2000   | FROUIN.V     | Add in uei : isotope code and 1/2 life
 *                    |              |   proc_code, proc_list & start/duration
 *                    |              | Add definition like applied_proc
 *                    |              |
 *--------------------|--------------|---------------------------------------
 *       16/11/2000   | COMTAT.C     | Remove in uei : isotope code and 1/2 life,
 *                    |              |                 rawlist_nb as is equal
 *                    |              |                 to size.t
 *                    |              | Add in uei    : radiopharmacuetical,
 *                    |              |                 Calibration Units,
 *                    |              |                 Data Calibrated ?
 *                    |              |
 *                    |              | Correct handling of mh->calibration:
 *                    |              |
 *                    |              |    Uncalibrated :
 *                    |              |
 *                    |              |      calibration factor == 1.0
 *                    |              |      calibration units = Ecat Counts/sec
 *                    |              |
 *                    |              |
 *                    |              |    Calibrated :
 *                    |              |
 *                    |              |      calibration factor is mandatory
 *                    |              |      calibration units = Bq/cc
 *                    |              |
 *                    |              |
 *                    |              |    Other :
 *                    |              |
 *                    |              |      calibration factor == 1.0
 *                    |              |      calibration units = Unknown
 *                    |              |
 *--------------------|--------------|---------------------------------------
 *                    |              |
 *       18/07/2001   | COMTAT.C     |   Remove error message if opened file
 *                    |              |   doesn't exist
 *                    |              |
 ****************************************************************************/
/*}}}  */

/* This macro should be set to allow swab() function to be defined.
   But if we do, things don't compile in ecat headers. */

/* #define _XOPEN_SOURCE */

/* Another hack (on linux/gcc at least) would be:
#include <features.h>
#define __USE_XOPEN
but it's maybe a bit too ugly (too specific).
So let's go on with this warning...
*/

#include <unistd.h>
#include <ecat/kernel/matrix.h>
#include <ecat+/io/io.h>
#include <string.h>
#include <float.h>
#include <limits.h>
#include <math.h>

static char *applied_proc[] = { /* dirty cause defintions may vary ...!!! */
  "Norm", "Atten_Meas", "Atten_Calc",
  "Smooth_X", "Smooth_Y", "Smooth_Z", "Scat2d", "Scat3d",
  "ArcPrc", "DecayPrc", "OnCompPrc", "Random","","","",""}; 



/* from matrix_extra.c in ecat library */
int mh_update(MatrixFile *mptr);

/*{{{  sortUniq*/
static int sortUniq( UnifiedEcatInfo *uei )
{
  int  *cpylist, *sulist,*suptr ;
  long *indice, *indiceptr;
  long indmin;
  int  i, j, min;
  
  if ( (sulist = (int *)calloc(uei->size.t, sizeof( int ))) == NULL)
    return(ECATSHFJ_FAIL);
  if ( (cpylist = (int *)calloc(uei->size.t, sizeof( int ))) == NULL)
    return(ECATSHFJ_FAIL);
  if ( (indice = (long *)calloc(uei->size.t, sizeof( long ))) == NULL)
      return(ECATSHFJ_FAIL);
  memcpy(cpylist, uei->rawlist, uei->size.t*sizeof(*cpylist));

  /* Tri */
  indiceptr = indice; suptr = sulist;
  for (i=0; i < uei->size.t; i++)
    {
      min = *(cpylist+0); indmin = 0;
      for (j=0; j < uei->size.t; j++)
	{
	  if (min > *(cpylist +j)) 
	    {
	      min = *(cpylist +j);
	      indmin = j;
	    }
	}
      *suptr++ = min;
      *indiceptr++ = indmin;
      *(cpylist + indmin) = INT_MAX;
    }

  if ( (uei->sortuniqlist = (long *)calloc(uei->size.t, sizeof(long))) == NULL)
    return(ECATSHFJ_FAIL);
  indiceptr = uei->sortuniqlist; 
  for (i=0; i < uei->size.t; i++ ) *indiceptr++ = *(indice+i);

  free( sulist ); free( cpylist); free( indice );
  return( ECATSHFJ_OK );
}
/*}}}  */

/*{{{  EcatVersion*/
#include <stdio.h>
#ifndef _WIN32
#include <netinet/in.h>
// #else
// #include <winsock.h>
#endif

/* "Home made" guess of the ECAT version of the file from scratch ! */

int EcatVersion(fname)

char *fname;
{
  FILE  *cfPtr;
  int         software_version;
  long  int   ecat6_offset=48, ecat7_offset=46;
  short int   word,sv;

  if ((cfPtr = fopen(fname,"r")) == NULL)
    return -1;
  else {
    if (fseek(cfPtr,ecat6_offset,SEEK_SET) != 0) {
      fclose(cfPtr);
      return -2;
    }
    if (fread(&word,sizeof(short int),1,cfPtr) != 1) {
      fclose(cfPtr);
      return -3;
    }
    if (ntohs(1) == 1) {
      swab((void*) &word, (void *) &sv,(ssize_t)2);
    } else {
      sv = word;
    }/* Ecat 6.x */
    if ((sv == 6)||((sv >= 60)&&(sv < 70))) software_version=6;
    else {
      if (fseek(cfPtr,ecat7_offset,SEEK_SET) != 0) {
	fclose(cfPtr);
	return -4;
      }
      if (fread(&word,sizeof(short int),1,cfPtr) != 1) {
	fclose(cfPtr);
	return -5;
      }
      if (ntohs(1) != 1) {
	swab((void*)&word,(void*)&sv,(ssize_t)2);
      } else {
	sv=word;
      } /* Ecat 7.x */
      if ((sv == 7)||((sv >= 70)&&(sv < 80))) software_version=7;
      else software_version=0;
    }
  }
  fclose(cfPtr);
  return software_version;
}
/*}}}  */

/*{{{  EcatClose*/
int EcatClose( UnifiedEcatInfo *uei )
{
  MatrixFile       *mptr,*mptr_orig;
  Main_header      *mh;
  char             *ch_in,*ch_out,*suffixe;
  if (uei) {
    if (uei->open_mode[0] == 'w') {
      /* Main header is set just before closing the file */
      if ((mptr = matrix_open(uei->fname,MAT_OPEN_EXISTING, MAT_UNKNOWN_FTYPE)) == NULL) {
	printf(" EcatClose> !!! Error: can not re-open %s\n",uei->fname);
	free( uei->fname );
	free(uei);
	return ECATSHFJ_FAIL;
      }
      mh = (Main_header*)  calloc(1,sizeof(Main_header));
      /* Take main header from original file if exist and is ecat7 formatted */
      if (EcatVersion(uei->original_filename) == 7) {
	if ((mptr_orig = matrix_open(uei->original_filename, MAT_READ_ONLY, MAT_UNKNOWN_FTYPE)) != NULL) {
	  ch_out = (caddr_t) mh;
	  ch_in = (caddr_t) mptr_orig->mhptr;
	  memcpy(ch_out,ch_in,sizeof(Main_header));
	  matrix_close(mptr_orig);
	}
      }
      /* Update some fields of the main header */
      mh->sw_version = V7;
      suffixe = strrchr(uei->fname,'.');
      if (suffixe) {
	suffixe++;
	if (strcmp(suffixe,"v") == 0) mh->file_type = PetVolume;
	else if (strcmp(suffixe,"p") == 0) mh->file_type = PetProjection;
	else {
	  printf(" EcatClose> !!! Error: file name '%s' should end by '.v' (volume) or by '.p' (projection)\n",
		 uei->fname);
	  matrix_close(mptr);
	  free( uei->fname );
	  free(uei);
	  return ECATSHFJ_FAIL;
	}
      } else {
	printf(" EcatClose> !!! Error: file name '%s' should end by '.v' (volume) or by '.p' (projection)\n",
		uei->fname);
	matrix_close(mptr);
	free( uei->fname );
	free(uei);
	return ECATSHFJ_FAIL;   
      }
      mh->num_planes = EcatSizeZ(uei);
      mh->num_bed_pos = 0;

      if (strcmp(EcatCalibUnit(uei),ECATSHFJ_BQCC) == 0) {
	/* Only if data are in Bq/cc is the field mh->calibration_factor used */
	mh->calibration_units = 1;
	mh->calibration_factor = EcatCalib(uei);
      } else if (strcmp(EcatCalibUnit(uei),ECATSHFJ_ECS) == 0) {
	mh->calibration_units = 0;
	mh->calibration_factor = 1.0;
      } else if (strcmp(EcatCalibUnit(uei),ECATSHFJ_LABELS) == 0) {
	mh->calibration_units = 1001;
	mh->calibration_factor = 1.0;
      } else {
	mh->calibration_units = -1;
	mh->calibration_factor = 1.0;
      }

      strncpy(mh->radiopharmaceutical,EcatRadiopharmaceutical(uei),32);
      mh->radiopharmaceutical[31] = '\0';
      
      mh->isotope_halflife = uei->isotope_halflife ;
      
      if (mat_write_main_header(mptr->fptr, mh)) {
	printf(" EcatClose> !!! Error: can not write main header in %s\n",mptr->fname);
	matrix_close(mptr);
	free( uei->fname );
	free(uei);
	return ECATSHFJ_FAIL;
      }
      free(mh);
      /* force writting */
      matrix_close(mptr);
      if ((mptr = matrix_open(uei->fname,MAT_OPEN_EXISTING, MAT_UNKNOWN_FTYPE)) == NULL) {
	printf(" EcatClose> !!! Error: can not re-open %s\n",uei->fname);
	free( uei->fname );
	free(uei);
	return ECATSHFJ_FAIL;
      }
      /* Update number of frames, gates, and bed positions */
      if (mh_update(mptr)) {
	printf(" EcatClose> !!! Error: can not update main header in %s\n",mptr->fname);
	matrix_close(mptr);
	free( uei->fname );
	free(uei);
	return ECATSHFJ_FAIL;
      }
      matrix_close(mptr);
    }
    free( uei->fname );
    free(uei);
    return ECATSHFJ_OK;
  } else       printf("null uei") ;

  return ECATSHFJ_FAIL;
}
/*}}}  */

/*{{{  EcatOpen*/
UnifiedEcatInfo *EcatOpen( char *fname, char *mode )
{
  /*{{{  declaration*/
  UnifiedEcatInfo   *uei;
  struct MatDir      matdir;
  struct Matval      matval;
  struct matdirnode *matnode;
  Image_subheader   *sh;
  Main_header       *mh;
  MatrixFile        *mptr;
  int                matrix, frame_is_const, gate_is_const, nmatrix, bin,
		     first_bed,last_bed,bed,plane_overlap,bed_motion;
  float              epsilon_mm = 1e-6;
  int                v, cumul;
  int		     ver;

  first_bed = 0;
  last_bed = 0;
  /*}}}  */
  /*{{{  creation of strct. uei*/
  if ( (uei =(UnifiedEcatInfo*) calloc(1, sizeof(UnifiedEcatInfo))) == NULL)
    {
      printf("EcatOpen : Can't calloc uei\n");
      return( ECATSHFJ_FAIL );
    }
  
  if ( (uei->fname = (char *)calloc(strlen(fname)+1,sizeof(char))) == NULL)
    {
      printf("EcatOpen: can't calloc fname uei struct\n");
      free(uei);
      return( ECATSHFJ_FAIL );
    }
  strncpy(uei->fname,fname,strlen(fname)+1);
  uei->fname[strlen(fname)] = '\0';
  strncpy(uei->open_mode,mode,4);
  uei->open_mode[3] = '\0';
  /*}}}  */
  switch( mode[0] )
    {
      case 'w' :
	/*{{{  open new file*/
	mh = (Main_header*) calloc(1,sizeof(Main_header));
	mh->sw_version = V7;
	mh->file_type = PetVolume;
	if ((mptr = matrix_create(fname,MAT_CREATE,mh)) == NULL) {
	  /* printf("EcatOpen : Can't open new file %s\n", fname); */
	  free( uei->fname );
	  free(uei);
	  free(mh);
	  return( ECATSHFJ_FAIL );
	}
	free(mh);
	/*}}}  */
	break;
      case 'r' :
	/*{{{  open existing file*/
	/*{{{  open file*/
	ver = EcatVersion( fname );
	if (ver <= 0)
	  {
	    free( uei->fname );
	    free( uei );
	    return ECATSHFJ_FAIL;
	  }
	if ((mptr = matrix_open( fname, MAT_READ_ONLY, MAT_UNKNOWN_FTYPE)) == NULL) {
	  free( uei->fname );
	  free(uei);
	  return ECATSHFJ_FAIL;
	} else if (mptr->mhptr->file_type != PetVolume &&
		   mptr->mhptr->file_type != PetImage  &&
		   mptr->mhptr->file_type != PetProjection ) {
	  printf("EcatOpen: file type %0d for %s not supported\n",
		 mptr->mhptr->file_type,mptr->fname);
	  matrix_close(mptr);
	  free( uei->fname );
	  free(uei);
	  return ECATSHFJ_FAIL;
	} else if (mptr->dirlist->nmats == 0) {
	  printf("EcatOpen: %s contains NO matrix\n",
		 mptr->fname);
	  matrix_close(mptr);
	  free( uei->fname );
	  free(uei);
	  return ECATSHFJ_FAIL;
	}
	
	uei->trueFileType = mptr->mhptr->file_type;
	uei->zero_start_time = mptr->mhptr->scan_start_time;
	
	if ((mptr->mhptr->file_type == PetVolume) || (mptr->mhptr->file_type == PetImage))
	  strcpy(uei->category, "IMAGE");
	else if (mptr->mhptr->file_type == PetProjection)
	  strcpy(uei->category, "PROJECTION");
	
	strncpy(uei->radiopharmaceutical,mptr->mhptr->radiopharmaceutical,32);
	uei->radiopharmaceutical[199] = '\0';
	
	uei->isotope_halflife = mptr->mhptr->isotope_halflife;
	      
	strncpy(uei->original_filename,fname,200);
	uei->original_filename[199] = '\0';
	/*}}}  */
	/*{{{  reading of first sub-header*/
	/* -------------------------------Recuperation du subheader de reference */
	/* ------------------------------- a partir de la premiere matrice       */
	matnode = mptr->dirlist->first;
	if ((sh = (Image_subheader*) calloc(1,sizeof(Image_subheader))) == NULL) {
	  printf("EcatOpen: can't calloc sh\n");
	  matrix_close(mptr);
	  free( uei->fname );
	  free(uei);
	  return ECATSHFJ_FAIL;
	} else if (matrix_find(mptr,matnode->matnum,&matdir)) {
	  printf("EcatOpen : can not find matrix %0d in %s\n",
		 matnode->matnum,mptr->fname);
	  free(sh);
	  matrix_close(mptr);
	  free( uei->fname );
	  free(uei);
	  return ECATSHFJ_FAIL;
	} else if (mat_read_image_subheader(mptr->fptr,mptr->mhptr,matdir.strtblk,sh)) {
	  printf("EcatOpen : can not read PetVolume subheader %0d in '%s'\n",
		  matnode->matnum,mptr->fname);
	  free(sh);
	  matrix_close(mptr);
	  free( uei->fname );
	  free(uei);
	  return ECATSHFJ_FAIL;
	} else if (sh->data_type != SunShort) {
	  printf("EcatOpen : data type of matrix %0d is %0d\n",
		 matnode->matnum,sh->data_type);
	  free(sh);
	  matrix_close(mptr);
	  free( uei->fname );
	  free(uei);
	  return ECATSHFJ_FAIL;
	}

	if ((mptr->mhptr->file_type == PetImage) && (sh->z_dimension > 1 )) /*Pb : should be eq to 1 */
	  {
	    if (sh->z_dimension ==  mptr->mhptr->num_planes)
	      uei->trueFileType = (int) PetVolume;
	    else
	      {
		printf("EcatREadVolume_S16  : !!! Error : for a PetImage,  number of plane should be 1 (%0d)\n",sh->z_dimension);
		free(sh);
		matrix_close(mptr);
		free( uei->fname );
		free(uei);
		return ECATSHFJ_FAIL;
	      }
	  }
	/*}}}  */
	/*{{{  load uei according to first sub-header*/
	uei->size.x      = sh->x_dimension;
	uei->size.y      = sh->y_dimension;
	if (mptr->mhptr->num_bed_pos > 0) { /* Multi-bed acquisition */
	  matnode = mptr->dirlist->first;
	  mat_numdoc(matnode->matnum,&matval);
	  first_bed = 0;
	  last_bed = 0;
	  for (bed=1;bed<=mptr->mhptr->num_bed_pos;bed++) {
	    if (matrix_find(mptr,mat_numcod(matval.frame,matval.plane,matval.gate,0,bed),&matdir) == 0)
	      last_bed = bed;
	  }
	  if (last_bed > first_bed) {
	    if (mptr->mhptr->bed_offset[0] > 0.0) bed_motion = 1;
	    else bed_motion = -1;
	    plane_overlap = (int) ((float) mptr->mhptr->num_planes -
				   (float) bed_motion*((mptr->mhptr->bed_offset[0])/mptr->mhptr->plane_separation) + 0.5);
	    if (plane_overlap < 0 || plane_overlap > mptr->mhptr->num_planes) {
	      printf("EcatREadVolume_S16  : !!! Error : number of plane overlap = %0d\n",plane_overlap);
	      free(sh);
	      matrix_close(mptr);
	      free( uei->fname );
	      free(uei);
	      return ECATSHFJ_FAIL;
	    }
	    uei->size.z = (mptr->mhptr->num_planes) + (last_bed - first_bed)*(mptr->mhptr->num_planes - plane_overlap);
	  } else uei->size.z      = mptr->mhptr->num_planes;
	} else if ( EcatFileType(uei) == PetVolume || EcatFileType( uei ) == PetProjection)
	  uei->size.z      = sh->z_dimension;
	else if ( EcatFileType( uei ) == PetImage) 
	  uei->size.z      = mptr->mhptr->num_planes;
	
	/* uei->size.t deja affect par sortUniq */     
	uei->voxelsize.x = 10.0 * sh->x_pixel_size;
	uei->voxelsize.y = 10.0 * sh->y_pixel_size;
	uei->voxelsize.z = 10.0 * sh->z_pixel_size;
	uei->voxelsize.t = 1.0;
	uei->proc_code = sh->processing_code;
	if ((sh->processing_code) > 0) {
	  for (v=0, cumul = 0;v<16; v++)
	    if ((sh->processing_code & (1<<v)) != 0) 
	      {
		cumul += strlen( applied_proc[v] )+ 1; /*to account for space car*/
	      }
	  uei->proc_list = calloc(cumul+1, 1); /* to account for \0*/
	  for (v=0;v<16; v++)
	    if ((sh->processing_code & (1<<v)) != 0) 
	      {
		strcat(uei->proc_list, applied_proc[v]);
		strcat(uei->proc_list, " ");
	      }
	}
	if (fabs(sh->scale_factor - 1.0) < FLT_MIN) {
	  /* Labels */
	  EcatCalib(uei) = 1.0;
	  strncpy(EcatCalibUnit(uei),ECATSHFJ_LABELS,127);
	} else if (mptr->mhptr->calibration_units == 1 && mptr->mhptr->calibration_factor != 0.0) {
	  /* calibrated, in Bq/cc */
	  EcatCalib(uei) = mptr->mhptr->calibration_factor;
	  strncpy(EcatCalibUnit(uei),ECATSHFJ_BQCC,127);
	} else if (mptr->mhptr->calibration_units == 0) {
	  /* uncalibrated, in ECS */
	  EcatCalib(uei) = 1.0;
	  strncpy(EcatCalibUnit(uei),ECATSHFJ_ECS,127);
	} else {
	  /* unknown .... */
	  EcatCalib(uei) = 1.0;
	  strncpy(EcatCalibUnit(uei),ECATSHFJ_UNKN,127);
	}
	
	EcatDataCalibrated(uei) = 0;
	/*}}}  */
	/*{{{  gated or dynamic acquisition ? How many volumes*/
	/* ----- On considere comme dynamique les acquis multi frame ou multigate*/
	/* ---- Attention on ne peut avoir simultanement multi frame ET multigate*/
	matnode = mptr->dirlist->first;
	mat_numdoc(matnode->matnum,&matval);
	frame_is_const = matval.frame;
	gate_is_const  = matval.gate;
	nmatrix = 0;
	for (matrix=0;matrix< mptr->dirlist->nmats;matrix++) {
	  mat_numdoc(matnode->matnum,&matval);
	  /* !!! PetImage, one matrix per plane) */
	  if (matval.plane == 1 && matval.data == 0 && matval.bed == 0) {
	    frame_is_const &= (matval.frame == frame_is_const);
	    gate_is_const  &= (matval.gate  == gate_is_const);
	    nmatrix++;
	  }
	  matnode = matnode->next;
	}
	if ( !frame_is_const && !gate_is_const) 
	  {
	    printf("EcatOpen : Image %s must be either multi gate or multi frame exclusively\n", mptr->fname);
	    free(sh);
	    matrix_close(mptr);
	    free( uei->fname );
	    free(uei);
	    return( ECATSHFJ_FAIL );
	  }
	if ( !gate_is_const ) uei->dyn_type = ECATSHFJ_GATED;
	else                  uei->dyn_type = ECATSHFJ_FRAMED;
	uei->size.t = nmatrix;
	/*}}}  */
	/*{{{  raw and ordred sorted matrices*/
	if ((uei->rawlist = (int *)calloc(uei->size.t,sizeof(int))) == NULL) {
	  printf("EcatOpen: can't calloc rawlist uei struct\n");
	  free(sh);
	  matrix_close(mptr);
	  free( uei->fname );
	  free(uei);
	  return ECATSHFJ_FAIL;
	}
	matnode = mptr->dirlist->first;
	bin = 0;
	for (matrix=0; matrix< mptr->dirlist->nmats; matrix++) {
	  mat_numdoc(matnode->matnum,&matval);
	  if (matval.plane == 1 && matval.data == 0 && matval.bed == 0) {
	    if (uei->dyn_type == ECATSHFJ_FRAMED) {
	      *( uei->rawlist + bin ) = matval.frame;
	    } else {
	      *( uei->rawlist + bin ) = matval.gate;
	    }
	    bin++;
	  }
	  matnode = matnode->next;
	}
	if (sortUniq( uei ) == ECATSHFJ_FAIL)  {
	  printf("EcatOpen: can't calloc rawlist uei struct\n");
	  free(sh);
	  matrix_close(mptr);
	  free( uei->fname );
	  free(uei);
	  return( ECATSHFJ_FAIL );
	}

	/*}}}  */
	/*{{{  Allocation  des structs scale et time*/
	/*{{{  memory allocation*/
	if ((uei->scale = (float *)calloc(uei->size.t,sizeof(float))) == NULL) {
	  printf("EcatOpen: can't calloc scale or time uei struct\n");
	  free(sh);
	  matrix_close(mptr);
	  free( uei->fname );
	  free(uei);
	  return( ECATSHFJ_FAIL );
	} else if ((uei->start_time = (int *)calloc(uei->size.t,
						       sizeof(int))) == NULL) {
	  printf("EcatOpen: can't calloc scale or time uei struct\n");
	  free(sh);
	  matrix_close(mptr);
	  free( uei->fname );
	  free(uei);
	  return( ECATSHFJ_FAIL );
	} else if ((uei->duration_time = (int *)calloc(uei->size.t,
							 sizeof(int))) == NULL) {
	  printf("EcatOpen: can't calloc scale or time uei struct\n");
	  free(sh);
	  matrix_close(mptr);
	  free( uei->fname );
	  free(uei);
	  return( ECATSHFJ_FAIL );
	}
	/*}}}  */
	matnode = mptr->dirlist->first;
	bin = 0;
	for (matrix=0; matrix < mptr->dirlist->nmats; matrix++) {
	  mat_numdoc(matnode->matnum,&matval);
	  if (matval.plane == 1 && matval.data == 0 && matval.bed == 0) {
	    if (matrix_find(mptr,matnode->matnum,&matdir)) {
	      printf("EcatOpen_PetVolumeType: can not find matrix %0d in %s\n",matnode->matnum,mptr->fname);
	      free(sh);
	      matrix_close(mptr);
	      free( uei->fname );
	      free(uei);
	      return( ECATSHFJ_FAIL );
	    } else if (mat_read_image_subheader(mptr->fptr,mptr->mhptr,
						matdir.strtblk,sh)) {
	      printf("EcatOpen : can not read PetVolume subheader %0d in '%s'\n",
		     matnode->matnum,mptr->fname);
	      free(sh);
	      matrix_close(mptr);
	      free( uei->fname );
	      free(uei);
	      return( ECATSHFJ_FAIL );
	    }
	    /* test taille de l'image par rapport a uei */
	    if (sh->x_dimension != uei->size.x || sh->y_dimension != uei->size.y ||
		fabs(uei->voxelsize.x - 10.0 * sh->x_pixel_size) > epsilon_mm ||
		fabs(uei->voxelsize.y - 10.0 * sh->y_pixel_size) > epsilon_mm ||
		fabs(uei->voxelsize.z - 10.0 * sh->z_pixel_size) > epsilon_mm) {
	      printf("EcatOpen : matrix size varies from matrix to matrix\n");
	      printf("           uei : %0d X %0d pixels, %f X %f X %f mm^3\n",
		     uei->size.x,uei->size.y,uei->voxelsize.x,uei->voxelsize.y,uei->voxelsize.z);
	      printf("           mat : %0d X %0d pixels, %f X %f X %f mm^3\n",
		     sh->x_dimension,sh->y_dimension,10.0 * sh->x_pixel_size,
		     10.0 * sh->y_pixel_size,10.0 * sh->z_pixel_size);
	      printf("     uei - mat : %g X %g X %g mm^3\n",
		     uei->voxelsize.x - 10.0 * sh->x_pixel_size,
		     uei->voxelsize.y - 10.0 * sh->y_pixel_size,
		     uei->voxelsize.z - 10.0 * sh->z_pixel_size);
	      free(sh);
	      matrix_close(mptr);
	      free( uei->fname );
	      free(uei);
	      return( ECATSHFJ_FAIL );
	    }
	    if (last_bed == first_bed && EcatFileType(uei) == PetVolume) {
	      if (fabs(10.0*sh->z_dimension - uei->size.z) < FLT_MIN) {
		printf("EcatOpen : matrix size varies from matrix to matrix\n");
		free(sh);
		matrix_close(mptr);
		free( uei->fname );
		free(uei);
		return( ECATSHFJ_FAIL );
	      }
	    }
	    if ( sh->processing_code != uei->proc_code ) {
	      printf("EcatOpen : matrix processing code varies from matrix to matrix\n");
	      free(sh);
	      matrix_close(mptr);
	      free( uei->fname );
	      free(uei);
	      return( ECATSHFJ_FAIL );
	    }
	    
	    if (strcmp(EcatCalibUnit(uei),ECATSHFJ_LABELS) == 0) {
	      if (fabs(sh->scale_factor - 1.0) > FLT_MIN) {
		printf("EcatOpen : not all matrices are labels\n");
		free(sh);
		matrix_close(mptr);
		free( uei->fname );
		free(uei);
		return( ECATSHFJ_FAIL );
	      }
	    } else {
	      if (fabs(sh->scale_factor - 1.0) < FLT_MIN) {
		printf("EcatOpen : some matrices are labels\n");
		free(sh);
		matrix_close(mptr);
		free( uei->fname );
		free(uei);
		return( ECATSHFJ_FAIL );
	      }
	    }
	
	    *( uei->scale + bin ) = (sh->scale_factor>0.0 ? sh->scale_factor:1.0);
	    /* on garde le calibration factor a part                             */
	    /* *( uei->scale + bin )     *= mptr->mhptr->calibration_factor;*/
	    if ( uei->dyn_type == ECATSHFJ_FRAMED ) {
	      *( uei->start_time + bin )    = sh->frame_start_time;
	      *( uei->duration_time + bin ) = sh->frame_duration;
	    } else {
	      *( uei->start_time + bin )    = sh->frame_start_time;
	      /* better to keep frame duration */
	      *( uei->duration_time + bin ) = sh->frame_duration;
	    }
	    bin++;
	  }
	  matnode = matnode->next;
	}
	/*}}}  */
	free( sh );
	/*}}}  */
	break;
      default  :
	printf("ecatOpen : open mode should be 'w' or 'r'\n");
	return( ECATSHFJ_FAIL );
	break;
    }
  matrix_close(mptr);
  return( uei );
}
/*}}}  */

/*{{{  EcatUeiCopy*/
/* ------------------------------------------------------------------------*/
int EcatUeiCopy(UnifiedEcatInfo *uei_in , UnifiedEcatInfo *uei_out)
/* ------------------------------------------------------------------------*/
{
  printf("ecat uei copy\n");
  int matrix, len;
  uei_out->size.x = uei_in->size.x;
  uei_out->size.y = uei_in->size.y;
  uei_out->size.z = uei_in->size.z;
  uei_out->size.t = uei_in->size.t;

  uei_out->voxelsize.x = uei_in->voxelsize.x;
  uei_out->voxelsize.y = uei_in->voxelsize.y;
  uei_out->voxelsize.z = uei_in->voxelsize.z;
  uei_out->voxelsize.t = uei_in->voxelsize.t;

  uei_out->calib = uei_in->calib;
  uei_out->dyn_type = uei_in->dyn_type;
  uei_out->size.t = uei_in->size.t;
  strncpy(uei_out->original_filename,uei_in->original_filename,200);
  uei_out->original_filename[199] = '\0';
  strncpy(uei_out->radiopharmaceutical,uei_in->radiopharmaceutical,200);
  uei_out->radiopharmaceutical[199] = '\0';
  uei_out->isotope_halflife = uei_in->isotope_halflife ;

  strncpy(uei_out->calibUnits,uei_in->calibUnits,128);
  uei_out->calibUnits[127] = '\0';
  uei_out->dataCalibrated = uei_in->dataCalibrated;

  uei_out->proc_code = uei_in->proc_code;
  if (uei_in->proc_code > 0) {
    len = strlen(uei_in->proc_list)+1;
    uei_out->proc_list = (char *)calloc(len,sizeof(char));
    strncpy(uei_out->proc_list,uei_in->proc_list,len);
    uei_out->proc_list[len-1] = '\0';
 }
  uei_out->dyn_type = uei_in->dyn_type;
  uei_out->rawlist = (int *)calloc(uei_out->size.t,sizeof(int));
  uei_out->scale = (float *)calloc(uei_out->size.t,sizeof(float));
  uei_out->start_time = (int *)calloc(uei_out->size.t,sizeof(int));
  uei_out->duration_time = (int *)calloc(uei_out->size.t,sizeof(int));
  uei_out->sortuniqlist = (long *)calloc(uei_out->size.t,sizeof(long));
  for (matrix=0;matrix<uei_out->size.t;matrix++) {
    uei_out->rawlist[matrix] = uei_in->rawlist[matrix];
    uei_out->scale[matrix] = uei_in->scale[matrix];
    uei_out->start_time[matrix] = uei_in->start_time[matrix];
    uei_out->duration_time[matrix] = uei_in->duration_time[matrix];
    uei_out->sortuniqlist[matrix] = uei_in->sortuniqlist[matrix];
  }
  return ECATSHFJ_OK;
}
/*}}}  */
