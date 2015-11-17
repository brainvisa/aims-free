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



#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <float.h>

#include <ecat/kernel/matrix.h>
#include <ecat/kernel/machine_indep.h>

/* declare an internal function from libecat */
int mh_update( MatrixFile *file );

/* #include <scanner_model_5_3.h> */
/* #include <machine_indep.h> */
/* #include <ecat+/io/rwToolBox.h> */
/* #include <ecat+/io/general.h> */

int Write_Scan3D_Ecat7_S16(MatrixFile *mptr, Scan3D_subheader *sh, short indata[], int matnum, int segment)
/*

   - Writes segment "segment" of matrix "matnum" in the matrix file pointed by "mptr".

   - Overwrites the scan3D subheader with "sh", except for: scale_factor, scan_min and scan_max which
     are calculated according to the content of the current segment and according to the previous values.

   - If sh->scale_factor == 1.0, then there is no scaling to 32767
*/
{
  short            min,max ;
  float            scale_factor;
  int              segment_occurance,seg,tot_data_size,nblks,blkno,file_pos,offset,
                   s_seg,nseg,csize,type_size,data_size_0;
  size_t           size, data_size, bin;
  struct MatDir    matdir, dir_entry;
  short            sdata;
  Scan3D_subheader *lsh;
  char             *cdata;
  int              i;


  /*{{{  sizes and scale factors*/
  type_size = sizeof(short);
  data_size_0 = sh->num_r_elements*sh->num_angles*sh->num_z_elements[0];
  if ((cdata = (char*) calloc(type_size,data_size_0)) == NULL) {
    printf(" Write_Scan3D_Ecat7> !!! Error: can not allocate %0d bytes\n",type_size*data_size_0);
    return -1;
  }
  /*{{{  occurance of the current segment, its size and its offset*/
  seg=0;
  data_size=0;
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
    printf(" Write_Scan3D_Ecat7> !!! Error: unexpected segment requested (%0d)\n",segment);
    return -1;
  }
  /*}}}  */
  /*{{{  total size of the matrix*/
  tot_data_size = 0;
  seg = 0;
  while (sh->num_z_elements[seg]>0) {
    tot_data_size += sh->num_r_elements*sh->num_angles*sh->num_z_elements[seg];
    seg++;
  }
  nblks = (tot_data_size*type_size+MatBLKSIZE-1)/MatBLKSIZE;
  /*}}}  */
  /*{{{  scale factor, min and max of the current segment*/
  min=indata[0];
  max=indata[0];
  for (bin=1;bin<data_size;bin++) {
    if (indata[bin]>max) max=indata[bin];
    if (indata[bin]<min) min=indata[bin];
  }
  scale_factor = 1.0;
  if (mptr->mhptr->file_type == Short3dSinogram) {
    if (sh->scale_factor == 1.0) {
      if (max > 32767.0) {
        printf(" Write_Scan3D_Ecat7> !!! Error: max is bigger than 32767 (%d)\n",max);
        return -1;
      } else if (min < -32767.0) {
        printf(" Write_Scan3D_Ecat7> !!! Error: min is smaller than -32767 (%d)\n",min);
        return -1;
      }
    } else {
      if ((max > 0.0) && (max > -min))     scale_factor = max/32767;
      else if ((min < 0.0) &&(min < -max)) scale_factor = -min/32767.0;
    }
  }
  /*}}}  */
  /*}}}  */
  /*{{{  the matrix does not already exist*/
  if (matrix_find(mptr,matnum,&matdir) == -1) {
    printf("\n");
    /*{{{  Create an entry for it in the directory block*/
    blkno = mat_enter(mptr->fptr,mptr->mhptr,matnum,nblks+1);
    dir_entry.matnum = matnum;
    dir_entry.strtblk = blkno;
    dir_entry.endblk  = dir_entry.strtblk + nblks + 1;
    dir_entry.matstat = 1;
    insert_mdir(dir_entry,mptr->dirlist);
    matdir = dir_entry;
    /*}}}  */
    /*{{{  Write the subheader*/
    sh->scale_factor = scale_factor;
    sh->scan_min = min/sh->scale_factor;
    sh->scan_max = max/sh->scale_factor;
    sh->data_type = SunShort ;
    mat_write_Scan3D_subheader(mptr->fptr,mptr->mhptr,matdir.strtblk,sh);
    /*}}}  */
    /*{{{  Fill with zeroes the entire matrix*/
    csize = 0;
    for (i=0;i<data_size_0;i++) bufWrite_s(0,cdata,&csize);

    file_pos = (matdir.strtblk+1)*MatBLKSIZE;
    if (fseek(mptr->fptr,file_pos,0) == EOF) {
      printf(" Write_Scan3D_Ecat7> !!! Error: fseek for position %0d for %s\n",file_pos,mptr->fname);
      return -1;
    }
    seg = 0;
    while (sh->num_z_elements[seg]>0) {
      if (seg == 0) {
        size = sh->num_r_elements*sh->num_angles*sh->num_z_elements[seg];
        nseg = 1;
      } else {
        size = sh->num_r_elements*sh->num_angles*sh->num_z_elements[seg]/2;
        nseg = 2;
      }
      for (i=0;i<nseg;i++) {
        s_seg = pow(-1,i)*seg;
        if (fwrite(cdata,type_size,size,mptr->fptr) != size) {
          printf(" Write_Scan3D_Ecat7> !!! error writting matrix %0d and segment %0d in %s\n",matnum,s_seg,mptr->fname);
          return -1;
        }
      }
      seg++;
    }
    /*}}}  */
  /*}}}  */
  /*{{{  the matrix already exists*/
  } else {
    lsh = (Scan3D_subheader*) calloc(1,sizeof(Scan3D_subheader));
    if (mat_read_Scan3D_subheader(mptr->fptr,mptr->mhptr, matdir.strtblk,lsh)) {
      printf(" Write_Scan3D_Ecat7> !!! Error: can not read already existing scan3D subheader %0d in '%s'\n",
             matnum,mptr->fname);
      return -1;
    }
    if (((mptr->mhptr->file_type == Short3dSinogram) && (lsh->data_type != SunShort)) ||
        ((mptr->mhptr->file_type == Float3dSinogram) && (lsh->data_type != IeeeFloat))) {
      printf(" Write_Scan3D_Ecat7> !!! Error: data type of already existing matrix %0d is %0d\n",matnum,lsh->data_type);
      return -1;
    }
    /*{{{  the scale factor changes ==> need to update the entire matrix*/
    if (scale_factor > lsh->scale_factor) {
      file_pos = (matdir.strtblk+1)*MatBLKSIZE;
      if (fseek(mptr->fptr,file_pos,0) == EOF) {
	printf(" Write_Scan3D_Ecat7> !!! Error: fseek position %0d for %s\n",file_pos,mptr->fname);
	return -1;
      }
      seg = 0;
      while (lsh->num_z_elements[seg]>0) {
	if (seg == 0) {
	  size = lsh->num_r_elements*lsh->num_angles*lsh->num_z_elements[seg];
	  nseg = 1;
	} else {
	  size = lsh->num_r_elements*lsh->num_angles*lsh->num_z_elements[seg]/2;
	  nseg = 2;
	}
	for (i=0;i<nseg;i++) {
	  s_seg = pow(-1,i)*seg;
	  if (fread(cdata,type_size,size,mptr->fptr) != size) {
	    printf(" Write_Scan3D_Ecat7> !!! Error: can not read matrix %0d and segment %0d in %s\n",matnum,s_seg,mptr->fname);
	    return -1;
	  }
	  csize = 0;
	  for (bin=0; bin<size; bin++) {
	    bufRead_s(&sdata,cdata,&csize);
	    sdata = (short) ((float)sdata*lsh->scale_factor/scale_factor);
	    csize -= type_size;
	    bufWrite_s(sdata,cdata,&csize);
	  }
	  if (fseek(mptr->fptr,-size*type_size,SEEK_CUR)) {
	    printf(" Write_Scan3D_Ecat7> !!! Error: can not set file position to segment %0d for matrix %0d in %s\n",
		   s_seg,matnum,mptr->fname);
	    return -1;
	  }
	  if (fwrite(cdata,type_size,size,mptr->fptr) != size) {
	    printf(" Write_Scan3D_Ecat7> !!! Error: can not write matrix %0d and segment %0d in %s\n",matnum,s_seg,mptr->fname);
	    return -1;
	  }
	}
	seg++;
      }
      sh->scale_factor = scale_factor;
    } else sh->scale_factor = lsh->scale_factor;
    /*}}}  */
    if (lsh->scan_min*lsh->scale_factor < min) sh->scan_min = lsh->scan_min*lsh->scale_factor/sh->scale_factor;
    else sh->scan_min = min/sh->scale_factor;
    if (lsh->scan_max*lsh->scale_factor > max) sh->scan_max = lsh->scan_max*lsh->scale_factor/sh->scale_factor;
    else sh->scan_max = max/sh->scale_factor;
    sh->data_type = SunShort;
    
    free(lsh);
    mat_write_Scan3D_subheader(mptr->fptr,mptr->mhptr,matdir.strtblk,sh);
  }
  /*}}}  */
  /*{{{  write segment*/
  csize = 0;
  for (bin=0; bin<data_size; bin++) {
    sdata = (short) (indata[bin]/sh->scale_factor);
    bufWrite_s(sdata,cdata,&csize);
  }
  file_pos = (matdir.strtblk+1)*MatBLKSIZE+offset;
  if (fseek(mptr->fptr,file_pos,0) == EOF) {
    printf(" Write_Scan3D_Ecat7> !!! Error fseek position %0d for %s\n",file_pos,mptr->fname);
    return -1;
  } else if (fwrite(cdata,type_size,data_size,mptr->fptr) != data_size) {
    printf(" Write_Scan3D_Ecat7> !!! Error: can not write matrix %0d in %s\n",matnum,mptr->fname);
    return -1;
  }
  /*}}}  */
  if (mh_update(mptr)) {
    printf(" Write_Scan3D_Ecat7> !!! Error: can not update main header in %s\n",mptr->fname);
    return -1;
  }
  free(cdata);
  return data_size;
}
/*}}}  */

