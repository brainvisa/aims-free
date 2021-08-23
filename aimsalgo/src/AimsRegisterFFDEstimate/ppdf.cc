/* Copyright (C) 2000-2013 CEA
 *
 * This software and supporting documentation were developed by
 *     bioPICSEL
 *     CEA/DSV/I²BM/MIRCen/LMN, Batiment 61,
 *     18, route du Panorama
 *     92265 Fontenay-aux-Roses
 *     France
 */


#include "ppdf.h"
#include <aims/data/data.h>
#include <aims/math/mathelem.h>                              // aims::MathUtil
#include <cmath>                                                   // std::log

using namespace aims;
using namespace std;

ParzenProbDensFunction::~ParzenProbDensFunction()
{
}

ParzenProbDensFunction::ParzenProbDensFunction(
  SplineFfd & d,
  bool is2dx,
  bool is2dy,
  bool is2dz,
  short refBinNb,
  short testBinNb
):
  _jpdf(refBinNb, testBinNb+2*PAD),
  _xpdf(refBinNb),
  _ypdf(testBinNb + 2*PAD),
  _derjpdf(d.dimX(), d.dimY(), d.dimZ()),
  _refBinNb(refBinNb),
  _testBinNb(testBinNb),
  _spline(3, 1),
  _is2d( is2dx, is2dy, is2dz ),
  _dimParam(!is2dx + !is2dy + !is2dz)
{
  _jpdf = 0.0;
  _xpdf = 0.0;
  _ypdf = 0.0;

  // Zone pour stocker les derives
  AimsData< Point3dd > tmp(refBinNb, testBinNb+2*PAD);
  tmp = Point3dd(.0,.0,.0);

  int i, j, k;
  ForEach3d(_derjpdf, i, j, k)
  {
    _derjpdf(i, j, k) = tmp.clone();
    _derjpdf(i, j, k) = Point3dd((double)i, (double)j, (double)k);
  }
}


void
ParzenProbDensFunction::normalize(AimsData<double>& n)
{
  int i, j, k, x, y;
  ForEach3d(_derjpdf, i, j, k)
    if( n(i,j,k) )
      ForEach2d( _derjpdf(i, j, k), x, y)
        _derjpdf(i, j, k)(x,y) /= n(i, j, k);
}

float
ParzenProbDensFunction::getMutualInfo()
{
  double mi=0.0;
  int x, y, levelsx, levelsy;
  double log2 = std::log(2);

  // normalize joint density histogram (i.e. compute alpha)
  _alpha = 0.;
  ForEach2d( _jpdf, x, y )
    _alpha += _jpdf( x, y );
  _alpha = (double)1. / _alpha;
  ForEach2d( _jpdf, x, y )
    _jpdf( x, y ) *= _alpha;

  // compute marginal density histograms
  levelsx = _jpdf.dimX();
  levelsy = _jpdf.dimY();
  for ( y = 0; y < levelsy; y++ )
    for ( x = 0; x < levelsx; x++ )
    {
      _xpdf(x) += _jpdf(x, y);
      _ypdf(y) += _jpdf(x, y);
    }

  // compute MI
  // MI = SUM[ jpdf * log( jpdf / (xpdf*ypdf) ) ]
  for ( y = 0; y < levelsy; y++ )
    for ( x = 0; x < levelsx; x++ )
      if ( _ypdf( y ) && _jpdf( x, y ) && _xpdf( x ) ) {
        mi += _jpdf( x, y ) * log( _jpdf( x, y ) / ( _xpdf( x ) * _ypdf( y ) ) );
      }
  mi /= log2;

  return float( -mi );
}


