
%ModuleHeaderCode
#include <aims/registration/ffd.h>
%End

namespace aims
{

  //==========================================================================
  //   FFD TRANSFORMATION
  //==========================================================================

  class SplineFfd: aims::Transformation3d
  {
%TypeHeaderCode
#include <aims/registration/ffd.h>
#include <pyaims/data/data.h>
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
    double spline3( double x ) const;

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

  class Reader_SplineFfd : Reader_AimsData_POINT3DF
  {
%TypeHeaderCode
#include <aims/registration/ffd.h>
#ifndef PYAIMS_READER_SPLINEFFD_DEFINED
#define PYAIMS_READER_SPLINEFFD_DEFINED
typedef aims::Reader<aims::SplineFfd> Reader_SplineFfd;
typedef aims::Reader<AimsData<Point3df> >Reader_AimsData_POINT3DF;
#endif
%End

  public:
    Reader_SplineFfd();
    Reader_SplineFfd( const std::string& filename );
    virtual ~Reader_SplineFfd();
    virtual bool read( aims::SplineFfd & obj,
                       int border=0,
                       const std::string* format = 0,
                       int frame = -1 );
  };

  class Writer_SplineFfd : Writer_AimsData_POINT3DF
  {
%TypeHeaderCode
#include <aims/registration/ffd.h>
#ifndef PYAIMS_WRITER_SPLINEFFD_DEFINED
#define PYAIMS_WRITER_SPLINEFFD_DEFINED
typedef aims::Writer<aims::SplineFfd> Writer_SplineFfd;
typedef aims::Writer<AimsData<Point3df> > Writer_AimsData_POINT3DF;
#endif
%End

  public:
    Writer_SplineFfd();
    Writer_SplineFfd( const std::string& filename,
                   carto::Object options = carto::none() );
    virtual ~Writer_SplineFfd();
    virtual bool write( const aims::SplineFfd & obj,
                        bool ascii = false,
                        const std::string* format = 0 );
  };

namespace aims
{

  //==========================================================================
  //   FFD TRILINEAR RESAMPLED TRANSFORMATION
  //==========================================================================

  class TrilinearFfd: aims::SplineFfd
  {
%TypeHeaderCode
#include <aims/registration/ffd.h>
#include <pyaims/data/data.h>
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

    //--- Deformation --------------------------------------------------------
    Point3dd     deformation( const Point3dd& p_mm ) const;
  };


  /** Transform a mesh using FFD vector field deformation.
      Changes the mesh vertices position in place, modifying the input mesh.
  */
  void ffdTransformMesh( AimsTimeSurface_2_VOID & mesh, aims::SplineFfd & spline,
                         const aims::AffineTransformation3d & affine
                           = aims::AffineTransformation3d() );
  void ffdTransformMesh( AimsTimeSurface_3_VOID & mesh, aims::SplineFfd & spline,
                         const aims::AffineTransformation3d & affine
                           = aims::AffineTransformation3d() );
  void ffdTransformMesh( AimsTimeSurface_4_VOID & mesh, aims::SplineFfd & spline,
                         const aims::AffineTransformation3d & affine
                           = aims::AffineTransformation3d() );

  /** Transform a bucket using FFD vector field deformation.
      Returns a new bucket, with possibly a different voxel size from the input
      one.
  */
  rc_ptr_BucketMap_VOID
  ffdTransformBucket( const BucketMap_VOID & bck, aims::SplineFfd & spline,
                      const aims::AffineTransformation3d & affine
                        = aims::AffineTransformation3d(),
                      const Point3df & vs = Point3df( 0., 0., 0. ) );

  /** Transform a graph using FFD vector field deformation.
      Changes the graph meshes and buckets in place, modifying the input graph.
  */
  void ffdTransformGraph( Graph & graph, aims::SplineFfd & spline,
                          const aims::AffineTransformation3d & affine
                            = aims::AffineTransformation3d(),
                          const Point3df & vs = Point3df( 0., 0., 0. ) );

  /** FFD vector field transform for bundles in stream.
      This is a BundleListener / BundleProducer stream processing class
      which applies vector field deformation to bundle data. It can be
      typically connected to a BundleReader and a BundleWriter.
  */
  /*
  class BundleFFDTransformer : aims::BundleListener, aims::BundleProducer
  {
  public:
    BundleFFDTransformer( rc_ptr_SplineFfd deformation,
                          const aims::AffineTransformation3d & affine );
    virtual ~BundleFFDTransformer();
  };
  */

}; // namespace aims