/*{{{  Write_Full_Scan3D_Ecat7*/
int Write_Full_Scan3D_Ecat7_S16(MatrixFile *mptr, Scan3D_subheader *sh, short indata[], int matnum)
/*

   - Writes all segments of matrix "matnum" in the matrix file pointed by "mptr".

   - If sh->scale_factor == 1.0, then there is no scaling to 32767
*/
{
  short            min,max ;
  float            scale_factor;
  int              seg,data_size,data_size_0,bin,nblks,blkno,file_pos,type_size,
                   s_seg,csize,nseg;
  size_t           size;
  struct MatDir    matdir, dir_entry;
  short            sdata ;
  char             *cdata;
  int              i;
  size_t           j;


  /*{{{  sizes*/
  type_size = sizeof(short);
  data_size_0 = sh->num_r_elements*sh->num_angles*sh->num_z_elements[0];
  if ((cdata = (char*) calloc(type_size,data_size_0)) == NULL) {
    printf(" Write_Full_Scan3D_Ecat7> !!! Error: can not allocate %0d bytes\n",type_size*data_size_0);
    return -1;
  }
  data_size = 0;
  seg = 0;
  while (sh->num_z_elements[seg]>0) {
    data_size += sh->num_r_elements*sh->num_angles*sh->num_z_elements[seg];
    seg++;
  }
  nblks = (data_size*type_size+MatBLKSIZE-1)/MatBLKSIZE;
  /*}}}  */
  /*{{{  if new, create matrix entry*/
  if (matrix_find(mptr,matnum,&matdir) == -1) {
    blkno = mat_enter(mptr->fptr,mptr->mhptr,matnum,nblks+1);
    dir_entry.matnum = matnum;
    dir_entry.strtblk = blkno;
    dir_entry.endblk  = dir_entry.strtblk + nblks + 1;
    dir_entry.matstat = 1;
    insert_mdir(dir_entry,mptr->dirlist);
    matdir = dir_entry;
  }
  /*}}}  */
  /*{{{  scale factor, min and max*/
  min=indata[0];
  max=indata[0];
  for (bin=1;bin<data_size;bin++) {
    if (indata[bin]>max) max=indata[bin];
    if (indata[bin]<min) min=indata[bin];
  }
  scale_factor = 1.0;
  if (mptr->mhptr->file_type == Short3dSinogram) {
    if (sh->scale_factor == 1.0) {
      if (max > 32767.0) {
        printf(" Write_Full_Scan3D_Ecat7> !!! Error: max is bigger than 32767 (%d)\n",max);
        return -1;
      } else if (min < -32767.0) {
        printf(" Write_Full_Scan3D_Ecat7> !!! Error: min is smaller than -32767 (%d)\n",min);
        return -1;
      }
    } else {
      if ((max > 0.0) && (max > -min))     scale_factor = max/32767;
      else if ((min < 0.0) &&(min < -max)) scale_factor = -min/32767.0;
    }
  }
  /*}}}  */
  /*{{{  subheader*/
  sh->scale_factor = scale_factor;
  sh->scan_min = min/sh->scale_factor;
  sh->scan_max = max/sh->scale_factor;
  sh->data_type = SunShort;
  mat_write_Scan3D_subheader(mptr->fptr,mptr->mhptr,matdir.strtblk,sh);
  /*}}}  */
  /*{{{  write matrix*/
  file_pos = (matdir.strtblk+1)*MatBLKSIZE;
  if (fseek(mptr->fptr,file_pos,0) == EOF) {
    printf(" Write_Full_Scan3D_Ecat7> !!! Error: can not seek position %0d for %s\n",file_pos,mptr->fname);
    return -1;
  }
  seg = 0;
  bin = 0;
  while (sh->num_z_elements[seg]>0) {
    if (seg == 0) {
      size = sh->num_r_elements*sh->num_angles*sh->num_z_elements[seg];
      nseg = 1;
    } else {
      size = sh->num_r_elements*sh->num_angles*sh->num_z_elements[seg]/2;
      nseg = 2;
    }
    for (i=0;i<nseg;i++) {
      s_seg = pow(-1,i)*seg;
      csize = 0;
      for (j=0; j<size; j++) {
	sdata = (short) (indata[bin]/sh->scale_factor);
	bufWrite_s(sdata,cdata,&csize);
	bin++;
      }
      if (fwrite(cdata,type_size,size,mptr->fptr) != size) {
        printf(" Write_Full_Scan3D_Ecat7> !!! Error: can not write matrix %0d and segment %0d in %s\n",matnum,s_seg,mptr->fname);
        return -1;
      }
    }
    seg++;
  }
  if (bin != data_size) {
    printf(" Write_Full_Scan3D_Ecat7> !!! Error: do not loop on all 4D elements (%0d) of the sinogram (%0d)\n",data_size,bin);
    return -1;
  }
  /*}}}  */
  if (mh_update(mptr)) {
    printf(" Write_Full_Scan3D_Ecat7> !!! Error: can not update main header in %s\n",mptr->fname);
    return -1;
  }
  free(cdata);
  return data_size;
}

