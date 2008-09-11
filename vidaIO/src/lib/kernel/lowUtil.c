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


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <pwd.h>
#include <vidaIO/kernel/vidaio.h>


int XdrRead(char* ptr,FILE* stream);
int FillStructSData(S_DATA* infos,FILE* fp);


/*----------------------------------------------------------------------------
float GetCalib

   Cette fonction retourne le facteur de calibration correspondant
   a la frame et a la coupe donnee en entree

   ENTREES: numero de frame
            numero de coupe
            structure s_data

   SORTIE:  facteur de calibration
----------------------------------------------------------------------------*/

float GetCalib(S_DATA *s_data,int sl_numb,int fr_numb)
{ 
  float coeff;
  char  *addr;
 
  if (s_data->val_calib.active != VIDA_NOCALIB)
    {
      addr = (char *)s_data + s_data->val_calib.coeff_calib
	+ (fr_numb*s_data->nb_pl + sl_numb)*sizeof(float);
      memcpy(&coeff, addr, sizeof(float));
    }
  else coeff = 1.0;
  return(coeff); 
}

/*----------------------------------------------------------------------------
void SetCalib

   Cette fonction ecrit le facteur de calibration correspondant
   a la frame et a la coupe donnee en entree

   ENTREES: numero de frame
            numero de coupe
            facteur de calibration
            structure s_data
----------------------------------------------------------------------------*/

void SetCalib(S_DATA *s_data,int sl_numb,int fr_numb,float coeff)
{ 
  float tmp;
  char  *addr;

  tmp = coeff;
  addr = (char *) s_data + s_data->val_calib.coeff_calib
    + (fr_numb*s_data->nb_pl + sl_numb)*sizeof(float);
  memcpy((addr), &tmp, sizeof(float));
}

/*----------------------------------------------------------------------------
float GetOffset

   Cette fonction retourne l'offset correspondant
   a la frame et a la coupe donnee en entree

   ENTREES: numero de frame
            numero de coupe
            structure s_data

   SORTIE:  offset

----------------------------------------------------------------------------*/

float GetOffset(S_DATA *s_data,int sl_numb,int fr_numb)
{ 
  float offset;
  char  *addr;

  if (s_data->val_calib.active != VIDA_NOCALIB)
    {
      addr = (char *)s_data + s_data->val_calib.offset
	+ (fr_numb*s_data->nb_pl + sl_numb)*sizeof(float);
      memcpy(&offset, addr, sizeof(float));
    }
  else offset = 0.0;
  return(offset);
}

/*----------------------------------------------------------------------------
void SetOffset

   Cette fonction ecrit l' offset correspondant
   a la frame et a la coupe donnee en entree

   ENTREES: numero de frame
            numero de coupe
            offset
            structure s_data

----------------------------------------------------------------------------*/

void SetOffset(S_DATA *s_data,int sl_numb,int fr_numb,float offset)
{
  float tmp;
  char  *addr;
 
  tmp  = offset;
  addr = (char *) s_data + s_data->val_calib.offset
    + (fr_numb*s_data->nb_pl + sl_numb)*sizeof(float);
  memcpy(addr, &tmp, sizeof(float));
}

/*----------------------------------------------------------------------------
int GetMinPl

   Cette fonction retourne le minimum correspondant
   a la frame et a la coupe donnee en entree

   ENTREES: numero de frame
            numero de coupe
            structure s_data

   SORTIE:  minimum
----------------------------------------------------------------------------*/

int GetMinPl(S_DATA *s_data,int sl_numb,int fr_numb)
{ 
  int  min;
  char *addr;

  addr = (char *) s_data + s_data->val_ext.min_pl
    + (fr_numb*s_data->nb_pl + sl_numb)*sizeof(int);
  memcpy(&min, addr, sizeof(int));
  return(min);
}

/*----------------------------------------------------------------------------
void SetMinPl

   Cette fonction ecrit le minimum correspondant
   a la frame et a la coupe donnee en entree

   ENTREES: numero de frame
            numero de coupe
            minimum
            structure s_data
----------------------------------------------------------------------------*/

void SetMinPl(S_DATA *s_data,int sl_numb,int fr_numb,int min)
{ 
  char *addr;
 
  s_data->val_ext.active = 1;
  addr = (char *) s_data + s_data->val_ext.min_pl
    +(fr_numb*s_data->nb_pl + sl_numb)*sizeof(int);
  memcpy(addr, &min, sizeof(int)); 
}

