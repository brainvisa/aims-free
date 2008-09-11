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

#ifndef VIDAIO_KERNEL_VIDAIO_H
#define VIDAIO_KERNEL_VIDAIO_H 

/* #ifndef bool_t
#define bool_t int
#endif

#ifndef enum_t
#define enum_t int
#endif */
/* BUG : ces definitions essayaient de corriger un probleme sur Solaris 2.5:
         on ne pouvait pas inclure <rpc/xdr.h> directement.
	 En fait il faut inclure <rpc/rpc.h> avant ou a la place (cf manpage).
	 (cette modif n'etait pas propre et provoquait des warnings sur 
	  Solaris 2.7)
*/

#include <stdio.h>
#include <vidaIO/kernel/analyzeDb.h>
#include <vidaIO/kernel/shfjData.h>

#define PROCESSOR_SPARC (char) 0
#define PROCESSOR_MIPS  (char) 1
#define vOUT_OF_RANGE " Normalizing out of rounding error range."

			
#ifdef __cplusplus
  extern "C" {
#endif


typedef struct AnalyzeDb AnaHdr;


typedef struct
{ 
  char            mode[4];
  int             size_x,size_y,size_z,size_t;
  float           psize_x,psize_y,psize_z,psize_t;
  int             scale_shift;
  int             type;
  float           scale,offset;
  int             unite;
}consigne;


typedef struct 
{ 
  FILE        *fpi;                  /* pointeur de fichier sur .vimg      */
  FILE        *fph;                  /* pointeur de fichier sur .vhdr      */
  FILE        *fpf;                  /* pointeur de fichier sur .vinfo     */
  AnaHdr      *ana_db;	             /* pointeur sur la strt analyze xdr   */
  S_DATA      *shfj_db;              /* ptr sur la str base de data SHFJ   */
  int         ref_count;             /* Not used                           */
  consigne    *consign;              /* Ptr sur les cons lien entre        */
                                     /*vidaopen et vidaread ou vidawrite   */
}VIDAim;


extern  char         *VidaAlloc(VIDAim *vp);
extern	VIDAim       *VidaOpen(const char *nom_fic,consigne *cons);
extern 	int           VidaClose(VIDAim *vp);
extern 	int           VidaRead(char *buf,int slice,int frame,VIDAim *vp);
extern 	int           VidaWrite(char *buf,int slice,int frame,VIDAim *vp);
extern  consigne      *VidaConsigne( int, ... );
extern 	int           VidaAbsMin();    /* pour tout le fic exprime en short  */
extern  int           VidaAbsMax();    /* pour tout le fic exprime en short  */
extern  int           VidaAbsZero();   /* pour tout le fic exprime en short  */


/*************************************************************
  Definititon des masques de controle et des type de formats
  Convention de nommage : FORMmemoire_type_FORMdisque
  Exemple               : FORMmemoire est FX ou FL
                          FORMdisque  est FX ou FL
                          type est int short char ou float
*************************************************************/
#define MSK_INT_FX   0x010000
#define FX_INT_FL    MSK_INT_FX + 1
#define FX_INT_FX    MSK_INT_FX + 2
#define MSK_SHORT_FX 0x020000
#define FX_SHORT_FL  MSK_SHORT_FX + 1
#define FX_SHORT_FX  MSK_SHORT_FX + 2
#define MSK_SHORT_FL 0x040000
#define FL_SHORT_FL  MSK_SHORT_FL + 1
#define MSK_FLOAT_FL 0x080000
#define FL_FLOAT_FL  MSK_FLOAT_FL + 1

#define STRETCH_AROUND_ZERO 1
#define STRETCH_ANYWAY      2

	
/***********************************************************************
  Definititon des entrees possible de VidaConsigne() dans le va_arg de
  vidaconsigne cf le formalisme de XView
***********************************************************************/
#define VMODE        1     /* Desc mode d'ouverture de l'image */
#define VMDTYPE      3     /* Desc type de l'image             */
#define VSIZE_X      4     /* Desc sizex de l'image            */
#define VSIZE_Y      5     /* Desc sizey de l'image            */
#define VSIZE_Z      6     /* Desc sizez de l'image            */
#define VSIZE_T      7     /* Desc sizet de l'image            */
#define VSCALE_SHIFT 8     /* Desc type de l'image	            */
#define VSIZE_PX     9     /* Taille X en mm                   */
#define VSIZE_PY    10     /* Taille Y en mm                   */
#define VSIZE_PZ    11     /* Taille Z en mm                   */
#define VSIZE_PT    12     /* Taille T en mm                   */
#define VUNITE      13     /* unite de quantification          */

/************************************** 
  Definititon des macros fonctions
**************************************/
#define VIDA_VIMG(v)   v->fpi
#define VIDA_VHDR(v)   (v->consign->mode == VIDA_LEC : v->fph; NULL)
#define VIDA_VINFO(v)  (v->consign->mode == VIDA_LEC : v->fpf; NULL)
#define VIDA_X(v)      v->ana_db->img.dim[1]
#define VIDA_Y(v)      v->ana_db->img.dim[2]
#define VIDA_Z(v)      v->ana_db->img.dim[3]
#define VIDA_T(v)      v->ana_db->img.dim[4]
#define VIDA_PX(v)     v->ana_db->img.pixdim[1]
#define VIDA_PY(v)     v->ana_db->img.pixdim[2]
#define VIDA_PZ(v)     v->ana_db->img.pixdim[3]
#define VIDA_PT(v)     v->ana_db->img.pixdim[4]
#define VIDA_TYPE(v)   v->consign->type
#define VIDA_UNITE(v)  v->consign->unite


#ifdef __cplusplus
  }
#endif

#endif
