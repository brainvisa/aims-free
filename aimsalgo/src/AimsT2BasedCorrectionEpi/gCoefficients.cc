/* Copyright (c) 1995-2007 CEA
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



#include "gCoefficients.h"


GCoefficients::GCoefficients( float s )
{

  a0 = 0.657f / s;
  a1 = 1.979f / s;
  c0 = -0.258f / s;
  c1 = -0.239f / s;
  b0 = 1.906f;
  b1 = 1.881f;
  w0 = 0.651f;
  w1 = 2.053f;

  float b0s = b0 / s;
  float b1s = b1 / s;
  float w0s = w0 / s;
  float w1s = w1 / s;
  float eb0s = (float)exp( (double)( -b0s ) );
  float eb1s = (float)exp( (double)( -b1s ) );
  float e2b0s = (float)exp( (double)( -2.0f * b0s ) );
  float e2b1s = (float)exp( (double)( -2.0f * b1s ) );
  float cw0s = (float)cos( (double)w0s );
  float sw0s = (float)sin( (double)w0s );
  float cw1s = (float)cos( (double)w1s );
  float sw1s = (float)sin( (double)w1s );

  _n00 = a0 + c0;
  _n11 = ( eb1s * ( c1 * sw1s - ( c0 + 2.0f * a0 ) * cw1s ) +
	   eb0s * ( a1 * sw0s - ( 2.0f * c0 + a0 ) * cw0s ) );
  _n22 = ( 2.0f * eb1s * eb0s * ( ( a0 + c0 ) * cw1s * cw0s - 
	   cw1s * a1 * sw0s - cw0s * c1 * sw1s ) + c0 * e2b0s + a0 * e2b1s );
  _n33 = ( eb1s * e2b0s * ( c1 * sw1s - cw1s * c0 ) + 
	   eb0s * e2b1s * ( a1 * sw0s - cw0s * a0 ) );

  _d11 = -2.0f * eb1s * cw1s - 2.0f * eb0s * cw0s;
  _d22 = 4.0f * cw1s * cw0s * eb0s * eb1s + e2b1s + e2b0s;
  _d33 = -2.0f * cw0s * eb0s * e2b1s - 2.0f * cw1s * eb1s * e2b0s;
  _d44 = e2b0s * e2b1s;

  _n11b = _n11 - _d11 * _n00;
  _n22b = _n22 - _d22 * _n00;
  _n33b = _n33 - _d33 * _n00;
  _n44b = -1.0f * _d44 * _n00;

}


GCoefficients::~GCoefficients()
{
}
