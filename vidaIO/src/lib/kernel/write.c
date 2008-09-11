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
#include <limits.h>
#include <float.h>
#include <vidaIO/kernel/vidaio.h>

int GetPlane(S_DATA* s_data);
int PuWrite(char* buff,int size,int nitems,FILE* stream);

#define vSIGN "libiovda : VidaWrite :"

/*----------------------------------------------------------------------------
int VidaWrite(char *buff, int slice, int frame, VIDAim *vp)
   
   Cette fonction fait l'ecriture commandee pour un VIDAim pointeur.
   Cette fonction mime l'execution d'un write et en mime les entrees sorties.
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
             0 : si probleme ou EOF. Cf man de fwrite
             size_byte : si ecriture de size byte.
                  Ce nombre est issu de l'interpretation des donnees dans vp.
                  Ce doit etre exactement le nombre de byte rendu par le 
                  pu_write de bas niveau qui est appele tout en bas 


----------------------------------------------------------------------------*/

int VidaWrite(char *buff,int slice,int frame,VIDAim *vp)
{
  short *s_pt1;
  int   *i_ima, *i_pt1, i_min, i_max;
  float *f_pt1, f_min, f_max;
  int   size_im, size_byte, i, nbyte_written=0;
  long  pos;
  float scale, offset, tmp;
  short *s_to_wri;
  short *pts;

  /**********************************************/
  /* Taille du buffer a allouer                 */
  /**********************************************/
  size_im   = vp->ana_db->img.dim[1] * vp->ana_db->img.dim[2];
  size_byte = size_im * vp->ana_db->img.bitpix / 8;
  switch(vp->consign->type)
    { 
      /**********************************************/
      /* Buffer d'entree: entiers sur 32 bits       */
      /* Buffer de sortie: entiers sur 16 bits      */
      /* determines etalement de l'histogramme entre*/
      /* -32768 et 32767                            */
      /**********************************************/
    case FX_INT_FL :
      /**********************************************/
      /* Allocation d'un buffer intermediaire       */
      /**********************************************/
      s_to_wri = (short *) calloc(1,size_byte);
      if( s_to_wri == NULL )
	{ 
	  fprintf(stderr,
		  "VidaWrite: Erreur d'allocation du buffer intermediaire\n");
	  return(0);
	}
      i_ima = (int *) buff;
      /**********************************************/
      /* Determination des extrema                  */
      /**********************************************/
      i_max = 0x80000000; i_min = 0x7fffffff;
      for (i=size_im,i_pt1=i_ima;i>0;i--,i_pt1++)
	{ 
	  if (*i_pt1 > i_max) i_max = *i_pt1;
	  if (*i_pt1 < i_min) i_min = *i_pt1;
	}	
      /**********************************************/
      /* Determination des offset et facteurs de    */
      /* calibration pour rescaling entre -32768 et */
      /* 32767                                      */
      /**********************************************/
      scale  = ((float)i_max - (float)i_min)/ 65535.0;
      offset = ((float)i_max + (float)i_min + scale)/ 2.0;
      i_max  = -32768; i_min = 32767;
      for(i=size_im, pts = s_to_wri, i_pt1 = i_ima;i > 0; i--, pts++, i_pt1++)
	{ 
	  tmp = (float)(*i_pt1);
	  tmp = (tmp - offset)/scale;
	  *pts = (short) tmp;
	  if (*pts > i_max) i_max = *pts;
	  if (*pts < i_min) i_min = *pts;
	}
      SetCalib (vp->shfj_db, slice, frame, scale);
      SetOffset(vp->shfj_db, slice, frame, offset);
      SetMaxPl (vp->shfj_db, slice, frame, i_max);
      SetMinPl (vp->shfj_db, slice, frame, i_min);
      /**********************************************/
      /* On se positionne au debut de la coupe dans */
      /* le fichier .vimg                           */
      /**********************************************/
      pos = (long)(size_byte*(slice + frame*GetPlane(vp->shfj_db)));
      fseek(vp->fpi,pos, 0);
      /**********************************************/
      /* On ecrit la coupe dans le fichier .vimg    */
      /**********************************************/
      nbyte_written = PuWrite((char *)s_to_wri,sizeof(short),size_im,vp->fpi);
      /**********************************************/
      /* Desallocation du buffer intermediaire      */
      /**********************************************/
      free(s_to_wri);
      break;
      /**********************************************/
      /* Buffer d'entree: entiers sur 32 bits       */
      /* Buffer de sortie: entiers sur 16 bits      */
      /* determines par decalage bit a bit vers la  */
      /* droite                                     */
      /**********************************************/
    case FX_INT_FX :
      /**********************************************/
      /* Allocation d'un buffer intermediaire       */
      /**********************************************/
      s_to_wri = (short *) calloc(1,size_byte);
      if( s_to_wri == NULL )
	{
	  fprintf(stderr,
		  "VidaWrite: Erreur d'allocation du buffer intermediaire\n");
	  return(0);
	}
      i_ima = (int *) buff;
      /**********************************************/
      /* Decalage bit a bit des entiers vers la     */
      /* droite                                     */
      /**********************************************/
      i_max = -32768; i_min = 32767;
      for(i=size_im,pts=s_to_wri,i_pt1=i_ima;i>0;i--,pts++,i_pt1++)
	{ 
	  *pts = ((*i_pt1) >> (vp->consign->scale_shift));
	  if (*pts > i_max) i_max = *pts;
	  if (*pts < i_min) i_min = *pts;
	}
      /**********************************************/
      /* Determination des offset et facteurs de    */
      /* calibration par decalage bit a bit vers la */
      /* droite                                     */
      /**********************************************/
      scale = 1 << (vp->consign->scale_shift);
      offset = 0.0;
      SetCalib (vp->shfj_db, slice, frame, scale);
      SetOffset(vp->shfj_db, slice, frame, offset);
      SetMaxPl (vp->shfj_db, slice, frame, i_max);
      SetMinPl (vp->shfj_db, slice, frame, i_min);
      /**********************************************/
      /* On se positionne au debut de la coupe dans */
      /* le fichier .vimg                           */
      /**********************************************/
      pos = (long)(size_byte*(slice + frame*GetPlane(vp->shfj_db)));
      fseek(vp->fpi,pos, 0);
      /**********************************************/
      /* On ecrit la coupe dans le fichier .vimg    */
      /**********************************************/
      nbyte_written = PuWrite((char *)s_to_wri,sizeof(short),size_im,vp->fpi);
      /**********************************************/
      /* Desallocation du buffer intermediaire      */
      /**********************************************/
      free(s_to_wri);
      break;
      /**********************************************/
      /* Buffer d'entree: entiers sur 16 bits       */
      /* Buffer de sortie: entiers sur 16 bits      */
      /* Les facteurs de calibration sont mis a 1   */
      /* et les offsets a 0                         */
      /**********************************************/
    case FX_SHORT_FX :
      /**********************************************/
      /* Le pointeur de sortie est identique au     */
      /* pointeur en entree                         */
      /**********************************************/
      s_to_wri = (short *) buff;
      /**********************************************/
      /* Determination des extrema                  */
      /**********************************************/
      i_max = -32768; i_min = 32767;
      for(i=size_im, pts = s_to_wri;i > 0; i--, pts++, i_pt1++)
	{ 
	  if (*pts > i_max) i_max = *pts;
	  if (*pts < i_min) i_min = *pts;
	}
      SetCalib (vp->shfj_db, slice, frame, 1.0);
      SetOffset(vp->shfj_db, slice, frame, 0.0);
      SetMaxPl (vp->shfj_db, slice, frame, i_max);
      SetMinPl (vp->shfj_db, slice, frame, i_min);
      /**********************************************/
      /* On se positionne au debut de la coupe dans */
      /* le fichier .vimg                           */
      /**********************************************/
      pos = (long)(size_byte*(slice + frame*GetPlane(vp->shfj_db)));
      fseek(vp->fpi,pos, 0);
      /**********************************************/
      /* On ecrit la coupe dans le fichier .vimg    */
      /**********************************************/
      nbyte_written = PuWrite((char *)s_to_wri,sizeof(short),size_im,vp->fpi);
      break;
      /**********************************************/
      /* Buffer d'entree: flottants                 */
      /* Buffer de sortie: entiers sur 16 bits      */
      /**********************************************/
    case FL_FLOAT_FL :
      /**********************************************/
      /* Allocation d'un buffer intermediaire       */
      /**********************************************/
      s_to_wri = (short *) calloc(1,size_byte);
      if( s_to_wri == NULL )
	{ 
	  fprintf(stderr,
		  "VidaWrite: Erreur d'allocation du buffer intermediaire\n");
	  return(0);
	}
      /**********************************************/
      /* Rescaling entre -32768 et 32767            */
      /**********************************************/
      /*
	f_max = -2147483648.0; f_min = 2147483647.0;
      */
      f_max = FLT_MIN; f_min = FLT_MAX;

      for(i=size_im, f_pt1 = (float *)buff; i > 0; i--, f_pt1++)
	{ 
	  if (*f_pt1 > f_max) f_max = *f_pt1;
	  if (*f_pt1 < f_min) f_min = *f_pt1;
	}
      /*
	scale  = (f_max - f_min) / 65535.0;
	offset = (f_max + f_min + scale) / 2.0;
	le 27/06/2000
	On force desormais le offset a 0*/
      offset = 0.0;
      if (f_min < 0.0) 
	{
	  if (f_max >  - f_min) 
	    scale =  f_max / 32767;
	  else
	    scale =  f_min / (-32768);
	}
      else
	{
	  scale =  f_max / 32767;
	}

      if (scale==0.0) 
	{ 
	  free(s_to_wri);
	  fprintf(stderr,
		  "VidaWrite: Tous les pixels de la coupe sont identiques\n");
	  fflush(stderr);
	  return(0);
	}
      for(i=size_im, f_pt1 = (float *)buff, s_pt1 = s_to_wri; i > 0; 
	  i--, f_pt1++, s_pt1++)
	{ 
	  tmp  = (*f_pt1 - offset) /scale;
	  if ((float)floor((double)tmp) > 32767.0)
	    {
	      fprintf(stderr, "%s %s\n", vSIGN, vOUT_OF_RANGE);
	      *s_pt1 = 32767;
	    }
	  else if ( (float)ceil((double)tmp) < -32768.0)
	    { 
	      fprintf(stderr, "%s %s\n", vSIGN, vOUT_OF_RANGE);
	      *s_pt1 = -32768;
	    }
	  *s_pt1 = (short) tmp;
	}
      i_max = -32768; i_min = 32767;
      for(i=size_im, pts = s_to_wri;i > 0; i--, pts++)
	{ 
	  if (*pts > i_max) i_max = *pts;
	  if (*pts < i_min) i_min = *pts;
	} 
      SetCalib (vp->shfj_db, slice, frame, scale);
      SetOffset(vp->shfj_db, slice, frame, offset);
      SetMaxPl (vp->shfj_db, slice, frame, i_max);
      SetMinPl (vp->shfj_db, slice, frame, i_min);
      /**********************************************/
      /* On se positionne au debut de la coupe dans */
      /* le fichier .vimg                           */
      /**********************************************/
      pos = (long)(size_byte*(slice + frame*GetPlane(vp->shfj_db)));
      fseek(vp->fpi,pos, 0);
      /**********************************************/
      /* On ecrit la coupe dans le fichier .vimg    */
      /**********************************************/
      nbyte_written = PuWrite((char *)s_to_wri,sizeof(short),size_im,vp->fpi);
      /**********************************************/
      /* Desallocation du buffer intermediaire      */
      /**********************************************/
      free(s_to_wri);
      break;                              
    case FX_SHORT_FL :
    case FL_SHORT_FL :
    default          : 
      fprintf(stderr,"VidaWrite: mode d'ecriture non repertorie\n");
      break;
    }
  /**********************************************/
  /* Everything OK                              */
  /**********************************************/
  if (nbyte_written == size_im) return (size_im);
  /**********************************************/
  /* Incohernce entre les tailles demandees en  */
  /* ecriture et celle reellement ecrite        */
  /**********************************************/
  else 
    {
      fprintf(stderr,"VidaWrite: nombre de valeurs ecrites erronees\n");
      return(0);
    }
}