int Write_Scan3D_Ecat7_FLOAT(MatrixFile *mptr, Scan3D_subheader *sh, float indata[], int matnum, int segment)
/*

   - Writes segment "segment" of matrix "matnum" in the matrix file pointed by "mptr".

   - Overwrites the scan3D subheader with "sh", except for: scale_factor, scan_min and scan_max which
     are calculated according to the content of the current segment and according to the previous values.

   - If sh->scale_factor == 1.0, then there is no scaling to 32767
*/
{
  float            min,max,scale_factor;
  int              segment_occurance,seg,tot_data_size,nblks,blkno,file_pos,offset,
                   s_seg,nseg,csize,type_size,data_size_0;
  size_t           size, data_size, bin;
  struct MatDir    matdir, dir_entry;
  Scan3D_subheader *lsh;
  char             *cdata;
  int              i;

  //printf("Write_Scan3D_Ecat7_FLOAT\n");

  /*{{{  sizes and scale factors*/
  type_size = sizeof(float);
  data_size_0 = sh->num_r_elements*sh->num_angles*sh->num_z_elements[0];
  if ((cdata = (char*) calloc(type_size,data_size_0)) == NULL) {
    printf(" Write_Scan3D_Ecat7> !!! Error: can not allocate %0d bytes\n",type_size*data_size_0);
    return -1;
  }
  /*{{{  occurance of the current segment, its size and its offset*/
  seg=0;
  data_size=0;
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
    printf(" Write_Scan3D_Ecat7> !!! Error: unexpected segment requested (%0d)\n",segment);
    return -1;
  }
  /*}}}  */
  /*{{{  total size of the matrix*/
  tot_data_size = 0;
  seg = 0;
  while (sh->num_z_elements[seg]>0) {
    tot_data_size += sh->num_r_elements*sh->num_angles*sh->num_z_elements[seg];
    seg++;
  }
  nblks = (tot_data_size*type_size+MatBLKSIZE-1)/MatBLKSIZE;
  /*}}}  */
  /*{{{  scale factor, min and max of the current segment*/
  min=indata[0];
  max=indata[0];
  for (bin=1;bin<data_size;bin++) {
    if (indata[bin]>max) max=indata[bin];
    if (indata[bin]<min) min=indata[bin];
  }
  scale_factor = 1.0;
  /*{{{  the matrix does not already exist*/
  if (matrix_find(mptr,matnum,&matdir) == -1) {
    printf("\n");
    //printf("Matrix does not exist\n");
    /*{{{  Create an entry for it in the directory block*/
    blkno = mat_enter(mptr->fptr,mptr->mhptr,matnum,nblks+1);
    dir_entry.matnum = matnum;
    dir_entry.strtblk = blkno;
    dir_entry.endblk  = dir_entry.strtblk + nblks + 1;
    dir_entry.matstat = 1;
    insert_mdir(dir_entry,mptr->dirlist);
    matdir = dir_entry;
    /*}}}  */
    /*{{{  Write the subheader*/
    sh->scale_factor = scale_factor;
    sh->scan_min = min/sh->scale_factor;
    sh->scan_max = max/sh->scale_factor;
    sh->data_type = IeeeFloat;
    mat_write_Scan3D_subheader(mptr->fptr,mptr->mhptr,matdir.strtblk,sh);
    /*}}}  */
    /*{{{  Fill with zeroes the entire matrix*/
    csize = 0;
    
    for (i=0;i<data_size_0;i++) bufWrite_f(0.0,cdata,&csize);
    
    file_pos = (matdir.strtblk+1)*MatBLKSIZE;
    if (fseek(mptr->fptr,file_pos,0) == EOF) {
      printf(" Write_Scan3D_Ecat7> !!! Error: fseek for position %0d for %s\n",file_pos,mptr->fname);
      return -1;
    }
    seg = 0;
    while (sh->num_z_elements[seg]>0) {
      if (seg == 0) {
        size = sh->num_r_elements*sh->num_angles*sh->num_z_elements[seg];
        nseg = 1;
      } else {
        size = sh->num_r_elements*sh->num_angles*sh->num_z_elements[seg]/2;
        nseg = 2;
      }
      for (i=0;i<nseg;i++) {
        s_seg = pow(-1,i)*seg;
        if (fwrite(cdata,type_size,size,mptr->fptr) != size) {
          printf(" Write_Scan3D_Ecat7> !!! error writting matrix %0d and segment %0d in %s\n",matnum,s_seg,mptr->fname);
          return -1;
        }
      }
      seg++;
    }
    /*}}}  */
  /*}}}  */
  /*{{{  the matrix already exists*/
  } else {
    //printf("Matrix exists\n");
    
    lsh = (Scan3D_subheader*) calloc(1,sizeof(Scan3D_subheader));
    if (mat_read_Scan3D_subheader(mptr->fptr,mptr->mhptr, matdir.strtblk,lsh)) {
      printf(" Write_Scan3D_Ecat7> !!! Error: can not read already existing scan3D subheader %0d in '%s'\n",
             matnum,mptr->fname);
      return -1;
    }
    if (((mptr->mhptr->file_type == Short3dSinogram) && (lsh->data_type != SunShort)) ||
        ((mptr->mhptr->file_type == Float3dSinogram) && (lsh->data_type != IeeeFloat))) {
      printf(" Write_Scan3D_Ecat7> !!! Error: data type of already existing matrix %0d is %0d\n",matnum,lsh->data_type);
      return -1;
    }
    sh->scale_factor = scale_factor;
    if (lsh->scan_min > min) sh->scan_min = min;
    else sh->scan_min = lsh->scan_min;
    if (lsh->scan_max < max) sh->scan_max = max;
    else sh->scan_max = lsh->scan_max;
    sh->data_type = IeeeFloat;
    free(lsh);
    mat_write_Scan3D_subheader(mptr->fptr,mptr->mhptr,matdir.strtblk,sh);
  }
  /*}}}  */
  /*{{{  write segment*/
  csize = 0;
  //printf( "writing %d elts in buffer", data_size ) ;
  for (bin=0; bin<data_size; bin++) {
    bufWrite_f(indata[bin],cdata,&csize);
  }
  
  //printf( "write %d elts in buffer", data_size ) ;

  file_pos = (matdir.strtblk+1)*MatBLKSIZE+offset;
  if (fseek(mptr->fptr,file_pos,0) == EOF) {
    printf(" Write_Scan3D_Ecat7> !!! Error fseek position %0d for %s\n",file_pos,mptr->fname);
    return -1;
  } else if (fwrite(cdata,type_size,data_size,mptr->fptr) != data_size) {
    printf(" Write_Scan3D_Ecat7> !!! Error: can not write matrix %0d in %s\n",matnum,mptr->fname);
    return -1;
  }
  //printf( "data writen" ) ;
  /*}}}  */
  if (mh_update(mptr)) {
    printf(" Write_Scan3D_Ecat7> !!! Error: can not update main header in %s\n",mptr->fname);
    return -1;
  }
  free(cdata);
  return data_size;
}
/*}}}  */

