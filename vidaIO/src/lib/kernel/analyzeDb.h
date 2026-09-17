
#ifndef VIDAIO_KERNEL_ANALYZEDB_H
#define VIDAIO_KERNEL_ANALYZEDB_H

#ifdef __cplusplus
  extern "C" {
#endif

/* SCCS control */
#ifdef SCCS_CONTROL
static char SCCSIDH_analyze_db[] = "@(#)analyze_db.h	1.2\t7/30/90";
#endif

struct HdrKey
{ 
  int       size;
  char      data_type[10];
  char      name[18];
  int       extents;
  short int session_error;
  char      regular;
  char      hkey_un0;
};


struct ImageDimension
{ 
  short dim[16];
  short bitpix;
  short dim_un0;
  float pixdim[16];
  int   glmax,glmin;
};


struct HdrHistory 
{ 
  char descrip[80];
  char aux_file[24];
  char orient;
  char originator[10];
  char generated[10];
  char scannum[10];
  char patient_id[10];
  char exp_date[10];
  char exp_time[10];
  char hist_un0[3];
  int  views;
  int  vols_added;
  int  start_field;
  int  field_skip;
  int  omax,omin;
  int  smax,smin;
};


struct AnalyzeDb 
{ 
  struct HdrKey          key;
  struct ImageDimension  img;
  struct HdrHistory      his;
};


#ifdef __cplusplus
  }
#endif

#endif
