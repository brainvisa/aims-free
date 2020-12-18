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
#include <ecat/kernel/matrix.h>
#include <ecat/kernel/machine_indep.h>
#include <ecat+/io/io.h>
/* #include <ecat+/io/general.h> */
#include <math.h>
#include <ecat+/io/rwToolBox.h>

#define my_abs( v ) ( (v <0 ? -v : v) )



#ifdef AJETER

int Read_Scan3D_Ecat7( MatrixFile *mptr, Scan3D_subheader *sh, short *outdata,
			   int matnum, int segment)
{
  int            seg,segment_occurance,offset,file_pos;
  unsigned int   bin, data_size;
  float          scalefac=1.0;
  struct MatDir  matdir;
  short          sdata;
  float          fdata;
  char           *cdata;
  int            type_size,csize;

  /*{{{  type size*/
  if (mptr->mhptr->file_type == Short3dSinogram) {
    type_size = sizeof(short);
  } else if (mptr->mhptr->file_type == Float3dSinogram) {
    type_size = sizeof(float);
  } else {
    printf(" Read_Scan3D_Ecat7> !!! Error: '%s' is not a 3dSinogram matrix file (%0d)\n",
           mptr->fname,mptr->mhptr->file_type);
    return -1;
  }
  /*}}}  */
  /*{{{  read subheader*/
  if (matrix_find(mptr,matnum,&matdir)) {
    printf(" Read_Scan3D_Ecat7> !!! Error: can not find matrix %0d in %s\n",matnum,mptr->fname);
    return -1;
  } else if (mat_read_Scan3D_subheader(mptr->fptr,mptr->mhptr, matdir.strtblk,sh)) {
    printf(" Read_Scan3D_Ecat7> !!! Error: can not read scan3D subheader %0d in '%s'\n",
           matnum,mptr->fname);
    return -1;
  } else if ((mptr->mhptr->file_type == Short3dSinogram && sh->data_type != SunShort) ||
             (mptr->mhptr->file_type == Float3dSinogram && sh->data_type != IeeeFloat)) {
    printf(" Read_Scan3D_Ecat7> !!! Error: data type of matrix %0d is %0d\n",matnum,sh->data_type);
    return -1;
  }
  /*}}}  */
  /*{{{  data size and segment offset*/
  seg = 0;
  data_size = 0;
  offset = 0;
  if (segment != 0) {
    segment_occurance = -1;
    while ((segment < -seg)||(segment > +seg)) {
      offset += sh->num_r_elements*sh->num_angles*sh->num_z_elements[seg]*type_size;
      seg++;
      segment_occurance +=2;
    }
    if (segment < 0) {
      segment_occurance++;
      offset += sh->num_r_elements*sh->num_angles*sh->num_z_elements[seg]*type_size/2;
    }
    data_size = sh->num_r_elements*sh->num_angles*sh->num_z_elements[seg]/2;
  } else {
    segment_occurance = 0;
    data_size = sh->num_r_elements*sh->num_angles*sh->num_z_elements[seg];
  }
  if (data_size == 0) {
    printf(" Read_Scan3D_Ecat7> !!! Error: unexpected segment requested (%0d)\n",segment);
    return -1;
  }
  /*}}}  */
  if ((cdata = (char*) calloc(type_size,data_size)) == NULL) {
    printf(" Read_Scan3D_Ecat7> !!! Error: can not allocate %0d bytes\n",type_size*data_size);
    return -1;
  }
  scalefac = 1.0;
  if (sh->scale_factor != 0.0) scalefac *= sh->scale_factor;
  /*{{{  read segment*/
  file_pos = (matdir.strtblk+1)*MatBLKSIZE+offset;
  if (fseek(mptr->fptr,file_pos,0) == EOF) {
    printf(" Read_Scan3D_Ecat7> !!! Error: can not fseek position %0d for %s\n",
           file_pos,mptr->fname);
    return -1;
  } else if (fread(cdata,type_size,data_size,mptr->fptr) != data_size) {
    printf(" Read_Scan3D_Ecat7> !!! Error: can not read matrix %0d in %s\n",matnum,mptr->fname);
    return -1;
  }
  /*}}}  */
  csize = 0;
  if (mptr->mhptr->file_type == Short3dSinogram) {
    for (bin=0; bin<data_size; bin++) {
      bufRead_s(&sdata,cdata,&csize);
      outdata[bin] = (float)sdata*scalefac; 
    }
  } else if (mptr->mhptr->file_type == Float3dSinogram) {
    for (bin=0; bin<data_size; bin++) {
      bufRead_f(&fdata,cdata,&csize);
      outdata[bin] = fdata*scalefac;
    }
  } else {
    printf(" Read_Scan3D_Ecat7> !!! Error: can not read Float3dSinogram");
    return( -1);
  }

  /* Il faut mieux g�rer cela */
/*   if (mptr->mhptr->file_type == Short3dSinogram) */
/*     { */
/*       for (bin=0; bin<data_size; bin++) */
/* 	{ */
/* 	  bufRead_s(&sdata,cdata,&csize); */
/* 	  outdata[bin] = sdata; */
/* 	}  */
/*     } */
/*   else */
/*     { */
/*     printf(" Read_Scan3D_Ecat7> !!! Error: can not read Float3dSinogram"); */
/*     return( -1); */
/*     } */
  free(cdata);
  return data_size;
}

