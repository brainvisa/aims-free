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
#include <aims/graph/graphmanip.h>
#include <graph/graph/graph.h>
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

FfdTransformation::FfdTransformation( int dimX, int dimY, int dimZ,
                      float sizeX, float sizeY, float sizeZ ):
  _ctrlPointDelta( dimX, dimY, dimZ ), _dimx( dimX ), _dimy( dimY ),
  _dimz( dimZ ), _vsx( sizeX ), _vsy( sizeY ), _vsz( sizeZ ),
  _flatx( dimX == 1 ), _flaty( dimY == 1 ), _flatz( dimZ == 1 )
{
  _ctrlPointDelta = Point3df(0., 0., 0.);
  _ctrlPointDelta.setSizeXYZT( sizeX, sizeY, sizeZ );
}

FfdTransformation::FfdTransformation( const FfdTransformation & other ):
  _dimx( other._dimx ), _dimy( other._dimy ),
  _dimz( other._dimz ), _vsx( other._vsx ), _vsy( other._vsy ),
  _vsz( other._vsz ),
  _flatx( other._flatx ), _flaty( other._flaty ), _flatz( other._flatz )
{
  updateAllCtrlKnot(other._ctrlPointDelta);
}

FfdTransformation::FfdTransformation( const AimsData<Point3df> & other ):
  _dimx( other.dimX() ), _dimy( other.dimY() ),
  _dimz( other.dimZ() ), _vsx( other.sizeX() ), _vsy( other.sizeY() ),
  _vsz( other.sizeZ() ),
  _flatx( _dimx == 1 ), _flaty( _dimy == 1 ), _flatz( _dimz == 1 )
{
  updateAllCtrlKnot(other);
}

FfdTransformation & FfdTransformation::operator=( const FfdTransformation & other )
{
  if( this != &other )
  {
    updateAllCtrlKnot(other._ctrlPointDelta);
  }
  return *this;
}

Point3df FfdTransformation::getCtrlKnot( int nx, int ny, int nz ) const
{
  return _ctrlPointDelta( aims::mirrorCoeff(nx, dimX()),
                          aims::mirrorCoeff(ny, dimY()),
                          aims::mirrorCoeff(nz, dimZ()) );
}

void FfdTransformation::updateCtrlKnot( int nx, int ny, int nz, const Point3df & s )
{
  _ctrlPointDelta( nx, ny, nz ) = s;
}


void FfdTransformation::updateDimensions()
{
  _dimx = _ctrlPointDelta.dimX();
  _dimy = _ctrlPointDelta.dimY();
  _dimz = _ctrlPointDelta.dimZ();
  _vsx = _ctrlPointDelta.sizeX();
  _vsy = _ctrlPointDelta.sizeY();
  _vsz = _ctrlPointDelta.sizeZ();
  _flatx = _dimx == 1;
  _flaty = _dimy == 1;
  _flatz = _dimz == 1;
}


void FfdTransformation::updateGridResolution( const AimsData<Point3df> & newGrid )
{
  // Ctrl Point Grid dimensions
  if( newGrid.dimX() != _dimx ||
      newGrid.dimY() != _dimy ||
      newGrid.dimZ() != _dimz )
  {
    _ctrlPointDelta = AimsData<Point3df>( newGrid.dimX(),
                                          newGrid.dimY(),
                                          newGrid.dimZ() );
  }

  if( newGrid.sizeX() != _vsx ||
      newGrid.sizeY() != _vsy ||
      newGrid.sizeZ() != _vsz )
  {
    _ctrlPointDelta.setSizeXYZT( newGrid.sizeX(), newGrid.sizeY(), newGrid.sizeZ() );
  }
  updateDimensions();
}

void FfdTransformation::updateAllCtrlKnot( const AimsData<Point3df> & newCtrlKnotGrid )
{
  updateGridResolution( newCtrlKnotGrid );
  int dx = newCtrlKnotGrid.dimX(), dy = newCtrlKnotGrid.dimY(),
    dz = newCtrlKnotGrid.dimZ();
  for( int k = 0; k < dz; ++k )
  for( int j = 0; j < dy; ++j )
  for( int i = 0; i < dx; ++i )
    _ctrlPointDelta(i, j, k) = newCtrlKnotGrid(i, j, k);
}