/*----------------------------------------------------------------------------
int GetMaxPl

   Cette fonction retourne le maximum correspondant
   a la frame et a la coupe donnee en entree

   ENTREES: numero de frame
            numero de coupe
            structure s_data

   SORTIE:  maximum
----------------------------------------------------------------------------*/

int GetMaxPl(S_DATA *s_data,int sl_numb,int fr_numb)
{ 
  int  max;  
  char *addr;
 
  addr = (char *) s_data + s_data->val_ext.max_pl
    +(fr_numb*s_data->nb_pl + sl_numb)*sizeof(int);
  memcpy(&max, addr, sizeof(int));
  return(max);
}

/*----------------------------------------------------------------------------
void SetMaxPl

   Cette fonction ecrit le maximum correspondant
   a la frame et a la coupe donnee en entree

   ENTREES: numero de frame
            numero de coupe
            maximum
            structure s_data
----------------------------------------------------------------------------*/

void SetMaxPl(S_DATA *s_data,int sl_numb,int fr_numb,int max)
{
  int  tmp = max;
  char *addr;

  s_data->val_ext.active = 1;
  addr = (char *) s_data + s_data->val_ext.max_pl
    +(fr_numb*s_data->nb_pl + sl_numb)*sizeof(int);
  memcpy(addr, &tmp, sizeof(int));
}

/*----------------------------------------------------------------------------
int GetPlane

   Cette fonction retourne le nombre de plans par frame

   ENTREES: structure s_data

   SORTIE:  nombre de plans
----------------------------------------------------------------------------*/

int GetPlane(S_DATA *s_data)
{ 
  int plane;

  plane = s_data->nb_pl;
  return(plane);
}

/*----------------------------------------------------------------------------
void SetPlane

   Cette fonction ecrit le nombre de plans par frame

   ENTREES: nombre de plans par frame
            structure s_data
----------------------------------------------------------------------------*/

void SetPlane(S_DATA *s_data,int plane)
{ 
  s_data->nb_pl = plane;
}

/*----------------------------------------------------------------------------
int GetFrame

   Cette fonction retourne le nombre de frames

   ENTREES: structure s_data

   SORTIE:  nombre de frames
----------------------------------------------------------------------------*/

int GetFrame(S_DATA *s_data)
{ 
  int frame;

  frame = s_data->nb_fr;
  return(frame);
}

/*----------------------------------------------------------------------------
void SetFrame

   Cette fonction ecrit le nombre de frames

   ENTREES: nombre de frames
            structure s_data
----------------------------------------------------------------------------*/

void SetFrame(S_DATA *s_data,int frame)
{ 
  s_data->nb_fr = frame; 
}

/*----------------------------------------------------------------------------
int GetTimeStart

   Cette fonction retourne l'heure de debut d'acquisition d'une frame

   ENTREES: structure s_data
            numero de frame

   SORTIE:  heure de debut d'acquisition
----------------------------------------------------------------------------*/

int GetTimeStart(S_DATA *s_data,int fr_numb)
{ 
  int  start_time;
  char *addr;

  addr = (char *) s_data + s_data->fr_time.start_time + fr_numb*sizeof(int);
  memcpy(&start_time, addr, sizeof(int));
  return(start_time);
}

/*----------------------------------------------------------------------------
void SetTimeStart

   Cette fonction ecrit l'heure de debut d'acquisition d'une frame

   ENTREES: heure de debut d'acquisition
            numero de frame
            structure s_data
----------------------------------------------------------------------------*/

void SetTimeStart(S_DATA *s_data,int fr_numb,int start_time)
{ 
  int  tmp = start_time;
  char *addr;
 
  s_data->fr_time.active = 1;
  addr = (char *)s_data + s_data->fr_time.start_time + fr_numb*sizeof(int);
  memcpy(addr, &tmp, sizeof(int));
}

/*----------------------------------------------------------------------------
int GetTimeDura

   Cette fonction retourne la duree d'acquisition d'une frame

   ENTREES: structure s_data
            numero de frame

   SORTIE:  duree d'acquisition
----------------------------------------------------------------------------*/

int GetTimeDura(S_DATA *s_data,int fr_numb)
{ 
  int  dura_time;
  char *addr;

  addr = (char *)s_data + s_data->fr_time.duration_time + fr_numb*sizeof(int);
  memcpy(&dura_time, addr, sizeof(int));
  return(dura_time);
}

