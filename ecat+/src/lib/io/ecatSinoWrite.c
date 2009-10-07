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

#include <string.h>
#include <limits.h>
#include <math.h>
#include <sys/types.h>
#include <ecat/kernel/matrix.h>
#include <ecat+/io/io.h>
#include <ecat+/io/rwToolBox.h>

/* from machine_indep.c in ecat library */
int file_data_to_host(char *dptr, int nblks, int data_type);

/* ------------------------------------------------------------------------*/
int EcatWriteSino_S16(UnifiedEcatSinoInfo *uesi , short indata[], int segment, 
		      int bed, int gate, int frame )
/* ------------------------------------------------------------------------*/
{
    int              matnum;
    
    MatrixFile      *mptr;
/*     MatrixFile      *mptr_orig; */
/*     struct MatDir    matdir, matdir_orig,dir_entry; */
    int i;
    Scan3D_subheader *sh;

    int 	     numFile = bed ;
    if ( gate > bed )
      numFile = gate ;
    if( frame > bed )
      numFile = frame ;

/*     if (t >= EcatSizeT(uei)) { */
/*       printf(" EcatWriteVolume_S16> !!! Error: volume number (%0d) should be smaller than %0d\n", */
/* 	     t,EcatSizeT(uei)); */
/*       return ECATSHFJ_FAIL; */
/*     } */
    /* Need to re-open output file */
    if ((mptr = matrix_open(uesi->fname,MAT_OPEN_EXISTING, MAT_UNKNOWN_FTYPE))
	== NULL) {
      printf(" EcatWriteVolume_S16> !!! Error: can not re-open %s\n",uesi->fname);
      return ECATSHFJ_FAIL;
    }

    /* Correct type (PetVolume) for output file ? */
/*     if (mptr->mhptr->file_type != PetVolume) { */
/*       printf(" EcatWriteVolume_S16> !!! Error: '%s' is not a PetVolume matrix file (%0d)\n", */
/* 	     mptr->fname,mptr->mhptr->file_type); */
/*       return ECATSHFJ_FAIL; */
/*     } */

    /* Determination of matnum(frame,plane=1,gate,data=0,bed=0) */
    matnum =  mat_numcod(frame+1,1,gate+1,0,bed) ;
/*     if (EcatRealFrameGate(uei,t) > 0) { */
/*       switch(uei->dyn_type) */
/* 	{ */
/* 	case ECATSHFJ_GATED : */
/* 	  matnum = mat_numcod(1,1,EcatRealFrameGate(uei,t),0,0); */
/* 	  break; */
/* 	case ECATSHFJ_FRAMED : */
/* 	default : */
/* 	  matnum = mat_numcod(EcatRealFrameGate(uei,t),1,1,0,0); */
/* 	  break; */
/*       } */
/*     } else matnum = mat_numcod(t+1,1,1,0,0); */

    /* Are the sizes provided in uei ? */
/*     data_size=EcatSizeX(uei)*EcatSizeY(uei)*EcatSizeZ(uei); */
/*     if (data_size <= 0.0) { */
/*       printf(" EcatWriteVolume_S16 > !!! Error: image size should be provided in struct uei (uei->size)\n"); */
/*       return ECATSHFJ_FAIL; */
/*     } else if (EcatVoxSizeX(uei)*EcatVoxSizeY(uei)*EcatVoxSizeZ(uei) <= 0.0) { */
/*       printf(" EcatWriteVolume_S16 > !!! Error: voxel size should be provided in struct uei (uei->voxelsize)\n"); */
/*       return ECATSHFJ_FAIL; */
/*     } */

    /* entry in directory */
/*     nblks = (data_size*sizeof(short)+MatBLKSIZE-1)/MatBLKSIZE; */
/*     if (matrix_find(mptr,matnum,&matdir) == -1) { */
      /* new entry */
/*       blkno = mat_enter(mptr->fptr,mptr->mhptr,matnum,nblks); */
/*       dir_entry.matnum = matnum; */
/*       dir_entry.strtblk = blkno; */
/*       dir_entry.endblk  = dir_entry.strtblk + nblks; */
/*       dir_entry.matstat = 1; */
/*       insert_mdir(dir_entry,mptr->dirlist); */
/*       matdir = dir_entry; */
/*     } else blkno = matdir.strtblk;  overwrite */

    /* Write Volume sub-header */
    if ((sh = ( Scan3D_subheader*) calloc(1,sizeof(Scan3D_subheader)) ) == NULL)
      {
	printf("EcatWriteSino_S16: can't calloc sh\n");
	return ECATSHFJ_FAIL;
      }
    /*     if (EcatVersion(uei->original_filename) == 7) { */
    /*       if ((mptr_orig = matrix_open(uei->original_filename, MAT_READ_ONLY, MAT_UNKNOWN_FTYPE)) != NULL) { */
    /* 	if ((mptr_orig->mhptr->file_type == PetVolume || mptr_orig->mhptr->file_type == PetImage) && */
    /* 	    (matrix_find(mptr_orig,matnum,&matdir_orig) == 0)) { */
    /* 	  if (mat_read_image_subheader(mptr_orig->fptr,mptr_orig->mhptr,matdir_orig.strtblk,sh)) { */
    /* 	    printf("EcatWriteVolume_S16 : can not read PetVolume subheader %0d in '%s'\n", */
    /* 		    matnum,mptr_orig->fname); */
    /* 	    free(sh); */
    /* 	    return ECATSHFJ_FAIL; */
    /* 	  } */
    /* 	} */
    /* 	matrix_close(mptr_orig); */
    /*       } */
    /*     } */
    
    
    /* init du Main header */
    mptr->mhptr->file_type = Short3dSinogram ;
    
    mptr->mhptr->system_type 		  = uesi->system_type;
    mptr->mhptr->scan_start_time	  = uesi->scan_start_time;
    strncpy(mptr->mhptr->isotope_code, uesi->isotope_code,8);
    mptr->mhptr->isotope_halflife	  = uesi->isotope_halflife;
    mptr->mhptr->distance_scanned	  = uesi->distance_scanned ;
    mptr->mhptr->transaxial_fov		  = uesi->transaxial_fov ;
    mptr->mhptr->acquisition_type =uesi->acquisition_type;
    mptr->mhptr->acquisition_mode =uesi->acquisition_mode;
    strncpy(mptr->mhptr->study_name, uesi->study_name,12);
    mptr->mhptr->angular_compression	  = uesi->angular_compression ;
    strncpy( mptr->mhptr->original_file_name,uesi->original_file_name,32);
    strncpy(mptr->mhptr->radiopharmaceutical,uesi->radiopharmaceutical,32);
    mptr->mhptr->num_planes		  = uesi->num_planes ;
    mptr->mhptr->num_gates		  = ( uesi->num_gates == 1 ? 0 : uesi->num_gates ) ;
    mptr->mhptr->num_frames 		  = uesi->num_frames;
    mptr->mhptr->num_bed_pos 		  = uesi->num_bed_pos;
    mptr->mhptr->init_bed_position	  = uesi->init_bed_position ;
    mptr->mhptr->bin_size		  = uesi->bin_size;
    mptr->mhptr->plane_separation 	  = uesi->plane_separation;
    mptr->mhptr->lwr_true_thres		  = uesi->lwr_true_thres ;
    mptr->mhptr->upr_true_thres		  = uesi->upr_true_thres ;
    mptr->mhptr->branching_fraction	  = uesi->branching_fraction ;
    mptr->mhptr->dose_start_time	  = uesi->dose_start_time ;
    mptr->mhptr->patient_orientation 	  = uesi->patient_orientation ;
    mptr->mhptr->bed_elevation    = uesi->bed_elevation ;
    mptr->mhptr->coin_samp_mode    = uesi->coin_samp_mode ;
    mptr->mhptr->axial_samp_mode    = uesi->axial_samp_mode ;
    mptr->mhptr->calibration_factor    = uesi->calibration_factor ;
    for (i = 0; i <15;++i)  
      (mptr->mhptr->bed_offset)[i] = (uesi->bed_offset)[i];
    mptr->mhptr->lwr_sctr_thres    = uesi->lwr_sctr_thres ;
    mptr->mhptr->dosage    = uesi->dosage ;
    mptr->mhptr->well_counter_factor    = uesi->well_counter_factor ;
    mptr->mhptr->septa_state    = uesi->septa_state ;
    
    
    /* init du Sub header */
    /* ici pb car on annonce potentiellement u mutlisegment alors que l'on ecrit sur la base
       d'un segment. C'est pour economiser la mémoire....*/
    sh->data_type      = SunShort;
    sh->scale_factor   = 1;/*Ici qq champs nécessaires interoges par la suite*/

    for (i = 0; i <64;++i)
      (sh->num_z_elements)[i] = (uesi->num_z_elements)[i];
    sh->num_dimensions = uesi->num_dimensions;
    sh->num_r_elements =  uesi->num_r_elements;
    sh->num_angles = uesi->num_angles;
    sh->corrections_applied = uesi->corrections_applied;
    sh->ring_difference = uesi->ring_difference ;
    sh->axial_compression = uesi->axial_compression;
    sh->storage_order = uesi->storage_order;
    sh->x_resolution = uesi->x_resolution;
    sh->v_resolution =  uesi->v_resolution;
    sh->z_resolution =  uesi->z_resolution;
    sh->w_resolution =  uesi->w_resolution;
    
    
    sh->prompts                      = uesi->subfirst[numFile].prompts;
    sh->delayed                      = uesi->subfirst[numFile].delayed;
    sh->multiples                    = uesi->subfirst[numFile].multiples;
    sh->net_trues                    = uesi->subfirst[numFile].net_trues;
    sh->tot_avg_cor                  = uesi->subfirst[numFile].tot_avg_cor;
    sh->tot_avg_uncor                = uesi->subfirst[numFile].tot_avg_uncor;
    sh->total_coin_rate              = uesi->subfirst[numFile].total_coin_rate;
    sh->loss_correction_fctr         = uesi->subfirst[numFile].loss_correction_fctr;
    sh->frame_start_time             = uesi->subfirst[numFile].frame_start_time;
    sh->frame_duration               = uesi->subfirst[numFile].frame_duration;
    sh->gate_duration                = uesi->subfirst[numFile].gate_duration;
    sh->scan_min                     = uesi->subfirst[numFile].scan_min ;
    sh->scan_max                     = uesi->subfirst[numFile].scan_max ;
    
    for (i = 0; i <128;++i)  
      (sh->uncor_singles)[i]         = ((uesi->subfirst[numFile]).uncor_singles)[i];
    
    if( Write_Scan3D_Ecat7_S16( mptr, sh, indata,  matnum,  segment) 
	== -1 )
      {
	printf(" EcatWriteSino_S16> !!! error writting matrix %0d in %s\n",matnum,mptr->fname);
	free(sh);
	return ECATSHFJ_FAIL;
      }
    
    matrix_close(mptr);
    free(sh);
    return ECATSHFJ_OK;
}