/*{{{  Write_Full_Scan3D_Ecat7*/
int Write_Full_Scan3D_Ecat7_FLOAT(MatrixFile *mptr, Scan3D_subheader *sh, float indata[], int matnum)
/*

   - Writes all segments of matrix "matnum" in the matrix file pointed by "mptr".

   - If sh->scale_factor == 1.0, then there is no scaling to 32767
*/
{
  float            min,max,scale_factor;
  int              seg,data_size,data_size_0,bin,nblks,blkno,file_pos,type_size,
                   s_seg,csize,nseg;
  size_t           size;
  struct MatDir    matdir, dir_entry;
  char             *cdata;
  int              i;
  size_t           j;


  /*{{{  sizes*/
  type_size = sizeof(float);
  data_size_0 = sh->num_r_elements*sh->num_angles*sh->num_z_elements[0];
  if ((cdata = (char*) calloc(type_size,data_size_0)) == NULL) {
    printf(" Write_Full_Scan3D_Ecat7> !!! Error: can not allocate %0d bytes\n",type_size*data_size_0);
    return -1;
  }
  data_size = 0;
  seg = 0;
  while (sh->num_z_elements[seg]>0) {
    data_size += sh->num_r_elements*sh->num_angles*sh->num_z_elements[seg];
    seg++;
  }
  nblks = (data_size*type_size+MatBLKSIZE-1)/MatBLKSIZE;
  /*}}}  */
  /*{{{  if new, create matrix entry*/
  if (matrix_find(mptr,matnum,&matdir) == -1) {
    blkno = mat_enter(mptr->fptr,mptr->mhptr,matnum,nblks+1);
    dir_entry.matnum = matnum;
    dir_entry.strtblk = blkno;
    dir_entry.endblk  = dir_entry.strtblk + nblks + 1;
    dir_entry.matstat = 1;
    insert_mdir(dir_entry,mptr->dirlist);
    matdir = dir_entry;
  }
  /*}}}  */
  /*{{{  scale factor, min and max*/
  min=indata[0];
  max=indata[0];
  for (bin=1;bin<data_size;bin++) {
    if (indata[bin]>max) max=indata[bin];
    if (indata[bin]<min) min=indata[bin];
  }
  scale_factor = 1.0;
  /*}}}  */
  /*{{{  subheader*/
  sh->scale_factor = scale_factor;
  sh->scan_min = min/sh->scale_factor;
  sh->scan_max = max/sh->scale_factor;
  sh->data_type = IeeeFloat;
  mat_write_Scan3D_subheader(mptr->fptr,mptr->mhptr,matdir.strtblk,sh);
  /*}}}  */
  /*{{{  write matrix*/
  file_pos = (matdir.strtblk+1)*MatBLKSIZE;
  if (fseek(mptr->fptr,file_pos,0) == EOF) {
    printf(" Write_Full_Scan3D_Ecat7> !!! Error: can not seek position %0d for %s\n",file_pos,mptr->fname);
    return -1;
  }
  seg = 0;
  bin = 0;
  while (sh->num_z_elements[seg]>0) {
    if (seg == 0) {
      size = sh->num_r_elements*sh->num_angles*sh->num_z_elements[seg];
      nseg = 1;
    } else {
      size = sh->num_r_elements*sh->num_angles*sh->num_z_elements[seg]/2;
      nseg = 2;
    }
    for (i=0;i<nseg;i++) {
      s_seg = pow(-1,i)*seg;
      csize = 0;
      for (j=0; j<size; j++) {
	bufWrite_f(indata[bin],cdata,&csize);
	bin++;
      }
      
      if (fwrite(cdata,type_size,size,mptr->fptr) != size) {
        printf(" Write_Full_Scan3D_Ecat7> !!! Error: can not write matrix %0d and segment %0d in %s\n",matnum,s_seg,mptr->fname);
        return -1;
      }
    }
    seg++;
  }
  if (bin != data_size) {
    printf(" Write_Full_Scan3D_Ecat7> !!! Error: do not loop on all 4D elements (%0d) of the sinogram (%0d)\n",data_size,bin);
    return -1;
  }
  /*}}}  */
  if (mh_update(mptr)) {
    printf(" Write_Full_Scan3D_Ecat7> !!! Error: can not update main header in %s\n",mptr->fname);
    return -1;
  }
  free(cdata);
  return data_size;
}

