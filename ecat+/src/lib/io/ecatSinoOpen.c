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
 *
 *--------------------|--------------|---------------------------------------
 *                    |              |
 *       15/01/2013   | RASTELLO.F   |   Use V72 for version value when ecat7
 *                    |              |   format, for biograph reads
 *                    |              |
 */
#include <unistd.h>
#include <string.h>
#include <ecat/kernel/matrix.h>
#include <ecat+/io/io.h>
#include <float.h>
#include <limits.h>
#include <math.h>




/* IL faudrait mettre cei en dénominateur commun dans un ficheir
ou mieux le mettre au niveau de ecat+ */

int EcatSinoVersion(fname)
  char *fname;
{
  FILE  *cfPtr;
  int         software_version;
  long  int   ecat6_offset=48, ecat7_offset=46;
  short int   word,sv;
  
  if ((cfPtr = fopen(fname,"r")) == NULL){
    printf("EcatSinoVersion : Can't open file !\n") ;
    return -1;
  }else {
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

int EcatGetCommInfoMainHeader( fname, uesi )
  char                  *fname;
  UnifiedEcatSinoInfo   *uesi;
{
  MatrixFile        *mptr = NULL;

  unsigned int j ;
  if ((mptr = matrix_open( fname, MAT_READ_ONLY, MAT_UNKNOWN_FTYPE))
      == NULL)
    {
      return ECATSHFJ_FAIL;
    } 
  else if (  mptr->mhptr->file_type != Short3dSinogram &&
	     mptr->mhptr->file_type != Float3dSinogram  )
    {
      printf("EcatSinoOpen: file type %0d for %s not supported\n",
	     mptr->mhptr->file_type,mptr->fname);
      matrix_close(mptr);
      free( uesi->fname );
      free(uesi);
      return ECATSHFJ_FAIL;
    } 
  else if (mptr->dirlist->nmats == 0)
    {
      printf("EcatSinoOpen: %s contains NO matrix\n",
	     mptr->fname);
      matrix_close(mptr);
      return ECATSHFJ_FAIL;
    }

  /* Operations de filtrage des donnees actuellement connues */
  if ( (mptr->mhptr->num_bed_pos != 0 && mptr->mhptr->num_gates != 1) || 
       (mptr->mhptr->num_bed_pos != 0 && mptr->mhptr->num_frames != 1) || 
       (mptr->mhptr->num_frames != 1 && mptr->mhptr->num_gates != 1) ) 
    {
      fprintf(stderr, "EcatSinoOpen: %s contains multiples frames/bed/gates\n",
	     mptr->fname);
      fprintf(stderr, "              format not currently managed\n");
      matrix_close(mptr);
      return ECATSHFJ_FAIL;      
    }
  
/*   if ( ( mptr->mhptr->num_gates  != 1 ) || */
/*        ( mptr->mhptr->num_bed_pos != 0 )) */
/*     { */
/*       fprintf(stderr, "EcatSinoOpen: %s contains multiples frames/bed\n", */
/* 	     mptr->fname); */
/*       fprintf(stderr, "              format not currently managed\n"); */
/*       matrix_close(mptr); */
/*       return ECATSHFJ_FAIL; */
/*     }        */
/*   if ( mptr->mhptr->num_frames != 1 ) */
/*     { */
/*       fprintf(stderr, "EcatSinoOpen: %s contains multiples frames\n", */
/* 	     mptr->fname); */
/*       fprintf(stderr, "              format not currently managed\n"); */
/*       matrix_close(mptr); */
/*       return ECATSHFJ_FAIL; */
/*     } */

  /* Necessaire pour des questions de découplage de compialtions */
  if (mptr->mhptr->file_type == Short3dSinogram )
    sprintf(uesi->file_type, "Short3dSinogram");
  if  (mptr->mhptr->file_type == Float3dSinogram )
    sprintf(uesi->file_type, "Float3dSinogram");
  uesi->system_type               = mptr->mhptr->system_type;
  uesi->scan_start_time           = mptr->mhptr->scan_start_time;
  strncpy(uesi->isotope_code, mptr->mhptr->isotope_code,8);
  uesi->isotope_halflife          = mptr->mhptr->isotope_halflife;
  uesi->distance_scanned	  = mptr->mhptr->distance_scanned ;
  uesi->transaxial_fov		  = mptr->mhptr->transaxial_fov ;
  uesi->angular_compression	  = mptr->mhptr->angular_compression ;
  strncpy(uesi->study_name, mptr->mhptr->study_name,12);
  uesi->acquisition_type          = mptr->mhptr->acquisition_type;
  uesi->acquisition_mode          = mptr->mhptr->acquisition_mode;
  strncpy(uesi->original_file_name, mptr->mhptr->original_file_name,32);
  strncpy(uesi->radiopharmaceutical,mptr->mhptr->radiopharmaceutical,32);
  uesi->num_planes		  = mptr->mhptr->num_planes ;
  uesi->num_gates		  = mptr->mhptr->num_gates ;
  uesi->num_frames                = mptr->mhptr->num_frames;
  uesi->num_bed_pos               = mptr->mhptr->num_bed_pos;
  uesi->init_bed_position	  = mptr->mhptr->init_bed_position ;
  uesi->bin_size                  = mptr->mhptr->bin_size;
  uesi->plane_separation          = mptr->mhptr->plane_separation;
  uesi->lwr_true_thres		  = mptr->mhptr->lwr_true_thres ;
  uesi->upr_true_thres		  = mptr->mhptr->upr_true_thres ;
  uesi->branching_fraction	  = mptr->mhptr->branching_fraction ;
  uesi->dose_start_time		  = mptr->mhptr->dose_start_time ;
  uesi->patient_orientation 	  = mptr->mhptr->patient_orientation ;
  uesi->bed_elevation  = mptr->mhptr->bed_elevation ;
  uesi->coin_samp_mode  = mptr->mhptr->coin_samp_mode ;
  uesi->axial_samp_mode  = mptr->mhptr->axial_samp_mode ;
  uesi->calibration_factor  = mptr->mhptr->calibration_factor ;
  for( j = 0; j <15;++j )
    (uesi->bed_offset)[j] = (mptr->mhptr->bed_offset)[j];
  uesi->lwr_sctr_thres  = mptr->mhptr->lwr_sctr_thres ;
  uesi->dosage  = mptr->mhptr->dosage ;
  uesi->well_counter_factor  = mptr->mhptr->well_counter_factor ;
  uesi->septa_state  = mptr->mhptr->septa_state ;

  if (mptr) matrix_close( mptr );
  return ECATSHFJ_OK;
}

int EcatGetCommInfoSubHeader( fname, uesi )
  char                  *fname;
  UnifiedEcatSinoInfo   *uesi;

{
  MatrixFile        *mptr = NULL;
  struct MatDir      matdir;
  struct Matval      matval;
  struct matdirnode *matnode;
  Scan3D_subheader   *sh;
  int                i;
  
  if ((mptr = matrix_open( fname, MAT_READ_ONLY, MAT_UNKNOWN_FTYPE))
      == NULL)
    {
      return ECATSHFJ_FAIL;
    }
  matnode = mptr->dirlist->first;
  if ((sh = (Scan3D_subheader*) calloc(1,sizeof(Scan3D_subheader))) == NULL)
    {
      printf("EcatSinoOpen: can't calloc sh\n");
      matrix_close(mptr);
      return ECATSHFJ_FAIL;
    }
  matnode = mptr->dirlist->first;
  if ( !sh )
    if ((sh = (Scan3D_subheader*) calloc(1,sizeof(Scan3D_subheader))) == NULL)
      {
	printf("EcatSinoOpen: can't calloc sh\n");
	matrix_close(mptr);
	return ECATSHFJ_FAIL;
      }
  /* Read subheader */
  mat_numdoc(matnode->matnum,&matval);
  if (matrix_find(mptr,matnode->matnum,&matdir))
    {
      free(sh);
      matrix_close(mptr);
      return( ECATSHFJ_FAIL );
    }
  else
    if (mat_read_Scan3D_subheader(mptr->fptr,mptr->mhptr,
				 matdir.strtblk,sh))
      {
	free(sh);
	matrix_close(mptr);
	return( ECATSHFJ_FAIL );
      }
  
  /*Eventually get the common information from the main subheader */
  
  for (i = 0; i <64;++i)  
    (uesi->num_z_elements)[i] = (sh->num_z_elements)[i];
  uesi->num_dimensions         = sh->num_dimensions;
  uesi->num_r_elements         = sh->num_r_elements;
  uesi->num_angles             = sh->num_angles;
  uesi->corrections_applied    = sh->corrections_applied;
  uesi->ring_difference        = sh->ring_difference;
  uesi->axial_compression      = sh->axial_compression;
  uesi->storage_order          = sh->storage_order;
  uesi->x_resolution           = sh->x_resolution;
  uesi->v_resolution           = sh->v_resolution;
  uesi->z_resolution           = sh->z_resolution;
  uesi->w_resolution           = sh->w_resolution;
  
  /*Housekeeping */
  if ( mptr ) matrix_close( mptr );
  if ( sh ) free(sh);
  return ECATSHFJ_OK;
}

int EcatGetSpecificInfoSubHeader( fname, uesi )
  char                  *fname;
  UnifiedEcatSinoInfo   *uesi;

{
  MatrixFile        *mptr = NULL;
  struct MatDir      matdir;
  struct Matval      matval;
  struct matdirnode *matnode;
  Scan3D_subheader   *sh = NULL;
  int                i;
  scan3DSub            *currentUesiSub;
  int matrix;
  
  if ((mptr = matrix_open( fname, MAT_READ_ONLY, MAT_UNKNOWN_FTYPE))
      == NULL)
    {
      return ECATSHFJ_FAIL;
    }
  matnode = mptr->dirlist->first;
/*   bin = 0 */;
  for (matrix=0; matrix < mptr->dirlist->nmats; matrix++)
    {
      /* Sh allocation and SUBHEADER */
      if ( !sh )
	if ((sh = (Scan3D_subheader*) calloc(1,sizeof(Scan3D_subheader))) == NULL)
	  {
	    printf("EcatSinoOpen: can't calloc sh\n");
	    matrix_close(mptr);
	    return ECATSHFJ_FAIL;
	  }
      if (( currentUesiSub = (scan3DSub *)calloc(1,sizeof(scan3DSub))) == NULL)
	{
	  printf("EcatSinoOpen: can't calloc sh\n");
	  matrix_close(mptr);
	  return ECATSHFJ_FAIL;
	}
      currentUesiSub->next = NULL;
      if (!( uesi->subfirst) )
	{
	  uesi->subfirst = uesi->sublast = currentUesiSub;	  
	}
      else
	{
	  uesi->sublast->next = currentUesiSub;
	  uesi->sublast = currentUesiSub;
	}
      

      /* Read subheader */
      mat_numdoc(matnode->matnum,&matval);
      if (matrix_find(mptr,matnode->matnum,&matdir))
	{
	  free(sh);
	  matrix_close(mptr);
	  return( ECATSHFJ_FAIL );
	}
      else
	if (mat_read_Scan3D_subheader(mptr->fptr,mptr->mhptr,
				     matdir.strtblk,sh))
	  {
	    free(sh);
	    matrix_close(mptr);
	    return( ECATSHFJ_FAIL );
	  }
      
      /* Eventually get info from subheader */
      currentUesiSub->prompts                      = sh->prompts;
      currentUesiSub->delayed                      = sh->delayed;
      currentUesiSub->multiples                    = sh->multiples;
      currentUesiSub->net_trues                    = sh->net_trues;
      currentUesiSub->tot_avg_cor                  = sh->tot_avg_cor;
      currentUesiSub->tot_avg_uncor                = sh->tot_avg_uncor;
      currentUesiSub->total_coin_rate              = sh->total_coin_rate;
      currentUesiSub->loss_correction_fctr         = sh->loss_correction_fctr;
      currentUesiSub->frame_start_time             = sh->frame_start_time;
      currentUesiSub->frame_duration               = sh->frame_duration;
      currentUesiSub->gate_duration                = sh->gate_duration;      
      currentUesiSub->scan_min                     = sh->scan_min ;
      currentUesiSub->scan_max                     = sh->scan_max ;
      for (i = 0; i <128;++i)  
	(currentUesiSub->uncor_singles)[i]         = (sh->uncor_singles)[i];
      
      
      /* Increment */
      free( sh ); sh = NULL;
      matnode = matnode->next;
    }
  
  /*Housekeeping */
  if ( mptr ) matrix_close( mptr );
  if ( sh ) free(sh);
  //if( currentUesiSub ) free(currentUesiSub) ;
  return ECATSHFJ_OK;
}


void EcatSinoFree( uesi )
  UnifiedEcatSinoInfo   *uesi;
{
  scan3DSub               *it, *itnext ;
  
  if (uesi)
    {
      free( uesi->fname );
      it=uesi->subfirst ;
      int i = 0 ;
      while (it!=/*uesi->sublast*/NULL)
	if (it){
	  itnext=it->next ;
	  if( it != itnext ){
	    printf( "Deleting scan3DSub %d : %p before %p\n", i, it, itnext ) ;
	    printf( "frame_start_time %d \n", it->frame_start_time) ;
	    free(it) ;
/* 	    printf( "Deleted\n" ) ; */
	  }
	  it = itnext ;
	  ++i ;
	}
      free(uesi);
    }
}

UnifiedEcatSinoInfo* EcatSinoOpen( char *fname, char *mode  )
{
  UnifiedEcatSinoInfo   *uesi;
  int		        ver;
  Main_header           *mh;
  scan3DSub               *it;
  MatrixFile        *mptr = NULL;


  /*   printf("EcatSinoOpen ouvre %s\n", fname); */
  /*  creation of strct. uei*/
  if ( (uesi =(UnifiedEcatSinoInfo*) calloc(1, sizeof(UnifiedEcatSinoInfo)))
       == NULL)
    {
      printf("EcatSinoOpen : Can't calloc uei\n");
      return( ECATSHFJ_FAIL );
    }
  
  if ( (uesi->fname = (char *)calloc(strlen(fname)+1,sizeof(char))) == NULL)
    {
      printf("EcatOpen: can't calloc fname uesi struct\n");
      if (uesi)
	{
	  for (it=uesi->subfirst; it!=uesi->sublast; it=it->next)
	    if (it) free(it);
	  free(uesi);
	}
      return( ECATSHFJ_FAIL );
    }
  strncpy(uesi->fname,fname,strlen(fname)+1);
  uesi->fname[strlen(fname)] = '\0';
  strncpy(uesi->open_mode,mode,4);
  uesi->open_mode[3] = '\0';
  uesi->subfirst =  uesi->sublast = NULL;

  switch( mode[0] )
    {
    case 'w' :
      /*{{{  open new file*/
      mh = (Main_header*) calloc(1,sizeof(Main_header));
      //FR: V7 -> V72 for biograph reads
      mh->sw_version = V72;
      //mh->file_type = Short3dSinogram ;
      if ((mptr = matrix_create(fname,MAT_CREATE,mh)) == NULL) {
	printf("EcatSinoOpen : Can't open new file %s\n", fname);
	EcatSinoFree( uesi ) ;
/* 	if (uesi) */
/* 	  { */
/* 	    free( uesi->fname ); */
/* 	    for (it=uesi->subfirst; it!=uesi->sublast; it=it->next) */
/* 	      if (it) free(it); */
/* 	    free(uesi); */
/* 	  } */
	free(mh);
	return( ECATSHFJ_FAIL );
      }
      free(mh);
      if( mptr )
	matrix_close(mptr) ;
      /*}}}  */
      break;
    case 'r':
      /* Verification de la version du fichier accede */
      if ( (ver = EcatSinoVersion( fname )) <= 0)
	{
	  printf("EcatSinoOpen : Can't read version of %s\n", fname);
	  EcatSinoFree( uesi ); return ECATSHFJ_FAIL;
	}

      /* Acces aux information du main header */
      if ( EcatGetCommInfoMainHeader( fname, uesi ) == ECATSHFJ_FAIL )
	{
	  printf("EcatSinoOpen : Can't access main header of %s\n", fname);
	  EcatSinoFree( uesi ); return ECATSHFJ_FAIL;
	}

      /* Acces aux information du premier subheader */
      /* Initialisation à partir des infos          */
      /* communes à tous les subheaders             */
      if ( EcatGetCommInfoSubHeader( fname, uesi ) == ECATSHFJ_FAIL )
	{
	  printf("EcatSinoOpen : Can't access first sub header of %s\n", fname);
	  EcatSinoFree( uesi ); return ECATSHFJ_FAIL;
	}

      /* Acces aux information de tous les subheaders */
      /* Initialisation à partir des infos            */
      /* communes à tous les subheaders              */
      if ( EcatGetSpecificInfoSubHeader( fname, uesi ) == ECATSHFJ_FAIL )
	{
	  printf("EcatSinoOpen : Can't access all sub headers\n");
	  EcatSinoFree( uesi ); return ECATSHFJ_FAIL;
	}
      break;
    default  :
      printf("ecatOpen : open mode should be 'w' or 'r'\n");
      return( ECATSHFJ_FAIL );
      break;
    }

  return( uesi );
}


int EcatSinoClose(UnifiedEcatSinoInfo  *uesi )
{
  if (uesi)
    {
      if (uesi->open_mode[0] == 'w')
	{
	}
      else
	{
	if (uesi)
		EcatSinoFree( uesi ) ;	 
	}
    }
  return ECATSHFJ_OK;
}

