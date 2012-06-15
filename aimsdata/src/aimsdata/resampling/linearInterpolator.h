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

/*
 * 3D linear image interpolation
 */

#ifndef AIMS_LINEAR_INTERPOLATOR
#define AIMS_LINEAR_INTERPOLATOR

#include <cassert>

#include <aims/data/data.h>
#include <cartobase/smart/rcptr.h>
#include <vector>

namespace aims {


  //----------------//
 //  Interpolator  //
//----------------//

//-----------------------------------------------------------------------------
class Interpolator : public carto::RCObject
{
public:

  typedef float Coordinate_t;
  typedef Point3df Point_t;
  typedef double Scalar_t;

  virtual ~Interpolator();

  /** Return true if point ( x, y, z ) can be used for interpolation. If
      interpolation is tried on an invalid point, it may crash the program.
  */
  virtual bool isValid( Coordinate_t x, Coordinate_t y, 
                        Coordinate_t z ) const = 0;
  /** Return true if point can be used for interpolation. If
      interpolation is tried on an invalid point, it may crash the program.
  */
  inline bool isValid( const Interpolator::Point_t &point ) const
  {
    return isValid( point[ 0 ], point[ 1 ], point[ 2 ] );
  }
  /** Obsolete, use value() method */
  virtual Scalar_t operator()( Coordinate_t x, Coordinate_t y, 
                                Coordinate_t z ) const = 0;
  /** Obsolete, use value() method */
  inline Scalar_t operator()( const Interpolator::Point_t &point ) const
  {
    return operator ()( point[ 0 ], point[ 1 ], point[ 2 ] );
  }
  /** Obsolete, use values() method */
  virtual void operator()( Coordinate_t x, Coordinate_t y, Coordinate_t z,
                             std::vector<Scalar_t> & ) const = 0;
  /** Obsolete, use values() method */
  inline void operator()( const Interpolator::Point_t &point,
                            std::vector<Scalar_t> &v ) const
  {
    operator ()( point[ 0 ], point[ 1 ], point[ 2 ], v );
  }
  /** Interpolate to get a value from point ( x, y ,z ). */
  inline Scalar_t value( Coordinate_t x, Coordinate_t y, 
                         Coordinate_t z ) const
  {
    return operator ()( x, y, z );
  }
  /** Interpolate to get a value from point */
  inline Scalar_t value( const Interpolator::Point_t &point ) const
  {
    return operator ()( point[ 0 ], point[ 1 ], point[ 2 ] );
  }
  /** Interpolate to get a series of values from point ( x, y ,z ) */
  inline void values( Coordinate_t x, Coordinate_t y, Coordinate_t z,
                      std::vector<Scalar_t> &v ) const
  {
    operator ()( x, y, z, v );
  }
  /** Interpolate to get a series of values from point ( x, y ,z ) */
  inline void values( const Interpolator::Point_t &point,
                            std::vector<Scalar_t> &v ) const
  {
    operator ()( point[ 0 ], point[ 1 ], point[ 2 ], v );
  }
  /** Return the header of the image.
  */
  virtual const carto::PropertySet &header() const = 0;
};


  //--------------------------------//
 //  linear interpolator factories //
//--------------------------------//
//-----------------------------------------------------------------------------
carto::rc_ptr< Interpolator > getLinearInterpolator( const std::string & );
template <typename T>
carto::rc_ptr< Interpolator > getLinearInterpolator( const AimsData<T> & );


  //--------------------------//
 //   LinearInterpolator<T>  //
//--------------------------//

class LinearInterpolatorFactory;

//-----------------------------------------------------------------------------
template <typename T>
class LinearInterpolator : public Interpolator
{
  friend class LinearInterpolatorFactory;

  template <typename U> friend carto::rc_ptr< Interpolator > 
  getLinearInterpolator( const AimsData<U> & );

  LinearInterpolator( const AimsData<T> & image );
  virtual ~LinearInterpolator();

  bool isValid( Coordinate_t x, Coordinate_t y, Coordinate_t z ) const;
  Scalar_t operator()( Coordinate_t x, Coordinate_t y, Coordinate_t z ) const;
  void operator()( Coordinate_t x, Coordinate_t y, Coordinate_t z,
                   std::vector<Scalar_t> & ) const;

  virtual const carto::PropertySet &header() const;

  const AimsData<T> _image;

private:

  inline static
  void _interpolationCoefficients( Coordinate_t xx,
                                   int &x,
                                   int &X,
                                   Coordinate_t &ax,
                                   int dx, float sx );

  inline static
  Scalar_t linint(Scalar_t a, Scalar_t b, Scalar_t x) { return a + x*(b-a); }


  int _dimX, _dimY, _dimZ;
  float _invsizeX, _invsizeY, _invsizeZ;
};





  //--------------------------------//
 //  linear interpolator factories //
//--------------------------------//

//-----------------------------------------------------------------------------
template <typename T>
carto::rc_ptr< Interpolator > getLinearInterpolator( const AimsData<T> &image )
{
  return carto::rc_ptr< Interpolator >( new LinearInterpolator<T>( image ) );
}


//-----------------------------------------------------------------------------
extern template carto::rc_ptr< Interpolator > 
 getLinearInterpolator( const AimsData<uint8_t> & );
extern template carto::rc_ptr< Interpolator >
 getLinearInterpolator( const AimsData<int8_t> & );
extern template carto::rc_ptr< Interpolator > 
 getLinearInterpolator( const AimsData<unsigned short> & );
extern template carto::rc_ptr< Interpolator >
 getLinearInterpolator( const AimsData<short> & );
extern template carto::rc_ptr< Interpolator > 
 getLinearInterpolator( const AimsData<uint32_t> & );
extern template carto::rc_ptr< Interpolator >
 getLinearInterpolator( const AimsData<int32_t> & );
extern template carto::rc_ptr< Interpolator >
 getLinearInterpolator( const AimsData<float> & );
extern template carto::rc_ptr< Interpolator >
 getLinearInterpolator( const AimsData<double> & );


