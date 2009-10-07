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

#ifndef VIDAIO_KERNEL_SHFJDATA_H
#define VIDAIO_KERNEL_SHFJDATA_H


#include <sys/param.h>

#define	VIDA_NOCALIB      0
#define	VIDA_SANSUNITE    1
#define	VIDA_nCi          2
#define	VIDA_Bq           3

#ifdef __cplusplus
  extern "C" {
#endif


typedef struct
{ 
  unsigned char active;
  int start_time;	         /* debut de la frame  en ms*/
  int duration_time;	         /* duree de la frame en ms */
}S_TIME;


typedef struct
{ 
  unsigned char	active;
  int           min_pl;
  int           max_pl;
}S_EXT;


typedef struct
{ 
  unsigned char	active;
  int           coeff_calib;     /* produit du quantif scale factor*/
                                 /* par l'ecat calibration factor  */
  int           offset;          /* valeur de l'offset             */
                                 /* =0 si IRM et ECAT              */
}S_CALIB;


typedef struct
{ 
  unsigned char active;
  float         halflife;  /* periode de demi vie du radioisotope */
  float         decay;     /* decay correction factor             */
}S_ISOT;


typedef struct                 /* Structure globale de vinfo         */
{ 
  int         nb_pl;           /* nombre de plans                    */
  int         nb_fr;           /* nombre de frames                   */
  char        acq_device[128]; /* nom systeme d'acquisition          */
  S_ISOT  iso;                 /* gna gna                            */
  S_TIME  fr_time;             /* temps pour les acqui dynamiques    */
  S_EXT   val_ext;             /* valeurs extrema pour (frame, plan) */
  S_CALIB val_calib;           /* valeurs de calib pour (frame,plan) */
}S_DATA;


extern float GetCalib(S_DATA *s_data,int sl_numb,int fr_numb);
extern void  SetCalib(S_DATA *s_data,int sl_numb,int fr_numb,float coeff);
extern float GetOffset(S_DATA *s_data,int sl_numb,int fr_numb);
extern void  SetOffset(S_DATA *s_data,int sl_numb,int fr_numb,float offset);
extern int   GetTimeStart(S_DATA *s_data,int fr_numb);
extern void  SetTimeStart(S_DATA *s_data,int fr_numb,int start_time);
extern int   GetTimeDura(S_DATA *s_data,int fr_numb);
extern void  SetTimeDura(S_DATA *s_data,int fr_numb,int dura_time);
extern int   GetMinPl(S_DATA *s_data,int sl_numb,int fr_numb);
extern void  SetMinPl(S_DATA *s_data,int sl_numb,int fr_numb,int min);
extern int   GetMaxPl(S_DATA *s_data,int sl_numb,int fr_numb);
extern void  SetMaxPl(S_DATA *s_data,int sl_numb,int fr_numb,int max);


#ifdef __cplusplus
  }
#endif

#endif
