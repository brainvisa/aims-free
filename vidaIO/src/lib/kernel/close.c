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
#include <vidaIO/kernel/vidaio.h>

void SWAllVinfo(S_DATA* infos,FILE* fp);
int  XdrWrite(char* ptr,FILE* stream);

/*---------------------------------------------------------------------------
int VidaClose(VIDAim *vp) : Fonction publique de vidaClose.c

   Cette fonction est la fonction publique qui fait l'ouverture d'une image 
   Vida. Suivant les types de donnees elle fait un travail de sur les 
   structures descriptives de shfj_db et ana_db en particulier sur les min/max.
   Elle ferme les pointeur de fichiers FILE* et fait la liberation des buffers
   de memoire de la structure pointee par vp.
   
   ENTREES:
   VIDAim *vp: le pointeur d'image VIDA.

   SORTIES:
   La fonction rend le status d'erreur de cette fonction. On suit les 
   conventions de la fonction fclose() classique du systeme Unix.
       EOF : Erreur qui doit etre interpree
       0   : Si pas d'erreur
    
---------------------------------------------------------------------------*/

int VidaClose(VIDAim *vp)
{ 
  int              min,max,i,j,tmp;

  if (vp==NULL) return(EOF);

  switch(vp->consign->mode[0])      
    { 
    case 'w': 
      {
	min = 0x7fffffff; 
	max = 0x80000000;
	for(i=0;i<vp->shfj_db->nb_fr;i++)
	  for(j=0;j<vp->shfj_db->nb_pl;j++)
	    { 
	      if ((tmp = GetMinPl(vp->shfj_db,j,i)) < min)
		min = tmp;
	      if ((tmp = GetMaxPl(vp->shfj_db,j,i)) > max)
		max = tmp;
	    }
	vp->ana_db->img.glmin = min;
	vp->ana_db->img.glmax = max;
	XdrWrite((char*)vp->ana_db,vp->fph);
	SWAllVinfo(vp->shfj_db,vp->fpf);
	break;
      }
    case 'r': break;
    }

  if (vp->fpi)      fclose(vp->fpi);
  if (vp->fph)      fclose(vp->fph);
  if (vp->fpf)      fclose(vp->fpf);

  if (vp->ana_db)   free(vp->ana_db);
  if (vp->shfj_db)  free(vp->shfj_db);
  if (vp->consign)  free(vp->consign);

  if (vp) free(vp);

  return(0);
}