int Write_Atten_Ecat7(MatrixFile *mptr, Attn_subheader *sh, float indata[], int matnum, int segment)
/*

   - Writes segment "segment" of matrix "matnum" in the matrix file pointed by "mptr".

   - Overwrites the atten subheader with "sh", except for: attenuation_min and attenuation_max which
     are calculated according to the content of the current segment and according to the previous values.

*/
{
  float          min,max;
  int            segment_occurance,seg,tot_data_size,nblks,blkno,file_pos,offset;
  struct MatDir  matdir, dir_entry;
  Attn_subheader *lsh;
  char           *cdata;
  int            i;
  int            type_size,data_size_0,nseg,s_seg,csize;
  size_t         size, data_size, bin;

  /*{{{  type and segment 0 sizes*/
  if (mptr->mhptr->file_type != AttenCor) {
    printf(" Write_Atten_Ecat7> !!! Error: '%s' is not a AttenCor matrix file (%0d)\n",
           mptr->fname,mptr->mhptr->file_type);
    return -1;
  }
  type_size = sizeof(float);
  data_size_0 = sh->num_r_elements*sh->num_angles*sh->z_elements[0];
  if ((cdata = (char*) calloc(type_size,data_size_0)) == NULL) {
    printf(" Write_Atten_Ecat7> !!! Error: can not allocate %0d bytes\n",type_size*data_size_0);
    return -1;
  }
  /*}}}  */
  /*{{{  occurance of the current segment, its size and its offset*/
  seg = 0;
  data_size = 0;
  offset = 0;
  if (segment != 0) {
    segment_occurance = -1;
    while ((segment < -seg)||(segment > +seg)) {
      offset += sh->num_r_elements*sh->num_angles*sh->z_elements[seg]*type_size;
      seg++;
      segment_occurance +=2;
    }
    if (segment < 0) {
      segment_occurance++;
      offset += sh->num_r_elements*sh->num_angles*sh->z_elements[seg]*type_size/2;
    }
    data_size = sh->num_r_elements*sh->num_angles*sh->z_elements[seg]/2;
  } else {
    segment_occurance = 0;
    data_size = sh->num_r_elements*sh->num_angles*sh->z_elements[seg];
  }
  if (data_size == 0) {
    printf(" Write_Atten_Ecat7> !!! Error: unexpected segment requested (%0d)\n",segment);
    return -1;
  }
  /*}}}  */
  /*{{{  total size of the matrix (in 4bytes and 512bytes blocks)*/
  tot_data_size = 0;
  seg = 0;
  while (sh->z_elements[seg]>0) {
    tot_data_size += sh->num_r_elements*sh->num_angles*sh->z_elements[seg];
    seg++;
  }
  nblks = (tot_data_size*type_size+MatBLKSIZE-1)/MatBLKSIZE;
  /*}}}  */
  /*{{{  min and max of the current segment*/
  min=indata[0];
  max=indata[0];
  for (bin=1;bin<data_size;bin++) {
    if (indata[bin] > max) max=indata[bin];
    if (indata[bin] < min) min=indata[bin];
  }
  /*}}}  */
  /*{{{  the matrix does not already exist*/
  if (matrix_find(mptr,matnum,&matdir) == -1) {
    /*{{{  Create an entry for it in the directory block*/
    blkno = mat_enter(mptr->fptr,mptr->mhptr,matnum,nblks+1);
    dir_entry.matnum = matnum;
    dir_entry.strtblk = blkno;
    dir_entry.endblk  = dir_entry.strtblk + nblks;
    dir_entry.matstat = 1;
    insert_mdir(dir_entry,mptr->dirlist);
    matdir = dir_entry;
    /*}}}  */
    /*{{{  Write the subheader*/
    sh->scale_factor = 1.0;
    sh->attenuation_min = min;
    sh->attenuation_max = max;
    sh->data_type = IeeeFloat;
    mat_write_attn_subheader(mptr->fptr,mptr->mhptr,matdir.strtblk,sh);
    /*}}}  */
    /*{{{  Fill with zeroes the entire matrix*/
    csize = 0;
    for (i=0;i<data_size_0;i++) bufWrite_f(0.0,cdata,&csize);
    file_pos = matdir.strtblk*MatBLKSIZE;
    if (fseek(mptr->fptr,file_pos,0) == EOF) {
      printf(" Write_Atten_Ecat7> !!! Error: fseek for position %0d for %s\n",file_pos,mptr->fname);
      return -1;
    }
    seg = 0;
    while (sh->z_elements[seg]>0) {
      if (seg == 0) {
        size = sh->num_r_elements*sh->num_angles*sh->z_elements[seg];
        nseg = 1;
      } else {
        size = sh->num_r_elements*sh->num_angles*sh->z_elements[seg]/2;
        nseg = 2;
      }
      for (i=0;i<nseg;i++) {
        s_seg = pow(-1,i)*seg;
        if (fwrite(cdata,type_size,size,mptr->fptr) != size) {
          printf(" Write_Atten_Ecat7> !!! error writting matrix %0d and segment %0d in %s\n",matnum,s_seg,mptr->fname);
          return -1;
        }
      }
      seg++;
    }
    /*}}}  */
  /*}}}  */
  /*{{{  the matrix already exists*/
  } else {
    lsh = (Attn_subheader*) calloc(1,sizeof(Attn_subheader));
    if (mat_read_attn_subheader(mptr->fptr,mptr->mhptr, matdir.strtblk,lsh)) {
      printf(" Write_Atten_Ecat7> !!! error: can not read already existing Atten subheader %0d in '%s'\n",
             matnum,mptr->fname);
      return -1;
    }
    if (lsh->data_type != IeeeFloat) {
      printf(" Read_Atten_Ecat7> !!! error: data type of already existing matrix %0d is %0d\n",matnum,sh->data_type);
      return -1;
    }
    if (lsh->attenuation_min > min) sh->attenuation_min = min;
    else sh->attenuation_min = lsh->attenuation_min;
    if (lsh->attenuation_max < max) sh->attenuation_max = max;
    else sh->attenuation_max = lsh->attenuation_max;
    free(lsh);
    mat_write_attn_subheader(mptr->fptr,mptr->mhptr,matdir.strtblk,sh);
  }
  /*}}}  */
  /*{{{  write segment*/
  csize = 0;
  for (bin=0; bin<data_size; bin++) bufWrite_f(indata[bin],cdata,&csize);
  file_pos = matdir.strtblk*MatBLKSIZE+offset;
  if (fseek(mptr->fptr,file_pos,0) == EOF) {
    printf(" Write_Atten_Ecat7> !!! Error: fseek position %0d for %s\n",file_pos,mptr->fname);
    return -1;
  } else if (fwrite(cdata,type_size,data_size,mptr->fptr) != data_size) {
    printf(" Write_Atten_Ecat7> !!! Error: can not write matrix %0d in %s\n",matnum,mptr->fname);
    return -1;
  }
  /*}}}  */
  if (mh_update(mptr)) {
    printf(" Write_Atten_Ecat7> !!! Error: can not update main header in %s\n",mptr->fname);
    return -1;
  }
  free(cdata);
  return data_size;
}

