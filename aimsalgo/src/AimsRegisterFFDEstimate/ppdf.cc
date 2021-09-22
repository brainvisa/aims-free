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
#include <cmath>                                             // std::log

using namespace aims;
using namespace carto;
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
  VolumeRef< Point3dd > tmp(refBinNb, testBinNb+2*PAD);
  tmp = Point3dd(.0,.0,.0);

  int i, j, k;
  vector<int> dim = _derjpdf->getSize();

  for( k=0; k<dim[2]; ++k )
    for( j=0; j<dim[1]; ++j )
      for( i=0; i<dim[0]; ++i )
      {
        _derjpdf(i, j, k) = tmp.deepcopy();
        _derjpdf(i, j, k) = Point3dd((double)i, (double)j, (double)k);
      }
}


void
ParzenProbDensFunction::normalize( rc_ptr<Volume<double> > & n )
{
  int i, j, k, x, y;
  vector<int> dim = _derjpdf->getSize();

  for( k=0; k<dim[2]; ++k )
    for( j=0; j<dim[1]; ++j )
      for( i=0; i<dim[0]; ++i )
        if( n->at(i,j,k) )
          for( y=0; y<_derjpdf(i, j, k)->getSizeY(); ++y )
            for( x=0; x<_derjpdf(i, j, k)->getSizeX(); ++x )
              _derjpdf(i, j, k)(x,y) /= n->at(i, j, k);
}

