
#ifndef _AIMS_ACTIVPIXEL_H_
#define _AIMS_ACTIVPIXEL_H_

#include <vector>
#include <aims/vector/vector.h>

class AimsActivPixel
{
  public:
    AimsActivPixel() { }
    virtual ~AimsActivPixel();

    std::vector< Point3df >& activPoints() { return _activPoints; }
    const std::vector< Point3df >& activPoints() const { return _activPoints; }

    Point3df& voxelSize() { return _voxelSize; }
    const Point3df& voxelSize() const { return _voxelSize; }

  protected:
    Point3df _voxelSize;
    std::vector< Point3df > _activPoints;
};


inline
AimsActivPixel::~AimsActivPixel()
{
  _activPoints.clear();
}

#endif
