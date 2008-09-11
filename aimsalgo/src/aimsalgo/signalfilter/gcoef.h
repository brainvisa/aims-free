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


#ifndef AIMS_SIGNALFILTER_GCOEF_H
#define AIMS_SIGNALFILTER_GCOEF_H

#include <math.h>


class GCoef
{

public:

  enum GOrder
  {
    smoothing,
    gradient,
    laplacian
  };

  GCoef( float s = 1.0f, GOrder o = smoothing ) { initialize( s, o ); }
  virtual ~GCoef() { }

  void initialize( float, GOrder );

  float n00() const  { return _n00; }
  float n11() const { return _n11; }
  float n22() const { return _n22; }
  float n33() const { return _n33; }

  float n11b() const { return _n11b; }
  float n22b() const { return _n22b; }
  float n33b() const { return _n33b; }
  float n44b() const { return _n44b; }

  float d11() const { return _d11; }
  float d22() const { return _d22; }
  float d33() const { return _d33; }
  float d44() const { return _d44; }

private:

  float a0;
  float a1;
  float c0;
  float c1;
  float b0;
  float b1;
  float w0;
  float w1;

  float _n00;
  float _n11;
  float _n22;
  float _n33;

  float _n11b;
  float _n22b;
  float _n33b;
  float _n44b;

  float _d11;
  float _d22;
  float _d33;
  float _d44;

  void fillSmoothing( float );
  void fillGradient( float );
  void fillLaplacian( float );
};


#endif
