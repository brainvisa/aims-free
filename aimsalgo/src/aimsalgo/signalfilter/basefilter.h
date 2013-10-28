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


#ifndef AIMSALGO_SIGNALFILTER_BASESMOOTHING_H
#define AIMSALGO_SIGNALFILTER_BASESMOOTHING_H

#include <cartobase/config/verbose.h>
#include <aims/signalfilter/nonlin_filt-func.h>
#include <aims/signalfilter/multichannelfilter.h>

#define AIMSALGO_SIGNALFILTER_DECLARE_FILTER( S, T, F ) \
template <class T> \
class S : public BaseSmoothing<T, F > { \
  public: \
    S( int sx = 3, int sy = 3, int sz = 1 ); \
    virtual ~S() { } \
}; \
\
template< class T > inline \
S< T >::S(int sx, int sy, int sz ) \
  : BaseSmoothing<T, F >(sx, sy, sz) \
{ \
} \

#define AIMSALGO_SIGNALFILTER_SPECIALIZE_FILTER( S, T, F ) \
template <> \
class S<T> : public BaseSmoothing<T, F > { \
  public: \
    S( int sx = 3, int sy = 3, int sz = 1 ); \
    virtual ~S() { } \
}; \
\
inline \
S<T>::S(int sx, int sy, int sz ) \
  : BaseSmoothing<T, F >(sx, sy, sz) \
{ \
} \
\

template< class T, class F >
class BaseSmoothing
{
public:

  BaseSmoothing( int sx = 3, int sy = 3, int sz = 1 );
  virtual ~BaseSmoothing() { }

  AimsData< T >  doit( const AimsData<T>& );

protected:

  int                        _win_size_x;
  int                        _win_size_y;
  int                        _win_size_z;
  const F                    _func;
};


template< class T, class F >
inline 
BaseSmoothing< T, F >::BaseSmoothing(int sx, int sy, int sz )
  : _win_size_x(sx), 
    _win_size_y(sy),
    _win_size_z(sz)
{
}


template< class T, class F > 
inline
AimsData< T > BaseSmoothing< T, F >::doit( const AimsData< T >& ref )
{

  int x, y, z, t, nx, ny, nz, n;
  short dx, dy, dz, bz, by, bx;

  dx = _win_size_x / 2 ;
  dy = _win_size_y / 2 ;
  dz = _win_size_z / 2 ;
 
  AimsData<T> out(ref.dimX(), ref.dimY(), ref.dimZ(), ref.dimT());
  out = ref.clone();
  if( ref.header() ) {
    out.setHeader( ref.header()->cloneHeader( true ) );
  }
 
  // Update mask dims since it is always an odd number
  _win_size_x = dx * 2 + 1;
  _win_size_y = dy * 2 + 1;
  _win_size_z = dz * 2 + 1;

  AimsData<T> tab( _win_size_x * _win_size_y * _win_size_z );
  bz = (ref.dimZ() - dz);
  by = (ref.dimY() - dy);
  bx = (ref.dimX() - dx);
  
  for ( t = 0; t < ref.dimT(); t++ )
    for ( z = dz; z < bz; z++ ) {
      if (carto::verbose) {
        if( z > 0 ) std::cout << "\b\b\b\b\b" << std::setw(3) << ( 100 * (z+1) / ref.dimZ() ) << " %" << std::flush;
        else std::cout << "Progress: " << std::setw(3) << ( 100 * (z+1) / ref.dimZ() ) << " %" << std::flush;
      }
      for ( y = dy; y < by; y++ )
        for ( x = dx; x < bx; x++ )
        {
          n = 0;
          for ( nz = -dz; nz <= dz; nz++ )
            for ( ny = -dy; ny <= dy; ny++ )
              for ( nx = -dx; nx <= dx; nx++ )
              {
                tab(n) = ref(nx+x, ny+y, nz+z, t);
                ++n ;
              }
          out( x, y, z, t ) = _func.doit( tab );
        }
    }

  return( out );

}

AIMSALGO_SIGNALFILTER_SPECIALIZE_MULTICHANNELFILTER(BaseSmoothing, AimsRGB)

AIMSALGO_SIGNALFILTER_SPECIALIZE_MULTICHANNELFILTER(BaseSmoothing, AimsRGBA)

#endif

