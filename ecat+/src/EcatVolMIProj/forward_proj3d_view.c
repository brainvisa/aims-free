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

#include <math.h>

/*                      forward_proj3d_view(), written by Xuan for ecat_osem3d
 *      purpose: forward projection, i.e., estimating parallel projections from a 3D image.  
 *               for one (theta, view)  
 */

int  forward_proj3d_view(float *image, float *projection2d, float cosfi, float sinfi, float theta,
			 float x_size, float y_size, float z_size, int x_pixels, int y_pixels,int z_pixels,
			 float x_offset, float y_offset, float z_offset,
			 float xr_size, float yr_size, int xr_pixels, int yr_pixels)
{

  int     x,y,z;                  /* integer voxel indices        */
  int     xr, yr;                 /* integer indices for 2D-projections  */
  float   incx, incy, incz;       /* floating steps when moving to next xr elements; in pixel */
  float   xx,yy,zz;               /* float voxel position, in the unit of pixel  */
  float   xx0, yy0, zz0,
	  xx00,yy00,zz00;         /* float voxel position in pixel corresponding to the first
				     one in a sinogram, i.e. xr=0  */
  float   delxx0, delyy0, delzz0;
				  /* floating steps (in pixel) when moving one pixel in x (or) y */
  float   rescale;                /* Jacobian for usinf x or y as integration var. , lenrth element */
  float   tantheta,cos_theta;
  float   x1,x2,z1,z2;            /* interpolation parameters */
  int     bin_img,bin_proj;
  float   xr_off,x_off,y_off,z_off,scale_z,value;
 
  /*{{{  initializations*/
  delxx0 = delyy0 = xx00 = yy00 = 0;
  cos_theta = cos(theta);
  tantheta = tan(theta);
  xr_off = ((float)xr_pixels - 1.)/2.;
  x_off  = ((float)x_pixels  - 1.)/2. - x_offset/x_size;
  y_off  = ((float)y_pixels  - 1.)/2. - y_offset/y_size;
  z_off  = ((float)z_pixels  - 1.)/2. - z_offset/z_size;
  if (cosfi == 0.0)
    incx = 9999;
  else
    incx =  xr_size/(x_size*cosfi);
  if (sinfi == 0.0)
    incy = 9999.0;
  else
    incy =  xr_size/(y_size*sinfi);
  
  /*
    y = cos(psi)*xr -sin(psi)*sin(theta)*yr + sin(psi)*cos(theta)*zr; delta_zr~delta_y
  */
  
  scale_z = yr_size/z_size;
  if (fabs(incy) > fabs(incx)) {
    rescale = y_size / fabs (cosfi*cos_theta);
    delxx0 = -(y_size*sinfi)/(x_size*cosfi);
    xx00 = x_off - xr_off*incx - y_off*delxx0;
    incz = -tantheta*xr_size*sinfi/(cosfi*z_size);
    delzz0 = (y_size*tantheta)/(z_size*cosfi);
    zz00 = -((float)yr_pixels -1.)/2.*scale_z + z_off - xr_off*incz - y_off*delzz0;
  } else {
    rescale = fabs(x_size/(sinfi*cos_theta));
    delyy0 = - (x_size*cosfi) / (y_size*sinfi);
    yy00 = y_off -xr_off*incy - x_off*delyy0;
    incz = tantheta*xr_size*cosfi/(sinfi*z_size);
    delzz0 = -(x_size*tantheta)/(z_size*sinfi);
    zz00 = -((float)yr_pixels -1.)/2.*scale_z + z_off  - xr_off*incz - x_off*delzz0;
  }
  
  for (yr=0; yr<yr_pixels; yr++) {
    for (xr=0; xr<xr_pixels; xr++) {
      bin_proj = xr + yr*xr_pixels;
      projection2d[bin_proj] = 0.0;
    }
  }
  /*}}}  */
  for (yr=0; yr<yr_pixels; yr++) {
    /* determine whether line direction is closer to x or y axis */
    /*{{{  Take y as int. var. since it changes faster*/
    if (fabs(incy) > fabs(incx)) {        /* Take y as int. var. since it changes faster */
      xx0 = xx00;
      zz0 = yr*scale_z + zz00;
      for (y=0; y<y_pixels; y++) {
	xx=xx0;
	zz=zz0;
	for (xr=0; xr<xr_pixels; xr++) {
	  bin_proj = xr + yr*xr_pixels;
	  value = 0.0;
	  if (xx<0)
	    x = (int)(xx-1.0);
	  else
	    x = (int)xx;
	  if (zz<0)
	    z = (int)(zz-1.0);
	  else
	    z = (int)zz;
	  bin_img = x + y*x_pixels + z*x_pixels*y_pixels;
	  if ((x>=0) && (x<(x_pixels-1))) { /* x interpolation possible */
	    x1=xx-x;
	    x2=1.0-x1;
	    z1=zz-z;
	    z2=1.0-z1;
	    if ((z>=0) && (z<z_pixels)) /* interpolation along x */
	      value += z2*( x1*image[bin_img+1] + x2*image[bin_img] );
	    if ((z>=-1) && (z<(z_pixels-1))) /* interpolation along z if possible */
	      value += z1*( x1*image[bin_img+x_pixels*y_pixels+1] + x2*image[bin_img+x_pixels*y_pixels] );
	  } else if (x == (x_pixels-1)) { /* x interpolation impossible */
	    if ((z>=0) && (z<z_pixels))
	      value += (1.0-zz+z)* (1-xx+x) * image[bin_img] ;
	    if ((z>=-1) && (z<(z_pixels-1) ) ) /* interpolation along z if possible */
	      value += (zz-z)*(1-xx+x)*image[bin_img+x_pixels*y_pixels]  ;
	  } else if (x == -1) {       /* x interpolation impossible */
	    if ((z>=0) && (z<z_pixels))
	      value += (1.0-zz+z) * (xx-x) * image[bin_img+1] ;
	    if ((z>=-1) && (z<(z_pixels-1))) /* interpolation along z if possible */
	      value += (zz-z)* (xx-x)*image[bin_img+x_pixels*y_pixels+1] ;
	  }
	  if (value > projection2d[bin_proj]) projection2d[bin_proj] = value;
	  xx += incx;
	  zz += incz;
	}
	xx0 += delxx0;
	zz0 += delzz0;
      }
    /*}}}  */
    /*{{{  Take x as int. var. since it changes faster*/
    } else { /* Take x as int. var. since it changes faster */
      yy0 = yy00;
      zz0 = yr*scale_z + zz00;
      for (x=0; x<x_pixels; x++) {
	yy=yy0;
	zz=zz0;
	for (xr=0; xr<xr_pixels; xr++) {
	  bin_proj = xr + yr*xr_pixels;
	  value = 0.0;
	  if (yy<0)
	    y = (int)(yy-1.0);
	  else
	    y = (int)yy;
	  if (zz<0)
	    z = (int)(zz-1.0);
	  else
	    z = (int)zz;
	  bin_img = x + y*x_pixels + z*x_pixels*y_pixels;
	  if ((y>=0) && (y<(y_pixels-1))) { /* y interpolation possible */
	    x1 = yy - y;
	    x2 = 1.0 - x1;
	    z1 = zz - z;
	    z2 = 1.0 - z1;
	    if ((z >= 0) && (z < z_pixels)) /* interpolation along y */
	      value += z2*( x1*image[bin_img+x_pixels] + x2*image[bin_img] );
	    if ((z >= -1) && (z < (z_pixels-1))) /* interpolation along z if possible */
	      value += z1*( x1*image[bin_img+x_pixels*y_pixels+x_pixels] + x2*image[bin_img+x_pixels*y_pixels] );
	  } else if (y==(y_pixels-1)) {  /* y interpolation impossible */
	    if ((z>=0) && (z<z_pixels))   /* interpolation along y */
	      value += (1.0-zz+z) * (1-yy+y)*image[bin_img] ;
	    if ((z>=-1) && (z<(z_pixels-1))) /* interpolation along z if possible */
	      value += (zz-z)* (1-yy+y)*image[bin_img+x_pixels*y_pixels] ;
	  } else if (y == -1) { /* y interpolation impossible */
	    if ((z>=0) && (z<z_pixels))   /* interpolation along y */
	      value += (1.0-zz+z) * (yy-y)*image[bin_img+x_pixels] ;
	    if ((z>=-1) && (z<(z_pixels-1))) /* interpolation along z if possible */
	      value += (zz-z) * (yy-y)*image[bin_img+x_pixels*y_pixels+x_pixels];
	  }
	  if (value > projection2d[bin_proj]) projection2d[bin_proj] = value;
	  yy += incy;
	  zz += incz;
	}
	yy0 += delyy0;
	zz0 += delzz0;
      }
    }
    /*}}}  */
    /*
      apply Jacobian for replacing linear integral by x or y integral

      NO for MIP

      for (xr=0;xr<xr_pixels;++xr) projection2d[xr + yr*xr_pixels] *= rescale;
    */
  }
  return  1;
}