float
ParzenProbDensFunction::getMutualInfo()
{
  double mi=0.0;
  int x, y, levelsx, levelsy;
  double log2 = std::log(2);
  vector<int> dim = _jpdf->getSize();

  // normalize joint density histogram (i.e. compute alpha)
  _alpha = 0.;
  for( y=0; y<dim[1]; ++y )
    for( x=0; x<dim[0]; ++x )
      _alpha += _jpdf( x, y );
  _alpha = (double)1. / _alpha;
  for( y=0; y<dim[1]; ++y )
    for( x=0; x<dim[0]; ++x )
      _jpdf( x, y ) *= _alpha;

  // compute marginal density histograms
  levelsx = _jpdf->getSizeX();
  levelsy = _jpdf->getSizeY();
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
  int levelsx = _derjpdf(0,0,0)->getSizeX();
  int levelsy = _derjpdf(0,0,0)->getSizeY();

  ASSERT( (v.size() % _dimParam) == 0); // Multiple de triplet
  ASSERT( levelsx ==  _jpdf->getSizeY()- 2*PAD ); // Dimension derPart coherentes

  double log2 = std::log(2);

  // //---< compute d[p_y]/d[mu_i] = SUM[x](d[p_xy]/d[mu_i])
  // VolumeRef<VolumeRef<Point3dd> > derypdf( _derjpdf.dimX(), _derjpdf.dimY(), _derjpdf.dimZ() );
  // VolumeRef<Point3dd> tmp( _testBinNb + 2*PAD );
  // tmp = Point3dd(0.);
  // int pi, pj, pk;
  // ForEach3d(derypdf, pi, pj, pk)
  // {
  //   derypdf(pi, pj, pk) = tmp.deepcopy();
  //   VolumeRef<Point3dd> & dpT_d_mu_i = derypdf(pi, pj, pk);
  //   for( int j=0; j<dpT_d_mu_i.dimX(); ++j )
  //     for( int i=0; i<_refBinNb; ++i )
  //       dpT_d_mu_i[j] += _derjpdf(pi, pj, pk)(i, j);
  // }
  // //--->


  double p_xy, p_y/*, p_x*/;
  Point3dd t, dp_dmu_i_xy, dpT_dmu_i_y;

  int ind = 0;
  for( int k = 0; k < _derjpdf->getSizeZ(); ++k )
  for( int j = 0; j < _derjpdf->getSizeY(); ++j )
  for( int i = 0; i < _derjpdf->getSizeX(); ++i )
  {
    const VolumeRef< Point3dd > & dp_dmu_i  = _derjpdf(i, j, k);

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
  _derjpdf = VolumeRef<VolumeRef<Point3dd> >( newDim[0],
                                              newDim[1],
                                              newDim[2]);

  VolumeRef<Point3dd> tmp(_refBinNb, _testBinNb + 2 * PAD);
  tmp = Point3dd(.0,.0,.0);

  int i, j, k;
  vector<int> dim = _derjpdf->getSize();
  for( int k = 0; k < dim[2]; ++k )
    for( int j = 0; j < dim[1]; ++j )
      for( int i = 0; i < dim[0]; ++i )
      {
        // Instanciate new Volume
        _derjpdf(i, j, k) = tmp.deepcopy();

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
ParzenProbDensFunction::updateBinSizeAndMin(rc_ptr<Volume<short> >& refImage,
                                            rc_ptr<Volume<short> >& testImage,
                                            bool             prepro )
{
  // Find Ref second minimum
  _refMax = refImage->max();
  if( prepro )
  {
    _refMin = _refMax;
    short refTrueMin = refImage->min();
    vector<int> dim = refImage->getSize();
    for( int k=0; k<dim[2]; ++k )
      for( int j=0; j<dim[1]; ++j )
        for( int i=0; i<dim[0]; ++i )
        {
          if( refImage->at(i, j, k) != refTrueMin
              && refImage->at(i, j, k) < _refMin )
            _refMin = refImage->at(i, j, k);
        }
    _refMin -= 1;
  }
  else
    _refMin = refImage->min();

  // Find test second minimum
  _testMax = testImage->max();
  if( prepro )
  {
    _testMin = _testMax;
    short testTrueMin = testImage->min();
    vector<int> dim = testImage->getSize();
    for( int k=0; k<dim[2]; ++k )
      for( int j=0; j<dim[1]; ++j )
        for( int i=0; i<dim[0]; ++i )
        {
          if( testImage->at(i, j, k) != testTrueMin
              && testImage->at(i, j, k) < _testMin )
            _testMin = testImage->at(i, j, k);
        }
    _testMin -= 1;
  }
  else
    _testMin = testImage->min();

  // Find Ref 99% max
  if( prepro )
  {
    std::vector<long> refHisto( _refMax - _refMin, 0 );
    vector<int> dim = refImage->getSize();
    for( int k=0; k<dim[2]; ++k )
      for( int j=0; j<dim[1]; ++j )
        for( int i=0; i<dim[0]; ++i )
        {
          short val = refImage->at(i, j, k);
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
    vector<int> dim = testImage->getSize();
    for( int k=0; k<dim[2]; ++k )
      for( int j=0; j<dim[1]; ++j )
        for( int i=0; i<dim[0]; ++i )
        {
          short val = testImage->at(i, j, k);
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
    vector<int> dim = refImage->getSize();
    for( int k=0; k<dim[2]; ++k )
      for( int j=0; j<dim[1]; ++j )
        for( int i=0; i<dim[0]; ++i )
        {
          if( refImage->at(i, j, k) < _refMin )
            refImage->at(i, j, k) = _refMin;
          else if( refImage->at(i, j, k) > _refMax )
            refImage->at(i, j, k) = _refMax;
        }
  }

  // Replace Test levels
  if( prepro )
  {
    vector<int> dim = testImage->getSize();
    for( int k=0; k<dim[2]; ++k )
      for( int j=0; j<dim[1]; ++j )
        for( int i=0; i<dim[0]; ++i )
        {
          if( testImage->at(i, j, k) < _testMin )
            testImage->at(i, j, k) = _testMin;
          else if( testImage->at(i, j, k) > _testMax )
            testImage->at(i, j, k) = _testMax;
        }
  }

  // Compute delta
  _refDeltaBin = double( _refMax - _refMin + 1 ) / _jpdf->getSizeX();
  _testDeltaBin = double( _testMax - _testMin + 1 ) / ( _jpdf->getSizeY() - 2*PAD );

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
  Writer<Volume<double> > w( filename );
  w.write( *_jpdf );
}

// instantiate a fancy Volume class

#include <cartodata/volume/volume_d.h>

template class carto::Volume< VolumeRef<Point3dd> >;
template class carto::VolumeProxy< VolumeRef<Point3dd> >;
template class carto::VolumeRef< VolumeRef<Point3dd> >;
template class carto::Creator<carto::Volume< VolumeRef<Point3dd> > >;

