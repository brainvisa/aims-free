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

namespace aims
{


  class GeodesicPath
  {
    public:
      typedef std::vector<unsigned> vect_ui;
      int _strain;
      int _method;
      AimsSurfaceTriangle _surface;
      TimeTexture<float> _texCurv;
      geodesic::Mesh _meshSP;

      //Constructor
      GeodesicPath( const AimsSurfaceTriangle & surface,
                    const TimeTexture<float> & texCurv, int method, int strain);
      GeodesicPath( const AimsSurfaceTriangle & surface , int method,
                    int strain);
      ~GeodesicPath();

      // method: 1=sulci 2=gyri 3=geodesic

    //private :
      //private methods
      void computeGraphDijkstra( const AimsSurfaceTriangle & surface,
                                const TimeTexture<float> & texCurv, int method,
                                int strain);
      void updateWeight( const TimeTexture<float> & texCurv, int method,
                        int strain, double sigmo);

    public :
      vector<unsigned> shortestPath_1_1_ind(unsigned source, unsigned target);
      vector<unsigned> shortestPath_1_1_ind(unsigned source, unsigned target, TimeTexture<short> subset);
      vector<unsigned> shortestPath_1_1_1_ind(unsigned source, unsigned middle, unsigned target);
      void shortestPath_1_N_ind(unsigned source, vector<unsigned> targets, unsigned *target, double *length);
      vector<Point3df> shortestPath_1_1_xyz(unsigned source, unsigned target);
      void shortestPath_1_1_ind_xyz(unsigned source, unsigned target, std::vector<unsigned>& indice, std::vector<Point3df>& coord3D);
      void shortestPath_1_1_tex(unsigned source, unsigned target, float texturevalue, TimeTexture<float> &tex);
      double shortestPath_1_1_len(unsigned source, unsigned target);
      void shortestPath_1_N_All_ind(unsigned sources, vector<unsigned> targets, vector<vector<unsigned> >&indices);

      void longestPath_1_N_ind(unsigned source, vector<unsigned> targets, unsigned *target, double *length, int type_distance);
      vector<vect_ui> longestPath_1_N_len(unsigned source, vector<unsigned> targets, vector<double> &length, int type_distance);


      vector<unsigned> longestPath_N_N_ind(vector<unsigned> points, int* s, int *d, double *length, int type_distance);
      void distanceMap_1_N_ind(unsigned source, vector<float> &distanceMap,double *length, int type_distance);
  };

}

#endif
