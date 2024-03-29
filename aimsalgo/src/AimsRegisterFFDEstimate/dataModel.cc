/* Copyright (C) 2000-2013 CEA
 *
 * This software and supporting documentation were developed by
 *     bioPICSEL
 *     CEA/DSV/I²BM/MIRCen/LMN, Batiment 61,
 *     18, route du Panorama
 *     92265 Fontenay-aux-Roses
 *     France
 */

// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include "dataModel.h"
#include <aims/resampling/cubicresampler.h>

using namespace aims;
using namespace carto;
using namespace std;

// Copied from SplineResampler
// This allows to get mirror coefficients when in the border
int getFold( int i, int size )
{

  i = std::abs( i );

  if ( i < size )
    return i;
  if ( size == 1 )
    return 0;

  int size2 = ( size << 1 ) - 2;
  ldiv_t modOp = std::ldiv( i, size2 );
  return ( modOp.rem < size ) ? modOp.rem : ( size2 - modOp.rem );

}

DataModel::~DataModel()
{
}

DataModel::DataModel( BucketMap< Void >&                s,
                      VolumeRef< short >                r,
                      VolumeRef< short >                t,
                      AffineTransformation3d&           m,
                      SplineFfd&                        d,
                      ParzenProbDensFunction&           pdf,
                      bool                              prepro ):
    _current( ((t->getSizeX() != 1) + (t->getSizeY() != 1) + (t->getSizeZ() != 1))
              * d.getSizeX() * d.getSizeY() * d.getSizeZ(), 0.),
    _deformation( d ),
    _rigid( m ),
    _pdf( pdf ),
    _spline(3, 1),
    _partDer( ((t->getSizeX() != 1) + (t->getSizeY() != 1) + (t->getSizeZ() != 1))
              * d.getSizeX() * d.getSizeY() * d.getSizeZ(), 0.),
    _reference( r ),
    _test( t ),
    _is2d( t->getSizeX() == 1, t->getSizeY() == 1, t->getSizeZ() == 1 ),
    _dimParam( (t->getSizeX() != 1) + (t->getSizeY() != 1) + (t->getSizeZ() != 1) ),
    _contrib( r->getSizeX(), r->getSizeY(), r->getSizeZ() )
{
  // debug
  _contrib->header().setProperty( "voxel_size", r->getVoxelSize() );

  // Compute histo parameter + replace min max values
  VolumeRef<short> ar( r ); // FIXME remove this after updateBinSizeAndMin changes
  VolumeRef<short> at( t );
  _pdf.updateBinSizeAndMin( ar, at, prepro );

  // Construction des coefficients splines pour l'image test
  CubicResampler<short> interpolator;
  _splineTest = interpolator.getSplineCoef( t );

  //
  // Initialize evaluation points
  //

  vector<float> vs = r->getVoxelSize();
  this->_selection.setSizeXYZT( vs[0],
                                vs[1],
                                vs[2],
                                vs[3] );

  vector<int> sdim = _splineTest->getSize();

  if (s.size() > 0)
  {
    // Use input bucket points
    BucketMap<Void>::Bucket bucket = s.begin()->second;
    BucketMap<Void>::Bucket::iterator ib, eb = bucket.end();
    vector<float> tvs = _test->getVoxelSize();

    for( ib = bucket.begin(); ib != eb; ++ib )
    {
      Point3df p_ref_mm( float(ib->first[0]) * s.sizeX(),
                         float(ib->first[1]) * s.sizeY(),
                         float(ib->first[2]) * s.sizeZ() );
      Point3df p_test_mm = m.transform( p_ref_mm );
      Point3df p_test_vox( p_test_mm[0] / tvs[0],
                           p_test_mm[1] / tvs[1],
                           p_test_mm[2] / tvs[2] );

      // On saute les bords car interpolation imparfaite
      if( 0 <= p_test_vox[0] && p_test_vox[0] < sdim[0] &&
          0 <= p_test_vox[1] && p_test_vox[1] < sdim[1] &&
          0 <= p_test_vox[2] && p_test_vox[2] < sdim[2] )
      {
        this->_selection.insert(ib->first, p_test_mm);
      }
    }
  }
  else
  {
    // No available points in bucket map
    // Default use of the reference image
    int i, j, k;
    vector<int> dim = ar->getSize();
    for( k=0; k<dim[2]; ++k )
      for( j=0; j<dim[1]; ++j )
        for( i=0; i<dim[0]; ++i )
        {
        Point3d a(i, j, k);
        vector<float> vs = _reference->getVoxelSize();
        Point3df p_ref_mm( float(a[0]) * vs[0],
                          float(a[1]) * vs[1],
                          float(a[2]) * vs[2] );
        Point3df p_test_mm = m.transform( p_ref_mm );
        vector<float> tvs = _test->getVoxelSize();
        Point3df p_test_vox( p_test_mm[0] / tvs[0],
                            p_test_mm[1] / tvs[1],
                            p_test_mm[2] / tvs[2] );
        // On saute les bords car interpolation imparfaite
        if( 0 <= p_test_vox[0] && p_test_vox[0] < sdim[0] &&
            0 <= p_test_vox[1] && p_test_vox[1] < sdim[1] &&
            0 <= p_test_vox[2] && p_test_vox[2] < sdim[2] )
        {
          this->_selection.insert(a, p_test_mm);
        }
      }
  }

  cout << "Number of evaluation points: " << this->_selection[0].size() << endl;

}

