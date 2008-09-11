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
#ifndef ECATSHFJ_RWTB_HH
#define ECATSHFJ_RWTB_HH


#include <ecat+/io/general.h>


#ifdef __cplusplus
  extern "C" {
#endif

#define MAXFNAMESIZE 200
 
int  Write_Scan3D_Ecat7_S16(MatrixFile *, Scan3D_subheader *, short *, int, int);
int  Write_Full_Scan3D_Ecat7_S16(MatrixFile *, Scan3D_subheader *, short *, int);
int  Write_Scan3D_Ecat7_FLOAT(MatrixFile *, Scan3D_subheader *, float *, int, int);
int  Write_Full_Scan3D_Ecat7_FLOAT(MatrixFile *, Scan3D_subheader *, float *, int);
int  Write_Atten_Ecat7(MatrixFile *, Attn_subheader *, float *, int, int);
int  Write_Full_Atten_Ecat7(MatrixFile *, Attn_subheader *, float *, int);
int  Write_Image_Ecat7(MatrixFile *, Image_subheader *, float *, int);

int  Read_Scan3D_Ecat7(MatrixFile *, Scan3D_subheader *, short *, int,int);
int  Read_Scan3D_Ecat7_FLOAT(MatrixFile *, Scan3D_subheader *, float *, int,int);
int  Read_Atten_Ecat7(MatrixFile *, Attn_subheader *, float *, int, int);
int  Read_Image_Ecat7(MatrixFile *, Image_subheader *, float *, int);
int  ecat_version(char *);
int  mult(int,int,int,int,int);
int  Bin_Scan3D(int,int,int,int,int,int,int,int,int *,int *,int *,int *);
int  Sino2Det(int,int,char *,int *,int *);
int  Det2Sino(int,int,char *,int *,int *);

#ifdef __cplusplus
  }
#endif

#endif
