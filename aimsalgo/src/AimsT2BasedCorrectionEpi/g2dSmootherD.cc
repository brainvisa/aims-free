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


#include "g2dSmootherD.h"
#include <iostream>
#include <math.h>
#include <string.h>

using namespace std;

DiscreteGaussian2dSmoother::DiscreteGaussian2dSmoother( int sizeMask,
                                                        int levels )
                           : Smoother( levels ),
                             _coefCount( ( sizeMask + 1 ) / 2 )
{

  _levels2 = _levels * _levels;
  _coef = new float[ _coefCount ];
  _tmp = new float[ _levels2 ];

  // never use _coef[ 0 ] = 1.0 !
  for ( int m = 1; m < _coefCount; m++ )
    _coef[ m ] = exp( - 0.5 * m * m );

}


DiscreteGaussian2dSmoother::~DiscreteGaussian2dSmoother()
{

  delete [] _tmp;
  delete [] _coef;

}


void DiscreteGaussian2dSmoother::doIt( float* p12linear )
{

  register float* in = p12linear;
  register float* out = _tmp;

  register float* pIn = 0;
  register float* pOut = 0;

  register int finalIndex = 0;
  register int l = 0;
  register int m = 0;
  register float coef;

  // smoothing of lines
  for ( int line = 0; line < _levels; line++ )
  {

    pIn = in;
    pOut = out;
    for ( l = 0; l < _levels; l++ )
      *pOut++ = *pIn++;


    for ( m = 1; m < _coefCount; m++ )
    {

      coef = _coef[ m ];
      pIn = in + m;
      pOut = out;
      finalIndex = _levels - m;
      for ( l = 0; l < finalIndex; l++ )
        *pOut++ += coef * *pIn++;

      pIn = in;
      pOut = out + m;
      for ( l = m; l < _levels; l++ )
        *pOut++ += coef * *pIn++;

    }

    in += _levels;
    out += _levels;


  }

  in = _tmp;
  out = p12linear;

  // smoothing of columns
  for ( int column = 0; column < _levels; column++ )
  {

    pIn = in;
    pOut = out;
    for ( l = 0; l < _levels; l++, pOut += _levels, pIn += _levels )
      *pOut = *pIn;

    int offset = _levels;
    for ( m = 1; m < _coefCount; m++ )
    {

      coef = _coef[ m ];
      pIn = in + offset;
      pOut = out;
      finalIndex = _levels - m;
      for ( l = 0; l < finalIndex; l++, pOut += _levels, pIn += _levels )
        *pOut += coef * *pIn;

      pIn = in;
      pOut = out + offset;
      for ( l = m; l < _levels; l++, pOut += _levels, pIn += _levels )
        *pOut += coef * *pIn;

      offset += _levels;

    }

    in ++;
    out ++;

  }

}