void DataModel::resizeParam(int dimParam, int dimX, int dimY, int dimZ)
{
  int l = dimParam*dimX*dimY*dimZ;
  cout  << "Resizing _current, dim = " << l  << " ... " << flush;
  _current.resize( l );
  _partDer.resize( l );

  for (int i=0; i<l; ++i)
  {
    _current[i] = 0.;
    _partDer[i] = 0.;
  }
  cout << "done" << endl;

  _pdf.resizeParam( Point3d(dimX, dimY, dimZ) );
}

void DataModel::update( vector<float>& p )
{
  //  if (p.size() != 1000) return;

  vector<int> dims = _deformation.getSize();

  // Methode ASSERT a remplacer par une exception
  ASSERT( p.size() == size_t(_dimParam * dims[0] * dims[1] * dims[2]));
  ASSERT( p.size() == _current.size() );

  _current = p;

  for( int k = 0, pi = 0; k < dims[2]; k++ )
  for( int j = 0; j < dims[1]; j++ )
  for( int i = 0; i < dims[0]; i++ )
  {
    Point3df cp;
    cp[0] = ( _is2d[0] ? 0. : p[pi++] );
    cp[1] = ( _is2d[1] ? 0. : p[pi++] );
    cp[2] = ( _is2d[2] ? 0. : p[pi++] );
    _deformation.updateCtrlKnot( i, j, k, cp );
  }

  evalCrit();
}


void DataModel::initBound( const Point3df & maxdef )
{
  _maxDef = maxdef;
  updateBound();
}

void DataModel::updateBound()
{
  vector<int> dims = _deformation.getSize();

  _lowerBound.resize( _dimParam * dims[0] * dims[1] * dims[2] );
  _upperBound.resize( _dimParam * dims[0] * dims[1] * dims[2] );
  Point3df def;
  vector<float> vs = _deformation.getVoxelSize();
  def[0] = ( _maxDef[0] >= 0 ? _maxDef[0] : vs[0] );
  def[1] = ( _maxDef[1] >= 0 ? _maxDef[1] : vs[1] );
  def[2] = ( _maxDef[2] >= 0 ? _maxDef[2] : vs[2] );

  std::cout << "max def: " << def << std::endl;

  int l = 0;
  for( int k = 0; k < dims[2]; ++k )
  for( int j = 0; j < dims[1]; ++j )
  for( int i = 0; i < dims[0]; ++i )
  for( int c = 0; c < 3; ++c )
  {
    if( !_deformation.isFlat(c) ) {
      _lowerBound[l] = _deformation.getCtrlKnot(i, j, k)[c] - def[c];
      _upperBound[l] = _deformation.getCtrlKnot(i, j, k)[c] + def[c];
      ++l;
    }
  }
}

void  DataModel::evalCrit()
{
  _pdf.zeroPdfAndDerPdf();
  BucketMap<Point3df>::Bucket b = _selection.begin()->second;
  BucketMap<Point3df>::Bucket::iterator sib, seb = b.end();
  _contrib = 0;

  for (sib = b.begin(); sib != seb; sib++) {

    // variable kappa decrit la variable aletoire Reference
    // variable lambda decrit la variable aletoire Test

    // Calcul de p(l,k ; mu)
    //----------------------


    // Calcul de d/d mu_i p(l,k ; mu)
    //-------------------------------
    // = Term0 x Term1 x ( Term2 *T* Term3 ) (*T* is a matrixproduct between vectors)
    // Terme 0 : est un kroenecker en kappa
    // Terme 1 : d bSpline3() (derive de la fenetre de Parzen)
    // Terme 2 : Gradient spatial de l'image test.
    //           Calcule dans boucle precedente
    //           Contenu dand gradTest
    // Terme 3 : Matrice (compresse donnant la der de ffd en mu_i)

    if( evalCrit( sib->second, _reference(sib->first) ) )
      _contrib(sib->first) = _reference(sib->first) ;
  } // fin du parcours des selVoxel

  _mi = _pdf.getMutualInfo() ;
  _pdf.getPartDer( _partDer );
}

