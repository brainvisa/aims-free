%ModuleHeaderCode
#include <aims/registration/ffd.h>
%End

namespace aims
{

  //==========================================================================
  //   FFD TRANSFORMATION
  //==========================================================================

  class FfdTransformation: soma::Transformation3d
  {
%TypeHeaderCode
#include <aims/registration/ffd.h>
#include <pyaims/data/data.h>
%End

%Docstring
FFD vector field deformation transform

Free Form Deformation is the registration technique used to build the
vector fields. This class is dedicated to the application of the vector
field deformation to transform coordinates.

Vector fields are stored in volumes AimsData_POINT3DF.
%End

   public:
    //--- Constructor --------------------------------------------------------
    FfdTransformation( int dimX = 0, int dimY = 1, int dimZ = 1,
                       float sizeX = 1., float sizeY = 1., float sizeZ = 1. );
    FfdTransformation( int dimX, int dimY, int dimZ,
                       const AimsData_U8 & test_volume );
    FfdTransformation( int dimX, int dimY, int dimZ,
                       const AimsData_S16 & test_volume );
    FfdTransformation( int dimX, int dimY, int dimZ,
                       const AimsData_U16 & test_volume );
    FfdTransformation( int dimX, int dimY, int dimZ,
                       const AimsData_S32 & test_volume );
    FfdTransformation( int dimX, int dimY, int dimZ,
                       const AimsData_U32 & test_volume );
    FfdTransformation( int dimX, int dimY, int dimZ,
                       const AimsData_FLOAT & test_volume );
    FfdTransformation( int dimX, int dimY, int dimZ,
                       const AimsData_DOUBLE & test_volume );
    FfdTransformation( int dimX, int dimY, int dimZ,
                       const AimsData_RGB & test_volume );
    FfdTransformation( int dimX, int dimY, int dimZ,
                       const AimsData_RGBA & test_volume );
    FfdTransformation( int dimX, int dimY, int dimZ,
                       const AimsData_HSV & test_volume );
    FfdTransformation( int dimX, int dimY, int dimZ,
                       const AimsData_POINT3DF & test_volume );

    FfdTransformation( const aims::FfdTransformation & other );
    FfdTransformation( const AimsData_POINT3DF & other );

    operator AimsData_POINT3DF &();

    bool isIdentity() const;

    //--- Control Knots ------------------------------------------------------
    Point3df     getCtrlKnot( int nx, int ny, int nz ) const;
    void         updateCtrlKnot( int nx, int ny, int nz, const Point3df & newCtrlKnot );
    void         updateAllCtrlKnot( const AimsData_POINT3DF & newCtrlKnotGrid );
    void         updateAllCtrlKnotFromDeformation( const AimsData_POINT3DF & newDeformationGrid );

    //--- Modify -------------------------------------------------------------
    void         increaseResolution( const Point3d & addKnots );
    // void         inverseTransform();
    // void         estimateLocalDisplacement( const Point3df & voxelSize);

    //--- Deformation --------------------------------------------------------
    Point3dd     deformation( const Point3dd& p_mm ) const;
    Point3dd     ffdCoord( const Point3dd& p_mm ) const;

    //--- Parameters ---------------------------------------------------------
    int          dimX() const;
    int          dimY() const;
    int          dimZ() const;
    float        sizeX() const;
    float        sizeY() const;
    float        sizeZ() const;
    bool isFlat( int i ) const;
    bool isXFlat() const;
    bool isYFlat() const;
    bool isZFlat() const;

    //--- Debug --------------------------------------------------------------
    void  printControlPointsGrid() const;
    void  writeDebugCtrlKnots( const std::string & filename ) const;
    void  writeDebugDeformations( const std::string & filename,
                                  int dimX, int dimY, int dimZ,
                                  float sizeX, float sizeY, float sizeZ ) const;

    //--- Output -------------------------------------------------------------
    void   write( const std::string & filename ) const;