int Write_Full_Atten_Ecat7(MatrixFile *mptr, Attn_subheader *sh, float indata[], int matnum)
/*

   - Writes all segments of matrix "matnum" in the matrix file pointed by "mptr".

   - Overwrites the atten subheader with "sh", except for: attenuation_min and attenuation_max which

*/
{
  float          min,max;
  int            seg,data_size,bin,nblks,blkno,file_pos;
  struct MatDir  matdir, dir_entry;
  char           *cdata;
  int            i;
  int            type_size,data_size_0,nseg,s_seg,csize;
  size_t         size, j;

  /*{{{  type and segment 0 sizes*/
  if (mptr->mhptr->file_type != AttenCor) {
    printf(" Write_Full_Atten_Ecat7> !!! Error: '%s' is not a AttenCor matrix file (%0d)\n",
           mptr->fname,mptr->mhptr->file_type);
    return -1;
  }
  type_size = sizeof(float);
  data_size_0 = sh->num_r_elements*sh->num_angles*sh->z_elements[0];
  if ((cdata = (char*) calloc(type_size,data_size_0)) == NULL) {
    printf(" Write_Atten_Ecat7> !!! Error: can not allocate %0d bytes\n",type_size*data_size_0);
    return -1;
  }
  /*}}}  */
  /*{{{  total size of the matrix (in 4B and 512B units)*/
  data_size = 0;
  seg = 0;
  while (sh->z_elements[seg]>0) {
    data_size += sh->num_r_elements*sh->num_angles*sh->z_elements[seg];
    seg++;
  }
  nblks = (data_size*type_size+MatBLKSIZE-1)/MatBLKSIZE;
  /*}}}  */
  /*{{{  create new entry*/
  if (matrix_find(mptr,matnum,&matdir) == -1) {
    blkno = mat_enter(mptr->fptr,mptr->mhptr,matnum,nblks+1);
    dir_entry.matnum = matnum;
    dir_entry.strtblk = blkno;
    dir_entry.endblk  = dir_entry.strtblk + nblks;
    dir_entry.matstat = 1;
    insert_mdir(dir_entry,mptr->dirlist);
    matdir = dir_entry;
  }
  /*}}}  */
  /*{{{  min/max and scale factor*/
  min=indata[0];
  max=indata[0];
  for (bin=1;bin<data_size;bin++) {
    if (indata[bin]>max) max=indata[bin];
    if (indata[bin]<min) min=indata[bin];
  }
  sh->scale_factor = 1.0;
  sh->attenuation_min = min;
  sh->attenuation_max = max;
  /*}}}  */
  /*{{{  write subheader*/
  sh->data_type = IeeeFloat;
  mat_write_attn_subheader(mptr->fptr,mptr->mhptr,matdir.strtblk,sh);
  /*}}}  */
  /*{{{  write matrix*/
  file_pos = matdir.strtblk*MatBLKSIZE;
  if (fseek(mptr->fptr,file_pos,0) == EOF) {
    printf(" Write_Full_Atten_Ecat7> !!! Error: can not seek position %0d for %s\n",file_pos,mptr->fname);
    return -1;
  }
  seg = 0;
  bin = 0;
  while (sh->z_elements[seg]>0) {
    if (seg == 0) {
      size = sh->num_r_elements*sh->num_angles*sh->z_elements[seg];
      nseg = 1;
    } else {
      size = sh->num_r_elements*sh->num_angles*sh->z_elements[seg]/2;
      nseg = 2;
    }
    for (i=0;i<nseg;i++) {
      s_seg = pow(-1,i)*seg;
      csize = 0;
      for (j=0; j<size; j++) {
        bufWrite_f(indata[bin],cdata,&csize);
        bin++;
      }
      if (fwrite(cdata,type_size,size,mptr->fptr) != size) {
        printf(" Write_Full_Atten_Ecat7> !!! Error: can not write matrix %0d and segment %0d in %s\n",matnum,s_seg,mptr->fname);
        return -1;
      }
    }
    seg++;
  }
  if (bin != data_size) {
    printf(" Write_Full_Atten_Ecat7> !!! Error: do not loop on all 4D elements (%0d) of the sinogram (%0d)\n",data_size,bin);
    return -1;
  }
  /*}}}  */
  if (mh_update(mptr)) {
    printf(" Write_Full_Atten_Ecat7> !!! Error: can not update main header in %s\n",mptr->fname);
    return -1;
  }
  free(cdata);
  return data_size;
}

int Write_Image_Ecat7(MatrixFile *mptr, Image_subheader *sh, float indata[], int matnum)
/*

   - Writes matrix "matnum" in the matrix file pointed by "mptr".

   - Overwrites the image subheader with "sh", except for: scale_factor, image_min and image_max which
     are calculated according to the content of the current volume.

*/
{
  float          min,max;
  int            nblks,blkno,file_pos;
  struct MatDir  matdir, dir_entry;
  char           *cdata;
  short          sdata;
  int            type_size,csize;
  size_t         data_size, bin;

  if (mptr->mhptr->file_type != PetVolume) {
    printf(" Write_Image_Ecat7> !!! error: '%s' is not a PetVolume matrix file (%0d)\n",
           mptr->fname,mptr->mhptr->file_type);
    return -1;
  }
  type_size = sizeof(short);
  data_size = sh->x_dimension*sh->y_dimension*sh->z_dimension;
  nblks = (data_size*type_size+MatBLKSIZE-1)/MatBLKSIZE;
  if ((cdata = (char*) calloc(type_size,data_size)) == NULL) {
    printf(" Write_Image_Ecat7> !!! Error: can not allocate %0zu bytes\n",type_size*data_size);
    return -1;
  }
  if (matrix_find(mptr,matnum,&matdir) == -1) {
    blkno = mat_enter(mptr->fptr,mptr->mhptr,matnum,nblks);
    dir_entry.matnum = matnum;
    dir_entry.strtblk = blkno;
    dir_entry.endblk  = dir_entry.strtblk + nblks;
    dir_entry.matstat = 1;
    insert_mdir(dir_entry,mptr->dirlist);
    matdir = dir_entry;
  } else blkno = matdir.strtblk;
  sh->data_type = SunShort;
  min=indata[0];
  max=indata[0];
  for (bin=1;bin<data_size;bin++) {
    if (indata[bin]>max) max=indata[bin];
    if (indata[bin]<min) min=indata[bin];
  }
  if (sh->scale_factor != 1.0) {
    sh->scale_factor = 1.0;
    if ((max > 0.0) && (max > -min))     sh->scale_factor = max/32767;
    else if ((min < 0.0) &&(min < -max)) sh->scale_factor = -min/32767.0;
  }
  sh->image_min = min/sh->scale_factor;
  sh->image_max = max/sh->scale_factor;
  mat_write_image_subheader(mptr->fptr,mptr->mhptr,matdir.strtblk,sh);
  csize = 0;
  for (bin=0; bin<data_size; bin++) {
    sdata = (short) (indata[bin]/sh->scale_factor);
    bufWrite_s(sdata,cdata,&csize);
  }
  file_pos = matdir.strtblk*MatBLKSIZE;
  if (fseek(mptr->fptr,file_pos,0) == EOF) {
    printf(" Write_Image_Ecat7> !!! Error: can not fseek position %0d for %s\n",file_pos,mptr->fname);
    return -1;
  } else if (fwrite(cdata,type_size,data_size,mptr->fptr) != data_size) {
    printf(" Write_Image_Ecat7> !!! Error: can not write matrix %0d in %s\n",matnum,mptr->fname);
    return -1;
  }
  if (mh_update(mptr)) {
    printf(" Write_Image_Ecat7> !!! Error: can not update main header in %s\n",mptr->fname);
    return -1;
  }
  free(cdata);
  return data_size;
}

