/* Copyright (C) 2000-2013 CEA
 *
 * This software and supporting documentation were developed by
 *     bioPICSEL
 *     CEA/DSV/I²BM/MIRCen/LMN, Batiment 61,
 *     18, route du Panorama
 *     92265 Fontenay-aux-Roses
 *     France
 */


#ifndef AIMS_REGISTRATION_FFD_H
#define AIMS_REGISTRATION_FFD_H

#include <aims/data/data.h>                           // AimsData
#include <aims/io/io_g.h>                             // aims::Reader / Writer
#include <aims/math/bspline.h>                        // aims::TabulBSpline
#include <aims/resampling/nearestneighborresampler.h>
#include <aims/resampling/linearresampler.h>
#include <aims/resampling/cubicresampler.h>
#include <aims/resampling/resamplerfactory.h>
#include <soma-io/transformation/transformation.h>    // soma::Transformation
#include <aims/utility/channel.h>
#include <aims/vector/vector.h>                       // Point*
#include <aims/mesh/surface.h>
#include <aims/bucket/bucketMap.h>
#include <aims/fibers/bundles.h>
#include <limits>
#include <string>

class Graph;

namespace aims {

//==========================================================================
  //   FFD TRANSFORMATION
  //==========================================================================

  /** FFD vector field deformation transform

      Free Form Deformation is the registration technique used to build the
      vector fields. This class is dedicated to the application of the vector
      field deformation to transform coordinates.

      Vector fields are stored in volumes AimsData<Point3df>.
  **/
  class FfdTransformation : public soma::Transformation3d
  {
  public:
    FfdTransformation( int dimX = 0, int dimY = 1, int dimZ = 1,
                       float sizeX = 1., float sizeY = 1., float sizeZ = 1. );
    template <typename T>
    FfdTransformation( int dimX, int dimY, int dimZ,
                       const AimsData<T> & test_volume );
    template <typename T>
    FfdTransformation( int dimX, int dimY, int dimZ,
                       const carto::Volume<T> & test_volume );

    FfdTransformation( const FfdTransformation & other );
    FfdTransformation( const AimsData<Point3df> & other );
    FfdTransformation & operator=( const FfdTransformation & other );

    operator const AimsData<Point3df>&() const { return _ctrlPointDelta; }
    operator AimsData<Point3df>&() { return _ctrlPointDelta; }

    /// Always false, because testing for identity is expensive
    bool isIdentity() const CARTO_OVERRIDE { return false; }

    //--- Control Knots ------------------------------------------------------
    Point3df     getCtrlKnot( int nx, int ny, int nz ) const;
    void         updateCtrlKnot( int nx, int ny, int nz, const Point3df & newCtrlKnot );
    void         updateAllCtrlKnot( const AimsData<Point3df> & newCtrlKnotGrid );
    void         updateAllCtrlKnotFromDeformation( const AimsData<Point3df> & newDeformationGrid );

    //--- Modify -------------------------------------------------------------
    void         increaseResolution( const Point3d & addKnots );
    // void         inverseTransform();
    // void         estimateLocalDisplacement( const Point3df & voxelSize);

    //--- Deformation --------------------------------------------------------
    Point3dd     deformation( const Point3dd& p_mm ) const;
    Point3dd     ffdCoord( const Point3dd& p_mm ) const
    { return mmToSplineVox(p_mm); }

    //--- Parameters ---------------------------------------------------------
    int          dimX() const { return _dimx; }
    int          dimY() const { return _dimy; }
    int          dimZ() const { return _dimz; }
    int getSizeX() const { return _dimx; }
    int getSizeY() const { return _dimy; }
    int getSizeZ() const { return _dimz; }
    std::vector<int> getSize() const
    { std::vector<int> s( 3 ); s[0] = _dimx; s[1] = _dimy; s[2] = _dimz;
      return s; }
    float        sizeX() const { return _vsx; }
    float        sizeY() const { return _vsy; }
    float        sizeZ() const { return _vsz; }
    std::vector<float> getVoxelSize() const
    { std::vector<float> v( 3 ); v[0] = _vsx; v[1] = _vsy; v[2] = _vsz;
      return v; }
    bool isFlat( int i ) const
    {
      switch(i)
      {
        case 0:  return _flatx;
        case 1:  return _flaty;
        case 2:  return _flatz;
        default: return false;
      }
    }
    bool isXFlat() const { return _flatx; }
    bool isYFlat() const { return _flaty; }
    bool isZFlat() const { return _flatz; }
    virtual void updateDimensions();

