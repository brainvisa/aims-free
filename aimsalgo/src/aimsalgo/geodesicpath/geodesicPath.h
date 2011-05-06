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
#include <aims/scalespace/meshDiffuse.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/geodesicpath/geodesic_algorithm_exact.h>
#include <aims/geodesicpath/geodesic_algorithm_dijkstra.h>
#include <aims/geodesicpath/geodesic_mesh.h>

using namespace geodesic;
using namespace aims;
using namespace std;

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
    vector<int> shortestPath_1_1_ind(unsigned source, unsigned target);
    vector<int> shortestPath_1_1_1_ind(unsigned source, unsigned middle, unsigned target);
    void shortestPath_1_N_ind(unsigned source, vector<unsigned> targets, unsigned *target, double *length);
    void longestPath_1_N_ind(unsigned source, vector<unsigned> targets, unsigned *target, double *length);
    vector<Point3df> shortestPath_1_1_xyz(unsigned source, unsigned target);
    void shortestPath_1_1_ind_xyz(unsigned source, unsigned target, std::vector<int>& indice, std::vector<Point3df>& coord3D);
    void shortestPath_1_1_tex(unsigned source, unsigned target, float texturevalue, TimeTexture<float> &tex);
    double shortestPath_1_1_len(unsigned source, unsigned target);

    void shortestPath_1_N_All_ind(unsigned sources, vector<unsigned> targets, vector<vector<int> >&indices);

    vector<int> longestPath_N_N_ind(vector<int> points, int* s, int *d);

};

#endif
