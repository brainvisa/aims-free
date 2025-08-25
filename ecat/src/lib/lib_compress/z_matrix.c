#include <stdlib.h>
#include <ecat/lib_compress/compress.h>
#include <ecat/kernel/matrix_extra.h>

#define OK 0
#define ERROR -1

int write_z_data(mptr, matnum, data)
MatrixFile *mptr ;
MatrixData *data ;
int	matnum;
{
	struct MatDir matdir, dir_entry;
	Scan_subheader *scansub=NULL ;
	Image_subheader *imagesub=NULL ;
	Attn_subheader *attnsub=NULL ;
	Norm_subheader *normsub=NULL ;
	int	status, blkno, nblks, data_type;
	int z_size, z_nblks;
	caddr_t z_data=NULL;
	int compress_method;

	compress_method = mptr->mhptr->compression_code/256;
	if (!compress_method) return write_host_data(mptr, matnum, data);

	status = OK ;
	nblks = (data->data_size+511)/512;
	switch(mptr->mhptr->file_type)
	{
		case Sinogram : default :
			scansub = (Scan_subheader *) data->shptr ;
			data_type = scansub->data_type;
			break ;
		case PetVolume :
			imagesub = (Image_subheader *) data->shptr ;
			data_type = imagesub->data_type;
			break ;
		case AttenCor :
			attnsub = (Attn_subheader *) data->shptr ;
			data_type = attnsub->data_type;
			break ;
		case Normalization :
			normsub = (Norm_subheader *) data->shptr ;
			data_type = normsub->data_type;
			break ;
	}
	z_data = (caddr_t)calloc(1,nblks*512);
	if (z_data == NULL) return(ERROR) ;
	z_size = compress(data, data_type, z_data, compress_method);
	if (z_size == 0 ) {
		free(z_data);
		return (ERROR);
	}
	z_nblks = (z_size+511)/512;
	if (matrix_find(mptr,matnum,&matdir) == ERROR)		/* new entry */
	{
		blkno = mat_enter(mptr->fptr, mptr->mhptr, matnum, z_nblks) ;
		dir_entry.matnum = matnum ;
		dir_entry.strtblk = blkno ;
		dir_entry.endblk = dir_entry.strtblk + z_nblks - 1 ;
		dir_entry.matstat = 1 ;
		insert_mdir(dir_entry, mptr->dirlist) ;
		matdir = dir_entry ;
	}
	switch(mptr->mhptr->file_type)
	{
		case Sinogram : default :
			mat_write_scan_subheader(mptr->fptr, mptr->mhptr, matdir.strtblk, scansub);
			break ;
		case PetVolume :
			mat_write_image_subheader(mptr->fptr,mptr->mhptr, matdir.strtblk,imagesub);
			break ;
		case AttenCor :
			mat_write_attn_subheader(mptr->fptr, mptr->mhptr, matdir.strtblk, attnsub);
			break ;
		case Normalization :
			mat_write_norm_subheader(mptr->fptr, mptr->mhptr, matdir.strtblk, normsub);
			break ;
	}
	status = mat_wblk(mptr->fptr, matdir.strtblk+1,z_data,z_nblks);
	free(z_data);
	return(status) ;
}

int read_z_data(mptr, matnum, data, dtype) 
MatrixFile	*mptr ;
MatrixData	*data ;
int	matnum , dtype;
{
	struct MatDir matdir;
	int	z_nblks=0, nblks=0;
	Scan_subheader *scansub=NULL ;
	Image_subheader *imagesub=NULL ;
	Attn_subheader *attnsub=NULL ;
	Norm_subheader *normsub=NULL ;
	caddr_t z_data=NULL;
	int compress_method, size = 0;

	compress_method = mptr->mhptr->compression_code/256;
	if (!compress_method) return read_host_data(mptr, matnum, data, dtype);

	if (matrix_find(mptr,matnum,&matdir) == ERROR) return ERROR;
	data->matnum = matnum;
	data->matfile = mptr;
	data->mat_type = mptr->mhptr->file_type;
/*
 read header 
*/
	switch(mptr->mhptr->file_type)
	{
	case Sinogram :
		scansub = (Scan_subheader *) data->shptr ;
		mat_read_scan_subheader(mptr->fptr, mptr->mhptr, matdir.strtblk, scansub);
		data->data_type = scansub->data_type ;
		data->xdim = scansub->num_r_elements ;
		data->ydim = scansub->num_angles ;
		data->scale_factor = scansub->scale_factor ;
		data->pixel_size = scansub->x_resolution;
		data->zdim = scansub->num_z_elements;
		data->data_max = scansub->scan_max * scansub->scale_factor ;
		break ;
	case PetVolume :
		imagesub = (Image_subheader *) data->shptr ;
		mat_read_image_subheader(mptr->fptr, mptr->mhptr, matdir.strtblk, imagesub);
		data->data_type = imagesub->data_type ;
		data->xdim = imagesub->x_dimension ;
		data->ydim = imagesub->y_dimension ;
		data->zdim = imagesub->z_dimension ;
		data->scale_factor = imagesub->scale_factor ;
		data->pixel_size = imagesub->x_pixel_size;
		data->data_max = imagesub->image_max * imagesub->scale_factor;
		break ;
	case AttenCor :
		attnsub = (Attn_subheader *) data->shptr ;
		mat_read_attn_subheader(mptr->fptr, mptr->mhptr, matdir.strtblk, attnsub);
		data->data_type = attnsub->data_type ;
		data->xdim = attnsub->num_r_elements ;
		data->ydim = attnsub->num_angles ;
		data->zdim = attnsub->num_z_elements ;
		data->scale_factor = attnsub->scale_factor ;
		data->pixel_size = attnsub->x_resolution;
		data->data_max = data->scale_factor * 
			find_fmax((float*)data->data_ptr, data->xdim * data->ydim);
		break ;
	case Normalization :
		normsub = (Norm_subheader *) data->shptr ;
		mat_read_norm_subheader(mptr->fptr, mptr->mhptr, matdir.strtblk, normsub);
		data->data_type = normsub->data_type ;
		data->xdim = normsub->num_r_elements ;
		data->ydim = normsub->num_angles ;
		data->zdim = normsub->num_z_elements ;
		data->scale_factor = normsub->scale_factor ;
		data->data_max = data->scale_factor * 
			find_fmax((float*)data->data_ptr, data->xdim * data->ydim);
		break ;
	default :
		return(ERROR) ;
		break ;
	}

	if (dtype == MAT_SUB_HEADER) return(OK);
/*
	read matrix raw data
*/
	data->data_size = data->xdim*data->ydim*sizeof(short);
	nblks = (data->data_size+511)/512;		/* block size */
	data->data_ptr = (caddr_t)calloc(1,nblks*512);
	if (data->data_ptr==NULL) return(ERROR) ;
	z_nblks = matdir.endblk - matdir.strtblk;
	z_data = (caddr_t)calloc(1,z_nblks*512);
	if (z_data==NULL) return(ERROR) ;
	mat_rblk(mptr->fptr, matdir.strtblk+1, z_data ,z_nblks);
	size = uncompress(z_data, z_nblks*512, data, compress_method);
	free(z_data);
	if (size == 0) return (ERROR);
	return(OK) ;
}