  //--------------------------//
 //   LinearInterpolator<T>  //
//--------------------------//

//-----------------------------------------------------------------------------
template <typename T>
LinearInterpolator<T>::LinearInterpolator( const AimsData<T> & image ) :
  _image( image )
{
  _dimX = _image.dimX();
  _dimY = _image.dimY();
  _dimZ = _image.dimZ();

  assert(_image.sizeX() > 0);
  _invsizeX = float(1.0 / _image.sizeX());
  assert(_image.sizeY() > 0);
  _invsizeY = float(1.0 / _image.sizeY());
  assert(_image.sizeZ() > 0);
  _invsizeZ = float(1.0 / _image.sizeZ());
}


//-----------------------------------------------------------------------------
template <typename T>
LinearInterpolator<T>::~LinearInterpolator()
{
}

//-----------------------------------------------------------------------------
template <typename T>
bool LinearInterpolator<T>::isValid( Interpolator::Coordinate_t x, 
                                     Interpolator::Coordinate_t y, 
                                     Interpolator::Coordinate_t z ) const
{
  const Interpolator::Coordinate_t hx = - _image.sizeX() / 2;
  const Interpolator::Coordinate_t hy = - _image.sizeY() / 2;
  const Interpolator::Coordinate_t hz = - _image.sizeZ() / 2;
  return x >= hx && y >= hy && z >= hz &&
    x < _image.dimX() * _image.sizeX() + hx &&
    y < _image.dimY() * _image.sizeY() + hy &&
    z < _image.dimZ() * _image.sizeZ() + hz;
}


//-----------------------------------------------------------------------------
template <typename T>
void LinearInterpolator<T>::
  _interpolationCoefficients( Interpolator::Coordinate_t xx,
                              int & x,
                              int & X,
                              Interpolator::Coordinate_t & ax,
                              int dx, float isx)
{
  const Interpolator::Coordinate_t px = xx * isx;
  
  x = int(px);
  X = x+1;
  if ( px < 0 )
  {
    x = X = 0;
    ax = 1;
  }
  else if (X >= dx)
  {
    X = x = dx - 1;
    ax = 0;
  }
  else
  {
    ax = px - x;
    assert(ax >= 0);
    assert(ax <= 1);
  }
}


//-----------------------------------------------------------------------------
template <typename T>
Interpolator::Scalar_t
LinearInterpolator<T>::operator ()( Interpolator::Coordinate_t xx,
                                    Interpolator::Coordinate_t yy,
                                    Interpolator::Coordinate_t zz ) const
{
  int x, X, y, Y, z, Z;
  Interpolator::Coordinate_t ax, ay, az;
  _interpolationCoefficients(xx, x, X, ax, _dimX, _invsizeX);
  _interpolationCoefficients(yy, y, Y, ay, _dimY, _invsizeY);
  _interpolationCoefficients(zz, z, Z, az, _dimZ, _invsizeZ);

  return
      linint(
             linint(
                    linint(_image( x, y, z ), _image( X, y, z ), ax),
                    linint(_image( x, Y, z ), _image( X, Y, z ), ax), ay),
             linint(
                    linint(_image( x, y, Z ), _image( X, y, Z ), ax),
                    linint(_image( x, Y, Z ), _image( X, Y, Z ), ax), ay), az);
}


//-----------------------------------------------------------------------------
template <typename T>
void LinearInterpolator<T>::
  operator ()( Interpolator::Coordinate_t xx,
               Interpolator::Coordinate_t yy,
               Interpolator::Coordinate_t zz,
               std::vector< Interpolator::Scalar_t > &values ) const
{
  values.resize( _image.dimT() );
  int x, X, y, Y, z, Z;
  Interpolator::Coordinate_t ax, ay, az;
  _interpolationCoefficients(xx, x, X, ax, _dimX, _invsizeX);
  _interpolationCoefficients(yy, y, Y, ay, _dimY, _invsizeY);
  _interpolationCoefficients(zz, z, Z, az, _dimZ, _invsizeZ);

  for( int t = 0; t < _image.dimT(); ++t ) {
    values[ t ] = 
      linint(
             linint(
                    linint(_image( x, y, z, t ), _image( X, y, z, t ), ax),
                    linint(_image( x, Y, z, t ), _image( X, Y, z, t ), ax), ay),
             linint(
                    linint(_image( x, y, Z, t ), _image( X, y, Z, t ), ax),
                    linint(_image( x, Y, Z, t ), _image( X, Y, Z, t ), ax), ay), az);
  }
}


//-----------------------------------------------------------------------------
template <typename T>
const carto::PropertySet & LinearInterpolator<T>::header() const
{
  return _image.volume()->header();
}


//-----------------------------------------------------------------------------
extern template class LinearInterpolator<uint8_t>;
extern template class LinearInterpolator<int8_t>;
extern template class LinearInterpolator<uint16_t>;
extern template class LinearInterpolator<int16_t>;
extern template class LinearInterpolator<uint32_t>;
extern template class LinearInterpolator<int32_t>;
extern template class LinearInterpolator<float>;
extern template class LinearInterpolator<double>;



} // namespace aims

#endif // ifdef AIMS_LINEAR_INTERPOLATOR
