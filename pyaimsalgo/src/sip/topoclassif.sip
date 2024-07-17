class TopologicalClassificationBase
{
%TypeHeaderCode
#include <aims/topology/topoClassifBase.h>
%End

public:

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
  virtual ~TopologicalClassificationBase();

  int Cstar();
  int Cbar();

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
};


class TopologicalClassificationMeaning
{
%TypeHeaderCode
#include <aims/topology/classifMeaning.h>
%End

%Docstring
Meaning of TopologicalClassification results
%End

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
