#ifndef AIMS_MATH_SOBEL_H
#define AIMS_MATH_SOBEL_H

#include <iostream>
#include <cartodata/volume/volume.h>

/**
* Sobel gradient
*/
template <class T, int D>
class SobelGradient
{
 
  public :
    void compute();
    Point3df compute(const int x,
                     const int y, 
                     const int z, 
                     const int t);
};

/**
* Sobel gradient 2D specialization
*/
template <class T>
class SobelGradient<T, 2>
{

  public :
    /**
    *  Constructor
    */
    SobelGradient(const carto::rc_ptr<carto::Volume<T> > & image,
                  const T levels = (T)255);

    Point3df compute(const int x, 
                     const int y, 
                     const int z, 
                     const int t);
                     
    void compute();
    
    carto::VolumeRef<double> & gradX();
    carto::VolumeRef<double> & gradY();
    
  private :
    // Parameters
    T _levels;
    
    // Image
    carto::VolumeRef<T> _image;
    
    carto::VolumeRef<double> _gradX;
    carto::VolumeRef<double> _gradY;
};

#endif
