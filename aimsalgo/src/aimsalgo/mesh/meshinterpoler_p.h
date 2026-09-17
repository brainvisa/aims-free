
#ifndef AIMS_MESH_MESHINTERPOLER_P_H
#define AIMS_MESH_MESHINTERPOLER_P_H

#include <aims/mesh/meshinterpoler.h>

namespace aims
{

  struct MeshInterpoler::Private
  {
    Private();

    const AimsSurfaceTriangle *source;
    const AimsSurfaceTriangle *dest;
    TimeTexture<uint> projTriangles;
    TimeTexture<float> projCoord1;
    TimeTexture<float> projCoord2;
    TimeTexture<float> projCoord3;
    const Point3df *sourceVert;
    const AimsVector<uint,3> *sourcePoly;
    uint nSourcePolygon;
    const Point3df *destVertex;
    const AimsVector<uint,3> *destPolygon;
    uint nDestVertex;
    uint nDestPolygon;
    float xthresh;
    float ythresh;
    float zthresh;
  };

}

#endif



