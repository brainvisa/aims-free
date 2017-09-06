/* Copyright (C) 2000-2013 CEA
 *
 * This software and supporting documentation were developed by
 *     bioPICSEL
 *     CEA/DSV/I²BM/MIRCen/LMN, Batiment 61,
 *     18, route du Panorama
 *     92265 Fontenay-aux-Roses
 *     France
 */

#ifndef AIMS_PPDF_H
#define AIMS_PPDF_H

#include <aims/registration/ffd.h>
#include <aims/vector/vector.h>
#include <aims/math/bspline.h>
#include <cartobase/type/string_conversion.h>
#include <cmath>
#include <string>
#include <vector>


class ParzenProbDensFunction
{
  public:
    static const int PAD = 2;

    ParzenProbDensFunction(aims::SplineFfd& deformation,
                          bool is2dx = false,
                          bool is2dy = false,
                          bool is2dz = false,
                          short refBinNb = 64,
                          short testbinNb = 64);

    virtual  ~ParzenProbDensFunction();
    void      updateBinSizeAndMin(AimsData<short>& refImage,
                                 AimsData<short>&  testImage,
                                 bool              prepro = true );
    // add [lambda,kappa] point to the joint histogram
    // testlevel is used to better manage discretisation.
    bool      addContrib(int lambda, int kappa, double testLevel);
    bool      addDerContrib(int i,int j,int k,
                           int lambda, int kappa,double testLevel,
                           double coef, Point3dd &p);
    void      normalize(AimsData<double>& n); // unused
    void      zeroPdfAndDerPdf();
    /// Returns the mutual information score
    float     getMutualInfo();
    /// Returns the partial derivatives of MI w.r.t the FFD parameters.
    /// The results are put in v, a preallocated vector of size N_parameters
    void      getPartDer( std::vector<float>&  v );

    void      resizeParam( Point3d newDim );

    AimsData<double> &    dumpJPdf() { return _jpdf; };
    AimsData<Point3dd> &  dumpDerJPdf(int i,int j,int k)
                               {return _derjpdf(i,j,k);}
    int      getKappa( double r ) const;
    int      getLambda( double t) const;
    double   parzenDerivative(int k , double t);


    double spline3( double x ) const { return _spline(x); }
    double spline3derivative( double x ) const { return _spline.derivative(x); }

    //--- debug --------------------------------------------------------------
    void writeDebugJPdf( const std::string & filename ) const;

 private:
     AimsData<double>  _jpdf;  ///< joint density (refBinNb x testBinNb+2*PAD)
     AimsData<double>  _xpdf;  ///< reference marginal density (refBinNb)
     AimsData<double>  _ypdf;  ///< test marginal density (testBinNb+2*PAD)

     /// joint density derived w.r.t. each parameter
     /// size: (ffd.dimX x ffd.dimY x ffd.dimZ)
     /// -> size: (refBinNb x testBinNb+2*PAD)
     /// ---> size: 3 (param x/y/z)
     AimsData<AimsData<Point3dd> >  _derjpdf;

     double              _refDeltaBin;   ///< bin size for ref image
     double              _testDeltaBin;  ///< bin size for test image
     short               _refBinNb;      ///< bin count for ref image
     short               _testBinNb;     ///< bin count for test image
     short               _refMin;        ///< min value in ref image
     short               _testMin;       ///< min value in test image
     short               _refMax;        ///< max value in ref image
     short               _testMax;       ///< max value in test image

     double _alpha;  ///< joint histogram normalization term
     aims::TabulBSpline _spline;

     // 2d
     AimsVector<bool,3>  _is2d;  ///< ( size == 1 ) in each dimension
     long  _dimParam;            ///< nb of > 1 dimensions
};

inline void
ParzenProbDensFunction::zeroPdfAndDerPdf()
{
  _jpdf = 0.0;
  _xpdf = 0.0;
  _ypdf = 0.0;

  int i, j, k;

  ForEach3d(_derjpdf, i, j, k) {
    _derjpdf(i, j, k) = Point3dd(0.0);
  }
}


inline int
ParzenProbDensFunction::getKappa( double refLevel) const
{
  return int( std::floor( (refLevel - (double)_refMin) / _refDeltaBin ) );
}

