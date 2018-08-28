/* Copyright (C) 2000-2013 CEA
 *
 * This software and supporting documentation were developed by
 *     bioPICSEL
 *     CEA/DSV/I²BM/MIRCen/LMN, Batiment 61,
 *     18, route du Panorama
 *     92265 Fontenay-aux-Roses
 *     France
 */

#ifndef _AIMSDATAMODEL_H_
#define _AIMSDATAMODEL_H_

//#include "ffd.h"
#include "ppdf.h"
#include <aims/registration/ffd.h>
#include <aims/data/data_g.h>
#include <aims/utility/converter_bucket.h>
#include <aims/utility/utility_g.h>
#include <aims/io/motionR.h>
#include <aims/resampling/motion.h>
#include <aims/math/bspline.h>
#include <vector>
#include <iterator>

class DataModel
{
  public:
    /// DataModel constructor
    /// \param s    Bucket of reference image points to be used for MI
    ///             estimation. If none is given, All reference image points
    ///             are used.
    /// \param r    Reference image (target for registration)
    /// \param t    Test image (source for registration)
    /// \param m    Pre-computed affine transformation (dir: target->source)
    /// \param d    FFD transformation source->m(target)
    /// \param pdf  Parzen density function
    /// \param path        Path for write_file
    /// \param write_file  Write spline coefficient image as
    ///                    "path/dumpSplineTest.ima"
    DataModel( aims::BucketMap< Void >&            s,
                carto::VolumeRef< short >          r,
                carto::VolumeRef< short >          t,
                Motion&                            m,
                aims::SplineFfd&                    d,
                ParzenProbDensFunction&            pdf,
                bool                               prepro  = true );

    virtual ~DataModel();

    /// Resize parameter vector
    void              resizeParam(int dimParam, int dimX, int dimY, int dimZ);
    /// update FFD parameters
    /// Paramters of the SplinsFfd (FFd transformation) are set with values
    /// contained in p.
    /// \param p  new parameter values
    /// \param path  path for write_file
    /// \param write_file
    void              update( std::vector<float>& p );
    /// Get the current value of Mutual Information score.
    float             mutInfo() { return( _mi );}
    /// Get dMI/dParam
          std::vector<float>&  derMutInfo()       { return ( _partDer ); }
    const std::vector<float>&  derMutInfo() const { return ( _partDer ); }
          std::vector<float>&  param()            { return ( _current ); }
    const std::vector<float>&  param() const      { return ( _current ); }

    void initBound( const Point3df & maxdef = Point3df(-1, -1, -1) );
    void updateBound();
    std::vector<float>&    getLowerBound() { return _lowerBound; }
    std::vector<float>&    getUpperBound() { return _upperBound; }

    /// For each evaluation point, compute its contribution to MI and its
    /// derivative.
    /// Then compute the final MI score and its partial derivatives w.r.t.
    /// the FFD parameters.
    void              evalCrit();
    /// Compute the contribution of point p_rig.
    bool              evalCrit( const Point3df & p_rig, const short & value );

    // debug
    void              writeDebugCtrlKnots( const std::string & filename ) const;
    void              writeDebugDeformations( const std::string & filename,
                                  int dimX, int dimY, int dimZ,
                                  float sizeX, float sizeY, float sizeZ ) const;
    void              writeDebugTestSpline( const std::string & filename ) const;
    void              writeDebugContrib( const std::string & filename ) const;
    void              writeDebugJPdf( const std::string & filename ) const;
    void              writeDebugDerMI( const std::string & filename ) const;

    double spline3( double x ) const { return _spline(x); }
    double spline3derivative( double x ) const { return _spline.derivative(x); }
  private:
    std::vector<float>         _current;       ///< Param
    carto::VolumeRef<double>   _splineTest;    ///< B-Spline coefficients for the test image
    aims::SplineFfd&            _deformation;   ///< FFD transformation
    Motion&                    _rigid;         ///< Pre computed affine motion
    ParzenProbDensFunction&    _pdf;
    /// Points in which to compute MI
    /// keys   are reference points in voxels
    /// values are test points in mm
    aims::BucketMap<Point3df>  _selection;
    aims::TabulBSpline         _spline;

    float                   _mi;            ///< Mutual information score
    std::vector<float>      _partDer;       ///< dMI/dParam
    std::vector<float>      _upperBound;    ///< parameters bound for optimization
    std::vector<float>      _lowerBound;    ///< parameters bound for optimization
    carto::VolumeRef<int16_t> & _reference;     ///< reference image
    carto::VolumeRef<int16_t> & _test;          ///< test image
    Point3df                _maxDef;        ///< maximum deformation allowed in mm

    // 2d case
    AimsVector<bool,3>      _is2d;          ///< dimension in X/Y/Z directions
    long                    _dimParam;      ///< FFD dimension (1d/2d/3d)

    // debug
    carto::VolumeRef<short> _contrib;
};

#endif
