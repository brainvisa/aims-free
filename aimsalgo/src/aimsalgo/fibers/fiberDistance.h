#ifndef AIMS_FIBERS_FIBERDISTANCE_H
#define AIMS_FIBERS_FIBERDISTANCE_H

#include <aims/fibers/bundles.h>

namespace aims
{

  float fiberDistance( FiberPoint* f1, FiberPoint* f2, int &fiberSize1,
                       int &fiberSize2 );
  float fiberDistanceMax( FiberPoint* f1, FiberPoint* f2, int &fiberSize1,
                          int &fiberSize2 );

  float fiberDistance( std::vector<Point3df>& f1, std::vector<Point3df>& f2 );
  float fiberDistanceMax( std::vector<Point3df>& f1,
                          std::vector<Point3df>& f2 );

  float getClosestFiberPointDist( Point3df& p1, FiberPoint* f2, Point3df& pMin,
                                  int &fiberSize2 );
  float getClosestFiberPointDist( Point3df& p1, std::vector<Point3df>& f2,
                                  Point3df& pMin );

  float fiberDistanceMaxDistCorrespPoints( FiberPoint* f1, FiberPoint* f2,
                                           int &fiberSize1 );
  float fiberDistanceMaxDistCorrespPoints( std::vector<Point3df>& f1,
                                           std::vector<Point3df>& f2 );


  float calculateMinFibersLength( FiberPoint* f1, FiberPoint* f2,
                                  int &fiberSize1, int &fiberSize2 );
  float calculateMinFibersLength( std::vector<Point3df>& f1,
                                  std::vector<Point3df>& f2 );


  float fiberDistanceMaxDistCorrespPointsNormByMinLength(
    FiberPoint* f1, FiberPoint* f2, float normFactor, int &fiberSize1,
    int &fiberSize2 );
  float fiberDistanceMaxDistCorrespPointsNormByMinLength(
    std::vector<Point3df>& f1, std::vector<Point3df>& f2, float normFactor );


  float fiberAffinity( std::vector<Point3df>& f1, std::vector<Point3df>& f2,
                       float var );
  float fiberAffinityMax( std::vector<Point3df>& f1, std::vector<Point3df>& f2,
                          float var );
  float fiberAffinityMaxDistCorrespPoints( std::vector<Point3df>& f1,
                                           std::vector<Point3df>& f2,
                                           float var );
  float fiberAffinityMaxDistCorrespPointsNormByMinLength(
      std::vector<Point3df>& f1, std::vector<Point3df>& f2, float var,
      float normFactor );

  float fiberAffinity( FiberPoint* f1, FiberPoint* f2, float var,
                       int &fiberSize1, int &fiberSize2 );
  float fiberAffinityMax( FiberPoint* f1, FiberPoint* f2, float var,
                          int &fiberSize1, int &fiberSize2 );
  float fiberAffinityMaxDistCorrespPoints( FiberPoint* f1, FiberPoint* f2,
                                           float var, int &fiberSize1,
                                           int &fiberSize2 );
  float fiberAffinityMaxDistCorrespPointsNormByMinLength(
    FiberPoint* f1,  FiberPoint* f2, float var, float normFactor,
    int &fiberSize1, int &fiberSize2 );

}

#endif // ifndef AIMS_FIBERS_FIBERDISTANCE_H
