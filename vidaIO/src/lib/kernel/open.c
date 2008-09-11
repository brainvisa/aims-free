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
 
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vidaIO/kernel/vidaio.h>
#include <string.h>

int GetPlane(S_DATA* s_data);
int GetFrame(S_DATA* s_data);
int SGiveFullHandler(const char *nm,FILE **fp_im,FILE **fp_hd,FILE **fp_info,
                     S_DATA **infos,struct AnalyzeDb *ana_db,consigne *cons);


/*----------------------------------------------------------------------------
static void ComputeGso(cons, f_gran_min, f_gran_max, mode)
   fonction ancillaire privee de vidaopen.
   
   Cette fonction calcule les valeurs de scale et offset qui seront utilisees
   par vidaRead() afin de normaliser les donnees.

      ENTREES: f_gran_min et f_gran_max: extrema quantifies de l'image sur 
               toutes les frames
               mode = STRETCH_AROUND_ZERO ou STRETCH_ANYWAY

      SORTIES: cons->scale et cons->offset
  
   Cette fonction est appelee par des fonctions de chargement d'images qui
   desirent etendre la dynamique des images disques. Cette extension a lieu 
   essentiellement dans les cas de scale/offset differents pour les images 
   ou les frames d''un meme fichier. 

   Cette extension peut soit etre faite de deux manieres :
      -Avec conservation du zero des donnees physiques dans les donnees 16 bits
      qui vont etre chargees en memoire (l''ofset sera d''ailleurs egal a 0.0,
               >>> STRETCH_AROUND_ZERO  <<<
      -Sans conservation du zero des donnees physiques dans les donnees 
       16 bits.
      c''est ce mode qui recupere le maximum de dynamique.
               >>> STRETCH_ANYWAY  <<<

   En traitement des images le mode Avec conservation est preferee. 
   Deux options
   sont virtuellement disponibles dans ce code mais actuellement une seule est
   activee. Peut etre une directive de compilation serait appropriee.

----------------------------------------------------------------------------*/

static void ComputeGso(consigne *cons,float f_gran_min,float f_gran_max,
		       int mode)
{ 
  switch(mode) 
    { 
    case STRETCH_ANYWAY      : 
      cons->scale  = (f_gran_max-f_gran_min)/ 65535.0;
      cons->offset = (f_gran_max+f_gran_min+ cons->scale) / 2.0;
      break;
    case STRETCH_AROUND_ZERO :
    default                      :
      if ((float)fabs((double)f_gran_min)>f_gran_max)
	{
	  cons->scale = f_gran_min / -32768.0;
	  cons->offset = 0.0;
	}
      else
	{ 
	  cons->scale = f_gran_max / 32767.0;
	  cons->offset = 0.0;
	}
      break;
    }
}

/*----------------------------------------------------------------------------
VIDAim *VidaOpen(nom_fic, cons)
        
   Cette fonction est la fonction publique qui fait l'ouverture d'une image 
   Vida
   Un nouveau type est defini dans vidaio VIDAim, qu'il faut concevoir comme
   un pointeur d''image VIDA. Au meme titre qu'un pointeur de fichier FILE, ce 
   pointeur possede des informations sur le fichier qu'il decrit qui lui 
   permet de repondre a des sollicitations de lecture ecriture. Comme pour
   les pointeurs de type FILE, le mode d'ouverture conditionne essentiellement
   les lectures qui seront faites ensuite. Les donnees du fichiers sont 
   chargees  
   au seul moment de la lecture du fichier vimg (une des trois composantes 
   disque d''un fichier vida).

   ENTREES:
   char    *nom_de_fichier
   consigne cons structure de donnees de type consigne qui donne les infos 
           necessaires


   SORTIES:
   Un pointeur VIDAim; ce dernier prend soit une valeur legale si tout c'est
   passe correctement. Sinon une valeur NULL est retournee qui doit etre 
   testee.
----------------------------------------------------------------------------*/