    //--- Debug --------------------------------------------------------------
    void  printControlPointsGrid() const;
    void  writeDebugCtrlKnots( const std::string & filename ) const;
    void  writeDebugDeformations( const std::string & filename,
                                  int dimX, int dimY, int dimZ,
                                  float sizeX, float sizeY, float sizeZ ) const;

    //--- Output -------------------------------------------------------------
    void   write( const std::string & filename ) const;

  protected:
    //--- Protected methods --------------------------------------------------
    Point3dd     splineVoxToMm( const Point3dd& p ) const;
    Point3dd     mmToSplineVox( const Point3dd& p ) const;
    void         updateGridResolution( const AimsData<Point3df> & newGrid );

    virtual Point3dd _deformation( const Point3dd& p_mm ) const = 0;

    AimsData<Point3df>  _ctrlPointDelta;
    int _dimx, _dimy, _dimz;
    float _vsx, _vsy, _vsz;
    bool _flatx, _flaty, _flatz;
  };

  template <typename T>
  inline
  FfdTransformation::FfdTransformation( int dimX, int dimY, int dimZ,
                                        const AimsData<T> & test_volume ):
    _ctrlPointDelta( dimX, dimY, dimZ ),
    _dimx( dimX ), _dimy( dimY ), _dimz( dimZ ),
    _flatx( dimX == 1 ), _flaty( dimY == 1 ), _flatz( dimZ == 1 )
  {
    _ctrlPointDelta = Point3df(0., 0., 0.);
    _ctrlPointDelta.setSizeXYZT(
      _flatx ? test_volume.sizeX() : double(test_volume.dimX() - 1) / double(dimX - 1) * test_volume.sizeX(),
      _flaty ? test_volume.sizeY() : double(test_volume.dimY() - 1) / double(dimY - 1) * test_volume.sizeY(),
      _flatz ? test_volume.sizeZ() : double(test_volume.dimZ() - 1) / double(dimZ - 1) * test_volume.sizeZ()
    );
    updateDimensions();
  }

  template <typename T>
  inline
  FfdTransformation::FfdTransformation( int dimX, int dimY, int dimZ,
                                        const carto::Volume<T> & test_volume ):
    _ctrlPointDelta( dimX, dimY, dimZ ),
    _dimx( dimX ), _dimy( dimY ), _dimz( dimZ ),
    _flatx( dimX == 1 ), _flaty( dimY == 1 ), _flatz( dimZ == 1 )
  {
    _ctrlPointDelta = Point3df(0., 0., 0.);
    std::vector<float> vs = test_volume.getVoxelSize();
    _ctrlPointDelta.setSizeXYZT(
      _flatx ? vs[0] : double(test_volume.getSizeX() - 1) / double(dimX - 1) * vs[0],
      _flaty ? vs[1] : double(test_volume.getSizeY() - 1) / double(dimY - 1) * vs[1],
      _flatz ? vs[2] : double(test_volume.getSizeZ() - 1) / double(dimZ - 1) * vs[2]
    );
    updateDimensions();
  }

  inline Point3dd
  FfdTransformation::splineVoxToMm( const Point3dd & p ) const
  {
    return Point3dd( p[0] * sizeX(),
                     p[1] * sizeY(),
                     p[2] * sizeZ() );
  }

  inline Point3dd
  FfdTransformation::mmToSplineVox( const Point3dd & p ) const
  {
    return Point3dd( p[0] / sizeX(),
                     p[1] / sizeY(),
                     p[2] / sizeZ() );
  }

  //==========================================================================
  //   FFD TRANSFORMATION
  //==========================================================================