  protected:
    virtual Point3dd _deformation( const Point3dd& p_mm ) const = 0;
  };

}; // namespace aims

//============================================================================
//   FFD READER/WRITER
//============================================================================

  class Reader_FfdTransformation
  {
%TypeHeaderCode
#include <aims/registration/ffd.h>
#ifndef PYAIMS_READER_FFDTRANSFORMATION_DEFINED
#define PYAIMS_READER_FFDTRANSFORMATION_DEFINED
typedef aims::Reader<aims::FfdTransformation> Reader_FfdTransformation;
#endif
%End

%Docstring
FFD vector field transformation reader. It actually reads a volume of
Point3df.
%End

  public:
    Reader_FfdTransformation();
    Reader_FfdTransformation( const std::string& filename );
    virtual ~Reader_FfdTransformation();
    virtual bool read( aims::FfdTransformation & obj,
                       int border=0,
                       const std::string* format = 0,
                       int frame = -1 );
    virtual aims::FfdTransformation* read(
      int border = 0, const std::string* format = 0, int frame = 0 )
        /Factory/;
  };

  class Writer_FfdTransformation : Writer_AimsData_POINT3DF
  {
%TypeHeaderCode
#include <aims/registration/ffd.h>
#ifndef PYAIMS_WRITER_FFDTRANSFORMATION_DEFINED
#define PYAIMS_WRITER_FFDTRANSFORMATION_DEFINED
typedef aims::Writer<aims::FfdTransformation> Writer_FfdTransformation;
typedef aims::Writer<AimsData<Point3df> > Writer_AimsData_POINT3DF;
#endif
%End

%Docstring
FFD vector field transformation writer. It actually reads a volume of
Point3df.
%End

  public:
    Writer_FfdTransformation();
    Writer_FfdTransformation( const std::string& filename,
                   carto::Object options = carto::none() );
    virtual ~Writer_FfdTransformation();
    virtual bool write( const aims::FfdTransformation & obj,
                        bool ascii = false,
                        const std::string* format = 0 );
  };

namespace aims
{

  class SplineFfd: aims::FfdTransformation
  {
%TypeHeaderCode
#include <aims/registration/ffd.h>
#include <pyaims/data/data.h>
%End

%Docstring
FFD vector field deformation transform

Free Form Deformation is the registration technique used to build the
vector fields. This class is dedicated to the application of the vector
field deformation to transform coordinates.

Vector fields are stored in volumes AimsData_POINT3DF.

This Spline FFD uses cubic spline interpolation between displacement
vectors to process transformed coordinates. See TrilinearFfd for a
variant using trilinear interpolation.

This class is the "base" vector field deformation class, which can perform
point-to-point transformation. It is used by various higher-level classes
or functions to work on higher-level objects:

To resample full 2D or 3D images, see the Resampler class and its derived
classes (see also ResamplerFactory).

As a Transformation3d specialization, the main method of this class is the
transform() method, which actually performs 3D coordinates transformation.
The other methods can be seen as "internal machinery".

\see transformMesh, transformBucket, transformGraph and
BundleTransformer to apply vector field deformations to various types
of objects.
%End

  public:
    //--- Constructor --------------------------------------------------------
    SplineFfd( int dimX = 0, int dimY = 1, int dimZ = 1,
               float sizeX = 1., float sizeY = 1., float sizeZ = 1. );
    SplineFfd( int dimX, int dimY, int dimZ,
               const AimsData_U8 & test_volume );
    SplineFfd( int dimX, int dimY, int dimZ,
               const AimsData_S16 & test_volume );
    SplineFfd( int dimX, int dimY, int dimZ,
               const AimsData_U16 & test_volume );
    SplineFfd( int dimX, int dimY, int dimZ,
               const AimsData_S32 & test_volume );
    SplineFfd( int dimX, int dimY, int dimZ,
               const AimsData_U32 & test_volume );
    SplineFfd( int dimX, int dimY, int dimZ,
               const AimsData_FLOAT & test_volume );
    SplineFfd( int dimX, int dimY, int dimZ,
               const AimsData_DOUBLE & test_volume );
    SplineFfd( int dimX, int dimY, int dimZ,
               const AimsData_RGB & test_volume );
    SplineFfd( int dimX, int dimY, int dimZ,
               const AimsData_RGBA & test_volume );
    SplineFfd( int dimX, int dimY, int dimZ,
               const AimsData_HSV & test_volume );
    SplineFfd( int dimX, int dimY, int dimZ,
               const AimsData_POINT3DF & test_volume );