void FfdTransformation::updateAllCtrlKnotFromDeformation( const AimsData<Point3df> & newDeformationGrid )
{
  updateGridResolution( newDeformationGrid );

  // Spline interpolation of displacement vectors of control points
  for( int c = 0; c <= 2; ++c )
  {
    AimsData<float> def( newDeformationGrid.dimX(),
                         newDeformationGrid.dimY(),
                         newDeformationGrid.dimZ() );
    def = 0.;

    int dx = def.dimX(), dy = def.dimY(), dz = def.dimZ();
    // Copy of coefficients to def
    for( int k = 0; k < dz; ++k )
    for( int j = 0; j < dy; ++j )
    for( int i = 0; i < dx; ++i )
      def(i, j, k) = newDeformationGrid(i, j, k)[c];

    // Construction of spline coefficients for test image
    AimsData<double> splineCoeff;
    {
      CubicResampler<float> interpolator;
      splineCoeff = interpolator.getSplineCoef( def );
    }

    for( int k = 0; k < dz; ++k )
    for( int j = 0; j < dy; ++j )
    for( int i = 0; i < dx; ++i )
      _ctrlPointDelta(i, j, k)[c] = (float)splineCoeff(i, j, k);
  }
}

Point3dd FfdTransformation::deformation(const Point3dd& p_mm) const
{
  return _deformation(p_mm);
}

