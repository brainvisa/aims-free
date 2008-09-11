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


#include <aims/transform/hilbert1d.h>
#include <aims/data/data.h>
#include <aims/math/mathelem.h>
#include <aims/transform/fft1d.h>
#include <math.h>
#include <complex>

using namespace std;

AimsData<float>
AimsHilbert1D(const AimsData<float> &vec)
{ AimsData<float> *vec1;
  int                x;
  int                newdim;

  newdim = (vec.dimX()==AimsNextPowerOfTwo(vec.dimX())/2) ?
            vec.dimX() : AimsNextPowerOfTwo(vec.dimX());

  vec1 = new AimsData<float>(newdim);
  AimsData< cfloat > cvec(newdim);
  for (x=0;x<cvec.dimX();x++)
     cvec(x) = (*vec1)(x);
  delete(vec1);

  cvec = AimsFFT1D(cvec,1);

  for (x=0;x<=cvec.dimX()/2;x++)
    cvec(x) *= (float)2.0;

  for (x=cvec.dimX()/2+1;x<cvec.dimX();x++)
    cvec(x) = 0.0;

  cvec = AimsFFT1D(cvec,-1);

  AimsData<float> vec2(vec.dimX());
  for (x=0;x<cvec.dimX();x++)
     vec2(x) = real(cvec(x));
  
  return(vec2);
}
