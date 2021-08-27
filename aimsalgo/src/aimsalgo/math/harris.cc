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

// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <math.h>
#include <cartobase/config/verbose.h>
#include <aims/math/harris.h>
#include <aims/math/sobel.h>

using namespace aims;
using namespace carto;


/**
*  Constructor
*/
template <class T>
BaseHarrisDetector<T>::BaseHarrisDetector(const rc_ptr<Volume<T> > & image,
                                          const double sigma, 
                                          const double k, 
                                          const double epsilon,
                                          const T levels)
  : _epsilon(epsilon),
    _k(k),
    _sigma(sigma),
    _levels(levels),
    _image(image)
{
                      
//     if (! levels){
//       _levels = DataTypeInfo<T>::depth() * 8 - 1;
//     }

  // Initializes parameters
  _sigma2 = _sigma * _sigma;
  _u = 1.0 / (2 * M_PI * _sigma2);
  _f = ((double)_levels) / log(1 + (double)_levels);
  _radius = (int)(2 * _sigma);
  _window = 1 + 2 * _radius;
  
  this->_hmap = VolumeRef<double>( _image->getSizeX(),
                                   _image->getSizeY(),
                                   _image->getSizeZ(),
                                   _image->getSizeT() );
  this->_hmap.setVoxelSize( _image->getVoxelSize() );
  this->_hmap = 0;
}

 
/**
* Gaussian function
*/
template <class T>
double BaseHarrisDetector<T>::gaussian(const double x, 
                                       const double y, 
                                       const double z)
{
  return (_u * exp( -(x * x + y * y + z * z) / (2 * _sigma2) ) );
}

/**
* Compute the Harris measure for each pixel of the image
*/
template <class T>
void BaseHarrisDetector<T>::computeHarrisMap()
{
  
  // precompute derivatives
  this->computeDerivatives();

  // for each pixel in the _image
  for (int t = 0; t < _image->getSizeT(); t++)
    for (int z = 0; z < _image->getSizeZ(); z++)
      for (int y = 0; y < _image->getSizeY(); y++)
        for (int x = 0; x < _image->getSizeX(); x++)
        {
          // compute the harris measure
          double h = this->harrisMeasure(x, y, z, t);
          //std::cout << "h : " << carto::toString(h) << std::endl << std::flush;
          if (h <= 0) continue;
          // log scale
          h = _f * log(1 + h);
          // store
          this->_hmap(x, y, z, t) = h;
        }
}

/**
* Perfom the Harris Corner Detection
*/
template <class T>
void BaseHarrisDetector<T>::computeCorners()
{
  
  // Initializes corners
  _corners = aims::BucketMap<double>();
  _corners.setSizeXYZT( _image->getVoxelSize() );
                      
  // Harris measure map
  this->computeHarrisMap();

  // For each pixel in the harrismap, check for spatial maxima
  int cfound = 0;
  for (int t = 0; t < _image->getSizeT(); t++)
    for (int z = 0; z < _image->getSizeZ(); z++)
      for (int y = 1; y < _image->getSizeY() - 1; y++)
        for (int x = 1; x < _image->getSizeX() - 1; x++)
        {
          // thresholding : harris measure > epsilon
          double h = this->_hmap(x, y, z, t);
          if (h <= this->_epsilon) continue;
          // keep only a local maxima
          if (!this->isSpatialMaxima(x, y, z, t)) continue;
          // add the corner to the list
          const Point3d & p = Point3d((int)x, (int)y, z);
          this->_corners[t].insert(std::pair<Point3d, double>(p, h));
          cfound++;
        }

  if (carto::verbose)
    std::cout << carto::toString(cfound) 
              << " potential corners found." 
              << std::endl << std::flush;

}