void FfdTransformation::increaseResolution( const Point3d & addKnot )
{
  Point3d prevDim( dimX(), dimY(), dimZ() );
  Point3d newDim( prevDim + addKnot );
  cout << "Increase grid resolution: "
       << prevDim << " -> " << newDim
       << endl;

  // 1. we compute a volume of deformations in the new referential, but only
  // inside the image domain
  AimsData<Point3df> newDef( newDim[0], newDim[1], newDim[2] );
  newDef.setSizeXYZT( _flatx ? sizeX() : double(dimX() - 1) / double(newDef.dimX() - 1) * sizeX(),
                      _flaty ? sizeY() : double(dimY() - 1) / double(newDef.dimY() - 1) * sizeY(),
                      _flatz ? sizeZ() : double(dimZ() - 1) / double(newDef.dimZ() - 1) * sizeZ() );
  newDef = Point3df(0.);
  Point3dd nd;

  float vsx = newDef.sizeX(), vsy = newDef.sizeY(), vsz = newDef.sizeZ();
  int dx = newDef.dimX(), dy = newDef.dimY(), dz = newDef.dimZ();

  for( int k = 0; k < dz; ++k )
  for( int j = 0; j < dy; ++j )
  for( int i = 0; i < dx; ++i )
  {
    Point3dd p_mm( i * vsx,
                   j * vsy,
                   k * vsz );
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
void FfdTransformation::inverseTransform()
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

void FfdTransformation::estimateLocalDisplacement( const Point3df & VoxelSize)
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

void FfdTransformation::printControlPointsGrid() const
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

void FfdTransformation::writeDebugCtrlKnots( const string & filename ) const
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

void FfdTransformation::writeDebugDeformations( const std::string & filename,
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

void FfdTransformation::write( const string & filename ) const
{
  Writer<FfdTransformation> w( filename );
  w << *this;
}



//============================================================================
//   FFD SPLINE RESAMPLED TRANSFORMATION
//============================================================================

SplineFfd::SplineFfd( int dimX, int dimY, int dimZ,
                      float sizeX, float sizeY, float sizeZ ):
  FfdTransformation( dimX, dimY, dimZ, sizeX, sizeY, sizeZ ),
  _spline(3, 0)
{
  // Set _mirrorcoef(vec)?[xyz]
  updateDimensions();
}

SplineFfd::SplineFfd( const SplineFfd & other ):
  FfdTransformation( other ),
  _spline(3, 0)
{
  // Set _mirrorcoef(vec)?[xyz]
  updateDimensions();
}

SplineFfd::SplineFfd( const AimsData<Point3df> & other ):
  FfdTransformation( other ),
  _spline(3, 0)
{
  // Set _mirrorcoef(vec)?[xyz]
  updateDimensions();
}

SplineFfd & SplineFfd::operator=( const SplineFfd & other )
{
  FfdTransformation::operator = ( other );
  return *this;
}

void SplineFfd::updateDimensions()
{
  FfdTransformation::updateDimensions();
  /* 26/6/2018 (Yael)
   * I went back to computing mirrored indices on the fly because it is hard
   * to know in advance how many will be necessary.
   */
  // int i;
  // _mirrorcoefvecx.resize( _dimx + 3 );
  // _mirrorcoefvecy.resize( _dimy + 3 );
  // _mirrorcoefvecz.resize( _dimz + 3 );
  // _mirrorcoefx = &_mirrorcoefvecx[1];
  // _mirrorcoefy = &_mirrorcoefvecy[1];
  // _mirrorcoefz = &_mirrorcoefvecz[1];
  // for( i=-1; i<_dimx + 2; ++i )
  //   _mirrorcoefx[i] = aims::mirrorCoeff( i, _dimx );
  // for( i=-1; i<_dimy + 2; ++i )
  //   _mirrorcoefy[i] = aims::mirrorCoeff( i, _dimy );
  // for( i=-1; i<_dimz + 2; ++i )
  //   _mirrorcoefz[i] = aims::mirrorCoeff( i, _dimz );
}

Point3dd SplineFfd::transformDouble( double x, double y, double z ) const
{
  Point3dd p(x, y, z);
  return p + deformation_private(p);
}

// This would be more precise if done in double
Point3dd SplineFfd::deformation_private( const Point3dd& pImage ) const
{
  Point3dd deformation(0., 0., 0.);
  Point3df fdef;
  Point3dd pSpline( mmToSplineVox(pImage) );

  Point3dl kSpline( (int)std::floor(pSpline[0]),
                    (int)std::floor(pSpline[1]),
                    (int)std::floor(pSpline[2]) );

  /* 26/6/2018 (Yael)
   * I removed this test because sometime the input point is outside of the
   * FOV but the input + deformation falls back inside the FOV.
   * Now, the deformation is always computed (this might add some unnecessary
   * computations in some cases, but this is more safe)
   */
  // if( kSpline[0] < 0 || kSpline[0] >= dimX() ||
  //     kSpline[1] < 0 || kSpline[1] >= dimY() ||
  //     kSpline[2] < 0 || kSpline[2] >= dimZ() )
  //   return deformation;

  Point3dl kDown( ( _flatx ? 0 : kSpline[0] - 1 ),
                  ( _flaty ? 0 : kSpline[1] - 1 ),
                  ( _flatz ? 0 : kSpline[2] - 1 ) );
  Point3dl kUp  ( ( _flatx ? 0 : kSpline[0] + 2 ),
                  ( _flaty ? 0 : kSpline[1] + 2 ),
                  ( _flatz ? 0 : kSpline[2] + 2 ) );

  double bz, by, bx, byz;
  int    cz, cy, cx;

  for( int k = kDown[2]; k <= kUp[2]; ++k )
  {
    bz = ( _flatz ? 1. : spline3( pSpline[2] - k ) );
    cz  = aims::mirrorCoeff(k, _dimz);
    for( int j = kDown[1]; j <= kUp[1]; ++j )
    {
      by = ( _flaty ? 1. : spline3( pSpline[1] - j ) );
      cy  = aims::mirrorCoeff(j, _dimy);
      byz = bz * by;
      for( int i = kDown[0]; i <= kUp[0]; ++i )
      {
        bx = ( _flatx ? 1. : spline3( pSpline[0] - i ) );
        cx  = aims::mirrorCoeff(i, _dimx);
        fdef = _ctrlPointDelta( cx, cy, cz ) * bx * byz;
        deformation[0] += fdef[0];
        deformation[1] += fdef[1];
        deformation[2] += fdef[2];
      }
    }
  }

  return deformation;
}


//============================================================================
//   FFD TRILINEAR RESAMPLED TRANSFORMATION
//============================================================================

TrilinearFfd::TrilinearFfd( int dimX, int dimY, int dimZ,
                            float sizeX, float sizeY, float sizeZ ):
  FfdTransformation( dimX, dimY, dimZ, sizeX, sizeY, sizeZ )
{
}

TrilinearFfd::TrilinearFfd( const TrilinearFfd & other ):
  FfdTransformation( other )
{
}

TrilinearFfd::TrilinearFfd( const AimsData<Point3df> & other ):
  FfdTransformation( other )
{
}

TrilinearFfd & TrilinearFfd::operator=( const TrilinearFfd & other )
{
  FfdTransformation::operator = ( other );
  return *this;
}

Point3dd TrilinearFfd::deformation_private( const Point3dd& pImage ) const
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

  Point3dl kUp  ( kSpline[0] + 1,
                  kSpline[1] + 1,
                  kSpline[2] + 1 );

  double bz, by, byz;
  double bxt[2], byt[2], bzt[2];

  if( _flatx || kUp[0] >= dimX() )
  {
    kUp[0] = kSpline[0];
    bxt[0] = 1.;
    bxt[1] = 0.;
  }
  else
  {
    bxt[1] = pSpline[0] - kSpline[0];
    bxt[0] = 1. - bxt[1];
  }

  if( _flaty || kUp[1] >= dimY() )
  {
    kUp[1] = kSpline[1];
    byt[0] = 1.;
    byt[1] = 0.;
  }
  else
  {
    byt[1] = pSpline[1] - kSpline[1];
    byt[0] = 1. - byt[1];
  }

  if( _flatz || kUp[2] >= dimZ() )
  {
    kUp[2] = kSpline[2];
    bzt[0] = 1.;
    bzt[1] = 0.;
  }
  else
  {
    bzt[1] = pSpline[2] - kSpline[2];
    bzt[0] = 1. - bzt[1];
  }

  const Point3df *pCtrlPt;
  long incr = &_ctrlPointDelta( 1 ) - & _ctrlPointDelta( 0 );
  kUp[0] -= kSpline[0];

  for( int k = kSpline[2]; k <= kUp[2]; ++k )
  {
    bz = bzt[ k - kSpline[2] ];
    for( int j = kSpline[1]; j <= kUp[1]; ++j )
    {
      by = byt[ j - kSpline[1] ];
      byz = bz * by;
      pCtrlPt = &_ctrlPointDelta( kSpline[0], j, k );

      fdef = *pCtrlPt * bxt[0] * byz;
      pCtrlPt += incr;
      deformation[0] += fdef[0];
      deformation[1] += fdef[1];
      deformation[2] += fdef[2];

      if( kUp[0] != 0 )
      {
        fdef = *pCtrlPt * bxt[1] * byz;
        deformation[0] += fdef[0];
        deformation[1] += fdef[1];
        deformation[2] += fdef[2];
      }
    }
  }

  return deformation;
}

Point3dd TrilinearFfd::transformDouble( double x, double y, double z ) const
{
  Point3dd p(x, y, z);
  return p + deformation_private(p);
}


//

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
T SplineFfdResampler<T, C>::defaultBackground()
{
  return T( 0 );
}

template <class T, class C>
void SplineFfdResampler<T, C>::init()
{
  _samples = DataTypeInfo<T>::samples();
  _channelcoef.resize(_samples);
  _min.resize(_samples);
  _max.resize(_samples);
  _dimx = 1;
  _dimy = 1;
  _dimz = 1;
  _vsx = 1.f;
  _vsy = 1.f;
  _vsz = 1.f;
  _idaffine = _affine.isIdentity();
}

template <class T, class C>
SplineFfdResampler<T, C>::SplineFfdResampler(
    const FfdTransformation & transformation, T background ):
  _transformation(transformation),
  _spline(3, 0),
  _background(background)
{
  init();
}

template <class T, class C>
SplineFfdResampler<T, C>::SplineFfdResampler(
    const FfdTransformation & transformation, const AffineTransformation3d & affine,
    T background ):
  _affine(affine),
  _transformation(transformation),
  _spline(3, 0),
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
      _cubicresampler.reset();
      const AimsData<C> & channel = dataselector.select(_ref, c);
      _channelcoef[c] = _cubicresampler.getSplineCoef( channel, t );
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
  vector<int> dims = _ref.volume()->getSize();
  _dimx = dims[0];
  _dimy = dims[1];
  _dimz = dims[2];
  vector<float> vs = _ref.volume()->getVoxelSize();
  _vsx = vs[0];
  _vsy = vs[1];
  _vsz = vs[2];
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
  Point3dd  p_ref( output_location[0], output_location[1], output_location[2] );

  // transform point
  if( !_idaffine )
    p_ref = _affine.transform( p_ref );
  p_ref = _transformation.transform( p_ref );

  // Spline resampling

  Point3dd pv_ref( p_ref[0] / _vsx,
                   p_ref[1] / _vsy,
                   p_ref[2] / _vsz );

  Point3dl pi_ref( (int)(floor(pv_ref[0])),
                   (int)(floor(pv_ref[1])),
                   (int)(floor(pv_ref[2])) );

  Point3dl kDown( ( _transformation.isXFlat() ? pi_ref[0] : pi_ref[0] - 1 ),
                  ( _transformation.isYFlat() ? pi_ref[1] : pi_ref[1] - 1 ),
                  ( _transformation.isZFlat() ? pi_ref[2] : pi_ref[2] - 1 ) );
  Point3dl kUp  ( ( _transformation.isXFlat() ? pi_ref[0] : pi_ref[0] + 2 ),
                  ( _transformation.isYFlat() ? pi_ref[1] : pi_ref[1] + 2 ),
                  ( _transformation.isZFlat() ? pi_ref[2] : pi_ref[2] + 2 ) );

  if( pi_ref[0] >= 0 && pi_ref[0] < _dimx &&
      pi_ref[1] >= 0 && pi_ref[1] < _dimy &&
      pi_ref[2] >= 0 && pi_ref[2] < _dimz )
  {
    // Loop on channels to resample data in each one
    for ( int c = 0; c < _samples; ++c )
    {
      output_channel_value = 0;

      // Integrate spline values in 3 dimensions
      // pffd[0] - i, pffd[1] - j, pffd[2] - k,  vary in the range [-2, 2[
      for( int k = kDown[2]; k <= kUp[2]; ++k ) {
        bk3 = ( _transformation.isZFlat() ? 1. : _spline(pv_ref[2] - k) );
        ck = aims::mirrorCoeff( k, _dimz );
        for( int j = kDown[1]; j <= kUp[1]; ++j ) {
          bj3 = ( _transformation.isYFlat() ? 1. : _spline(pv_ref[1] - j) );
          cj = aims::mirrorCoeff( j, _dimy );
          for( int i = kDown[0]; i <= kUp[0]; ++i ) {
            bi3 = ( _transformation.isXFlat() ? 1. : _spline(pv_ref[0] - i) );
            ci = aims::mirrorCoeff( i, _dimx );
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

// template <>
// Point3df SplineFfdResampler<Point3df, float>::defaultBackground()
// {
//   return Point3df( 0.f );
// }



//============================================================================
//   N E A R E S T N E I G H B O R  R E S A M P L E R
//============================================================================
template <class T, class C>
NearestNeighborFfdResampler<T, C>::~NearestNeighborFfdResampler()
{}


template <class T, class C>
T NearestNeighborFfdResampler<T, C>::defaultBackground()
{
  return T( 0 );
}

template <class T, class C>
void NearestNeighborFfdResampler<T, C>::init()
{
  _dimx = 1;
  _dimy = 1;
  _dimz = 1;
  _vsx = 1.f;
  _vsy = 1.f;
  _vsz = 1.f;
  _idaffine = _affine.isIdentity();
}

template <class T, class C>
NearestNeighborFfdResampler<T, C>::NearestNeighborFfdResampler(
    const FfdTransformation & transformation, T background ):
  _transformation(transformation),
  _background(background)
{
  init();
}

template <class T, class C>
NearestNeighborFfdResampler<T, C>::NearestNeighborFfdResampler(
    const FfdTransformation & transformation, const AffineTransformation3d & affine,
    T background ):
  _affine(affine),
  _transformation(transformation),
  _background(background)
{
  init();
}

template <class T, class C>
void NearestNeighborFfdResampler<T, C>::setRef(const AimsData<T> & ref) {
  _ref = ref;
  vector<int> dims = _ref.volume()->getSize();
  _dimx = dims[0];
  _dimy = dims[1];
  _dimz = dims[2];
  vector<float> vs = _ref.volume()->getVoxelSize();
  _vsx = vs[0];
  _vsy = vs[1];
  _vsz = vs[2];
}

template <class T, class C>
Point3df NearestNeighborFfdResampler<T, C>::resample(
  const Point3df & output_location,
  T & output_value, int t)
{
  // output_location is in the output image referential (unit: mm)
  Point3df p_ref = output_location;

  if (! _idaffine )
    p_ref = _affine.transform( p_ref );
  p_ref = _transformation.transform( p_ref );

  // Nearest neighbor resampling
  Point3dl pi_ref( (int)(p_ref[0] / _vsx + .5),
                   (int)(p_ref[1] / _vsy + .5),
                   (int)(p_ref[2] / _vsz + .5) );

  if( pi_ref[0] >= 0 && pi_ref[0] < _dimx &&
      pi_ref[1] >= 0 && pi_ref[1] < _dimy &&
      pi_ref[2] >= 0 && pi_ref[2] < _dimz )
  {
    output_value = _ref( pi_ref[0], pi_ref[1], pi_ref[2], t );
  }
  else
    output_value = _background;

  return p_ref;
}

//============================================================================
//   T R I L I N E A R   R E S A M P L E R
//============================================================================
template <class T, class C>
TrilinearFfdResampler<T, C>::~TrilinearFfdResampler()
{}


template <class T, class C>
T TrilinearFfdResampler<T, C>::defaultBackground()
{
  return T( 0 );
}

template <class T, class C>
void TrilinearFfdResampler<T, C>::init()
{
  _dimx = 1;
  _dimy = 1;
  _dimz = 1;
  _vsx = 1.f;
  _vsy = 1.f;
  _vsz = 1.f;
  _idaffine = _affine.isIdentity();
}

template <class T, class C>
TrilinearFfdResampler<T, C>::TrilinearFfdResampler(
    const FfdTransformation & transformation, T background ):
  _transformation(transformation),
  _background(background)
{
  init();
}

template <class T, class C>
TrilinearFfdResampler<T, C>::TrilinearFfdResampler(
    const FfdTransformation & transformation, const AffineTransformation3d & affine,
    T background ):
  _affine(affine),
  _transformation(transformation),
  _background(background)
{
  init();
}

template <class T, class C>
void TrilinearFfdResampler<T, C>::setRef(const AimsData<T> & ref) {
  _ref = ref;
  vector<int> dims = _ref.volume()->getSize();
  _dimx = dims[0];
  _dimy = dims[1];
  _dimz = dims[2];
  vector<float> vs = _ref.volume()->getVoxelSize();
  _vsx = vs[0];
  _vsy = vs[1];
  _vsz = vs[2];
}

template <class T, class C>
Point3df TrilinearFfdResampler<T, C>::resample(
  const Point3df & output_location,
  T & output_value, int t)
{
  double    bk3, bj3, bi3;
  // output_location is in the output image referential (unit: mm)
  Point3df p_ref = output_location;

  if (! _idaffine )
    p_ref = _affine.transform( p_ref );
  p_ref = _transformation.transform( p_ref );

  // Linear resampling

  Point3dd pv_ref( p_ref[0] / _vsx,
                   p_ref[1] / _vsy,
                   p_ref[2] / _vsz );

  Point3dl pi_ref( (int)(floor(pv_ref[0])),
                   (int)(floor(pv_ref[1])),
                   (int)(floor(pv_ref[2])) );

  bool flatx = _transformation.isXFlat();
  bool flaty = _transformation.isYFlat();
  bool flatz = _transformation.isZFlat();

  Point3dl kUp  ( ( flatx ? pi_ref[0] : pi_ref[0] + 1 ),
                  ( flaty ? pi_ref[1] : pi_ref[1] + 1 ),
                  ( flatz ? pi_ref[2] : pi_ref[2] + 1 ) );

  if( kUp[0] >= _dimx )
  {
    kUp[0] = _dimx - 1;
    flatx = true;
  }
  if( kUp[1] >= _dimy )
  {
    kUp[1] = _dimy - 1;
    flaty = true;
  }
  if( kUp[2] >= _dimz )
  {
    kUp[2] = _dimz - 1;
    flatz = true;
  }

  // Warning: interpolation is done in "source" space
  if( pi_ref[0] >= 0 && pi_ref[0] < _dimx &&
      pi_ref[1] >= 0 && pi_ref[1] < _dimy &&
      pi_ref[2] >= 0 && pi_ref[2] < _dimz )
  {
    for ( int c = 0; c < _samples; ++c )
      _channelselector.set(output_value, c, 0.);

    for( int k = pi_ref[2]; k <= kUp[2]; ++k )
    {
      bk3 = ( flatz ? 1. : 1. - std::abs( pv_ref[2] - k ) );
      for( int j = pi_ref[1]; j <= kUp[1]; ++j )
      {
        bj3 = ( flaty ? 1. : 1. - std::abs( pv_ref[1] - j ) );
        for( int i = pi_ref[0]; i <= kUp[0]; ++i )
        {
          bi3 = ( flatx ? 1. : 1. - std::abs( pv_ref[0] - i ) );
          output_value += _ref(i, j, k, t) * bi3 * bj3 * bk3;
        }
      }
    }
  }
  else
    output_value = _background;

  return p_ref;
}


template <int D>
void ffdTransformMesh( AimsTimeSurface<D, Void> & mesh,
                       FfdTransformation & deformation,
                       const AffineTransformation3d & affine )
{
  typename AimsTimeSurface<D, Void>::iterator is, es = mesh.end();
  bool idaffine = affine.isIdentity();

  for( is=mesh.begin(); is!=es; ++is )
  {
    vector<Point3df> & vert = is->second.vertex();
    vector<Point3df>::iterator iv, ev = vert.end();
    for( iv=vert.begin(); iv!=ev; ++iv )
    {
      Point3df & p = *iv;
      if( !idaffine )
        p = affine.transform( p );
      p = deformation.transform( p );
    }
  }

  mesh.updateNormals();
}


rc_ptr<BucketMap<Void> >
ffdTransformBucket( const BucketMap<Void> & bck, FfdTransformation & deformation,
                    const AffineTransformation3d & affine,
                    const Point3df & vso )
{
  Point3df vvs = vso;
  if( vvs == Point3df( 0., 0., 0. ) )
  {
    vvs[0] = bck.sizeX();
    vvs[1] = bck.sizeY();
    vvs[2] = bck.sizeZ();
  }
  BucketMap<Void>::const_iterator ib, eb = bck.end();
  bool idaffine = affine.isIdentity();
  rc_ptr<BucketMap<Void> > out( new BucketMap<Void> );
  out->setSizeX( vvs[0] );
  out->setSizeY( vvs[1] );
  out->setSizeZ( vvs[2] );
  Point3df vs( bck.sizeX(), bck.sizeY(), bck.sizeZ() );

  for( ib=bck.begin(); ib!=eb; ++ib )
  {
    BucketMap<Void>::Bucket & obk = (*out)[ ib->first ];
    BucketMap<Void>::Bucket::const_iterator ip, ep = ib->second.end();
    for( ip=ib->second.begin(); ip!=ep; ++ip )
    {
      Point3df p( ip->first[0] * vs[0], ip->first[1] * vs[1],
                  ip->first[2] * vs[2] );
      if( !idaffine )
        p = affine.transform( p );
      p = deformation.transform( p );
      obk[ Point3d( int( rint( p[0] / vvs[0] ) ),
                    int( rint( p[1] / vvs[1] ) ),
                    int( rint( p[2] / vvs[2] ) ) ) ] = Void();
    }
  }

  return out;
}


namespace
{

  void transformGraphObject( GraphObject *go, FfdTransformation & deformation,
                        const AffineTransformation3d & affine,
                        const Point3df & vso, vector<int> &bbmin,
                        vector<int> & bbmax )
  {
    Object iter;
    for( iter=go->objectIterator(); iter->isValid(); iter->next() )
    {
      rc_ptr<AimsSurfaceTriangle> mesh;
      try
      {
        mesh = iter->currentValue()->value<rc_ptr<AimsSurfaceTriangle> >();
        ffdTransformMesh( *mesh, deformation, affine );
        continue;
      }
      catch( ... )
      {
      }

      rc_ptr<BucketMap<Void> > bck;
      try
      {
        bck = iter->currentValue()->value<rc_ptr<BucketMap<Void> > >();
        rc_ptr<BucketMap<Void> > obk
          = ffdTransformBucket( *bck, deformation, affine, vso );
        go->setProperty( iter->key(), obk );

        // bounding box
        BucketMap<Void>::Bucket::const_iterator
          ip, ep = obk->begin()->second.end();
        for( ip=obk->begin()->second.begin(); ip!=ep; ++ip )
        {
          const Point3d & p = ip->first;
          if( bbmin.empty() )
          {
            bbmin.resize( 3 );
            bbmin[0] = p[0];
            bbmin[1] = p[1];
            bbmin[2] = p[2];
            bbmax = bbmin;
          }
          else
          {
            if( p[0] < bbmin[0] )
              bbmin[0] = p[0];
            if( p[1] < bbmin[1] )
              bbmin[1] = p[1];
            if( p[1] < bbmin[1] )
              bbmin[1] = p[1];
            if( p[0] > bbmax[0] )
              bbmax[0] = p[0];
            if( p[1] > bbmax[1] )
              bbmax[1] = p[1];
            if( p[2] > bbmax[2] )
              bbmax[2] = p[2];
          }
        }

        continue;
      }
      catch( ... )
      {
      }

      rc_ptr<AimsTimeSurface<2, Void> > mesh2;
      try
      {
        mesh2
          = iter->currentValue()->value<rc_ptr<AimsTimeSurface<2, Void> > >();
        ffdTransformMesh( *mesh2, deformation, affine );
        continue;
      }
      catch( ... )
      {
      }

      rc_ptr<AimsTimeSurface<4, Void> > mesh4;
      try
      {
        mesh4
          = iter->currentValue()->value<rc_ptr<AimsTimeSurface<4, Void> > >();
        ffdTransformMesh( *mesh4, deformation, affine );
        continue;
      }
      catch( ... )
      {
      }
    }
  }

}


void ffdTransformGraph( Graph & graph, FfdTransformation & deformation,
                        const AffineTransformation3d & affine,
                        const Point3df & vso )
{
  Point3df vvs = vso;
  vector<float> vs;
  Object ovs = graph.getProperty( "voxel_size" );
  Object ivs = ovs->objectIterator();
  for( ; ivs->isValid(); ivs->next() )
    vs.push_back( float( ivs->currentValue()->getScalar() ) );
  while( vs.size() < 3 )
    vs.push_back( 1.f );

  if( vvs == Point3df( 0., 0., 0. ) )
  {
    vvs[0] = vs[0];
    vvs[1] = vs[1];
    vvs[2] = vs[2];
  }
  else
    graph.setProperty( "voxel_size", vvs.toStdVector() );

  vector<int> bbmin, bbmax;

  Graph::iterator iv, ev = graph.end();
  Object iter;
  for( iv=graph.begin(); iv!=ev; ++iv )
    transformGraphObject( *iv, deformation, affine, vvs, bbmin, bbmax );

  const set<Edge *> & edges = graph.edges();
  set<Edge *>::const_iterator ie, ee = edges.end();
  for( ie=edges.begin(); ie!=ee; ++ie )
    transformGraphObject( *ie, deformation, affine, vvs, bbmin, bbmax );

  graph.setProperty( "boundingbox_min", bbmin );
  graph.setProperty( "boundingbox_max", bbmax );

  bool idaffine = affine.isIdentity();
  vector<int> pos;
  vector<string> poskeys(3);
  poskeys[0] = "anterior_commissure";
  poskeys[1] = "posterior_commissure";
  poskeys[2] = "interhemi_point";
  vector<string>::iterator is, es = poskeys.end();
  for( is=poskeys.begin(); is!=es; ++is )
    if( graph.getProperty( *is, pos ) )
    {
      Point3df p( pos[0] * vs[0], pos[1] * vs[1], pos[2] * vs[2] );
      if( !idaffine )
        p = affine.transform( p );
      p = deformation.transform( p );
      pos[0] = int( rint( p[0] / vvs[0] ) );
      pos[1] = int( rint( p[1] / vvs[1] ) );
      pos[2] = int( rint( p[2] / vvs[2] ) );
      graph.setProperty( *is, pos );
    }
}


// Bundles

BundleFFDTransformer::BundleFFDTransformer(
  rc_ptr<FfdTransformation> deformation, const AffineTransformation3d & affine )
  : BundleListener(), BundleProducer(),
    _deformation( deformation ), _affine( affine ),
    _idaffine( affine.isIdentity() )
{
}


BundleFFDTransformer::~BundleFFDTransformer()
{
}


void BundleFFDTransformer::bundleStarted( const BundleProducer &,
                                          const BundleInfo &bi )
{
  startBundle( bi );
}


void BundleFFDTransformer::bundleTerminated( const BundleProducer &,
                                             const BundleInfo & bi )
{
  terminateBundle( bi );
}


void BundleFFDTransformer::fiberStarted( const BundleProducer &,
                                         const BundleInfo & bi,
                                         const FiberInfo & fi )
{
  startFiber( bi, fi );
}


void BundleFFDTransformer::fiberTerminated( const BundleProducer &,
                                            const BundleInfo & bi,
                                            const FiberInfo & fi )
{
  terminateFiber( bi, fi );
}


void BundleFFDTransformer::newFiberPoint( const BundleProducer &,
                                          const BundleInfo & bi,
                                          const FiberInfo & fi,
                                          const FiberPoint & pt )
{
  FiberPoint opt;
  if( !_idaffine )
    opt = _affine.transform( pt );
  else
    opt = pt;
  opt = _deformation->transform( opt );
  addFiberPoint( bi, fi, opt );
}


void BundleFFDTransformer::noMoreBundle( const BundleProducer & )
{
  BundleProducer::noMoreBundle();
}

//

template <>
Point3df SplineFfdResampler<Point3df, float>::defaultBackground()
{
  return Point3df( 0.f );
}

template <>
Point3df NearestNeighborFfdResampler<Point3df, float>::defaultBackground()
{
  return Point3df( 0.f );
}

template <>
Point3df TrilinearFfdResampler<Point3df, float>::defaultBackground()
{
  return Point3df( 0.f );
}


// template instantiations

template void ffdTransformMesh( AimsTimeSurface<2, Void> &, FfdTransformation &,
                                const AffineTransformation3d & );
template void ffdTransformMesh( AimsTimeSurface<3, Void> &, FfdTransformation &,
                                const AffineTransformation3d & );
template void ffdTransformMesh( AimsTimeSurface<4, Void> &, FfdTransformation &,
                                const AffineTransformation3d & );

template class FfdResampler<int8_t>;
template class FfdResampler<uint8_t>;
template class FfdResampler<int16_t>;
template class FfdResampler<uint16_t>;
template class FfdResampler<int32_t>;
template class FfdResampler<uint32_t>;
template class FfdResampler<float>;
template class FfdResampler<double>;
template class FfdResampler<Point3df>;
template class FfdResampler<AimsRGB>;
template class FfdResampler<AimsRGBA>;

template class SplineFfdResampler<int8_t>;
template class SplineFfdResampler<uint8_t>;
template class SplineFfdResampler<int16_t>;
template class SplineFfdResampler<uint16_t>;
template class SplineFfdResampler<int32_t>;
template class SplineFfdResampler<uint32_t>;
template class SplineFfdResampler<float>;
template class SplineFfdResampler<double>;
template class SplineFfdResampler<Point3df, float>;
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
template class NearestNeighborFfdResampler<Point3df, float>;
template class NearestNeighborFfdResampler<AimsRGB, AimsRGB::ChannelType>;
template class NearestNeighborFfdResampler<AimsRGBA, AimsRGBA::ChannelType>;

template class TrilinearFfdResampler<int8_t>;
template class TrilinearFfdResampler<uint8_t>;
template class TrilinearFfdResampler<int16_t>;
template class TrilinearFfdResampler<uint16_t>;
template class TrilinearFfdResampler<int32_t>;
template class TrilinearFfdResampler<uint32_t>;
template class TrilinearFfdResampler<float>;
template class TrilinearFfdResampler<double>;
template class TrilinearFfdResampler<Point3df, float>;
template class TrilinearFfdResampler<AimsRGB, AimsRGB::ChannelType>;
template class TrilinearFfdResampler<AimsRGBA, AimsRGBA::ChannelType>;

} // namespace aims
