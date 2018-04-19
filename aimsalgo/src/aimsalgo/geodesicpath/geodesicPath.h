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

  /** Geodesic paths or distance maps, using the Dijkstra algorithm

      ex:

      \code
      GeodesicPath gp( mesh, 0, 0 );
      TimeTexture<float>  dmap;
      double dmax = 0;
      // get a distance map from vertex no 12
      gp.distanceMap_1_N_ind( 12, dmap[0].data(), &dmax, 0 );
      \endcode
  */
  class GeodesicPath
  {
    public:
      typedef std::vector<unsigned> vect_ui;
      int _strain;
      int _method;
      AimsSurfaceTriangle _surface;
      TimeTexture<float> _texCurv;
      geodesic::Mesh _meshSP;

      /** Initializes the geodesic path structures for a given mesh geometry,
          using a constraint map (curvature, typically). Several methods are
          available.

          \param surface mesh to compute geodesic paths or distances on
          \param texCurv constraint map texture
          \param method 0: unconstrained,
            1: minimize the constraint map, suitable for sulci using the
               curvature as constraint,
            2: maximize constraint map, suitable for gyri using the curvature
               as constraint,
            3: unconstrained
          \param strain weight factor on the constraints map.
            In practice we use 3 for sulci and gyri.
      */
      GeodesicPath( const AimsSurfaceTriangle & surface,
                    const TimeTexture<float> & texCurv, int method, int strain);
      /** Same as the above constructor, except that the constraints map is
          computed internally as the curvature of the mesh.
      */
      GeodesicPath( const AimsSurfaceTriangle & surface , int method,
                    int strain);
      ~GeodesicPath();

      // method: 1=sulci 2=gyri 3=geodesic

    //private :
      //private methods
      /** initialize internal stuctures, should be private. */
      void computeGraphDijkstra( const AimsSurfaceTriangle & surface,
                                const TimeTexture<float> & texCurv, int method,
                                int strain);
      /** update internal structures, should be private. */
      void updateWeight( const TimeTexture<float> & texCurv, int method,
                        int strain, double sigmo);

    public :
      std::vector<unsigned> shortestPath_1_1_ind(unsigned source, unsigned target);
      std::vector<unsigned> shortestPath_1_1_ind(unsigned source, unsigned target, TimeTexture<short> subset);
      std::vector<unsigned> shortestPath_1_1_1_ind(unsigned source, unsigned middle, unsigned target);
      void shortestPath_1_N_ind(unsigned source, std::vector<unsigned> targets, unsigned *target, double *length);
      std::vector<Point3df> shortestPath_1_1_xyz(unsigned source, unsigned target);
      void shortestPath_1_1_ind_xyz(unsigned source, unsigned target, std::vector<unsigned>& indice, std::vector<Point3df>& coord3D);
      void shortestPath_1_1_tex(unsigned source, unsigned target, float texturevalue, TimeTexture<float> &tex);
      double shortestPath_1_1_len(unsigned source, unsigned target);
      void shortestPath_1_N_All_ind(unsigned sources, std::vector<unsigned> targets, std::vector<std::vector<unsigned> >&indices);

      void longestPath_1_N_ind(unsigned source, std::vector<unsigned> targets, unsigned *target, double *length, int type_distance);
      std::vector<vect_ui> longestPath_1_N_len(unsigned source, std::vector<unsigned> targets, std::vector<double> &length, int type_distance);


      std::vector<unsigned> longestPath_N_N_ind(std::vector<unsigned> points, int* s, int *d, double *length, int type_distance);
      /** Compute a distance map from a given point

          \param source index of the starting point (vertex number)
          \param distanceMap output distance map, the vector can be empty, it
            will be filled with as many values as the mesh vertices number.
          \param length output max distance
          \param type_distance 0: weighted distance,
            1: euclidean distance
      */
      void distanceMap_1_N_ind(unsigned source,
                               std::vector<float> &distanceMap,double *length,
                               int type_distance);
  };

}

#endif