/**
* Filter detected corners
* 
* @param minDistance minimum distance between corners
*/
template <class T>
void BaseHarrisDetector<T>::filter(int minDistance)
{
  this->computeCorners();
  
  // Remove corners too close to each other (keeping the highest measure)
  int sz = this->_corners.size();
  int s, c, r;
  int dist;
  Point3d n, p;
  for (int t = 0; t < sz; t++)
  {
    aims::BucketMap<double>::Bucket::iterator it = this->_corners[t].begin(), 
                                              ir, it2, ie = this->_corners[t].end();
    s = this->_corners[t].size();
    c = 0;
    r = 0;
    while(it != ie) {
      c++;
      ir = it;
      // It is important to increment iterator here
      // because iterator could be invalidated if element is removed
      it++;
      
      for(it2 = this->_corners[t].begin(); it2 != ie; ++it2) {
        if (ir->first != it2->first) {
          dist = (int)(ir->first - it2->first).norm();
          if (dist <= minDistance)
            if (ir->second <= it2->second) {
              this->_corners[t].erase(ir);
              r++;
              break;
            }
        }
      }
    }
    
    if (carto::verbose)
      std::cout << "Frame " << carto::toString(t) 
                << ", checked : " << carto::toString(c) 
                << " / " << carto::toString(s)
                << ", filtered : " << carto::toString(r)
                << ", remains : " << carto::toString(this->_corners[t].size())
                << std::endl << std::flush;
  }
  
}

template <class T>
VolumeRef<double> BaseHarrisDetector<T>::harrisMap()
{
  return this->_hmap;
}

template <class T>
aims::BucketMap<Void> BaseHarrisDetector<T>::corners()
{
  int sz = _corners.size();
  aims::BucketMap<Void> corners;
  corners.setVoxelSize( _corners.getVoxelSize() );
  for (int t = 0; t < sz; t++)
  {
    aims::BucketMap<double>::Bucket::iterator it, ie = this->_corners[t].end();
    for(it = this->_corners[t].begin(); it != ie; it++)
    {
      corners[t].insert(std::pair<Point3d, Void>(it->first, Void()));
    }
  }
  
  return corners;
}


 
/**
*  Constructor
*/
template <class T>
HarrisDetector<T, 2>::HarrisDetector(const rc_ptr<Volume<T> > & image,
                                     double sigma, 
                                     double k, 
                                     double epsilon,
                                     T levels)
  : BaseHarrisDetector<T>(
      image, 
      sigma, 
      k, 
      epsilon,
      levels
    )
{
  // Initializes derivative images
  this->_lx2 = VolumeRef<double>(this->_image->getSizeX(),
                                this->_image->getSizeY(),
                                this->_image->getSizeZ(),
                                this->_image->getSizeT());
  this->_ly2 = VolumeRef<double>(this->_image->getSizeX(),
                                this->_image->getSizeY(),
                                this->_image->getSizeZ(),
                                this->_image->getSizeT());
  this->_lxy = VolumeRef<double>(this->_image->getSizeX(),
                                this->_image->getSizeY(),
                                this->_image->getSizeZ(),
                                this->_image->getSizeT());

  
  // Precompute the coefficients of the gaussian filter
  this->_g = VolumeRef<double>(this->_window, this->_window);
  for(int j = (-this->_radius); j <= this->_radius; j++)
    for(int i = (-this->_radius); i <= this->_radius; i++)
      this->_g(i + this->_radius, j + this->_radius) = this->gaussian(i, j, 0);
    
}

