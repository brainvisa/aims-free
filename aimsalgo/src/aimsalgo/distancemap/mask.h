
#ifndef AIMS_DISTANCEMAP_MASK_H
#define AIMS_DISTANCEMAP_MASK_H

#include <aims/config/aimsalgo_config.h>
#include <aims/vector/vector.h>


class ChamferMask
{
  public:
    ChamferMask() : _length( 0 ), _offset( 0 ), _distance( 0 ) {}
    ~ChamferMask() { erase(); }

    void set( int dimx, int dimy, int dimz,
              float sizex, float sizey, float sizez,
              float mult_factor );
    void set( const std::vector<int> & dims, const std::vector<float> & vsize,
              float mult_factor );
    void set( int dimx, int dimy, int dimz, const std::vector<float> & vsize,
              float mult_factor );

    int length() const { return _length; }
    const Point3d& offset( int n ) const { return _offset[ n ]; }
    int distance( int n ) const { return _distance[ n ]; }

  protected:
    int _length;
    Point3d* _offset;
    int* _distance;

    void erase();
};


#endif
