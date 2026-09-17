
#ifndef AIMS_TOPOLOGY_CLASSIFMEANING_H
#define AIMS_TOPOLOGY_CLASSIFMEANING_H


#include <aims/topology/topoClassifBase.h>


class TopologicalClassificationMeaning
{
public:

  enum TopoType
  {
    TopoA = 10,
    TopoB = 20,
    TopoC = 30,
    TopoD = 40,
    TopoE = 50,
    TopoF = 60,
    TopoG = 70,
    TopoH = 80,
    TopoI = 90
  };

  enum TopoTypeFull
  {
    VolumePoint = 10,
    IsolatedPoint = 20,
    BorderPoint = 30,
    SimplePoint = 30,
    CurvePoint = 40,
    CurvesJunction = 50,
    SurfacePoint = 60,
    SurfaceCurvesJunction = 70,
    SurfacesJunction = 80,
    SurfacesCurvesJunction = 90
  };

  static std::string stringFromDefine( int );
  static std::string name( int, int );
  static int classification( int, int );
};


#endif