/*----------------------------------------------------------------------------
void SetTimeDura

   Cette fonction ecrit la duree d'acquisition d'une frame

   ENTREES: duree d'acquisition
            numero de frame
            structure s_data
----------------------------------------------------------------------------*/

void SetTimeDura(S_DATA *s_data,int fr_numb,int dura_time)    
{ 
  int  tmp = dura_time;
  char *addr;
 
  s_data->fr_time.active = 1;
  addr = (char *)s_data + s_data->fr_time.duration_time + fr_numb*sizeof(int);
  memcpy(addr, &tmp, sizeof(int)); 
}

/*----------------------------------------------------------------------------
float GetIsoHalf

   Cette fonction retourne la demie-vie de l'isotope

   ENTREES: structure s_data

   SORTIE:  demie-vie de l'isotope
----------------------------------------------------------------------------*/

float GetIsoHalf(S_DATA *s_data)
{ 
  float half;

  half = s_data->iso.halflife;
  return(half);
}

/*----------------------------------------------------------------------------
void SetIsoHalf

   Cette fonction ecrit la demie-vie de l'isotope

   ENTREES: demie-vie de l'isotope
            structure s_data
----------------------------------------------------------------------------*/

void SetIsoHalf(S_DATA *s_data,float half)
{ 
  s_data->iso.active = 1;
  s_data->iso.halflife = half;
}

/*----------------------------------------------------------------------------
float GetIsoDecay

   Cette fonction retourne le facteur de decroissance de l'isotope

   ENTREES: structure s_data

   SORTIE:  facteur de decroissance
----------------------------------------------------------------------------*/

float GetIsoDecay(S_DATA *s_data)
{ 
  float decay;

  decay= s_data->iso.decay;
  return(decay);
}

/*----------------------------------------------------------------------------
void SetIsoDecay

   Cette fonction ecrit le facteur de decroissance de l'isotope

   ENTREES: facteur de decroissance
            structure s_data
----------------------------------------------------------------------------*/

void SetIsoDecay(S_DATA *s_data,float decay)
{ 
  s_data->iso.active = 1;
  s_data->iso.decay = decay;
}

/*----------------------------------------------------------------------------
int SDataMalloc

   Cette fonction alloue une structure sdata et remplit la partie de taille
   fixe

   ENTREES: nombre de plans
            nombre de frames

   SORTIES: structure s_data
            longueur de la structure sdata
----------------------------------------------------------------------------*/

int SDataMalloc(int nb_pl,int nb_fr,S_DATA **data)
{ 
  int length;
  int addr;
  /**********************************************/
  /* Calcul de la longueur de la structure sdata*/
  /**********************************************/
  length =   sizeof(S_DATA) +
    2*sizeof(int)*nb_fr*nb_pl +
    2*sizeof(float)*nb_fr*nb_pl +
    2*sizeof(int)*nb_fr;
  /**********************************************/
  /* Allocation de la structure sdata           */
  /**********************************************/
  if ((*data = (S_DATA *)calloc(1,length)) == NULL)
    {
      fprintf(stderr,"SDataMalloc malloc : struct S_DATA not allocated\n");
      return(1);
    }
  /**********************************************/
  /* Initialisation a zero de toute la structure*/
  /* sdata                                      */
  /**********************************************/
  memset(*data, 0, length);
  /**********************************************/
  /* Remplissage de la partie fixe de la        */
  /* structure sdata                            */
  /**********************************************/
  (*data)->nb_pl = nb_pl;
  (*data)->nb_fr = nb_fr;
  (*data)->iso.active = 0;
  (*data)->fr_time.active = 0;
  (*data)->val_ext.active = 0;
  (*data)->val_calib.active = VIDA_NOCALIB;
  /**********************************************/
  /* Attention : ce sont des deplacements qui   */
  /* sont codes et non des adresses absolues    */
  /**********************************************/
  addr =  sizeof(S_DATA);
  (*data)->fr_time.start_time = addr;
  addr += sizeof(int)*nb_fr;
  (*data)->fr_time.duration_time =  addr;
  addr += sizeof(int)*nb_fr;
  (*data)->val_ext.min_pl = addr;
  addr += sizeof(int)*nb_fr*nb_pl;
  (*data)->val_ext.max_pl = addr;
  addr += sizeof(int)*nb_fr*nb_pl;
  (*data)->val_calib.coeff_calib = addr;
  addr += sizeof(float)*nb_fr*nb_pl;
  (*data)->val_calib.offset = addr;
  return(0);
}