/* ------------------------------------------------------------------------*/
int EcatWriteSino_FLOAT( UnifiedEcatSinoInfo *uesi , float indata[], int segment, 
			 int bed, int gate, int frame )
/* ------------------------------------------------------------------------*/
{
    int              matnum;
    
    MatrixFile      *mptr;
    int i;
    Scan3D_subheader *sh;

    int 	     numFile = bed ;
    if ( gate > bed )
      numFile = gate ;
    if( frame > bed )
      numFile = frame ;
    
    //printf("ECAT WRITE SINO ! !!!!!!!!!!!!!!!!!!!!!!!!!\n") ;
    
    /* Need to re-open output file */
    if ((mptr = matrix_open(uesi->fname,MAT_OPEN_EXISTING, MAT_UNKNOWN_FTYPE))
	== NULL) {
      printf(" EcatWriteVolume_S16> !!! Error: can not re-open %s\n",uesi->fname);
      return ECATSHFJ_FAIL;
    }


    /* Determination of matnum(frame,plane=1,gate,data=0,bed=0) */
    matnum =  mat_numcod(frame+1,1,gate+1,0,bed) ;

    /* Write Volume sub-header */
    if ((sh = ( Scan3D_subheader*) calloc(1,sizeof(Scan3D_subheader)) ) == NULL)
      {
	printf("EcatWriteSino_S16: can't calloc sh\n");
	return ECATSHFJ_FAIL;
      }
    
    
    /* init du Main header */
    mptr->mhptr->file_type = Float3dSinogram ;
    
    mptr->mhptr->system_type 		  = uesi->system_type;
    mptr->mhptr->scan_start_time	  = uesi->scan_start_time;
    strncpy(mptr->mhptr->isotope_code, uesi->isotope_code,8);
    mptr->mhptr->isotope_halflife	  = uesi->isotope_halflife;
    mptr->mhptr->distance_scanned	  = uesi->distance_scanned ;
    mptr->mhptr->transaxial_fov		  = uesi->transaxial_fov ;
    mptr->mhptr->acquisition_type =uesi->acquisition_type;
    mptr->mhptr->acquisition_mode =uesi->acquisition_mode;
    strncpy(mptr->mhptr->study_name, uesi->study_name,12);
    mptr->mhptr->angular_compression	  = uesi->angular_compression ;
    strncpy( mptr->mhptr->original_file_name,uesi->original_file_name,32);
    strncpy(mptr->mhptr->radiopharmaceutical,uesi->radiopharmaceutical,32);
    mptr->mhptr->num_planes		  = uesi->num_planes ;
    mptr->mhptr->num_gates		  = ( uesi->num_gates == 1 ? 0 : uesi->num_gates ) ;
    mptr->mhptr->num_frames 		  = uesi->num_frames;
    mptr->mhptr->num_bed_pos 		  = uesi->num_bed_pos;
    mptr->mhptr->init_bed_position	  = uesi->init_bed_position ;
    mptr->mhptr->bin_size		  = uesi->bin_size;
    mptr->mhptr->plane_separation 	  = uesi->plane_separation;
    mptr->mhptr->lwr_true_thres		  = uesi->lwr_true_thres ;
    mptr->mhptr->upr_true_thres		  = uesi->upr_true_thres ;
    mptr->mhptr->branching_fraction	  = uesi->branching_fraction ;
    mptr->mhptr->dose_start_time	  = uesi->dose_start_time ;
    mptr->mhptr->patient_orientation 	  = uesi->patient_orientation ;
    mptr->mhptr->bed_elevation    = uesi->bed_elevation ;
    mptr->mhptr->coin_samp_mode    = uesi->coin_samp_mode ;
    mptr->mhptr->axial_samp_mode    = uesi->axial_samp_mode ;
    mptr->mhptr->calibration_factor    = uesi->calibration_factor ;
    for (i = 0; i <15;++i)  
      (mptr->mhptr->bed_offset)[i] = (uesi->bed_offset)[i];
    mptr->mhptr->lwr_sctr_thres    = uesi->lwr_sctr_thres ;
    mptr->mhptr->dosage    = uesi->dosage ;
    mptr->mhptr->well_counter_factor    = uesi->well_counter_factor ;
    mptr->mhptr->septa_state    = uesi->septa_state ;
    
    
    /* init du Sub header */
    /* ici pb car on annonce potentiellement u mutlisegment alors que l'on ecrit sur la base
       d'un segment. C'est pour economiser la mémoire....*/
    sh->data_type      = IeeeFloat;
    sh->scale_factor   = 1;/*Ici qq champs nécessaires interoges par la suite*/

    for (i = 0; i <64;++i)
      (sh->num_z_elements)[i] = (uesi->num_z_elements)[i];
    sh->num_dimensions = uesi->num_dimensions;
    sh->num_r_elements =  uesi->num_r_elements;
    sh->num_angles = uesi->num_angles;
    sh->corrections_applied = uesi->corrections_applied;
    sh->ring_difference = uesi->ring_difference ;
    sh->axial_compression = uesi->axial_compression;
    sh->storage_order = uesi->storage_order;
    sh->x_resolution = uesi->x_resolution;
    sh->v_resolution =  uesi->v_resolution;
    sh->z_resolution =  uesi->z_resolution;
    sh->w_resolution =  uesi->w_resolution;
    
    
    sh->prompts                      = uesi->subfirst[numFile].prompts;
    sh->delayed                      = uesi->subfirst[numFile].delayed;
    sh->multiples                    = uesi->subfirst[numFile].multiples;
    sh->net_trues                    = uesi->subfirst[numFile].net_trues;
    sh->tot_avg_cor                  = uesi->subfirst[numFile].tot_avg_cor;
    sh->tot_avg_uncor                = uesi->subfirst[numFile].tot_avg_uncor;
    sh->total_coin_rate              = uesi->subfirst[numFile].total_coin_rate;
    sh->loss_correction_fctr         = uesi->subfirst[numFile].loss_correction_fctr;
    sh->frame_start_time             = uesi->subfirst[numFile].frame_start_time;
    sh->frame_duration               = uesi->subfirst[numFile].frame_duration;
    sh->gate_duration                = uesi->subfirst[numFile].gate_duration;
    sh->scan_min                     = uesi->subfirst[numFile].scan_min ;
    sh->scan_max                     = uesi->subfirst[numFile].scan_max ;
    
    for (i = 0; i <128;++i)  
      (sh->uncor_singles)[i]         = ((uesi->subfirst[numFile]).uncor_singles)[i];
    
    //printf("Writing data\n") ;
    if( Write_Scan3D_Ecat7_FLOAT( mptr, sh, indata,  matnum,  segment) 
	== -1 )
      {
	printf(" EcatWriteSino_S16> !!! error writting matrix %0d in %s\n",matnum,mptr->fname);
	free(sh);
	return ECATSHFJ_FAIL;
      }
    
    matrix_close(mptr);
    free(sh);
    return ECATSHFJ_OK;
}