int Read_Scan3D_Ecat7_FLOAT(MatrixFile *mptr, Scan3D_subheader *sh, float *outdata,
			    int matnum, int segment)
{
  int            seg,segment_occurance,offset,file_pos;
  unsigned int   bin, data_size;
  float          scalefac=1.0;
  struct MatDir  matdir;
  short          sdata;
  float          fdata;
  char           *cdata;
  int            type_size,csize;
  float maxOnData = 0. ;
  float maxOnScaledData = 0. ;
  int i = 0 ;
  
  printf("RS3DE7 1\n") ;
  
  /*{{{  type size*/
  type_size = sizeof(float);
  /*}}}  */
  /*{{{  read subheader*/
  if (matrix_find(mptr,matnum,&matdir)) {
    printf(" Read_Scan3D_Ecat7> !!! Error: can not find matrix %0d in %s\n",matnum,mptr->fname);
    return -1;
  } else if (mat_read_Scan3D_subheader(mptr->fptr,mptr->mhptr, matdir.strtblk,sh)) {
    printf(" Read_Scan3D_Ecat7> !!! Error: can not read scan3D subheader %0d in '%s'\n",
           matnum,mptr->fname);
    return -1;
  } else if ((mptr->mhptr->file_type == Short3dSinogram && sh->data_type != SunShort) ||
             (mptr->mhptr->file_type == Float3dSinogram && sh->data_type != IeeeFloat)) {
    printf(" Read_Scan3D_Ecat7> !!! Error: data type of matrix %0d is %0d\n",matnum,sh->data_type);
    return -1;
  }
  /*}}}  */
  /*{{{  data size and segment offset*/
  printf("RS3DE7 2\n") ;
  seg = 0;
  data_size = 0;
  offset = 0;
  if (segment != 0) {
    segment_occurance = -1;
    while ((segment < -seg)||(segment > +seg)) {
      offset += sh->num_r_elements*sh->num_angles*sh->num_z_elements[seg]*type_size;
      seg++;
      segment_occurance +=2;
    }
    if (segment < 0) {
      segment_occurance++;
      offset += sh->num_r_elements*sh->num_angles*sh->num_z_elements[seg]*type_size/2;
    }
    data_size = sh->num_r_elements*sh->num_angles*sh->num_z_elements[seg]/2;
  } else {
    segment_occurance = 0;
    data_size = sh->num_r_elements*sh->num_angles*sh->num_z_elements[seg];
  }
  if (data_size == 0) {
    printf(" Read_Scan3D_Ecat7> !!! Error: unexpected segment requested (%0d)\n",segment);
    return -1;
  }

  printf("RS3DE7 3\n") ;

  /*}}}  */
  if ((cdata = (char*) calloc(type_size,data_size)) == NULL) {
    printf(" Read_Scan3D_Ecat7> !!! Error: can not allocate %0d bytes\n",type_size*data_size);
    return -1;
  }
  scalefac = 1.0;
  if (sh->scale_factor != 0.0) scalefac *= sh->scale_factor;
  /*{{{  read segment*/
  file_pos = (matdir.strtblk+1)*MatBLKSIZE+offset;
  if (fseek(mptr->fptr,file_pos,0) == EOF) {
    printf(" Read_Scan3D_Ecat7> !!! Error: can not fseek position %0d for %s\n",
           file_pos,mptr->fname);
    return -1;
  } else if (fread(cdata,type_size,data_size,mptr->fptr) != data_size) {
    printf(" Read_Scan3D_Ecat7> !!! Error: can not read matrix %0d in %s\n",matnum,mptr->fname);
    return -1;
  }
  /*}}}  */
  printf("RS3DE7 4\n") ;
  csize = 0;
  maxOnData = 0 ;
  maxOnScaledData = 0. ;
  for (bin=0; bin<data_size; bin++) {
    bufRead_f(&fdata,cdata,&csize);
    if( fdata > maxOnData )
      maxOnData = fdata ;
    if( fdata*scalefac > maxOnScaledData )
      maxOnScaledData = fdata*scalefac ;
    outdata[bin] = fdata*scalefac;
  }
  printf( "max on data is %f and max on scaled data is %f", maxOnData, maxOnScaledData ) ;
  
  /* Il faut mieux g�rer cela */
/*   if (mptr->mhptr->file_type == Short3dSinogram) */
/*     { */
/*       for (bin=0; bin<data_size; bin++) */
/* 	{ */
/* 	  bufRead_s(&sdata,cdata,&csize); */
/* 	  outdata[bin] = sdata; */
/* 	}  */
/*     } */
/*   else */
/*     { */
/*     printf(" Read_Scan3D_Ecat7> !!! Error: can not read Float3dSinogram"); */
/*     return( -1); */
/*     } */
  free(cdata);
  return data_size;
}

