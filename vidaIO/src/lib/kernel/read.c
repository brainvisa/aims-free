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

int GetPlane(S_DATA* s_data);
int PuRead(char* buff,int size,int nitems,FILE* stream,
           AnaHdr *ana_db);

#define vSIGN "libvidaio : vidaread :"

/*----------------------------------------------------------------------------
int VidaRead(char *buff, int slice, int frame, VIDAim *vp)

   Cette fonction fait l'ecriture commandee pour un VIDAim pointeur.
   Cette fonction mime l'execution d'un read et en mime les entrees sorties.
   Elle prend a la places des  parametres size et nitems les parametres
   coupes et frame.

   Le parametre VIDAim *vp permet d'interpreter les donnees placees dans le
   buffer. Ce parametre permet aussi de faire tous les scaling et fix'ing
   des min/max, scale et offset que requiert certain fichiers vida.

   ENTREE:
      char *buff buffer de donnees a interpreter suivant le type decrit dans
                 le VIDAim pointeur
      int slice frame : parametre de coupe et de frame dont on veut forcer
                 l'ecriture. On suppose un fichier en acces random.
      VIDAim *vp : Pointeur de l'image VIDAim qui contient les infos
                 qui permette d'interpreter correctement les donnees mise dans
                 le buffer.
    SORTIE :
      valeur rendue par la fonction :
             0 : si probleme ou EOF. Cf man de fread
             size_byte : si ecriture de size byte.
                  Ce nombre est issu de l'interpretation des donnees dans vp.
                  Ce doit etre exactement le nombre de byte rendu par le
                  pu_read de bas niveau qui est appele tout en bas


----------------------------------------------------------------------------*/

int VidaRead(char *buff,int slice,int frame,VIDAim *vp)
{
  short *s_ima,*s_pt1;
  float *f_pt1;
  int   size_im,size_byte,i,nbyte_read=0;
  float scale,offset,tmp;
  long  pos;
  short *s_to_rea;
  float *f_to_rea;

  /**********************************************/
  /* Calcul de la taille d'une coupe            */
  /**********************************************/  
  size_im = vp->ana_db->img.dim[1] * vp->ana_db->img.dim[2];
  size_byte = size_im * vp->ana_db->img.bitpix / 8;
  switch(vp->consign->type)
    {                           
      /**********************************************/
      /* Buffer d'entree: entiers sur 16 bits       */
      /* Buffer de sortie: flottants                */
      /**********************************************/
    case FL_FLOAT_FL : f_to_rea = (float *) buff;
      /**********************************************/
      /* Allocation d'un buffer intermediaire       */
      /**********************************************/
      s_ima = (short *)calloc(1,size_byte);
      if (!s_ima) 
	{ 
	  fprintf(stderr,
		  "VidaRead: Erreur d'allocation du buffer intermediaire\n");
	  return(0);
	}
      /**********************************************/
      /* On se positionne au debut de la coupe dans */
      /* le fichier .vimg                           */
      /**********************************************/
      pos = (long)(size_byte*(slice + frame*GetPlane(vp->shfj_db)));
      fseek(vp->fpi, pos, 0);
      /**********************************************/
      /* On lit la coupe dans le fichier .vimg      */
      /**********************************************/
      nbyte_read = PuRead((char*)s_ima,sizeof(short int),size_im, vp->fpi,
			  vp->ana_db);
      /**********************************************/
      /* Transformation des entiers en flottants    */
      /* a l'aide des facteurs de calibration et des*/
      /* offsets                                    */
      /**********************************************/
      for (i=size_im,f_pt1=f_to_rea,s_pt1=s_ima;i>0;i--,f_pt1++,s_pt1++)
	{ 
	  *f_pt1 = (float)*s_pt1 * GetCalib(vp->shfj_db,slice,frame) +
	    GetOffset(vp->shfj_db, slice, frame);
	}
      SetCalib (vp->shfj_db, slice, frame, 1.0);
      SetOffset(vp->shfj_db, slice, frame, 0.0);
      /**********************************************/
      /* Desallocation du buffer intermediaire      */
      /**********************************************/
      free(s_ima);
      break;
      /**********************************************/
      /* Buffer d'entree: entiers sur 16 bits       */
      /* Buffer de sortie: entiers sur 16 bits      */
      /* rescales entre -32768 et 32767             */
      /**********************************************/
    case FX_SHORT_FL : s_to_rea =(short *) buff;
      /**********************************************/
      /* On se positionne au debut de la coupe dans */
      /* le fichier .vimg                           */
      /**********************************************/
      pos = (long)(size_byte*(slice + frame*GetPlane(vp->shfj_db)));
      fseek(vp->fpi, pos, 0);
      /**********************************************/
      /* On lit la coupe dans le fichier .vimg      */
      /**********************************************/
      nbyte_read = PuRead((char*)s_to_rea,sizeof(short int),size_im, vp->fpi,
			  vp->ana_db);
      /**********************************************/
      /* Rescaling entre -32768 et 32767            */
      /**********************************************/
      scale = GetCalib(vp->shfj_db, slice, frame)/ vp->consign->scale;
      offset = ( GetOffset(vp->shfj_db, slice, frame) - vp->consign->offset ) 
	/ vp->consign->scale;
      SetCalib (vp->shfj_db, slice, frame, vp->consign->scale);
      SetOffset(vp->shfj_db, slice, frame, vp->consign->offset);
      for(i=size_im,s_pt1 = s_to_rea; i> 0; i--, s_pt1++)
	{
	  /**********************************************/
	  /* Verification des problemes d'arrondis      */
	  /**********************************************/
	  tmp = ((float)*s_pt1 * scale +offset);
	  if ((float)floor((double)tmp) > (float) vp->ana_db->img.glmax)
	    fprintf(stderr, "%s %s\n", vSIGN, vOUT_OF_RANGE);
	  else if ( (float) ceil((double)tmp) < (float) vp->ana_db->img.glmin)
	    fprintf(stderr, "%s %s\n", vSIGN, vOUT_OF_RANGE);
	  else *s_pt1 = (short) tmp;
	}
      break;
      /**********************************************/
      /* Buffer d'entree: entiers sur 16 bits       */
      /* Buffer de sortie: entiers sur 16 bits      */
      /**********************************************/
    case FX_SHORT_FX : s_to_rea =(short *) buff;
      /**********************************************/
      /* On se positionne au debut de la coupe dans */
      /* le fichier .vimg                           */
      /**********************************************/
      pos = (long)(size_byte*(slice + frame*GetPlane(vp->shfj_db)));
      fseek(vp->fpi, pos, 0);
      /**********************************************/
      /* On lit la coupe dans le fichier .vimg      */
      /**********************************************/
      nbyte_read = PuRead((char*)s_to_rea,sizeof(short),size_im, 
			  vp->fpi,vp->ana_db);
      break;
    case FX_INT_FX   :  
    case FX_INT_FL   :
    case FL_SHORT_FL :
    default              : 
      fprintf(stderr,"VidaRead: mode de lecture non repertorie\n");
      break;
    }
  if (nbyte_read == size_im)
    return (size_im);
  else 
    { 
      fprintf(stderr,"VidaRead: nombre de valeurs lues erronees %d %d \n",
	      size_im,nbyte_read);
    return(0);
    }
}