inline
bool  DataModel::evalCrit(const Point3df & pTest, const short & value)
{
  // p = g(p_rig) i.e. transformed point
  Point3df pDef = _deformation.transform( pTest );
  vector<float> vs = _test->getVoxelSize();
  double pX = pDef[0] / vs[0];
  double pY = pDef[1] / vs[1];
  double pZ = pDef[2] / vs[2];

  //--------------------------------------//
  // Compute the interpoled Test value    //
  // Compute the Test image derivative    //
  //   (i.e. Term 2)
  //--------------------------------------//

  // t = Test*( p ) i.e. interpoled test value in p
  double t = 0.0;
  // B-spline transform coefficient for the Test image
  double st;
  // Coefficient indices
  int ck, cj, ci;
  // cubic B-spline values in x-xi, y-yi, z-zi
  double bk3 = 1., bj3 = 1., bi3 = 1.;
  // cubic B-spline derivative values in x-xi, y-yi, z-zi
  double dbk3 = 1., dbj3 = 1., dbi3 = 1.;
  // gradTestX = -[dTest/dX](p) i.e. Derived Test image in p
  double gradTestX = 0., gradTestY = 0., gradTestZ = 0.;

  // Bounds for interpolation
  // Point3d kSplineTestLow( max((int)(std::floor(pX)) - 1, -1),
  //                         max((int)(std::floor(pY)) - 1, -1),
  //                         max((int)(std::floor(pZ)) - 1, -1) );
  // Point3d kSplineTestUpp( min((int)(std::floor(pX)) + 2, _splineTest->getSizeX() + 1),
  //                         min((int)(std::floor(pY)) + 2, _splineTest->getSizeY() + 1),
  //                         min((int)(std::floor(pZ)) + 2, _splineTest->getSizeZ() + 1) );
  Point3d kSplineTestLow( (int)(std::floor(pX)) - 1,
                          (int)(std::floor(pY)) - 1,
                          (int)(std::floor(pZ)) - 1 );
  Point3d kSplineTestUpp( (int)(std::floor(pX)) + 2,
                          (int)(std::floor(pY)) + 2,
                          (int)(std::floor(pZ)) + 2 );
  // 2d cases
  kSplineTestLow[0] = ( _is2d[0] ? 0 : kSplineTestLow[0] );
  kSplineTestLow[1] = ( _is2d[1] ? 0 : kSplineTestLow[1] );
  kSplineTestLow[2] = ( _is2d[2] ? 0 : kSplineTestLow[2] );
  kSplineTestUpp[0] = ( _is2d[0] ? 0 : kSplineTestUpp[0] );
  kSplineTestUpp[1] = ( _is2d[1] ? 0 : kSplineTestUpp[1] );
  kSplineTestUpp[2] = ( _is2d[2] ? 0 : kSplineTestUpp[2] );


  vector<int> tdim = _splineTest->getSize();
  // resample test image
  for ( int k = kSplineTestLow[2]; k <= kSplineTestUpp[2]; ++k )
  {
    bk3 = ( _is2d[2] ? 1. : spline3(pZ - k) );
    dbk3 = ( _is2d[2] ? 0. : spline3derivative(pZ - k) );
    ck = getFold( k, tdim[2] );
    for ( int j = kSplineTestLow[1]; j <= kSplineTestUpp[1]; ++j )
    {
      bj3 = ( _is2d[1] ? 1. : spline3(pY - j) );
      dbj3 = ( _is2d[1] ? 0. : spline3derivative(pY - j) );
      cj = getFold( j, tdim[1] );
      for ( int i = kSplineTestLow[0]; i <= kSplineTestUpp[0]; ++i )
      {
        bi3 = ( _is2d[0] ? 1. : spline3(pX - i) );
        dbi3 = ( _is2d[0] ? 0. : spline3derivative(pX - i) );
        ci = getFold( i, tdim[0] );
        st = _splineTest( ci, cj, ck );

        t += st * bi3 * bj3 * bk3;
        gradTestX -= st * dbi3 * bj3  * bk3;
        gradTestY -= st * bi3  * dbj3 * bk3;
        gradTestZ -= st * bi3  * bj3  * dbk3;
      }
    }
  }
  vector<float> tvs = _splineTest->getVoxelSize();
  gradTestX /= tvs[0];
  gradTestY /= tvs[1];
  gradTestZ /= tvs[2];

  int kappa  = _pdf.getKappa( value );
  int lambda = _pdf.getLambda( t );
  if( _pdf.addContrib( lambda, kappa, t ) )
  {

    //----------------------------------------//
    // Compute deformation derivative         //
    //   (i.e. Term 3)                        //
    // and does matrix product with Term 2    //
    //----------------------------------------//

    Point3dd pTestd( pTest[0], pTest[1], pTest[2] );
    Point3dd pFFD = _deformation.ffdCoord( pTestd );
    double pffdX = pFFD[0], pffdY = pFFD[1], pffdZ = pFFD[2];

    // Il ne devrait pas y avoir de pb de bord par construction
    Point3dd gradTest(gradTestX, gradTestY, gradTestZ);

    // Point3d kFFDTestLow( max((int)(std::floor(pFFD[0])) - 1, 0),
    //                      max((int)(std::floor(pFFD[1])) - 1, 0),
    //                      max((int)(std::floor(pFFD[2])) - 1, 0) );
    // Point3d kFFDTestUpp( min((int)(std::floor(pFFD[0])) + 2, _deformation.dimX() - 1),
    //                      min((int)(std::floor(pFFD[1])) + 2, _deformation.dimY() - 1),
    //                      min((int)(std::floor(pFFD[2])) + 2, _deformation.dimZ() - 1) );
    Point3d kFFDTestLow( (int)(std::floor(pFFD[0])) - 1,
                         (int)(std::floor(pFFD[1])) - 1,
                         (int)(std::floor(pFFD[2])) - 1 );
    Point3d kFFDTestUpp( (int)(std::floor(pFFD[0])) + 2,
                         (int)(std::floor(pFFD[1])) + 2,
                         (int)(std::floor(pFFD[2])) + 2 );
    // 2d cases
    kFFDTestLow[0] = ( _is2d[0] ? 0 : kFFDTestLow[0] );
    kFFDTestLow[1] = ( _is2d[1] ? 0 : kFFDTestLow[1] );
    kFFDTestLow[2] = ( _is2d[2] ? 0 : kFFDTestLow[2] );
    kFFDTestUpp[0] = ( _is2d[0] ? 0 : kFFDTestUpp[0] );
    kFFDTestUpp[1] = ( _is2d[1] ? 0 : kFFDTestUpp[1] );
    kFFDTestUpp[2] = ( _is2d[2] ? 0 : kFFDTestUpp[2] );

    for( int k = kFFDTestLow[2]; k <= kFFDTestUpp[2]; ++k )
    {
      ck = getFold(k, _deformation.dimZ());
      bk3 = ( _is2d[2] ? 1. : spline3(pffdZ - k) );
      for( int j = kFFDTestLow[1]; j <= kFFDTestUpp[1]; ++j )
      {
        cj = getFold(j, _deformation.dimY());
        bj3 = ( _is2d[1] ? 1. : spline3(pffdY - j) );
        for( int i = kFFDTestLow[0]; i <= kFFDTestUpp[0]; ++i )
        {
          ci = getFold(i, _deformation.dimX());
          bi3 = ( _is2d[0] ? 1. : spline3(pffdX - i) );
          _pdf.addDerContrib( ci, cj, ck,
                              lambda, kappa, t,
                              bk3 * bj3 * bi3,
                              gradTest );
        }
      }
    }

    return true;

  } else return false;

}