VIDAim *VidaOpen( const char *nom_fic,consigne *cons )
{
  VIDAim      *vp;

  FILE        *fp_im, *fp_info, *fp_hd;
  S_DATA      *infos = NULL;
  AnaHdr      *ana_db = NULL;
  consigne    *local;
  float       f_gran_min,f_gran_max,f_temp;
  float       scale,offset;
  int         i,j,max,min;
  int         status;
 
  if (!cons) 
    {
      fprintf(stderr,"VidaOpen: consigne vide\n");
      return(NULL);
    }
  vp = (VIDAim *)calloc(1,sizeof(VIDAim));
  if( vp == NULL ) 
    { 
      fprintf(stderr,"Erreur d''allocation de la structure vida\n");
      return(NULL);
    }
  vp->consign = (consigne *)calloc(1,sizeof(consigne));
  if( vp->consign == NULL ) 
    { 
      free(vp);
      fprintf(stderr,"Erreur d''allocation de la structure vp->consign\n");
      return(NULL);
    }
  local = (consigne *)cons;
  memcpy(vp->consign,local,sizeof(consigne));
  ana_db = (AnaHdr *)calloc(1,sizeof(AnaHdr));
  if( ana_db == NULL ) 
    { 
      free(vp->consign);
      free(vp);
      fprintf(stderr,"Erreur d''allocation de la structure ana_db\n");
      return(NULL);
    }
  status = SGiveFullHandler(nom_fic,&fp_im,&fp_hd,&fp_info,&infos,
			    ana_db,local);
  if( status != 0 ) 
    {
      free(ana_db);
      free(vp->consign);
      free(vp);
      return(NULL);
    }
  vp->ana_db   = ana_db;
  vp->shfj_db  = infos;
  vp->fpi      = fp_im;
  switch(local->mode[0])
    { 
    case 'r' :
      vp->fph = NULL;
      vp->fpf = NULL;
      switch(vp->consign->type)
	{ 
	case FX_SHORT_FL :
	  f_gran_max = GetMaxPl (vp->shfj_db,0,0) *
	    GetCalib (vp->shfj_db,0,0) +
	    GetOffset(vp->shfj_db,0,0);
	  f_gran_min = GetMinPl (vp->shfj_db,0,0) * 
	    GetCalib (vp->shfj_db,0,0)+ 
	    GetOffset(vp->shfj_db,0,0);
	  for(i =-1+GetFrame(vp->shfj_db);i>=0;i--)
	    for(j =-1+GetPlane(vp->shfj_db);j>=0;j--) 
	      { 
		f_temp = GetMaxPl(vp->shfj_db,j,i) * 
		  GetCalib(vp->shfj_db,j,i)+ 
		  GetOffset(vp->shfj_db,j,i);
		if (f_temp > f_gran_max) f_gran_max = f_temp;
		f_temp = GetMinPl(vp->shfj_db,j,i) *  
		  GetCalib(vp->shfj_db,j,i)+     
		  GetOffset(vp->shfj_db,j,i);
		if (f_temp < f_gran_min)  f_gran_min = f_temp;
	      }	
	  ComputeGso(vp->consign,f_gran_min,f_gran_max,STRETCH_AROUND_ZERO);
	  for(i = -1+GetFrame(vp->shfj_db);i>=0;i--)
	    for(j =-1+GetPlane(vp->shfj_db);j>=0;j--)
	      { 
		scale  = GetCalib(vp->shfj_db, j, i) / vp->consign->scale;
		offset = (GetOffset(vp->shfj_db,j,i) -
			  vp->consign->offset) / vp->consign->scale;
		f_temp = ((float)GetMaxPl(vp->shfj_db,j,i)* scale + offset);
		SetMaxPl(vp->shfj_db,j,i,(int)f_temp);
		f_temp = ((float)GetMinPl(vp->shfj_db,j,i)* scale + offset);
		SetMinPl(vp->shfj_db,j,i,(int)f_temp);
	      }
	  vp->ana_db->img.glmin = 32767;
	  vp->ana_db->img.glmax = -32768;
	  for(i =-1+GetFrame(vp->shfj_db);i>=0;i--)
	    for(j =-1+GetPlane(vp->shfj_db);j>=0;j--)
	      { 
		if ((max=GetMaxPl(vp->shfj_db,j,i))> vp->ana_db->img.glmax)
		  vp->ana_db->img.glmax = max;
		if ((min=GetMinPl(vp->shfj_db,j,i))< vp->ana_db->img.glmin)
		  vp->ana_db->img.glmin = min;
	      }
	  break;

	case FX_SHORT_FX :
	case FL_SHORT_FL :
	case FX_INT_FX   :
	case FX_INT_FL   :
	case FL_FLOAT_FL :
	default              : break;
	}
      break;
    case 'w' : vp->fph = fp_hd;
      vp->fpf = fp_info;
      break;
    default  : free(ana_db);
      free(vp->shfj_db);
      free(vp->consign);
      free(vp);
      fprintf(stderr,"VidaOpen: mode d''ouverture inconnu\n");
      return(NULL);
      break;
    }

  return(vp);
}
