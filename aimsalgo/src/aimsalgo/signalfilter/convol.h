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


#ifndef AIMS_SIGNALFILTER_CONVOL_H
#define AIMS_SIGNALFILTER_CONVOL_H

#include <aims/config/aimsalgo_config.h>
#include <aims/data/data.h>
#include <cartobase/type/limits.h>
#include <float.h>


/** The template class to make convolutions.
    This class is useful to perform a convolution of a data
    by a given kernel.

    if the image has a border unsafe version of convolution is used
    (no test on borders).
*/
template <class T>
class AimsConvolution
{
public:

  AimsConvolution(bool safe=true) : _safe(safe) {
    set_safe_status(safe);
    }
  virtual ~AimsConvolution() { }

  AimsData< T > doit( AimsData< T >&, AimsData< T >& );

  void set_safe_status(bool safe=true) {
    if (safe)
      _doit_voxel_method = &AimsConvolution<T>::doit_voxel_safe;
    else
      _doit_voxel_method = &AimsConvolution<T>::doit_voxel_unsafe;
    _safe = safe;
  }

protected:
  /// called for each voxel (user selected function)
  T doit_voxel(AimsData< T >& img, AimsData< T >& kernel,
					int x, int y, int z, int t);
  /// called for each voxel (safe version)
  virtual T doit_voxel_safe(AimsData< T >& img, AimsData< T >& kernel,
					int x, int y, int z, int t);
  /// called for each voxel (unsafe version)
  virtual T doit_voxel_unsafe(AimsData< T >& img, AimsData< T >& kernel,
					int x, int y, int z, int t);
  T min() const;
  T max() const;

  T (AimsConvolution::*_doit_voxel_method) (AimsData< T >& img,
		AimsData< T >& kernel, int x, int y, int z, int t);

  bool _safe;
};

/** Make convolution only on a specified mask.
    In the mask, voxels with value :
      - sources : omitted from convolution computation,
      but are used by unmasked neighbouring voxels (corresponding to Dirichlet
      border conditions when convolution is used in ODE or PDE).
      - other : corresponds to unmask voxels.
*/
template <class T>
class AimsMaskedConvolution : public AimsConvolution< T >
{
public:

  AimsMaskedConvolution(AimsData< short >&mask, short sources=0,
                      bool safe=true) : AimsConvolution< T >(safe),
                      _sources(sources), _mask(mask) { }
  virtual ~AimsMaskedConvolution() { }
  /// called for each voxel (safe version)
  virtual T doit_voxel_safe(AimsData< T >& img, AimsData< T >& kernel,
					int x, int y, int z, int t);
  /// called for each voxel (unsafe version)
  virtual T doit_voxel_unsafe(AimsData< T >& img, AimsData< T >& kernel,
					int x, int y, int z, int t);

protected:
  short			_sources;
  AimsData<short>	&_mask;
};


template< class T > inline
AimsData< T > AimsConvolution< T >::doit( AimsData< T >& img, 
					  AimsData< T >& kernel )
{
  ASSERT( kernel.dimT() == 1 );
  if (_safe == false) ASSERT(img.borderWidth() >= 1);
  else if (img.borderWidth() >= 1) set_safe_status(false);

  int x, y, z, t;

  int dx = img.dimX();
  int dy = img.dimY();
  int dz = img.dimZ();
  int dt = img.dimT();

  AimsData< T > res( dx, dy, dz, dt );
  res.setSizeXYZT( img.sizeX(), img.sizeY(), img.sizeZ(), img.sizeT() );

  for ( t=0; t<dt; t++ )
    for ( z=0; z<dz; z++ )
      for ( y=0; y<dy; y++ )
	for ( x=0; x<dx; x++ )
        {
	    T val = doit_voxel(img, kernel, x, y, z, t);
            if ( val > max() )  val = max();
            else if ( val < min() )  val = min();
	    res( x, y, z, t ) = val;
        }

  return res;
}

template< class T > inline
T AimsConvolution< T >::doit_voxel(AimsData< T >& img, AimsData< T >& kernel,
						int x, int y, int z, int t)
{
	return (this->*_doit_voxel_method)(img, kernel, x, y, z, t);
};

template< class T > inline
T AimsConvolution< T >::doit_voxel_safe(AimsData< T >& img,
		AimsData< T >& kernel, int x, int y, int z, int t)
{
	int i, j, k, idx, idy, idz;

	int mx = kernel.dimX();
	int my = kernel.dimY();
	int mz = kernel.dimZ();

	int mx2 = mx / 2;
	int my2 = my / 2;
	int mz2 = mz / 2;

	int dx = img.dimX();
	int dy = img.dimY();
	int dz = img.dimZ();

	T val = (T)0;

	for ( k=0; k<mz; k++ )
	for ( j=0; j<my; j++ )
	for ( i=0; i<mx; i++ )
	{
		idx = x - i + mx2;
		idy = y - j + my2;
		idz = z - k + mz2;

		if ( idx >= 0 && idy >= 0 && idz >= 0 &&
				idx < dx && idy < dy && idz < dz )
			val += kernel( i, j, k ) * img( idx, idy, idz, t );
	}

	return val;
}