#include <aims/io/io_g.h>

void DataModel::writeDebugTestSpline( const std:: string & filename ) const
{
  Writer<VolumeRef<double> > w( filename );
  w << _splineTest;
}

void DataModel::writeDebugContrib( const std::string & filename ) const
{
  Writer<VolumeRef<short> > w( filename );
  w << _contrib;
}

void DataModel::writeDebugDerMI( const std::string & filename ) const
{
  vector<int> dims = _deformation.getSize();
  VolumeRef<float> der( dims[0],
                        dims[1],
                        dims[2], 3 );
  der.header().setProperty( "voxel_size", _deformation.getVoxelSize() );
  der = 0.;

  int ind = 0;
  for( int k = 0; k < dims[2]; ++k )
  for( int j = 0; j < dims[1]; ++j )
  for( int i = 0; i < dims[0]; ++i )
  for( int c = 0; c < 3; ++c )
  {
    if( !_is2d[c] )
      der(i, j, k, c) = _partDer[ind++];
  }

  Writer<VolumeRef<float> > w( filename );
  w.write( der );

}

void DataModel::writeDebugCtrlKnots( const std::string & filename ) const
{
  _deformation.writeDebugCtrlKnots( filename );
}

void DataModel::writeDebugDeformations( const std::string & filename,
                                  int dimX, int dimY, int dimZ,
                                  float sizeX, float sizeY, float sizeZ ) const
{
  _deformation.writeDebugDeformations( filename, dimX, dimY, dimZ,
                                       sizeX, sizeY, sizeZ );
}

void DataModel::writeDebugJPdf( const std::string & filename ) const
{
  _pdf.writeDebugJPdf( filename );
}

