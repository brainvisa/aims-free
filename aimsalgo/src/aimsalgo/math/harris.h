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


#ifndef AIMS_MATH_HARRIS_H
#define AIMS_MATH_HARRIS_H

#include <iostream>
#include <aims/bucket/bucketMap.h>
#include <cartodata/volume/volume.h>

namespace aims
{

  /**
  * Harris Corner Detector
  *
  *  k = det(A) - k * trace(A)^2
  *
  *  Where A is the second-moment matrix
  *
  *            | Lx²(x+dx,y+dy)    Lx.Ly(x+dx,y+dy) |
  *  A =  Sum  |                                    | * Gaussian(dx, dy)
  *      dx,dy | Lx.Ly(x+dx,y+dy)  Ly²(x+dx,y+dy)   |
  *
  *  and k = a / (1 + a)^2,
  *
  *  where "a" is the mininmum ratio between the two eigenvalues
  *  for a point to be considered as a corner.
  */

  template <class T>
  class BaseHarrisDetector
  {
    public :

      /**
      *  Constructor
      */
      BaseHarrisDetector( const carto::rc_ptr<carto::Volume<T> > & image,
                          const double sigma = 1.2,
                          const double k = 0.6,
                          const double epsilon = 1E-3,
                          const T levels = (T)255 );

      /**
      * Gaussian function
      */
      virtual double gaussian(const double x,
                              const double y,
                              const double z);

      virtual void computeDerivatives() {}

      /**
      * Compute harris measure for a pixel
      */
      virtual double harrisMeasure(const int x,
                                  const int y,
                                  const int z,
                                  const int t) = 0;

      /**
      * Compute the Harris measure for each pixel of the image
      */
      virtual void computeHarrisMap();

      /**
      * Check that pixel is a maxima of the Harris map
      */
      virtual bool isSpatialMaxima(const int x,
                                  const int y,
                                  const int z,
                                  const int t) = 0;


      /**
      * Perfom the Harris Corner Detection
      */
      virtual void computeCorners();

      /**
      * Filter detected corners
      *
      * @param minDistance minimum distance between corners
      */
      virtual void filter(int minDistance);

      /**
      * Get processed Harris map
      */
      carto::VolumeRef<double> harrisMap();

      /**
      * Get detected corners
      */
      aims::BucketMap<Void> corners();

    protected :

      // Parameters
      double _epsilon;
      double _k;
      double _sigma, _sigma2;
      double _u;
      T _levels;
      double _f;
      int _radius;
      int _window;

      // Image
      carto::VolumeRef<T> _image;
      carto::VolumeRef<double> _g; // Gaussian window

      // Harris measure map
      carto::VolumeRef<double> _hmap;

      // Corners
      aims::BucketMap<double> _corners;
  };

  template <class T, int D>
  class HarrisDetector
  {
  };

  template <class T>
  class HarrisDetector<T, 2>
    : public BaseHarrisDetector<T>
    {

    public :

      /**
      *  Constructor
      */
      HarrisDetector( const carto::rc_ptr<carto::Volume<T> > & image,
                      double sigma = 1.2,
                      double k = 0.6,
                      double epsilon = 1E-3,
                      T levels = 255 );

      /**
      * Compute the 3 arrays Ix, Iy and Ixy
      */
      virtual void computeDerivatives();

      /**
      * Compute harris measure for a pixel
      */
      virtual double harrisMeasure( const int x,
                                    const int y,
                                    const int z,
                                    const int t );

      /**
      * Return true if the measure at pixel (x,y) is a local spatial Maxima
      */
      virtual bool isSpatialMaxima( const int x,
                                    const int y,
                                    const int z,
                                    const int t );

    private :
      // Derivatives
      carto::VolumeRef<double> _lx2, _ly2, _lxy;
  };

} // namespace aims

#endif