#endif



short* EcatSinoRead_S16( UnifiedEcatSinoInfo *uesi, int segment, 
			 int bed, int gate, int frame )
{
  
  Main_header 		*Mh;
  MatrixFile 	        *mptr;
  Scan3D_subheader 	*sh;
  MatDirList            *mdlist;
  int                    matnum;
  int                    dataByte_num;
  int                    data_size, ret;
  short                 *outdata;



/*  printf("EcatSinoRead_S16 segment n� %d de %s \n", segment, uesi->fname );*/

  if ( (bed != 0 && gate != 1) || (bed != 0 && frame != 1) || (frame != 0 && gate != 1) ) 
    {
      printf("EcatSinoRead : file can not be simultaneously multy bed, multi frame and multi data");
      return ECATSHFJ_FAIL;
    }

  if ((mptr=matrix_open( uesi->fname,MAT_READ_ONLY, MAT_UNKNOWN_FTYPE))==NULL) 
    {
      printf(" EcatSinoRead>!!! Error: can not re-open %s\n",uesi->fname);
      return ECATSHFJ_FAIL;
    }


  /* test du type de sinogram : return si mauvais type */
  if (mptr->mhptr->file_type == Short3dSinogram || 
      mptr->mhptr->file_type == Float3dSinogram) 
    {
      /* lit  Main Header :                      */
      /*.. he oui on le relit.. cela a d�j� �t� fait par le EcatSinoOpen */
      Mh = mptr->mhptr;
 

      /* On lit le premier subheader */
      sh = (Scan3D_subheader*) calloc(1,sizeof(Scan3D_subheader));
      if ( !sh)
	{
	  printf(" EcatSinoRead>!!! error: cannot allocate subheader\n");
	  return ECATSHFJ_FAIL;
	}
      if (mat_read_Scan3D_subheader(mptr->fptr,mptr->mhptr,
				    mptr->dirlist->first->strtblk,sh))
	{
	  printf(" EcatSinoRead>!!!error: can not read first scan3D subheader\n");;
	  return ECATSHFJ_FAIL;
	}

      /* Allocation du buffer interm�diaire pour remonter les donn�es */
      /* a la proc�dure appelante */
      if (mptr->mhptr->file_type == Short3dSinogram)
	{
	  dataByte_num  = sizeof(short);
	} 
      else
	{
	  printf(" EcatSinoRead>!!! Error: '%s' is not a Short3dSinogram matrix file (%0d)\n",
		 mptr->fname,mptr->mhptr->file_type);
	  return ECATSHFJ_FAIL;
	}
      data_size = sh->num_r_elements        * 
	          sh->num_z_elements[ my_abs(segment) ] *
	          sh->num_angles      / 
	          (segment ? 2 : 1);
      outdata = (short*) calloc(1,dataByte_num*data_size);
      if ( !outdata )
	{
	  printf(" EcatSinoRead>!!! error: cannot allocate outdata buffer\n");
	  return ECATSHFJ_FAIL;
	}


      /* Recherche du matnum */
      mdlist = mptr->dirlist;
      if (mdlist->nmats != 1) /* On est pas dans un sino mono lit */
      /*-----------Ici faire un algo de calcul du matnum */
      /* La fonction devra prendre un argument suppl�mentaire: lit */
      /* L'algorithme sera
       matnode = mdlist->first; 
       for (l=0; l < bedOrDataOrFrame ; l++)
	 matnode = matnode->next;
       matnum = matnode->matnum;
       matnode = mptr->dirlist->first;
      */
	matnum = mat_numcod( frame, 1, gate, 0, bed ) ;
      else
	matnum = mdlist->first->matnum;
      /* Appel � la lecture des donn�es */
      if ( (ret=Read_Scan3D_Ecat7( mptr, sh, outdata, matnum, segment))
	     != data_size )
	{
	  printf(" EcatSinoRead>!!! Error: Read %d bytes instead of %d from %s)\n",
		 ret, data_size,uesi->fname);
	  return ECATSHFJ_FAIL;
	}
      else
	{
	  matrix_close( mptr );
	  free (Mh) ;
	  free (sh) ;
	  free (mdlist);
	  return( outdata );
	}
    }
  else
    {
      printf(" EcatSinoRead>!!! Error: can not re-open %s\n",uesi->fname);
      return ECATSHFJ_FAIL;
    }
  
}

