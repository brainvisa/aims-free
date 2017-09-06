/* Copyright (C) 2000-2013 CEA
 *
 * This software and supporting documentation were developed by
 *     bioPICSEL
 *     CEA/DSV/I²BM/MIRCen/LMN, Batiment 61,
 *     18, route du Panorama
 *     92265 Fontenay-aux-Roses
 *     France
 */

#include <aims/registration/ffd.h>
#include <aims/data/data_g.h>
#include <aims/resampling/resampling_g.h>
#include <aims/math/bspline3.h>
#include <aims/math/bspline2.h>
#include <aims/io/io_g.h>
#include <cstdio>
#include <cmath>
#include <string>

using namespace aims;
using namespace std;
using namespace carto;

namespace aims {

//============================================================================
//   FFD TRANSFORMATION
//============================================================================

SplineFfd::SplineFfd( int dimX, int dimY, int dimZ,
                      float sizeX, float sizeY, float sizeZ ):
  _spline(3, 0),
  _ctrlPointDelta( dimX, dimY, dimZ )
{
  _ctrlPointDelta = Point3df(0., 0., 0.);
  _ctrlPointDelta.setSizeXYZT( sizeX, sizeY, sizeZ );
}

SplineFfd::SplineFfd( const SplineFfd & other ):
  _spline(3, 0)
{
  updateAllCtrlKnot(other._ctrlPointDelta);
}

SplineFfd::SplineFfd( const AimsData<Point3df> & other ):
  _spline(3, 0)
{
  updateAllCtrlKnot(other);
}

SplineFfd & SplineFfd::operator=( const SplineFfd & other )
{
  if( this != &other )
  {
    updateAllCtrlKnot(other._ctrlPointDelta);
  }
  return *this;
}

Point3df SplineFfd::getCtrlKnot( int nx, int ny, int nz ) const
{
  return _ctrlPointDelta( aims::mirrorCoeff(nx, dimX()),
                          aims::mirrorCoeff(ny, dimY()),
                          aims::mirrorCoeff(nz, dimZ()) );
}

void SplineFfd::updateCtrlKnot( int nx, int ny, int nz, const Point3df & s )
{
  _ctrlPointDelta( nx, ny, nz ) = s;
}

void SplineFfd::updateGridResolution( const AimsData<Point3df> & newGrid )
{
  // Ctrl Point Grid dimensions
  if( newGrid.dimX() != dimX() ||
      newGrid.dimY() != dimY() ||
      newGrid.dimZ() != dimZ() )
  {
    _ctrlPointDelta = AimsData<Point3df>( newGrid.dimX(),
                                          newGrid.dimY(),
                                          newGrid.dimZ() );
  }

  if( newGrid.sizeX() != sizeX() ||
      newGrid.sizeY() != sizeY() ||
      newGrid.sizeZ() != sizeZ() )
  {
    _ctrlPointDelta.setSizeXYZT( newGrid.sizeX(), newGrid.sizeY(), newGrid.sizeZ() );
  }
}

void SplineFfd::updateAllCtrlKnot( const AimsData<Point3df> & newCtrlKnotGrid )
{
  updateGridResolution( newCtrlKnotGrid );
  for( int k = 0; k < newCtrlKnotGrid.dimZ(); ++k )
  for( int j = 0; j < newCtrlKnotGrid.dimY(); ++j )
  for( int i = 0; i < newCtrlKnotGrid.dimX(); ++i )
    _ctrlPointDelta(i, j, k) = newCtrlKnotGrid(i, j, k);
}


void SplineFfd::updateAllCtrlKnotFromDeformation( const AimsData<Point3df> & newDeformationGrid )
{
  updateGridResolution( newDeformationGrid );

  // Spline interpolation of displacement vectors of control points
  for( int c = 0; c <= 2; ++c )
  {
    AimsData<float> def( newDeformationGrid.dimX(),
                         newDeformationGrid.dimY(),
                         newDeformationGrid.dimZ() );
    def = 0.;

    // Copy of coefficients to def
    for( int k = 0; k < def.dimZ(); ++k )
    for( int j = 0; j < def.dimY(); ++j )
    for( int i = 0; i < def.dimX(); ++i )
      def(i, j, k) = newDeformationGrid(i, j, k)[c];

    // Construction of spline coefficients for test image
    AimsData<double> splineCoeff;
    {
      CubicResampler<float> interpolator;
      splineCoeff = interpolator.getSplineCoef( def );
    }

    for( int k = 0; k < _ctrlPointDelta.dimZ(); ++k )
    for( int j = 0; j < _ctrlPointDelta.dimY(); ++j )
    for( int i = 0; i < _ctrlPointDelta.dimX(); ++i )
      _ctrlPointDelta(i, j, k)[c] = (float)splineCoeff(i, j, k);
  }
}

// This would be more precise if done in double
Point3dd SplineFfd::deformation( const Point3dd& pImage ) const
{
  Point3dd deformation(0., 0., 0.);
  Point3df fdef;
  Point3dd pSpline( mmToSplineVox(pImage) );

  Point3dl kSpline( (int)std::floor(pSpline[0]),
                    (int)std::floor(pSpline[1]),
                    (int)std::floor(pSpline[2]) );

  if( kSpline[0] < 0 || kSpline[0] >= dimX() ||
      kSpline[1] < 0 || kSpline[1] >= dimY() ||
      kSpline[2] < 0 || kSpline[2] >= dimZ() )
    return deformation;

  Point3dl kDown( ( isFlat(0) ? 0 : kSpline[0] - 1 ),
                  ( isFlat(1) ? 0 : kSpline[1] - 1 ),
                  ( isFlat(2) ? 0 : kSpline[2] - 1 ) );
  Point3dl kUp  ( ( isFlat(0) ? 0 : kSpline[0] + 2 ),
                  ( isFlat(1) ? 0 : kSpline[1] + 2 ),
                  ( isFlat(2) ? 0 : kSpline[2] + 2 ) );

  double bz, by, bx;
  int    cz, cy, cx;

  for( int k = kDown[2]; k <= kUp[2]; ++k )
  {
    bz = ( isFlat(2) ? 1. : spline3( pSpline[2] - k ) );
    cz  = aims::mirrorCoeff( k, dimZ() );
    for( int j = kDown[1]; j <= kUp[1]; ++j )
    {
      by = ( isFlat(1) ? 1. : spline3( pSpline[1] - j ) );
      cy  = aims::mirrorCoeff( j, dimY() );
      for( int i = kDown[0]; i <= kUp[0]; ++i )
      {
        bx = ( isFlat(0) ? 1. : spline3( pSpline[0] - i ) );
        cx  = aims::mirrorCoeff( i, dimX() );
        fdef = _ctrlPointDelta( cx, cy, cz ) * bx * by * bz;
        deformation[0] += fdef[0];
        deformation[1] += fdef[1];
        deformation[2] += fdef[2];
      }
    }
  }

  return deformation;
}

Point3dd SplineFfd::transformDouble( double x, double y, double z ) const
{
  Point3dd p(x, y, z);
  return p + deformation(p);
}


void SplineFfd::increaseResolution( const Point3d & addKnot )
{
  Point3d prevDim( dimX(), dimY(), dimZ() );
  Point3d newDim( prevDim + addKnot );
  cout << "Increase grid resolution: "
       << prevDim << " -> " << newDim
       << endl;

  // 1. we compute a volume of deformations in the new referential, but only
  // inside the image domain
  AimsData<Point3df> newDef( newDim[0], newDim[1], newDim[2] );
  newDef.setSizeXYZT( isFlat(0) ? sizeX() : double(dimX() - 1) / double(newDef.dimX() - 1) * sizeX(),
                      isFlat(1) ? sizeY() : double(dimY() - 1) / double(newDef.dimY() - 1) * sizeY(),
                      isFlat(2) ? sizeZ() : double(dimZ() - 1) / double(newDef.dimZ() - 1) * sizeZ() );
  newDef = Point3df(0.);
  Point3dd nd;

  for( int k = 0; k < newDef.dimZ(); ++k )
  for( int j = 0; j < newDef.dimY(); ++j )
  for( int i = 0; i < newDef.dimX(); ++i )
  {
    Point3dd p_mm( i * newDef.sizeX(),
                   j * newDef.sizeY(),
                   k * newDef.sizeZ() );
    nd = deformation( p_mm );
    newDef(i, j, k)[0] = nd[0];
    newDef(i, j, k)[1] = nd[1];
    newDef(i, j, k)[2] = nd[2];
  }

  // 2. we compute the corresponding spline coefficients and copy them into
  // the FFD coeff image. We then update all parameters
  updateAllCtrlKnotFromDeformation( newDef );
}

#if 0
void SplineFfd::inverseTransform()
{
  int x, y, z, i, j, k;

  // Save FFD object
  AimsData<Point3df> tmpPointDelta( _ctrlPointDelta.dimX(),
                                    _ctrlPointDelta.dimY(),
                                    _ctrlPointDelta.dimZ() );

  cout << "Structure copied 1: dX = " << _ctrlPointDelta.dimX()
       << "\tdY = " << _ctrlPointDelta.dimY()
       << "\tdZ = " << _ctrlPointDelta.dimZ() << endl;

  cout << "Structure copied 2: dX = " << dimX()
       << "\tdY = " << dimY()
       << "\tdZ = " << dimZ() << endl << endl;

  ForEach3d(tmpPointDelta, i, j, k)
    tmpPointDelta(i,j,k) = _ctrlPointDelta(i,j,k);

  //   Creation d un reseau de pts de controle etendu (bord de bord...)
  AimsData<Point3df> tmpPointDeltaBis( 1+1+this->dimX()+2+1,
                                       1+1+this->dimY()+2+1,
                                       1+1+this->dimZ()+2+1 );
  tmpPointDeltaBis(i,j,k) = Point3df( 0., 0., 0. );

  cout << "Grid size before: " << dimX() << "\t"
                               << dimY() << "\t"
                               << dimZ() << endl;
  cout << "Grid size after: " << tmpPointDeltaBis.dimX()
                              << "\t" << tmpPointDeltaBis.dimY()
                              << "\t" << tmpPointDeltaBis.dimZ() << endl << endl;

  // Report values
  for(k = 1; k <= dimZ(); ++k)
    for(j = 1; j <= dimY(); ++j)
      for(i = 1; i <= dimX(); ++i) {
        tmpPointDeltaBis(i+1, j+1, k+1) = _ctrlPointDelta(i, j, k);
      }

  // Before replacement of dimensions, we save precedings
  Point3d this_init;

  this_init[0] = dimX();
  this_init[1] = dimY();
  this_init[2] = dimZ();

  Point3df range =  Point3df( (float)_imageDimX / (float)(this_init[0] - 1),
                              (float)_imageDimY / (float)(this_init[1] - 1),
                              (float)_imageDimZ / (float)(this_init[2] - 1) );
  cout << "Range = " << range << endl << endl;

  Point3df dimension = Point3df( _imageDimX + 2 * range[0],
                                 _imageDimY + 2 * range[1],
                                 _imageDimZ + 2 * range[2] );

  cout << "Preceding image dimensions: " << _imageDimX << "\t"
                                         << _imageDimY << "\t"
                                         << _imageDimZ << "\t" << endl;
  cout << "New image dimensions: " << dimension << endl << endl;

  // FFD replacement
  _ctrlPointDelta = tmpPointDeltaBis;

  _imageToSplineFactor[0] = (float)(_ctrlPointDelta.dimX() - 4) / dimension[0] ;
  if ( _imageToSplineFactor[0] == 0.0 )
    _imageToSplineFactor[0] = 1E-20;
  _imageToSplineFactor[1] = (float)(_ctrlPointDelta.dimY() - 4) / dimension[1] ;
  if ( _imageToSplineFactor[1] == 0.0 )
    _imageToSplineFactor[1] = 1E-20;
  _imageToSplineFactor[2] = (float)(_ctrlPointDelta.dimZ() - 4) / dimension[2] ;
  if ( _imageToSplineFactor[2] == 0.0 )
    _imageToSplineFactor[2] = 1E-20;

  cout << "Control points number: " << dimX() << "\t"
                                    << dimY() << "\t"
                                    << dimZ() << endl << endl;

  // Calcul des coordonnees des points de ctrl
  string name_inverse;
  AimsData< Point3df > coordCtrlKnot(this_init[0],
                                     this_init[1],
                                     this_init[2]);

  AimsData< Point3d > origine(this_init[0],
                              this_init[1],
                              this_init[2]);

  AimsData< Point3df > inverseDisplacement(this_init[0],
                                           this_init[1],
                                           this_init[2]);

  AimsData< float > error(this_init[0],
                          this_init[1],
                          this_init[2]);

  float error_tmp;

  int cpt = 0;
  // Processing control points
  for(z = 0; z < this_init[2]; ++z)
    for(y = 0; y < this_init[1]; ++y)
      for(x = 0; x < this_init[0]; ++x)
      {
        Point3df coord;

        coord[0] = (x + 1) * range[0];
        coord[1] = (y + 1) * range[1];
        coord[2] = (z + 1) * range[2];
        coordCtrlKnot(x,y,z) = coord;

        ++cpt;
        cout << "Point " << cpt << "\t" << coord << endl;

        // Default inititialization
        inverseDisplacement(x, y, z) =  Point3df(dimension[0], dimension[1], dimension[2]);
        error(x, y, z) = float(dimension[0]);
      }

  cout << endl << endl;

  // Courses the image and minimize
  cout << "Coarse exploration: " << endl << endl;

  for(z = 0; z < int(dimension[2]); ++z)
  {
    cout << "z = " << z << endl;
    for(y = 0; y < int(dimension[1]); ++y)
      for(x = 0; x < int(dimension[0]); ++x)
      {
        Point3df p_displacement = deformation( Point3df(x,y,z) );
        Point3df p_ffd = apply( Point3df(x,y,z) );

        for(int zz = 0; zz < this_init[2]; ++zz)
          for(int yy = 0; yy < this_init[1]; ++yy)
            for(int xx = 0; xx < this_init[0]; ++xx)
            {
              error_tmp = sqrt( sqr( coordCtrlKnot(xx, yy, zz)[0] - p_ffd[0] ) +
                                sqr( coordCtrlKnot(xx, yy, zz)[1] - p_ffd[1] ) +
                                sqr( coordCtrlKnot(xx, yy, zz)[2] - p_ffd[2] ));

              if(error_tmp < error(xx, yy, zz))
              {
                error(xx, yy, zz) = error_tmp;
                inverseDisplacement(xx, yy, zz) = -p_displacement;
                origine(xx, yy, zz) = Point3d(x,y,z);
              }
            }
        }
  }

  // Refine inverse displacement
  // Possibility to use anisotropic factor ...
  float step = 0.1;
  int amplitude = (int)(.6 / step);
//   cout << "Pas fin d exploration = " << step << "\tamplitude = " << amplitude << endl << endl;

  cout << endl << "Fine exploration: " << endl << endl;

  for(int zz = 0; zz < this_init[2]; ++zz) {
    cout << "\b" << zz;
    for(int yy = 0; yy < this_init[1]; ++yy)
      for(int xx = 0; xx < this_init[0]; ++xx) {
        for(z = -amplitude; z <= amplitude; ++z)
          for(y = -amplitude; y <= amplitude; ++y)
            for(x = -amplitude; x <= amplitude; ++x)
            {
              Point3df p_displacement = deformation(
                                          Point3df(coordCtrlKnot(xx, yy, zz)[0] + step * x,
                                                   coordCtrlKnot(xx, yy, zz)[1] + step * y,
                                                   coordCtrlKnot(xx, yy, zz)[2] + step * z));

              Point3df p_ffd = apply(
                                 Point3df(origine(xx, yy, zz)[0] + step * x,
                                          origine(xx, yy, zz)[1] + step * y,
                                          origine(xx, yy, zz)[2] + step * z));

              error_tmp = sqrt( sqr( coordCtrlKnot(xx, yy, zz)[0] - p_ffd[0] ) +
                                sqr( coordCtrlKnot(xx, yy, zz)[1] - p_ffd[1] ) +
                                sqr( coordCtrlKnot(xx, yy, zz)[2] - p_ffd[2] ));

              if(error_tmp < error(xx, yy, zz))
              {
                error(xx, yy, zz) = error_tmp;
                inverseDisplacement(xx, yy, zz) = -p_displacement;
              }
            }
        }
    }

  // Displacement / coefficient
  for(z = 0; z < this_init[2]; ++z)
    for(y = 0; y < this_init[1]; ++y)
      for(x = 0; x < this_init[0]; ++x)
      {
        for(int c = 0; c < 3; ++c)
        {
          inverseDisplacement(x, y, z)[c] *= _imageToSplineFactor[c];
        }
      }

  // Spline interpolation of control points displacement vectors
  AimsData< Point3df > coefInvDisplacement(this_init[0], this_init[1], this_init[2]);

  for( int c = 0; c <= 2; c++ )
  {
    AimsData<float> tmp( 1 + this_init[0] + 2,
                         1 + this_init[1] + 2,
                         1 + this_init[2] + 2 );

    tmp = 0.;

    // Copy of coefficients to tmp
    for(k = 0; k < this_init[2]; ++k)
      for(j = 0; j < this_init[1]; ++j)
        for(i = 0; i < this_init[0]; ++i)
        {
          tmp(i + 1, j + 1, k + 1) = inverseDisplacement(i, j, k)[c];
        }

    // Construction of spline coefficients for test image
    AimsData< double > tmpReech;
    {
      CubicResampler<float> interpolator;
      tmpReech = interpolator.getSplineCoef( tmp );
    }

    for(k=0; k<this_init[2]; ++k)
      for(j=0; j<this_init[1]; ++j)
        for(i=0; i<this_init[0]; ++i)
        {
          coefInvDisplacement(i,j,k)[c] = (float)tmpReech(i+1,j+1,k+1);
        }
  }

  // Save inverse

//   sprintf( name_inverse.c_str(),
//            "ctrlshift_inv_%d%d%d",
//            (int)this->dimX()-2,
//            (int)this->dimY()-2,
//            (int)this->dimZ()-2 );
//
//   Writer< AimsData< Point3df > > winverse( name_inverse );
//   winverse << (*coefInvDisplacement);

//   Writer< AimsData< float > > werror( "error" );
//   werror << error;

  // Restore FFD
  _ctrlPointDelta = tmpPointDelta;

  _imageToSplineFactor[0] = (float)(_ctrlPointDelta.dimX() - 4) / _imageDimX ;
  if ( _imageToSplineFactor[0] == 0.0 )
    _imageToSplineFactor[0] = 1E-20;
  _imageToSplineFactor[1] = (float)(_ctrlPointDelta.dimY() - 4) / _imageDimY ;
  if ( _imageToSplineFactor[1] == 0.0 )
    _imageToSplineFactor[1] = 1E-20;
  _imageToSplineFactor[2] = (float)(_ctrlPointDelta.dimZ() - 4) / _imageDimZ ;
  if ( _imageToSplineFactor[2] == 0.0 )
    _imageToSplineFactor[2] = 1E-20;

}

void SplineFfd::estimateLocalDisplacement( const Point3df & VoxelSize)
{
  int i, j, k;

  AimsData< float > estim_tmp( _imageDimX, _imageDimY, _imageDimZ, 8 );
  estim_tmp.setSizeXYZT( VoxelSize[0], VoxelSize[1], VoxelSize[2], 1 );
  estim_tmp = 0.;

  for(k = 0; k < estim_tmp.dimZ(); ++k)
    for(j = 0; j < estim_tmp.dimY(); ++j)
      for(i = 0; i < estim_tmp.dimX(); ++i)
      {
        Point3df p = this->deformation( Point3df(i, j, k) );
        for(int c = 0; c < 3; ++c)
        {
          estim_tmp(i, j, k, c) = p[c];
        }
        float norm = sqrt( sqr(p[0]) + sqr(p[1]) + sqr(p[2]) );
        estim_tmp(i, j, k, 3) = norm;
      }

  // Process local variations
  for(k = 1; k < estim_tmp.dimZ() - 1; ++k)
    for(j = 1; j < estim_tmp.dimY() - 1; ++j)
      for(i = 1; i < estim_tmp.dimX() - 1; ++i)
      {
        estim_tmp(i, j, k, 4) = abs( estim_tmp(i - 1, j, k, 3) - estim_tmp(i + 1, j, k, 3) );
        estim_tmp(i, j, k, 5) = abs( estim_tmp(i, j - 1, k, 3) - estim_tmp(i, j + 1, k, 3) );
        estim_tmp(i, j, k, 6) = abs( estim_tmp(i, j, k - 1, 3) - estim_tmp(i, j, k + 1, 3) );
      }

   for(k = 1; k < estim_tmp.dimZ() - 1; ++k)
    for(j = 1; j < estim_tmp.dimY() - 1; ++j)
      for(i = 1; i < estim_tmp.dimX() - 1; ++i)
      {
        estim_tmp(i, j, k, 7) = sqrt( sqr( estim_tmp(i, j, k, 4))
                              + sqr( estim_tmp(i, j, k, 5))
                              + sqr( estim_tmp(i, j, k, 6)) );
      }

//   Writer< AimsData< float > > westim( "estimate3d.ima" );
//   westim << estim_tmp;
}
#endif

void SplineFfd::printControlPointsGrid() const
{
  for( int z = 0; z < _ctrlPointDelta.dimZ(); ++z ) {
    for( int y = 0; y < _ctrlPointDelta.dimY(); ++y ) {
      for( int x = 0; x < _ctrlPointDelta.dimX(); ++x ) {
        cout << "\t" << _ctrlPointDelta(x, y, z);
      }
      cout << endl ;
    }
    cout << endl << endl ;
  }
}

void SplineFfd::writeDebugCtrlKnots( const string & filename ) const
{
  AimsData<float> ctrlknots( dimX(), dimY(), dimZ(), 3 );
  ctrlknots.setSizeXYZT( sizeX(), sizeY(), sizeZ() );

  for( int k = 0; k < dimZ(); ++k )
  for( int j = 0; j < dimY(); ++j )
  for( int i = 0; i < dimX(); ++i )
  for( int c = 0; c < 3; ++c )
    ctrlknots( i, j, k, c ) = _ctrlPointDelta( i, j, k )[c];

  Writer<AimsData<float> > wcoef( filename );
  wcoef << ctrlknots;
}

void SplineFfd::writeDebugDeformations( const std::string & filename,
                                        int dimX, int dimY, int dimZ,
                                        float sizeX, float sizeY, float sizeZ ) const
{
  AimsData<float> def(dimX, dimY, dimZ, 3);
  def.setSizeXYZT( sizeX, sizeY, sizeZ );

  for( int z = 0; z < def.dimZ(); ++z )
  for( int y = 0; y < def.dimY(); ++y )
  for( int x = 0; x < def.dimX(); ++x )
  {
    Point3dd p = deformation( Point3dd( x * sizeX,
                                        y * sizeY,
                                        z * sizeZ ) );
    for( int c = 0; c < 3; ++c )
      def( x, y, z, c ) = p[c];
  }

  Writer<AimsData<float> > wima( filename );
  wima << def;
}

void SplineFfd::write( const string & filename ) const
{
  Writer<SplineFfd> w( filename );
  w << *this;
}


template <typename T>
FfdResampler<T>::~FfdResampler()
{
}

//============================================================================
//   S P L I N E   R E S A M P L E R
//============================================================================
template <class T, class C>
SplineFfdResampler<T, C>::~SplineFfdResampler()
{
}

template <class T, class C>
void SplineFfdResampler<T, C>::init()
{
  _samples = DataTypeInfo<T>::samples();
  _channelcoef.resize(_samples);
  _min.resize(_samples);
  _max.resize(_samples);
}

template <class T, class C>
SplineFfdResampler<T, C>::SplineFfdResampler(
    const SplineFfd & spline, T background ):
  CubicResampler<C>(),
  _transformation(spline),
  _background(background)
{
  init();
}

template <class T, class C>
SplineFfdResampler<T, C>::SplineFfdResampler(
    const SplineFfd & spline, Motion affine, T background ):
  CubicResampler<C>(),
  _transformation(spline),
  _affine(affine),
  _background(background)
{
  init();
}

template <class T, class C>
void SplineFfdResampler<T, C>::updateCoef( int t )
{
  if( t != _last_t )
  {
    ChannelSelector< AimsData<T>, AimsData<C> > dataselector;
    for ( uint8_t c = 0; c < _samples; ++c )
    {
      this->reset();
      const AimsData<C> & channel = dataselector.select(_ref, c);
      _channelcoef[c] = this->getSplineCoef( channel, t );
      _min[c] = channel.minimum();
      _max[c] = channel.maximum();
    }
    _last_t = t;
  }
}

template <class T, class C>
void SplineFfdResampler<T, C>::setRef(const AimsData<T> & ref)
{
  _ref = ref;
  _last_t = -1;
  updateCoef();
}

template <class T, class C>
Point3df SplineFfdResampler<T, C>::resample(
  const Point3df & output_location,
  T & output_value, int t )
{
  updateCoef(t);
  // output_location is in the output image referential (unit: mm)
  double    output_channel_value;
  double    bk3, bj3, bi3;
  int       ck, cj, ci;
  bool      aberrantvalue = false;
  Point3dd  p_ref( output_location[0], output_location[1], output_location[2] );

  // transform point
  if( !_affine.isIdentity() )
    p_ref = _affine.transform( p_ref );
  p_ref = _transformation.transform( p_ref );

  // Spline resampling

  Point3dd pv_ref( p_ref[0] / _ref.sizeX(),
                   p_ref[1] / _ref.sizeY(),
                   p_ref[2] / _ref.sizeZ() );

  Point3dl pi_ref( (int)(floor(pv_ref[0])),
                   (int)(floor(pv_ref[1])),
                   (int)(floor(pv_ref[2])) );

  Point3dl kDown( ( _transformation.isFlat(0) ? pi_ref[0] : pi_ref[0] - 1 ),
                  ( _transformation.isFlat(1) ? pi_ref[1] : pi_ref[1] - 1 ),
                  ( _transformation.isFlat(2) ? pi_ref[2] : pi_ref[2] - 1 ) );
  Point3dl kUp  ( ( _transformation.isFlat(0) ? pi_ref[0] : pi_ref[0] + 2 ),
                  ( _transformation.isFlat(1) ? pi_ref[1] : pi_ref[1] + 2 ),
                  ( _transformation.isFlat(2) ? pi_ref[2] : pi_ref[2] + 2 ) );

  if( pi_ref[0] >= 0 && pi_ref[0] < _ref.dimX() &&
      pi_ref[1] >= 0 && pi_ref[1] < _ref.dimY() &&
      pi_ref[2] >= 0 && pi_ref[2] < _ref.dimZ() )
  {
    // Loop on channels to resample data in each one
    for ( int c = 0; c < _samples; ++c )
    {
      output_channel_value = 0;

      // Integrate spline values in 3 dimensions
      // pffd[0] - i, pffd[1] - j, pffd[2] - k,  vary in the range [-2, 2[
      for( int k = kDown[2]; k <= kUp[2]; ++k ) {
        bk3 = ( _transformation.isFlat(2) ? 1. : _transformation.spline3(pv_ref[2] - k) );
        ck = aims::mirrorCoeff( k, _ref.dimZ() );
        for( int j = kDown[1]; j <= kUp[1]; ++j ) {
          bj3 = ( _transformation.isFlat(1) ? 1. : _transformation.spline3(pv_ref[1] - j) );
          cj = aims::mirrorCoeff( j, _ref.dimY() );
          for( int i = kDown[0]; i <= kUp[0]; ++i ) {
            bi3 = ( _transformation.isFlat(0) ? 1. : _transformation.spline3(pv_ref[0] - i) );
            ci = aims::mirrorCoeff( i, _ref.dimX() );
            output_channel_value += (_channelcoef[c])(ci, cj, ck) * bi3 * bj3 * bk3;
          }
        }
      }

      if( output_channel_value < _min[c] )
        output_channel_value = _min[c];
      else if( output_channel_value > _max[c] )
        output_channel_value = _max[c];
      _channelselector.set(output_value, c, output_channel_value);
    }

  }
  else
    output_value = _background;

  return Point3df( p_ref[0], p_ref[1], p_ref[2] );
}

//============================================================================
//   N E A R E S T N E I G H B O R  R E S A M P L E R
//============================================================================
template <class T, class C>
NearestNeighborFfdResampler<T, C>::~NearestNeighborFfdResampler()
{}


template <class T, class C>
void NearestNeighborFfdResampler<T, C>::init()
{}

template <class T, class C>
NearestNeighborFfdResampler<T, C>::NearestNeighborFfdResampler(
    const SplineFfd & spline, T background ):
  NearestNeighborResampler<C>(),
  _transformation(spline),
  _background(background)
{
  init();
}

template <class T, class C>
NearestNeighborFfdResampler<T, C>::NearestNeighborFfdResampler(
    const SplineFfd & spline, Motion affine, T background ):
  NearestNeighborResampler<C>(),
  _transformation(spline),
  _affine(affine),
  _background(background)
{
  init();
}

template <class T, class C>
void NearestNeighborFfdResampler<T, C>::setRef(const AimsData<T> & ref) {
  _ref = ref;
}

template <class T, class C>
Point3df NearestNeighborFfdResampler<T, C>::resample(
  const Point3df & output_location,
  T & output_value, int t)
{
  // output_location is in the output image referential (unit: mm)
  Point3df p_ref = output_location;

  if (! _affine.isIdentity() )
    p_ref = _affine.transform( p_ref );
  p_ref = _transformation.transform( p_ref );

  // Spline resampling
  Point3dl pi_ref( (int)(p_ref[0] / _ref.sizeX() + .5),
                   (int)(p_ref[1] / _ref.sizeY() + .5),
                   (int)(p_ref[2] / _ref.sizeZ() + .5) ) ;

  if( pi_ref[0] >= 0 && pi_ref[0] < _ref.dimX() &&
      pi_ref[1] >= 0 && pi_ref[1] < _ref.dimY() &&
      pi_ref[2] >= 0 && pi_ref[2] < _ref.dimZ() )
  {
    output_value = _ref( pi_ref[0], pi_ref[1], pi_ref[2], t );
  }
  else
    output_value = _background;

  return p_ref;
}

template class SplineFfdResampler<int8_t>;
template class SplineFfdResampler<uint8_t>;
template class SplineFfdResampler<int16_t>;
template class SplineFfdResampler<uint16_t>;
template class SplineFfdResampler<int32_t>;
template class SplineFfdResampler<uint32_t>;
template class SplineFfdResampler<float>;
template class SplineFfdResampler<double>;
template class SplineFfdResampler<AimsRGB, AimsRGB::ChannelType>;
template class SplineFfdResampler<AimsRGBA, AimsRGBA::ChannelType>;

template class NearestNeighborFfdResampler<int8_t>;
template class NearestNeighborFfdResampler<uint8_t>;
template class NearestNeighborFfdResampler<int16_t>;
template class NearestNeighborFfdResampler<uint16_t>;
template class NearestNeighborFfdResampler<int32_t>;
template class NearestNeighborFfdResampler<uint32_t>;
template class NearestNeighborFfdResampler<float>;
template class NearestNeighborFfdResampler<double>;
template class NearestNeighborFfdResampler<AimsRGB, AimsRGB::ChannelType>;
template class NearestNeighborFfdResampler<AimsRGBA, AimsRGBA::ChannelType>;

} // namespace aims
