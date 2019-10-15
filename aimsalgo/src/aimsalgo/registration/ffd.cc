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
#include <limits>
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
    carto::VolumeRef<float> def( newDeformationGrid.dimX(),
                                 newDeformationGrid.dimY(),
                                 newDeformationGrid.dimZ() );
    def->fill( 0. );

    int dx = def->getSizeX(), dy = def->getSizeY(), dz = def->getSizeZ();
    // Copy of coefficients to def
    for( int k = 0; k < dz; ++k )
    for( int j = 0; j < dy; ++j )
    for( int i = 0; i < dx; ++i )
      def->at(i, j, k) = newDeformationGrid(i, j, k)[c];

    // Construction of spline coefficients for test image
    VolumeRef<double> splineCoeff;
    {
      CubicResampler<float> interpolator;
      splineCoeff = interpolator.getSplineCoef( def );
    }

    for( int k = 0; k < dz; ++k )
    for( int j = 0; j < dy; ++j )
    for( int i = 0; i < dx; ++i )
      _ctrlPointDelta(i, j, k)[c] = (float)splineCoeff->at(i, j, k);
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

  Point3di kSpline(static_cast<int>(std::floor(pSpline[0])),
                   static_cast<int>(std::floor(pSpline[1])),
                   static_cast<int>(std::floor(pSpline[2])));

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

  Point3di kDown( ( _flatx ? 0 : kSpline[0] - 1 ),
                  ( _flaty ? 0 : kSpline[1] - 1 ),
                  ( _flatz ? 0 : kSpline[2] - 1 ) );
  Point3di kUp  ( ( _flatx ? 0 : kSpline[0] + 2 ),
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
  const Point3dd pSpline( mmToSplineVox(pImage) );

  // Return NaN if the position is not covered by the deformation field
  if( !( pSpline[0] >= 0 && pSpline[0] < dimX() &&
         pSpline[1] >= 0 && pSpline[1] < dimY() &&
         pSpline[2] >= 0 && pSpline[2] < dimZ() ) )
  {
    const double NaN = std::numeric_limits<double>::quiet_NaN();
    return Point3dd(NaN, NaN, NaN);
  }

  const Point3di kSpline ( static_cast<int>( std::floor( pSpline[0] ) ),
                           static_cast<int>( std::floor( pSpline[1] ) ),
                           static_cast<int>( std::floor( pSpline[2] ) ) );
  Point3di kUp ( kSpline[0] + 1,
                 kSpline[1] + 1,
                 kSpline[2] + 1 );

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

  Point3dd deformation(0., 0., 0.);
  for( int k = kSpline[2]; k <= kUp[2]; ++k )
  {
    for( int j = kSpline[1]; j <= kUp[1]; ++j )
    {
      for( int i = kSpline[0]; i <= kUp[0]; ++i )
      {
        deformation += static_cast<Point3dd>(_ctrlPointDelta( i, j, k ))
                       * (bxt[i - kSpline[0]]
                          * byt[j - kSpline[1]]
                          * bzt[k - kSpline[2]]);
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

} // namespace aims