  /** FFD vector field deformation transform

      Free Form Deformation is the registration technique used to build the
      vector fields. This class is dedicated to the application of the vector
      field deformation to transform coordinates.

      Vector fields are stored in volumes AimsData<Point3df>.

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
  */
  class SplineFfd: public FfdTransformation
  {
   public:
    //--- Constructor --------------------------------------------------------
    SplineFfd( int dimX = 0, int dimY = 1, int dimZ = 1,
                  float sizeX = 1., float sizeY = 1., float sizeZ = 1. );
    template <typename T>
    SplineFfd( int dimX, int dimY, int dimZ,
                  const AimsData<T> & test_volume );
    template <typename T>
    SplineFfd( int dimX, int dimY, int dimZ,
                  const carto::Volume<T> & test_volume );

    SplineFfd( const SplineFfd & other );
    SplineFfd( const AimsData<Point3df> & other );
    SplineFfd & operator=( const SplineFfd & other );

    virtual void updateDimensions();
    //--- Deformation --------------------------------------------------------
    Point3dd     deformation( const Point3dd& p_mm ) const;
    double spline3( double x ) const { return _spline(x); }

  private:
    virtual Point3dd transformDouble( double x, double y, double z ) const;
    virtual Point3dd _deformation( const Point3dd& p_mm ) const
    { return deformation_private(p_mm); };
    Point3dd deformation_private( const Point3dd& p_mm ) const;

    const aims::TabulBSpline _spline;
    std::vector<int> _mirrorcoefvecx;
    std::vector<int> _mirrorcoefvecy;
    std::vector<int> _mirrorcoefvecz;
    int *_mirrorcoefx;
    int *_mirrorcoefy;
    int *_mirrorcoefz;
  };

  template <typename T>
  inline
  SplineFfd::SplineFfd( int dimX, int dimY, int dimZ,
                        const AimsData<T> & test_volume ):
    FfdTransformation( dimX, dimY, dimZ, test_volume ),
    _spline(3, 0)
  {
  }

  template <typename T>
  inline
  SplineFfd::SplineFfd( int dimX, int dimY, int dimZ,
                        const carto::Volume<T> & test_volume ):
    FfdTransformation( dimX, dimY, dimZ, test_volume ),
    _spline(3, 0)
  {
  }


  //==========================================================================
  //   FFD TRILINEAR RESAMPLED TRANSFORMATION
  //==========================================================================

  /** FFD vector field deformation transform

      Free Form Deformation is the registration technique used to build the
      vector fields. This class is dedicated to the application of the vector
      field deformation to transform coordinates.

      This is a variant of SplineFfd which is performing trilinear interpolation
      between displacement vectors. See SplineFfd for details.
  */
  class TrilinearFfd: public FfdTransformation
  {
   public:
    //--- Constructor --------------------------------------------------------
    TrilinearFfd( int dimX = 0, int dimY = 1, int dimZ = 1,
                  float sizeX = 1., float sizeY = 1., float sizeZ = 1. );
    template <typename T>
    TrilinearFfd( int dimX, int dimY, int dimZ,
                  const AimsData<T> & test_volume );

    TrilinearFfd( const TrilinearFfd & other );
    TrilinearFfd( const AimsData<Point3df> & other );
    TrilinearFfd & operator=( const TrilinearFfd & other );

    //--- Deformation --------------------------------------------------------
    Point3dd deformation( const Point3dd& p_mm ) const;
  private:
    virtual Point3dd transformDouble( double x, double y, double z ) const;
    virtual Point3dd _deformation( const Point3dd& p_mm ) const
    { return deformation_private(p_mm); };
    Point3dd deformation_private( const Point3dd& p_mm ) const;
  };

  template <typename T>
  inline
  TrilinearFfd::TrilinearFfd( int dimX, int dimY, int dimZ,
                              const AimsData<T> & test_volume ):
    FfdTransformation( dimX, dimY, dimZ, test_volume )
  {
  }

//============================================================================
//   FFD READER/WRITER
//============================================================================

  /** FFD vector field transformation writer. It actually reads a volume of
      Point3df.
  */
  template<>
  class Writer<aims::FfdTransformation> : public Writer<AimsData<Point3df> >
  {
    typedef Writer<AimsData<Point3df> > base;
  public:
    inline Writer(): base() {}
    inline Writer( const std::string& filename,
                   carto::Object options = carto::none() ):
      base( filename, options ) {}
    virtual ~Writer() {}
    virtual bool write( const aims::FfdTransformation & obj,
                        bool ascii = false,
                        const std::string* format = 0 );
  };

} // namespace aims

#endif
