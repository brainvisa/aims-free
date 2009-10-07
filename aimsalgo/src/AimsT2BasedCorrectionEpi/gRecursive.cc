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



#include "gRecursive.h"


RecursiveGaussian::RecursiveGaussian( const GCoefficients& coef, int levels )
                  : _c( coef ),
                    _levels( levels )
{
}


RecursiveGaussian::~RecursiveGaussian()
{
}


void RecursiveGaussian::recurse( float* in, float* out, float* w )
{

  int n = _levels - 4;

  // Calcul des yk+
  float *optr0 = out;
  float *optr1 = out + 1;
  float *optr2 = out + 2;
  float *optr3 = out + 3;
  float *optr4 = out + 4;

  float *iptr0 = in;
  float *iptr1 = in + 1;
  float *iptr2 = in + 2;
  float *iptr3 = in + 3;

  *optr0 = _c.n00() * *iptr0;
  *optr1 = _c.n00() * *iptr1 + _c.n11() * *iptr0 - _c.d11() * *optr0;
  *optr2 = _c.n00() * *iptr2 + _c.n11() * *iptr1 + _c.n22() * *iptr0 - 
           _c.d11() * *optr1 - _c.d22() * *optr0;
  *optr3 = _c.n00() * *iptr3++ + _c.n11() * *iptr2++ + _c.n22() * *iptr1++ +
           _c.n33() * *iptr0++ -
           _c.d11() * *optr2 - _c.d22() * *optr1 - _c.d33() * *optr0;

  for ( ; n--; )
    *optr4++ = _c.n00() * *iptr3++ + _c.n11() * *iptr2++ + _c.n22() * *iptr1++ +
               _c.n33() * *iptr0++ - _c.d11() * *optr3++ - _c.d22() * *optr2++ - 
               _c.d33() * *optr1++ - _c.d44() * *optr0++;

  // Calcul des yk-
  optr0 = w + _levels - 1;
  optr1 = optr0 - 1;
  optr2 = optr0 - 2;
  optr3 = optr0 - 3;
  optr4 = optr0 - 4;

  iptr0 = in + _levels - 1;
  iptr1 = iptr0 - 1;
  iptr2 = iptr0 - 2;
  iptr3 = iptr0 - 3;

  *optr0 = 0.0f;
  *optr1 = _c.n11b() * *iptr0;
  *optr2 = _c.n11b() * *iptr1 + _c.n22b() * *iptr0 - 
           _c.d11() * *optr1 - _c.d22() * *optr0;
  *optr3 = _c.n11b() * *iptr2 + _c.n22b() * *iptr1 + _c.n33b() * *iptr0 - 
           _c.d11() * *optr2 - _c.d22() * *optr1 - _c.d33() * *optr0;

  for ( n = _levels - 4; n--; )
    *optr4-- = _c.n11b() * *iptr3-- + _c.n22b() * *iptr2-- +
               _c.n33b() * *iptr1-- +
               _c.n44b() * *iptr0-- - _c.d11() * *optr3-- - 
               _c.d22() * *optr2-- - 
               _c.d33() * *optr1-- - _c.d44() * *optr0--;

  // Calcul des yk = yk+ + yk-
  optr0 = out;
  iptr0 = w;

  for ( n = _levels; n--; optr0++ )
    *optr0 += *iptr0++;
/*
  for ( int i = 0; i < _levels; i++ )
    out[ i ] = 2 * in[ i ];
*/
}
