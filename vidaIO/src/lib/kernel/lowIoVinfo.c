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

#include <vidaIO/kernel/vidaio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

void SetIsoHalf(S_DATA* s_data,float half);
void SetIsoDecay(S_DATA* s_data,float decay);

/*----------------------------------------------------------------------------
int FillStructSData(infos, fp)

   Cette fonction lit le fichier .vinfo etiquette par etiquette
   et remplit la structure shfj_db

   ENTREES: fp : pointeur sur le fichier .vinfo

   SORTIES: infos : structure shfj_db

----------------------------------------------------------------------------*/

int FillStructSData(S_DATA *infos,FILE *fp)
{ 
  char  *buff;
  char  *device;
  char  *ptbuf;
  int   pl_numb,fr_numb;
  float half,decay,calib,offset;
  int   start,dura;
  int   min,max;
  int   nb,fr,pl,i,j;
  int   key_word_find;
  struct stat	statbuf;
  off_t	buflen;

  if( fstat( fileno( fp ), &statbuf ) != 0 )
    {
      fprintf(stderr,"FillStructSData : could not fstat() .vinfo file\n");
      return( 1 );
    }
  buflen = statbuf.st_size;
  buff = (char *)calloc(1,buflen);
  device = (char *)calloc(1,buflen);
  infos->iso.active       = 0;
  infos->val_ext.active   = 0;
  infos->fr_time.active   = 0;
  infos->val_calib.active = VIDA_NOCALIB;

  fgets(buff,buflen,fp);	/* le champs ## VidaInfo		 */

  if (strncmp(buff,"##VidaInfo",10) != 0)
    {
      fprintf(stderr,"FillStructSData : fichier .vinfo invalide\n");
      return(1);
    }
  fgets(buff,buflen,fp);
  while (strncmp(buff, "##EndVidaInfo",13) !=0)
    { 
      key_word_find = 0;
      if (strncmp(buff, "#Dev", 4) == 0)
	{ key_word_find = 1;
	fgets(buff,buflen,fp);
	sscanf(buff,"%s\n",device);
	sprintf(infos->acq_device,"%s",device);
	fgets(buff, buflen, fp);
	}
      if (strncmp(buff, "#Fra", 4) == 0)
	{
	  key_word_find = 1;
	  fgets(buff,buflen,fp);
	  sscanf(buff,"%d	%d\n",&fr_numb,&pl_numb);
	  infos->nb_pl = pl_numb;
	  infos->nb_fr = fr_numb;
	  fgets(buff, buflen, fp);
	}
      if (strncmp(buff, "#Half",5) == 0)
	{ 
	  key_word_find = 1;
	  fgets(buff,buflen,fp);
	  sscanf(buff, "%f %f\n",&half, &decay);
	  SetIsoHalf(infos,half);
	  SetIsoDecay(infos,decay);
	  fgets(buff, buflen, fp);
	}
      if (strncmp(buff, "#Start",6) == 0)
	{ 
	  key_word_find = 1;
	  fgets(buff, buflen, fp);
	  for(nb=0;nb<fr_numb;nb++)
	    { 
	      sscanf(buff,"%d %d\n",&start,&dura);
	      SetTimeStart(infos,nb,start);
	      SetTimeDura (infos,nb,dura);
	      fgets(buff,buflen,fp);
	    }
	}  
      if (strncmp(buff,"#Min",4) == 0)
	{ 
	  key_word_find = 1;
	  for(i=0;i<infos->nb_fr;i++)
	    {
	      fgets(buff, buflen, fp);
	      fgets(buff, buflen, fp);
	      ptbuf = buff;
	      for(j=0;j<infos->nb_pl;j++)
		{ 
		  sscanf(ptbuf,"%d(%d*%d)",&pl,&(min) ,&(max));
		  SetMaxPl(infos,j,i,max);
		  SetMinPl(infos,j,i,min);
		  for(;*ptbuf != ' '; ptbuf++);
		  ptbuf += 3;
		}
	    }
	  fgets(buff, buflen, fp);
	}
      if (strncmp(buff, "#Cali", 5) == 0)
	/* assure la compatibilite avec la version 3.0 en ecriture */
	{ 
	  if (strncmp(buff+14,"nCi/ml",6) == 0)    
	    infos->val_calib.active = VIDA_nCi;
	  else if (strncmp(buff+14,"Bq/cc",5) == 0) 
	    infos->val_calib.active = VIDA_Bq;
	  else if (strncmp(buff+14,"inconnue",8) == 0) 
	    infos->val_calib.active = VIDA_SANSUNITE;
	  else if (strncmp(buff+14,"pas",3) == 0)
	    infos->val_calib.active = VIDA_NOCALIB;

	  /* assure la compatibilite avec la version 1.0 en lecture */
	  else if (strncmp(buff+14,"sans",4) == 0)
	    infos->val_calib.active = VIDA_SANSUNITE;

	  else              
	    infos->val_calib.active = VIDA_NOCALIB;

	  key_word_find = 1;
	  for (i=0;i<infos->nb_fr;i++)
	    { 
	      fgets(buff,buflen,fp);
	      sscanf(buff, "#Fr.%d", &fr);
	      fgets(buff, buflen, fp);
	      ptbuf = buff;
	      for(j=0;j<infos->nb_pl;j++)
		{ 
		  sscanf(ptbuf,"%d(%e*%e)",&pl,&(calib),&(offset));
		  SetCalib (infos,j,i,calib);
		  SetOffset(infos,j,i,offset);
		  for (;*ptbuf != ' '; ptbuf++); 
		  ptbuf +=3;
		}
	    }
	  fgets(buff,buflen,fp);
	}
      if (!key_word_find) fgets(buff, buflen, fp);
    }
  if (!infos->fr_time.active) 
    for(nb=0;nb<fr_numb;nb++)
      {
	SetTimeDura (infos,nb,1);	
	SetTimeStart(infos,nb,nb);	
      }
  return(0);
}

