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


#include <aims/signalfilter/grecurse.h>


void RecursiveGaussian::initialize( const GCoef& coef )
{
  // Lourd, mais evite des appels lors de l'execution de la boucle
  n00 = coef.n00();
  n11 = coef.n11();
  n22 = coef.n22();
  n33 = coef.n33();

  n11b = coef.n11b();
  n22b = coef.n22b();
  n33b = coef.n33b();
  n44b = coef.n44b();

  d11 = coef.d11();
  d22 = coef.d22();
  d33 = coef.d33();
  d44 = coef.d44();
}


void RecursiveGaussian::recurse( float *din, float *dout, float *dw, int dim )
{
  int n = dim - 4;

  // Calcul des yk+
  float *optr0 = dout;
  float *optr1 = dout + 1;
  float *optr2 = dout + 2;
  float *optr3 = dout + 3;
  float *optr4 = dout + 4;

  float *iptr0 = din;
  float *iptr1 = din + 1;
  float *iptr2 = din + 2;
  float *iptr3 = din + 3;

  *optr0 = n00 * *iptr0;
  *optr1 = n00 * *iptr1 + n11 * *iptr0 - d11 * *optr0;
  *optr2 = n00 * *iptr2 + n11 * *iptr1 + n22 * *iptr0 - 
           d11 * *optr1 - d22 * *optr0;
  *optr3 = n00 * *iptr3++ + n11 * *iptr2++ + n22 * *iptr1++ + n33 * *iptr0++ -
           d11 * *optr2 - d22 * *optr1 - d33 * *optr0;

  for ( --n; n>=0; --n )
    *optr4++ = n00 * *iptr3++ + n11 * *iptr2++ + n22 * *iptr1++ +
               n33 * *iptr0++ - d11 * *optr3++ - d22 * *optr2++ - 
               d33 * *optr1++ - d44 * *optr0++;

  // Calcul des yk-
  optr0 = dw + dim - 1;
  optr1 = optr0 - 1;
  optr2 = optr0 - 2;
  optr3 = optr0 - 3;
  optr4 = optr0 - 4;

  iptr0 = din + dim - 1;
  iptr1 = iptr0 - 1;
  iptr2 = iptr0 - 2;
  iptr3 = iptr0 - 3;

  *optr0 = 0.0f;
  *optr1 = n11b * *iptr0;
  *optr2 = n11b * *iptr1 + n22b * *iptr0 - d11 * *optr1 - d22 * *optr0;
  *optr3 = n11b * *iptr2 + n22b * *iptr1 + n33b * *iptr0 - d11 * *optr2 -
           d22 * *optr1 - d33 * *optr0;

  for ( n=dim-5; n>=0; --n )
    *optr4-- = n11b * *iptr3-- + n22b * *iptr2-- + n33b * *iptr1-- +
               n44b * *iptr0-- - d11 * *optr3-- - d22 * *optr2-- - 
               d33 * *optr1-- - d44 * *optr0--;

  // Calcul des yk = yk+ + yk-
  optr0 = dout;
  iptr0 = dw;

  for ( n=dim-1; n>=0; optr0++, --n )
    *optr0 += *iptr0++;
}


