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
#include <aims/transformation/transformation.h>       // aims::Transformatin
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

      \see ffdTransformMesh, ffdTransformBucket, ffdTransformGraph and
      BundleFFDTransformer to apply vector field deformations to various types
      of objects.
  */
  class SplineFfd: public aims::Transformation3d
  {
   public:
    //--- Constructor --------------------------------------------------------
    SplineFfd( int dimX = 0, int dimY = 1, int dimZ = 1,
               float sizeX = 1., float sizeY = 1., float sizeZ = 1. );
    template <typename T>
    SplineFfd( int dimX, int dimY, int dimZ,
               const AimsData<T> & test_volume );

    SplineFfd( const SplineFfd & other );
    SplineFfd( const AimsData<Point3df> & other );
    SplineFfd & operator=( const SplineFfd & other );

    operator const AimsData<Point3df>&() const { return _ctrlPointDelta; }
    operator AimsData<Point3df>&() { return _ctrlPointDelta; }

    bool isIdentity() const
    {
      for( int z = 0; z < dimZ(); ++z )
      for( int y = 0; y < dimY(); ++y )
      for( int x = 0; x < dimX(); ++x )
      {
        if( _ctrlPointDelta(x, y, z) != Point3df(0., 0., 0.) )
          return false;
      }
      return true;
    }

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
    double spline3( double x ) const { return _spline(x); }

    //--- Parameters ---------------------------------------------------------
    int          dimX() const { return _dimx; }
    int          dimY() const { return _dimy; }
    int          dimZ() const { return _dimz; }
    float        sizeX() const { return _vsx; }
    float        sizeY() const { return _vsy; }
    float        sizeZ() const { return _vsz; }
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
    void updateDimensions();

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
    virtual Point3dd transformDouble( double x, double y, double z ) const;

    AimsData<Point3df>  _ctrlPointDelta;
    aims::TabulBSpline  _spline;
    int _dimx, _dimy, _dimz;
    float _vsx, _vsy, _vsz;
    bool _flatx, _flaty, _flatz;
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
    _spline(3, 0),
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

  inline Point3dd
  SplineFfd::splineVoxToMm( const Point3dd & p ) const
  {
    return Point3dd( p[0] * sizeX(),
                     p[1] * sizeY(),
                     p[2] * sizeZ() );
  }

  inline Point3dd
  SplineFfd::mmToSplineVox( const Point3dd & p ) const
  {
    return Point3dd( p[0] / sizeX(),
                     p[1] / sizeY(),
                     p[2] / sizeZ() );
  }

//============================================================================
//   FFD READER/WRITER
//============================================================================

  /** FFD vector field transformation reader. It actually reads a volume of
      Point3df.
  */
  template <>
  class Reader<aims::SplineFfd>: public Reader<AimsData<Point3df> >
  {
    typedef Reader<AimsData<Point3df> > base;
  public:
    Reader(): base() {}
    Reader( const std::string& filename ): base(filename) {}
    virtual ~Reader() {}
    virtual bool read( aims::SplineFfd & obj,
                       int border=0,
                       const std::string* format = 0,
                       int frame = -1 )
    {
      bool res = base::read(obj, border, format, frame);
      if( res )
      {
        obj.updateDimensions();
      }
      return res;
    }
  };

  /** FFD vector field transformation writer. It actually reads a volume of
      Point3df.
  */
  template<>
  class Writer<aims::SplineFfd> : public Writer<AimsData<Point3df> >
  {
    typedef Writer<AimsData<Point3df> > base;
  public:
    inline Writer(): base() {}
    inline Writer( const std::string& filename,
                   carto::Object options = carto::none() ):
      base( filename, options ) {}
    virtual ~Writer() {}
    virtual bool write( const aims::SplineFfd & obj,
                        bool ascii = false,
                        const std::string* format = 0 )
    {
      return base::write(obj, ascii, format);
    }
  };

  //==========================================================================
  //   FFD TRILINEAR RESAMPLED TRANSFORMATION
  //==========================================================================

  /** FFD vector field deformation transform

      Free Form Deformation is the registration technique used to build the
      vector fields. This class is dedicated to the application of the vector
      field deformation to transform coordinates.

      This is a variant of SplineFfd which is performing trilinear interpolation
      between displacement vectors. See SplineFfd for details.

      The implementation of TrilinearFfd inheriting SplineFfd is actually a
      quick-and-dirty convenience implementation and is a design flaw.
  */
  class TrilinearFfd: public SplineFfd
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
    Point3dd     deformation( const Point3dd& p_mm ) const;

  private:
    virtual Point3dd transformDouble( double x, double y, double z ) const;
  };

  template <typename T>
  inline
  TrilinearFfd::TrilinearFfd( int dimX, int dimY, int dimZ,
                              const AimsData<T> & test_volume ):
    SplineFfd( dimX, dimY, dimZ, test_volume )
  {
  }