    SplineFfd( const aims::SplineFfd & other );
    SplineFfd( const AimsData_POINT3DF & other );

    double spline3( double x ) const;

    static aims::SplineFfd* instance() /Factory/;
%MethodCode
    /** Sip persists in seeing this class as abstract. We need to have a means
        to instantiate it. It is a temporary workaround, of course. */
    aims::SplineFfd* ffd = new aims::SplineFfd;
    sipRes = ffd;
%End

  private:
    virtual Point3dd _deformation( const Point3dd& p_mm ) const;
  };

  //==========================================================================
  //   FFD TRILINEAR RESAMPLED TRANSFORMATION
  //==========================================================================

  class TrilinearFfd: aims::FfdTransformation
  {
%TypeHeaderCode
#include <aims/registration/ffd.h>
#include <pyaims/data/data.h>
%End

%Docstring
FFD vector field deformation transform

Free Form Deformation is the registration technique used to build the
vector fields. This class is dedicated to the application of the vector
field deformation to transform coordinates.

This is a variant of SplineFfd which is performing trilinear interpolation
between displacement vectors. See SplineFfd for details.
%End

  public:
    //--- Constructor --------------------------------------------------------
    TrilinearFfd( int dimX = 0, int dimY = 1, int dimZ = 1,
                  float sizeX = 1., float sizeY = 1., float sizeZ = 1. );
    TrilinearFfd( int dimX, int dimY, int dimZ,
                  const AimsData_U8 & test_volume );
    TrilinearFfd( int dimX, int dimY, int dimZ,
                  const AimsData_S16 & test_volume );
    TrilinearFfd( int dimX, int dimY, int dimZ,
                  const AimsData_U16 & test_volume );
    TrilinearFfd( int dimX, int dimY, int dimZ,
                  const AimsData_S32 & test_volume );
    TrilinearFfd( int dimX, int dimY, int dimZ,
                  const AimsData_U32 & test_volume );
    TrilinearFfd( int dimX, int dimY, int dimZ,
                  const AimsData_FLOAT & test_volume );
    TrilinearFfd( int dimX, int dimY, int dimZ,
                  const AimsData_DOUBLE & test_volume );
    TrilinearFfd( int dimX, int dimY, int dimZ,
                  const AimsData_RGB & test_volume );
    TrilinearFfd( int dimX, int dimY, int dimZ,
                  const AimsData_RGBA & test_volume );
    TrilinearFfd( int dimX, int dimY, int dimZ,
                  const AimsData_HSV & test_volume );
    TrilinearFfd( int dimX, int dimY, int dimZ,
                  const AimsData_POINT3DF & test_volume );

    TrilinearFfd( const aims::TrilinearFfd & other );
    TrilinearFfd( const AimsData_POINT3DF & other );

    static aims::TrilinearFfd* instance() /Factory/;
%MethodCode
    /** Sip persists in seeing this class as abstract. We need to have a means
        to instantiate it. It is a temporary workaround, of course. */
    aims::TrilinearFfd* ffd = new aims::TrilinearFfd;
    sipRes = ffd;
%End

  private:
    virtual Point3dd _deformation( const Point3dd& p_mm ) const;
  };

}; // namespace aims