/*----------------------------------------------------------------------------
void SWFooterVinfo(fp)

   Cette fonction ecrit l'etiquette de fin de fichier .vinfo

   ENTREES: fp : pointeur sur le fichier .vinfo

----------------------------------------------------------------------------*/

void SWFooterVinfo(FILE *fp)
{ 
  fprintf(fp,"##EndVidaInfo\n");
}

/*----------------------------------------------------------------------------
void SWHeaderVinfo(infos, fp)

   Cette fonction ecrit l'etiquette de debut de fichier .vinfo
   ainsi que le nombre de frames et de coupes par frame

   ENTREES: fp : pointeur sur le fichier .vinfo
            infos : structure shfj_db

----------------------------------------------------------------------------*/

void SWHeaderVinfo(S_DATA *infos,FILE *fp)
{ 
  fprintf(fp,"##VidaInfo\n");
  if (infos->nb_fr != -1 || infos->nb_pl != -1) 
    {
      fprintf(fp, "#Frames Planes\n");
      fprintf(fp, "%d %d\n", infos->nb_fr, infos->nb_pl);
      fprintf(fp, "#Device\n");
      fprintf(fp, "%s\n", infos->acq_device);
    }
}

/*----------------------------------------------------------------------------
void SWMinmaxVinfo(infos, fp)

   Cette fonction ecrit les extrema par coupe dans le fichier .vinfo

   ENTREES: fp : pointeur sur le fichier .vinfo
            infos : structure shfj_db

----------------------------------------------------------------------------*/

void SWMinMaxVinfo(S_DATA *infos,FILE *fp)
{
  int i,j;

  if (infos->val_ext.active  != 0) 
    {
      fprintf(fp, "#MinMax\n");
      for (i=0;i<infos->nb_fr;i++)
	{
	  fprintf(fp,"#Fr.%d\n",i+1);
	  for(j=0;j<infos->nb_pl;j++)
	    {
	      fprintf(fp,"%d(%d*%d)   ",j+1,GetMinPl(infos,j,i),
		      GetMaxPl(infos,j,i));
	    }
	  fprintf(fp, "\n");
	}
    }
}

/*----------------------------------------------------------------------------
void SWQsoVinfo(infos, fp)

   Cette fonction ecrit les facteurs de calibration et les offset par coupe

   ENTREES: fp : pointeur sur le fichier .vinfo
            infos : structure shfj_db

----------------------------------------------------------------------------*/

void SWQsoVinfo(S_DATA *infos,FILE *fp)
{
  int fr,pl;

  switch(infos->val_calib.active)
    {
    case VIDA_SANSUNITE : 
      fprintf(fp, "#Calibration (inconnue)\n");
      break;
    case VIDA_nCi       : 
      fprintf(fp, "#Calibration (nCi/ml)\n");    
      break;
    case VIDA_Bq        : 
      fprintf(fp, "#Calibration (Bq/cc)\n");    
      break;
    case VIDA_NOCALIB   :
      fprintf(fp, "#Calibration (pas)\n");          
      break;
    default                 : 
      fprintf(stderr,"Calibration not active\n");
      return;
    }
  for (fr=0;fr<infos->nb_fr;fr++)
    { 
      fprintf(fp, "#Fr.%d\n", fr+1);
      for (pl=0;pl<infos->nb_pl;pl++)
	{
	  fprintf(fp, "%d(%1.5e*%1.5e)   ",pl+1,GetCalib(infos,pl,fr),
		  GetOffset(infos,pl,fr));
	}
      fprintf(fp, "\n");
    }
}

/*----------------------------------------------------------------------------
void SWTimeVinfo(infos, fp)

   Cette fonction ecrit les debuts et durees d'acquisition par frame

   ENTREES: fp : pointeur sur le fichier .vinfo
            infos : structure shfj_db

----------------------------------------------------------------------------*/

void SWTimeVinfo(S_DATA *infos,FILE *fp)
{
  int fr;
  if (infos->fr_time.active != 0) 
    {
      fprintf(fp, "#Start time(ms)       Duration Time(ms)\n");
      for (fr=0;fr<infos->nb_fr;fr++)
	{
	  fprintf(fp,"%d    %d\n",GetTimeStart(infos,fr),
		  GetTimeDura(infos,fr));
	}
    }
}

/*----------------------------------------------------------------------------
void SWDecayVinfo(infos, fp)

   Cette fonction ecrit la demie-vie et le facteur de decroissance 
   de l'isotope

   ENTREES: fp : pointeur sur le fichier .vinfo
            infos : structure shfj_db

----------------------------------------------------------------------------*/

void SWDecayVinfo(S_DATA *infos,FILE *fp)
{ 
  if (infos->iso.active != 0) 
    {
      fprintf(fp,"#Halflife(s)  Decay\n");
      fprintf(fp,"%f    %f\n",infos->iso.halflife,infos->iso.decay);
    }
}

/*----------------------------------------------------------------------------
void SWAllVinfo(infos, fp)

   Cette fonction genere le fichier .vinfo a partir de la structure shfj_db

   ENTREES: infos : structure shfj_db
            fp : pointeur sur le fichier .vinfo

----------------------------------------------------------------------------*/

void SWAllVinfo(S_DATA *infos,FILE *fp)
{
  SWHeaderVinfo(infos,fp);
  SWTimeVinfo  (infos,fp);
  SWDecayVinfo (infos,fp);
  SWQsoVinfo   (infos,fp);
  SWMinMaxVinfo(infos,fp);
  SWFooterVinfo(fp);
}