int Read_Scan3D_Ecat7(MatrixFile *mptr, Scan3D_subheader *sh, short *outdata, int matnum, int segment)
{
  int            seg,segment_occurance,offset,file_pos;
  float          scalefac=1.0;
  struct MatDir  matdir;
  short          sdata;
  char           *cdata;
  int            type_size,csize;
  size_t         data_size, bin;

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
    printf(" Read_Scan3D_Ecat7> !!! Error: can not read scan3D subheader %0d in '%s'\n",matnum,mptr->fname);
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
    printf(" Read_Scan3D_Ecat7> !!! Error: can not allocate %0zu bytes\n",type_size*data_size);
    return -1;
  }
  scalefac = 1.0;
  if (sh->scale_factor != 0.0) scalefac *= sh->scale_factor;
  /*{{{  read segment*/
  file_pos = (matdir.strtblk+1)*MatBLKSIZE+offset;
  if (fseek(mptr->fptr,file_pos,0) == EOF) {
    printf(" Read_Scan3D_Ecat7> !!! Error: can not fseek position %0d for %s\n",file_pos,mptr->fname);
    return -1;
  } else if (fread(cdata,type_size,data_size,mptr->fptr) != data_size) {
    printf(" Read_Scan3D_Ecat7> !!! Error: can not read matrix %0d in %s\n",matnum,mptr->fname);
    return -1;
  }
  /*}}}  */
  csize = 0;

  /* Il faut revoir cela*/
  if (mptr->mhptr->file_type == Short3dSinogram)
    {
      for (bin=0; bin<data_size; bin++)
	{
	  bufRead_s(&sdata,cdata,&csize);
	  outdata[bin] = sdata*scalefac;
	}
    }
  else if (mptr->mhptr->file_type == Float3dSinogram)
    {
      printf(" Read_Scan3D_Ecat7> !!! Error: can not read Float3dSinograme");
      return( -1);
      /*       for (bin=0; bin<data_size; bin++) */
      /* 	{ */
      /* 	  bufRead_f(&fdata,cdata,&csize); */
      /* 	  outdata[bin] = fdata*scalefac; */
      /* 	} */
    }
  free(cdata);
  return data_size;
}

int Read_Scan3D_Ecat7_FLOAT(MatrixFile *mptr, Scan3D_subheader *sh, float *outdata, int matnum, int segment)
{
  int            seg,segment_occurance,offset,file_pos;
  float          scalefac=1.0;
  struct MatDir  matdir;
  float          fdata;
  char           *cdata;
  int            type_size,csize;
  size_t         data_size, bin;

  //printf("RS3DE7 1\n") ;
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
    printf(" Read_Scan3D_Ecat7> !!! Error: can not read scan3D subheader %0d in '%s'\n",matnum,mptr->fname);
    return -1;
  } else if ((mptr->mhptr->file_type == Short3dSinogram && sh->data_type != SunShort) ||
             (mptr->mhptr->file_type == Float3dSinogram && sh->data_type != IeeeFloat)) {
    printf(" Read_Scan3D_Ecat7> !!! Error: data type of matrix %0d is %0d\n",matnum,sh->data_type);
    return -1;
  }
  /*}}}  */
  /*{{{  data size and segment offset*/
  //printf("RS3DE7 2\n") ;
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
  //printf("RS3DE7 3\n") ;
  if ((cdata = (char*) calloc(type_size,data_size)) == NULL) {
    printf(" Read_Scan3D_Ecat7> !!! Error: can not allocate %0zu bytes\n",type_size*data_size);
    return -1;
  }
  scalefac = 1.0;
  if (sh->scale_factor != 0.0) scalefac *= sh->scale_factor;
  /*{{{  read segment*/
  file_pos = (matdir.strtblk+1)*MatBLKSIZE+offset;
  if (fseek(mptr->fptr,file_pos,0) == EOF) {
    printf(" Read_Scan3D_Ecat7> !!! Error: can not fseek position %0d for %s\n",file_pos,mptr->fname);
    return -1;
  } else if (fread(cdata,type_size,data_size,mptr->fptr) != data_size) {
    printf(" Read_Scan3D_Ecat7> !!! Error: can not read matrix %0d in %s\n",matnum,mptr->fname);
    return -1;
  }
  /*}}}  */
  //printf("RS3DE7 4\n") ;
  csize = 0;
/*   maxOnData = 0 ; */
/*   maxOnScaledData = 0. ; */

  /* Il faut revoir cela*/
  if (mptr->mhptr->file_type == Float3dSinogram)
    {
      for (bin=0; bin<data_size; bin++)
	{
	  bufRead_f(&fdata,cdata,&csize);
/* 	  if( fdata > maxOnData ) */
/* 	    maxOnData = fdata ; */
/* 	  if( fdata*scalefac > maxOnScaledData ) */
/* 	    maxOnScaledData = fdata*scalefac ; */
	  outdata[bin] = fdata*scalefac;
	}
      //printf( "max on data is %f and max on scaled data is %f", maxOnData, maxOnScaledData ) ;
    }
  else if (mptr->mhptr->file_type == Short3dSinogram)
    {
      printf(" Read_Scan3D_Ecat7> !!! Error: can not read Short3dSinogram");
      return( -1);
      /*       for (bin=0; bin<data_size; bin++) */
      /* 	{ */
      /* 	  bufRead_f(&fdata,cdata,&csize); */
      /* 	  outdata[bin] = fdata*scalefac; */
      /* 	} */
    }
  free(cdata);
  return data_size;
}