inline int
ParzenProbDensFunction::getLambda( double testLevel) const
{
  return int( std::floor( (testLevel - (double)_testMin) / _testDeltaBin) );
}

inline bool
ParzenProbDensFunction::addContrib(int lambda, int kappa, double testval)
{
  // try an other way to deal with mask pixel
  if( kappa < 0 )
    kappa = 0;
  else if( kappa >= _refBinNb )
    kappa = _refBinNb - 1;
  if( lambda < 0 ) {
    lambda = 0;
    testval = _testMin;
  } else if( lambda >= _testBinNb ) {
    lambda = _testBinNb - 1;
    testval = _testMax;
  }

  if( kappa < 0 || kappa >= _refBinNb || lambda < 0 || lambda >= _testBinNb )
    return false;

  // Attention ici la spline sert de fenetre pas de resampler
  double dt = double(lambda + 0.5) - ((testval - (double)_testMin) / _testDeltaBin);
  int idf;

  if( dt >= 0.0 ) {
    idf = lambda - 2 + PAD;
    dt -= 2.0;
  } else {
    idf = lambda - 1 + PAD;
    dt -= 1.0;
  }
  ASSERT( dt >= -2.  && dt + 3. <= 2. );

  if( idf >= 0 && idf < _testBinNb + 2*PAD )
    _jpdf(kappa, idf) += (double) spline3(dt);
  if( idf+1 >= 0 && idf+1 < _testBinNb + 2*PAD )
    _jpdf(kappa, idf + 1) += (double) spline3(dt + 1);
  if( idf+2 >= 0 && idf+2 < _testBinNb + 2*PAD )
    _jpdf(kappa, idf + 2) += (double) spline3(dt + 2);
  if( idf+2 >= 0 && idf+2 < _testBinNb + 2*PAD )
    _jpdf(kappa, idf + 3) += (double) spline3(dt + 3);

  return true;
}


inline bool
ParzenProbDensFunction::addDerContrib(int i, int j, int k,
                                      int lambda, int kappa, double testval,
                                      double coef, Point3dd & p)
{
  // try an other way to deal with mask pixel
  if( kappa < 0 )
    kappa = 0;
  else if( kappa >= _refBinNb )
    kappa = _refBinNb - 1;
  if( lambda < 0 ) {
    lambda = 0;
    testval = _testMin;
  } else if( lambda >= _testBinNb ) {
    lambda = _testBinNb - 1;
    testval = _testMax;
  }


  if( kappa < 0 || kappa >= _refBinNb || lambda < 0 || lambda >= _testBinNb )
    return false;

  double dt = double(lambda + 0.5) - ((testval -(double)_testMin) / _testDeltaBin);
  int idf;
  if( dt >= 0.0 ) {
    idf = lambda - 2 + PAD;
    dt -= 2.0;
  } else {
    idf = lambda - 1 + PAD;
    dt -= 1.0;
  }
  ASSERT( dt >= -2.  && dt + 3. <= 2. );

  // /!\ changed
  AimsData<Point3dd> & df = _derjpdf(i, j, k);
  Point3dd cp = p * coef;

  // Term 1 * (Term0 * Term2 *t* Term3)
  if( idf >= 0 && idf < _testBinNb + 2*PAD )
    df(kappa, idf) += (double) spline3derivative(dt) * cp;
  if( idf+1 >= 0 && idf+1 < _testBinNb + 2*PAD )
    df(kappa, idf + 1) += (double) spline3derivative(dt + 1) * cp;
  if( idf+2 >= 0 && idf+2 < _testBinNb + 2*PAD )
    df(kappa, idf + 2) += (double) spline3derivative(dt + 2) * cp;
  if( idf+3 >= 0 && idf+3 < _testBinNb + 2*PAD )
    df(kappa, idf + 3) += (double) spline3derivative(dt + 3) * cp;

  return true;
}


inline double
ParzenProbDensFunction::parzenDerivative( int lambda, double testval )
{
  double dt = double(lambda+0.5) - ((testval-_testMin)/_testDeltaBin);
  return( spline3derivative( dt ) );
}

#endif