//============================================================================
//   R E S A M P L I N G
//============================================================================

  /** Resampling image using Free Form Deformation transformations.

      FfdResampler is using a SplineFfd or TrilinearFfd transformation to
      resample a full volume. It is an abstract class which is specialized to
      implement cubic spline, trilinear, or nearest neighbor interpolation in
      voxels values.
  */
  template <class T>
  class FfdResampler
  {
    public:

      virtual ~FfdResampler();
      virtual void init() = 0;
      virtual void setRef(const AimsData<T> & ref) = 0;
      virtual Point3df resample( const Point3df & output_location,
                                 T & output_value, int t = 0 ) = 0;
  };

  /** Resampling image using Free Form Deformation transformations.

      This variant of FfdResampler performs cubic spline interpolation in voxels
      values.
  */
  template <class T, class C = T>
  class SplineFfdResampler : public FfdResampler<T>,
                             public CubicResampler<C>
  {
    public:

      virtual ~SplineFfdResampler();
      virtual void init();
      SplineFfdResampler( const SplineFfd & spline,
                          T background = defaultBackground() );
      SplineFfdResampler( const SplineFfd & spline,
                          const AffineTransformation3d & affine,
                          T background = defaultBackground() );
      virtual void setRef( const AimsData<T> & ref);
      virtual Point3df resample( const Point3df & output_location,
                                 T & output_value, int t = 0 );
      static T defaultBackground();

    private:
      void updateCoef( int t = 0 );
      AffineTransformation3d       _affine;
      const SplineFfd &  _transformation;
      AimsData<T>        _ref;
      std::vector<AimsData<double> >  _channelcoef;
      ChannelSelector<T,C>            _channelselector;

      T   _background;
      int _samples;
      int _last_t;
      std::vector<C>      _min;
      std::vector<C>      _max;
      int _dimx, _dimy, _dimz;
      float _vsx, _vsy, _vsz;
      bool _idaffine;
  };

  /** Resampling image using Free Form Deformation transformations.

      This variant of FfdResampler uses nearest neighbor voxels values.
  */
  template <class T, class C = T>
  class NearestNeighborFfdResampler : public FfdResampler<T>,
                                       public NearestNeighborResampler<C>
  {
    public:

      virtual ~NearestNeighborFfdResampler();
      virtual void init();
      NearestNeighborFfdResampler( const SplineFfd & spline,
                                   T background = defaultBackground() );
      NearestNeighborFfdResampler( const SplineFfd & spline,
                                   const AffineTransformation3d & affine, T background = defaultBackground() );
      virtual void setRef( const AimsData<T> & ref );
      virtual Point3df resample( const Point3df & output_location,
                                 T & output_value, int t = 0 );
      static T defaultBackground();

    private:
      AffineTransformation3d       _affine;
      const SplineFfd &  _transformation;
      AimsData<T>        _ref;

      T   _background;
      int _samples;
      int _dimx, _dimy, _dimz;
      float _vsx, _vsy, _vsz;
      bool _idaffine;
  };


  /** Resampling image using Free Form Deformation transformations.

      This variant of FfdResampler performs trilinear interpolation in voxels
      values.
  */
  template <class T, class C = T>
  class TrilinearFfdResampler : public FfdResampler<T>,
                                public LinearResampler<C>
  {
    public:

      virtual ~TrilinearFfdResampler();
      virtual void init();
      TrilinearFfdResampler( const SplineFfd & spline,
                             T background = defaultBackground() );
      TrilinearFfdResampler( const SplineFfd & spline,
                             const AffineTransformation3d & affine,
                             T background = (T)0);
      virtual void setRef( const AimsData<T> & ref );
      virtual Point3df resample( const Point3df & output_location,
                                 T & output_value, int t = 0 );
      static T defaultBackground();

    private:
      const Motion         _affine;
      const SplineFfd &    _transformation;
      AimsData<T>          _ref;
      ChannelSelector<T,C> _channelselector;

      T   _background;
      int _samples;
      int _dimx, _dimy, _dimz;
      float _vsx, _vsy, _vsz;
      bool _idaffine;
  };

  /** Transform a mesh using FFD vector field deformation.
      Changes the mesh vertices position in place, modifying the input mesh.
  */
  template <int D>
  void ffdTransformMesh( AimsTimeSurface<D, Void> & mesh, SplineFfd & spline,
                         const AffineTransformation3d & affine
                           = AffineTransformation3d() );

  /** Transform a bucket using FFD vector field deformation.
      Returns a new bucket, with possibly a different voxel size from the input
      one.
  */
  carto::rc_ptr<BucketMap<Void> >
  ffdTransformBucket( const BucketMap<Void> & bck, SplineFfd & spline,
                      const AffineTransformation3d & affine
                        = AffineTransformation3d(),
                      const Point3df & vs = Point3df( 0., 0., 0. ) );

  /** Transform a graph using FFD vector field deformation.
      Changes the graph meshes and buckets in place, modifying the input graph.
  */
  void ffdTransformGraph( Graph & graph, SplineFfd & spline,
                          const AffineTransformation3d & affine
                            = AffineTransformation3d(),
                          const Point3df & vs = Point3df( 0., 0., 0. ) );

  /** FFD vector field transform for bundles in stream.
      This is a BundleListener / BundleProducer stream processing class
      which applies vector field deformation to bundle data. It can be
      typically connected to a BundleReader and a BundleWriter.
  */
  class BundleFFDTransformer : public BundleListener, public BundleProducer
  {
  public:
    BundleFFDTransformer( carto::rc_ptr<SplineFfd> deformation,
                          const AffineTransformation3d & affine );
    virtual ~BundleFFDTransformer();

    virtual void bundleStarted( const BundleProducer &, const BundleInfo & );
    virtual void bundleTerminated( const BundleProducer &, const BundleInfo & );
    virtual void fiberStarted( const BundleProducer &, const BundleInfo &,
                               const FiberInfo & );
    virtual void fiberTerminated( const BundleProducer &, const BundleInfo &,
                                  const FiberInfo & );
    virtual void newFiberPoint( const BundleProducer &, const BundleInfo &,
                                const FiberInfo &, const FiberPoint & );
    virtual void noMoreBundle( const BundleProducer & );

  private:
    carto::rc_ptr<SplineFfd> _deformation;
    const AffineTransformation3d & _affine;
    bool _idaffine;
  };

} // namespace aims

#endif