/*----------------------------------------------------------------------------
int SGiveFullHandler

   Cette fonction ouvre les trois fichiers vida, remplit la structure ana_db
   alloue et remplit la structure sdata

   ENTREES: nom du fichier image vida
            structure consigne

   SORTIES: pointeurs des trois fichiers vida
            structure sdata
            structure ana_db
            status = 0 si ok
                     1 si erreur
----------------------------------------------------------------------------*/

/* modifie le 10/08/99 par C.Poupon pour reactiver le byte swapping
   La fonction machine() renvoie ABCD pour les machines du type SUN
   et DCBA pour les machines du type DEC, INTEL */

#define ABCD 1
#define DCBA 2

int machine()
{
  unsigned long magic = 0x41424344;

  char c1,c2,c3,c4;
  char* tmp;
  tmp = (char*)&magic;

  c1 = *tmp++;
  c2 = *tmp++;
  c3 = *tmp++;
  c4 = *tmp++;

  if ( c1 == 'A' && c2 == 'B' && c3 == 'C' && c4 == 'D' )
    return ABCD;
  else if ( c1 == 'D' && c2 == 'C' && c3 == 'B' && c4 == 'A' )
    return DCBA;
  else
    {
      printf( "Byte order : machine inconnue ...\n" );
      exit( 0 );
    }
}



