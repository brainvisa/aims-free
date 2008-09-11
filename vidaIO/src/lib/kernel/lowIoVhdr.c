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
#include <rpc/types.h>
#include <vidaIO/kernel/vidaio.h>
#include <rpc/rpc.h>
#include <rpc/xdr.h>


bool_t XdrHdrKey(XDR *xdrf,struct HdrKey *ptr)
{ 
  char *ptr_data_type,*ptr_name;

  ptr_data_type = ptr->data_type;
  ptr_name      = ptr->name;
  return(xdr_int   (xdrf, &(ptr->size))          &&
         xdr_string(xdrf, &ptr_data_type, 10)    &&
         xdr_string(xdrf, &ptr_name, 18)         &&
         xdr_int   (xdrf, &(ptr->extents))       &&
         xdr_short (xdrf, &(ptr->session_error)) &&
         xdr_char  (xdrf, &(ptr->regular))       &&
         xdr_char  (xdrf, &(ptr->hkey_un0))        );
}


bool_t XdrImageDimension(XDR *xdrf,struct ImageDimension *ptr)
{ 
  unsigned int seize_pt;
  char         *ptr_dim;
  char         *ptr_pixdim;
  bool_t result;

  seize_pt   = 16;
  ptr_dim    = (char *)ptr->dim;
  ptr_pixdim = (char *)ptr->pixdim;


  result = xdr_array(xdrf, &ptr_dim,&seize_pt, 16, sizeof(short int), 
		     (xdrproc_t)xdr_short) && 
    xdr_short(xdrf, &(ptr->bitpix)) &&
    xdr_short(xdrf, &(ptr->dim_un0)) &&
    xdr_array(xdrf, &ptr_pixdim,&seize_pt, 16, sizeof(float), 
	      (xdrproc_t)xdr_float) &&
    xdr_int(xdrf, &(ptr->glmax)) &&
    xdr_int(xdrf, &(ptr->glmin));

  return result;
}
  

bool_t XdrHdrHistory(XDR *xdrf,struct HdrHistory *ptr)
{
  char *ptr_descrip,
    *ptr_aux_file,
    *ptr_originator,
    *ptr_generated,
    *ptr_scannum,
    *ptr_patient_id,
    *ptr_exp_date,
    *ptr_exp_time,
    *ptr_hist_un0;
	
  ptr_descrip    = ptr->descrip;
  ptr_aux_file   = ptr->aux_file;
  ptr_originator = ptr->originator;
  ptr_generated  = ptr->generated;
  ptr_scannum    = ptr->scannum;
  ptr_patient_id = ptr->patient_id;
  ptr_exp_date   = ptr->exp_date;
  ptr_exp_time   = ptr->exp_time;
  ptr_hist_un0   = ptr->hist_un0;

  return(xdr_string(xdrf, &ptr_descrip, 80)    &&
	 xdr_string(xdrf, &ptr_aux_file, 24)   &&
	 xdr_char(xdrf, &(ptr->orient))        &&
	 xdr_string(xdrf, &ptr_originator, 10) &&
	 xdr_string(xdrf, &ptr_generated, 10)  &&
	 xdr_string(xdrf, &ptr_scannum, 10)    &&
	 xdr_string(xdrf, &ptr_patient_id, 10) &&
	 xdr_string(xdrf, &ptr_exp_date, 10)   &&
	 xdr_string(xdrf, &ptr_exp_time,10)    &&
	 xdr_string(xdrf, &ptr_hist_un0, 3)    &&
	 xdr_int(xdrf, &(ptr->views))          &&
	 xdr_int(xdrf, &(ptr->vols_added))     &&
	 xdr_int(xdrf, &(ptr->start_field))    &&
	 xdr_int(xdrf, &(ptr->field_skip))     &&
	 xdr_int(xdrf, &(ptr->omax))           &&
	 xdr_int(xdrf, &(ptr->omin))           &&
	 xdr_int(xdrf, &(ptr->smax))           &&
	 xdr_int(xdrf, &(ptr->smin))            );
}


bool_t XdrAnalyzeDb(XDR *xdrf,struct AnalyzeDb *ptr)
{ 
  return(XdrHdrKey        (xdrf, &(ptr->key))&&
         XdrImageDimension(xdrf, &(ptr->img))&&
         XdrHdrHistory    (xdrf, &(ptr->his)) );
}

/*----------------------------------------------------------------------------
int XdrWrite(char *ptr ,FILE *stream)

   Fonction d'encodage sur flot xdr.
   ENTREES:
      char * ptr buffer
      FILE stream fichier ouvert en lecture
  
   SORTIE : 
      valeur entiere rendue par la fonction 
      0 : faux
      1 : vrai

----------------------------------------------------------------------------*/

int XdrWrite(char *ptr,FILE *stream)
{ 
  XDR xdrf;

  xdrstdio_create(&xdrf,stream,XDR_ENCODE);
  if (!(XdrAnalyzeDb(&xdrf,(struct AnalyzeDb *)ptr))) return (0);
  xdr_destroy(&xdrf);
  return (1);
}

/*----------------------------------------------------------------------------
int XdrRead(char *ptr ,FILE *stream)


   Fonction decodage sur flot xdr.
   ENTREES:
      char * ptr buffer
      FILE stream fichier ouvert en lecture
  
   SORTIE : 
      valeur entiere rendue par la fonction 
      0 : faux
      1 : vrai
----------------------------------------------------------------------------*/

int XdrRead(char *ptr,FILE *stream)
{
  XDR xdrf;

  xdrstdio_create(&xdrf,stream,XDR_DECODE);
  if (!(XdrAnalyzeDb(&xdrf,(struct AnalyzeDb *)ptr))) return (0);
  xdr_destroy(&xdrf);
  return(1);
}


