/*
 *  Copyright (C) 2000-2004 CEA
 *
 *  This software and supporting documentation were developed by
 *    CEA/DSV/SHFJ
 *    4 place du General Leclerc
 *    91401 Orsay cedex
 *    France
 */



#ifndef AIMS_GEODESIC_PATH_H
#define AIMS_GEODESIC_PATH_H

#include <aims/mesh/surface.h>
#include <aims/mesh/texture.h>
#include <aims/mesh/surfacegen.h>
//#include <aims/mesh/surfaceOperation.h>
//#include <aims/distancemap/meshdistance.h>
//#include <aims/distancemap/meshvoronoi.h>
#include <aims/scalespace/meshDiffuse.h>
//#include <aims/distancemap/meshmorphomat.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/geodesicpath/geodesic_algorithm_dijkstra.h>
#include <aims/geodesicpath/geodesic_mesh.h>

using namespace geodesic;
using namespace aims;
using namespace std;
//using namespace aims::meshdistance;

class GeodesicPath
{
  public:
    int _strain;
    int _method;
    AimsSurfaceTriangle _surface;
    TimeTexture<float> _texCurv;
    geodesic::Mesh _meshSP;

    //Constructor
    GeodesicPath( AimsSurfaceTriangle surface ,TimeTexture<float> texCurv, int method, int strain);
    GeodesicPath( AimsSurfaceTriangle surface , int method, int strain);
    ~GeodesicPath();

  private :
    //private methods
    void computeGraphDijkstra (AimsSurfaceTriangle surface ,TimeTexture<float> texCurv, int method, int strain);

  public :
    vector<int> shortestPathIndiceVextex(unsigned source, unsigned target);
    vector<int> shortestPathIndice3Vextex(unsigned source, unsigned middle, unsigned target);
    vector<Point3df> shortestPathCoordVextex(unsigned source, unsigned target);
    void shortestPathIndiceCoordVextex(unsigned source, unsigned target, std::vector<int>& indice, std::vector<Point3df>& coord3D);
    void shortestPath2Texture(unsigned source, unsigned target, float texturevalue, TimeTexture<float> &tex);
    float shortestPathLength(unsigned source, unsigned target);
    vector<int> maxGeodesicDistance(vector<int> points, int* s, int *d);


};

#endif