/**
* Compute the 3 arrays Ix, Iy and Ixy
*/
template <class T>
void HarrisDetector<T, 2>::computeDerivatives()
{
  SobelGradient<T, 2> s(this->_image, 
                        this->_levels);
  s.compute();
  
  _lx2 = 0.; _ly2 = 0.; _lxy = 0.;

//     gW.setFileName("gradX.ima");
//     gW << gradX;
//     
//     gW.setFileName("gradY.ima");
//     gW << gradY;

  // Convolve gradient with gaussian filter:
  //
  // Ix2 = (F) * (Gx^2)
  // Iy2 = (F) * (Gy^2)
  // Ixy = (F) * (Gx.Gy)
  //
  int xk, yk;
  double gw, gx, gy;
  for (int t = 0; t < this->_image->getSizeT(); t++)
    for (int z = 0; z < this->_image->getSizeZ(); z++)
      for (int y = 0; y < this->_image->getSizeY(); y++)
        for (int x = 0; x < this->_image->getSizeX(); x++)
          for (int dy = (-this->_radius); dy <= this->_radius; dy++)
            for (int dx = (-this->_radius); dx <= this->_radius; dx++)
            {
              xk = x + dx;
              yk = y + dy;
              if (xk < 0 || xk >= this->_image->getSizeX()) continue;
              if (yk < 0 || yk >= this->_image->getSizeY()) continue;
  
              // gaussian weight
              gw = this->_g(dx + this->_radius, dy + this->_radius);
  
              // convolution
              gx = s.gradX()(xk, yk, z, t);
              gy = s.gradY()(xk, yk, z, t);
              
              _lx2(x, y, z, t) += gw * gx * gx;
              _ly2(x, y, z, t) += gw * gy * gy;
              _lxy(x, y, z, t) += gw * gx * gy;
            }
/*    
  gW.setFileName("Lx2.ima");
  gW << _lx2;
  
  gW.setFileName("Ly2.ima");
  gW << _ly2;
  
  gW.setFileName("Lxy.ima");
  gW << _lxy;*/
}

/**
* Compute harris measure for a pixel
*/
template <class T>
double HarrisDetector<T, 2>::harrisMeasure(const int x, 
                                           const int y, 
                                           const int z, 
                                           const int t)
{
  // matrix elements (normalized)
  double m00 = _lx2(x, y, z, t); 
  double m01 = _lxy(x, y, z, t);
  double m10 = _lxy(x, y, z, t);
  double m11 = _ly2(x, y, z, t);

//     if ((m00 != m01)
//         || (m00 != m11))
//       std::cout << "m00 : " << carto::toString(m00) 
//                 << ", m01 : " << carto::toString(m01) 
//                 << ", m10 : " << carto::toString(m10) 
//                 << ", m11 : " << carto::toString(m11) 
//                 << ", r : " << carto::toString(m00 * m11 - m01 * m10 - k * (m00 + m11) * (m00 + m11)) 
//                 << std::endl << std::flush;
              
  // Harris corner measure = det(M) - k.trace(M) ^ 2
  return m00 * m11 - m01 * m10 - this->_k * (m00 + m11) * (m00 + m11);
}

/**
* Return true if the measure at pixel (x,y) is a local spatial Maxima
*/
template <class T>
bool HarrisDetector<T, 2>::isSpatialMaxima(const int x, 
                                           const int y, 
                                           const int z, 
                                           const int t)
{
  int n = 8;
  int dx[] = {-1, 0, 1, 1, 1, 0, -1, -1};
  int dy[] = {-1, -1, -1, 0, 1, 1, 1, 0};
  double w = this->_hmap(x, y, z, t);
  for(int i = 0; i < n; i++) {
    if (this->_hmap(x + dx[i], y + dy[i], z, t) >= w)
      return false;
  }
  return true;
}

template class BaseHarrisDetector<uint8_t>;
template class BaseHarrisDetector<int8_t>;
template class BaseHarrisDetector<uint16_t>;
template class BaseHarrisDetector<int16_t>;
template class BaseHarrisDetector<uint32_t>;
template class BaseHarrisDetector<int32_t>;

template class HarrisDetector<uint8_t, 2>;
template class HarrisDetector<int8_t, 2>;
template class HarrisDetector<uint16_t, 2>;
template class HarrisDetector<int16_t, 2>;
template class HarrisDetector<uint32_t, 2>;
template class HarrisDetector<int32_t, 2>;