int Read_Atten_Ecat7(MatrixFile *mptr, Attn_subheader *sh, float *outdata, int matnum, int segment)
{
  int            seg,segment_occurance,offset,file_pos;
  struct MatDir  matdir;
  float          fdata,scalefac;
  char           *cdata;
  int            type_size,csize;
  size_t         data_size,bin;

  /*{{{  type size*/
  if (mptr->mhptr->file_type != AttenCor) {
    printf(" Read_Attn_Ecat7> !!! Error: '%s' is not a AttenCor matrix file (%0d)\n",
           mptr->fname,mptr->mhptr->file_type);
    return -1;
  }
  type_size = sizeof(float);
  /*}}}  */
  /*{{{  read subheader*/
  if (matrix_find(mptr,matnum,&matdir)) {
    printf(" Read_Attn_Ecat7> !!! Error: can not find matrix %0d in %s\n",matnum,mptr->fname);
    return -1;
  } else if (mat_read_attn_subheader(mptr->fptr,mptr->mhptr, matdir.strtblk,sh)) {
    printf(" Read_Attn_Ecat7> !!! Error: can not read AttenCor subheader %0d in '%s'\n",matnum,mptr->fname);
    return -1;
  } else if (sh->data_type != IeeeFloat) {
    printf(" Read_Attn_Ecat7> !!! Error: data type of matrix %0d is %0d\n",matnum,sh->data_type);
    return -1;
  }
  /*}}}  */
  /*{{{  data size and offset*/
  seg = 0;
  data_size = 0;
  offset = 0;
  if (segment != 0) {
    segment_occurance = -1;
    while ((segment < -seg)||(segment > +seg)) {
      offset += sh->num_r_elements*sh->num_angles*sh->z_elements[seg]*type_size;
      seg++;
      segment_occurance +=2;
    }
    if (segment < 0) {
      segment_occurance++;
      offset += sh->num_r_elements*sh->num_angles*sh->z_elements[seg]*type_size/2;
    }
    data_size = sh->num_r_elements*sh->num_angles*sh->z_elements[seg]/2;
  } else {
    segment_occurance = 0;
    data_size = sh->num_r_elements*sh->num_angles*sh->z_elements[seg];
  }
  if (data_size == 0) {
    printf(" Read_Attn_Ecat7> !!! Error: unexpected segment requested (%0d)\n",segment);
    return -1;
  }
  /*}}}  */
  if ((cdata = (char*) calloc(type_size,data_size)) == NULL) {
    printf(" Read_Atten_Ecat7> !!! Error: can not allocate %0zu bytes\n",type_size*data_size);
    return -1;
  }
  scalefac = 1.0;
  if (sh->scale_factor != 0) scalefac *= sh->scale_factor;
  /*{{{  read segment*/
  file_pos = matdir.strtblk*MatBLKSIZE+offset;
  if (fseek(mptr->fptr,file_pos,0) == EOF) {
    printf(" Read_Attn_Ecat7> !!! Error: can not fseek position %0d for %s\n",file_pos,mptr->fname);
    return -1;
  } else if (fread(cdata,type_size,data_size,mptr->fptr) != data_size) {
    printf(" Read_Attn_Ecat7> !!! Error: can not read matrix %0d in %s\n",matnum,mptr->fname);
    return -1;
  }
  /*}}}  */
  csize = 0;
  for (bin=0; bin<data_size; bin++) {
    bufRead_f(&fdata,cdata,&csize);
    outdata[bin] = fdata*scalefac;
  }
  free(cdata);
  return data_size;
}

int Read_Image_Ecat7(MatrixFile *mptr, Image_subheader *sh, float *outdata, int matnum)
{
  int            file_pos;
  struct MatDir  matdir;
  short          sdata;
  char           *cdata;
  int            type_size,csize;
  float          scalefac;
  size_t         data_size,bin;

  /*{{{  type size*/
  if (mptr->mhptr->file_type != PetVolume && mptr->mhptr->file_type != PetImage) {
    printf(" Read_Image_Ecat7> !!! Error: '%s' is not a PetVolume nor a PetImage matrix file (%0d)\n",
           mptr->fname,mptr->mhptr->file_type);
    return -1;
  }
  type_size = sizeof(short);
  /*}}}  */
  /*{{{  read subheader*/
  if (matrix_find(mptr,matnum,&matdir)) {
    printf(" Read_Image_Ecat7> !!! Error: can not find matrix %0d in %s\n",matnum,mptr->fname);
    return -1;
  } else if (mat_read_image_subheader(mptr->fptr,mptr->mhptr, matdir.strtblk,sh)) {
    printf(" Read_Image_Ecat7> !!! Error: can not read PetVolume subheader %0d in '%s'\n",matnum,mptr->fname);
    return -1;
  } else if (sh->data_type != SunShort) {
    printf(" Read_Image_Ecat7> !!! Error: data type of matrix %0d is %0d\n",matnum,sh->data_type);
    return -1;
  }
  /*}}}  */
  data_size = sh->x_dimension*sh->y_dimension*sh->z_dimension;
  if ((cdata = (char*) calloc(type_size,data_size)) == NULL) {
    printf(" Read_Image_Ecat7> !!! Error: can not allocate %0zu bytes\n",type_size*data_size);
    return -1;
  }
  /*{{{  read image*/
  file_pos = matdir.strtblk*MatBLKSIZE;
  if (fseek(mptr->fptr,file_pos,0) == EOF) {
    printf(" Read_Image_Ecat7> !!! Error: can not fseek position %0d for %s\n",file_pos,mptr->fname);
    return -1;
  } else if (fread(cdata,type_size,data_size,mptr->fptr) != data_size) {
    printf(" Read_Image_Ecat7> !!! Error: can not read matrix %0d in %s\n",matnum,mptr->fname);
    return -1;
  }
  /*}}}  */
  scalefac = 1.0;
  if (sh->scale_factor != 0.0) scalefac *= sh->scale_factor;
  csize = 0;
  for (bin=0; bin<data_size; bin++) {
    bufRead_s(&sdata,cdata,&csize);
    outdata[bin] = (float)sdata*scalefac;
  }
  free(cdata);
  return data_size;
}


int Bin_Scan3D(int ring_1, int ring_2, int elem, int view, int nelem, int nview, int nsegment,
               int storage_order, int *delring_min, int *delring_max, int *zmin_seg, int *zmax_seg)
{
  int segment_occurance,bin,ring_diff;

  segment_occurance = 0;
  bin = 0;
  ring_diff = ring_2 - ring_1;
  while (segment_occurance<(2*nsegment+1) && (ring_diff < delring_min[segment_occurance] ||
                                              ring_diff > delring_max[segment_occurance]    )) {
    bin += (zmax_seg[segment_occurance]-zmin_seg[segment_occurance]+1)*nelem*nview;
    segment_occurance++;
  }
  if (ring_diff >= delring_min[segment_occurance] && ring_diff <= delring_max[segment_occurance]) {
    if (storage_order == ElAxVwRd) {
      bin += view*(zmax_seg[segment_occurance]-zmin_seg[segment_occurance]+1)*nelem;
      bin += (ring_1 + ring_2 - zmin_seg[segment_occurance])*nelem;
      bin += elem;
    } else {
      bin += (ring_1 + ring_2 - zmin_seg[segment_occurance])*nelem*nview;
      bin += view*nelem;
      bin += elem;
    }
  } else bin = -1;
  return bin;
}

