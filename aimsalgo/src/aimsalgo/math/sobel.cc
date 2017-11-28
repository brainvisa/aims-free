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
#include <aims/math/sobel.h>

/**
* Sobel gradient 2D specialization
*/
template <class T>
SobelGradient<T, 2>::SobelGradient(const AimsData<T> & image,
                                   const T levels)
  : _levels(levels),
    _image(image) {
  // gradient values: Gx,Gy
  _gradX = AimsData<double>(_image.dimX(), 
                            _image.dimY(), 
                            _image.dimZ(), 
                            _image.dimT());
  _gradY = AimsData<double>(_image.dimX(), 
                            _image.dimY(), 
                            _image.dimZ(), 
                            _image.dimT());
}

template <class T>
Point3df SobelGradient<T, 2>::compute(const int x, 
                                      const int y, 
                                      const int z, 
                                      const int t) {
  int v00 = 0, v01 = 0, v02 = 0, 
      v10 = 0,          v12 = 0, 
      v20 = 0, v21 = 0, v22 = 0;

  int x0 = x - 1, x1 = x, x2 = x + 1;
  int y0 = y - 1, y1 = y, y2 = y + 1;
  if (x0 < 0) x0 = 0;
  if (y0 < 0) y0 = 0;
  if (x2 >= _image.dimX()) x2 = _image.dimX() - 1;
  if (y2 >= _image.dimY()) y2 = _image.dimY() - 1;

  v00 = _image(x0, y0, z, t); v10 = _image(x1, y0, z, t); v20 = _image(x2, y0, z, t);
  v01 = _image(x0, y1, z, t);                             v21 = _image(x2, y1, z, t);
  v02 = _image(x0, y2, z, t); v12 = _image(x1, y2, z, t); v22 = _image(x2, y2, z, t);

  float sx = (float)((v20 + 2 * v21 + v22) - (v00 + 2 * v01 + v02)) / (4 * (double)_levels);
  float sy = (float)((v02 + 2 * v12 + v22) - (v00 + 2 * v10 + v20)) / (4 * (double)_levels);

  return Point3df(sx, sy, 0);
}

template <class T>
void SobelGradient<T, 2>::compute(){
  Point3df s;
  for (int t = 0; t < _image.dimT(); t++)
    for (int z = 0; z < _image.dimZ(); z++)
      for (int y = 0; y < _image.dimY(); y++)
        for (int x = 0; x < _image.dimX(); x++){
          s = compute(x, y, z, t);
          _gradX(x, y, z, t) = (double)(s[0]);
          _gradY(x, y, z, t) = (double)(s[1]);
        }
}

template <class T>
AimsData<double> & SobelGradient<T, 2>::gradX(){
  return _gradX;
}

template <class T>
AimsData<double> & SobelGradient<T, 2>::gradY(){
  return _gradY;
}

template class SobelGradient<uint8_t, 2>;
template class SobelGradient<int8_t, 2>;
template class SobelGradient<uint16_t, 2>;
template class SobelGradient<int16_t, 2>;
template class SobelGradient<uint32_t, 2>;
template class SobelGradient<int32_t, 2>;