void
ParzenProbDensFunction::getPartDer( vector<float>&  v )
{
  int levelsx = _derjpdf(0,0,0).dimX();
  int levelsy = _derjpdf(0,0,0).dimY();

  ASSERT( (v.size() % _dimParam) == 0); // Multiple de triplet
  ASSERT( levelsx ==  _jpdf.dimY()- 2*PAD ); // Dimension derPart coherentes

  double log2 = std::log(2);

  // //---< compute d[p_y]/d[mu_i] = SUM[x](d[p_xy]/d[mu_i])
  // AimsData<AimsData<Point3dd> > derypdf( _derjpdf.dimX(), _derjpdf.dimY(), _derjpdf.dimZ() );
  // AimsData<Point3dd> tmp( _testBinNb + 2*PAD );
  // tmp = Point3dd(0.);
  // int pi, pj, pk;
  // ForEach3d(derypdf, pi, pj, pk)
  // {
  //   derypdf(pi, pj, pk) = tmp.clone();
  //   AimsData<Point3dd> & dpT_d_mu_i = derypdf(pi, pj, pk);
  //   for( int j=0; j<dpT_d_mu_i.dimX(); ++j )
  //     for( int i=0; i<_refBinNb; ++i )
  //       dpT_d_mu_i[j] += _derjpdf(pi, pj, pk)(i, j);
  // }
  // //--->


  double p_xy, p_y/*, p_x*/;
  Point3dd t, dp_dmu_i_xy, dpT_dmu_i_y;

  int ind = 0;
  for( int k = 0; k < _derjpdf.dimZ(); ++k )
  for( int j = 0; j < _derjpdf.dimY(); ++j )
  for( int i = 0; i < _derjpdf.dimX(); ++i )
  {
    const AimsData< Point3dd > & dp_dmu_i  = _derjpdf(i, j, k);
    // const AimsData< Point3dd > & dpT_dmu_i = derypdf(i, j, k);

    t = Point3dd(0.0, 0.0, 0.0);
    for ( int y = 0; y < levelsy; y++ )
    for ( int x = 0; x < levelsx; x++ )
    {
        dp_dmu_i_xy = dp_dmu_i(x, y);
        // dpT_dmu_i_y = dpT_dmu_i( y );
        p_xy = _jpdf( x, y);
        p_y  = _ypdf( y );
        // p_x  = _xpdf( x );
        if( (dp_dmu_i_xy != Point3dd(0.) ) &&
            // (dpT_dmu_i_y != Point3dd(0.) ) &&
            p_xy && /*p_x &&*/ p_y )
        {
          // t -= dp_dmu_i_xy * (log( p_xy / (p_y * p_x) ) + 1.) -
          //      dpT_dmu_i_y * p_xy / p_y;
          t -= dp_dmu_i_xy * log( p_xy / p_y );
        }
    }
    t /= log2;

    if( !_is2d[0] )  v[ind++] = float( _alpha * t[0] / _testDeltaBin );
    if( !_is2d[1] )  v[ind++] = float( _alpha * t[1] / _testDeltaBin );
    if( !_is2d[2] )  v[ind++] = float( _alpha * t[2] / _testDeltaBin );
  }
}

void ParzenProbDensFunction::resizeParam( Point3d newDim )
{
  cout  << "Resizing DerJpdf, dim = " << newDim << " ... " << flush;

  // Zone pour stocker les deives
  _derjpdf = AimsData<AimsData<Point3dd> >( newDim[0],
                                            newDim[1],
                                            newDim[2]);

  AimsData<Point3dd> tmp(_refBinNb, _testBinNb + 2 * PAD);
  tmp = Point3dd(.0,.0,.0);

  int i, j, k;
  ForEach3d(_derjpdf, i, j, k)
  {
    // Instanciate new AimsData
    _derjpdf(i, j, k) = tmp.clone();

    // Initialize Point3dd
    _derjpdf(i, j, k) = Point3dd((double)i, (double)j, (double)k);
  }

  cout << "done" << endl;
}


