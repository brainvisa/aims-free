#ifndef AIMS_UTILITY_BOUNDINGBOX_H
#define AIMS_UTILITY_BOUNDINGBOX_H

#include <aims/vector/vector.h>
#include <aims/bucket/bucket.h>


namespace aims
{
  class AffineTransformation3d;

  class BoundingBox
  {
  public:
    BoundingBox( const AffineTransformation3d* motion = 0 );
    ~BoundingBox();

    void reset();
    /// \c motion will be copied. It's valid to pass a null pointer.
    void setMotion( const AffineTransformation3d* motion );
    const AffineTransformation3d *motion() const;
    /// number of points recorded
    unsigned count() const;
    Point3df minimum() const;
    Point3df maximum() const;
    void add( const Point3df & pos );
    template <typename T>
    void add( const BucketMap<T> & bck );

  private:
    unsigned	_npoints;
    Point3df	_minimum;
    Point3df	_maximum;
    AffineTransformation3d	*_motion;
  };


  inline 
  const AffineTransformation3d *BoundingBox::motion() const
  {
    return _motion;
  }


  inline 
  unsigned BoundingBox::count() const
  {
    return _npoints;
  }


  inline 
  Point3df BoundingBox::minimum() const
  {
    return _minimum;
  }


  inline 
  Point3df BoundingBox::maximum() const
  {
    return _maximum;
  }

}

#endif

