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

/****************************************************************************
 * PROJECT     : E.C.A.T.S.H.F.J. Library
 * MODULE NAME : general.h               * TYPE     : header
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
 *       07/09/2000   | COMTAT C.    |   Modification de la structure
 *                    |              |   UnifiedEcatInfo :
 *                    |              |
 *                    |              |     - suppression de la variable mptr
 *                    |              |       (MatrixFile), afin de ne pas
 *                    |              |       avoir a inclure
 *                    |              |       ecat/kernel/matrix.h dans les
 *                    |              |       routines faisant appel a la
 *                    |              |       librairie ecat+
 *                    |              |
 *                    |              |     - ajout de la variable fname
 *                    |              |
 *                    |              |     - ajout de la variable
 *                    |              |       original_filename pour
 *                    |              |       referencer le fichier d'ou
 *                    |              |       proviennent les donnees.
 *                    |              |
 *                    |              |     - ajout de la variable open_mode,
 *                    |              |       pour savoir si fname est ouvert
 *                    |              |       pour ecriture ou lecture
 *                    |              |
 *--------------------|--------------|---------------------------------------
 *       07/11/2000   | Frouin V.    |   Ajout de champs dans la struture uei
 *                    |              |    - isotope, halflife
 *                    |              |    - proc_code et proc_list
 *                    |              |
 *--------------------|--------------|---------------------------------------
 *       16/11/2000   | Frouin V.    |  Remplac. de isotope et halflife par
 *                    | Comtat C.    |  radiopharmaceutic
 *                    |              |  Ajout de calibUnits et dataCalibrated
 *                    |              |
 ****************************************************************************/
#ifndef ECATSHFJ_IO_GENERAL_H
#define ECATSHFJ_IO_GENERAL_H

#include <stdlib.h>
#include <time.h>

#define ECATSHFJ_FAIL   0
#define ECATSHFJ_OK     1

#define ECATSHFJ_FRAMED 0
#define ECATSHFJ_GATED  1

#define ECATSHFJ_ECS    "ECAT counts/sec"
#define ECATSHFJ_BQCC   "Bq/cc"
#define ECATSHFJ_LABELS "Labels"
#define ECATSHFJ_UNKN    "Unknown"

typedef struct 
{
  char          *fname;
  struct 
  {
    int         x, y, z, t;
  }             size;
  struct 
  {
    float       x, y, z, t;
  }             voxelsize;
  
  /*            Pointeur vers les infos sur le timing               */
  float        *scale;
  int          *start_time, *duration_time;
  int           dyn_type;
  
  /*            FileType dans le main header peut etre faux         */
  /*            trueFileType contient la valeur correct :           */
  /*                Image, Volume, Projection                       */
  /*            Cette variable sera initialisée avec les valeurs    */
  /*            enum de ecat : PetImage, PetVolume et PetProjection */
  int           trueFileType;
  
  /*            Argument du fichier ouvert                          */
  char         original_filename[200];
                                   /* nom du fichier ouvert en mode */
                                   /* lecture (== fname) ou         */
                                   /* du  fichier d'ou              */
                                   /* proviennent les donnees qui   */
                                   /* vont etre ecrites             */
  char         open_mode[4];       /* mode d'ouverture du fichier   */



  /*            Pointeur vers les infos sur le timing               */
  int          *rawlist;      /* list des framegate number tels     */
                              /* que presentes dans les headers     */

  long         *sortuniqlist; /*index de parcours logique de rawlist*/
                              /* contient size.t elements           */


  /* Pointeurs sur des informations complémentai destinées au header*/
  char          radiopharmaceutical[200];
  float         isotope_halflife;
  
  float         calib;
  char          calibUnits[128];
  char          dataCalibrated;
  
  
  char         *proc_list;
  int           proc_code;
  
  char         category[64];
  time_t       zero_start_time;
} UnifiedEcatInfo;


typedef struct scan3dsub
{
  struct scan3dsub *next; 
  /* ---------- SUBHEADER --------------------------------*/

  unsigned int gate_duration;
  int r_wave_offset;
  int num_accepted_beats;
  float scale_factor;
  short scan_min;
  short scan_max;
  int prompts;
  int delayed;
  int multiples;
  int net_trues;
  float tot_avg_cor;
  float tot_avg_uncor;
  int total_coin_rate;
  unsigned int frame_start_time;
  unsigned int frame_duration;
  float loss_correction_fctr;
  float uncor_singles[128];
  
  /* --------------------------------------------- */
#ifdef QUOICESTCA
  MatrixFile		*matfile ;	/* pointer to parent */
  DataSetType		mat_type ;	/* type of matrix? */
  MatrixDataType	data_type ;	/* type of data */
  int			data_size ;	/* size of data in bytes */
  int			xdim;		/* dimensions of data */
  int			ydim;		/* y dimension */
  int			zdim;		/* for volumes */
  float			scale_factor ;	/* valid if data is int? */
  float			pixel_size;	/* xdim data spacing (cm) */
  float			y_size;		/* ydim data spacing (cm) */
  float			z_size;		/* zdim data spacing (cm) */
  float			data_min;	/* min value of data */
  float			data_max;	/* max value of data */
  float        		x_origin;       /* x origin of data */
  float        		y_origin;       /* y origin of data */
  float        		z_origin;       /* z origin of data */
#endif
} scan3DSub;


typedef struct 
{
  char        *fname;
  char         open_mode[4];       /* mode d'ouverture du fichier   */

  /* ------- MAIN HEADER -----------------------*/
  char magic_number[14];
  char original_file_name[32];
  short sw_version;
  short system_type;
  char file_type[32];
  char serial_number[10];
  short align_0;
  unsigned int scan_start_time;
  char isotope_code[8];
  float isotope_halflife;
  char radiopharmaceutical[32];
  float gantry_tilt;
  float gantry_rotation;
  float bed_elevation;
  float intrinsic_tilt;
  short wobble_speed;
  short transm_source_type;
  float distance_scanned;
  float transaxial_fov;
  short angular_compression;
  short coin_samp_mode;
  short axial_samp_mode;
  short align_1;
  float calibration_factor;
  short calibration_units;
  short calibration_units_label;
  short compression_code;
  char study_name[12];
  char patient_id[16];
  char patient_name[32];
  char patient_sex[1];
  char patient_dexterity[1];
  float patient_age;
  float patient_height;
  float patient_weight;
  int patient_birth_date;
  char physician_name[32];
  char operator_name[32];
  char study_description[32];
  short acquisition_type;
  short patient_orientation;
  char facility_name[20];
  short num_planes;
  short num_frames;
  short num_gates;
  short num_bed_pos;
  float init_bed_position;
  float bed_offset[15];
  float plane_separation;
  short lwr_sctr_thres;
  short lwr_true_thres;
  short upr_true_thres;
  char user_process_code[10];
  short acquisition_mode;
  short align_2;
  float bin_size;
  float branching_fraction;
  unsigned int dose_start_time;
  float dosage;
  float well_counter_factor;
  char data_units[32];
  short septa_state;
  short align_3;
  
  /* A mon avis ce sont des champs des subheaders qui sont communes 
     a toutes les données ... */

  short data_type;
  short num_dimensions;
  short num_r_elements;
  short num_angles;
  short corrections_applied;
  short num_z_elements[64];
  short ring_difference;
  short storage_order;
  short axial_compression;
  float x_resolution;
  float v_resolution;
  float z_resolution;
  float w_resolution;



  scan3DSub *subfirst, *sublast;

} UnifiedEcatSinoInfo;


#endif
