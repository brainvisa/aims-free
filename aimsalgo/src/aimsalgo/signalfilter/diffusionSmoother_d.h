/* This software and supporting documentation are distributed by
 *     Institut Federatif de Recherche 49
 *     CEA/NeuroSpin, Batiment 145,
 *     91191 Gif-sur-Yvette cedex
 *     France
 *
 * This software is governed by the CeCILL-B license under
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the
 * terms of the CeCILL-B license as circulated by CEA, CNRS
 * and INRIA at the following URL "http://www.cecill.info".
 *
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability.
 *
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or
 * data to be ensured and,  more generally, to use and operate it in the
 * same conditions as regards security.
 *
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL-B license and that you accept its terms.
 */



#ifndef AIMS_DIFFUSION_SMOOTHER_D_H
#define AIMS_DIFFUSION_SMOOTHER_D_H

#include <cstdlib>
#include <aims/signalfilter/diffusionSmoother.h>
#include <aims/signalfilter/convol.h>
#include <aims/utility/converter_volume.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

namespace aims {


template<typename T>
carto::VolumeRef<float> BaseDiffusionSmoother<T>::laplacian =
  BaseDiffusionSmoother<T>::init_laplacian();

template<typename T>
carto::VolumeRef<float> BaseDiffusionSmoother<T>::init_laplacian(void)
{
  carto::VolumeRef<float>	laplacian( 3, 3, 3, 1,
                                       carto::AllocatorContext::fast() );
  laplacian = 0.;
  /* WARNING: this kernel is only suitable for isotropic, 1mm voxels.
     Moreover I don't explain the 1/2 factor on the whole kernel
     (I would expect coefs 1 -6 1 instead of 0.5 -3 0.5)
     Anyway, this kernel is now completely rebuilt in doSmoothing() to
     take into account voxel size, so it is not used as is.
     (Denis, 2014/02/14)
  */
  laplacian(1,1,0) = laplacian(1,0,1) = laplacian(0,1,1) =
    laplacian(2,1,1) = laplacian(1,2,1) = laplacian(1,1,2) = 0.5;
  laplacian(1,1,1) = -3;

  return laplacian;
}

template<class T>
void BaseDiffusionSmoother<T>::check(int maxiter)
{
  if (maxiter < 0)
  {
    std::cerr << "diffusionConvolution Smoother: "
        << "maxiter negative value is forbidden" << std::endl;
    exit(EXIT_FAILURE);
  }
}

template<class T>
carto::VolumeRef<T> DiffusionSmoother<T>::doSmoothing(
  const carto::VolumeRef<T> & ima, int maxiter, bool verbose)
{
  this->check(maxiter);
  carto::VolumeRef<float> kernel = DiffusionSmoother<T>::laplacian.copy();
  std::vector<float> vs = ima->getVoxelSize();
  // rebuild the kernel accounting for possibly anisotropic voxel size
  kernel(0,1,1) = kernel(2,1,1) = this->_dt / ( vs[0] * vs[0] );
  kernel(1,0,1) = kernel(1,2,1) = this->_dt / ( vs[1] * vs[1] );
  kernel(1,1,0) = kernel(1,1,2) = this->_dt / ( vs[2] * vs[2] );
  kernel(1,1,1) = -this->_dt * (2. / (vs[0] * vs[0])
    + 2. / (vs[1] * vs[1])
    + 2. / (vs[2] * vs[2]) );
  //
  // 1 (center) + kernel * dt
  kernel(1,1,1) = 1. + kernel(1,1,1);
  /*
  kernel(1,1,1) = 1. + kernel(1,1,1) * this->_dt;
  kernel(1,1,0) = kernel(1,0,1) = kernel(0,1,1) =
    kernel(2,1,1) = kernel(1,2,1) = kernel(1,1,2) = 0.5 * this->_dt;
  */

  carto::VolumeRef<float> *tmp1, *tmp2, *swap;
  AimsConvolution<float> convolution;

  std::vector<int> dim = ima->getSize();
  carto::Converter< carto::VolumeRef<T>, carto::VolumeRef<float> > conv;
  carto::VolumeRef< float > imaF( dim ), ima2( dim );
  conv.convert(ima, imaF);
  tmp1 = &imaF; tmp2 = &ima2;

  for (int i = 0; i < maxiter; i++)
  {
    if (verbose) std::cout << "+" << std::flush;
    if( _hasConstantSources )
    {
      int x, y, z;
      for( z=0; z<dim[2]; ++z )
        for( y=0; y<dim[1]; ++y )
          for( x=0; x<dim[0]; ++x )
          {
            T & v = _constantSources( x, y, z );
            if( v != _constantSourcesBackground )
              (*tmp1)( x, y, z ) = v;
          }
    }
    (*tmp2) = convolution.doit( *tmp1, kernel );
    swap = tmp1;
    tmp1 = tmp2;
    tmp2 = swap;
  }

  if( _hasConstantSources )
  {
    int x, y, z;
      for( z=0; z<dim[2]; ++z )
        for( y=0; y<dim[1]; ++y )
          for( x=0; x<dim[0]; ++x )
          {
            T & v = _constantSources( x, y, z );
            if( v != _constantSourcesBackground )
              (*tmp1)( x, y, z ) = v;
          }
  }

  if (verbose) std::cout << "Finished" << std::endl;
  carto::ShallowConverter< carto::VolumeRef<float>, carto::VolumeRef<T> >
    conv2;
  carto::VolumeRef<T>  ima3( dim );
  conv2.convert( (*tmp1), ima3);

  return ima3;
}


template<typename T>
void DiffusionSmoother<T>::setConstantSources(
  const carto::VolumeRef<T> & sources, const T & bgr )
{
  _constantSources = sources;
  _constantSourcesBackground = bgr;
  _hasConstantSources = true;
}


template <typename T>
void DiffusionSmoother<T>::removeConstantSources()
{
  _hasConstantSources = false;
  _constantSources = carto::VolumeRef<T>();
}


template<typename T> carto::VolumeRef<T>
MaskedDiffusionSmoother<T, std::vector<Point3df> >::
doSmoothing(const carto::VolumeRef<T> & ima, int maxiter, bool verbose)
{
  this->check(maxiter);
  carto::VolumeRef<float> kernel = DiffusionSmoother<T>::laplacian.copy();
  std::vector<float> vs = ima->getVoxelSize();
  // rebuild the kernel accounting for possibly anisotropic voxel size
  kernel(0,1,1) = kernel(2,1,1) = this->_dt / ( vs[0] * vs[0] );
  kernel(1,0,1) = kernel(1,2,1) = this->_dt / ( vs[1] * vs[1] );
  kernel(1,1,0) = kernel(1,1,2) = this->_dt / ( vs[2] * vs[2] );
  kernel(1,1,1) = -this->_dt * (2. / (vs[0] * vs[0])
    + 2. / (vs[1] * vs[1])
    + 2. / (vs[2] * vs[2]) );
  //
  // 1 (center) + kernel * dt
  kernel(1,1,1) = 1. + kernel(1,1,1);
  /*
  kernel(1,1,1) = 1. + kernel(1,1,1) * this->_dt;
  kernel(1,1,0) = kernel(1,0,1) = kernel(0,1,1) =
    kernel(2,1,1) = kernel(1,2,1) = kernel(1,1,2) = 0.5 * this->_dt;
  */

  carto::VolumeRef<float> *tmp1, *tmp2, *swap;
  AimsMaskedConvolution<float> convolution( this->_mask,
    this->_background, this->_safe );

  carto::Converter< carto::VolumeRef<T>, carto::VolumeRef<float> > conv;
  std::vector<int> dim = ima->getSize();
  carto::VolumeRef< float > imaF( dim ), ima2( dim );
  conv.convert(ima, imaF);
  tmp1 = &imaF; tmp2 = &ima2;

  for (int i = 0; i < maxiter; i++)
  {
    if (verbose) std::cout << "+" << std::flush;
    if (_has_neumann_condition)
      update_neumann_conditions(*tmp1);
    (*tmp2) = convolution.doit((*tmp1), kernel);
    swap = tmp1;
    tmp1 = tmp2;
    tmp2 = swap;
  }

  if (verbose) std::cout << "Finished" << std::endl;
  carto::ShallowConverter< carto::VolumeRef<float>, carto::VolumeRef<T> >
    conv2;
  carto::VolumeRef<T>  ima3( dim );
  conv2.convert( (*tmp1), ima3);

  return ima3;
}


template<typename T> inline
void MaskedDiffusionSmoother<T, std::vector<Point3df> >::
add_neumann_condition(const Point3df &p)
{
  _has_neumann_condition = true;
  _neumann_conditions.push_back(p);
}



template<typename T> 
void MaskedDiffusionSmoother<T, std::vector<Point3df> >::
update_neumann_conditions(carto::VolumeRef<float> &ima)
{
  int    d;
  Point3di  dim( ima->getSizeX(), ima->getSizeY(), ima->getSizeZ() );

  for (unsigned int i = 0; i < _neumann_conditions.size(); ++i)
  {
    //copy first encountred unmasked value among neighbours
    const Point3df  &p = _neumann_conditions[i];
    for (i = 0; i < 3; ++i)
    for (d = -1; d <= 1; d += 2)
    {
      Point3di  p2(p);
      p2[i] += d;
      if (p2[i] < 0 or p2[i] >= dim[i]) continue;
      if (((this->_mask))(p2[0], p2[1], p2[2]) != 0)
      {
        ima(p[0], p[1], p[2]) =ima(p2[0], p2[1], p2[2]);
        i = d = 3;
        break;
      }
    }
  }
}


template<typename T> carto::VolumeRef<T>
MaskedDiffusionSmoother<T, carto::VolumeRef<short> >::
doSmoothing(const carto::VolumeRef<T> & ima, int maxiter, bool verbose)
{
  this->check(maxiter);

  carto::VolumeRef<float> *tmp1, *tmp2, *swap;

  carto::Converter< carto::VolumeRef<T>, carto::VolumeRef<float> > conv;
  std::vector<int> dim = ima->getSize();
  carto::VolumeRef< float > imaF( dim ), ima2( dim );
  conv.convert(ima, imaF);
  tmp1 = &imaF; tmp2 = &ima2;

  for (int i = 0; i < maxiter; i++)
  {
    if (verbose) std::cout << "+" << std::flush;
    convolution((*tmp1), (*tmp2));
    swap = tmp1;
    tmp1 = tmp2;
    tmp2 = swap;
  }

  if (verbose) std::cout << "Finished" << std::endl;
  carto::ShallowConverter< carto::VolumeRef<float>, carto::VolumeRef<T> >
    conv2;
  carto::VolumeRef<T>  ima3( dim );
  conv2.convert( (*tmp1), ima3);

  return ima3;
}

template<typename T> void
MaskedDiffusionSmoother<T, carto::VolumeRef<short> >::
convolution(const carto::VolumeRef<float> &ima1, carto::VolumeRef<float> &ima2) const
{
  int i, d, x, y, z, t, n;
  int dx = ima1->getSizeX();
  int dy = ima1->getSizeY();
  int dz = ima1->getSizeZ();
  int dt = ima1->getSizeT();
  Point3di  dim( dx, dy, dz );

  for (t = 0; t < dt; t++)
    for (z = 0; z < dz; z++)
      for (y = 0; y < dy; y++)
        for (x = 0; x < dx; x++)
        {
          std::cout << "x,y,z = " << x <<", " << y << ", " << z << std::endl;
          std::cout << "mask = " << ((*this->_mask)(x, y, z, t)) << std::endl;
          if ((*this->_mask)(x, y, z, t) == this->_background or
          (*this->_mask)(x, y, z, t) == this->_neumann_value)
          {
            ima2(x, y, z, t) = ima1(x, y, z, t);
            continue;
          }
          n = 0;
          T val = (T) 0.;

          for (i = 0; i < 3; ++i)
          for (d = -1; d <= 1; d += 2)
          {
            Point3di  p2(x, y, z);
            p2[i] += d;
            if (p2[i] < 0 or p2[i] >= dim[i]) continue;
            if ((*this->_mask)(p2[0], p2[1], p2[2], t)
              == this->_neumann_value) continue;
            n++;
            val += ima1(p2[0], p2[1], p2[2], t);
          }
          val -= n * ima1(x, y, z, t);
          ima2(x, y, z, t) = ima1(x, y, z, t) + this->_dt * val;
        }
}
//FIXME : manque une version en connexité 26

}

#endif
