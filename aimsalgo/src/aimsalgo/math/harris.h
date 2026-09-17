
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