// To account for masked image, we actually used (second minimum - 1) of the
// images as minimum for histogram creation. (The second minimum is the
// smallest value bigger than the minimum).
// When the dynamic is "normal", 2ndmin -1 should be close to min
// When the image is masked, and there is thus a big gap in the dynamic,
// mask pixels should fall in a "negative" histogram bin.
void
ParzenProbDensFunction::updateBinSizeAndMin(AimsData<short>& refImage,
                                            AimsData<short>& testImage,
                                            bool             prepro )
{
  // Find Ref second minimum
  _refMax = refImage.maximum();
  if( prepro )
  {
    _refMin = _refMax;
    short refTrueMin = refImage.minimum();
    for( int k=0; k<refImage.dimZ(); ++k )
      for( int j=0; j<refImage.dimY(); ++j )
        for( int i=0; i<refImage.dimX(); ++i )
        {
          if( refImage(i, j, k) != refTrueMin && refImage(i, j, k) < _refMin )
            _refMin = refImage(i, j, k);
        }
    _refMin -= 1;
  }
  else
    _refMin = refImage.minimum();

  // Find test second minimum
  _testMax = testImage.maximum();
  if( prepro )
  {
    _testMin = _testMax;
    short testTrueMin = testImage.minimum();
    for( int k=0; k<testImage.dimZ(); ++k )
      for( int j=0; j<testImage.dimY(); ++j )
        for( int i=0; i<testImage.dimX(); ++i )
        {
          if( testImage(i, j, k) != testTrueMin && testImage(i, j, k) < _testMin )
            _testMin = testImage(i, j, k);
        }
    _testMin -= 1;
  }
  else
    _testMin = testImage.minimum();

  // Find Ref 99% max
  if( prepro )
  {
    std::vector<long> refHisto( _refMax - _refMin, 0 );
    for( int k=0; k<refImage.dimZ(); ++k )
      for( int j=0; j<refImage.dimY(); ++j )
        for( int i=0; i<refImage.dimX(); ++i )
        {
          short val = refImage(i, j, k);
          if( val > _refMin )
            refHisto[ val - _refMin - 1 ] += 1;
        }
    long refHistoSize = MathUtil<long>::sum( refHisto.begin(), refHisto.end() );
    long refAccumulate = 0;
    while( _refMax - _refMin - 1 >= 0 && (double)refAccumulate / refHistoSize < 0.01 )
    {
      refAccumulate += refHisto[_refMax - _refMin - 1];
      _refMax -= 1;
    }
    _refMax += 1;
    refHisto.clear();
  }

  // Find Test 99% max*
  if( prepro )
  {
    std::vector<long> testHisto( _testMax - _testMin, 0 );
    for( int k=0; k<testImage.dimZ(); ++k )
      for( int j=0; j<testImage.dimY(); ++j )
        for( int i=0; i<testImage.dimX(); ++i )
        {
          short val = testImage(i, j, k);
          if( val > _testMin )
            testHisto[ val - _testMin - 1 ] += 1;
        }
    long testHistoSize = MathUtil<long>::sum( testHisto.begin(), testHisto.end() );
    long testAccumulate = 0;
    while( _testMax - _testMin - 1 >= 0 && (double)testAccumulate / testHistoSize < 0.01 )
    {
      testAccumulate += testHisto[_testMax - _testMin - 1];
      _testMax -= 1;
    }
    _testMax += 1;
  }

  // Replace Ref levels
  if( prepro )
  {
    for( int k=0; k<refImage.dimZ(); ++k )
      for( int j=0; j<refImage.dimY(); ++j )
        for( int i=0; i<refImage.dimX(); ++i )
        {
          if( refImage(i, j, k) < _refMin )
            refImage(i, j, k) = _refMin;
          else if( refImage(i, j, k) > _refMax )
            refImage(i, j, k) = _refMax;
        }
  }

  // Replace Test levels
  if( prepro )
  {
    for( int k=0; k<testImage.dimZ(); ++k )
      for( int j=0; j<testImage.dimY(); ++j )
        for( int i=0; i<testImage.dimX(); ++i )
        {
          if( testImage(i, j, k) < _testMin )
            testImage(i, j, k) = _testMin;
          else if( testImage(i, j, k) > _testMax )
            testImage(i, j, k) = _testMax;
        }
  }

  // Compute delta
  _refDeltaBin = double( _refMax - _refMin + 1 ) / _jpdf.dimX();
  _testDeltaBin = double( _testMax - _testMin + 1 ) / ( _jpdf.dimY() - 2*PAD );

  cout << "Test  Max   " << _testMax << endl
       << "      Min   " << _testMin << endl
       << "      Delta " << _testDeltaBin << endl
       << "Ref   Max   " << _refMax << endl
       << "      Min   " << _refMin << endl
       << "      Delta " << _refDeltaBin << endl
       << endl;

}

#include <aims/io/io_g.h>
void ParzenProbDensFunction::writeDebugJPdf( const std::string & filename ) const
{
  Writer<AimsData<double> > w( filename );
  w << _jpdf;
}

// instantiate a fancy Volume class

#include <cartodata/volume/volume_d.h>

template class carto::Volume< AimsData<Point3dd> >;
template class carto::VolumeProxy< AimsData<Point3dd> >;
template class carto::VolumeRef< AimsData<Point3dd> >;
template class carto::Creator<carto::Volume< AimsData<Point3dd> > >;

