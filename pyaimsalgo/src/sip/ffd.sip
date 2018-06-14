
%ModuleHeaderCode
#include <aims/registration/ffd.h>
%End

namespace aims
{

  //==========================================================================
  //   FFD TRANSFORMATION
  //==========================================================================

  class FfdTransformation: aims::Transformation3d /Abstract/
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

      To resample full 2D or 3D images, see also the FfdResampler classs and
      its derivatives - these classes are using SplineFfd and resample in
      voxels space.

      As a Transformation3d specialization, the main method of this class is the
      transform() method, which actually performs 3D coordinates transformation.
      The other methods can be seen as "internal machinery".

      .. seealso::

          ffdTransformMesh, ffdTransformBucket, ffdTransformGraph and
          BundleFFDTransformer to apply vector field deformations to various
          types of objects.
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
    void updateDimensions();

    //--- Debug --------------------------------------------------------------
    void  printControlPointsGrid() const;
    void  writeDebugCtrlKnots( const std::string & filename ) const;
    void  writeDebugDeformations( const std::string & filename,
                                  int dimX, int dimY, int dimZ,
                                  float sizeX, float sizeY, float sizeZ ) const;

    //--- Output -------------------------------------------------------------
    void   write( const std::string & filename ) const;
  };

}; // namespace aims

//============================================================================
//   FFD READER/WRITER
//============================================================================

  class Reader_FfdTransformation : Reader_AimsData_POINT3DF
  {
%TypeHeaderCode
#include <aims/registration/ffd.h>
#ifndef PYAIMS_READER_FFDTRANSFORMATION_DEFINED
#define PYAIMS_READER_FFDTRANSFORMATION_DEFINED
typedef aims::Reader<aims::FfdTransformation> Reader_FfdTransformation;
typedef aims::Reader<AimsData<Point3df> >Reader_AimsData_POINT3DF;
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
  };

};

/* for an unknown reason functions defined in aimsalgo in the aims namespace
   are not seen from the python module. Classes are, so are functions defined
   the same way in aimsdata. The only workaround found is to define them
   outside of their namespace
*/

  void ffdTransformMesh( AimsTimeSurface_2_VOID & mesh,
                         aims::FfdTransformation & transformation,
                         const aims::AffineTransformation3d & affine
                           = aims::AffineTransformation3d() );
%Docstring
      Transform a mesh using FFD vector field deformation.
      Changes the mesh vertices position in place, modifying the input mesh.
%End
%MethodCode
  aims::ffdTransformMesh( *a0, *a1, *a2 );
%End

  void ffdTransformMesh( AimsTimeSurface_3_VOID & mesh,
                         aims::FfdTransformation & transformation,
                         const aims::AffineTransformation3d & affine
                           = aims::AffineTransformation3d() );
%MethodCode
  aims::ffdTransformMesh( *a0, *a1, *a2 );
%End

  void ffdTransformMesh( AimsTimeSurface_4_VOID & mesh,
                         aims::FfdTransformation & transformation,
                         const aims::AffineTransformation3d & affine
                           = aims::AffineTransformation3d() );
%MethodCode
  aims::ffdTransformMesh( *a0, *a1, *a2 );
%End

  rc_ptr_BucketMap_VOID
  ffdTransformBucket( const BucketMap_VOID & bck, aims::FfdTransformation & transformation,
                      const aims::AffineTransformation3d & affine
                        = aims::AffineTransformation3d(),
                      const Point3df & vs = Point3df( 0., 0., 0. ) );
%Docstring
      Transform a bucket using FFD vector field deformation.
      Returns a new bucket, with possibly a different voxel size from the input
      one.
%End
%MethodCode
  sipRes
    = new carto::rc_ptr<aims::BucketMap<Void> >( aims::ffdTransformBucket(
      *a0, *a1, *a2, *a3 ) );
%End

  void ffdTransformGraph( Graph & graph, aims::FfdTransformation & transformation,
                          const aims::AffineTransformation3d & affine
                            = aims::AffineTransformation3d(),
                          const Point3df & vs = Point3df( 0., 0., 0. ) );
%Docstring
      Transform a graph using FFD vector field deformation.
      Changes the graph meshes and buckets in place, modifying the input graph.
%End
%MethodCode
  aims::ffdTransformGraph( *a0, *a1, *a2, *a3 );
%End

namespace aims
{

  class BundleFFDTransformer : aims::BundleListener, aims::BundleProducer
  {
%TypeHeaderCode
#include <aims/registration/ffd.h>
%End

%Docstring
      FFD vector field transform for bundles in stream.
      This is a BundleListener / BundleProducer stream processing class
      which applies vector field deformation to bundle data. It can be
      typically connected to a BundleReader and a BundleWriter.
%End

  public:
    BundleFFDTransformer( rc_ptr_FfdTransformation & deformation,
                          const aims::AffineTransformation3d & affine );
    virtual ~BundleFFDTransformer();
  };

}; // namespace aims