template< class T > inline
T AimsConvolution< T >::doit_voxel_unsafe(AimsData< T >& img,
		AimsData< T >& kernel, int x, int y, int z, int t)
{
	int i, j, k, idx, idy, idz;

	int mx = kernel.dimX();
	int my = kernel.dimY();
	int mz = kernel.dimZ();

	int mx2 = mx / 2;
	int my2 = my / 2;
	int mz2 = mz / 2;

	T val = (T)0;

	for ( k=0; k<mz; k++ )
	for ( j=0; j<my; j++ )
	for ( i=0; i<mx; i++ )
	{
		idx = x - i + mx2;
		idy = y - j + my2;
		idz = z - k + mz2;

		val += kernel( i, j, k ) * img( idx, idy, idz, t );
	}

	return val;
}

template< class T > inline
T AimsMaskedConvolution< T >::doit_voxel_safe(AimsData< T >& img,
		AimsData< T >& kernel, int x, int y, int z, int t)
{
	if (_mask(x, y, z, t) == _sources) return img(x, y, z, t);
	return AimsConvolution<T>::doit_voxel_safe(img, kernel, x, y, z, t);
}

template< class T > inline
T AimsMaskedConvolution< T >::doit_voxel_unsafe(AimsData< T >& img,
		AimsData< T >& kernel, int x, int y, int z, int t)
{
	if (_mask(x, y, z, t) == _sources) return img(x, y, z, t);
	return AimsConvolution<T>::doit_voxel_unsafe(img, kernel, x, y, z, t);
}

template <>
inline
int8_t AimsConvolution< int8_t >::min() const
{
  return std::numeric_limits<int8_t>::min();
}


template <>
inline 
int8_t AimsConvolution< int8_t >::max() const
{
  return std::numeric_limits<int8_t>::max();
}


template <>
inline
uint8_t AimsConvolution< uint8_t >::min() const
{
  return 0;
}


template <>
inline 
uint8_t AimsConvolution< uint8_t >::max() const
{
  return std::numeric_limits<uint8_t>::max();
}


template <>
inline
int16_t AimsConvolution< int16_t >::min() const
{
  return std::numeric_limits<int16_t>::min();
}


template <>
inline
int16_t AimsConvolution< int16_t >::max() const
{
  return std::numeric_limits<int16_t>::max();
}


template <>
inline
uint16_t AimsConvolution< uint16_t >::min() const
{
  return 0;
}


template <>
inline
uint16_t AimsConvolution< uint16_t >::max() const
{
  return std::numeric_limits<uint16_t>::max();
}


template <>
inline
int32_t AimsConvolution< int32_t >::min() const
{
  return std::numeric_limits<int32_t>::min();
}


template <>
inline
int32_t AimsConvolution< int32_t >::max() const
{
  return std::numeric_limits<int32_t>::max();
}


template <>
inline
uint32_t AimsConvolution< uint32_t >::min() const
{
  return 0;
}


template <>
inline
uint32_t AimsConvolution< uint32_t >::max() const
{
  return std::numeric_limits<uint32_t>::max();
}


template <>
inline
float AimsConvolution< float >::min() const
{
  return (-FLT_MAX);
}


template <>
inline
float AimsConvolution< float >::max() const
{
  return FLT_MAX;
}


template <>
inline
double AimsConvolution< double >::min() const
{
  return DBL_MIN;
}


template <>
inline
double AimsConvolution< double >::max() const
{
  return DBL_MAX;
}


// ### remove after everything has been moved to intN_t/uintN_t
#include <limits.h>
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
template<>
inline
char AimsConvolution< char >::min() const
{
  return std::numeric_limits<char>::min();
}


template <>
inline
char AimsConvolution< char >::max() const
{
  return std::numeric_limits<char>::max();
}
#endif


template <>
inline
long AimsConvolution< long >::min() const
{
  return LONG_MIN;
}


template <>
inline
long AimsConvolution< long >::max() const
{
  return LONG_MAX;
}


template <>
inline
ulong AimsConvolution< ulong >::min() const
{
  return 0;
}


template <>
inline
ulong AimsConvolution< ulong >::max() const
{
  return ULONG_MAX;
}


#endif
