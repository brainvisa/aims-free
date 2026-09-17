

#ifndef AIMS_TOPOLOGY_TOPOCLASSIFBASE_H
#define AIMS_TOPOLOGY_TOPOCLASSIFBASE_H

#include <aims/topology/topoBase.h>


class TopologicalClassificationBase
{
public:

  // Numbering of neighborhood points according to Malandain
  // 
  //  0  1  2     9 10 11    17 18 19
  //  3  4  5    12  C 13    20 21 22
  //  6  7  8    14 15 16    23 24 25
  // 
  // Numbering according to Mangin
  //
  // 19 11 23     7  2 15    20 12 24
  //  8  3 16     1  0  6     9  4 17
  // 21 13 25    10  5 18    22 14 26
  //
  enum PointNumber
  {
    PointC  =  0,
    Point0  = 19,
    Point1  = 11,
    Point2  = 23,
    Point3  =  8,
    Point4  =  3,
    Point5  = 16,
    Point6  = 21,
    Point7  = 13,
    Point8  = 25,
    Point9  =  7,
    Point10 =  2,
    Point11 = 15,
    Point12 =  1,
    Point13 =  6,
    Point14 = 10,
    Point15 =  5,
    Point16 = 18,
    Point17 = 20,
    Point18 = 12,
    Point19 = 24,
    Point20 =  9,
    Point21 =  4,
    Point22 = 17,
    Point23 = 22,
    Point24 = 14,
    Point25 = 26
  };

  TopologicalClassificationBase();
  virtual ~TopologicalClassificationBase() { }

  int Cstar() { return _topo->Cstar(); }
  int Cbar()  { return _topo->Cbar(); }

  void computeLocalCCNumbers( const Point3d&, int );
  void computeLocalCCNumbers( const Point3d&, int, int );
  void computeLocalCCNumbersComplement( const Point3d&, int );

  bool isSimplePoint();
  bool isCurvesPoint();
  bool isSurfacesPoint();
  bool isRealSurfacePoint();

  // Faster method using a binary graph
  bool isSimplePoint( const Point3d&, int );
  bool isSimplePoint( const Point3d&, int, int );
  bool isSimplePointComplement( const Point3d&, int );

protected:

  int bdd( int * );

  TopologyBase *_topo;

  bool _statInv;
};

#endif