float* EcatSinoRead_FLOAT( UnifiedEcatSinoInfo *uesi, int segment, 
			   int bed, int gate, int frame )
{
  
  /*Main_header 		*Mh = 0;*/
  MatrixFile 	        *mptr = 0;
  Scan3D_subheader 	*sh = 0;
  MatDirList            *mdlist = 0;
  int                    matnum;
  int                    dataByte_num;
  int                    data_size, ret;
  float                 *outdata = 0;
  float maxOnData = 0. ;
  int i ;


/*  printf("EcatSinoRead_S16 segment n� %d de %s \n", segment, uesi->fname );*/

  if ( (bed != 0 && gate != 1) || (bed != 0 && frame != 1) || (frame != 0 && gate != 1) ) 
    {
      printf("EcatSinoRead : file can not be simultaneously multy bed, multi frame and multi data");
      return ECATSHFJ_FAIL;
    }

  if ((mptr=matrix_open( uesi->fname,MAT_READ_ONLY, MAT_UNKNOWN_FTYPE))==NULL) 
    {
      printf(" EcatSinoRead>!!! Error: can not re-open %s\n",uesi->fname);
      return ECATSHFJ_FAIL;
    }


  /* test du type de sinogram : return si mauvais type */
  if (mptr->mhptr->file_type == Short3dSinogram || 
      mptr->mhptr->file_type == Float3dSinogram) 
    {
      /* lit  Main Header :                      */
      /*.. he oui on le relit.. cela a d�j� �t� fait par le EcatSinoOpen */
      /*Mh = mptr->mhptr;*/
      //printf("A\n");

      /* On lit le premier subheader */
      sh = (Scan3D_subheader*) calloc(1,sizeof(Scan3D_subheader));
      if ( !sh)
	{
	  printf(" EcatSinoRead>!!! error: cannot allocate subheader\n");
	  return ECATSHFJ_FAIL;
	}
      if (mat_read_Scan3D_subheader(mptr->fptr,mptr->mhptr,
				    mptr->dirlist->first->strtblk,sh))
	{
	  printf(" EcatSinoRead>!!!error: can not read first scan3D subheader\n");;
	  return ECATSHFJ_FAIL;
	}

      /* Allocation du buffer interm�diaire pour remonter les donn�es */
      /* a la proc�dure appelante */
      if (mptr->mhptr->file_type == Float3dSinogram)
	{
	  dataByte_num  = sizeof(float);
	} 
      else
	{
	  printf(" EcatSinoRead>!!! Error: '%s' is not a Float3dSinogram matrix file (%0d)\n",
		 mptr->fname,mptr->mhptr->file_type);
	  return ECATSHFJ_FAIL;
	}

      //printf("B\n");

      data_size = sh->num_r_elements        * 
	          sh->num_z_elements[ my_abs(segment) ] *
	          sh->num_angles      / 
	          (segment ? 2 : 1);
      outdata = (float*) calloc(1,dataByte_num*data_size);
      if ( !outdata )
	{
	  printf(" EcatSinoRead>!!! error: cannot allocate outdata buffer\n");
	  return ECATSHFJ_FAIL;
	}

      //printf("C\n");

      /* Recherche du matnum */
      mdlist = mptr->dirlist;
      if (mdlist->nmats != 1) /* On est pas dans un sino mono lit */
      /*-----------Ici faire un algo de calcul du matnum */
      /* La fonction devra prendre un argument suppl�mentaire: lit */
      /* L'algorithme sera
       matnode = mdlist->first; 
       for (l=0; l < bedOrDataOrFrame ; l++)
	 matnode = matnode->next;
       matnum = matnode->matnum;
       matnode = mptr->dirlist->first;
      */
	matnum = mat_numcod( frame, 1, gate, 0, bed ) ;
      else
	matnum = mdlist->first->matnum;
      
      
      //printf("D bef\n");
      ret=Read_Scan3D_Ecat7_FLOAT( mptr, sh, outdata, matnum, segment) ;
      //printf("D aft\n");

      /* Appel � la lecture des donn�es */
      if ( ret != data_size )
	{
	  printf(" EcatSinoRead>!!! Error: Read %d bytes instead of %d from %s)\n",
		 ret, data_size,uesi->fname);
	  return ECATSHFJ_FAIL;
	}
      else
	{
	  maxOnData = 0. ;
	  //printf("E\n");
	  for( i = 0 ; i < data_size ; ++i )
	    if( outdata[i] > maxOnData )
	      maxOnData = outdata[i] ;
	  
	  //printf( "max on data %f\n", maxOnData) ;
	  matrix_close( mptr );
	  //free (Mh) ;
	  free (sh) ;
	  //free (mdlist);
	  return( outdata );
	}
    }
  else
    {
      printf(" EcatSinoRead>!!! Error: can not re-open %s\n",uesi->fname);
      return ECATSHFJ_FAIL;
    }
}

