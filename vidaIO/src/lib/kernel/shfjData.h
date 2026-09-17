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