int SGiveFullHandler(const char *filename,FILE **fp_im,FILE **fp_hd,
                     FILE **fp_info, S_DATA **infos,
                     struct AnalyzeDb *ana_db,consigne *cons)
  /*char  *nm;       Nom du fichier image vida		      */
  /*FILE  **fp_im; 	 Pointeur sur le fichier image	   */
  /*FILE  **fp_info;	          Pointeur sur le fichier info shfj	*/
  /*FILE  **fp_hd; 	          Pointeur sur le fichier header	   */
  /*S_DATA  **infos;	          Structure Information shfj		   */
  /*struct AnalyzeDb  *ana_db;	  Structure Header de l'image		   */
  /*consigne *cons;   Consigne d'ouverture			      */
{
  char *header,*info,*name;
  int  status;
  int  l;
  char	*nm;

  nm = malloc( strlen( filename ) + 6 );
  strcpy( nm, filename );
  /**********************************************/
  /* Si le suffixe .vimg n'est pas dans le nom  */
  /* du fichier, on le rajoute                  */
  /**********************************************/
  if (strstr(nm, ".vimg") == NULL)
    strcat(nm,".vimg");
  /**********************************************/
  /* Determination du nom des fichier .vinfo    */
  /* et .vhdr                                   */
  /**********************************************/
  l = strlen(nm);
  name   = (char *)calloc(1,l+1);
  header = (char *)calloc(1,l+1);
  info   = (char *)calloc(1,l+1+1);  /* ".vinfo" + long que ".vimg"*/

  if (name == NULL || header == NULL || info == NULL)
    { 
      fprintf(stderr, "SGiveFullHandler: le module d'ouverture vida \
                       ne peut allouer de memoire\n");
      if (name)   free(name);
      if (header) free(header);
      if (info)   free(info);
      free( nm );
      return(1);
    }
  strcpy(name, nm);
  strcpy(header, name);
  strcpy(header+l-4, "vhdr");
  strcpy(info, name);
  strcpy(info+l-4, "vinfo");
  /**********************************************/
  /* Ouverture du fichier .vimg                 */
  /**********************************************/
  if ((*fp_im = fopen(name, cons->mode)) == NULL)
    { 
      /*fprintf(stderr, "SGiveFullHandler: le module d'ouverture vida \
	ne peut ouvrir %s\n",name);*/
      free(name); free(header); free(info);
      free( nm );
      return(1);
    }
  /**********************************************/
  /* Ouverture du fichier .vinfo                */
  /**********************************************/
  if ((*fp_info = fopen(info, cons->mode)) == NULL)
    {
      /*fprintf(stderr, "SGiveFullHandler: le module d'ouverture vida \
	ne peut ouvrir %s\n",info);*/
      free(name); free(header); free(info);
      free( nm );
      return(1);
    }
  /**********************************************/
  /* Ouverture du fichier .vhdr                 */
  /**********************************************/
  if ((*fp_hd = fopen(header, cons->mode)) == NULL)
    { 
      /*fprintf(stderr, "SGiveFullHandler: le module d'ouverture vida \
	ne peut ouvrir %s\n",header);*/
      free(name); free(header); free(info);
      free( nm );
      return(1);
    }
  switch( cons->mode[0] )
    { 
    case 'r' : 
      if ((XdrRead((char *)ana_db,*fp_hd)) != 1 )
	{ 
	  /*fprintf(stderr,"SGiveFullHandler: %s n'est pas encode xdr\n", 
	    header);*/
	  free(name); free(header); free(info);
          free( nm );
	  return(1);
	}
      fclose(*fp_hd);
      status = SDataMalloc(ana_db->img.dim[3], ana_db->img.dim[4], infos);
      if( status != 0 ) 
	{ 
	  free(*infos);
	  free(name); free(header); free(info);
          free( nm );
	  return(1);
	}
      status = FillStructSData(*infos,*fp_info);
      if( status != 0 ) 
	{ 
	  free(*infos);
	  free(name); free(header); free(info);
          free( nm );
	  return(1);
	}
      if ((ana_db->img.dim[3] != (*infos)->nb_pl) || 
	  (ana_db->img.dim[4] != (*infos)->nb_fr))
	{ 
	  /*fprintf(stderr,
	    "SGiveFullHandler: Incoherence entre vinfo et vhdr\n");*/
	  free(*infos);
	  free(name); free(header); free(info);
          free( nm );
	  return(1);
	}
      fclose(*fp_info);
      /* modifie par C.Poupon le 10/08/99 pour activer le byte swapping */
      if ( machine() != (int)ana_db->img.pixdim[15] && 
	   ( (int)ana_db->img.pixdim[15] == ABCD || 
	     (int)ana_db->img.pixdim[15] == DCBA ) )
	{
	  //printf("byte swapping actif\n");
	  ana_db->key.hkey_un0 = 1;
	}
      else if ( machine() == DCBA &&
		!( (int)ana_db->img.pixdim[15] == ABCD || 
		   (int)ana_db->img.pixdim[15] == DCBA ) )
	{
	  //printf("byte swapping actif par defaut\n");
	  ana_db->key.hkey_un0 = 1;
	}
      else
	{
	  //printf("byte swapping inactif\n");
	  ana_db->key.hkey_un0 = 0;
	}
      break;
    case 'w' : sprintf(ana_db->key.name, " ");
      ana_db->key.hkey_un0      = 0;
      ana_db->key.size          = sizeof(AnaHdr);
      ana_db->key.extents       = 65384;
      ana_db->key.regular       = 'r';
      ana_db->img.dim[0]        = 4;
      ana_db->img.dim[1]        = cons->size_x;
      ana_db->img.dim[2]        = cons->size_y;
      ana_db->img.dim[3]        = cons->size_z;
      ana_db->img.dim[4]        = cons->size_t;
      ana_db->img.pixdim[1]     = cons->psize_x;
      ana_db->img.pixdim[2]     = cons->psize_y;
      ana_db->img.pixdim[3]     = cons->psize_z;
      ana_db->img.pixdim[4]     = cons->psize_t;
      /* stockage de l'info sur le type de machine qui a genere le fichier */
      /* modifie par C.Poupon -> byte order */
      ana_db->img.pixdim[15] = (float)machine();

      switch(cons->type)
	{
	case FX_SHORT_FX :
	case FX_SHORT_FL :
	case FL_SHORT_FL :
	case FX_INT_FX   :
	case FX_INT_FL   :
	case FL_FLOAT_FL : ana_db->img.bitpix  = 16;break;
	default          : ana_db->img.bitpix  = 8; break;
	}
      memset(&(ana_db->his), 0, sizeof(ana_db->his));
      if (cons->size_t <=  1) cons->size_t = 1;
      status = SDataMalloc(cons->size_z, cons->size_t, infos);
      if( status != 0 )
	{ 
	  free(name); free(header); free(info);
          free( nm );
	  return(1);
	}
      (*infos)->val_calib.active   = cons->unite;
      break;
    default  : free(name); free(header); free(info);
      //fprintf(stderr, "SGiveFullHandler: mode d'ouverture inconnu\n");
      free( nm );
      return(1);
      break;
    }
  free(name); free(header); free(info);
  free( nm );
  return(0);
}
